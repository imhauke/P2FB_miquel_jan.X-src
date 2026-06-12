#include "tad_manager_lcd.h"

static char flagPrint, flagPrint2, flagClear;
static char j;
static char *print_lcd;
static char *print_lcd2;

// Cua de notificacions (guardem nomes kind/tipus/num, no el text)
static char notif_kind[NOTIF_MAX];
static char notif_tipus[NOTIF_MAX];
static char notif_num[NOTIF_MAX];
static char notif_cap;     // index d'entrada (on encuem)
static char notif_cua;     // index de sortida (on llegim)
static char notif_n;       // nombre de notificacions a la cua

// Buffer del text de la notificacio actual
static char notif_text[17];
static unsigned char t_notif;

// Noms (acabats en '\0')
static const char NOM_ANIMAL[NOTIF_MAX][8] = {
    TXT_VACA, TXT_CAVALL, TXT_PORC, TXT_GALLINA
};
static const char NOM_PRODUCTE[NOTIF_MAX][9] = {
    TXT_LLET, TXT_PINZELL, TXT_PERNIL, TXT_OUS
};

void initManagerLcd(void) {
    LcInit(2, 16);
    LcCursorOff();
    flagPrint  = 0;
    flagPrint2 = 0;
    flagClear  = 0;
    j = 0;
    notif_cap = 0;
    notif_cua = 0;
    notif_n   = 0;
    TI_NewTimer(&t_notif);
}

void LCD_print(char *frase) {
    print_lcd = frase;
    flagPrint = 1;
}

void LCD_print2(char *frase) {
    print_lcd2 = frase;
    flagPrint2 = 1;
}

void LCD_clear(void) {
    flagClear = 1;
}

char LCD_flag1(void) { return flagPrint; }
char LCD_flag2(void) { return flagPrint2; }

void LCD_notifica(char kind, char tipus, char num) {
    if (notif_n >= NOTIF_MAX) return; // cua plena, descartem
    notif_kind[notif_cap]  = kind;
    notif_tipus[notif_cap] = tipus;
    notif_num[notif_cap]   = num;
    notif_cap++;
    if (notif_cap >= NOTIF_MAX) notif_cap = 0;
    notif_n++;
}

// --- Construccio del text: estats interns, un caracter per pas (sense bucles) ---
// Subestat de formatat
static char fmt_state;   // 0=prefix 1=nom 2=":"/" " 3=numero 4=fi
static char fmt_p;       // index a notif_text on escrivim
static char fmt_m;       // index dins la cadena origen (prefix/nom)
static const char *fmt_src;

// Descompon el numero (0..255) sense divisions: comparacions i restes
static char numCentenes(char n) {
    if (n >= 200) return 2;
    if (n >= 100) return 1;
    return 0;
}
static char numDesenes(char n) {
    if (n >= 100) n -= 100;
    if (n >= 100) n -= 100;
    if (n >= 90) return 9;
    if (n >= 80) return 8;
    if (n >= 70) return 7;
    if (n >= 60) return 6;
    if (n >= 50) return 5;
    if (n >= 40) return 4;
    if (n >= 30) return 3;
    if (n >= 20) return 2;
    if (n >= 10) return 1;
    return 0;
}
static char numUnitats(char n) {
    if (n >= 100) n -= 100;
    if (n >= 100) n -= 100;
    if (n >= 90) n -= 90; else if (n >= 80) n -= 80;
    else if (n >= 70) n -= 70; else if (n >= 60) n -= 60;
    else if (n >= 50) n -= 50; else if (n >= 40) n -= 40;
    else if (n >= 30) n -= 30; else if (n >= 20) n -= 20;
    else if (n >= 10) n -= 10;
    return n;
}

// Prepara el formatat de la notificacio del cap de cua
static void iniciaFormat(void) {
    if (notif_kind[notif_cua] == NOTIF_ANIMAL) {
        fmt_src = TXT_NOU_ANIMAL;
    } else {
        fmt_src = TXT_NOU_PRODUCTE;
    }
    fmt_state = 0;
    fmt_p = 0;
    fmt_m = 0;
}

// Avanca el formatat un caracter. Retorna 1 mentre treballa, 0 quan ha acabat.
static char passaFormat(void) {
    char num;
    char cent, des;

    switch (fmt_state) {
        case 0: // copia prefix
            if (fmt_src[fmt_m] != '\0') {
                notif_text[fmt_p] = fmt_src[fmt_m];
                fmt_p++; fmt_m++;
            } else {
                if (notif_kind[notif_cua] == NOTIF_ANIMAL)
                    fmt_src = NOM_ANIMAL[notif_tipus[notif_cua]];
                else
                    fmt_src = NOM_PRODUCTE[notif_tipus[notif_cua]];
                fmt_m = 0;
                fmt_state = 1;
            }
            break;

        case 1: // copia nom especie/producte
            if (fmt_src[fmt_m] != '\0' && fmt_p < 13) {
                notif_text[fmt_p] = fmt_src[fmt_m];
                fmt_p++; fmt_m++;
            } else {
                notif_text[fmt_p] = ':';  fmt_p++;
                notif_text[fmt_p] = ' ';  fmt_p++;
                fmt_state = 2;
            }
            break;

        case 2: // escriu el numero (3 digits com a molt)
            num  = notif_num[notif_cua];
            cent = numCentenes(num);
            des  = numDesenes(num);
            if (cent > 0) { notif_text[fmt_p] = '0' + cent; fmt_p++; }
            if (cent > 0 || des > 0) { notif_text[fmt_p] = '0' + des; fmt_p++; }
            notif_text[fmt_p] = '0' + numUnitats(num); fmt_p++;
            notif_text[fmt_p] = '\0';
            fmt_state = 3;
            break;

        default:
            return 0;
    }
    return 1;
}

void motorManagerLcd(void) {
    static char state = 0;

    switch (state) {

        case 0:
            // Prioritat: notificacions pendents
            if (notif_n > 0) {
                iniciaFormat();
                state = 6;
            } else if (flagClear == 1) {
                LcClear();
                flagClear = 0;
            } else if (flagPrint == 1) {
                LcGotoXY(0, 0);
                j = 0;
                state = 1;
            } else if (flagPrint2 == 1) {
                LcGotoXY(0, 1);
                j = 0;
                state = 2;
            }
            break;

        case 1:
            if (print_lcd[j] != '\0') {
                LcPutChar(print_lcd[j]);
                j++;
            } else {
                flagPrint = 0;
                state = 0;
            }
            break;

        case 2:
            if (print_lcd2[j] != '\0') {
                LcPutChar(print_lcd2[j]);
                j++;
            } else {
                flagPrint2 = 0;
                state = 0;
            }
            break;

        // Construeix el text de la notificacio, un caracter per pas
        case 6:
            if (passaFormat()) {
                // segueix formatant
            } else {
                LcClear();
                state = 3;
            }
            break;

        // Posiciona cursor i comenca a pintar la notificacio
        case 3:
            LcGotoXY(0, 0);
            j = 0;
            state = 4;
            break;

        // Pinta la notificacio caracter a caracter
        case 4:
            if (notif_text[j] != '\0') {
                LcPutChar(notif_text[j]);
                j++;
            } else {
                TI_ResetTics(t_notif);
                state = 5;
            }
            break;

        // Mante la notificacio visible 3 segons
        case 5:
            if (TI_GetTics(t_notif) >= TICKS_NOTIF) {
                notif_cua++;
                if (notif_cua >= NOTIF_MAX) notif_cua = 0;
                notif_n--;
                state = 0;
            }
            break;
    }
}

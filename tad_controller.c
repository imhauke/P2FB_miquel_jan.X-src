#include "tad_controller.h"

#define MAX_NOM    16
#define MAX_BUFFER  2

// --- Notificacions LCD (cua propia del controller) ---
#define NOTIF_ANIMAL    0
#define NOTIF_PRODUCTE  1
#define NOTIF_MAX       4         // cua de minim 3 (enunciat)
#define TICKS_NOTIF     15000     // 3s visibles (0.2ms/tic -> 15000 tics)

#define TXT_NOU_ANIMAL    "Nou Animal "
#define TXT_NOU_PRODUCTE  "Nou Producte "

static const char NOM_ANIMAL[NUM_ESPECIES][8] = {
    "Vaca", "Cavall", "Porc", "Gallina"
};
static const char NOM_PRODUCTE[NUM_ESPECIES][9] = {
    "Llet", "Pinzell", "Pernil", "Ous"
};

static char notif_kind[NOTIF_MAX];
static char notif_tipus[NOTIF_MAX];
static char notif_num[NOTIF_MAX];
static char notif_cap, notif_cua, notif_n;
static char notif_text[17];       // text de la notificacio en curs
static unsigned char t_notif;     // timer dels 3s
static char fmt_state, fmt_p, fmt_m;  // sub-estats del formatat (sense bucles)
static const char *fmt_src;

// --- Variables privades ---
static char linia1[16] = {"                "};   // nom granja  (16 chars + espais)
static char linia2[16] = {" 00/00/2026     "};   // " DD/MM/2026" o el que vulguis
static char flag_amunt;
static char buffer[MAX_BUFFER];
static char c;
static char i, j, k;
static char flag_init, flag_comptatge;

static char temps_animal[NUM_ESPECIES];
static char temps_decimal;
static char count_temps_animal[NUM_ESPECIES];
static char count_temps_producte[NUM_ESPECIES];
static const char temps_producte[NUM_ESPECIES] = {
    TEMPS_PROD_VACA, TEMPS_PROD_CAVALL, TEMPS_PROD_PORC, TEMPS_PROD_GALLINA
};

static Animal animals_arr[MAX_ANIMALS_TOTAL];
static char desperts_flag[MAX_ANIMALS_TOTAL]; // 1=despert 0=son, paral.lel a animals_arr
static char animals[NUM_ESPECIES];
static char animals_desperts[NUM_ESPECIES];
static char q_animals;
static char productes[NUM_ESPECIES];
static char flag_rebellio;

static char resp[24];     // buffer per construir respostes a Java
static char cnt_esp[NUM_ESPECIES]; // comptador per especie en enviar animals
static char sleep_esp;    // especie de l'animal a dormir (SLEEP)
static char sleep_num;    // numero de l'animal a dormir
static unsigned char t_sleep;      // timer dels 5s d'espera LDR

static char ee_addr;      // adreca base EEPROM de l'animal que dorm/llegeix
static char ee_pas;       // pas dins l'escriptura dels 6 bytes
static char flag_restaura; // 1 mentre s'esta llegint l'EEPROM a l'arrencada
static char ee_tipus;     // tipus llegit d'un slot durant la restauracio
static char ee_total;     // nombre d'animals guardats a l'EEPROM (a restaurar)

static char *str;

static unsigned char t;
static unsigned char t2;
static char flag_puls;

// --- Rellotge del sistema (abans tad_hora) ---
static char dia, mes, hora, minut, segon;
static char flag_hora;          // 1 = ja s'ha rebut una data/hora valida
static unsigned char t_rellotge; // timer per avancar els segons
static const char DIES_MES[13] = {
    0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};


// --- Init ---
void initController(void) {
    flag_init = 0;
    flag_comptatge = 0;
    flag_rebellio = 0;
    flag_restaura = 1;     // a l'arrencada, restaura animals des de l'EEPROM
    flag_hora = 0;         // encara no s'ha rebut la data/hora
    dia = 1; mes = 1; hora = 0; minut = 0; segon = 0;
    i = 0; j = 0; k = 0;
    q_animals = 0;
    notif_cap = 0; notif_cua = 0; notif_n = 0;

    animals[0] = 0; animals[1] = 0;
    animals[2] = 0; animals[3] = 0;
    animals_desperts[0] = 0;    animals_desperts[1] = 0;
    animals_desperts[2] = 0;    animals_desperts[3] = 0;
    count_temps_animal[0] = 0;  count_temps_animal[1] = 0;
    count_temps_animal[2] = 0;  count_temps_animal[3] = 0;
    count_temps_producte[0] = 0;    count_temps_producte[1] = 0;
    count_temps_producte[2] = 0;    count_temps_producte[3] = 0;
    productes[0] = 0;   productes[1] = 0;
    productes[2] = 0;   productes[3] = 0;

    // Array compactat: els animals ocupen sempre [0..q_animals-1].
    // No cal inicialitzar els 24 slots: q_animals = 0 ja indica granja buida.

    TI_NewTimer(&t);
    TI_NewTimer(&t_sleep);
    TI_NewTimer(&t2);
    TI_NewTimer(&t_rellotge);
    TI_NewTimer(&t_notif);
}

// --- Encua una notificacio (kind=NOTIF_ANIMAL/PRODUCTE, tipus 0..3, num) ---
static void notifica(char kind, char tipus, char num) {
    if (notif_n >= NOTIF_MAX) return;   // cua plena, descartem
    notif_kind[notif_cap]  = kind;
    notif_tipus[notif_cap] = tipus;
    notif_num[notif_cap]   = num;
    notif_cap++;
    if (notif_cap >= NOTIF_MAX) notif_cap = 0;
    notif_n++;
}

// --- Rellotge del sistema (abans tad_hora) ---
// El validador crida aquesta funcio quan rep una data/hora valida.
void HORA_set(char d, char mo, char h, char mi, char s) {
    dia = d; mes = mo; hora = h; minut = mi; segon = s;
    flag_hora = 1;
    TI_ResetTics(t_rellotge);
}

// Avanca el rellotge un segon (cridat des del motor cada SEGON tics)
static void avancaRellotge(void) {
    segon++;
    if (segon >= 60) {
        segon = 0;
        minut++;
        if (minut >= 60) {
            minut = 0;
            hora++;
            if (hora >= 24) {
                hora = 0;
                dia++;
                if (dia > DIES_MES[mes]) {
                    dia = 1;
                    mes++;
                    if (mes > 12) {
                        mes = 1;
                    }
                }
            }
        }
    }
}

// --- Descompon un numero 0..99 en desenes/unitats (sense divisions) ---
static char desenes(char n) {
    char d = 0;
    if (n >= 90) return 9;
    if (n >= 80) return 8;
    if (n >= 70) return 7;
    if (n >= 60) return 6;
    if (n >= 50) return 5;
    if (n >= 40) return 4;
    if (n >= 30) return 3;
    if (n >= 20) return 2;
    if (n >= 10) return 1;
    return d;
}

static char unitats(char n) {
    if (n >= 90) return n - 90;
    if (n >= 80) return n - 80;
    if (n >= 70) return n - 70;
    if (n >= 60) return n - 60;
    if (n >= 50) return n - 50;
    if (n >= 40) return n - 40;
    if (n >= 30) return n - 30;
    if (n >= 20) return n - 20;
    if (n >= 10) return n - 10;
    return n;
}

// --- Escriu un numero (0..255) a resp[p..]; retorna la nova posicio ---
static char posaNum(char p, char n) {
    char cent = 0;
    if (n >= 200) { cent = 2; n -= 200; }
    else if (n >= 100) { cent = 1; n -= 100; }
    if (cent > 0) {
        resp[p++] = '0' + cent;
        resp[p++] = '0' + desenes(n);
    } else if (n >= 10) {
        resp[p++] = '0' + desenes(n);
    }
    resp[p++] = '0' + unitats(n);
    return p;
}

// --- Centenes d'un numero 0..255 (sense divisions) ---
static char centenes(char n) {
    if (n >= 200) return 2;
    if (n >= 100) return 1;
    return 0;
}
// Treu les centenes per poder calcular desenes/unitats del que queda
static char treuCentenes(char n) {
    if (n >= 200) return n - 200;
    if (n >= 100) return n - 100;
    return n;
}

// --- Prepara el formatat de la notificacio del cap de cua ---
static void iniciaFormatNotif(void) {
    if (notif_kind[notif_cua] == NOTIF_ANIMAL) {
        fmt_src = TXT_NOU_ANIMAL;
    } else {
        fmt_src = TXT_NOU_PRODUCTE;
    }
    fmt_state = 0;
    fmt_p = 0;
    fmt_m = 0;
}

// --- Avanca el formatat un caracter. Retorna 1 mentre treballa, 0 al acabar ---
// Construeix "Nou Animal Vaca: 2" / "Nou Producte Ous: 11" a notif_text.
static char passaFormatNotif(void) {
    char num, cent, des;

    switch (fmt_state) {
        case 0: // copia el prefix
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

        case 1: // copia el nom de l'especie/producte
            if (fmt_src[fmt_m] != '\0' && fmt_p < 13) {
                notif_text[fmt_p] = fmt_src[fmt_m];
                fmt_p++; fmt_m++;
            } else {
                notif_text[fmt_p] = ':';  fmt_p++;
                notif_text[fmt_p] = ' ';  fmt_p++;
                fmt_state = 2;
            }
            break;

        case 2: // escriu el numero (fins 3 digits)
            num  = notif_num[notif_cua];
            cent = centenes(num);
            num  = treuCentenes(num);
            des  = desenes(num);
            if (cent > 0) { notif_text[fmt_p] = '0' + cent; fmt_p++; }
            if (cent > 0 || des > 0) { notif_text[fmt_p] = '0' + des; fmt_p++; }
            notif_text[fmt_p] = '0' + unitats(num); fmt_p++;
            notif_text[fmt_p] = '\0';
            fmt_state = 3;
            break;

        default:
            return 0;
    }
    return 1;
}

// --- Auxiliars parser INITIALIZE ---
void guardaCaracterTemps(void) {
    if (c != '\r') {
        buffer[i] = c;
        i++;
    }
}

void ferItoaTemps(void) {
    char sumand = buffer[0] - '0';
    if (i == 1) {
        temps_decimal = sumand;
    } else {
        temps_decimal = sumand + sumand + sumand + sumand + sumand +
                        sumand + sumand + sumand + sumand + sumand +
                        (buffer[1] - '0');
    }
}

void guardaTempsAnimal(void) {
    temps_animal[j] = temps_decimal;
}

// --- Afegeix un animal d'especie esp al final de l'array (compactat) ---
// Array compactat: el nou animal va directe a animals_arr[q_animals],
// sense buscar slots buits. Retorna 1 si s'ha afegit, 0 si la granja es plena.
static char afegirNouAnimal(char esp) {
    if (q_animals >= MAX_ANIMALS_TOTAL) {
        return 0;
    }

    animals_arr[q_animals].tipus = esp + 1;
    desperts_flag[q_animals] = 1;           // despert (array paral.lel)
    animals_arr[q_animals].count_son = 0;   // comenca a comptar els seus 2 min

    animals[esp]++;
    animals_desperts[esp]++;
    q_animals++;

    // Notifica al LCD: "Nou Animal <Tipus>: <num>"
    notifica(NOTIF_ANIMAL, esp, animals[esp]);

    return 1;
}

// --- Compta el segon de l'animal k; si arriba a 2 min, son critic (un pas) ---
// Array compactat: recorre nomes [0..q_animals-1].
static char posarAnimalsASon(void) {
    if (k >= q_animals) {
        return 0;
    }
    if (desperts_flag[k] == 1) {
        animals_arr[k].count_son++;
        if (animals_arr[k].count_son >= TEMPS_SON) {
            animals_arr[k].count_son = 0;
            desperts_flag[k] = 0;
            animals_desperts[animals_arr[k].tipus - 1]--;
        }
    }
    k++;
    return 1;
}

// --- Incrementa productes de l'especie i ---
static void incrementarProductes(char esp) {
    productes[esp] += animals_desperts[esp];
    // Notifica al LCD: "Nou Producte <Tipus>: <total>"
    notifica(NOTIF_PRODUCTE, esp, productes[esp]);
}

// --- Construeix "P llet$pernil$ous$pinzells" a resp[] (resposta GET_PRODUCTS) ---
// productes[]: [0]=llet [1]=pinzells [2]=pernil [3]=ous
static void construeixProductes(void) {
    char p = 0;
    resp[p++] = RSP_DATA_PRODUCTS;
    p = posaNum(p, productes[0]);   // llet
    resp[p++] = '$';
    p = posaNum(p, productes[2]);   // pernil
    resp[p++] = '$';
    p = posaNum(p, productes[3]);   // ous
    resp[p++] = '$';
    p = posaNum(p, productes[1]);   // pinzells
    resp[p++] = '\r';
    resp[p++] = '\n';
    resp[p]   = '\0';
}

// --- Copia el nom de l'especie (tipus 1..4) a resp[p..]; retorna nova posicio ---
// Sense bucles: assignacions explicites per especie.
static char posaEspecie(char p, char tipus) {
    if (tipus == 1) {            // VACA
        resp[p++]='V'; resp[p++]='A'; resp[p++]='C'; resp[p++]='A';
    } else if (tipus == 2) {     // CAVALL
        resp[p++]='C'; resp[p++]='A'; resp[p++]='V';
        resp[p++]='A'; resp[p++]='L'; resp[p++]='L';
    } else if (tipus == 3) {     // PORC
        resp[p++]='P'; resp[p++]='O'; resp[p++]='R'; resp[p++]='C';
    } else {                     // GALLINA
        resp[p++]='G'; resp[p++]='A'; resp[p++]='L'; resp[p++]='L';
        resp[p++]='I'; resp[p++]='N'; resp[p++]='A';
    }
    return p;
}

// --- Construeix "A <ESPECIE>$<num>$<estat>\r\n" a resp[] per l'animal del slot k ---
static void construeixAnimal(char num_especie) {
    char p = 0;
    resp[p++] = RSP_DATA_ANIMAL;
    p = posaEspecie(p, animals_arr[k].tipus);
    resp[p++] = '$';
    p = posaNum(p, num_especie);
    resp[p++] = '$';
    if (desperts_flag[k] == 1) {
        resp[p++]='A'; resp[p++]='W'; resp[p++]='A'; resp[p++]='K'; resp[p++]='E';
    } else {
        resp[p++]='S'; resp[p++]='L'; resp[p++]='E'; resp[p++]='E'; resp[p++]='P';
    }
    resp[p++] = '\r';
    resp[p++] = '\n';
    resp[p]   = '\0';
}

// --- RESET: buida la granja i torna a l'espera d'inicialitzacio ---
static void ferReset(void) {
    initController();      // reinicialitza tots els comptadors i arrays
    flag_restaura = 0;     // un reset no ha de restaurar res
    // Marca l'EEPROM com a buida (q_animals = 0 a l'adreca 0).
    // A l'arrencada, si q_animals val 0 no es restaura cap animal.
    EE_writeByte(EE_ADDR_QANIM, 0);
}

// --- CONSUME: descompta productes segons l'opcio escollida ---
// Opcions (segons enunciat):
//  0 = ou fregit          -> 1 ou
//  1 = truita amb pernil  -> 1 ou + 1 pernil
//  2 = cacaolat           -> 2 llets
//  3 = pintura            -> 2 pinzells
// productes[]: [0]=llet [1]=pinzells [2]=pernil [3]=ous
static void ferConsume(char opcio) {
    if (opcio == '0') {
        if (productes[3] >= 1) productes[3] -= 1;
    } else if (opcio == '1') {
        if (productes[3] >= 1 && productes[2] >= 1) {
            productes[3] -= 1;
            productes[2] -= 1;
        }
    } else if (opcio == '2') {
        if (productes[0] >= 2) productes[0] -= 2;
    } else if (opcio == '3') {
        if (productes[1] >= 2) productes[1] -= 2;
    }
}

// --- Motor principal ---
void motorController(void) {
    static char state = 0;

    switch (state) {

        // Espera inicialitzacio completa (Java + hora serial)
        case 0:
            // A l'arrencada, restaura primer els animals guardats a l'EEPROM
//            if (flag_restaura == 1) {
//                k = 0;
//                ee_addr = EE_ADDR_ANIMALS;
//                state = 80;
//                break;
//            }
            // L'hora ja la controla el controller: flag_hora l'activa HORA_set
            if (flag_init == 1 && flag_hora == 1 && flag_comptatge == 0) {
                TI_ResetTics(t);
                i = 0;
                LcGotoXY(0,0);
                flag_amunt = 1;
                state = 7;
                break;
            }
            // Notificacions pendents: les mostrem al LCD (>=3s cadascuna)
            if (flag_comptatge == 1 && notif_n > 0) {
                iniciaFormatNotif();
                state = 90;
                break;
            }
            if (SiCharAvail()) {
                c = SiGetChar();
                if (c == CMD_INITIALIZE) {
                    i = 0; j = 0;
                    state = 2;
                }
                if (c == CMD_RESET) {
                    ferReset();
                }
                if (c == CMD_START_REBELLION) {
                    flag_rebellio = 1;
                    HB_stop();          // apaga el heartbeat durant la rebel.lio
                }
                if (c == CMD_STOP_REBELLION) {
                    flag_rebellio = 0;
                    HB_start();         // restableix el heartbeat
                }
                if (c == CMD_GET_PRODUCTS) {
                    state = 30;     // envia DATA_PRODUCTS
                }
                if (c == CMD_GET_ANIMALS) {
                    k = 0;
                    state = 40;     // envia llista d'animals + FINISH
                }
                if (c == CMD_CONSUME) {
                    state = 50;     // llegeix l'opcio
                }
                if (c == CMD_SLEEP) {
                    state = 60;     // llegeix TIPUS$NUM
                }
                break;
            }
            // Avanca el rellotge cada segon (independent del comptatge d'animals)
            if (flag_hora == 1 && TI_GetTics(t_rellotge) >= SEGON) {
                TI_ResetTics(t_rellotge);
                avancaRellotge();
            }
            if (flag_comptatge == 1 && TI_GetTics(t) >= SEGON) {
                TI_ResetTics(t); //TODO no sabem si es necessari, depen
                i = 0;
                state = 9;
                break;
            }
            // Moviments del joystick: mateix patro que el polsador.
            // Preparem str amb la frase corresponent i enviem (state = 1).
            if (CJ_hiHaUp()) {
                str = RSP_MOVE_UP_STR;
                i = 0;
                state = 1;
                break;
            }
            if (CJ_hiHaDown()) {
                str = RSP_MOVE_DOWN_STR;
                i = 0;
                state = 1;
                break;
            }
            if (CJ_hiHaLeft()) {
                str = RSP_MOVE_LEFT_STR;
                i = 0;
                state = 1;
                break;
            }
            if (CJ_hiHaRight()) {
                str = RSP_MOVE_RIGHT_STR;
                i = 0;
                state = 1;
                break;
            }
            // Polsador SELECT (antirebot per RB2)
            if(PORTBbits.RB2 == 0 && !flag_puls) {
                TI_ResetTics(t2);
                state = 5;
            }
            if (PORTBbits.RB2 == 1 && flag_puls) {
                flag_puls = 0;
            }
            break;

        // Identifica comandament entrant
        case 1:
            if(SiAvail()) {
                if(str[i] != '\0') {
                    SiSendChar(str[i]);
                    i++;
                } else{
                    state = 0;
                }
            }
           
            break;

        // Llegeix nom granja fins '$'
        case 2:
            if (SiCharAvail()) { 
                c = SiGetChar(); 
                if (c != '$') {
                    linia1[i] = c;
                    i++; 
                } else {
                    i = 0;
                    state = 4;
                }
            }
            break;
        // Llegeix 4 temps de generacio
        case 4:
            
            if (SiCharAvail()) { 
                c = SiGetChar(); 
                if(c != '\r') {
                    if (c != '$' && c != '\n') {
                        buffer[i] = c; 
                        i++; 
                    } else{
                        ferItoaTemps();
                        guardaTempsAnimal();
                        j++; 
                        i = 0;
                        if (j >=NUM_ESPECIES) { 
                            flag_init = 1; 
                            state = 0; 
                        }
                    } 
                }
                
            }
            

            break;

        case 5:
            if(TI_GetTics(t2) > 100) {
                if(PORTBbits.RB2 == 0) {
                    flag_puls = 1;
                    str = RSP_SELECT_STR;
                    i = 0;
                    state = 1;
                }else {
                    state = 0;
                }
            }
            break;

        case 6:
            
            break;

        // Construeix linia 2 amb la data (font unica: TAD Hora) i dispara prints
        case 7:
            // Linia 2: " DD/MM/2026" (11 chars). Llegim del TAD Hora.
            
            // La data la te el propi controller; omplim linia2 amb dia/mes ASCII
            linia2[1]  = '0' + desenes(dia);
            linia2[2]  = '0' + unitats(dia);
            linia2[4]  = '0' + desenes(mes);
            linia2[5]  = '0' + unitats(mes);

            if(flag_amunt) {
                if(i < 16) {
                    LcPutChar(linia1[i]);
                    i++;
                }else if(i == 16){
                    LcGotoXY(0,1);
                    flag_amunt = 0;
                    i = 0;
                }
            }else{
                if(i < 16) {
                    LcPutChar(linia2[i]);
                    i++;
                }else{
                    //ja ha acabat de dibuixar la pantalla
                    TI_ResetTics(t);
                    flag_comptatge =1;
                    state = 0;
                }
            }
            break;
            
        // --- Cada segon: generacio animals, especie i ---
        case 9:
            if (i < NUM_ESPECIES) {
                count_temps_animal[i]++;
                state = 10;
            } else {
                i = 0;
                state = 11;
            }
            break;

        case 10:
            if (count_temps_animal[i] >= temps_animal[i]) {
                count_temps_animal[i] = 0;
                afegirNouAnimal(i);     // array compactat: afegeix directe (o ignora si ple)
            }
            i++;
            state = 9;
            break;

        // --- Cada segon: produccio, especie i ---
        case 11:
            if (i < NUM_ESPECIES) {
                count_temps_producte[i]++;
                state = 12;
            } else {
                i = 0;
                state = 13;
            }
            break;

        case 12:
            if (count_temps_producte[i] >= temps_producte[i]) {
                count_temps_producte[i] = 0;
                if (animals_desperts[i] > 0) {
                    incrementarProductes(i);
                }
            }
            i++;
            state = 11;
            break;

        // --- Cada segon: compta el son critic individual de cada animal ---
        case 13:
            k = 0;
            state = 14;
            break;

        // Compta el segon de cada animal; dorm els que arribin a 2 min
        case 14:
            if (posarAnimalsASon()) {
                // continua fins k >= MAX_ANIMALS_TOTAL
            } else {
                state = 0;
            }
            break;

        // === GET_PRODUCTS: envia "P llet$pernil$ous$pinzells\r\n" ===
        case 30:
            if (statusFiMissatge()) {
                construeixProductes();
                enviaMissatge(resp);
                state = 0;
            }
            break;

        // === GET_ANIMALS: prepara comptadors per especie ===
        case 40:
            cnt_esp[0] = 0; cnt_esp[1] = 0;
            cnt_esp[2] = 0; cnt_esp[3] = 0;
            k = 0;
            state = 41;
            break;

        // Recorre els animals [0..q_animals-1]; per cadascun envia una linia
        case 41:
            if (k >= q_animals) {
                state = 42;     // tots enviats, toca el FINISH
            } else if (statusFiMissatge()) {
                char esp = animals_arr[k].tipus - 1;
                construeixAnimal(cnt_esp[esp]);
                cnt_esp[esp]++;
                enviaMissatge(resp);
                k++;
            }
            break;

        // Envia FINISH
        case 42:
            if (statusFiMissatge()) {
                enviaMissatge(RSP_FINISH_STR);
                state = 0;
            }
            break;

        // === CONSUME: llegeix l'opcio i descompta ===
        case 50:
            if (SiCharAvail()) {
                c = SiGetChar();
                if (c != '\r' && c != '\n') {
                    ferConsume(c);
                }
                state = 51;
            }
            break;

        // Consumeix la resta fins '\n'
        case 51:
            if (SiCharAvail()) {
                c = SiGetChar();
                if (c == '\n') state = 0;
            }
            break;

        // === SLEEP: llegeix la primera lletra (especie) ===
        case 60:
            if (SiCharAvail()) {
                c = SiGetChar();
                if (c == 'V')      sleep_esp = 0;  // VACA
                else if (c == 'C') sleep_esp = 1;  // CAVALL
                else if (c == 'P') sleep_esp = 2;  // PORC
                else               sleep_esp = 3;  // GALLINA
                state = 61;
            }
            break;

        // Descarta la resta del nom fins '$'
        case 61:
            if (SiCharAvail()) {
                c = SiGetChar();
                if (c == '$') state = 62;
            }
            break;

        // Llegeix el numero d'animal (1 digit, fins '\r' o '\n')
        case 62:
            if (SiCharAvail()) {
                c = SiGetChar();
                if (c >= '0' && c <= '9') {
                    sleep_num = c - '0';
                } else if (c == '\n') {
                    TI_ResetTics(t_sleep);
                    state = 63;     // espera fins a 5s que es tapi l'LDR
                }
            }
            break;

        // Espera que l'LDR detecti foscor en menys de 5 segons
        case 63:
            if (CJ_hiHaFosc()) {
                state = 64;         // ha dormit: marca'l i respon OK
            } else if (TI_GetTics(t_sleep) >= TICKS_5S) {
                state = 65;         // timeout: respon NOK
            }
            break;

        // Cerca el slot de l'animal (especie sleep_esp, index sleep_num)
        case 64:
            k = 0;
            j = 0;                  // comptador d'animals de l'especie trobats
            ee_addr = EE_ADDR_ANIMALS; // acumula l'adreca base mentre recorre
            state = 66;
            break;

        case 66:
            if (k >= q_animals) {
                state = 65;     // no trobat (no hauria de passar) -> NOK
            } else if (animals_arr[k].tipus == sleep_esp + 1) {
                if (j == sleep_num) {
                    state = 67; // trobat!
                } else {
                    j++;
                    k++;
                    ee_addr += EE_BYTES_ANIMAL;
                }
            } else {
                k++;
                ee_addr += EE_BYTES_ANIMAL;
            }
            break;

        // Marca l'animal dormit i comenca a guardar el timestamp a EEPROM
        case 67:
            if (desperts_flag[k] == 1) {
                desperts_flag[k] = 0;
                animals_desperts[sleep_esp]--;
            }
            animals_arr[k].count_son = 0;   // reinicia el seu cicle de son
            ee_pas = 0;
            state = 68;
            break;

        // Guarda 7 bytes a EEPROM: marca q_animals i els 6 de l'animal, un per pas
        case 68:
            if (EE_busy()) break;   // espera que acabi l'escriptura anterior
            if (ee_pas == 0) {
                EE_writeByte(EE_ADDR_QANIM, q_animals);  // marca que hi ha dades
            } else if (ee_pas == 1) {
                EE_writeByte(ee_addr + EE_OFF_TIPUS, animals_arr[k].tipus);
            } else if (ee_pas == 2) {
                EE_writeByte(ee_addr + EE_OFF_DIA,  dia);
            } else if (ee_pas == 3) {
                EE_writeByte(ee_addr + EE_OFF_MES,  mes);
            } else if (ee_pas == 4) {
                EE_writeByte(ee_addr + EE_OFF_HORA, hora);
            } else if (ee_pas == 5) {
                EE_writeByte(ee_addr + EE_OFF_MIN,  minut);
            } else if (ee_pas == 6) {
                EE_writeByte(ee_addr + EE_OFF_SEG,  segon);
            } else {
                state = 69;     // tots escrits
                break;
            }
            ee_pas++;
            break;

        // Respon Y quan ha acabat l'EEPROM
        case 69:
            if (!EE_busy() && statusFiMissatge()) {
                enviaMissatge(RSP_SLEEP_OK_STR);
                state = 0;
            }
            break;

        // No ha pogut dormir: respon N
        case 65:
            if (statusFiMissatge()) {
                enviaMissatge(RSP_SLEEP_NOK_STR);
                state = 0;
            }
            break;

        // === RESTAURACIO EEPROM: comprova si hi ha dades guardades ===
        case 80:
            // q_animals invalid (0 o > maxim) -> EEPROM buida, no restaura res
            ee_total = EE_readByte(EE_ADDR_QANIM);
            if (ee_total == 0 || ee_total > MAX_ANIMALS_TOTAL) {
                flag_restaura = 0;
                state = 0;
            } else {
                state = 81;
            }
            break;

        // Restaura els ee_total animals guardats (array compactat, [0..ee_total-1])
        case 81:
            if (k >= ee_total) {
                flag_restaura = 0;
                state = 0;
                break;
            }
            ee_tipus = EE_readByte(ee_addr + EE_OFF_TIPUS);
            if (ee_tipus >= 1 && ee_tipus <= NUM_ESPECIES) {
                animals_arr[k].tipus   = ee_tipus;
                desperts_flag[k] = 1;           // arrenca despert
                animals_arr[k].count_son = 0;
                animals[ee_tipus - 1]++;
                animals_desperts[ee_tipus - 1]++;
                q_animals++;
            }
            k++;
            ee_addr += EE_BYTES_ANIMAL;
            break;

        // === NOTIFICACIONS LCD ===
        // Construeix el text de la notificacio, un caracter per pas
        case 90:
            if (passaFormatNotif()) {
                // segueix formatant
            } else {
                LcClear();
                LcGotoXY(0, 0);
                i = 0;
                state = 91;
            }
            break;

        // Pinta la notificacio caracter a caracter
        case 91:
            if (notif_text[i] != '\0') {
                LcPutChar(notif_text[i]);
                i++;
            } else {
                TI_ResetTics(t_notif);
                state = 92;
            }
            break;

        // Mante la notificacio visible 3 segons
        case 92:
            if (TI_GetTics(t_notif) >= TICKS_NOTIF) {
                notif_cua++;
                if (notif_cua >= NOTIF_MAX) notif_cua = 0;
                notif_n--;
                // Si no queden notificacions, repinta el LCD idle (nom + data)
                if (notif_n == 0) {
                    LcClear();
                    LcGotoXY(0, 0);
                    i = 0;
                    flag_amunt = 1;
                    state = 93;
                } else {
                    state = 0;      // mostra la seguent notificacio
                }
            }
            break;

        // Repinta el LCD idle (nom granja + data) despres de les notificacions
        case 93:
            linia2[1]  = '0' + desenes(dia);
            linia2[2]  = '0' + unitats(dia);
            linia2[4]  = '0' + desenes(mes);
            linia2[5]  = '0' + unitats(mes);
            if (flag_amunt) {
                if (i < 16) {
                    LcPutChar(linia1[i]);
                    i++;
                } else {
                    LcGotoXY(0, 1);
                    flag_amunt = 0;
                    i = 0;
                }
            } else {
                if (i < 16) {
                    LcPutChar(linia2[i]);
                    i++;
                } else {
                    state = 0;
                }
            }
            break;
    }
}
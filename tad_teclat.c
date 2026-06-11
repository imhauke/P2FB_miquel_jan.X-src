#include "tad_teclat.h"

#define TECLAT_QUEUE_SIZE 4

static unsigned char t_rebots;
static unsigned char t_sms;

static const char columnes0[4] = {'1', '4', '7', '*'};
static const char columnes1[4] = {'2', '5', '8', '0'};
static const char columnes2[4] = {'3', '6', '9', '#'};

static const char grup2[4] = {'A', 'B', 'C', '2'};
static const char grup3[4] = {'D', 'E', 'F', '3'};
static const char grup4[4] = {'G', 'H', 'I', '4'};
static const char grup5[4] = {'J', 'K', 'L', '5'};
static const char grup6[4] = {'M', 'N', 'O', '6'};
static const char grup7[5] = {'P', 'Q', 'R', 'S', '7'};
static const char grup8[4] = {'T', 'U', 'V', '8'};
static const char grup9[5] = {'W', 'X', 'Y', 'Z', '9'};
static const char grup0[2] = {' ', '0'};

static char fila_activa;
static char index_fila;
static char teclaDetectada;

static char smsActiu;
static char teclaAnterior;
static char indexLletra;

static char cuaTecles[TECLAT_QUEUE_SIZE];
static unsigned char cuaInici;
static unsigned char cuaFinal;
static unsigned char cuaCount;

static char hiHaContacteTeclat(void) {
    if (C0 == 0 || C1 == 0 || C2 == 0) {
        return 1;
    }
    return 0;
}

static char teclaFilaActual(void) {
    if (C0 == 0) {
        return columnes0[index_fila];
    }
    if (C1 == 0) {
        return columnes1[index_fila];
    }
    if (C2 == 0) {
        return columnes2[index_fila];
    }
    return 0;
}

static void rotarFila(void) {
    if (index_fila == 0) {
        fila_activa = 0xFD;
        index_fila = 1;
    } else if (index_fila == 1) {
        fila_activa = 0xFB;
        index_fila = 2;
    } else if (index_fila == 2) {
        fila_activa = 0xF7;
        index_fila = 3;
    } else {
        fila_activa = 0xFE;
        index_fila = 0;
    }

    LATC = (LATC & 0xF0) | (fila_activa & 0x0F);
}

static char esTeclaDirecta(char tecla) {
    if (tecla == '1' || tecla == '*' || tecla == '#') {
        return 1;
    }
    return 0;
}

static void pushTeclaFinal(char tecla) {
    if (cuaCount >= TECLAT_QUEUE_SIZE) {
        return;
    }

    cuaTecles[cuaFinal] = tecla;
    cuaFinal = (cuaFinal + 1) % TECLAT_QUEUE_SIZE;
    cuaCount++;
}

static char caracterSMSActual(void) {
    switch (teclaAnterior) {
        case '2': return grup2[indexLletra];
        case '3': return grup3[indexLletra];
        case '4': return grup4[indexLletra];
        case '5': return grup5[indexLletra];
        case '6': return grup6[indexLletra];
        case '7': return grup7[indexLletra];
        case '8': return grup8[indexLletra];
        case '9': return grup9[indexLletra];
        case '0': return grup0[indexLletra];
        default: return teclaAnterior;
    }
}

static void seguentCaracter(void) {
    indexLletra++;

    if (teclaAnterior == '7' || teclaAnterior == '9') {
        if (indexLletra >= 5) {
            indexLletra = 0;
        }
    } else if (teclaAnterior == '0') {
        if (indexLletra >= 2) {
            indexLletra = 0;
        }
    } else if (indexLletra >= 4) {
        indexLletra = 0;
    }
}

static void confirmarSMSActual(void) {
    if (!smsActiu) {
        return;
    }

    pushTeclaFinal(caracterSMSActual());
    smsActiu = 0;
}

static void iniciaSMS(char tecla) {
    smsActiu = 1;
    teclaAnterior = tecla;
    indexLletra = 0;
    TI_ResetTics(t_sms);
}

static void processaTecla(char tecla) {
    if (esTeclaDirecta(tecla)) {
        if (smsActiu) {
            confirmarSMSActual();
        }
        pushTeclaFinal(tecla);
        return;
    }

    if (!smsActiu) {
        iniciaSMS(tecla);
        return;
    }

    if (tecla == teclaAnterior) {
        seguentCaracter();
        TI_ResetTics(t_sms);
        return;
    }

    confirmarSMSActual();
    iniciaSMS(tecla);
}

char TECLAT_getTecla(void) {
    char tecla;

    if (cuaCount == 0) {
        return 0;
    }

    tecla = cuaTecles[cuaInici];
    cuaInici = (cuaInici + 1) % TECLAT_QUEUE_SIZE;
    cuaCount--;
    return tecla;
}

char TECLAT_HHT(void) {
    if (cuaCount > 0) {
        return 1;
    }
    return 0;
}

void initTeclat(void) {
    TI_NewTimer(&t_rebots);
    TI_NewTimer(&t_sms);

    TRISBbits.TRISB1 = 1;
    TRISBbits.TRISB2 = 1;
    TRISBbits.TRISB3 = 1;
    TRISCbits.TRISC0 = 0;
    TRISCbits.TRISC1 = 0;
    TRISCbits.TRISC2 = 0;
    TRISCbits.TRISC3 = 0;

    F1 = 1;
    F2 = 1;
    F3 = 1;
    F0 = 0;

    INTCON2bits.RBPU = 0;

    fila_activa = 0xFE;
    index_fila = 0;
    teclaDetectada = 0;

    smsActiu = 0;
    teclaAnterior = 0;
    indexLletra = 0;

    cuaInici = 0;
    cuaFinal = 0;
    cuaCount = 0;

    LATC = (LATC & 0xF0) | (fila_activa & 0x0F);
}

void motorTeclat(void) {
    static char estat = 0;

    if (smsActiu && (TI_GetTics(t_sms) >= TEMPS_TECLA)) {
        confirmarSMSActual();
    }

    switch (estat) {
        case 0:
            rotarFila();
            if (hiHaContacteTeclat()) {
                TI_ResetTics(t_rebots);
                estat = 1;
            }
            break;

        case 1:
            if (TI_GetTics(t_rebots) >= FILTREREBOTS) {
                if (hiHaContacteTeclat()) {
                    teclaDetectada = teclaFilaActual();
                    estat = 2;
                } else {
                    estat = 0;
                }
            }
            break;

        case 2:
            if (!hiHaContacteTeclat()) {
                if (teclaDetectada != 0) {
                    processaTecla(teclaDetectada);
                }
                estat = 0;
            }
            break;
    }
}

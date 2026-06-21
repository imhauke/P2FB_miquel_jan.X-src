#include "tad_validador.h"
#include "tad_controller.h"   // per cridar HORA_set(...) del controller

// ====================================================================
//  CANAL SERIE SOFTWARE (Serial_Time) integrat dins del validador
//  Maquina de bits propia, amb timer propi (tb), independent del parser.
// ====================================================================
static char start_tx;             // 1 = hi ha una transmissio en curs
static char byteTx;               // byte que s'esta transmetent
static char byteRx;               // byte rebut
static char flag_byte;            // 1 = hi ha un byte rebut pendent
static char nbit;                 // comptador de bits (TX i RX)
static unsigned char tb;          // timer dels bit-times

// ====================================================================
//  PARSER de la trama DD/MM HH:MM:SS
// ====================================================================
static char buffer[VAL_BUFFER];
static char idx;
static char c;
static char dia, mes, hora, minut, segon;
static const char *msg;
static char msg_i;

void initValidador(void) {
    CONFIG_TX = 0;
    CONFIG_RX = 1;
    PIN_TX = 1;
    start_tx = 0;
    flag_byte = 0;
    idx = 0;
    TI_NewTimer(&tb);
}

// --- Primitives del canal serie (abans SIO_AUX_*) ---
static char sioTxLliure(void) {
    return !start_tx;
}
static void sioEnvia(char x) {
    start_tx = 1;
    byteTx = x;
}
static char sioHiHaByte(void) {
    char aux = flag_byte;
    flag_byte = 0;
    return aux;
}

// Conversio 2 chars ASCII a numero (sense multiplicacions: x10 = sumes)
static char dosDigits(char a, char b) {
    char d = a - '0';
    char u = b - '0';
    return d + d + d + d + d + d + d + d + d + d + u;
}

static char rang(char v, char mn, char mx) {
    return (v >= mn && v <= mx);
}

// --- Sub-motor del canal serie software (bits): TX i RX ---
static void motorSerie(void) {
    static char state = 0;

    switch (state) {

        case 0:
            if (start_tx) {
                PIN_TX = 0;             // start bit
                TI_ResetTics(tb);
                state = 1;
            } else if (PIN_RX == 0) {   // start bit entrant
                byteRx = 0x00;
                nbit = 0;
                TI_ResetTics(tb);
                state = 5;
            }
            break;

        // --- Transmissio ---
        case 1:
            if (TI_GetTics(tb) >= TEMPS_BIT) {
                nbit = 0;
                PIN_TX = byteTx & 0x01;
                byteTx = byteTx >> 1;
                TI_ResetTics(tb);
                state = 2;
            }
            break;

        case 2:
            if (nbit < 7) {
                if (TI_GetTics(tb) >= TEMPS_BIT) {
                    TI_ResetTics(tb);
                    PIN_TX = byteTx & 0x01;
                    byteTx = byteTx >> 1;
                    nbit++;
                }
            } else {
                state = 3;
            }
            break;

        case 3:
            if (TI_GetTics(tb) >= TEMPS_BIT) {  // espera l'ultim bit
                TI_ResetTics(tb);
                PIN_TX = 1;                     // stop bit
                state = 4;
            }
            break;

        case 4:
            if (TI_GetTics(tb) >= TEMPS_BIT) {  // espera l'stop bit
                start_tx = 0;
                state = 0;
            }
            break;

        // --- Recepcio ---
        case 5:
            if (TI_GetTics(tb) >= 12) {         // mig bit del start
                TI_ResetTics(tb);
                state = 6;
            }
            break;

        case 6:
            if (nbit < 7) {
                if (TI_GetTics(tb) >= TEMPS_BIT) {
                    TI_ResetTics(tb);
                    byteRx = (byteRx & 0x7F) | ((PIN_RX << 7) & 0x80);
                    byteRx = byteRx >> 1;
                    nbit++;
                }
            } else {
                state = 7;
            }
            break;

        case 7:
            if (TI_GetTics(tb) >= TEMPS_BIT) {
                TI_ResetTics(tb);
                byteRx = (byteRx & 0x7F) | ((PIN_RX << 7) & 0x80);
                TI_ResetTics(tb);
                state = 8;
            }
            break;

        case 8:
            if (TI_GetTics(tb) >= TEMPS_BIT) {
                flag_byte = 1;
                state = 0;
            }
            break;
    }
}

// --- Motor del validador: serie (bits) + parser de la trama ---
void motorValidador(void) {
    static char state = 0;

    // El canal serie sempre actiu (TX i RX a nivell de bit)
    motorSerie();

    switch (state) {

        // Rep caracters, en fa eco i els acumula fins a l'Enter
        case 0:
            if (sioHiHaByte()) {
                c = byteRx;
                sioEnvia(c);            // eco
                if (c == '\r') {
                    state = 1;          // fi de trama: salt de linia i valida
                } else {
                    buffer[idx] = c;
                    idx++;
                }
            }
            break;

        // Salt de linia i validacio
        case 1:
            if (sioTxLliure()) {
                sioEnvia('\n');
                dia   = dosDigits(buffer[0],  buffer[1]);
                mes   = dosDigits(buffer[3],  buffer[4]);
                hora  = dosDigits(buffer[6],  buffer[7]);
                minut = dosDigits(buffer[9],  buffer[10]);
                segon = dosDigits(buffer[12], buffer[13]);

                if (rang(dia,1,31) && rang(mes,1,12) &&
                    rang(hora,0,23) && rang(minut,0,59) && rang(segon,0,59)) {
                    HORA_set(dia, mes, hora, minut, segon); // el controller pren el control de l'hora
                    msg = MSG_OK;
                } else {
                    msg = MSG_ERR;
                }
                idx = 0;
                msg_i = 0;
                state = 2;
            }
            break;

        // Envia el feedback pel terminal, caracter a caracter
        case 2:
            if (msg[msg_i] == '\0') {
                idx = 0;
                state = 0;
            } else if (sioTxLliure()) {
                sioEnvia(msg[msg_i]);
                msg_i++;
            }
            break;
    }
}

#include "tad_sio_manual.h"

static char start_tx;
static char byteTx, i, j, byteRx, flag_byte_available;
static unsigned char t;

void initSioAux(void) {

    CONFIG_TX = 0;
    CONFIG_RX = 1;

    PIN_TX = 1;

    TI_NewTimer(&t);
}

void motorSioAux(void) {
    static char state = 0;

    switch(state) {

        case 0:
            if(start_tx) {
                PIN_TX = 0;

                TI_ResetTics(t);

                state = 1;

            } else if(PIN_RX == 0) { //rebem start bit

                byteRx = 0x00;
                j = 0;

                TI_ResetTics(t);

                state = 5;
            }

            break;

        case 1:
            if(TI_GetTics(t) >= TEMPS_BIT) {
                i = 0;

                PIN_TX = byteTx & 0x01;
                byteTx = byteTx >> 1;

                TI_ResetTics(t);

                state = 2;
            }

            break;

        case 2:
            if(i < 7) {

                if(TI_GetTics(t) >= TEMPS_BIT) {

                    TI_ResetTics(t);

                    PIN_TX = byteTx & 0x01;
                    byteTx = byteTx >> 1;

                    i++;
                }

            } else {

                state = 3;
            }

            break;
        
        case 3:
            if(TI_GetTics(t) >= TEMPS_BIT) { //m'espero l'ultim bit
                //generar stop bit

                TI_ResetTics(t);

                PIN_TX = 1;

                state = 4;
            }

            break;

        case 4:
            if(TI_GetTics(t) >= TEMPS_BIT) {

                //ja acabo d'esperar l'stop bit

                start_tx = 0;

                state = 0;
            }

            break;

        case 5:
            if(TI_GetTics(t) >= 12) {

                TI_ResetTics(t);

                state = 6;
            }

            break;
        
        case 6:
            if(j < 7) {

                if(TI_GetTics(t) >= TEMPS_BIT) { //espero start bit

                    TI_ResetTics(t);

                    byteRx = (byteRx & 0x7F) | ((PIN_RX << 7) & 0x80);
                    byteRx = byteRx >> 1;

                    j++;
                }

            } else {

                state = 7;
            }

            break;

        case 7:
            if(TI_GetTics(t) >= TEMPS_BIT) { //espero start bit

                TI_ResetTics(t);

                byteRx = (byteRx & 0x7F) | ((PIN_RX << 7) & 0x80);

                TI_ResetTics(t);

                state = 8;
            }

            break;
        case 8:
            if(TI_GetTics(t) >= TEMPS_BIT) {

                flag_byte_available = 1;

                state = 0;
            }

            break;
    }
}

char SIO_AUX_txAvailable(void) {
    return !start_tx;
}

void SIO_AUX_sendByte(char c) {
    start_tx = 1;
    byteTx = c;
}

char SIO_AUX_byteAvailable(void) {
    char aux = flag_byte_available;

    flag_byte_available = 0;

    return aux;
}

char SIO_AUX_byteGet(void) {
    return byteRx;
}
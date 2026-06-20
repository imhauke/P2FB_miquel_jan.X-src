#include "tad_controlador.h"
#include "tad_sio_manual.h"

#define FRASE "Jan mariconazo\0"

static char i;
static char lletra;

//void initControlador(void) {
//}

void motorControlador(void) {
    static char state = 0;

    switch (state) {
        case 0:
            i = 0;
            state = 1;
            break;

        case 1:
            if (SIO_AUX_txAvailable()) {
                if (FRASE[i] != '\0') {
                    SIO_AUX_sendByte(FRASE[i]);
                    i++;
                } else {
                    state = 2;
                }
            }
            break;

        case 2:
            if (SIO_AUX_byteAvailable()) {
                lletra = SIO_AUX_byteGet();
                state = 3;
            }
            break;

        case 3:
            if (SIO_AUX_txAvailable()) {
                SIO_AUX_sendByte(lletra);
                state = 2;
            }
            break;
    }
}
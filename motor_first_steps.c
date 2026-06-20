#include "tad_controlador.h"
#include "motor_first_steps.h"

#define FRASE "Hola que tal\0"
#define FRASE_RIGHT "CMD_RIGHT\r\n\0"
#define FRASE_LEFT  "CMD_LEFT\r\n\0"
#define FRASE_UP    "CMD_UP\r\n\0"
#define FRASE_DOWN  "CMD_DOWN\r\n\0"
#define FRASE_FOSC  "CMD_SLEEP\r\n\0"
static char i;
static char lletra;
static char pointer_frase;
static char *frase;


//void initController(void) {
//
//}

void motortestSIO(void) {
    static char state = 0;

    switch(state) {

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

        case 4:
            state = 5;
            i = 0;
            if (CJ_hiHaRight()) {
                frase = FRASE_RIGHT;
            } else if (CJ_hiHaLeft()) {
                frase = FRASE_LEFT;
            } else if (CJ_hiHaUp()) {
                frase = FRASE_UP;
            } else if (CJ_hiHaDown()) {
                frase = FRASE_DOWN;
            }else if (CJ_hiHaFosc()) {
                frase = FRASE_FOSC;
            } else {
                state = 4;
            }
            break;
        case 5:
            if (SIO_AUX_txAvailable()) {
                if (frase[i] != '\0') {
                    SIO_AUX_sendByte(frase[i]);
                    i++;
                } else {
                    state = 4;
                }
            }
            break;
    }
}
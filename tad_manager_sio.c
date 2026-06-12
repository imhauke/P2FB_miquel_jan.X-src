#include "tad_manager_sio.h"

static char *msg;
static char start, i;

void MNG_SIO_init(void) {
}

void enviaMissatge(char* m) {
    msg = m;
    start = 1;
}

char statusFiMissatge(void) {
    return !start;
}

void enviaCaracter(char c) {
    SiSendChar(c);
}

void MNG_SIO_motor(void) {
    static char state = 0;

    switch(state) {
        case 0:
            if(start) {
                i = 0;
                state = 1;
            }
            break;

        case 1:
            if(SiAvail()) {
                if(msg[i] != '\0') {
                    SiSendChar(msg[i]);
                    i++;
                }else{
                    start = 0;
                    state = 0;
                }
            }
            break;
    }
}

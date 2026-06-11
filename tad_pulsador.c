#include "tad_pulsador.h"

static unsigned char t;
static char flag_premut;

void pulsadorInit(void) {
    TRISBbits.TRISB0 = 1;
    INTCON2bits.NOT_RBPU = 0;
    TI_NewTimer(&t);
}

void motorPulsador(void) {
    static char state = 0;
    switch(state) {
        case 0:
            if(!PORTBbits.RB0) {
                TI_ResetTics(t);
                state = 1;
            }
            break;
            
        case 1:
            if(TI_GetTics(t) >= 40) {
                state = 2;
            }
            break;
            
        case 2:
            if(!PORTBbits.RB0) {
                flag_premut = 1;
            }
            state = 3;
            break;
            
        case 3:
            if(PORTBbits.RB0) {
                state = 0;
            }
            break;
    }
}

char HiHaPulsador(void) {
    char aux = flag_premut;
    flag_premut = 0;
    return aux;
}
#include "tad_controller_adc.h"

static char flag_right, flag_left;
static char flag_up, flag_down;
static char pos;
static unsigned char ldr_value;
static char flag_fosc;

void initControllerAdc(void) {
    ADCON0bits.ADON = 1;
    ADCON1 = 0x0C;
    ADCON2 = 0x3E; //LEFT JUSTIFIED
}

void motorControllerAdc(void) {
    static char state = 0;

    switch(state) {

        case 0:
            JOYSTICK_start();
            state = 1;
            break;

        case 1:
            if(JOYSTICK_finish()) {
                state = 2;
            }
            break;

        case 2:
            if(JOYSTICK_hiHaX()) {
                pos = JOYSTICK_getPositionX();
                if(pos == 1) {
                    flag_right = 1;
                } else if(pos == 2) {
                    flag_left = 1;
                }
            }

            if(JOYSTICK_hiHaY()){
                pos = JOYSTICK_getPositionY();
                if(pos == 1) {
                    flag_up = 1;
                } else if(pos == 2) {
                    flag_down = 1;
                }
            }
            LDR_start();
            state = 3;
            break;
        
        case 3:
            if (LDR_finish()) {
                ldr_value = LDR_getValue();
                if (ldr_value < 50) {
                    flag_fosc = 1;
                } 
                state = 0;
            }
            break;
    }
}

char CJ_hiHaRight(void) {
    char aux = flag_right;
    flag_right = 0;
    return aux;
}

char CJ_hiHaLeft(void) {
    char aux = flag_left;
    flag_left = 0;
    return aux;
}

char CJ_hiHaUp(void) {
    char aux = flag_up;
    flag_up = 0;
    return aux;
}

char CJ_hiHaDown(void) {
    char aux = flag_down;
    flag_down = 0;
    return aux;
}

char CJ_hiHaFosc(void) {
    char aux = flag_fosc;
    flag_fosc = 0;
    return aux;
}

char CJ_hiHaSelect(void) {
    return PULSADOR_hiHaSelect();   // delega al TAD pulsador
}

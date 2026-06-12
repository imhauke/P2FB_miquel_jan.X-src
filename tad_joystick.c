#include "tad_joystick.h"

static char start;
static unsigned char result_x, result_y;
static char posX, posY;
static char flag_x, flag_y;
static char flag_mig_x, flag_mig_y;

void initJoystick(void) {

}

void selectChannel0(void) {
    ADCON0bits.CHS3 = 0;
    ADCON0bits.CHS2 = 0;
    ADCON0bits.CHS1 = 0;
    ADCON0bits.CHS0 = 0;
}

void selectChannel1(void) {
    ADCON0bits.CHS3 = 0;
    ADCON0bits.CHS2 = 0;
    ADCON0bits.CHS1 = 0;
    ADCON0bits.CHS0 = 1;
}

void motorJoystick(void) {
    static char state = 0;

    switch (state) {
        case 0:
            if (start) {
                selectChannel0();
                ADCON0bits.GO_DONE = 1;
                state = 1;
            }
            break;
        case 1:
            if (ADCON0bits.GO_DONE == 0) {
                result_x = ADRESH;
                state = 2;
            }
            break;
        case 2:
            if (result_x < 100 || result_x > 160) {
                if (flag_mig_x == 1) {
                    if (result_x > 160) {
                        posX = 1; //right
                        flag_x = 1;
                    } else {
                        posX = 2; //left
                        flag_x = 1;
                    }

                    flag_mig_x = 0;
                }
            } else {
                if (result_x > 110 && result_x < 150) {
                    //mig

                    posX = 0;
                    flag_mig_x = 1; //has passat pel mig
                }
            }

            start = 0;
            state = 3;
            break;

        case 3:
            selectChannel1();
            ADCON0bits.GO_DONE = 1;
            state = 4;
            break;

        case 4:
            if (ADCON0bits.GO_DONE == 0) {
                result_y = ADRESH;
                state = 5;
            }
            break;
        case 5:
            if (result_y < 100 || result_y > 160) {
                if (flag_mig_y == 1) {
                    if (result_y > 160) {
                        posY = 1; //up
                        flag_y = 1;
                    } else {
                        posY = 2; //down
                        flag_y = 1;
                    }

                    flag_mig_y = 0;
                }
            } else {
                if (result_y > 110 && result_y < 150) {
                    //mig

                    posY = 0;
                    flag_mig_y = 1; //has passat pel mig
                }
            }

            start = 0;
            state = 0;
            break;
    }
}

void JOYSTICK_start(void) {
    start = 1;
}

char JOYSTICK_finish(void) {
    return !start;
}

char JOYSTICK_hiHaX(void) {
    char aux = flag_x;
    flag_x = 0;
    return aux;
}

char JOYSTICK_hiHaY(void) {
    char aux = flag_y;
    flag_y = 0;
    return aux;
}

char JOYSTICK_getPositionX(void) { //1 right, 2 left, 0 mig
    return posX;
}

char JOYSTICK_getPositionY(void) { //1 up, 2 down, 0 mig
    return posY;
}
#include <xc.h>
#include "tad_leds.h"

static char duty;       // 0-100 %
static int period;
static unsigned char t;        // 0: no alarm, 1: alarm
static char start;

void LEDS_init(void){
    TI_NewTimer(&t);
    TRIS_INT = 0;
    TRIS_ALARM = 0;
    TRIS_OK = 0;

    LED_INT = 0;
    LED_ALARM = 0;
    LED_OK = 0;
}

void LEDS_startIntensity(void){
    start = 1;
    duty = 0;
    period = 0;
}

void LEDS_stopIntensity(void){
    start = 0;
}
 
void LEDS_setOK(char value){
    LED_OK = value;
}

void LEDS_setAlarm(char value){
    LED_ALARM = value;
}

void LEDS_setDuty(char value){
    duty = value;
}

void LEDS_motorIntensity(void){
    static char state = 0;
    switch (state){
        case 0:
            if (start){
                LED_INT = 1;
                TI_ResetTics(t);
                state = 1;
            } else{
                LED_INT = 0;
            }
            break;

        case 1:
            if (TI_GetTics(t) >= duty){
                LED_INT = 0;
                state = 2;
            }
            break;
        
        case 2:
            if (TI_GetTics(t) >= 40){ //40 ticks a 1/0
                period++;
                state = 3;
            }
            break;

        case 3:
        //Augmentar periodo prorque esto es para timer 1ms y nosotros vamos a 0,5ms
            if (period == 150){ //40 veces 150 = 6.000 = 3segons
                period = 0;
            }
            state = 0;
            /*else{
                state = 0;
            }*/
            break;

//        case 4:
//        //Dos flechas al diagrama de motor porque en el caso que sea mas grande o igual al 20 tambien va al estado 0
//            if (duty < 40){
//                duty++;
//            }
//            state = 0;
//            break;
    }
    return;
}
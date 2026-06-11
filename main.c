#include <xc.h>
#include "tad_controller.h"

#pragma config OSC = HSPLL
#pragma config PBADEN = DIG
#pragma config WDT = OFF
#pragma config MCLRE = ON
#pragma config DEBUG = OFF
#pragma config PWRT = OFF
#pragma config BOR = OFF
#pragma config LVP = OFF

void __interrupt() RSI_HIGH(void) {
    if (INTCONbits.TMR0IF == 1) {
        RSI_Timer0();
    }
}

void main(void) {
    TI_Init();
    CONTROLLER_init();
    HALL_init();
    SiInit();
    MNG_SIO_init();
    initTeclat();
    pulsadorInit();
    SPEAKER_init();
    LEDS_init();
    TRISBbits.TRISB4 = 0;
    while (1) {
        LATBbits.LATB4 = !LATBbits.LATB4;
        CONTROLLER_motor();
        MNG_SIO_motor();
        motorTeclat();
        motorPulsador();
        SPEAKER_motor();
        LEDS_motorIntensity();
    }
}

#include <xc.h>
#include "timer.h"
#include "tad_manager_lcd.h"

#pragma config OSC = HSPLL
#pragma config PBADEN = DIG
#pragma config WDT = OFF
#pragma config MCLRE = ON
#pragma config DEBUG = OFF
#pragma config PWRT = OFF
#pragma config BOR = OFF
#pragma config LVP = OFF

static char missatge[] = "Jan puto maricon";

void __interrupt() RSI_HIGH(void) {
    if (INTCONbits.TMR0IF == 1) {
        RSI_Timer0();
    }
}

void main(void) {
    TI_Init();

    LcInit(2,16);
    LcClear();
    LcCursorOn();
    LcGotoXY(0,0);

    initManagerLcd();
    LCD_print(missatge);

    while (1) {
        motorManagerLcd();
    }
}
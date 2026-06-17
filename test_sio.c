#include <xc.h>

#include "timer.h"
#include "tad_sio_manual.h"
#include "tad_controlador.h"
#include "tad_controller.h"
#include "tad_manager_lcd.h"

#pragma config OSC = HSPLL
#pragma config PBADEN = DIG
#pragma config WDT = OFF
#pragma config MCLRE = ON
#pragma config DEBUG = OFF
#pragma config PWRT = OFF
#pragma config BOR = OFF
#pragma config LVP = OFF

static char missatge[] = "Jan chupamingas";

void __interrupt() RSI_HIGH(void) {
    if (INTCONbits.TMR0IF) {
        RSI_Timer0();
    }
}

void main(void) {
    ADCON1 = 0x0F;
    SiInit();
    TI_Init();
    initSioAux();
    initControlador();
    
    LcInit(2,16);
    LcClear();
    LcCursorOn();
    LcGotoXY(0,0);

    initManagerLcd();
    LCD_print(missatge);
    initController();
    initPulsador();

    ei();

    while (1) {
//        motorSioAux();
//        motorControlador();
//        motorManagerLcd();
        motorController();
        //motorPulsador();
    }
}
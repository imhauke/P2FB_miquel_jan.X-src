#include <xc.h>
#include "tad_eeprom.h"
#include "tad_validador.h"
#include "tad_manager_sio.h"
#include "tad_heartbeat.h"
#include "tad_pulsador.h"
#include "tad_controller.h"
#include "tad_controller_adc.h"
#include "tad_joystick.h"
#include "tad_ldr.h"

#pragma config OSC = HSPLL
#pragma config PBADEN = DIG
#pragma config WDT = OFF
#pragma config MCLRE = ON
#pragma config DEBUG = OFF
#pragma config PWRT = OFF
#pragma config BOR = OFF
#pragma config LVP = OFF

void __interrupt() RSI_HIGH(void) {
    if (INTCONbits.TMR0IF) {
        RSI_Timer0();
    }
}

void main(void) {
    SiInit();
    TI_Init();

    LcInit(2,16);
    LcClear();
    LcCursorOn();
    LcGotoXY(0,0);

    initController();       // inclou el rellotge (abans tad_hora)
    initControllerAdc();
    initJoystick();
    initValidador();        // inclou el canal serie software (abans tad_sio_manual)
    initHeartbeat();

    while (1) {
        motorController();
        motorJoystick();
        motorControllerAdc();
        motorValidador();   // serie software + parser de la data/hora
        motorHeartbeat();
    }
}

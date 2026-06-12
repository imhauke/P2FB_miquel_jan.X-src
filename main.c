
// cambiar codigo usando numero diferente de variables, definidas de forma diferente, nombres diferentes, usar consatntes con defin,
//  y cambiar logica de flags e ifs, añadidendo mas o menos, y o juntando cosas en un mismo tad

#include "tad_eeprom.h"
#include "tad_validador.h"
#include "tad_hora.h"
#include "tad_manager_sio.h"
#include "tad_heartbeat.h"
#include "tad_pulsador.h"


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
    initSioAux();
    initValidador();
    initHora();
    MNG_SIO_init();
    initController();
    initControllerAdc();
    initJoystick();
    initPulsador();
    initLDR();
    initEeprom();
    initHeartbeat();
    while (1) {
        motorSioAux();
        motorValidador();
        motorHora();
        MNG_SIO_motor();
        motorController();
        motorControllerAdc();
        motorJoystick();
        motorPulsador();
        motorLDR();
        motorEeprom();
        motorHeartbeat();
    }
}
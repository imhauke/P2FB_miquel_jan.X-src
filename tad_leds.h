#ifndef TAD_LEDS_H
#define TAD_LEDS_H

#include <xc.h>
#include "timer.h"

#define LED_INT        LATAbits.LATA2   // Intensity
#define LED_ALARM      LATAbits.LATA3   // State_Alarm
#define LED_OK         LATAbits.LATA4   // State_OK

#define TRIS_INT       TRISAbits.TRISA2
#define TRIS_ALARM     TRISAbits.TRISA3
#define TRIS_OK        TRISAbits.TRISA4

void LEDS_init(void);
void LEDS_motorIntensity(void);

void LEDS_setOK(char value);
void LEDS_setAlarm(char value);
void LEDS_startIntensity(void);
void LEDS_stopIntensity(void);
void LEDS_setDuty(char value);

#endif
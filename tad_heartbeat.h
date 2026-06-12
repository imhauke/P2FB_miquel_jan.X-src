#ifndef _TAD_HEARTBEAT_H_
#define _TAD_HEARTBEAT_H_

#include <xc.h>
#include "timer.h"

// LED Heartbeat per software PWM (no cal CCP). Pin RB0.
#define TRIS_HB     TRISBbits.TRISB0
#define LED_HB      LATBbits.LATB0

// --- Software PWM ---
// Periode PWM = 40 tics (0.5ms/tic -> 20ms -> 50 periodes per segon)
#define HB_PERIODE      40
// Duty: 0..40 (0% .. 100%)
#define HB_DUTY_MAX     40

// Rampa: pugem duty de 0 a 40 i el baixem de 40 a 0.
// 40 passos pujant + 40 baixant = 80 periodes.
// 80 periodes x 20ms = 1600ms -> massa lent.
// Pugem/baixem 2 unitats per periode -> 20 + 20 = 40 periodes = 800ms -> 75 bpm.
#define HB_DUTY_PAS     2

void initHeartbeat(void);
void motorHeartbeat(void);

void HB_start(void);   // activa el batec
void HB_stop(void);    // apaga el LED (rebel.lio)

#endif

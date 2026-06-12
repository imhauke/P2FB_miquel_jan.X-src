#ifndef _TAD_PULSADOR_H_
#define _TAD_PULSADOR_H_

#include <xc.h>
#include "timer.h"

// Polsador del joystick (o un polsador separat segons l'enunciat).
// Digital amb pull-up: premut = 0. Pin RB1.
#define TRIS_SW       TRISBbits.TRISB1
#define PIN_SW        PORTBbits.RB1
#define SW_ANTIREBOT  40   // tics d'antirebot (40 x 0.5ms = 20ms)

void initPulsador(void);
void motorPulsador(void);

char PULSADOR_hiHaSelect(void);  // 1 si s'ha premut (consumeix el flag)

#endif

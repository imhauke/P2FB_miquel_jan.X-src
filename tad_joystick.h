#ifndef _TAD_JOYSTICK_H_
#define _TAD_JOYSTICK_H_

#include <xc.h>

// Joystick analogic: eix X = AN0 (RA0), eix Y = AN1 (RA1)
#define TRIS_JOY_X   TRISAbits.TRISA0
#define TRIS_JOY_Y   TRISAbits.TRISA1

void initJoystick(void);
void motorJoystick(void);

void JOYSTICK_start(void);
char JOYSTICK_finish(void);
char JOYSTICK_getPositionX(void); //1 right, 2 left, 0 mig
char JOYSTICK_hiHaX(void);
char JOYSTICK_hiHaY(void);
char JOYSTICK_getPositionY(void); //1 up, 2 down, 0 mig

#endif
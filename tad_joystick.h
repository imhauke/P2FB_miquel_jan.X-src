#ifndef _TAD_JOYSTICK_H_
#define _TAD_JOYSTICK_H_

#include <xc.h>

void initJoystick(void);
void motorJoystick(void);

void JOYSTICK_start(void);
char JOYSTICK_finish(void);
char JOYSTICK_getPositionX(void); //1 right, 2 left, 0 mig
char JOYSTICK_hiHaX(void);
char JOYSTICK_hiHaY(void);
char JOYSTICK_getPositionY(void); //1 up, 2 down, 0 mig

#endif
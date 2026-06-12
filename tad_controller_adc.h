#ifndef _TAD_CONTROLLER_ADC_H_
#define _TAD_CONTROLLER_ADC_H_

#include <xc.h>
#include "tad_joystick.h"
#include "tad_ldr.h"
#include "tad_pulsador.h"


void initControllerAdc(void);
void motorControllerAdc(void);
char CJ_hiHaRight(void);
char CJ_hiHaLeft(void);
char CJ_hiHaUp(void);
char CJ_hiHaDown(void);
char CJ_hiHaFosc(void);
char CJ_hiHaSelect(void);

#endif
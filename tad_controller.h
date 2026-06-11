#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_

#include <xc.h>
#include "tad_manager_sio.h"
#include "timer.h"
#include "tad_hall.h"
#include "tad_teclat.h"
#include "tad_pulsador.h"
#include "tad_speaker.h"
#include "tad_leds.h"

#define PIN_PREDEFINIT "1111111"
#define MAX_PIN 7
#define MAX_INTENTS 3

void CONTROLLER_init(void);
void CONTROLLER_motor(void);

#endif

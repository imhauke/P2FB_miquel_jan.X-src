#ifndef MANAGER_SIO_H
#define MANAGER_SIO_H

#include <xc.h>
#include "tad_sio.h"

// Frases fixes PIC -> Java (protocol 1 char + dades, acabades en \r\n\0)
#define RSP_FINISH_STR      "F\r\n\0"
#define RSP_SLEEP_OK_STR    "Y\r\n\0"
#define RSP_SLEEP_NOK_STR   "N\r\n\0"

// Moviments del joystick (PIC -> Java)
#define RSP_MOVE_UP_STR     "U\r\n\0"
#define RSP_MOVE_DOWN_STR   "D\r\n\0"
#define RSP_MOVE_LEFT_STR   "L\r\n\0"
#define RSP_MOVE_RIGHT_STR  "G\r\n\0"
#define RSP_SELECT_STR      "X\r\n\0"

void MNG_SIO_init(void);
void MNG_SIO_motor(void);

void enviaMissatge(char* m);
void enviaCaracter(char c);
char statusFiMissatge(void);

#endif

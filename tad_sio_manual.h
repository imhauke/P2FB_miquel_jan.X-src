#ifndef _TAD_SIO_MANUAL_H_
#define _TAD_SIO_MANUAL_H_

#include <xc.h>
#include "timer.h"

#define CONFIG_TX TRISCbits.TRISC0
#define CONFIG_RX TRISCbits.TRISC1

#define PIN_TX LATCbits.LATC0
#define PIN_RX PORTCbits.RC1

#define TEMPS_BIT 16

void initSioAux(void);
void motorSioAux(void);

char SIO_AUX_txAvailable(void);   // 1 si es pot enviar
void SIO_AUX_sendByte(char c);    // envia un byte (Pre: txAvailable)
char SIO_AUX_byteAvailable(void); // 1 si hi ha byte rebut pendent
char SIO_AUX_byteGet(void);       // recull el byte rebut

#endif
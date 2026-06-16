#ifndef SITSIO_H
#define SITSIO_H

#include <xc.h>
#include "timer.h"

// Canal Java (EUSART hardware): TX = RC6, RX = RC7
#define TRIS_SIO_TX   TRISCbits.TRISC6
#define TRIS_SIO_RX   TRISCbits.TRISC7

int SiCharAvail(void);

char SiGetChar(void);

void SiSendChar (char c);

void SiInit(void);

char SiAvail(void);


#endif
#ifndef SITSIO_H
#define SITSIO_H

#include <xc.h>
#include "timer.h"


int SiCharAvail(void);

char SiGetChar(void);

void SiSendChar (char c);

void SiInit(void);

char SiAvail(void);


#endif
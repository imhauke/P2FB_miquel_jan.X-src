#ifndef _TAD_TECLAT_H_
#define _TAD_TECLAT_H_

#include <xc.h>
#include "timer.h"

#define C0 PORTBbits.RB1
#define C1 PORTBbits.RB2
#define C2 PORTBbits.RB3 
#define F0 LATCbits.LATC0
#define F1 LATCbits.LATC1
#define F2 LATCbits.LATC2
#define F3 LATCbits.LATC3

#define FILTREREBOTS 10
#define TEMPS_TECLA 2000

void initTeclat(void);
void motorTeclat(void);

char TECLAT_getTecla(void);
char TECLAT_HHT(void);

#endif

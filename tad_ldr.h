#ifndef _TAD_LDR_H_
#define _TAD_LDR_H_

#include <xc.h>

// LDR (sensor de llum) analogic: AN2 (RA2)
#define TRIS_LDR   TRISAbits.TRISA2

void initLDR(void);
void motorLDR(void);

void LDR_start(void);
char LDR_finish(void);

unsigned char LDR_getValue(void);

#endif
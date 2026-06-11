#ifndef _TAD_LDR_H_
#define _TAD_LDR_H_

#include <xc.h>

void initLDR(void);
void motorLDR(void);

void LDR_start(void);
char LDR_finish(void);

unsigned char LDR_getValue(void);

#endif
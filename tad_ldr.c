#include "tad_ldr.h"

static char start;
static unsigned char value;

void initLDR(void) {

}

void motorLDR(void) {
    static char state = 0;

    switch(state) {

        case 0:
            if(start) {
                ADCON0bits.CHS3 = 0;
                ADCON0bits.CHS2 = 0;
                ADCON0bits.CHS1 = 1;
                ADCON0bits.CHS0 = 0;
                ADCON0bits.GO_DONE = 1;
                state = 1;
            }
            break;
        
        case 1:
            if(ADCON0bits.GO_DONE == 0) {
                value = ADRESH;
                state = 0;
                start = 0;
            }
            break;
        
    }
}

void LDR_start(void) {
    start = 1;
}

char LDR_finish(void) {
    return !start;
}

unsigned char LDR_getValue(void) {
    return value;
}
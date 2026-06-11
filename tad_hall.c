#include "tad_hall.h"

void HALL_init(void) {
    TRISCbits.RC5 = 1;
}

char hiHaCampMagnetic(void){
    return !PORTCbits.RC5;
}
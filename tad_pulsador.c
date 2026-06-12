#include "tad_pulsador.h"

static char flag_select;
static unsigned char t;

void initPulsador(void) {
    TRIS_SW = 1;        // entrada
    flag_select = 0;
    TI_NewTimer(&t);
}

char PULSADOR_hiHaSelect(void) {
    char aux = flag_select;
    flag_select = 0;
    return aux;
}

// Antirebot del polsador: detecta el flanc de premuda (premut = 0)
void motorPulsador(void) {
    static char state = 0;

    switch (state) {

        case 0:                                 // espera premuda
            if (PIN_SW == 0) {
                TI_ResetTics(t);
                state = 1;
            }
            break;

        case 1:                                 // confirma premuda (antirebot)
            if (PIN_SW == 1) {
                state = 0;                      // rebot, descarta
            } else if (TI_GetTics(t) >= SW_ANTIREBOT) {
                flag_select = 1;                // premuda valida
                state = 2;
            }
            break;

        case 2:                                 // espera que es deixi anar
            if (PIN_SW == 1) {
                TI_ResetTics(t);
                state = 3;
            }
            break;

        case 3:                                 // antirebot de deixar anar
            if (PIN_SW == 0) {
                state = 2;
            } else if (TI_GetTics(t) >= SW_ANTIREBOT) {
                state = 0;
            }
            break;
    }
}

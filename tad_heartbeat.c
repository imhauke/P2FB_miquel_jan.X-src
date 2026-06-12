#include "tad_heartbeat.h"

static char duty;        // 0..HB_DUTY_MAX (PWM actual)
static char pujant;      // 1 = la rampa puja, 0 = baixa
static char start;       // 1 = batec actiu
static unsigned char t;

void initHeartbeat(void) {
    TRIS_HB = 0;
    LED_HB  = 0;
    duty    = 0;
    pujant  = 1;
    start   = 1;          // arrenca bategant per defecte
    TI_NewTimer(&t);
}

void HB_start(void) {
    start = 1;
}

void HB_stop(void) {
    start = 0;
    LED_HB = 0;
    duty = 0;
    pujant = 1;
}

void motorHeartbeat(void) {
    static char state = 0;

    switch (state) {

        // Inici del periode PWM: encen el LED (si hi ha duty)
        case 0:
            if (!start) {
                LED_HB = 0;
                break;
            }
            if (duty > 0) {
                LED_HB = 1;
            }
            TI_ResetTics(t);
            state = 1;
            break;

        // Manté el LED encès durant 'duty' tics
        case 1:
            if (TI_GetTics(t) >= duty) {
                LED_HB = 0;
                state = 2;
            }
            break;

        // Espera fins completar el periode PWM
        case 2:
            if (TI_GetTics(t) >= HB_PERIODE) {
                state = 3;
            }
            break;

        // Ajusta el duty (rampa amunt/avall) i torna a comencar el periode
        case 3:
            if (pujant) {
                duty += HB_DUTY_PAS;
                if (duty >= HB_DUTY_MAX) {
                    duty = HB_DUTY_MAX;
                    pujant = 0;
                }
            } else {
                if (duty <= HB_DUTY_PAS) {
                    duty = 0;
                    pujant = 1;
                } else {
                    duty -= HB_DUTY_PAS;
                }
            }
            state = 0;
            break;
    }
}

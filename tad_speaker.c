#include <xc.h>
#include "tad_speaker.h"
#include "timer.h"

static unsigned char t;
static char start, start_greu, alarma;
static int num_periods;
static char duty;

void SPEAKER_init(void) {
    SPEAKER_TRIS = 0;
    SPEAKER_PIN = 0;

    TI_NewTimer(&t);
}

void SPEAKER_startAgut(void) {
    start = 1;
    duty = FREQ_AGUT;
}

void SPEAKER_startGreu(void) {
    start_greu = 1;
    duty = FREQ_GRAVE;
}

void SPEAKER_startAlarma(void) {
    alarma = 1;
    duty = FREQ_AGUT; //pitido agudo repetitivo
}

void SPEAKER_stop(void) {
    start = 0;
    alarma = 0;
}

void SPEAKER_motor(void) {  
    static unsigned char state = 0;

    switch(state) {
        case 0:
            if (start){
                TI_ResetTics(t);
                state = 1;
            }else if (start_greu) {
                num_periods = 0;
                TI_ResetTics(t);
                state = 4;
            } else if (alarma){
                TI_ResetTics(t);
                num_periods = 0;
                state = 2;
            } else {
                SPEAKER_PIN = 0;
                state = 0;
            }
            break;
            
        case 1:
            //sonido continuo
            if (TI_GetTics(t) >= duty) {
                SPEAKER_PIN = !SPEAKER_PIN;
                state = 0;
            }
            break;
            
        case 2:
            //sonido periódico - alarma (pi pi pi pi...)
            if (!alarma) {
                SPEAKER_PIN = 0;
                state = 0;
            } else {
                if (TI_GetTics(t) >= duty) {
                    SPEAKER_PIN = !SPEAKER_PIN;
                    num_periods++;
                    TI_ResetTics(t);
                }
                //después de 150ms (300 tics * 0.5ms) de tono, pasar a pausa
                if (num_periods == 600) {
                    SPEAKER_PIN = 0;
                    TI_ResetTics(t);
                    state = 3;  //ir a pausa
                }
            }
            break;
            
        case 3:
            //pausa entre pitidos (300ms = 600 tics * 0.5ms)
            if (!alarma) {
                state = 0;
            } else if (TI_GetTics(t) >= 600) {
                TI_ResetTics(t);
                state = 2;  //volver a generar pitido
                num_periods = 0;
            }
            break;
            
        case 4:
            if (TI_GetTics(t) >= duty) {
                TI_ResetTics(t);
                SPEAKER_PIN = 0;
                state = 5;
            }
            break;
            
        case 5:
            if (TI_GetTics(t) >= duty) {
                TI_ResetTics(t);
                SPEAKER_PIN = 1;
                num_periods++;
                state = 6;
            }
            break;
            
        case 6:
            if(num_periods == 50) {
                start_greu = 0;
                state = 0;
            }else{
                state = 4;
            }
            break;
            
        default:
            state = 0;
            break;
    }
}
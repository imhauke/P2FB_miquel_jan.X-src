#include "tad_controller.h"

static unsigned char t;
static char count_tecles,i;
static unsigned char count_segons;
static char pin[MAX_PIN];
static int ticks_alarma;
static char intents_erronis;
static char index = 0;
static char buffer[3];
static unsigned char t_led;
static char duty_leds;

void CONTROLLER_init(void) {
    TI_NewTimer(&t);
    TI_NewTimer(&t_led);
}

void CONTROLLER_motor(void){

    static char state = 0;

    switch(state){

        case 0:
            if(statusFiMissatge()) {
                enviaMissatge(FRASE_1);
                LEDS_setOK(1);
                LEDS_setAlarm(0);
                SPEAKER_stop();
                state = 1;
            }
        break;

        case 1:
            if(statusFiMissatge()) {
                if(hiHaCampMagnetic()) {
                    enviaMissatge(FRASE_2);
                    TI_ResetTics(t);
                    SPEAKER_startAgut();
                    state = 2;
                }
            }
            break;

        case 2:
            if(TI_GetTics(t) >= 4000) {
                SPEAKER_stop();
                enviaMissatge(FRASE_3);
                TI_ResetTics(t);
                state = 3;
            }
            break;

        case 3:
            if(TI_GetTics(t) >= 1000) {
                enviaMissatge(FRASE_4);
                count_tecles = 0;
                count_segons = 0;
                ticks_alarma = 2000; //1 segon entre tons greus
                intents_erronis = 0;
                LEDS_startIntensity();
                duty_leds = 0;
                LEDS_setDuty(duty_leds);
                TI_ResetTics(t);
                TI_ResetTics(t_led);
                state = 4;
            }
            break;

        case 4:
            if(TECLAT_HHT()) {
                pin[count_tecles] = TECLAT_getTecla();
                enviaCaracter(pin[count_tecles]);
                count_tecles++;
                state = 5;
            }

            // subir duty del LED cada 3 segundos durante los 2 minutos
            if(TI_GetTics(t_led) >= 6000) {
                TI_ResetTics(t_led);
                if(duty_leds < 40) {
                    duty_leds++;
                    LEDS_setDuty(duty_leds);
                }
            }

            if(TI_GetTics(t) >= ticks_alarma) {
                SPEAKER_startGreu();
                TI_ResetTics(t);
                count_segons++;
            }

            if(count_segons == 105) {
                // cuando pase 1:45, sonará el grave cada medio segundo
                ticks_alarma = 1000;
            }
            else if(count_segons == 135) {
                // han pasado 2 minutos y se activa el sistema de alarma
                state = 9;
            }

            break;

        case 5:
            if(count_tecles == MAX_PIN) {
                i = 0;
                state = 6;
            }else{
                state = 4;
            }
            break;

        case 6:
            if(i < MAX_PIN) {
                if(PIN_PREDEFINIT[i] == pin[i]) {
                    //coincideix el caracter
                    i++;
                }else{
                    //hi ha una diferencia en el pin
                    intents_erronis++;
                    state = 7;
                }
            }else{
                //els pins son iguals
                // JAN
                LEDS_stopIntensity();
                enviaMissatge(FRASE_8);
                TI_ResetTics(t);
                state = 11;
            }
            break;

        case 7:
            if(intents_erronis < MAX_INTENTS) {
                enviaMissatge(FRASE_5);
                state = 8;
            }else{
                state = 9;
            }
            break;

        case 8:
            if(statusFiMissatge()) {
                count_tecles = 0;
                enviaMissatge(FRASE_4);
                state = 4;
            }
            break;

        case 9:
            if(SiAvail()) {
                enviaMissatge(FRASE_6);
                LEDS_setOK(0); //Apaga led verde
                LEDS_setAlarm(1);
                LEDS_stopIntensity();
                SPEAKER_startAlarma();
                TI_ResetTics(t);
                state = 70;
            }
            break;
        case 70:
            if(TI_GetTics(t)>20500){
                SPEAKER_stop();
                state = 10;
            }
            
            break;
        case 10:
            if(statusFiMissatge()) {
                enviaMissatge(FRASE_7);
                //JAN
                index = 0;
                state = 16; //gestionar recepció del reset system
            }
            break;
        
        case 11:
            if (TI_GetTics(t) >= 4000) {
                enviaMissatge(FRASE_9);
                TI_ResetTics(t);
                SPEAKER_startAgut();
                state = 50;
            }
            break;

        case 50:
            if (TI_GetTics(t) >= 1000) {
                SPEAKER_stop();
                state = 12;
            }
            break;
        
        case 12:
            
            if (HiHaPulsador()) {
                //exit requested
                enviaMissatge(FRASE_10);
                index = 0;
                state = 13;
            }
            break;
        
        case 13:
            if (SiCharAvail()) {
                char c = SiGetChar();

                if(c == '\r') {
                    state = 18;
                }else{
                    SiSendChar(c);
                    if(index < 3) {
                        buffer[index] = c;
                    }
                    index++;
                }
            }
            break;
        
        case 14:
                enviaMissatge(FRASE_11);
                TI_ResetTics(t);
                
                
                state = 15;
            break;

        case 15:
            if (TI_GetTics(t) >= 4000) {
                enviaMissatge(FRASE_12);
                SPEAKER_startAgut();
                TI_ResetTics(t);
                state = 51;
            }
            break;
            
        case 51:
            if (TI_GetTics(t) >= 1000) {
                SPEAKER_stop();
                state = 0;
            }
            break;

        case 16:
            if (SiCharAvail()) {
                char c = SiGetChar();

                if(c == '\r') {
                    state = 17;
                }else{
                    SiSendChar(c);
                    if(index < 3) {
                        buffer[index] = c;
                    }
                    index++;
                }
            }
            break;

        case 17:
            if(index == 3) {
                if(buffer[0] == 'Y' && buffer[1] == 'e' && buffer[2] == 's') {
                    state = 30; //Reset system ok
                }else{
                    state = 10; //tornem a mostrar reset system
                }
            }else{
                state = 10;
            }
            break;
//
        case 18:
            if(index == 3) {
                if(buffer[0] == 'Y' && buffer[1] == 'e' && buffer[2] == 's') {
                    state = 20; //Obrir portes de sortida
                }else{
                    state = 19; //tornem a mostrar reset system
                }
            }else if(index == 2) {
                if(buffer[0] == 'N' && buffer[1] == 'o') {
                    TI_ResetTics(t);
                    state = 9; //Activar alarma
                }else{
                    state = 19; //tornem a mostrar reset system
                }
            }else{
                state = 19;
            }
            break;

        case 19:
            enviaMissatge(FRASE_10);
            index = 0;
            state = 13;
            break;

        case 20:
            if(statusFiMissatge()){
                enviaMissatge(FRASE_11);
                TI_ResetTics(t);
                state = 21;
            }
            break;

        case 21:
            if(TI_GetTics(t) >= 4000) {
                SPEAKER_startAgut();
                enviaMissatge(FRASE_12);
                TI_ResetTics(t);
                state = 22;
            }
            break;
            
        case 22:
            if(TI_GetTics(t) >= 1000) {
                SPEAKER_stop();
                state = 30;
            }
            break;
            
            
        case 30:
            if(statusFiMissatge()) {
                enviaMissatge(FRASE_13);
                state = 0; //nova linia i reset system
            }
            break;
    }
}
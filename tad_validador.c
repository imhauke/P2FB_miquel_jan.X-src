#include "tad_validador.h"

static char buffer[VAL_BUFFER];
static char idx;
static char flag_ok;
static char c;

// Valors parsejats
static char dia, mes, hora, minut, segon;

// Per enviar el feedback caracter a caracter
static const char *msg;

void initValidador(void) {
    idx     = 0;
    flag_ok = 0;
}

char V_isFlagOk(void) {
    return flag_ok;
}

// Converteix 2 chars ASCII a numero (sense multiplicacions: x10 = sumes)
char dosDigits(char a, char b) {
    char d = a - '0';
    char u = b - '0';
    return d + d + d + d + d + d + d + d + d + d + u; // d*10 + u
}

// Valida rang [mn, mx]
char rang(char v, char mn, char mx) {
    return (v >= mn && v <= mx);
}

void motorValidador(void) {
    static char state = 0;

    switch (state) {

        // Espera un caracter; quan arriba, el guarda i passa a fer-ne eco
        case 0:
            //TODO integrar sio manual directament dins del validador
            /*
             if(start_tx) {
                PIN_TX = 0;

                TI_ResetTics(t);

                state = 1;

            } else if(PIN_RX == 0) { //rebem start bit

                byteRx = 0x00;
                j = 0;

                TI_ResetTics(t);

                state = 5;
            }else{
             * 
             
             
             */
            if (SIO_AUX_byteAvailable()) { //TODO: no cal funcio, nomes activar un flag byte available quan rebis un char
                c = SIO_AUX_byteGet(); //TODO: no es una funcio sino una variable
                SIO_AUX_sendByte(c); //TODO: start_tx = 1
                if (c == '\r') {
                    state = 6;      // fi de trama: salt de linia i valida
                } else {
                    buffer[idx] = c;
                    idx++;
                }
            }
            break;

        // Salt de linia perque la resposta surti a la linia seguent
        case 6:
            if (SIO_AUX_txAvailable()) {
                SIO_AUX_sendByte('\n');
                // Valida format "DD/MM HH:MM:SS" (14 chars, separadors a la seva posicio)
                dia   = dosDigits(buffer[0],  buffer[1]);
                mes   = dosDigits(buffer[3],  buffer[4]);
                hora  = dosDigits(buffer[6],  buffer[7]);
                minut = dosDigits(buffer[9],  buffer[10]);
                segon = dosDigits(buffer[12], buffer[13]);

                if (rang(dia,1,31) && rang(mes,1,12) &&
                    rang(hora,0,23) && rang(minut,0,59) && rang(segon,0,59)) {
                    flag_ok = 1; //TODO moure al Hora_set del controller
                    HORA_set(dia, mes, hora, minut, segon); //TODO integrar tad hora amb el controller i el controller es l'unic que tindra el control de l'hora
                    
                    //TODO: enviar dia i mes ascii tambe al controller perque pugui mostrarho facil
                    
                    msg = MSG_OK;
                } else {
                    msg = MSG_ERR;
                }

                idx = 0;
                state = 3;
            }
            break;

        


        // Envia el feedback pel terminal, caracter a caracter
        case 3:
            if (msg[idx] == '\0') {
                idx = 0;
                state = 0;
            } else if (SIO_AUX_txAvailable()) {
                SIO_AUX_sendByte(msg[idx]);
                idx++;
            }
            break;
    }
}
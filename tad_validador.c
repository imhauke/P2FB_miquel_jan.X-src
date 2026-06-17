#include "tad_validador.h"

static char buffer[VAL_BUFFER];
static char idx;
static char flag_ok;
static char c;

// Valors parsejats
static char dia, mes, hora, minut, segon;

// Per enviar el feedback caracter a caracter
static const char *msg;
static char msg_i;

void initValidador(void) {
    idx     = 0;
    flag_ok = 0;
}

char V_isFlagOk(void) {
    return flag_ok;
}

// Converteix 2 chars ASCII a numero (sense multiplicacions: x10 = sumes)
static char dosDigits(char a, char b) {
    char d = a - '0';
    char u = b - '0';
    return d + d + d + d + d + d + d + d + d + d + u; // d*10 + u
}

// Valida rang [mn, mx]
static char rang(char v, char mn, char mx) {
    return (v >= mn && v <= mx);
}

void motorValidador(void) {
    static char state = 0;

    switch (state) {

        // Espera un caracter; quan arriba, el guarda i passa a fer-ne eco
        case 0:
            if (SIO_AUX_byteAvailable()) {
                c = SIO_AUX_byteGet();
                idx = 0;
                state = 4;          // eco del caracter abans de processar-lo
            }
            break;

        // Acumula la resta de la trama (amb eco de cada caracter)
        case 1:
            if (SIO_AUX_byteAvailable()) {
                c = SIO_AUX_byteGet();
                state = 5;          // eco i despres acumula
            }
            break;

        // --- Eco del primer caracter ---
        case 4:
            if (SIO_AUX_txAvailable()) {
                SIO_AUX_sendByte(c);
                if (c != '\r' && c != '\n') {
                    buffer[0] = c;
                    idx = 1;
                    state = 1;
                } else {
                    state = 0;      // linia buida, ignora
                }
            }
            break;

        // --- Eco d'un caracter de la trama, despres l'acumula ---
        // L'Enter (\r o \n) marca el final de la trama -> a validar.
        case 5:
            if (SIO_AUX_txAvailable()) {
                SIO_AUX_sendByte(c);
                if (c == '\r' || c == '\n') {
                    state = 6;      // fi de trama: salt de linia i valida
                } else {
                    if (idx < VAL_BUFFER) { buffer[idx] = c; }
                    idx++;
                    state = 1;      // seguent caracter
                }
            }
            break;

        // Salt de linia perque la resposta surti a la linia seguent
        case 6:
            if (SIO_AUX_txAvailable()) {
                SIO_AUX_sendByte('\n');
                state = 2;
            }
            break;

        // Valida format "DD/MM HH:MM:SS" (14 chars, separadors a la seva posicio)
        case 2:
            if (idx == VAL_BUFFER &&
                buffer[2] == '/' && buffer[5] == ' ' &&
                buffer[8] == ':' && buffer[11] == ':') {

                dia   = dosDigits(buffer[0],  buffer[1]);
                mes   = dosDigits(buffer[3],  buffer[4]);
                hora  = dosDigits(buffer[6],  buffer[7]);
                minut = dosDigits(buffer[9],  buffer[10]);
                segon = dosDigits(buffer[12], buffer[13]);

                if (rang(dia,1,31) && rang(mes,1,12) &&
                    rang(hora,0,23) && rang(minut,0,59) && rang(segon,0,59)) {
                    flag_ok = 1;
                    HORA_set(dia, mes, hora, minut, segon);
                    msg = MSG_OK;
                } else {
                    msg = MSG_ERR;
                }
            } else {
                msg = MSG_ERR;
            }
            msg_i = 0;
            state = 3;
            break;

        // Envia el feedback pel terminal, caracter a caracter
        case 3:
            if (msg[msg_i] == '\0') {
                idx = 0;
                state = 0;
            } else if (SIO_AUX_txAvailable()) {
                SIO_AUX_sendByte(msg[msg_i]);
                msg_i++;
            }
            break;
    }
}
#ifndef _TAD_VALIDADOR_H_
#define _TAD_VALIDADOR_H_

#include <xc.h>
#include "timer.h"

// El validador integra el canal serie software (Serial_Time) i el parser
// de la trama "DD/MM HH:MM:SS". Rep pel terminal, fa eco, valida i, si es
// correcta, crida HORA_set(...) del controller (que controla el rellotge).

// --- Pins del canal serie software (abans tad_sio_manual) ---
#define CONFIG_TX TRISCbits.TRISC0
#define CONFIG_RX TRISCbits.TRISC1
#define PIN_TX    LATCbits.LATC0
#define PIN_RX    PORTCbits.RC1

#define TEMPS_BIT 16      // tics per bit (a 0.2ms/tic -> ~300 baud)

#define VAL_BUFFER   14   // "DD/MM HH:MM:SS" = 14 chars

// Missatges de feedback pel terminal (acabats en \r\n\0)
#define MSG_OK    "Date and time correct\r\n\0"
#define MSG_ERR   "Please input a correct date\r\n\0"

void initValidador(void);
void motorValidador(void);

#endif

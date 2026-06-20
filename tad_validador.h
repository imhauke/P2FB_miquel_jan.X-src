#ifndef _TAD_VALIDADOR_H_
#define _TAD_VALIDADOR_H_

#include <xc.h>
#include "tad_sio_manual.h"
#include "tad_hora.h"

// Rep la trama "DD/MM HH:MM:SS" pel canal sioAux (Serial_Time),
// la valida i, si es correcta, arrenca el TAD Hora i posa flag_ok.
// Tambe envia feedback pel terminal ("Date and time correct" / error).

#define VAL_BUFFER   14   // "DD/MM HH:MM:SS" = 14 chars

// Missatges de feedback pel terminal
#define MSG_OK    "Date and time correct\r\n\0"
#define MSG_ERR   "Please input a correct date\r\n\0"

void initValidador(void);
void motorValidador(void);

// Consumit pel TAD Controller
char V_isFlagOk(void);    // 1 quan ja s'ha rebut una data/hora valida

#endif

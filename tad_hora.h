#ifndef _TAD_HORA_H_
#define _TAD_HORA_H_

#include <xc.h>
#include "timer.h"

// Fuente unica de tiempo del sistema.
// Recibe la hora inicial (del TAD Validador) y a partir de ahi
// avanza el reloj cada segundo. Todos los demas TADs piden la hora aqui.

// 1 segon en tics (1 tic = 0.5ms @ 40MHz -> 2000 tics = 1s)
#define HORA_SEGON      2000

void initHora(void);
void motorHora(void);

// Arranca el reloj con una hora inicial valida
void HORA_set(char dia, char mes, char hora, char minut, char segon);

// 1 si el reloj ja s'ha inicialitzat (hi ha hora valida)
char HORA_running(void);

// Getters de la hora actual (font unica)
char HORA_getDia(void);
char HORA_getMes(void);
char HORA_getHora(void);
char HORA_getMinut(void);
char HORA_getSegon(void);

#endif

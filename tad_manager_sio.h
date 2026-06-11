#ifndef MANAGER_SIO_H
#define MANAGER_SIO_H

#include <xc.h>
#include "tad_sio.h"

#define FRASE_1 "> LSBank - New Day!\r\n\0"
#define FRASE_2 "> LSBank - Open exterior door\r\n\0"
#define FRASE_3 "> LSBank - Closed exterior door\r\n\0"
#define FRASE_4 "> LSBank - Enter PIN: \0"
#define FRASE_5 "\r\n> LSBank - Permission Denied\r\n\0"
#define FRASE_6 "\r\n> LSBank - Thief Intercepted\0"
#define FRASE_7 "\r\n> LSBank - Reset System: \0"
#define FRASE_8 "\r\n> LSBank - Open interior door\0"
#define FRASE_9 "\r\n> LSBank - Closed interior door\0"
#define FRASE_10 "\r\n> LSBank - Exit Requested: \0"
#define FRASE_11 "\r\n> LSBank - Open both doors\0"
#define FRASE_12 "\r\n> LSBank - Closed both doors\0"
#define FRASE_13 "\r\n\0"

void MNG_SIO_init(void);
void MNG_SIO_motor(void);

void enviaMissatge(char* m);
void enviaCaracter(char c);
char statusFiMissatge(void);
/*char hiHaNovaResposta(void)
void getResposta(char *h)*/

#endif
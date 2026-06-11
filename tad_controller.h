#ifndef _TAD_CONTROLLER_H_
#define _TAD_CONTROLLER_H_

#include <xc.h>
#include "tad_sio.h"
#include "tad_manager_lcd.h"
#include "tad_lcd.h"
#include "timer.h"
#include "tad_controller_adc.h"
#include "tad_sio_manual.h"

// Comandament d'inicialitzacio (ha de coincidir amb FarmConstants.CMD_INITIALIZE)
#define CMD_INITIALIZE  'I'

// Especies: [0]=vaca [1]=cavall [2]=porc [3]=gallina
#define NUM_ESPECIES        4
#define MAX_ANIMALS_TOTAL   24

// Intervals de produccio fixos per especie (en segons)
#define TEMPS_PROD_VACA     47
#define TEMPS_PROD_CAVALL   23
#define TEMPS_PROD_PORC     31
#define TEMPS_PROD_GALLINA  13

// Son critic individual: 2 min des de la generacio de cada animal (en segons)
#define TEMPS_SON           120

// 1 segon en tics (1 tic = 0.5ms @ 40MHz -> 2000 tics = 1s)
#define SEGON               2000

// Wrappers canal Java (EUSART hardware)
#define SiCharAvail()       SIO_byteAvailable()
#define SiGetChar()         SIO_byteGet()
#define SiSendByte(x)       { while(!SIO_txReady()); SIO_sendByte(x); }

typedef struct {
    char tipus;     // 1=vaca 2=cavall 3=porc 4=gallina (0=slot buit)
    char despert;   // 1=despert, 0=son critic
    char count_son; // segons des de la seva generacio (cap a son critic)
} Animal;

void initController(void);
void motorController(void);
void guardaCaracterTemps(void);
void ferItoaTemps(void);
void guardaTempsAnimal(void);

// Oferts pel TAD Validador (sio_manual)
char V_isFlagOk(void);
char V_getData(char idx);

#endif

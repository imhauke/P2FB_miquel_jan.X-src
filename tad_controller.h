#ifndef _TAD_CONTROLLER_H_
#define _TAD_CONTROLLER_H_

#include <xc.h>
#include "tad_sio.h"
#include "tad_manager_lcd.h"
#include "tad_lcd.h"
#include "timer.h"
#include "tad_controller_adc.h"
#include "tad_sio_manual.h"
#include "tad_validador.h"
#include "tad_hora.h"
#include "tad_eeprom.h"
#include "tad_manager_sio.h"
#include "tad_heartbeat.h"

// resp[] ha de ser prou gran per "P 255$255$255$255\r\n\0"

// --- Protocol Java -> PIC (1 char per comanda) ---
#define CMD_INITIALIZE       'I'
#define CMD_GET_ANIMALS      'A'
#define CMD_GET_PRODUCTS     'P'
#define CMD_CONSUME          'C'
#define CMD_RESET            'R'
#define CMD_START_REBELLION  'B'
#define CMD_STOP_REBELLION   'E'
#define CMD_SLEEP            'S'

// --- Protocol PIC -> Java (1 char per resposta) ---
#define RSP_DATA_PRODUCTS    'P'
#define RSP_DATA_ANIMAL      'A'
#define RSP_FINISH           'F'
#define RSP_SLEEP_OK         'Y'
#define RSP_SLEEP_NOK        'N'

// Estats animal per al protocol (text)
#define TXT_SLEEP   "SLEEP"
#define TXT_AWAKE   "AWAKE"

// Noms d'especie per al protocol Java (tipus 1..4)
#define PROTO_VACA      "VACA"
#define PROTO_CAVALL    "CAVALL"
#define PROTO_PORC      "PORC"
#define PROTO_GALLINA   "GALLINA"

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
// 5 segons (espera per tapar l'LDR)
#define TICKS_5S            10000

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

#endif
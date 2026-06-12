#ifndef _MANAGER_LCD_
#define _MANAGER_LCD_

#include <xc.h>
#include "tad_lcd.h"
#include "timer.h"

// Tipus de notificacio
#define NOTIF_ANIMAL    0
#define NOTIF_PRODUCTE  1

// Capacitat de la cua de notificacions (>=3 segons enunciat)
#define NOTIF_MAX       4

// Temps minim que es mostra una notificacio: 3s (1 tic = 0.5ms -> 6000 tics)
#define TICKS_NOTIF     6000

// Prefixos de notificacio
#define TXT_NOU_ANIMAL    "Nou Animal "
#define TXT_NOU_PRODUCTE  "Nou Producte "

// Noms d'especie (index 0..3: vaca, cavall, porc, gallina)
#define TXT_VACA      "Vaca"
#define TXT_CAVALL    "Cavall"
#define TXT_PORC      "Porc"
#define TXT_GALLINA   "Gallina"

// Noms de producte (index 0..3: llet, pinzell, pernil, ous)
#define TXT_LLET      "Llet"
#define TXT_PINZELL   "Pinzell"
#define TXT_PERNIL    "Pernil"
#define TXT_OUS       "Ous"

void initManagerLcd(void);
void motorManagerLcd(void);

void LCD_print(char *frase);    // Demana pintar 'frase' a la linia 1
void LCD_print2(char *frase);   // Demana pintar 'frase' a la linia 2

char LCD_flag1(void);           // 1 si hi ha impressio pendent a linia 1
char LCD_flag2(void);           // 1 si hi ha impressio pendent a linia 2

void LCD_clear(void);           // Demana esborrar tot el LCD

// Encua una notificacio: "Nou Animal <Tipus>: <num>" o "Nou Producte <Tipus>: <num>
// kind  = NOTIF_ANIMAL / NOTIF_PRODUCTE
// tipus = 0..3 (especie o producte)
// num   = numero a mostrar (animal generat o total de producte)
void LCD_notifica(char kind, char tipus, char num);

#endif

#ifndef _TAD_EEPROM_H_
#define _TAD_EEPROM_H_

#include <xc.h>

// --- Mapa de la EEPROM ---
// Addr 0          : q_animals (total d'animals guardats)
// Addr 1..(1+144) : 24 animals x 6 bytes (tipus, dia, mes, hora, min, seg)
#define EE_ADDR_QANIM     0
#define EE_ADDR_ANIMALS   1
#define EE_BYTES_ANIMAL   6     // tipus + dia + mes + hora + min + seg

// Offsets dins del bloc de cada animal
#define EE_OFF_TIPUS      0
#define EE_OFF_DIA        1
#define EE_OFF_MES        2
#define EE_OFF_HORA       3
#define EE_OFF_MIN        4
#define EE_OFF_SEG        5

void initEeprom(void);
void motorEeprom(void);

// Lectura sincrona (1 cicle, no bloqueja): retorna el byte de l'adreca.
char EE_readByte(char addr);

// Escriptura asincrona cooperativa:
// EE_writeByte encua una escriptura. EE_busy() indica si encara treballa.
void EE_writeByte(char addr, char dada);
char EE_busy(void);   // 1 si hi ha una escriptura en curs

#endif

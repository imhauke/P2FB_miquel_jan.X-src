#include "tad_eeprom.h"

static char wr_addr;     // adreca pendent d'escriure
static char wr_dada;     // dada pendent d'escriure
static char wr_flag;     // 1 = hi ha una escriptura demanada/en curs

void initEeprom(void) {
    wr_flag = 0;
    EECON1bits.EEPGD = 0;   // acces a data EEPROM (no flash)
    EECON1bits.CFGS  = 0;
}

// --- Lectura sincrona: el datasheet diu que la dada esta disponible
//     al cicle seguent, aixi que no cal esperar ---
char EE_readByte(char addr) {
    EEADR = addr;
    EECON1bits.EEPGD = 0;
    EECON1bits.CFGS  = 0;
    EECON1bits.RD    = 1;   // hardware el neteja sol
    return EEDATA;
}

// --- Demana una escriptura (no bloqueja) ---
void EE_writeByte(char addr, char dada) {
    wr_addr = addr;
    wr_dada = dada;
    wr_flag = 1;
}

char EE_busy(void) {
    return wr_flag;
}

void motorEeprom(void) {
    static char state = 0;

    switch (state) {

        case 0:
            if (wr_flag) {
                EEADR  = wr_addr;
                EEDATA = wr_dada;
                EECON1bits.EEPGD = 0;   // data EEPROM
                EECON1bits.CFGS  = 0;
                EECON1bits.WREN  = 1;   // habilita escriptura

                // Sequencia obligatoria 55h/AAh, amb interrupcions OFF
                di();
                EECON2 = 0x55;
                EECON2 = 0xAA;
                EECON1bits.WR = 1;      // inicia el cicle (self-timed)
                ei();

                state = 1;
            }
            break;

        // Espera cooperativa: el hardware neteja WR en acabar (~4ms)
        case 1:
            if (EECON1bits.WR == 0) {
                EECON1bits.WREN = 0;    // deshabilita escriptura
                PIR2bits.EEIF   = 0;    // neteja flag (per si s'usa)
                wr_flag = 0;
                state = 0;
            }
            break;
    }
}

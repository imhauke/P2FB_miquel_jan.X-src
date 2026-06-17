// ============================================================
//  PROVA AILLADA DEL CANAL SERIAL_TIME (sioAux)
//
//  TEST D'ECO PUR: tot el que reps pel terminal, ho retornes.
//  Si escrius una tecla i la veus a la terminal -> RX i TX funcionen.
//  (Aixo aïlla el sioAux del validador per diagnosticar.)
//
//  Pins (tad_sio_manual.h): TX = RC0, RX = RC1  (UART software)
//  Velocitat: TEMPS_BIT=16 tics x 0.5ms = 8ms/bit -> 125 baud
//
//  IMPORTANT: nomes un main al build. Exclou main.c i altres main_test.
// ============================================================

#include <xc.h>
#include "timer.h"
#include "tad_sio_manual.h"

#pragma config OSC = HSPLL
#pragma config PBADEN = DIG
#pragma config WDT = OFF
#pragma config MCLRE = ON
#pragma config DEBUG = OFF
#pragma config PWRT = OFF
#pragma config BOR = OFF
#pragma config LVP = OFF

static char lletra;
static char te_lletra;   // 1 = hi ha un byte pendent de reenviar

void __interrupt() RSI_HIGH(void) {
    if (INTCONbits.TMR0IF == 1) {
        RSI_Timer0();
    }
}

void main(void) {
    ADCON1 = 0x0F;          // tots els pins AN digitals

    TI_Init();
    ei();                   // interrupcions ON (el sioAux usa el timer)

    initSioAux();
    te_lletra = 0;

    while (1) {
        motorSioAux();      // sempre actiu: progressa RX i TX

        // Si arriba un byte, el guardem per reenviar (eco)
        if (SIO_AUX_byteAvailable()) {
            lletra = SIO_AUX_byteGet();
            te_lletra = 1;
        }

        // Quan el TX estigui lliure i hi hagi byte pendent, el reenviem
        if (te_lletra && SIO_AUX_txAvailable()) {
            SIO_AUX_sendByte(lletra);
            te_lletra = 0;
        }
    }
}

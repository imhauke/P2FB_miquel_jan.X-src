#include <xc.h>
#include "tad_sio.h"


static void PosaRX(char);
static char TreuRX(void);
static void PosaTX(char*);
static char* TreuTX(void);


void SiInit(void){
    TRIS_SIO_TX = 1;   // TX com a entrada (la controla l'EUSART)
    TRIS_SIO_RX = 1;   // RX com a entrada
    
    TXSTAbits.SYNC = 0;     // Modo as�ncrono
    TXSTAbits.BRGH = 1;     // High Speed
    BAUDCONbits.BRG16 = 1;  // Baud Rate Generator 16 bits
    
    SPBRGH = 0x04;
    SPBRG  = 0x10;           // 9600 baudios @ 40MHz
    
    RCSTAbits.SPEN = 1;     // Habilitar puerto serial
    TXSTAbits.TXEN = 1;     // Habilitar transmisi�n
    TXSTAbits.SENDB = 0;
    RCSTAbits.CREN = 1;     // Habilitar recepci�n continua
    RCSTAbits.RX9 = 0;
}

int SiCharAvail(void){
// Pre: retorna el nombre de car?cters rebuts que no s'han recollit
// amb la funci? GetChar encara
    return PIR1bits.RCIF;
}

char SiGetChar(void){
// Pre: SiCharAvail() > 0
// Post: Treu i retorna el primer car?cter de la cua de recepci?
    return RCREG;
}

void SiSendChar(char c){
// Post: espera que el car?cter anterior s'hagi enviat i envia aquest
    while(TXSTAbits.TRMT == 0);
    TXREG = c;
    TXSTAbits.TXEN = 1;
}

char SiAvail(void){
    return TXSTAbits.TRMT;
}
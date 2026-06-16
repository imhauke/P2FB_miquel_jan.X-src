// ============================================================
//  PROVA AILLADA DEL LCD
//  Escriu "Hola Mundo" a la primera linia del LCD.
//
//  Connexions del driver (tad_lcd.h):
//    Dades D4-D7 -> RD4, RD5, RD6, RD7
//    RS -> RE0,  R/W -> RE1,  E (Enable) -> RE2
//
//  IMPORTANT: nomes pot haver-hi UN main al build alhora.
//  Per provar el LCD: inclou aquest fitxer i exclou main.c.
// ============================================================

#include <xc.h>
#include "timer.h"
#include "tad_manager_lcd.h"

#pragma config OSC = HSPLL
#pragma config PBADEN = DIG     // PORTB digital
#pragma config WDT = OFF
#pragma config MCLRE = ON
#pragma config DEBUG = OFF
#pragma config PWRT = OFF
#pragma config BOR = OFF
#pragma config LVP = OFF

// La frase a mostrar (variable global: LCD_print en guarda el punter)
static char missatge[] = "Hola Mundo";

void __interrupt() RSI_HIGH(void) {
    if (INTCONbits.TMR0IF == 1) {
        RSI_Timer0();
    }
}

void main(void) {
    // --- Pins del LCD en mode DIGITAL (imprescindible abans del LcInit) ---
    // RE0-RE2 son AN5-AN7; si queden analogics, RS/RW/E no arriben al pin
    // i el LCD nomes mostra rectangles. ADCON1 = 0x0F -> tot digital.
    ADCON1 = 0x0F;
    TRISE = 0x00;       // PORT E (RE0-2 = control) com a sortida
    TRISD = 0x00;       // PORT D (RD4-7 = dades) com a sortida

    TI_Init();
    ei();               // interrupcions ON (el LcInit usa delays per timer)

    initManagerLcd();   // inicialitza el LCD (LcInit) i la cua

    LCD_print(missatge);   // demana pintar "Hola Mundo" a la linia 1

    while (1) {
        motorManagerLcd();  // pinta la frase caracter a caracter
    }
}

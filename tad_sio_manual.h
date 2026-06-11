#ifndef _TAD_SIO_MANUAL_H_
#define _TAD_SIO_MANUAL_H_

#include <xc.h>
#include "timer.h"

#define CONFIG_TX TRISCbits.TRISC0
#define CONFIG_RX TRISCbits.TRISC1

#define PIN_TX LATCbits.LATC0
#define PIN_RX PORTCbits.RC1

#define TEMPS_BIT 16
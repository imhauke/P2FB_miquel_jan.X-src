#ifndef TAD_SPEAKER_H
#define TAD_SPEAKER_H

// Pin del speaker
#define SPEAKER_PIN LATAbits.LATA5
#define SPEAKER_TRIS TRISAbits.TRISA5

// Frecuencias (en tics de 0.5ms)
#define FREQ_AGUT 1      // ~1kHz (agudo)
#define FREQ_GRAVE 4      // ~250Hz (grave)
#define PERIODO_2 400 // 200ms ON/OFF para sonido periódico
#define DURACION_3 20000  // 10 segundos (10000ms / 0.5ms)

void SPEAKER_startAgut(void);
void SPEAKER_startGreu(void);
void SPEAKER_startAlarma(void);
void SPEAKER_stop(void);

void SPEAKER_init(void);
void SPEAKER_motor(void);

#endif
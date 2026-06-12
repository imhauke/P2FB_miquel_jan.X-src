#include "tad_hora.h"

static char dia, mes, hora, minut, segon;
static char running;
static unsigned char t;

// Dies de cada mes (index 1..12; index 0 no s'usa)
static const char DIES_MES[13] = {
    0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

void initHora(void) {
    running = 0;
    dia = 1; mes = 1; hora = 0; minut = 0; segon = 0;
    TI_NewTimer(&t);
}

void HORA_set(char d, char mo, char h, char mi, char s) {
    dia = d; mes = mo; hora = h; minut = mi; segon = s;
    running = 1;
    TI_ResetTics(t);
}

char HORA_running(void)  { return running; }
char HORA_getDia(void)   { return dia; }
char HORA_getMes(void)   { return mes; }
char HORA_getHora(void)  { return hora; }
char HORA_getMinut(void) { return minut; }
char HORA_getSegon(void) { return segon; }

void motorHora(void) {
    if (!running) return;

    if (TI_GetTics(t) >= HORA_SEGON) {
        TI_ResetTics(t);

        segon++;
        if (segon >= 60) {
            segon = 0;
            minut++;
            if (minut >= 60) {
                minut = 0;
                hora++;
                if (hora >= 24) {
                    hora = 0;
                    dia++;
                    if (dia > DIES_MES[mes]) {
                        dia = 1;
                        mes++;
                        if (mes > 12) {
                            mes = 1;
                        }
                    }
                }
            }
        }
    }
}

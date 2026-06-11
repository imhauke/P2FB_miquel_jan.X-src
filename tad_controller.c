#include "tad_controller.h"

#define MAX_NOM    16
#define MAX_BUFFER  3

// --- Variables privades ---
static char nom_granja[32];
static char buffer[MAX_BUFFER];
static char c;
static char i, j, k;
static char flag_init, flag_comptatge;

static char           temps_animal[NUM_ESPECIES];
static char           temps_decimal;
static char           count_temps_animal[NUM_ESPECIES];
static char           count_temps_producte[NUM_ESPECIES];
static const char     temps_producte[NUM_ESPECIES] = {
    TEMPS_PROD_VACA, TEMPS_PROD_CAVALL, TEMPS_PROD_PORC, TEMPS_PROD_GALLINA
};

static Animal animals_arr[MAX_ANIMALS_TOTAL];
static char   animals[NUM_ESPECIES];
static char   animals_desperts[NUM_ESPECIES];
static char   q_animals;
static char   productes[NUM_ESPECIES];
static char   count_son;

static unsigned char t;

// --- Init ---
void initController(void) {
    flag_init      = 0;
    flag_comptatge = 0;
    i = 0; j = 0; k = 0;
    q_animals = 0;
    count_son = 0;

    animals[0] = 0;          animals[1] = 0;
    animals[2] = 0;          animals[3] = 0;
    animals_desperts[0] = 0; animals_desperts[1] = 0;
    animals_desperts[2] = 0; animals_desperts[3] = 0;
    count_temps_animal[0] = 0;  count_temps_animal[1] = 0;
    count_temps_animal[2] = 0;  count_temps_animal[3] = 0;
    count_temps_producte[0] = 0; count_temps_producte[1] = 0;
    count_temps_producte[2] = 0; count_temps_producte[3] = 0;
    productes[0] = 0; productes[1] = 0;
    productes[2] = 0; productes[3] = 0;

    animals_arr[0].tipus=0;  animals_arr[1].tipus=0;  animals_arr[2].tipus=0;
    animals_arr[3].tipus=0;  animals_arr[4].tipus=0;  animals_arr[5].tipus=0;
    animals_arr[6].tipus=0;  animals_arr[7].tipus=0;  animals_arr[8].tipus=0;
    animals_arr[9].tipus=0;  animals_arr[10].tipus=0; animals_arr[11].tipus=0;
    animals_arr[12].tipus=0; animals_arr[13].tipus=0; animals_arr[14].tipus=0;
    animals_arr[15].tipus=0; animals_arr[16].tipus=0; animals_arr[17].tipus=0;
    animals_arr[18].tipus=0; animals_arr[19].tipus=0; animals_arr[20].tipus=0;
    animals_arr[21].tipus=0; animals_arr[22].tipus=0; animals_arr[23].tipus=0;

    TI_NewTimer(&t);
}

// --- Auxiliars parser INITIALIZE ---
void guardaCaracterTemps(void) {
    if (c != '\r') { buffer[i] = c; i++; }
}

void ferItoaTemps(void) {
    char sumand = buffer[0] - '0';
    if (i == 1) {
        temps_decimal = sumand;
    } else {
        temps_decimal = sumand + sumand + sumand + sumand + sumand +
                        sumand + sumand + sumand + sumand + sumand +
                        (buffer[1] - '0');
    }
}

void guardaTempsAnimal(void) {
    temps_animal[j] = temps_decimal;
}

// --- Afegeix un animal d'especie esp al slot k (un pas, sense bucle) ---
static char afegirNouAnimal(char esp) {
    if (q_animals >= MAX_ANIMALS_TOTAL) return 0;
    if (animals_arr[k].tipus != 0)      return 0; // slot ocupat
    animals_arr[k].tipus   = esp + 1;
    animals_arr[k].despert = 1;
    animals[esp]++;
    animals_desperts[esp]++;
    q_animals++;
    return 1;
}

// --- Posa l'animal k en son critic si escau (un pas, sense bucle) ---
static char posarAnimalsASon(void) {
    if (k >= MAX_ANIMALS_TOTAL) return 0;
    if (animals_arr[k].tipus != 0 && animals_arr[k].despert == 1) {
        animals_arr[k].despert = 0;
        animals_desperts[animals_arr[k].tipus - 1]--;
    }
    k++;
    return 1;
}

// --- Incrementa productes de l'especie i ---
static void incrementarProductes(char esp) {
    productes[esp] += animals_desperts[esp];
}

// --- Motor principal ---
void motorController(void) {
    static char state = 0;

    switch (state) {

        // Espera inicialitzacio completa (Java + hora serial)
        case 0:
            if (flag_init == 1 && V_isFlagOk()) {
                TI_ResetTics(t);
                i = 0;
                state = 7;
                break;
            }
            if (SiCharAvail()) {
                c = SiGetChar();
                state = 1;
                break;
            }
            if (flag_comptatge == 1 && TI_GetTics(t) >= SEGON) {
                TI_ResetTics(t);
                i = 0;
                state = 9;
            }
            break;

        // Identifica comandament entrant
        case 1:
            if (c == CMD_INITIALIZE) { i = 0; j = 0; state = 2; }
            break;

        // Llegeix nom granja fins '$'
        case 2:
            if (SiCharAvail()) { c = SiGetChar(); state = 3; }
            break;

        case 3:
            if (c != '$') {
                if (i < MAX_NOM) { nom_granja[i] = c; i++; }
                state = 2;
            } else {
                nom_granja[i] = '\0';
                i = 0;
                state = 4;
            }
            break;

        // Llegeix 4 temps de generacio
        case 4:
            if (j < NUM_ESPECIES) { state = 5; }
            else { flag_init = 1; state = 0; }
            break;

        case 5:
            if (SiCharAvail()) { c = SiGetChar(); state = 6; }
            break;

        case 6:
            if (c == '\r') {
                state = 5;
            } else if (c != '$' && c != '\n') {
                if (i < MAX_BUFFER - 1) { buffer[i] = c; i++; }
                state = 5;
            } else {
                buffer[i] = '\0';
                ferItoaTemps();
                guardaTempsAnimal();
                j++; i = 0;
                state = 4;
            }
            break;

        // Copia data al LCD
        case 7:
            if (i < 5) {
                nom_granja[i + 16] = V_getData(i);
                i++;
            } else {
                i = 0;
                state = 8;
            }
            break;

        // Imprimeix nom_granja al LCD caracter a caracter
        case 8:
            if (i < 32) {
                LcPutChar(nom_granja[i]);
                i++;
            } else {
                flag_comptatge = 1;
                state = 0;
            }
            break;

        // --- Cada segon: generacio animals, especie i ---
        case 9:
            if (i < NUM_ESPECIES) {
                count_temps_animal[i]++;
                state = 10;
            } else {
                i = 0;
                state = 11;
            }
            break;

        case 10:
            if (count_temps_animal[i] >= temps_animal[i]) {
                count_temps_animal[i] = 0;
                if (q_animals < MAX_ANIMALS_TOTAL) {
                    k = 0;
                    state = 20;
                    break;
                }
            }
            i++;
            state = 9;
            break;

        // Busca slot lliure per afegir animal d'especie i, un pas per crida
        case 20:
            if (afegirNouAnimal(i)) {
                i++;
                state = 9;
            } else if (k >= MAX_ANIMALS_TOTAL) {
                i++;
                state = 9;
            } else {
                k++;
            }
            break;

        // --- Cada segon: produccio, especie i ---
        case 11:
            if (i < NUM_ESPECIES) {
                count_temps_producte[i]++;
                state = 12;
            } else {
                i = 0;
                state = 13;
            }
            break;

        case 12:
            if (count_temps_producte[i] >= temps_producte[i]) {
                count_temps_producte[i] = 0;
                if (animals_desperts[i] > 0) {
                    incrementarProductes(i);
                }
            }
            i++;
            state = 11;
            break;

        // --- Son critic global cada 2 minuts ---
        case 13:
            count_son++;
            if (count_son >= TEMPS_SON) {
                count_son = 0;
                k = 0;
                state = 14;
            } else {
                state = 0;
            }
            break;

        // Posa animals a son critic, un per crida
        case 14:
            if (posarAnimalsASon()) {
                // continua fins k >= MAX_ANIMALS_TOTAL
            } else {
                state = 0;
            }
            break;
    }
}

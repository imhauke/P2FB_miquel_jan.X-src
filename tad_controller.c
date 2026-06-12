#include "tad_controller.h"

#define MAX_NOM    16
#define MAX_BUFFER  3

// --- Variables privades ---
static char linia1[17];   // nom granja + '\0'  (16 chars + terminador)
static char linia2[17];   // " DD/MM/2026" o el que vulguis + '\0'
static char buffer[MAX_BUFFER];
static char c;
static char i, j, k;
static char flag_init, flag_comptatge;

static char temps_animal[NUM_ESPECIES];
static char temps_decimal;
static char count_temps_animal[NUM_ESPECIES];
static char count_temps_producte[NUM_ESPECIES];
static const char temps_producte[NUM_ESPECIES] = {
    TEMPS_PROD_VACA, TEMPS_PROD_CAVALL, TEMPS_PROD_PORC, TEMPS_PROD_GALLINA
};

static Animal animals_arr[MAX_ANIMALS_TOTAL];
static char animals[NUM_ESPECIES];
static char animals_desperts[NUM_ESPECIES];
static char q_animals;
static char productes[NUM_ESPECIES];
static char flag_rebellio;

static char resp[24];     // buffer per construir respostes a Java

static unsigned char t;

// --- Init ---
void initController(void) {
    flag_init = 0;
    flag_comptatge = 0;
    flag_rebellio = 0;
    i = 0; j = 0; k = 0;
    q_animals = 0;

    animals[0] = 0; animals[1] = 0;
    animals[2] = 0; animals[3] = 0;
    animals_desperts[0] = 0;    animals_desperts[1] = 0;
    animals_desperts[2] = 0;    animals_desperts[3] = 0;
    count_temps_animal[0] = 0;  count_temps_animal[1] = 0;
    count_temps_animal[2] = 0;  count_temps_animal[3] = 0;
    count_temps_producte[0] = 0;    count_temps_producte[1] = 0;
    count_temps_producte[2] = 0;    count_temps_producte[3] = 0;
    productes[0] = 0;   productes[1] = 0;
    productes[2] = 0;   productes[3] = 0;

    animals_arr[0].tipus=0; animals_arr[1].tipus=0; animals_arr[2].tipus=0;
    animals_arr[3].tipus=0; animals_arr[4].tipus=0; animals_arr[5].tipus=0;
    animals_arr[6].tipus=0; animals_arr[7].tipus=0; animals_arr[8].tipus=0;
    animals_arr[9].tipus=0; animals_arr[10].tipus=0;    animals_arr[11].tipus=0;
    animals_arr[12].tipus=0;    animals_arr[13].tipus=0;    animals_arr[14].tipus=0;
    animals_arr[15].tipus=0;    animals_arr[16].tipus=0;    animals_arr[17].tipus=0;
    animals_arr[18].tipus=0;    animals_arr[19].tipus=0;    animals_arr[20].tipus=0;
    animals_arr[21].tipus=0;    animals_arr[22].tipus=0;    animals_arr[23].tipus=0;
    
    
    /*for (i = 0; i < NUM_ESPECIES; i++) {
        animals[i] = 0;
        animals_desperts[i] = 0;
        count_temps_animal[i] = 0;
        count_temps_producte[i] = 0;        //Solo mejora un 1%
        productes[i] = 0;
    }

    for (i = 0; i < MAX_ANIMALS_TOTAL; i++) {
        animals_arr[i].tipus = 0;
    }*/

    TI_NewTimer(&t);
}

// --- Descompon un numero 0..99 en desenes/unitats (sense divisions) ---
static char desenes(char n) {
    char d = 0;
    if (n >= 90) return 9;
    if (n >= 80) return 8;
    if (n >= 70) return 7;
    if (n >= 60) return 6;
    if (n >= 50) return 5;
    if (n >= 40) return 4;
    if (n >= 30) return 3;
    if (n >= 20) return 2;
    if (n >= 10) return 1;
    return d;
}

static char unitats(char n) {
    if (n >= 90) return n - 90;
    if (n >= 80) return n - 80;
    if (n >= 70) return n - 70;
    if (n >= 60) return n - 60;
    if (n >= 50) return n - 50;
    if (n >= 40) return n - 40;
    if (n >= 30) return n - 30;
    if (n >= 20) return n - 20;
    if (n >= 10) return n - 10;
    return n;
}

// --- Escriu un numero (0..255) a resp[p..]; retorna la nova posicio ---
static char posaNum(char p, char n) {
    char cent = 0;
    if (n >= 200) { cent = 2; n -= 200; }
    else if (n >= 100) { cent = 1; n -= 100; }
    if (cent > 0) {
        resp[p++] = '0' + cent;
        resp[p++] = '0' + desenes(n);
    } else if (n >= 10) {
        resp[p++] = '0' + desenes(n);
    }
    resp[p++] = '0' + unitats(n);
    return p;
}

// --- Auxiliars parser INITIALIZE ---
void guardaCaracterTemps(void) {
    if (c != '\r') {
        buffer[i] = c;
        i++;
    }
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
    if (q_animals >= MAX_ANIMALS_TOTAL) {
        return 0;
    }

    if (animals_arr[k].tipus != 0) {   // slot ocupat
        return 0;
    }

    animals_arr[k].tipus = esp + 1;
    animals_arr[k].despert = 1;
    animals_arr[k].count_son = 0;      // comença a comptar els seus 2 min

    animals[esp]++;
    animals_desperts[esp]++;
    q_animals++;

    // Notifica al LCD: "Nou Animal <Tipus>: <num>"
    LCD_notifica(NOTIF_ANIMAL, esp, animals[esp]);

    return 1;
}

// --- Compta el segon de l'animal k; si arriba a 2 min, son critic (un pas) ---
static char posarAnimalsASon(void) {
    if (k >= MAX_ANIMALS_TOTAL) {
        return 0;
    }
    if (animals_arr[k].tipus != 0 && animals_arr[k].despert == 1) {
        animals_arr[k].count_son++;
        if (animals_arr[k].count_son >= TEMPS_SON) {
            animals_arr[k].count_son = 0;
            animals_arr[k].despert = 0;
            animals_desperts[animals_arr[k].tipus - 1]--;
        }
    }
    k++;
    return 1;
}

// --- Incrementa productes de l'especie i ---
static void incrementarProductes(char esp) {
    productes[esp] += animals_desperts[esp];
    // Notifica al LCD: "Nou Producte <Tipus>: <total>"
    LCD_notifica(NOTIF_PRODUCTE, esp, productes[esp]);
}

// --- Construeix "P llet$pernil$ous$pinzells" a resp[] (resposta GET_PRODUCTS) ---
// productes[]: [0]=llet [1]=pinzells [2]=pernil [3]=ous
static void construeixProductes(void) {
    char p = 0;
    resp[p++] = RSP_DATA_PRODUCTS;
    p = posaNum(p, productes[0]);   // llet
    resp[p++] = '$';
    p = posaNum(p, productes[2]);   // pernil
    resp[p++] = '$';
    p = posaNum(p, productes[3]);   // ous
    resp[p++] = '$';
    p = posaNum(p, productes[1]);   // pinzells
    resp[p++] = '\r';
    resp[p++] = '\n';
    resp[p]   = '\0';
}

// --- RESET: buida la granja i torna a l'espera d'inicialitzacio ---
static void ferReset(void) {
    initController();   // reinicialitza tots els comptadors i arrays
    // TODO: esborrar EEPROM
}

// --- CONSUME: descompta productes segons l'opcio escollida ---
// Opcions (segons enunciat):
//  0 = ou fregit          -> 1 ou
//  1 = truita amb pernil  -> 1 ou + 1 pernil
//  2 = cacaolat           -> 2 llets
//  3 = pintura            -> 2 pinzells
// productes[]: [0]=llet [1]=pinzells [2]=pernil [3]=ous
static void ferConsume(char opcio) {
    if (opcio == '0') {
        if (productes[3] >= 1) productes[3] -= 1;
    } else if (opcio == '1') {
        if (productes[3] >= 1 && productes[2] >= 1) {
            productes[3] -= 1;
            productes[2] -= 1;
        }
    } else if (opcio == '2') {
        if (productes[0] >= 2) productes[0] -= 2;
    } else if (opcio == '3') {
        if (productes[1] >= 2) productes[1] -= 2;
    }
}

// --- Motor principal ---
void motorController(void) {
    static char state = 0;

    switch (state) {

        // Espera inicialitzacio completa (Java + hora serial)
        case 0:
            if (flag_init == 1 && V_isFlagOk() && flag_comptatge == 0) {
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
            if (c == CMD_INITIALIZE) {
                i = 0; j = 0;
                state = 2;
            } else if (c == CMD_RESET) {
                ferReset();
                state = 0;
            } else if (c == CMD_START_REBELLION) {
                flag_rebellio = 1;
                state = 0;
            } else if (c == CMD_STOP_REBELLION) {
                flag_rebellio = 0;
                state = 0;
            } else if (c == CMD_GET_PRODUCTS) {
                state = 30;     // envia DATA_PRODUCTS
            } else if (c == CMD_GET_ANIMALS) {
                k = 0;
                state = 40;     // envia llista d'animals + FINISH
            } else if (c == CMD_CONSUME) {
                state = 50;     // llegeix l'opcio
            } else if (c == CMD_SLEEP) {
                state = 60;     // llegeix TIPUS$NUM
            } else {
                state = 0;      // comanda desconeguda, ignora
            }
            break;

        // Llegeix nom granja fins '$'
        case 2:
            if (SiCharAvail()) { 
                c = SiGetChar(); 
                state = 3; 
            }
            break;
        
        case 3:
            if (c != '$') {
                if (i < MAX_NOM) { 
                    linia1[i] = c;
                    i++; 
                }
                state = 2;
            } else {
                linia1[i] = '\0';
                i = 0;
                state = 4;
            }
            break;
        // Llegeix 4 temps de generacio
        case 4:
            if (j < NUM_ESPECIES) { 
                state = 5; 
            }
            else { 
                flag_init = 1; 
                state = 0; 
            }
            break;

        case 5:
            if (SiCharAvail()) { 
                c = SiGetChar(); 
                state = 6; 
            }
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

        // Construeix linia 2 amb la data (font unica: TAD Hora) i dispara prints
        case 7:
            // Linia 2: " DD/MM/2026" (11 chars). Llegim del TAD Hora.
            linia2[0]  = ' ';
            linia2[1]  = '0' + desenes(HORA_getDia());
            linia2[2]  = '0' + unitats(HORA_getDia());
            linia2[3]  = '/';
            linia2[4]  = '0' + desenes(HORA_getMes());
            linia2[5]  = '0' + unitats(HORA_getMes());
            linia2[6]  = '/';
            linia2[7]  = '2';
            linia2[8]  = '0';
            linia2[9]  = '2';
            linia2[10] = '6';
            linia2[11] = '\0';

            LCD_print(linia1);
            LCD_print2(linia2);
            state = 8;
            break;
        // Espera que el manager hagi acabat de pintar les dues linies
        case 8:
            if (LCD_flag1() == 0 && LCD_flag2() == 0) {
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
                k = 0;
                state = 20;
            } else {
                i++;
                state = 9;
            }
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

        // --- Cada segon: compta el son critic individual de cada animal ---
        case 13:
            k = 0;
            state = 14;
            break;

        // Compta el segon de cada animal; dorm els que arribin a 2 min
        case 14:
            if (posarAnimalsASon()) {
                // continua fins k >= MAX_ANIMALS_TOTAL
            } else {
                state = 0;
            }
            break;

        // === GET_PRODUCTS: envia "P llet$pernil$ous$pinzells\r\n" ===
        case 30:
            if (statusFiMissatge()) {
                construeixProductes();
                enviaMissatge(resp);
                state = 0;
            }
            break;

        // === CONSUME: llegeix l'opcio i descompta ===
        case 50:
            if (SiCharAvail()) {
                c = SiGetChar();
                if (c != '\r' && c != '\n') {
                    ferConsume(c);
                }
                state = 51;
            }
            break;

        // Consumeix la resta fins '\n'
        case 51:
            if (SiCharAvail()) {
                c = SiGetChar();
                if (c == '\n') state = 0;
            }
            break;
    }
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>



#define NUM_CARTAS 52
#define NUM_PALOS   4
#define NUM_VALORES 13

typedef struct {
    int valor;  /* 1-13 */
    int palo;   /* 0=Treboles, 1=Espadas, 2=Corazones, 3=Diamantes */
} Carta;

typedef struct {
    Carta cartas[NUM_CARTAS];
    int tope;  /* siguiente carta a repartir */
} Baraja;

/* CONSTANTES DE NOMBRES*/

const char *PALOS[] = {"Treboles", "Espadas", "Corazones", "Diamantes"};
const char *SIMBOLOS[] = {"C", "E", "H", "D"};
const char *VALORES_STR[] = {"", "A", "2", "3", "4", "5", "6",
                              "7", "8", "9", "10", "J", "Q", "K"};

/* Rangos de mano (mayor = mejor) */
#define CARTA_ALTA 0
#define UN_PAR 1
#define DOS_PARES 2
#define TERCIA 3
#define CORRIDA 4
#define COLOR 5
#define CASA_LLENA 6
#define POKER_MANO 7
#define CORRIDA_COLOR 8
#define CORRIDA_REAL 9

const char *NOMBRES_MANO[] = {
    "Carta Alta", "Un Par", "Dos Pares", "Tercia",
    "Corrida", "Color", "Casa Llena", "Poker",
    "Corrida de Color", "Corrida Real"
};

/* BARAJA */

void inicializar_baraja(Baraja *b) {
    int idx = 0;
    for (int p = 0; p < NUM_PALOS; p++)
        for (int v = 1; v <= NUM_VALORES; v++) {
            b->cartas[idx].palo  = p;
            b->cartas[idx].valor = v;
            idx++;
        }
    b->tope = 0;
}

void barajar(Baraja *b) {
    for (int i = NUM_CARTAS - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        Carta tmp   = b->cartas[i];
        b->cartas[i] = b->cartas[j];
        b->cartas[j] = tmp;
    }
}

Carta repartir(Baraja *b) {
    return b->cartas[b->tope++];
}

/* MOSTRAR CARTA / MANO */

void mostrar_carta(Carta c) {
    printf("[%s%s]", VALORES_STR[c.valor], SIMBOLOS[c.palo]);
}

void mostrar_mano(Carta *cartas, int n) {
    for (int i = 0; i < n; i++) {
        mostrar_carta(cartas[i]);
        if (i < n - 1) printf(" ");
    }
}

/* EVALUACION DE MANOS */

/* Resultado de evaluar 5 cartas */
typedef struct {
    int rango;         /* 0-9 */
    int tiebreak[5];   /* valores para desempate, ordenados desc */
} EvalMano;


int cmp_desc(const void *a, const void *b) {
    return (*(int*)b) - (*(int*)a);
}

EvalMano evaluar_5(Carta *c) {
    EvalMano res;
    int vals[5], freq[14] = {0};
    int es_color, es_corrida, es_corrida_real;

    for (int i = 0; i < 5; i++) {
        vals[i] = c[i].valor;
        freq[c[i].valor]++;
    }

    /* Color: todos el mismo palo */
    es_color = 1;
    for (int i = 1; i < 5; i++)
        if (c[i].palo != c[0].palo) { 
            es_color = 0; 
            break; 
        }

    /* Corrida */
    int sv[5];
    memcpy(sv, vals, sizeof(vals));
    qsort(sv, 5, sizeof(int), cmp_desc); 

    /* Verificar A-2-3-4-5  */
    int es_rueda = (sv[0]==5 && sv[1]==4 && sv[2]==3 && sv[3]==2 && sv[4]==1);
    /* Verificar 10-J-Q-K-A */
    es_corrida_real = (sv[0]==13 && sv[1]==12 && sv[2]==11 && sv[3]==10 && sv[4]==1);
    /* Corrida normal */
    es_corrida = es_corrida_real || es_rueda ||
                 (sv[0]-sv[4]==4 && freq[sv[0]]==1 && freq[sv[1]]==1 &&
                  freq[sv[2]]==1 && freq[sv[3]]==1 && freq[sv[4]]==1);


    int cuatro=-1, tercia=-1, pares[2]={-1,-1}, npares=0, solos[5], nsolos=0;
    for (int v = 13; v >= 1; v--) {
        if (freq[v] == 4) cuatro = v;
        else if (freq[v] == 3) tercia = v;
        else if (freq[v] == 2) { if (npares < 2) pares[npares++] = v; }
        else if (freq[v] == 1) { if (nsolos < 5) solos[nsolos++] = v; }
    }
    /* solos ya en orden desc porque iteramos v de 13 a 1 */

    /* Asignar rango*/

    /* Corrida real */
    if (es_corrida_real && es_color) {
        res.rango = CORRIDA_REAL;
        res.tiebreak[0] = 14; /* As alto */
        return res;
    }
    /* Corrida de color */
    if (es_corrida && es_color) {
        res.rango = CORRIDA_COLOR;
        memcpy(res.tiebreak, sv, sizeof(sv));
        return res;
    }
    /* Poker */
    if (cuatro != -1) {
        res.rango = POKER_MANO;
        res.tiebreak[0] = cuatro;
        res.tiebreak[1] = (nsolos > 0) ? solos[0] : (tercia != -1 ? tercia : pares[0]);
        return res;
    }
    /* Casa llena */
    if (tercia != -1 && npares >= 1) {
        res.rango = CASA_LLENA;
        res.tiebreak[0] = tercia;
        res.tiebreak[1] = pares[0];
        return res;
    }
    /* Color */
    if (es_color) {
        res.rango = COLOR;
        memcpy(res.tiebreak, sv, sizeof(sv));
        return res;
    }
    /* Corrida */
    if (es_corrida) {
        res.rango = CORRIDA;
        if (es_rueda) {
            /* As vale 1 como bajo */
            int rueda[5] = {5,4,3,2,1};
            memcpy(res.tiebreak, rueda, sizeof(rueda));
        } else {
            memcpy(res.tiebreak, sv, sizeof(sv));
        }
        return res;
    }

    /* Tercia */
    if (tercia != -1) {
        res.rango = TERCIA;
        res.tiebreak[0] = tercia;
        res.tiebreak[1] = nsolos > 0 ? solos[0] : 0;
        res.tiebreak[2] = nsolos > 1 ? solos[1] : 0;
        return res;
    }
    /* Dos pares */
    if (npares == 2) {
        res.rango = DOS_PARES;
        /* pares[] ya en orden desc */
        res.tiebreak[0] = pares[0];
        res.tiebreak[1] = pares[1];
        res.tiebreak[2] = nsolos > 0 ? solos[0] : 0;
        return res;
    }
    /* Un par */
    if (npares == 1) {
        res.rango = UN_PAR;
        res.tiebreak[0] = pares[0];
        res.tiebreak[1] = nsolos > 0 ? solos[0] : 0;
        res.tiebreak[2] = nsolos > 1 ? solos[1] : 0;
        res.tiebreak[3] = nsolos > 2 ? solos[2] : 0;
        return res;
    }
    /* Carta alta */
    res.rango = CARTA_ALTA;
    memcpy(res.tiebreak, sv, sizeof(sv));
    return res;
}

/* Compara dos EvalMano: retorna 1 si a>b, -1 si a<b, 0 si empate */
int comparar_eval(EvalMano a, EvalMano b) {
    if (a.rango != b.rango)
        return (a.rango > b.rango) ? 1 : -1;
    for (int i = 0; i < 5; i++) {
        if (a.tiebreak[i] != b.tiebreak[i])
            return (a.tiebreak[i] > b.tiebreak[i]) ? 1 : -1;
    }
    return 0;
}

/* MEJOR MANO DE 7 CARTAS (combinaciones C(7,5)=21)*/

EvalMano mejor_mano_7(Carta *c7) {
    int combos[21][5] = {
        {0,1,2,3,4},{0,1,2,3,5},{0,1,2,3,6},{0,1,2,4,5},{0,1,2,4,6},
        {0,1,2,5,6},{0,1,3,4,5},{0,1,3,4,6},{0,1,3,5,6},{0,1,4,5,6},
        {0,2,3,4,5},{0,2,3,4,6},{0,2,3,5,6},{0,2,4,5,6},{0,3,4,5,6},
        {1,2,3,4,5},{1,2,3,4,6},{1,2,3,5,6},{1,2,4,5,6},{1,3,4,5,6},
        {2,3,4,5,6}
    };
    EvalMano mejor;
    mejor.rango = -1;
    for (int i = 0; i < 21; i++) {
        Carta mano5[5];
        for (int j = 0; j < 5; j++)
            mano5[j] = c7[combos[i][j]];
        EvalMano ev = evaluar_5(mano5);
        if (comparar_eval(ev, mejor) > 0)
            mejor = ev;
    }
    return mejor;
}

/* UTILIDADES DE ENTRADA*/

int pedir_si_no(const char *msg) {
    char buf[10];
    while (1) {
        printf("%s (s/n): ", msg);
        fflush(stdout);
        if (fgets(buf, sizeof(buf), stdin)) {
            if (buf[0] == 's' || buf[0] == 'S') return 1;
            if (buf[0] == 'n' || buf[0] == 'N') return 0;
        }
        printf("  Ingresa 's' o 'n'.\n");
    }
}

void separador() {
    printf("\n%s\n", "=======================================================");
}

/* LOGICA DEL JUEGO */

void jugar() {
    Baraja baraja;
    Carta  manos[2][2];   /* 2 cartas por jugador */
    Carta  river[5];      /* cartas del dealer */
    int    river_n = 0;
    int    en_juego[2] = {1, 1};

    inicializar_baraja(&baraja);
    barajar(&baraja);

    /* Repartir 2 cartas a cada jugador */
    for (int i = 0; i < 2; i++) {
        manos[i][0] = repartir(&baraja);
        manos[i][1] = repartir(&baraja);
    }

    /* ── PRIMERA RONDA ── */
    separador();
    printf("  PRIMERA RONDA — Cartas privadas\n");
    printf("-------------------------------------------------------\n");

    for (int i = 0; i < 2; i++) {
        printf("\n  Jugador %d — Tus cartas: ", i + 1);
        mostrar_mano(manos[i], 2);
        printf("\n");
        char msg[64];
        sprintf(msg, "  Jugador %d, deseas continuar en el juego?", i + 1);
        if (!pedir_si_no(msg)) {
            en_juego[i] = 0;
            printf("  Jugador %d se retira.\n", i + 1);
        }
    }

    /* Verificar retiros */
    if (!en_juego[0] && !en_juego[1]) {
        printf("\n  Ambos jugadores se retiraron. Sin ganador.\n");
        return;
    }
    if (!en_juego[0]) { 
        printf("\n  GANA Jugador 2 (Jugador 1 se retiro)\n"); 
        return; 
    }
    if (!en_juego[1]) { 
        printf("\n  GANA Jugador 1 (Jugador 2 se retiro)\n"); 
        return; 
    }

    /* ── SEGUNDA RONDA — (3 cartas) ── */
    separador();
    printf("  SEGUNDA RONDA — El Flop\n");
    printf("-------------------------------------------------------\n");

    for (int i = 0; i < 3; i++)
        river[river_n++] = repartir(&baraja);

    printf("  Cartas del dealer: ");
    mostrar_mano(river, river_n);
    printf("\n");

    for (int i = 0; i < 2; i++) {
        char msg[64];
        sprintf(msg, "  Jugador %d, deseas continuar?", i + 1);
        if (!pedir_si_no(msg)) {
            en_juego[i] = 0;
            printf("  Jugador %d se retira.\n", i + 1);
        }
    }

    if (!en_juego[0] && !en_juego[1]) { 
        printf("\n  Sin ganador.\n"); 
        return; 
    }
    if (!en_juego[0]) { 
        printf("\n  GANA Jugador 2 (Jugador 1 se retiro)\n"); 
        return; 
    }
    if (!en_juego[1]) { 
        printf("\n  GANA Jugador 1 (Jugador 2 se retiro)\n"); 
        return; 
    }

    /* ── TERCERA RONDA — TURN (4ta carta) ── */
    separador();
    printf("  TERCERA RONDA — El Turn\n");
    printf("-------------------------------------------------------\n");

    river[river_n++] = repartir(&baraja);

    printf("  Cartas del dealer: ");
    mostrar_mano(river, river_n);
    printf("\n");

    for (int i = 0; i < 2; i++) {
        char msg[64];
        sprintf(msg, "  Jugador %d, deseas continuar?", i + 1);
        if (!pedir_si_no(msg)) {
            en_juego[i] = 0;
            printf("  Jugador %d se retira.\n", i + 1);
        }
    }

    if (!en_juego[0] && !en_juego[1]) { printf("\n  Sin ganador.\n"); return; }
    if (!en_juego[0]) { printf("\n  GANA Jugador 2 (Jugador 1 se retiro)\n"); return; }
    if (!en_juego[1]) { printf("\n  GANA Jugador 1 (Jugador 2 se retiro)\n"); return; }

    /* ── CUARTA RONDA — RIVER (5ta carta) ── */
    separador();
    printf("  CUARTA RONDA — El River\n");
    printf("-------------------------------------------------------\n");

    river[river_n++] = repartir(&baraja);

    printf("  Cartas del dealer: ");
    mostrar_mano(river, river_n);
    printf("\n\n");

    /* Armar 7 cartas por jugador y evaluar */
    for (int i = 0; i < 2; i++) {
        Carta c7[7];
        c7[0] = manos[i][0];
        c7[1] = manos[i][1];
        for (int j = 0; j < 5; j++)
            c7[2 + j] = river[j];

        EvalMano ev = mejor_mano_7(c7);
        printf("  Jugador %d: ", i + 1);
        mostrar_mano(manos[i], 2);
        printf("  =>  %s\n", NOMBRES_MANO[ev.rango]);
    }

    /* Determinar ganador */
    Carta c7_1[7], c7_2[7];
    c7_1[0] = manos[0][0]; c7_1[1] = manos[0][1];
    c7_2[0] = manos[1][0]; c7_2[1] = manos[1][1];
    for (int j = 0; j < 5; j++) {
        c7_1[2 + j] = river[j];
        c7_2[2 + j] = river[j];
    }

    EvalMano ev1 = mejor_mano_7(c7_1);
    EvalMano ev2 = mejor_mano_7(c7_2);
    int cmp = comparar_eval(ev1, ev2);

    printf("\n");
    if      (cmp > 0) printf("  >>> GANA JUGADOR 1 con %s <<<\n", NOMBRES_MANO[ev1.rango]);
    else if (cmp < 0) printf("  >>> GANA JUGADOR 2 con %s <<<\n", NOMBRES_MANO[ev2.rango]);
    else              printf("  >>> EMPATE <<<\n");
}

/* MAIN */

int main(void) {
    srand((unsigned)time(NULL));

    printf("       SIMULADOR DE POKER - TEXAS HOLD'EM\n");

    do {
        jugar();
        separador();
    } while (pedir_si_no("Jugar otra partida?"));

    printf("\n  Fin!\n");
    return 0;
}

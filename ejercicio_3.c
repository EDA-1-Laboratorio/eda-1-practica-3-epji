#include <stdio.h>
#include <math.h>

typedef struct {
    float x;
    float y;
    float radio;
} Circulo;

float calcular_distancia(Circulo c1, Circulo c2);

int main() {
    Circulo c1, c2;

    // --- Círculo 1 ---
    printf("--- DATOS DEL CIRCULO 1 ---\n");
    printf("Ingrese X: ");
    scanf("%f", &c1.x);
    printf("Ingrese Y: ");
    scanf("%f", &c1.y);
    printf("Ingrese Radio: ");
    scanf("%f", &c1.radio);

   
    while (getchar() != '\n'); 

    // --- Círculo 2 ---
    printf("\n--- DATOS DEL CIRCULO 2 ---\n");
    printf("Ingrese X: ");
    scanf("%f", &c2.x);
    printf("Ingrese Y: ");
    scanf("%f", &c2.y);
    printf("Ingrese Radio: ");
    scanf("%f", &c2.radio);

    if (c1.radio <= 0 || c2.radio <= 0) {
        printf("\nError: Los radios deben ser mayores a cero.\n");
        return 1;
    }

    float d = calcular_distancia(c1, c2);
    float suma_radios = c1.radio + c2.radio;
    float diff_radios = fabsf(c1.radio - c2.radio);

    

    printf("\n--- RESULTADO GEOMETRICO ---\n");
    if (d > suma_radios) {
        printf("Estado: Separados (No hay contacto).\n");
    } else if (d == suma_radios) {
        printf("Estado: Tangentes exteriores (Se tocan en un punto).\n");
    } else if (d < diff_radios) {
        printf("Estado: Uno esta dentro del otro (Sin tocarse).\n");
    } else if (d == diff_radios) {
        printf("Estado: Tangentes interiores (Uno dentro, tocando el borde).\n");
    } else {
        printf("Estado: Interseccion (Se cortan en dos puntos).\n");
    }

    return 0;
}

float calcular_distancia(Circulo c1, Circulo c2) {
 
    return sqrtf(powf(c2.x - c1.x, 2) + powf(c2.y - c1.y, 2));
}

#include <stdio.h>

#define MAX_PIZZAS 10

// Definimos la estructura pizza
struct pizza {
    float radio;        // tamaño de la pizza (radio)
    int toppings;       // cantidad de ingredientes
    float precio;       // precio de la pizza
};

int main() {
    struct pizza orden[MAX_PIZZAS];
    int n;
    float total = 0;

    printf("Cuantas pizzas quiere ordenar? ");
    scanf("%d", &n);

    for(int i = 0; i < n; i++) {
        printf("\nPizza %d\n", i + 1);

        printf("Radio de la pizza en pulgadas (10 o 15): ");
        scanf("%f", &orden[i].radio);

        printf("Cantidad de toppings: ");
        scanf("%d", &orden[i].toppings);

        // Precio base según tamaño
        if(orden[i].radio == 10)
            orden[i].precio = 100;
        else if(orden[i].radio == 15)
            orden[i].precio = 150;

        // Cada topping cuesta 10 pesos
        orden[i].precio += orden[i].toppings * 20;
    }

    // Aplicar promoción 2x1 si hay dos pizzas del mismo tamaño
    for(int i = 0; i < n; i++) {
        if (orden[i].precio > 0){
            for(int j = i + 1; j < n; j++){
                if(orden[i].radio == orden[j].radio && orden[i].precio > 0){
                    orden[j].precio = 0;
                    break;
                }
            }
        }



    printf("\n----- TICKET -----\n");
    for(int i = 0; i < n; i++) {
        printf("Pizza %d: Radio %.2f, Toppings %d, Precio $%.2f\n",
               i + 1, orden[i].radio, orden[i].toppings, orden[i].precio);
        total += orden[i].precio;
    }

    printf("TOTAL A PAGAR: $%.2f\n", total);
    printf("se aplico el descuento de 2x1");

    return 0;
}
}

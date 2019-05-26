#include "loop.h"
#include "globals.h"
#include "moteur.h"
#include <p18f2520.h>

void loop(void) {
    int distance_objet_initial;
    // Mettre des tests ici
    // Tourner(10, 'd');
    // Tourner(25, 'g');
    // Decelerer();
    while (!IS_START);

    Avancer(50);
    /*while(IS_START) {
        if (DISTANCE_OBJET < 153 &&
            DISTANCE_OBJET > 102) { // TODO: Define 1m and 1m50
            distance_objet_initial = DISTANCE_OBJET;
            Calibration();
            while (distance_objet_initial - DISTANCE_OBJET < 102) { // TODO: Define 1m
                Avancer(50);  // TODO: Define 30cm/s
            }
            Decelerer();
            IS_START = 0;
        } else {
            Tourner(20, 'd'); // TODO: Calibrer l'angle de rotation.
        }
    }*/
}

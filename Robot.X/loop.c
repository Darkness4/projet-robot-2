#include "loop.h"
#include "globals.h"
#include "moteur.h"
#include <p18f2520.h>

void loop(void) {
    int distance_objet_initial;
    // Mettre des tests ici
    // Tourner(50, 'd');
    // Tourner(25, 'g');
    Avancer(10);

    while(!IS_START);

    while(IS_START) {
        if (DISTANCE_OBJET < 300 &&
            DISTANCE_OBJET > 100) { // TODO: Define 1m and 1m50
            distance_objet_initial = DISTANCE_OBJET;
            Calibration();
            if (distance_objet_initial - DISTANCE_OBJET > 100) { // TODO: Define 1m
                Decelerer();
                IS_START = 0;
            } else {
                Avancer(50);  // TODO: Define 30cm/s
            }
        } else {
            Tourner(30, 'd'); // TODO: Calibrer l'angle de rotation.
            Decelerer();
        }
    }
}

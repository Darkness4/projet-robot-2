#include "loop.h"
#include "globals.h"
#include "moteur.h"
#include <p18f2520.h>

void loop(void) {
    int distance_objet_initial;
    while (!IS_START)
        continue;

    if (DISTANCE_OBJET < 150 &&
        DISTANCE_OBJET > 100) { // TODO: Define 1m and 1m50
        distance_objet_initial = DISTANCE_OBJET;
        // Calibration();
        if (distance_objet_initial - DISTANCE_OBJET > 100) { // TODO: Define 1m
            DECELERER = 1;
            IS_START = 0;
        } else {
            // Avancer();
        }
    } else {
        Tourner(30, 'd'); // TODO: Speed, stop
        DECELERER = 1;
    }
}

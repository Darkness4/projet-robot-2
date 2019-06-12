#include "loop.h"
#include "globals.h"
#include "moteur.h"
#include <p18f2520.h>

// 6.7s = 1 tour
// delay 83 = 1m

void loop(void) {
    int distance_objet_initial;
    // Mettre des tests ici
    while (!IS_START);

    while(IS_START) {
        if (DISTANCE_OBJET < 153 &&
            DISTANCE_OBJET > 102) { // TODO: Define 1m and 1m50
            distance_objet_initial = DISTANCE_OBJET;
            Calibration();
            Avancer(50);
            delay_100ms(66);
            CommandeMoteur(0);
            IS_START = 0;
        } else {
            Tourner(30, 'd'); // TODO: Calibrer l'angle de rotation.
        }
    }
}
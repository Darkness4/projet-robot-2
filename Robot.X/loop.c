#include "loop.h"

#include <p18f2520.h>

#include "globals.h"
#include "moteur.h"


void loop(void) {
    int distance_objet_initial;
    while (ETAT != START);

    while (ETAT != NOT_START) {
        if (DISTANCE_OBJET < 153 &&
            DISTANCE_OBJET > 102) { // Entre 1m and 1m50
            distance_objet_initial = DISTANCE_OBJET;

            ETAT = CALIB;

            Calibration();

            ETAT = RUN;

            Avancer(50);  // 30 cm/s
            delay_100ms(66);  // Environ 1m
            CommandeMoteur(0);
            ETAT = NOT_START;
        } else
            Tourner(30, 'd');
    }
}
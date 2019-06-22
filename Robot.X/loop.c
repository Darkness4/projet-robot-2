/**
 * @file loop.c
 * @author Marc NGUYEN
 * @author Jonas LEFEVRE-DAUMAL
 * @brief Algorithme principal
 * @version 1.0.0
 * @date 2019-05-15
 *
 */
#include "loop.h"

#include <p18f2520.h>

#include "globals.h"
#include "moteur.h"

void loop(void) {
  while (1) {
    while (ETAT != NOT_START) {
      if (DISTANCE_OBJET < 150 && DISTANCE_OBJET > 100) {  // Entre 1m and 1m50
        ETAT = CALIB;

        Calibration();

        ETAT = RUN;

        Avancer(50);      // 30 cm/s
        delay_100ms(33);  // Environ 1m
        StopMoteur();
        ETAT = NOT_START;
      } else
        Tourner(30, 'd');
    }
  }
}
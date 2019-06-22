/**
 * @file globals.c
 * @author Marc NGUYEN
 * @author Jonas LEFEVRE-DAUMAL
 * @brief Configuration du programme
 * @version 1.0.0
 * @date 2019-05-15
 *
 */
#include "globals.h"

/// Condition de demarrage
volatile enum Etat ETAT = 0;

/// Voltage
volatile unsigned char VOLTAGE = 255;

/// Rapport cyclique
volatile int PERCENT = 0;

/// Distance de l'objet
volatile int DISTANCE_OBJET = 0;

/// Count
volatile long COUNT_100MS = 0;

/// Delay a 100ms via TMR0
void delay_100ms(int duration) {
  int time_origin = COUNT_100MS;
  while (COUNT_100MS - time_origin < duration)
    ;
}
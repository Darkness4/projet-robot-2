/**
 * @file moteur.c
 * @author Marc NGUYEN
 * @author Jonas LEFEVRE-DAUMAL
 * @brief Commandes moteurs
 * @version 1.0.0
 * @date 2019-05-15
 *
 */
#include "moteur.h"

#include <p18f2520.h>

#include "MI2C.h"
#include "globals.h"
#include "init.h"

/// Carte spatiale
static volatile int DISTANCE_OBJET_TABLE[60];
/// Carte temporelle
static volatile int TIME_OBJET_TABLE[60];

/// Controle de vitesse
void CommandeMoteur(int percent) {
  int ccpr_calc;
  // Protection moteur
  if (percent > 50)
    percent = 50;
  else if (percent < 0)
    percent = 0;

  // Calcul CCPR
  ccpr_calc = percent * 10;  // CCPR = percent*Fosc/(100*Prescaler*f_PWM)
                             // 8e6/(100*4*2000) = 10

  CCPR1L = ccpr_calc >> 2;              // CCPR low
  CCP1CONbits.DC1B = ccpr_calc & 0b11;  // CCPR last 2 bits
  CCPR2L = ccpr_calc >> 2;              // CCPR low
  CCP2CONbits.DC2B = ccpr_calc & 0b11;  // CCPR last 2 bits
}

/// Stop les moteurs
void StopMoteur(void) { CommandeMoteur(0); }

/// Tourner avec les moteurs
void Tourner(int percent, char direction) {
  if (direction == 'd') {
    PORTAbits.RA6 = 1;  // DIRD
    PORTAbits.RA7 = 0;  // DIRG
  } else if (direction == 'g') {
    PORTAbits.RA6 = 0;
    PORTAbits.RA7 = 1;
  }

  PERCENT = percent;

  CommandeMoteur(percent);
}

/// Avancer tout droit
void Avancer(int percent) {
  PORTAbits.RA6 = 0;
  PORTAbits.RA7 = 0;

  CommandeMoteur(percent);
}

/// Balayer et s'aligner avec la cible
void Calibration(void) {
  int time = COUNT_100MS;
  int mini_temp = 32767;
  int idx_mini;
  char i = 0;
  char j;

  // Balayage
  Tourner(25, 'd');
  while (COUNT_100MS - time < 30) {
    if (i < 60) {  // Protection overflow
      // Cartographier en 1D
      DISTANCE_OBJET_TABLE[i] = DISTANCE_OBJET;
      TIME_OBJET_TABLE[i] = COUNT_100MS - time;
      i++;

      // Afficher la progression d'enregistrement
      Write_PCF8574(0x40, ~i);
    }
    delay_100ms(1);
  }
  StopMoteur();

  // Recherche de minimum
  for (j = 0; j < i; j++) {
    if (DISTANCE_OBJET_TABLE[j] < mini_temp) {
      mini_temp = DISTANCE_OBJET_TABLE[j];
      idx_mini = j;
    }
  }
  Tourner(25, 'g');

  // Afficher le min
  Write_PCF8574(0x40, ~idx_mini);

  // Angle necessaire pour s'aligner
  delay_100ms(TIME_OBJET_TABLE[i - 1] - TIME_OBJET_TABLE[idx_mini]);
  StopMoteur();
}
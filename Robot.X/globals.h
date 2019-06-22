/**
 * @file globals.h
 * @author Marc NGUYEN
 * @author Jonas LEFEVRE-DAUMAL
 * @brief Configuration du programme
 * @version 1.0.0
 * @date 2019-05-15
 *
 */

#ifndef GLOBALS_H
#define GLOBALS_H

/// Active les tests unitaires (DEBUG_PRINT doit etre desactive)
#define UNIT_TESTS 0
/// Active l'affichage debug
#define DEBUG_PRINT 1
/// Seuil a ne pas depasser (10 V)
#define U_BAT_MIN 160
/// Activation du sonar. Desactivation utile pour les simu. pour ne pas bloquer.
#define SONAR_ENABLE 1

/// Etat du programme
enum Etat { NOT_START, START, CALIB, RUN };

extern volatile char ETAT;
extern volatile unsigned char VOLTAGE;
extern volatile int PERCENT;
extern volatile int DISTANCE_OBJET;
extern volatile long COUNT_100MS;

/// Attendre 100ms multiplie par [duration]
void delay_100ms(int duration);

#endif /* GLOBALS_H */

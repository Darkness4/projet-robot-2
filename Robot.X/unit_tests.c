/**
 * @file unit_tests.c
 * @author Marc NGUYEN
 * @author Jonas LEFEVRE-DAUMAL
 * @brief Test unitaires
 * @version 1.0.0
 * @date 2019-05-15
 *
 * 1: Batterie
 * 2: Sonar
 * 3: Telecommande
 * 4: Moteur et TMR2
 * 5: UART: Batterie Sonar Moteur ETAT
 */

#include "unit_tests.h"

#include <p18f2520.h>
#include <stdio.h>

#include "globals.h"

#if UNIT_TEST_NUMBER == 4
#include "moteur.h"
#endif

/**
 * @brief Tests unitaires
 *
 * NOTE: Veuillez desactiver DEBUG_PRINT
 * @return int Exit code 0
 */
int unit_tests(void) {
#if UNIT_TEST_NUMBER == 1
  // == Group 1: Batterie ==
  // Test manuel: Les valeurs affiches doivent correspondre au reel
  // Egalement, en dessous de 10 V, la LED Test Vie doit clignoter de periode 2T
  double voltage_convert;
  int voltage_dec;
  int i;
  char texte_uart[] = "  .  ";
  char buffer[10];
  while (1) {
    // Digital
    printf("Digital: %i, ", VOLTAGE);

    // Converti
    voltage_convert = (double)VOLTAGE * 5 / 255;

    // Partie entiere
    itoa((int)voltage_convert, buffer);
    if (voltage_convert < 10) {
      texte_uart[0] = ' ';
      texte_uart[1] = buffer[0];
    } else {
      for (i = 0; i < 2; i++) texte_uart[0 + i] = buffer[i];
    }
    // Partie decimale
    voltage_dec = (int)((voltage_convert - (int)voltage_convert) * 100);
    itoa(voltage_dec, buffer);
    if (voltage_dec < 10) {
      texte_uart[3] = '0';
      texte_uart[4] = buffer[0];
    } else {
      for (i = 0; i < 2; i++) texte_uart[3 + i] = buffer[i];
    }
    printf("Converti: %s, ", texte_uart);

    // Avant diviseur de tension
    voltage_convert = (double)VOLTAGE * 16 / 255;

    // Partie entiere
    itoa((int)voltage_convert, buffer);
    if (voltage_convert < 10) {
      texte_uart[0] = ' ';
      texte_uart[1] = buffer[0];
    } else {
      for (i = 0; i < 2; i++) texte_uart[0 + i] = buffer[i];
    }
    // Partie decimale
    voltage_dec = (int)((voltage_convert - (int)voltage_convert) * 100);
    itoa(voltage_dec, buffer);
    if (voltage_dec < 10) {
      texte_uart[3] = '0';
      texte_uart[4] = buffer[0];
    } else {
      for (i = 0; i < 2; i++) texte_uart[3 + i] = buffer[i];
    }
    printf("Avant diviseur: %s\r\n", texte_uart);
    delay_100ms(1);
  }
#elif UNIT_TEST_NUMBER == 2
  // == Group 2: Sonar ==
  // Test manuel: Les valeurs affiches doivent correspondre au reel
  while (1) {
    printf("ACTIVER LE DEBUG_PRINT ET DESACTIVE\n");
    printf("LES TESTS UNITAIRES ET OBSERVER\n");
  }

#elif UNIT_TEST_NUMBER == 3
  // == Group 3: Telecommande ==
  // Test: Le bouton du milleu doit pouvoir etre capture
  while(1) {
    printf("Veuillez appuyer sur le bouton du millieu\r\n");
    while (ETAT == NOT_START)
      ;
    ETAT = NOT_START;
    printf("Test OK\r\n");
  }

#elif UNIT_TEST_NUMBER == 4
  // == Group 4: Moteur ==
  // Test: Controller la vitesse
  printf("== 75% ramene a 50% ==\r\n");
  printf("Veuillez appuyer sur le bouton du millieu\r\n");
  while (ETAT == NOT_START)
    ;
  ETAT = NOT_START;
  CommandeMoteur(75);

  printf("== 50% ==\r\n");
  printf("Veuillez appuyer sur le bouton du millieu\r\n");
  while (ETAT == NOT_START)
    ;
  ETAT = NOT_START;
  CommandeMoteur(50);

  printf("== 25% ==\r\n");
  printf("Veuillez appuyer sur le bouton du millieu\r\n");
  while (ETAT == NOT_START)
    ;
  ETAT = NOT_START;
  CommandeMoteur(25);

  printf("== 0% ==\r\n");
  printf("Veuillez appuyer sur le bouton du millieu\r\n");
  while (ETAT == NOT_START)
    ;
  ETAT = NOT_START;
  StopMoteur();

  // Test: Tourner
  printf("== Turn D 50% ==\r\n");
  printf("Veuillez appuyer sur le bouton du millieu\r\n");
  while (ETAT == NOT_START)
    ;
  ETAT = NOT_START;
  Tourner(50, 'd');

  printf("== Turn G 50% ==\r\n");
  printf("Veuillez appuyer sur le bouton du millieu\r\n");
  while (ETAT == NOT_START)
    ;
  ETAT = NOT_START;
  Tourner(50, 'g');

  printf("== Avancer ==\r\n");
  printf("Veuillez appuyer sur le bouton du millieu et mesurer la vitesse\r\n");
  while (ETAT == NOT_START)
    ;
  ETAT = NOT_START;
  Avancer(50);
  
  printf("== Freiner ==\r\n");
  printf("Veuillez appuyer sur le bouton du millieu et mesurer la vitesse\r\n");
  while (ETAT == NOT_START)
    ;
  ETAT = NOT_START;
  StopMoteur();
  

#elif UNIT_TEST_NUMBER == 5
  // Observe
  while (1)
    ;
#else
  return 1;
#endif
  return 0;
}

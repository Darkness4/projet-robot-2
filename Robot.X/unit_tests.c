/* 
 * File:   unit_tests.c
 * Author: nguye
 * 
 * Configuration in unit_tests.h
 *
 * 
 * 1: Batterie
 * 2: Sonar
 * 3: Télécommande
 * 4: Moteur et TMR2
 * 5: UART: Batterie Sonar Moteur ETAT
 * 
 * 
 * Created on 12 juin 2019, 21:14
 */
#include "unit_tests.h"

#include <p18f2520.h>
#include <stdio.h>

#include "globals.h"

#if UNIT_TEST_NUMBER==4
#include "moteur.h"
#endif

/**
 * @brief Tests unitaires
 * 
 * NOTE: Veuillez desactiver DEBUG_PRINT
 * @return int Exit code 0
 */
int unit_tests(void) {
#if UNIT_TEST_NUMBER==1
    // == Group 1: Batterie ==
    // Test manuel: Les valeurs affichés doivent correspondre au réel
    // Egalement, en dessous de 10 V, la LED Test Vie doit clignoter de période 2T
    double voltage_convert;
    int voltage_dec;
    int i;
    char texte_uart[] = "  .  ";
    char buffer[10];
    while(1) {
        // Digital
        printf("Digital: %i, ", VOLTAGE);

        // Converti
        voltage_convert = (double)VOLTAGE * 5/255;

        // Partie entière
        itoa((int)voltage_convert, buffer);
        if (voltage_convert < 10) {
            texte_uart[0] = ' ';
            texte_uart[1] = buffer[0];
        } else {
            for (i = 0; i < 2; i++)
                texte_uart[0 + i] = buffer[i];
        }
        // Partie decimale
        voltage_dec = (int)((voltage_convert - (int)voltage_convert)*100);
        itoa(voltage_dec, buffer);
        if (voltage_dec < 10) {
            texte_uart[3] = '0';
            texte_uart[4] = buffer[0];
        }  else {
            for (i = 0; i < 2; i++)
                texte_uart[3 + i] = buffer[i];
        }
        printf("Converti: %s, ", texte_uart);

        // Avant diviseur de tension
        voltage_convert = (double)VOLTAGE * 16/255;

        // Partie entière
        itoa((int)voltage_convert, buffer);
        if (voltage_convert < 10) {
            texte_uart[0] = ' ';
            texte_uart[1] = buffer[0];
        } else {
            for (i = 0; i < 2; i++)
                texte_uart[0 + i] = buffer[i];
        }
        // Partie decimale
        voltage_dec = (int)((voltage_convert - (int)voltage_convert)*100);
        itoa(voltage_dec, buffer);
        if (voltage_dec < 10) {
            texte_uart[3] = '0';
            texte_uart[4] = buffer[0];
        }  else {
            for (i = 0; i < 2; i++)
                texte_uart[3 + i] = buffer[i];
        }
        printf("Avant diviseur: %s\r\n", texte_uart);
        delay_100ms(1);
    }
#elif UNIT_TEST_NUMBER==2
    // == Group 2: Sonar ==
    // Test manuel: Les valeurs affichés doivent correspondre au réel
    printf("Veuillez mesurer manuellement les valeurs réelles\r\n");
    while (1) {
        printf("%i\r\n", DISTANCE_OBJET);
        delay_100ms(1);
    }
#elif UNIT_TEST_NUMBER==3
    // == Group 3: Telecommande ==
    // Test: Le bouton du milleu doit pouvoir etre capture
    printf("Veuillez appuyer sur le bouton du millieu\r\n");
    while(ETAT == NOT_START);
    printf("Test OK\r\n");

#elif UNIT_TEST_NUMBER==4
    // == Group 4: Moteur ==
    // Test: Controller la vitesse
    printf("== 75% ramene a 50% ==\r\n");
    printf("Veuillez appuyer sur le bouton du millieu\r\n");
    while(ETAT == NOT_START);
    ETAT = NOT_START;
    CommandeMoteur(75);

    printf("== 50% ==\r\n");
    printf("Veuillez appuyer sur le bouton du millieu\r\n");
    while(ETAT == NOT_START);
    ETAT = NOT_START;
    CommandeMoteur(50);

    printf("== 25% ==\r\n");
    printf("Veuillez appuyer sur le bouton du millieu\r\n");
    while(ETAT == NOT_START);
    ETAT = NOT_START;
    CommandeMoteur(25);
    
    printf("== 0% ==\r\n");
    printf("Veuillez appuyer sur le bouton du millieu\r\n");
    while(ETAT == NOT_START);
    ETAT = NOT_START;
    CommandeMoteur(0);
    
    // Test: Tourner
    printf("== Turn D 75% ramene a 50% ==\r\n");
    printf("Veuillez appuyer sur le bouton du millieu\r\n");
    while(ETAT == NOT_START);
    ETAT = NOT_START;
    Tourner(75, 'd');

    printf("== Turn D 50% ==\r\n");
    printf("Veuillez appuyer sur le bouton du millieu\r\n");
    while(ETAT == NOT_START);
    ETAT = NOT_START;
    Tourner(50, 'd');

    printf("== Turn D 25% ==\r\n");
    printf("Veuillez appuyer sur le bouton du millieu\r\n");
    while(ETAT == NOT_START);
    ETAT = NOT_START;
    Tourner(25, 'd');

    printf("== Turn D 0% ==\r\n");
    printf("Veuillez appuyer sur le bouton du millieu\r\n");
    while(ETAT == NOT_START);
    ETAT = NOT_START;
    Tourner(0, 'd');

    printf("== Turn G 75% ramene a 50% ==\r\n");
    printf("Veuillez appuyer sur le bouton du millieu\r\n");
    while(ETAT == NOT_START);
    ETAT = NOT_START;
    Tourner(75, 'g');

    printf("== Turn G 50% ==\r\n");
    printf("Veuillez appuyer sur le bouton du millieu\r\n");
    while(ETAT == NOT_START);
    ETAT = NOT_START;
    Tourner(50, 'g');

    printf("== Turn G 25% ==\r\n");
    printf("Veuillez appuyer sur le bouton du millieu\r\n");
    while(ETAT == NOT_START);
    ETAT = NOT_START;
    Tourner(25, 'g');

    printf("== Turn G 0% ==\r\n");
    printf("Veuillez appuyer sur le bouton du millieu\r\n");
    while(ETAT == NOT_START);
    ETAT = NOT_START;
    Tourner(0, 'g');
    
    printf("== Avancer ==\r\n");
    printf("Veuillez appuyer sur le bouton du millieu et mesurer la vitesse\r\n");
    while(ETAT == NOT_START);
    ETAT = NOT_START;
    Avancer(50);
    
    printf("== Avancer 1m ==\r\n");
    printf("Veuillez appuyer sur le bouton du millieu et mesurer la distance\r\n");
    while(ETAT == NOT_START);
    ETAT = NOT_START;  
    Avancer(50);
    delay_100ms(66);

#elif UNIT_TEST_NUMBER==5
    // Observe
#else
    return 1;
#endif
    return 0;
}

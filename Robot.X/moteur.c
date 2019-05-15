#include "globals.h"
#include "init.h"
#include <p18f2520.h>

void CommandeMoteur(int percent, char moteur) {
    // Protection moteur
    if (percent > 50)
        percent = 50;
    else if (percent < 0)
        percent = 0;

    // Calcul CCPR
    CCPR_CALC = percent * 10; // CCPR = percent*Fosc/(100*Prescaler*f_PWM)
                              // 8e6/(100*4*2000) = 10

    if (moteur == 'g') {

        /** Documentation section 15.4.2 :
         * The CCPRxL contains the eight MSbs and the
         * CCPxCON<5 : 4> bits contain the two LSbs
         */
        CCPR1L = CCPR_CALC >> 2;              // CCPR low
        CCP1CONbits.DC1B = CCPR_CALC && 0b11; // CCPR last 2 bits
    } else if (moteur == 'd') {
        CCPR2L = CCPR_CALC >> 2;              // CCPR low
        CCP2CONbits.DC2B = CCPR_CALC && 0b11; // CCPR last 2 bits
    }
}

void Tourner(int percent, char direction) {
    DECELERER = 1;

    while(!(CCPR_CALC <= 0));

    if (direction == 'd') {
        PORTAbits.RA6 = 1;  // DIRD
        PORTAbits.RA7 = 0;  // DIRG
    }
    if (direction == 'g') {
        PORTAbits.RA6 = 0;
        PORTAbits.RA7 = 1;
    }

    CommandeMoteur(percent, 'd');
    CommandeMoteur(percent, 'g');
}

void Avancer(int percent) {
    DECELERER = 1;

    while(!(CCPR_CALC <= 0));

    PORTAbits.RA6 = 0;
    PORTAbits.RA7 = 0;

    CommandeMoteur(percent, 'd');
    CommandeMoteur(percent, 'g');
}

void Calibration() {
    int mesure = DISTANCE_OBJET;
    int percent = CCPR_CALC/10;
    while (percent < 6) {
        if (DISTANCE_OBJET > mesure) {
            if (PORTAbits.RA6 == 1) { //si il tourne à droite
                percent-=5;
                Tourner(percent, 'd');
            }
            if (PORTAbits.RA7 == 1) { //si il tourne à gauche
                percent-=5;
                Tourner(percent, 'd');
            }
        }
        else {
            mesure = DISTANCE_OBJET;
        }
    }
}
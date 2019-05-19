#include "moteur.h"
#include "globals.h"
#include "init.h"
#include <p18f2520.h>

void CommandeMoteur(int percent) {
    int ccpr_calc;

    // Protection moteur
    if (percent > 50)
        percent = 50;
    else if (percent < 0)
        percent = 0;

    // Calcul CCPR
    ccpr_calc = percent * 10; // CCPR = percent*Fosc/(100*Prescaler*f_PWM)
                                  // 8e6/(100*4*2000) = 10

    /** Documentation section 15.4.2 :
     * The CCPRxL contains the eight MSbs and the
     * CCPxCON<5 : 4> bits contain the two LSbs
     */
    CCPR1L = ccpr_calc >> 2;              // CCPR low
    CCP1CONbits.DC1B = ccpr_calc & 0b11;  // CCPR last 2 bits
    CCPR2L = ccpr_calc >> 2;              // CCPR low
    CCP2CONbits.DC2B = ccpr_calc & 0b11;  // CCPR last 2 bits
}

void Tourner(int percent, char direction) {
    // Decelerer();  // TODO: Checker si le systeme crash

    if (direction == 'd') {
        PORTAbits.RA6 = 1; // DIRD
        PORTAbits.RA7 = 0; // DIRG
    } else if (direction == 'g') {
        PORTAbits.RA6 = 0;
        PORTAbits.RA7 = 1;
    }

    PERCENT = percent;

    CommandeMoteur(percent);
}

void Avancer(int percent) {
    PORTAbits.RA6 = 0;
    PORTAbits.RA7 = 0;

    PERCENT = percent;

    CommandeMoteur(percent);
}

void Calibration(void) {
    int mesure = DISTANCE_OBJET;
    while (PERCENT < 6) {
        if (DISTANCE_OBJET > mesure) {
            if (PORTAbits.RA6 == 1) { // si il tourne a droite
                PERCENT -= 5;
                Tourner(PERCENT, 'g');
            }
            if (PORTAbits.RA7 == 1) { // si il tourne a gauche
                PERCENT -= 5;
                Tourner(PERCENT, 'd');
            }
        } else {
            mesure = DISTANCE_OBJET;
        }
    }
}

void Decelerer(void) {
    if(PERCENT > 0) DECELERER = 1;
    while (PERCENT > 0);
}
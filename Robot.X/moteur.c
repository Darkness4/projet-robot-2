#include "moteur.h"
#include "globals.h"
#include "init.h"
#include "MI2C.h"
#include <p18f2520.h>

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


/// Tourner
void Tourner(int percent, char direction) {
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

/// Avancer tout droit
void Avancer(int percent) {
    PORTAbits.RA6 = 0;
    PORTAbits.RA7 = 0;

    CommandeMoteur(percent);
}

/// Balayer et s'aligner avec la cible
void Calibration(void) {
    int time = COUNT_100MS;
    int time2;
    int mini_temp = 100000;
    int idx_temp;
    char i = 0;
    char j;

    // Balayage
    Tourner(25, 'd');
    while(COUNT_100MS - time < 30) {
        if (i < 60) {
            // Cartographier en 1D
            DISTANCE_OBJET_TABLE[i] = DISTANCE_OBJET;
            TIME_OBJET_TABLE[i] = COUNT_100MS - time;
            i++;
            time2 = COUNT_100MS;
            
            // Afficher la progression d'enregistrement
            Write_PCF8574(0x40, ~i);
        }
        delay_100ms(1);
    }
    CommandeMoteur(0);

    // Recherche de minimum
    for (j = 0; j < i; j++) {
        if (DISTANCE_OBJET_TABLE[j] < mini_temp) {
            mini_temp = DISTANCE_OBJET_TABLE[j];
            idx_temp = j;
        }
    }
    Tourner(25, 'g');

    // Afficher le min
    Write_PCF8574(0x40, ~idx_temp);

    // Angle nécéssaire pour s'aligner
    delay_100ms(TIME_OBJET_TABLE[i - 1] - TIME_OBJET_TABLE[idx_temp]);
    CommandeMoteur(0);
}
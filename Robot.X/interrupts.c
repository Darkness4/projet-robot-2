#include "interrupts.h"
#include "MI2C.h"
#include "globals.h"
#include "moteur.h"
#include <p18f2520.h>
#include <stdlib.h>

void HighISR(void);
void LowISR(void);
void decelererInterrupt(void);
void Traduire_i2c_Telecom(char * message);
void setState(void);

/* High-priority interrupt routine */
#pragma code HighVector = 0x08
void IntHighVector(void) { _asm goto HighISR _endasm }
#pragma code

#pragma interrupt HighISR
void HighISR(void) {
    char message[32];
    if (PIR1bits.TXIF) {
        PIR1bits.TXIF = 0;
        if (TEXTE_UART[POS_UART] != '\0') {
            TXREG = TEXTE_UART[POS_UART];
            POS_UART++;
        } else
            PIE1bits.TXIE = 0;
    }
    if (PIR1bits.ADIF) {
        PIR1bits.ADIF = 0;
        VOLTAGE_TMP += ADRESH;
        VOLTAGE_COUNT++;
        if (VOLTAGE_COUNT == 8) {  // Moyenne de 8 mesures
            VOLTAGE = (char)(VOLTAGE_TMP/VOLTAGE_COUNT);
            VOLTAGE_COUNT = 0;
            VOLTAGE_TMP = 0;
        }
    }
    if (INTCONbits.INT0IF) {
        INTCONbits.INT0IF = 0;

        Lire_i2c_Telecom(0xA2, message);
        Traduire_i2c_Telecom(message);
    } 
    if (INTCONbits.TMR0IF) {
        INTCONbits.TMR0IF = 0;
        TMR0H = 0x3C, TMR0L = 0xB0; // Remet a l'offset

        ADCON0bits.GO = 1; // ADC Start

        COUNT_100MS++; // Augmente le compteur
        // Si nouvel etat
        if (IS_START_OLD != IS_START || VOLTAGE_OLD != VOLTAGE ||
            PERCENT_OLD != PERCENT || DISTANCE_OBJET != DISTANCE_OBJET_OLD) {
            setState();
        }

        // Lire la valeur du sonar
        DISTANCE_OBJET = SONAR_Read(0xE0, 0x02);
        // Armer le sonar
        SONAR_Write(0xE0, 0x51); // TODO: Ignore for simulation

        // Si tension batterie, faire clignoter
        if (VOLTAGE < U_BAT_MIN) {
            PORTBbits.RB5 = !PORTBbits.RB5;
        } else if (PORTBbits.RB5 == 0) {  // Si la led est eteinte, allumer.
            PORTBbits.RB5 = 1;
        }

        // decelererInterrupt();
    }
}


/**
 * Decelere toute les 100ms
 */
void decelererInterrupt(void) {
    // Si decelerer, decroitre CCPR de 10 toute les 100ms
    if (DECELERER == 1 && PERCENT > 10) {
        PERCENT -= 10;
        CommandeMoteur(PERCENT);
    } else if (DECELERER == 1 && PERCENT <= 10) {
        PERCENT = 0;
        DECELERER = 0;
        CommandeMoteur(PERCENT);
    }
}


/**
 * Actualise le message UART
 */
void setState(void) {
    char buffer[10];   // UART
    int i;

    // New State
    IS_START_OLD = IS_START;
    VOLTAGE_OLD = VOLTAGE;
    PERCENT_OLD = PERCENT;
    DISTANCE_OBJET_OLD = DISTANCE_OBJET;
    POS_UART = 0;

    // Convert IS_START to char
    TEXTE_UART[6] = IS_START ? '1' : '0';

    TEXTE_UART[65] = PORTAbits.RA7 ? 'G' : '_';
    TEXTE_UART[67] = PORTAbits.RA6 ? 'D' : '_';

    // Convert COUNT_MS to chars
    for (i = 0; i < 5; i++) TEXTE_UART[38 + i] = ' ';  // Clean TEXT
    for (i = 0; i < 10; i++) buffer[i] = 0;
    itoa(COUNT_100MS, buffer);
    for (i = 0; i < 10; i++)
        if (buffer[i] != '\0')
            TEXTE_UART[53 + i] = buffer[i];

    // Convert DISTANCE_OBJET to chars
    for (i = 0; i < 5; i++)
        TEXTE_UART[38 + i] = ' ';
    for (i = 0; i < 10; i++) buffer[i] = 0;
    itoa(DISTANCE_OBJET, buffer);
    for (i = 0; i < 5; i++)
        if (buffer[i] != '\0')
            TEXTE_UART[39 + i] = buffer[i];

    // Convert PERCENT to chars
    for (i = 0; i < 4; i++)
        TEXTE_UART[26 + i] = ' ';
    for (i = 0; i < 10; i++) buffer[i] = 0;
    itoa(PERCENT, buffer); // Max = 50
    for (i = 0; i < 4; i++)
        if (buffer[i] != '\0')
            TEXTE_UART[27 + i] = buffer[i];

    // Convert VOLTAGE to chars
    for (i = 0; i < 3; i++)
        TEXTE_UART[15 + i] = ' ';
    for (i = 0; i < 10; i++) buffer[i] = 0;
    itoa(VOLTAGE, buffer); // Max = 256
    for (i = 0; i < 3; i++)
        if (buffer[i] != '\0')
            TEXTE_UART[15 + i] = buffer[i];

    // Send
    PIE1bits.TXIE = 1;
}


/**
 * Traite le message
 * @param message Message I2C de la telecommande
 */
void Traduire_i2c_Telecom(char * message) {
    int i;
    // Read buffer and send appropriate response
    for (i = 0; i < 32; i++) {
        if (message[i] == '\0')
            break; // End of String
        if (message[i] == 0x33)
            IS_START = 1; // Code Middle Button
    }
}
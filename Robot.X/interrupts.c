#include "interrupts.h"
#include "MI2C.h"
#include "globals.h"
#include "moteur.h"
#include <p18f2520.h>
#include <stdlib.h>

void HighISR(void);
void LowISR(void);
static void Traduire_i2c_Telecom(char * message);
static void setState(void);

/// Tension sauvegarde
static volatile char voltage_tab[8];

/// Texte à envoyer à l'UART
static volatile char texte_uart[] =
    " : VB:   .  , %:     , d:      , t:            ,    \r\n";

static char has_sent = 1;


/* High-priority interrupt routine */
#pragma code HighVector = 0x08
void IntHighVector(void) { _asm goto HighISR _endasm }
#pragma code

#pragma interrupt HighISR
void HighISR(void) {
    char message[32];
    if (INTCONbits.INT0IF) {
        INTCONbits.INT0IF = 0;

        Lire_i2c_Telecom(0xA2, message);
        Traduire_i2c_Telecom(message);
    } else if (INTCONbits.TMR0IF) {
        INTCONbits.TMR0IF = 0;
        TMR0H = 0x3C, TMR0L = 0xB0; // Remet a l'offset

        ADCON0bits.GO = 1; // ADC Start

        COUNT_100MS++; // Augmente le compteur
        // Si nouvel etat
        if (has_sent && (IS_START_OLD != IS_START || VOLTAGE_OLD != VOLTAGE ||
            PERCENT_OLD != PERCENT || DISTANCE_OBJET_OLD != DISTANCE_OBJET)) {
            setState();
        }

        // Lire la valeur du sonar
        DISTANCE_OBJET = SONAR_Read(0xE0, 0x02);
        // Armer le sonar
        // SONAR_Write(0xE0, 0x51); // TODO: Ignore for simulation

        // Si tension batterie, faire clignoter
        if (VOLTAGE < U_BAT_MIN) {
            PORTBbits.RB5 = !PORTBbits.RB5;
        } else if (PORTBbits.RB5 == 0) {  // Si la led est eteinte, allumer.
            PORTBbits.RB5 = 1;
        }

    } else if (PIR1bits.TXIF) {
        if (texte_uart[POS_UART] != '\0') {
            TXREG = texte_uart[POS_UART];
            POS_UART++;
        } else {
            PIE1bits.TXIE = 0;
            has_sent = 1;
        }
        PIR1bits.TXIF = 0;
    } else
        Write_PCF8574(0x40, 1);
}

/* High-priority interrupt routine */
#pragma code LowVector = 0x18
void IntLowVector(void) { _asm goto LowISR _endasm }
#pragma code

#pragma interruptlow LowISR
void LowISR(void) {
    int i;
    int voltage_tmp = 0;
    if (PIR1bits.ADIF) {
        PIR1bits.ADIF = 0;
        voltage_tab[VOLTAGE_COUNT] = ADRESH;
        VOLTAGE_COUNT++;
        if (VOLTAGE_COUNT > 8) VOLTAGE_COUNT = 0;
        for (i = 0; i < 8; i++) {
            voltage_tmp += voltage_tab[i];
        }
        VOLTAGE = voltage_tmp/8;
    }
}


/**
 * Actualise le message UART
 */
static void setState(void) {
    char buffer[10];   // UART
    int i;
    double voltage_convert;
    has_sent = 0;

    // New State
    IS_START_OLD = IS_START;
    VOLTAGE_OLD = VOLTAGE;
    PERCENT_OLD = PERCENT;
    DISTANCE_OBJET_OLD = DISTANCE_OBJET;
    POS_UART = 0;

    // Convert IS_START to char
    texte_uart[0] = IS_START_OLD ? '1' : '0';

    texte_uart[49] = PORTAbits.RA7 ? 'G' : '_';
    texte_uart[51] = PORTAbits.RA6 ? 'D' : '_';

    // Convert COUNT_MS to chars
    for (i = 0; i < 10; i++) buffer[i] = 0;
    itoa(COUNT_100MS, buffer);
    for (i = 0; i < 10; i++)
        if (buffer[i] != '\0')
            texte_uart[36 + i] = buffer[i];
        else
            texte_uart[36 + i] = ' ';

    // Convert DISTANCE_OBJET to chars
    for (i = 0; i < 10; i++) buffer[i] = 0;
    itoa(DISTANCE_OBJET_OLD, buffer);
    for (i = 0; i < 5; i++)
        if (buffer[i] != '\0')
            texte_uart[26 + i] = buffer[i];
        else
            texte_uart[26 + i] = ' ';

    // Convert PERCENT to chars
    for (i = 0; i < 10; i++) buffer[i] = 0;
    itoa(PERCENT_OLD, buffer); // Max = 50
    for (i = 0; i < 4; i++)
        if (buffer[i] != '\0')
            texte_uart[17 + i] = buffer[i];
        else
            texte_uart[17 + i] = ' ';

    // Convert VOLTAGE to chars
    for (i = 0; i < 10; i++) buffer[i] = 0;
    // itoa(VOLTAGE, buffer); // Max = 256
    // Convert to Volt
    // 16 V = 5 V = 255
    voltage_convert = (double)VOLTAGE_OLD * 16/255;

    // Partie entière
    itoa((int)voltage_convert, buffer);
    if (voltage_convert < 10) {
        texte_uart[7] = ' ';
        texte_uart[8] = buffer[0];
    } else {
        for (i = 0; i < 2; i++)
            texte_uart[7 + i] = buffer[i];
    }
    // Partie decimale
    itoa((int)(voltage_convert - (int)voltage_convert)*100, buffer);
    for (i = 0; i < 2; i++)
        if (buffer[i] != '\0')
            texte_uart[10 + i] = buffer[i];
        else
            texte_uart[10 + i] = ' ';

    // Send
    PIE1bits.TXIE = 1;
}


/**
 * Traite le message
 * @param message Message I2C de la telecommande
 */
static void Traduire_i2c_Telecom(char * message) {
    int i;
    // Read buffer and send appropriate response
    for (i = 0; i < 32; i++) {
        if (message[i] == '\0')
            break; // End of String
        if (message[i] == 0x33)
            IS_START = 1; // Code Middle Button
    }
}
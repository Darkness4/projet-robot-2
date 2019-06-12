#include "MI2C.h"
#include "globals.h"
#include "moteur.h"
#include <p18f2520.h>
#include <stdlib.h>

void HighISR(void);
void LowISR(void);
static void Traduire_i2c_Telecom(char * message);

/// Tension sauvegarde
static volatile char voltage_tab[8];
static volatile unsigned char voltage_count = 0;

static volatile char led_interrupt_not_handled = 0;

#if DEBUG_PRINT
static void setState(void);

/// Texte à envoyer à l'UART
static volatile char texte_uart[] =
    " : VB:   .  , %:     , d:      , t:            ,    \r\n";

static volatile char has_sent = 1;
static volatile int pos_uart = 0;
static volatile char etat_old = -1;
static volatile unsigned char voltage_old = -1;
static volatile int percent_old = -1;
static volatile int distance_objet_old = 0;
#endif

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

#if UNIT_TESTS
        ADCON0bits.GO = 1;
#else
        if (COUNT_100MS % 50 == 0) ADCON0bits.GO = 1; // ADC Start
#endif
        COUNT_100MS++; // Augmente le compteur

        // Lire la valeur du sonar
        DISTANCE_OBJET = SONAR_Read(0xE0, 0x02);
        // Armer le sonar
        SONAR_Write(0xE0, 0x51);

        // Si tension batterie, faire clignoter
        if (VOLTAGE < U_BAT_MIN)
            PORTBbits.RB5 = !PORTBbits.RB5;
        else if (PORTBbits.RB5 == 0)  // Si la led est eteinte, allumer
            PORTBbits.RB5 = 1;

#if DEBUG_PRINT
    // Si nouvel etat
    if (has_sent && (etat_old != ETAT || voltage_old != VOLTAGE ||
        percent_old != PERCENT || distance_objet_old != DISTANCE_OBJET))
        setState();  // Nouveau affichage UART
#endif
#if DEBUG_PRINT
    } else if (PIR1bits.TXIF) {
        if (texte_uart[pos_uart] != '\0') {
            TXREG = texte_uart[pos_uart];
            pos_uart++;
        } else {
            PIE1bits.TXIE = 0;
            has_sent = 1;
        }
        PIR1bits.TXIF = 0;
#endif
    } else  // Interruption non controlé, faire clignoter
        Write_PCF8574(0x40, !led_interrupt_not_handled);
}

/* Low-priority interrupt routine */
#pragma code LowVector = 0x18
void IntLowVector(void) { _asm goto LowISR _endasm }
#pragma code

#pragma interruptlow LowISR
void LowISR(void) {
    int i;
    int voltage_tmp = 0;
    if (PIR1bits.ADIF) {
        PIR1bits.ADIF = 0;
        voltage_tab[voltage_count] = ADRESH;
        voltage_count++;
        if (voltage_count > 8) voltage_count = 0;
        for (i = 0; i < 8; i++) voltage_tmp += voltage_tab[i];
        VOLTAGE = voltage_tmp/8;
    } else  // Interruption non controlé, faire clignoter
        Write_PCF8574(0x40, !led_interrupt_not_handled);
}


#if DEBUG_PRINT
/**
 * Actualise le message UART
 */
static void setState(void) {
    char buffer[10];   // UART
    int i;
    double voltage_convert;
    int voltage_dec;
    has_sent = 0;

    // New State
    etat_old = ETAT;
    voltage_old = VOLTAGE;
    percent_old = PERCENT;
    distance_objet_old = DISTANCE_OBJET;
    pos_uart = 0;

    // Convert ETAT to char
    texte_uart[0] = etat_old + 48;

    // Convert motor direction to char
    texte_uart[49] = PORTAbits.RA7 ? 'G' : '_';
    texte_uart[51] = PORTAbits.RA6 ? 'D' : '_';

    for (i = 0; i < 10; i++) buffer[i] = 0;
    // Convert COUNT_MS to chars
    itoa(COUNT_100MS, buffer);
    for (i = 0; i < 10; i++)
        if (buffer[i] != '\0')
            texte_uart[36 + i] = buffer[i];
        else
            texte_uart[36 + i] = ' ';

    for (i = 0; i < 10; i++) buffer[i] = 0;
    // Convert DISTANCE_OBJET to chars
    itoa(distance_objet_old, buffer);
    for (i = 0; i < 5; i++)
        if (buffer[i] != '\0')
            texte_uart[26 + i] = buffer[i];
        else
            texte_uart[26 + i] = ' ';

    for (i = 0; i < 10; i++) buffer[i] = 0;
    // Convert PERCENT to chars
    itoa(percent_old, buffer); // Max = 50
    for (i = 0; i < 4; i++)
        if (buffer[i] != '\0')
            texte_uart[17 + i] = buffer[i];
        else
            texte_uart[17 + i] = ' ';

    for (i = 0; i < 10; i++) buffer[i] = 0;
    // Convert VOLTAGE to chars
    // Convert to Volt
    // 16 V = 5 V = 255
    voltage_convert = (double)voltage_old * 16/255;

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
    voltage_dec = (int)((voltage_convert - (int)voltage_convert)*100);
    itoa(voltage_dec, buffer);
    if (voltage_dec < 10) {
        texte_uart[7] = '0';
        texte_uart[8] = buffer[0];
    }  else {
        for (i = 0; i < 2; i++)
            texte_uart[10 + i] = buffer[i];
    }

    // Send
    PIE1bits.TXIE = 1;
}
#endif


/**
 * Traite le message
 * @param message Message I2C de la telecommande
 */
static void Traduire_i2c_Telecom(char * message) {
    int i;
    for (i = 0; i < 32; i++) {
        if (message[i] == '\0') // End of String
            break;
        if (message[i] == 0x33) // Code Middle Button
            ETAT = START;
    }
}
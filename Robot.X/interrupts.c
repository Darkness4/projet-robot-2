/**
 * @file interrupts.c
 * @author Marc NGUYEN
 * @author Jonas LEFEVRE-DAUMAL
 * @brief Interruptions du programme
 * @version 1.0.0
 * @date 2019-05-15
 *
 */
#include <p18f2520.h>
#include <stdlib.h>

#include "MI2C.h"
#include "globals.h"
#include "moteur.h"

/// Interruptions haute priorite
void HighISR(void);
/// Interruptions basse priorite
void LowISR(void);
/// Compare le code de la telecommande et traduit en fonction
static void Traduire_i2c_Telecom(char* message);

/// Tension sauvegarde
static volatile int voltage_tab[] = {-1, -1, -1, -1, -1, -1, -1, -1};
/// Indice de la valeur a stocker dans le tableau voltage_tab
static volatile unsigned char voltage_count = 0;

/// Controle des LEDs
static struct {
  /// Led Test Vie
  unsigned interrupt_not_handled : 1;
  /// PCF8574: Toutes les leds
  unsigned battery_warning : 1;
} LEDS;

#if DEBUG_PRINT || UNIT_TEST_NUMBER == 5
/// Actualise UART
static void setState(void);

/// Texte a envoyer a l'UART
static volatile char texte_uart[] =
    " : VB:   .  , %:     , d:      , t:            ,    \r\n";
/// Si UART est libre, has_sent = 1.
static volatile char has_sent = 1;
/// Position du charactere dans le String a envoyer
static volatile int pos_uart = 0;

static volatile char etat_old = -1;
static volatile unsigned char voltage_old = -1;
static volatile int percent_old = -1;
static volatile int distance_objet_old = 0;
#endif

// High-priority interrupt routine
#pragma code HighVector = 0x08
/// Vecteur vers interruption haute priorite
void IntHighVector(void) { _asm goto HighISR _endasm }
#pragma code

#pragma interrupt HighISR
void HighISR(void) {
  char message[32];
  if (INTCONbits.INT0IF) {
    Lire_i2c_Telecom(0xA2, message);
    Traduire_i2c_Telecom(message);
    INTCONbits.INT0IF = 0;
  } else if (INTCONbits.TMR0IF) {
    INTCONbits.TMR0IF = 0;
    TMR0H = 0x3C, TMR0L = 0xB0;  // Remet a l'offset

    ADCON0bits.GO = 1;  // ADC Fast Start
    COUNT_100MS++;      // Augmente le compteur

#if SONAR_ENABLE
    // Lire la valeur du sonar
    DISTANCE_OBJET = SONAR_Read(0xE0, 0x02);
    // Armer le sonar
    SONAR_Write(0xE0, 0x51);
#endif

    // Si tension batterie, faire clignoter
    if (VOLTAGE < U_BAT_MIN) {
      LEDS.battery_warning = !LEDS.battery_warning;
      PORTBbits.RB5 = LEDS.battery_warning;
    } else if (LEDS.battery_warning == 0) {  // Si la led est eteinte, allumer
      LEDS.battery_warning = 1;
      PORTBbits.RB5 = 1;
    }

#if DEBUG_PRINT || UNIT_TEST_NUMBER == 5
    // Si nouvel etat
    if (has_sent &&
        (etat_old != ETAT || voltage_old != VOLTAGE || percent_old != PERCENT ||
         distance_objet_old != DISTANCE_OBJET))
      setState();  // Nouveau affichage UART
  } else if (PIR1bits.TXIF) {
    if (texte_uart[pos_uart] != '\0') {
      // Envoyer le texte
      TXREG = texte_uart[pos_uart];
      pos_uart++;
    } else {
      // Desactiver et "liberer" UART
      PIE1bits.TXIE = 0;
      has_sent = 1;
    }
    PIR1bits.TXIF = 0;
#endif

  } else {  // Interruption non controle, faire clignoter
    LEDS.interrupt_not_handled = !LEDS.interrupt_not_handled;
    Write_PCF8574(0x40, LEDS.interrupt_not_handled);
  }
}

// Low-priority interrupt routine
#pragma code LowVector = 0x18
/// Vecteur vers interruption basse priorite
void IntLowVector(void) { _asm goto LowISR _endasm }
#pragma code

#pragma interruptlow LowISR
void LowISR(void) {
  int i;
  int voltage_tmp = 0;
  if (PIR1bits.ADIF) {
    PIR1bits.ADIF = 0;
    // Enregistre les 8 dernieres mesures
    voltage_tab[voltage_count] = ADRESH;

    for (i = 0; i < 8; i++) {
      if (voltage_tab[i] != -1)
        voltage_tmp += voltage_tab[i];
      else
        break;
    }
    VOLTAGE = voltage_tmp / i;

    voltage_count++;
    if (voltage_count >= 8) voltage_count = 0;
  } else {  // Interruption non controle, faire clignoter
    LEDS.interrupt_not_handled = !LEDS.interrupt_not_handled;
    Write_PCF8574(0x40, LEDS.interrupt_not_handled);
  }
}

#if DEBUG_PRINT || UNIT_TEST_NUMBER == 5
/// Actualise le message UART
static void setState(void) {
  char buffer[10];  // UART
  int i;
  double voltage_convert;
  int voltage_dec;
  has_sent = 0;  // Occupe UART

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

  for (i = 0; i < 10; i++) buffer[i] = 0;  // Clean buffer
  // Convert COUNT_MS to chars
  itoa(COUNT_100MS, buffer);
  for (i = 0; i < 10; i++)
    if (buffer[i] != '\0')
      texte_uart[36 + i] = buffer[i];
    else
      texte_uart[36 + i] = ' ';

  for (i = 0; i < 10; i++) buffer[i] = 0;  // Clean buffer
  // Convert DISTANCE_OBJET to chars
  itoa(distance_objet_old, buffer);
  for (i = 0; i < 5; i++)
    if (buffer[i] != '\0')
      texte_uart[26 + i] = buffer[i];
    else
      texte_uart[26 + i] = ' ';

  for (i = 0; i < 10; i++) buffer[i] = 0;  // Clean buffer
  // Convert PERCENT to chars
  itoa(percent_old, buffer);  // Max = 50
  for (i = 0; i < 4; i++)
    if (buffer[i] != '\0')
      texte_uart[17 + i] = buffer[i];
    else
      texte_uart[17 + i] = ' ';

  for (i = 0; i < 10; i++) buffer[i] = 0;  // Clean buffer
  // Convert VOLTAGE to chars
  // Convert Digital to Volt
  // 16 V = 5 V = 255
  voltage_convert = (double)voltage_old * 16 / 255;

  // Partie entiere
  itoa((int)voltage_convert, buffer);
  if (voltage_convert < 10) {
    texte_uart[7] = ' ';
    texte_uart[8] = buffer[0];
  } else {
    for (i = 0; i < 2; i++) texte_uart[7 + i] = buffer[i];
  }
  // Partie decimale
  voltage_dec = (int)((voltage_convert - (int)voltage_convert) * 100);
  itoa(voltage_dec, buffer);
  if (voltage_dec < 10) {
    texte_uart[7] = '0';
    texte_uart[8] = buffer[0];
  } else {
    for (i = 0; i < 2; i++) texte_uart[10 + i] = buffer[i];
  }

  // Send
  PIE1bits.TXIE = 1;
}
#endif

/**
 * @brief Traite le message
 *
 * @param message Message I2C de la telecommande
 */
static void Traduire_i2c_Telecom(char* message) {
  int i;
  for (i = 0; i < 32; i++) {
    if (message[i] == '\0')  // End of String
      break;
    if (message[i] == 0x33) {  // Code Middle Button
      ETAT = START;
    }
  }
}
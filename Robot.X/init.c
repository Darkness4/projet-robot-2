/**
 * @file init.c
 * @author Marc NGUYEN
 * @author Jonas LEFEVRE-DAUMAL
 * @brief Initialisation du programme
 * @version 1.0.0
 * @date 2019-05-15
 *
 */
#include "init.h"

#include <p18f2520.h>

#include "MI2C.h"
#include "globals.h"
#include "unit_tests.h"

/// Initialise le CLK
static void init_clk(void);
/// Initialise le TMR0
static void init_tmr0(void);
/// Initialise le TMR2
static void init_tmr2(void);
/// Initialise UART
static void init_uart(void);
/// Initialise les Moteurs
static void init_motors(void);
/// Initialise les interruptions
static void init_interrupts(void);
/// Initialise les CAN
static void init_adc(void);
/// Initialise les Capteurs IR
static void init_IR(void);

int init(void) {
#if DEBUG_PRINT || UNIT_TEST_NUMBER == 5
  int i = 0;
  char texte_init[] = "FIN INIT\r\n";
#endif

  init_clk();

  // LED Vie: Output pour indiquer l'etat de la batterie
  TRISBbits.RB5 = 0;
  // Baterie: Input
  TRISAbits.RA2 = 1;
  // Telecommande: Input
  TRISBbits.RB0 = 1;

  MI2CInit();
  init_IR();
  init_tmr0();
  init_adc();
  init_motors();
  init_tmr2();
  init_interrupts();

#if SONAR_ENABLE
  SONAR_Write(0xE0, 0x51);
#endif

#if DEBUG_PRINT || UNIT_TEST_NUMBER
  init_uart();
#endif

#if DEBUG_PRINT || UNIT_TEST_NUMBER == 5
  while (i < 10) {
    if (PIR1bits.TXIF) {
      PIR1bits.TXIF = 0;
      TXREG = texte_init[i];
      i++;
    }
  }
#endif

  return 0;
}

static void init_IR(void) {
  // Eteindre les capteurs IR
  TRISAbits.RA0 = 0;
  PORTAbits.RA0 = 0;
  TRISAbits.RA1 = 0;
  PORTAbits.RA1 = 0;
}

static void init_clk(void) {
  // Clock: 8 MHz
  OSCCONbits.IRCF = 0b111;
}

/** TIMER0
 *
 * Actualise l'etat de la batterie, calcule le sonar et fait decelerer.
 *
 * Interrompt tout les T=100ms.
 * Mode 16bit => TMR0_Max = 65 536
 * T*f_osc/4 = 100e-3*8e6/4 = 200 000 > 65 536
 * Avec prescale :
 * T*f_osc/4/PS = 100e-3*8e6/4/4 = 50 000 < 65 536
 * Offset 15536 = 0x3CB0
 */
static void init_tmr0(void) {
  // TMR0: Mode 16 bits
  T0CONbits.T08BIT = 0;
  // TMR0: Prescale 1:4
  T0CONbits.T0PS = 0b001;

  // TMR0: Offset 0x3CB0
  TMR0H = 0x3C, TMR0L = 0xB0;

  // TMR0: Use Internal Clock (CLKO)
  T0CONbits.T0CS = 0;
  // TMR0: Prescale Enable
  T0CONbits.PSA = 0;

  // TMR0: Enable
  T0CONbits.TMR0ON = 1;
}

/**
 * @brief TIMER2
 *
 * En PWM, commande les moteurs.
 *
 * Frequence = 2 kHz.
 *
 * Init: CCPR = 0 (vitesse nulle)
 * PR2 = 249 avec Prescale = 4
 */
static void init_tmr2(void) {
  // TMR2: Prescale 4
  T2CONbits.T2CKPS = 0b01;

  // TMR2: PR=249
  PR2 = 249;
  // CCPR est sur 10 bits
  // CCPRxH | CCPRxL | CCPxCONbits.DCxB
  // 4 bits | 4 bits | 2 bits
  CCPR1L = 0;
  CCPR2L = 0;
  CCP1CONbits.DC1B = 0;
  CCP2CONbits.DC2B = 0;
  // TMR2: PWM mode
  CCP2CONbits.CCP2M = 0xff;
  CCP1CONbits.CCP1M = 0xff;

  // TMR2: Enable
  T2CONbits.TMR2ON = 1;
}

static void init_uart(void) {
  // Serial: TX (voir doc)
  TRISCbits.RC6 = 1;
  // Serial: RX
  TRISCbits.RC7 = 1;

  // Serial: 8-bit Baud Rate Generator
  BAUDCONbits.BRG16 = 1;
  // Serial: High Baud Rate Select bit : High Speed
  TXSTAbits.BRGH = 1;
  // Serial: Async mode
  TXSTAbits.SYNC = 0;
  // Serial: Voir documentation pour reglage conseille page 207, edition 2008
  SPBRG = 207;  // 9615 Hz

  // Serial: Transmit Enable
  TXSTAbits.TXEN = 1;
  // Serial: Receive Disable (no use)
  RCSTAbits.CREN = 0;
  // Serial: Enable
  RCSTAbits.SPEN = 1;
}

static void init_motors(void) {
  // Moteur: PWM: Droite Output
  TRISCbits.RC1 = 0;
  // Moteur: PWM: Gauche Output
  TRISCbits.RC2 = 0;
  // Moteur: Direction droite: Output
  TRISAbits.RA6 = 0;
  // Moteur: Direction gauche: Output
  TRISAbits.RA7 = 0;
}

static void init_adc(void) {
  // ADC: AN0 - AN2: Enable analog
  ADCON1bits.PCFG = 0b1100;
  // ADC: Vref+ = VDD et Vref- = VSS
  ADCON1bits.VCFG = 0;
  // ADC: T_AD = 1us > 0.7us
  ADCON2bits.ADCS = 0b001;  // F_osc/8
  // ADC: T_ACQ = 6us > 4.2us
  ADCON2bits.ACQT = 0b011;  // T_AD*6
  // ADC: Select AN2
  ADCON0bits.CHS = 0b0010;

  // ADC: Jusitifier a gauche
  ADCON2bits.ADFM = 0;
  // ADC: Enable
  ADCON0bits.ADON = 1;
}

static void init_interrupts(void) {
  // Telecommande: Clear Interrupt Flag
  INTCONbits.INT0IF = 0;
  // Telecommande: Interrupt Enable
  INTCONbits.INT0IE = 1;
  // TX: Clear Interrupt Flag
  PIR1bits.TXIF = 0;
  // TX: Interrupt Enable
  PIE1bits.TXIE = 0;
  // TMR0: Clear Interrupt Flag
  INTCONbits.TMR0IF = 0;
  // TMR0: Interrupt Enable
  INTCONbits.TMR0IE = 1;
  // ADC: Interrupt low priority
  IPR1bits.ADIP = 0;
  // ADC: Clear Interrupt Flag
  PIR1bits.ADIF = 0;
  // ADC: Interrupt Enable
  PIE1bits.ADIE = 1;

  // Global Low Interrupt: Enable
  INTCONbits.GIEL = 1;
  // Global High Interrupt: Enable
  INTCONbits.GIEH = 1;
  // Priority Mode: Low et High
  RCONbits.IPEN = 1;
}
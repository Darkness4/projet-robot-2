#include "init.h"
#include <p18f2520.h>
#include "MI2C.h"
#include "globals.h"

void init(void) {
  /// Clock: 8 MHz
  OSCCONbits.IRCF = 0b111;

  /// Eteindre les capteurs IR

  /// LED Vie: Output pour indiquer l'etat de la batterie
  TRISBbits.RB5 = 0;
  PORTBbits.RB5 = LED_STATUS;

  /// T�l�commande: Input
  TRISBbits.RB0 = 1;
  /// Télécommande: Clear Interrupt Flag
  INTCONbits.INT0IF = 0;
  /// Télécommande: Interrupt Enable
  INTCONbits.INT0IE = 1;
  /// Direction droite
  PORTAbits.RA6 = 1;
  /// Direction gauche
  PORTAbits.RA7 = 1;

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
  /// TMR0: Mode 16 bits
  T0CONbits.T08BIT = 0;
  /// TMR0: Prescale 1:4
  T0CONbits.T0PS = 0b001;

  /// TMR0: Offset 0x3CB0
  TMR0H = 0x3C, TMR0L = 0xB0;

  /// TMR0: Use Internal Clock (CLKO)
  T0CONbits.T0CS = 0;
  /// TMR0: Prescale Enable
  T0CONbits.PSA = 0;

  /// TMR0: Enable
  T0CONbits.TMR0ON = 1;
  /// TMR0: Clear Interrupt Flag
  INTCONbits.TMR0IF = 0;
  /// TMR0: Interrupt Enable
  INTCONbits.TMR0IE = 1;

  /*CAN*/
  /// ADC: AN0 - AN2: Enable analog
  ADCON1bits.PCFG = 0b1100;
  /// ADC: Vref+ = VDD et Vref- = VSS
  ADCON1bits.VCFG = 0;
  /// ADC: T_AD = 1us > 0.7us
  ADCON2bits.ADCS = 0b001;  // F_osc/8
  /// ADC: T_ACQ = 6us > 4.2us
  ADCON2bits.ACQT = 0b011;  // T_AD*6
  /// ADC: Select AN2
  ADCON0bits.CHS = 0b0010;

  /// ADC: Jusitifier a gauche
  ADCON2bits.ADFM = 0;
  /// ADC: Enable
  ADCON0bits.ADON = 1;
  /// ADC: Interrupt low priority
  IPR1bits.ADIP = 1;
  /// ADC: Clear Interrupt Flag
  PIR1bits.ADIF = 0;
  /// ADC: Interrupt Enable
  PIE1bits.ADIE = 1;

  /*I2C Init*/
  MI2CInit();

  /*UART Init*/
  /// Serial: TX (voir doc)
  TRISCbits.RC6 = 1;
  /// Serial: RX
  TRISCbits.RC7 = 1;

  /// Serial: 8-bit Baud Rate Generator
  BAUDCONbits.BRG16 = 0;
  /// Serial: High Baud Rate Select bit : High Speed
  TXSTAbits.BRGH = 1;
  /// Serial: Async mode
  TXSTAbits.SYNC = 0;
  /// Serial: Voir documentation pour reglage conseille page 207, edition 2008
  SPBRG = 51;  // 9.615 kHz

  /// Serial: Transmit Enable
  TXSTAbits.TXEN = 1;
  /// Serial: Receive Disable (no use)
  RCSTAbits.CREN = 0;
  /// Serial: Enable
  RCSTAbits.SPEN = 1;

  /// TX: Clear Interrupt Flag
  PIR1bits.TXIF = 0;
  /// TX: Interrupt Enable
  PIE1bits.TXIE = 1;

  /** TIMER2
   *
   * En PWM, commande les moteurs.
   *
   * Frequence = 2 kHz.
   *
   * Init: CCPR = 0 (vitesse nulle)
   * PR2 = 249 avec Prescale = 4
   */
  /// TMR2: Prescale 4
  T2CONbits.T2CKPS = 0b01;

  /// TMR2: PR=249
  PR2 = 249;
  // CCPR est sur 10 bits
  // CCPRxH | CCPRxL | CCPxCONbits.DCxB
  // 4 bits | 4 bits | 2 bits
  CCPR1L = 0;
  CCPR2L = 0;
  CCP1CONbits.DC1B = 0;
  CCP2CONbits.DC2B = 0;
  /// TMR2: PWM mode, P1A, P1C active-low; P1B, P1D active-low
  CCP2CONbits.CCP2M = 0xf;
  /// TMR2: PWM mode, P1A, P1C active-low; P1B, P1D active-low
  CCP1CONbits.CCP1M = 0xf;

  /// TMR2: Enable
  T2CONbits.TMR2ON = 1;
  /// TMR2: Clear Interrupt Flag
  PIR1bits.TMR2IF = 0;
  /// TMR2: Interrupt Enable
  PIE1bits.TMR2IE = 1;

  /*Interrupts*/
  /// Priority Mode: Low et High
  RCONbits.IPEN = 1;
  /// Global Interrupt: Enable High
  INTCONbits.GIEH = 1;
  /// Global Interrupt: Enable Low
  INTCONbits.GIEL = 1;
}
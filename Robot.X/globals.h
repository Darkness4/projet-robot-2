/*
 * File:   globals.h
 * Author: nguye
 *
 * Created on 7 mai 2019, 16:10
 */

#ifndef GLOBALS_H
#define GLOBALS_H

extern volatile char LED_STATUS;
extern volatile char IS_START;
extern volatile unsigned char VOLTAGE;
extern const unsigned char U_BAT_MIN;
extern volatile int CCPR_CALC;
extern volatile char DECELERER;
extern volatile char TEXTE_UART[];
extern volatile int POS_UART;
extern volatile int DISTANCE_OBJET;
extern volatile char IS_START_OLD;
extern volatile unsigned char VOLTAGE_OLD;
extern volatile int CCPR_CALC_OLD;
extern volatile int DISTANCE_OBJET_OLD;

#endif /* GLOBALS_H */

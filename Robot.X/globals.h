/*
 * File:   globals.h
 * Author: nguye
 *
 * Created on 7 mai 2019, 16:10
 */

#ifndef GLOBALS_H
#define GLOBALS_H

extern volatile char IS_START;
extern volatile unsigned char VOLTAGE;
extern volatile unsigned char VOLTAGE_COUNT;
extern volatile unsigned int VOLTAGE_TMP;
extern const unsigned char U_BAT_MIN;
extern volatile int PERCENT;
extern volatile char TEXTE_UART[];
extern volatile int POS_UART;
extern volatile int DISTANCE_OBJET;
extern volatile char IS_START_OLD;
extern volatile unsigned char VOLTAGE_OLD;
extern volatile int PERCENT_OLD;
extern volatile int DISTANCE_OBJET_OLD;
extern volatile long COUNT_100MS;
extern volatile int DISTANCE_OBJET_TABLE[];
extern volatile int TIME_OBJET_TABLE[];


void delay_100ms(int duration);

#endif /* GLOBALS_H */

/*
 * File:   globals.h
 * Author: nguye
 *
 * Created on 7 mai 2019, 16:10
 */

#ifndef GLOBALS_H
#define GLOBALS_H

#define UNIT_TESTS 0
#define DEBUG_PRINT 1
#define U_BAT_MIN 160

enum Etat {NOT_START, START, CALIB, RUN};

extern volatile char ETAT;
extern volatile unsigned char VOLTAGE;
extern volatile int PERCENT;
extern volatile int DISTANCE_OBJET;
extern volatile long COUNT_100MS;


void delay_100ms(int duration);

#endif /* GLOBALS_H */

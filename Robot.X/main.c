/*
 * File:   main.c
 * Author: nguye
 *
 * Created on 6 mai 2019, 16:44
 */

// La configuration globale est dans globals.h

#pragma config OSC = INTIO67
#pragma config PBADEN = OFF, WDT = OFF, LVP = OFF, DEBUG = OFF

#if UNIT_TESTS
#include "unit_tests.h"
#else
#include "loop.h"
#endif

#include "globals.h"
#include "init.h"

#include <p18f2520.h>

void main(void) {
    init();
    while (1) {
        ETAT = NOT_START;
#if UNIT_TESTS
        unit_tests();
#else
        loop();
#endif
    }
}
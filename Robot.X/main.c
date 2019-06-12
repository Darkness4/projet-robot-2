/*
 * File:   main.c
 * Author: nguye
 *
 * Created on 6 mai 2019, 16:44
 */

// La configuration globale est dans globals.h

#pragma config OSC = INTIO67
#pragma config PBADEN = OFF, WDT = OFF, LVP = OFF, DEBUG = ON

#include <p18f2520.h>

#include "globals.h"
#include "init.h"

#if UNIT_TESTS
#include "unit_tests.h"
#include <stdio.h>
#else
#include "loop.h"
#endif

void main(void) {
    init();
    while (1) {
        ETAT = NOT_START;
#if UNIT_TESTS
    if (unit_tests())
        printf("Erreur, test inconnu! \r\n");
#else
        loop();
#endif
    }
}
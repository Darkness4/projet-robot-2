/**
 * @file main.c
 * @author Marc NGUYEN
 * @author Jonas LEFEVRE-DAUMAL
 * @brief Coeur du programme
 * @version 1.0.0
 * @date 2019-05-15
 *
 */

// La configuration globale est dans globals.h

#pragma config OSC = INTIO67
#pragma config PBADEN = OFF, WDT = OFF, LVP = OFF, DEBUG = ON

#include <p18f2520.h>

#include "globals.h"
#include "init.h"

#if UNIT_TESTS
#include <stdio.h>
#include "unit_tests.h"
#else
#include "loop.h"
#endif

/// Algorithme principal
void main(void) {
  init();
#if UNIT_TESTS
  if (unit_tests()) printf("Erreur, test inconnu! \r\n");
#else
  loop();
#endif
}
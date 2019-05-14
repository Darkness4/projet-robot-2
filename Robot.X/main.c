/*
 * File:   main.c
 * Author: nguye
 *
 * Created on 6 mai 2019, 16:44
 */

#pragma config OSC = INTIO67
#pragma config PBADEN = OFF, WDT = OFF, LVP = OFF, DEBUG = ON

#include <p18f2520.h>
#include "init.h"
#include "interrupts.h"

/*
 *
 */
void main(void) {
  init();
  loop();
}

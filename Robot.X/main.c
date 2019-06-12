/*
 * File:   main.c
 * Author: nguye
 *
 * Created on 6 mai 2019, 16:44
 */

#pragma config OSC = INTIO67
#pragma config PBADEN = OFF, WDT = OFF, LVP = OFF

#include "init.h"
#include "interrupts.h"
#include "globals.h"
#include "loop.h"
#include <p18f2520.h>

/*
 *
 */
void main(void) {
    init();
    while (1) {
        IS_START = 0;
        loop();
    }
}

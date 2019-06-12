#include "globals.h"


/// Condition de demarrage
volatile char IS_START = 0;

/// Voltage
volatile unsigned char VOLTAGE = 255;
volatile unsigned char VOLTAGE_COUNT = 0;

/// Tension minimale pour l'alerte LED
const unsigned char U_BAT_MIN = 160;

/// Rapport cyclique
volatile int PERCENT = 0;

/// Distance de l'objet
volatile int DISTANCE_OBJET = 0;

/// Position du curseur UART
volatile int POS_UART = 0;

/// Count
volatile long COUNT_100MS = 0;

/// Sauvegarde des états
volatile char IS_START_OLD = -1;
volatile unsigned char VOLTAGE_OLD = -1;
volatile int PERCENT_OLD = -1;
volatile int DISTANCE_OBJET_OLD = 0;

volatile int DISTANCE_OBJET_TABLE[60];
volatile int TIME_OBJET_TABLE[60];

void delay_100ms(int duration) {
    int time_origin = COUNT_100MS;
    while (COUNT_100MS - time_origin < duration);
}
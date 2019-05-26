#include "globals.h"


/// Condition de demarrage
volatile char IS_START = 0;

/// Voltage
volatile unsigned char VOLTAGE = 255;
volatile unsigned char VOLTAGE_COUNT = 0;
volatile unsigned int VOLTAGE_TMP = 0;

/// Tension minimale pour l'alerte LED
const unsigned char U_BAT_MIN = 160;

/// Rapport cyclique
volatile int PERCENT = 0;

/// Ordre de decelerer
volatile char DECELERER = 0;

/// Texte à envoyer à l'UART
volatile char TEXTE_UART[] =
    "ETAT:  , VBAT:    , PERC:      , DIST:      , TMR0:            ,     \r\n\0";

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
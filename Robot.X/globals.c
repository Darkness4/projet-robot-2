#include "globals.h"

volatile char LED_STATUS = 1;
volatile char IS_START = 0;
volatile unsigned char VOLTAGE = 0;
const unsigned char U_BAT_MIN = 160;
volatile int CCPR_CALC = 0;
volatile char DECELERER = 0;
volatile char TEXTE_UART[] = "ETAT:  , VBAT:    , CCPR:     , DIST:      \r\n\0";
volatile int DISTANCE_OBJET = 0;
volatile int POS_UART = 0;

volatile char IS_START_OLD = -1;
volatile unsigned char VOLTAGE_OLD = -1;
volatile int CCPR_CALC_OLD = -1;
volatile int DISTANCE_OBJET_OLD = 0;
#include "globals.h"

char LED_STATUS = 1;
char IS_START = 0;
unsigned char VOLTAGE = 0;
const unsigned char U_BAT_MIN = 160;
int CCPR_CALC = 0;
char DECELERER = 0;
char TEXTE_UART[] = "ETAT:  , VBAT:    , CCPR:     \r\n\0";
int DISTANCE_OBJET = 0;
int POS_UART = 0;

char IS_START_OLD = -1;
unsigned char VOLTAGE_OLD = -1;
int CCPR_CALC_OLD = -1;
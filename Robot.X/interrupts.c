#include <p18f2520.h>
#include "MI2C.h"

int IS_START = 0;

void HighISR(void);
void LowISR(void);

/* High-priority interrupt routine */
#pragma code HighVector=0x08
void IntHighVector(void)
{
    _asm goto HighISR _endasm
}
/* Low-priority interrupt routine */
#pragma code LowVector=0x18
void IntLowVector(void)
{
    _asm goto LowISR _endasm
}
#pragma code

#pragma interrupt HighISR
void HighISR(void) {
    char message[32];
    int i;
        if(INTCONbits.INT0IF){
        INTCONbits.INT0IF = 0;
        Lire_i2c_Telecom(0xA2, message);
        for(i=0; i<32; i++) {
            if(message[i] == '\0') break;
            if(message[i] == 0x33) {
                IS_START = !IS_START;
            }
        }
    }
}


#pragma interrupt LowISR
void LowISR (void){}

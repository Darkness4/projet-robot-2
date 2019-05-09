#include <p18f2520.h>
#include "MI2C.h"
#include "globals.h"

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
        for(i=0; i < 32; i++) {
            if(message[i] == '\0') break;
            if(message[i] == 0x33) {
                IS_START = !IS_START;
            }
        }
    } else if (INTCONbits.TMR0IF) {
        INTCONbits.TMR0IF = 0;
        TMR0H = 0x3C, TMR0L = 0xB0;  // Remet � l'offset

        ADCON0bits.GO = 1;  // ADC Start

        DISTANCE_OBJET = SONAR_Read(0xE0, 0x02);
        SONAR_Write(0xE0, 0x51);

        // Si tension batterie, faire clignoter
        if (VOLTAGE < U_BAT_MIN) {
            LED_STATUS = !LED_STATUS;
            PORTBbits.RB5 = LED_STATUS;
        } else if (LED_STATUS == 0) {
            LED_STATUS = 1;
            PORTBbits.RB5 = LED_STATUS;
        }
        
        // Si decelerer, d�croite CCPR de 10 toute les 100ms
        if (DECELERER && CCPR_CALC > 10) {
            CCPR_CALC -= 10;
            CCPR1L = CCPR_CALC >> 2;
            CCPR2L = CCPR_CALC >> 2;
            CCP1CONbits.DC1B = CCPR_CALC && 0b11;
            CCP2CONbits.DC2B = CCPR_CALC && 0b11;
        } else if (CCPR_CALC < 10) {
            CCPR_CALC = 0;
            CCPR1L = 0;
            CCPR2L = 0;
            CCP1CONbits.DC1B = 0;
            CCP2CONbits.DC2B = 0;
        }
    } else if (PIR1bits.TXIF) {
        PIR1bits.TXIF = 0;
        if(TEXTE_UART[POS_UART] != '\0') {
            TXREG = TEXTE_UART[POS_UART];
            POS_UART++;
        } else PIE1bits.TXIE = 0;
    }
}


#pragma interrupt LowISR
void LowISR (void){
    if (PIR1bits.ADIF) {
        PIR1bits.ADIF = 0;
        VOLTAGE = ADRESH;
    }
}

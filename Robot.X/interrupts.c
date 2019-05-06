#include <p18f2520.h>

/* High-priority interrupt routine */
#pragma code high_isr=0x08
#pragma interrupt high_isr
void high_isr(void)
{
}

/* Low-priority interrupt routine */
#pragma code low_isr=0x18
#pragma interruptlow low_isr
void low_isr(void)
{
}

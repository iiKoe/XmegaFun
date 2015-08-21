#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 2000000UL


ISR(TCC0_OVF_vect)
{
	PORTE.OUTTGL = PIN0_bm;															// Just toggle LED 
}

int main (void)
{
	
	PORTE.DIRSET = PIN0_bm;
	PORTE.OUTSET = PIN0_bm;
	
	TCC0.PER = 15625;																// Tel tot 15625 ipv 65536 ( 2000000 / 65536  /2 = 15,... dus prescailer = 64) 2000000/64/2 = 15625
	TCC0.CTRLA =  TC_CLKSEL_DIV64_gc;												// Prescailer 64
	TCC0.INTCTRLA =  TC_OVFINTLVL_LO_gc;											// Maak het een LOW level interrupt
	
	PMIC.CTRL |= PMIC_LOLVLEN_bm;													// Enable LOW level interrupt
	sei();
	
	while(1) {}

}

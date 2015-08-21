#include <avr/io.h>
#include <avr/interrupt.h>

int main (void)
{
	
	PORTF.DIRSET = PIN0_bm;
	//PORTE.DIRCLR = PIN0_bm;				// Zet DIR bit 1 van PORTE als input (knop SW1)
	
	
	PORTE.INTCTRL = PORT_INT0LVL_MED_gc;	// Maak het een MEDIUM level interrupt
	PORTE.INT0MASK = PIN6_bm;				// Welke pin voor de interne interrupt
	PORTE.PIN6CTRL = PORT_ISC_RISING_gc;	// Zowel rising als faling edge trigger
	
	sei();
	PMIC.CTRL = PMIC_MEDLVLEN_bm;			// Enable MEDIUM interrupts
	
	PORTF.OUTSET = PIN0_bm;					// Maak pin0 van R Hoog
	
	while(1);								
	
}

ISR(PORTE_INT0_vect)
{
	PORTF.OUTTGL = PIN0_bm;					//Toggle de LED
}
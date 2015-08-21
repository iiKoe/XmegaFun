#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 2000000UL
#include <util/delay.h>



int main (void)
{
	PORTC_DIRSET = PIN0_bm;									//Output voor pwm, Timer0 kan pin 0-3 als output gebruiken, Timer1 4-5
	TCC0.CTRLA = TC_CLKSEL_DIV1_gc;							// De clock source is de clock
	TCC0.PER = 255;											// Bepaald de resolutie
	TCC0.CTRLB = TC_WGMODE_DSBOTTOM_gc | TC0_CCAEN_bm;		// De PWM mode (Dual Slope) en enable compare channel
	TCC0.CCA =10;											// Zet de pwm begin waarde
	sei();													// Enable global interrupts
	
	uint16_t i;												// Variabele voor het verhogen van de PWM waarde (0-255)
	
	while(1){
		
		for(i=10;i<250;i=i+10){
			TCC0_CCA = i;									//De waarde die het PWM signaal moet worden
			_delay_ms(15);
		}
		for(i=250;i>10;i=i-10){
			TCC0_CCA = i;									//De waarde die het PWM signaal moet worden
			_delay_ms(15);
		}	
	}
}

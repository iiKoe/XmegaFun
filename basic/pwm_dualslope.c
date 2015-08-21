#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 32000000UL
#include <util/delay.h>


void setClockTo32MHz() {
	CCP = CCP_IOREG_gc;              // disable register security for oscillator update
	OSC.CTRL = OSC_RC32MEN_bm;       // enable 32MHz oscillator
	while(!(OSC.STATUS & OSC_RC32MRDY_bm)); // wait for oscillator to be ready
	CCP = CCP_IOREG_gc;              // disable register security for clock update
	CLK.CTRL = CLK_SCLKSEL_RC32M_gc; // switch to 32MHz clock
}



int main (void)
{
	setClockTo32MHz();
	PORTE_DIRSET = PIN0_bm;									//Output voor pwm, Timer0 kan pin 0-3 als output gebruiken, Timer1 4-5
	TCE0.CTRLA = TC_CLKSEL_DIV1_gc;							// De clock source is de clock
	TCE0.PER = 0xFFFF;										// Maximale waarde (voor resolutie)
	TCE0.CTRLB = TC_WGMODE_DSBOTTOM_gc | TC0_CCAEN_bm;		// De PWM mode (Dual Slope) en enable compare channel
	TCE0.CCA = 200;											// Zet de pwm begin waarde
	sei();													// Enable global interrupts
	
	uint16_t i;												// Variabele voor het verhogen van de PWM waarde (0-65530)
	
	while(1){
		
		for(i=200;i<59000;i=i+100){
			TCE0_CCABUF = i;									//De waarde die het PWM signaal moet worden
			_delay_ms(2);
		}
		for(i=59000;i>200;i=i-100){
			TCE0_CCABUF = i;									//De waarde die het PWM signaal moet worden
			_delay_ms(2);
		}	
	}
}

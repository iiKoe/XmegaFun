/*
 * BLDC_beta1.c
 *
 * Created: 27-11-2012 10:10:42
 *  Author: Vito
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 32000000UL
#include <util/delay.h>
#include "port_config.h"
#include "bldc_dvr.h"
#include "awex_conf.h"
#include "ac_config.h"
#include "usart_driver.h"


void setClockTo32MHz(void) {
	CCP = CCP_IOREG_gc;						// disable register security for oscillator update
	OSC.CTRL = OSC_RC32MEN_bm;				// enable 32MHz oscillator
	while(!(OSC.STATUS & OSC_RC32MRDY_bm)); // wait for oscillator to be ready
	CCP = CCP_IOREG_gc;						// disable register security for clock update
	CLK.CTRL = CLK_SCLKSEL_RC32M_gc;		// switch to 32MHz clock
}


ISR(PORTF_INT0_vect)
{
	//_delay_ms(10);
	static uint16_t speed = 4000;
	if		((BLDC_SPEED_ADJUST() == PIN1_bm) & (speed<MAX_SPEED))	speed += 50;
	else if ((BLDC_SPEED_ADJUST() == PIN2_bm) & (speed>MIN_SPEED))  speed -= 50;
	TCC0.CCA = speed;
}

int main(void)
{

	setClockTo32MHz();
	setupUsart();
	setup_ticker();
	TRANSISTOR_CONFIG();
	AC_SETUP;
	bldc_forward();
	
	//setup_speed_interrupt();
	TCC0_init();
	awex_init();
	
	PMIC.CTRL |= PMIC_MEDLVLEN_bm | PMIC_LOLVLEN_bm;
	sei();
	
	bldc_run();
	
	USART_Puts("\nStart Routine \n");
	
    while(1)
    {  
		bldc_main_routine();		
	}
}


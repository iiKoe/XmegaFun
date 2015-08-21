/*
 * SkyBin.c
 *
 * Created: 10-3-2013 14:21:42
 *  Author: Vito
 */ 


#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <stdint.h>
#define F_CPU 32000000UL
#include <util/delay.h>
#include "UART/usart_driver.h"
#include "skyBin/skyBin_driver.h"


void printAll_UART(void);


void setClockTo32MHz(void) {
	CCP = CCP_IOREG_gc;						// disable register security for oscillator update
	OSC.CTRL = OSC_RC32MEN_bm;				// enable 32MHz oscillator
	while(!(OSC.STATUS & OSC_RC32MRDY_bm)); // wait for oscillator to be ready
	CCP = CCP_IOREG_gc;						// disable register security for clock update
	CLK.CTRL = CLK_SCLKSEL_RC32M_gc;		// switch to 32MHz clock
}

int main(void)
{
	setClockTo32MHz();
	USART_Init();
	
	PORTE.DIRSET = PIN0_bm;
	sei();
	
    while(1)
    {
		
		skyBin_fillBuffer();

	}		
}


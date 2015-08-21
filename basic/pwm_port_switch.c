/*
 * PWM_PORT_SWITCH.c
 *
 * Created: 29-11-2012 16:00:16
 *  Author: Vito
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 32000000UL
#include <util/delay.h>

#define TC_GetOverflowFlag( _tc ) ( (_tc)->INTFLAGS & TC0_OVFIF_bm )
#define TC_ClearOverflowFlag( _tc ) ( (_tc)->INTFLAGS = TC0_OVFIF_bm )



void TCC0_init()
{
	TCC0.CTRLB	|= (TC_WGMODE_SS_gc | TC0_CCAEN_bm);				// Set WGM Single Slope 
	TCC0.PER   	 =  60000;
	
	PORTC.DIRSET = 0xFF;
	//PORTC.OUTSET = 0xFF;
	
	TCC0.CTRLA	|= TC_CLKSEL_DIV1_gc;
}


uint8_t GetNewPattern(uint8_t pattern_index)
{
	
	uint8_t new_pattern=0;
	
	switch(pattern_index)
	{
		
		case 0:
		new_pattern = 0x0F;
		break;
		case 1:
		new_pattern = 0x3C;
		break;
		case 2:
		new_pattern = 0xF0;
		break;
		case 3:
		new_pattern = 0xC3;
		break;
	}

	return new_pattern;
}

void setClockTo32MHz() {
	CCP = CCP_IOREG_gc;              // disable register security for oscillator update
	OSC.CTRL = OSC_RC32MEN_bm;       // enable 32MHz oscillator
	while(!(OSC.STATUS & OSC_RC32MRDY_bm)); // wait for oscillator to be ready
	CCP = CCP_IOREG_gc;              // disable register security for clock update
	CLK.CTRL = CLK_SCLKSEL_RC32M_gc; // switch to 32MHz clock
}


int main(void)
{
	
	setClockTo32MHz();
	
	static volatile int16_t pwm_delta = 300;
	static volatile uint8_t pattern_index=0;
	static volatile uint8_t new_pattern=0;
	
	
	AWEXC.CTRL |= AWEX_DTICCDEN_bm  | AWEX_DTICCCEN_bm | AWEX_DTICCBEN_bm | AWEX_DTICCAEN_bm | AWEX_PGM_bm;
	AWEXC.OUTOVEN	 = 0x0F;
	
	TCC0_init();
	
	sei(); 
	

	 while(1)
	  {
     
		/* Check if switch is pressed */
    
		if((PORTE.IN & PIN5_bm) == 0)
		{     
		  _delay_ms(500);


		  pattern_index++;
		  pattern_index = (pattern_index & 0x03);  //we only use 2 bits for the mask 
      
      
		  new_pattern = GetNewPattern(pattern_index); 
      
		  AWEXC.DTLSBUF =  new_pattern;
		}
     
		/* Check is overflow flag (OVFIF) is set,
		 * clear flag and set a new duct cycle
		 */
       
		 if(TC_GetOverflowFlag(&TCC0) != 0)
		 {
			  TC_ClearOverflowFlag(&TCC0);      //Clear the IF by writing a logical 1 to the flag
         
			  if(TCC0.CCA >= 59000){         //Some "random" values above 0 and below TOP is selected
				pwm_delta = -300;           //for the PWM changes, and make the LED look ok.
			  }
			  else if(TCC0.CCA <= 5000){
				pwm_delta = +300;
			  }
          
			  TCC0.CCABUF += pwm_delta;             //Change the compare value to change duty cycle
		 }
     
	  }
}
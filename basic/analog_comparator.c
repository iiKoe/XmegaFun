#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 2000000UL
#include <util/delay.h>

int main(void)
{
	PORTF.DIR |= 0xFF;
	PORTF.OUT = 0xFF;
	PORTF.OUT &= ~PIN2_bm;
	
	ACA.AC0CTRL		=    AC_INTMODE_BOTHEDGES_gc | AC_INTLVL_LO_gc;
	ACA.AC0CTRL		=	(ACA.AC0CTRL  & ~ AC_HYSMODE_gm) | 	AC_HYSMODE_SMALL_gc;		// enable Hysteresis
	ACA.AC0MUXCTRL  =	(ACA.AC0MUXCTRL & ~ (AC_MUXPOS_gm | AC_MUXNEG_gm)) | 
						(AC_MUXPOS_PIN0_gc | AC_MUXNEG_PIN1_gc);				
	ACA.AC0CTRL		=	(ACA.AC0CTRL  & ~ AC_HSMODE_bm ) |  AC_ENABLE_bm;				// enable AC0	
			
	PMIC.CTRL |= PMIC_LOLVLEN_bm;
	sei();					
	
	
	
    while(1)
    {
        PORTF.OUTTGL = PIN1_bm;
		_delay_ms(500);
    }
}

ISR(ACA_AC0_vect)
{
	if(ACA.STATUS & AC_AC0STATE_bm){
		PORTF.OUT &= ~PIN0_bm;
	}
	else{
		PORTF.OUT |= PIN0_bm;
	}
}
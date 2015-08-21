/*
 * awex_conf.c
 *
 * Created: 9-12-2012 19:45:31
 *  Author: Vito
 */ 

#include "awex_conf.h"

void awex_init(void)
{
	AWEXC.CTRL		|= AWEX_DTICCAEN_bm | AWEX_DTICCBEN_bm | AWEX_DTICCCEN_bm | AWEX_PGM_bm;
	AWEXC.OUTOVEN	 = AWEX_BEGINVAL;
}


void TCC0_init(void)
{
	TCC0.CTRLB	|=	(TC_WGMODE_DS_B_gc | TC0_CCAEN_bm);				// Set WGM Single Slope
	TCC0.PER   	 =	TOP_VALUE;
	PORTC.DIRSET =	AWEX_PORTS;
	TCC0.CTRLA	|=	TC_CLKSEL_DIV1_gc;
	TCC0.CCA	 =	TIMER_BEGINCOMPARE;
}

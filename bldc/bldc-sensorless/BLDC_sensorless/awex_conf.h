/*
 * awex_conf.h
 *
 * Created: 9-12-2012 19:45:49
 *  Author: Vito
 */ 


#ifndef AWEX_CONF_H_
#define AWEX_CONF_H_

#include <avr/interrupt.h>

#define TOP_VALUE			900
#define TIMER_BEGINCOMPARE	100
#define AWEX_PORTS			0xFF
#define AWEX_BEGINVAL		0x00

#define MAX_SPEED			800
#define MIN_SPEED			100


void awex_init(void);

void TCC0_init(void);



#endif /* AWEX_CONF_H_ */
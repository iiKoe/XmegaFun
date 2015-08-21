/*
 * Configuration file for BLDC driver
 * By: Vito
 */
#ifndef __BLDC_CONFIG__
#define __BLDC_CONFIG__

#include <avr/io.h>
#include <stdio.h>

#define F_CPU 32000000UL

/* Enable dedugging */
#define BLDC_DEBUG

/* Number of samples in sine table */
#define BLDC_SINE_SAMPLES 512

/* Usart output, from:
 * driver/usart/jdb_serial.c */
extern FILE gComm485_IO;
extern FILE gCtrl_IO;

#endif /*__BLDC_CONFIG__*/

/*
 * Setting for the XMEGA clock
 * By: Vito
 */
#include <avr/io.h>
#include "clock.h"

void setClockTo32MHz(void)
{
    CCP = CCP_IOREG_gc;                         // disable register security for oscillator update
    OSC.CTRL = OSC_RC32MEN_bm;                  // enable 32MHz oscillator
    while(!(OSC.STATUS & OSC_RC32MRDY_bm));     // wait for oscillator to be ready
    CCP = CCP_IOREG_gc;                         // disable register security for clock update
    CLK.CTRL = CLK_SCLKSEL_RC32M_gc;            // switch to 32MHz clock
}


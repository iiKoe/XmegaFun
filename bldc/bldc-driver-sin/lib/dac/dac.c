/*
 * Library for using the xmega DAC
 *
 * By: Vito
 */
#include <avr/io.h>
#include <inttypes.h>

#include "dac.h"


/*
 * Init DAC channel single mode.
 */
void dac_single_init(volatile DAC_t *dac, DAC_REFSEL_t ref, DAC_REFRESH_t rate)
{
    dac->CTRLB      = DAC_CHSEL_SINGLE_gc;
    dac->CTRLC      = ref;
    dac->CTRLA      = DAC_CH0EN_bm | DAC_ENABLE_bm;
    dac->TIMCTRL    = rate;
}

/*
 * Init DAC channel dual mode.
 */
void dac_dual_init(volatile DAC_t *dac, DAC_REFSEL_t ref, DAC_REFRESH_t rate)
{
    dac->CTRLB      = DAC_CHSEL_DUAL_gc;
    dac->CTRLC      = ref;
    dac->CTRLA      = DAC_CH0EN_bm | DAC_CH1EN_bm | DAC_ENABLE_bm;
    dac->TIMCTRL    = rate;
}

/*
 * Set DAC trigger event.
 */
void dac_set_event(volatile DAC_t *dac, DAC_EVSEL_t event, uint8_t ch_trig)
{
    dac->EVCTRL = event;
    dac->CTRLB |= ch_trig;
}

/*
 * Set DAC value.
 */
void dac_set_value(volatile DAC_t *dac, uint16_t value)
{
    if (value > DAC_MAX)
        return;

    while(!(dac->STATUS & DAC_CH0DRE_bm));
    dac->CH0DATAL = (uint8_t)(value&0xFF);
    dac->CH0DATAH = (uint8_t)(value>>8);
}

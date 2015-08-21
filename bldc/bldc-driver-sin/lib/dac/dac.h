/*
 * Library for usign the xmega DAC
 */
#ifndef __DAC_H__
#define __DAC_H__

#define DAC_MAX     4095

void dac_single_init(volatile DAC_t *dac, DAC_REFSEL_t ref, DAC_REFRESH_t rate);
void dac_set_event(volatile DAC_t *dac, DAC_EVSEL_t event, uint8_t ch_trig);
void dac_set_value(volatile DAC_t *dac, uint16_t value);

#endif /*__DAC_H__*/

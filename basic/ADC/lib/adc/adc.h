/*
 * Library for using the Xmega ADC
 * By: Vito
 */
#ifndef __ADC_H__
#define __ADC_H__

#include <avr/io.h>

struct sADC_Res {
    volatile ADC_CH_t *channel;

    int16_t offset;
    int16_t raw_res;
    int16_t result;
};

void adc_enable(volatile ADC_t *adc, bool enable);
void adc_ch_set_input(volatile ADC_CH_t *adc_ch, ADC_CH_MUXPOS_t pos, ADC_CH_MUXNEG_t neg);
void adc_ch_set_input_mode(volatile ADC_CH_t *adc_ch, ADC_CH_INPUTMODE_t inp_mode);
//void adc_ch_set_input_mode(volatile ADC_CH_t *adc_ch, ADC_CH_GAIN_t gain, ADC_CH_INPUTMODE_t inp_mode);
//void adc_set_currlimit(volatile ADC_t *adc, ADC_CURRLIMIT_t curr_lim);
void adc_set_freerunning(volatile ADC_t *adc, bool freerun, ADC_SWEEP_t channels);
void adc_set_resolution(volatile ADC_t *adc, ADC_RESOLUTION_t res);
void adc_set_conversion_mode(volatile ADC_t *adc, bool signed_mode);
void adc_set_reference(volatile ADC_t *adc, ADC_REFSEL_t ref);
void adc_set_sample_rate(volatile ADC_t *adc, ADC_PRESCALER_t scale);
int16_t adc_ch_get_result(volatile ADC_CH_t *adc_ch);

uint8_t adc_read_cal_byte(uint8_t index);
void adc_load_callibration(void);

void adc_set_zero(struct sADC_Res *adc);
void adc_set_offset(struct sADC_Res *adc, int16_t offset);
int16_t adc_get_result(struct sADC_Res *adc);

#endif /*__ADC_H__*/

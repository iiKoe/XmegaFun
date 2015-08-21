/*
 * This is a driver for using the BLDC ADC for measurements.
 *
 * By: Vito 
 */
#include "bldc_config.h"
#include <stdio.h>
#include <avr/io.h>
#include "util/delay.h"

#include "lib/adc/adc.h"
#include "lib/delay/jdb_delay_timer.h"
#include "driver/measure/measure.h"

struct sADC_Res ADC_Res_U;
struct sADC_Res ADC_Res_V;
struct sADC_Res ADC_Res_Volt;

/*
 * Initialize the ADC for taking meshurement on the BLDC driver.
 */
void me_adc_init(void)
{
    adc_load_calibration();

    adc_ch_set_input(&ADC_CH_U, ADC_U_MUXPOS, ADC_U_MUXNEG);
    adc_ch_set_input(&ADC_CH_V, ADC_V_MUXPOS, ADC_V_MUXNEG);
    adc_ch_set_input(&ADC_CH_VOLT, ADC_VOLT_MUXPOS, ADC_VOLT_MUXNEG);

    adc_ch_set_input_mode(&ADC_CH_U, ADC_U_INP_MODE);
    adc_ch_set_input_mode(&ADC_CH_V, ADC_V_INP_MODE);
    adc_ch_set_input_mode(&ADC_CH_VOLT, ADC_VOLT_INP_MODE);

    adc_set_freerunning(&ADCA, true, ADCA_SWEEP);
    adc_set_freerunning(&ADCB, true, ADCB_SWEEP);

    adc_set_resolution(&ADCA, ADC_RESOLUTION);
    adc_set_resolution(&ADCB, ADC_RESOLUTION);

    adc_set_conversion_mode(&ADCA, true);
    adc_set_conversion_mode(&ADCB, true);

    adc_set_reference(&ADCA, ADC_REFERENCE);
    adc_set_reference(&ADCB, ADC_REFERENCE);

    adc_set_sample_rate(&ADCA, ADC_PRESCALER);
    adc_set_sample_rate(&ADCB, ADC_PRESCALER);

    adc_enable(&ADCA, true);
    adc_enable(&ADCB, true);

    _delay_ms(5);

    me_adc_set_channel(&ADC_Res_U, &ADC_CH_U);
    me_adc_set_channel(&ADC_Res_V, &ADC_CH_V);
    me_adc_set_channel(&ADC_Res_Volt, &ADC_CH_VOLT);

    me_adc_set_zero(&ADC_Res_U);
    me_adc_set_zero(&ADC_Res_V);
    //adc_set_zero(&ADC_Res_Volt);
}

/* Set the ADC channel. */
void me_adc_set_channel(struct sADC_Res *adc, volatile ADC_CH_t *adc_ch)
{
    adc->channel = adc_ch;
}

/* Set ADC offset to zero */
void me_adc_set_zero(struct sADC_Res *adc)
{
    /* TODO: This isn't really the way to determine this.. but yea. */
    me_adc_get_result(adc);
    me_adc_set_offset(adc, adc->raw_res * -1);
}

/* Set ADC offset */
void me_adc_set_offset(struct sADC_Res *adc, int16_t offset)
{
    adc->offset = offset;
}

/* Get the result of the ADC */
int16_t me_adc_get_result(struct sADC_Res *adc)
{
    adc->raw_res = adc_ch_get_result(adc->channel);
    adc->result = adc->raw_res + adc->offset;

    return adc->result;
}

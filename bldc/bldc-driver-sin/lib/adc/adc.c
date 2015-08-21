/*
 * Library for using the Xmega ADC
 * By: Vito
 */
#include <avr/io.h>
#include <stddef.h>
#include <avr/pgmspace.h>
#include <inttypes.h>
#include <stdbool.h>

#include "adc.h"


/*
 * Ebable the ADC.
 */
void adc_enable(volatile ADC_t *adc, bool enable)
{
    if (enable)
        adc->CTRLA |= ADC_ENABLE_bm;
    else
        adc->CTRLA &= ~ADC_ENABLE_bm;
}

/*
 * Set ADC channel input.
 */
void adc_ch_set_input(volatile ADC_CH_t *adc_ch, ADC_CH_MUXPOS_t pos, ADC_CH_MUXNEG_t neg)
{
    adc_ch->MUXCTRL &= ~(ADC_CH_MUXPOS_gm | ADC_CH_MUXNEG_gm);
    adc_ch->MUXCTRL |= pos | neg;
}

#if 0
void adc_ch_set_input_mode(volatile ADC_CH_t *adc_ch, ADC_CH_GAIN_t gain, ADC_CH_INPUTMODE_t inp_mode)
{
    adc_ch->CTRL &= ~(ADC_CH_GAIN_gm | ADC_CH_INPUTMODE_gm);
    adc_ch->CTRL |= gain | inp_mode;
}
#endif

/*
 * Set ADC channel input mode.
 */
void adc_ch_set_input_mode(volatile ADC_CH_t *adc_ch, ADC_CH_INPUTMODE_t inp_mode)
{
    adc_ch->CTRL &= ~ADC_CH_INPUTMODE_gm;
    adc_ch->CTRL |= inp_mode;
}

#if 0
void adc_set_currlimit(volatile ADC_t *adc, ADC_CURRLIMIT_t curr_lim)
{
    adc->CTRLB &= ~ADC_CURRLIMIT_gm;
    adc->CTRLB |= curr_lim;
}
#endif

/*
 * Enable or disable freerunning on ADC.
 */
void adc_set_freerunning(volatile ADC_t *adc, bool freerun, ADC_SWEEP_t channels)
{
    adc->EVCTRL &= ~ADC_SWEEP_gm;
    adc->EVCTRL |= channels;

    if (freerun)
        adc->CTRLB |= ADC_FREERUN_bm;
    else
        adc->CTRLB &= ~ADC_FREERUN_bm;
}

/*
 * Set ADC resolution.
 */
void adc_set_resolution(volatile ADC_t *adc, ADC_RESOLUTION_t res)
{
    adc->CTRLB &= ~ADC_RESOLUTION_gm;
    adc->CTRLB |= res;
}

/*
 * Set ADC conversion mode.
 */
void adc_set_conversion_mode(volatile ADC_t *adc, bool signed_mode)
{
    if (signed_mode)
        adc->CTRLB |= ADC_CONMODE_bm;
    else
        adc->CTRLB &= ~ADC_CONMODE_bm;
}

/*
 * Set ADC reference.
 */
void adc_set_reference(volatile ADC_t *adc, ADC_REFSEL_t ref)
{
    adc->REFCTRL &= ~ADC_REFSEL_gm;
    adc->REFCTRL |= ref;
}

/*
 * Set ADC sample rate.
 */
void adc_set_sample_rate(volatile ADC_t *adc, ADC_PRESCALER_t scale)
{
    adc->PRESCALER &= ~ADC_PRESCALER_gm;
    adc->PRESCALER |= scale;
}

/*
 * Get ADC channel result.
 */
int16_t adc_ch_get_result(volatile ADC_CH_t *adc_ch)
{
    return adc_ch->RES;
}

/*
 * Read ADC calibration byte.
 */
uint8_t adc_read_cal_byte(uint8_t index)
{
    uint8_t result;

    NVM_CMD = NVM_CMD_READ_CALIB_ROW_gc;
    result = pgm_read_byte(index);
    NVM_CMD = NVM_CMD_NO_OPERATION_gc;

    return(result);
}

/*
 * Load ADC calibration.
 */
void adc_load_calibration(void)
{
    ADCA.CALL = adc_read_cal_byte(offsetof(NVM_PROD_SIGNATURES_t, ADCACAL0));
    ADCA.CALH = adc_read_cal_byte(offsetof(NVM_PROD_SIGNATURES_t, ADCACAL1));

    ADCB.CALL = adc_read_cal_byte(offsetof(NVM_PROD_SIGNATURES_t, ADCBCAL0));
    ADCB.CALH = adc_read_cal_byte(offsetof(NVM_PROD_SIGNATURES_t, ADCBCAL1));
}


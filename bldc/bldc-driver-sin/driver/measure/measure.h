/*
 * This is a driver for using the BLDC ADC for measurements.
 * By: Vito 
 */
#ifndef __MEASURE_H__
#define __MEASURE_H__

struct sADC_Res {
    volatile ADC_CH_t *channel;

    int16_t offset;
    int16_t raw_res;
    int16_t result;
};

#define ADC_CH_U            ADCA.CH0
#define ADC_CH_V            ADCB.CH0
#define ADC_CH_VOLT         ADCA.CH1

#define ADC_U_INP_MODE      ADC_CH_INPUTMODE_DIFF_gc
#define ADC_V_INP_MODE      ADC_CH_INPUTMODE_DIFF_gc
#define ADC_VOLT_INP_MODE   ADC_CH_INPUTMODE_SINGLEENDED_gc

#define ADC_U_MUXPOS        ADC_CH_MUXPOS_PIN2_gc
#define ADC_U_MUXNEG        ADC_CH_MUXNEG_PIN1_gc
#define ADC_V_MUXPOS        ADC_CH_MUXPOS_PIN0_gc
#define ADC_V_MUXNEG        ADC_CH_MUXNEG_PIN1_gc
#define ADC_VOLT_MUXPOS     ADC_CH_MUXPOS_PIN3_gc
#define ADC_VOLT_MUXNEG     0b111 /* Not used in single ended */

#define ADCA_SWEEP          ADC_SWEEP_01_gc
#define ADCB_SWEEP          ADC_SWEEP_0_gc

#define ADC_RESOLUTION      ADC_RESOLUTION_12BIT_gc
#define ADC_REFERENCE       ADC_REFSEL_AREFA_gc
#define ADC_PRESCALER       ADC_PRESCALER_DIV128_gc

void me_adc_set_channel(struct sADC_Res *adc, volatile ADC_CH_t *adc_ch);
void me_adc_set_zero(struct sADC_Res *adc);
void me_adc_set_offset(struct sADC_Res *adc, int16_t offset);
int16_t me_adc_get_result(struct sADC_Res *adc);

extern struct sADC_Res ADC_Res_U;
extern struct sADC_Res ADC_Res_V;
extern struct sADC_Res ADC_Res_Volt;

#endif /*__MEASURE_H__*/

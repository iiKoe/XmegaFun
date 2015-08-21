#define F_CPU 32000000UL
#include <stdio.h>
#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>
#include <stddef.h>

#include "lib/clock/clock.h"
//#include "driver/usart/serial.h"
#include "lib/adc/adc.h"

#define ADC_CH_MUXNEG_INTGND_gc (0b111<<0)

#define ADC_CH ADCA.CH0

static struct sADC_Res ADC_ch_u;

void adc_init(void)
{
    adc_load_calibration();

    adc_ch_set_input(&ADC_CH, ADC_CH_MUXPOS_PIN2_gc, ADC_CH_MUXNEG_PIN1_gc);
    adc_ch_set_input_mode(&ADC_CH, ADC_CH_INPUTMODE_DIFF_gc);

    adc_set_freerunning(&ADCA, true, ADC_SWEEP_0_gc);
    adc_set_resolution(&ADCA, ADC_RESOLUTION_12BIT_gc);
    adc_set_conversion_mode(&ADCA, true);
    adc_set_reference(&ADCA, ADC_REFSEL_AREFA_gc);
    adc_set_sample_rate(&ADCA, ADC_PRESCALER_DIV128_gc);

    adc_enable(&ADCA, true);

    _delay_ms(10);
    ADC_ch_u.channel = &ADC_CH;
    adc_set_zero(&ADC_ch_u);
}

int main(void)
{
    int16_t adc_res;

    setClockTo32MHz();
    InitDelayTimer();
    //InitSerial();

    sei();

    adc_init();

    _delay_ms(2000);
    //printf("Initializing ADC.\r\n\tOffset ADC: %d\r\n", ADC_ch_u.offset);

    while(1) {
        _delay_ms(1000);
        adc_res = adc_get_result(&ADC_ch_u);
        //printf("ADC value: %d\r\n", adc_res);
    }
}

/*
 * Main program code for the BLDC driver.
 * By: Vito
 *
 * This code is intended to drive a BLDC driver
 * The code is not finished and needs some extending and cleaning up.
 *
 */
#include "bldc_config.h"

#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "lib/adc/adc.h"
#include "lib/clock/clock.h"
#include "lib/dac/dac.h"
#include "lib/sinew/sine_wave.h"
#include "driver/usart/jdb_serial.h"
#include "driver/debug/debug.h"
#include "driver/hall/hall_state.h"
#include "driver/measure/measure.h"
#include "driver/timer/update_timer.h"

/* Global pointer to the soon to be generated sine table. */
uint16_t *SineTable;

/* Can be disabled for debugging purposes. */
#define START_BLDC

/*
 * Print all kind of status information, for debugging purposes.
 */
void print_stats(void)
{

    DEBUG_PRINTF(
            "Status:\r\n"

            "\tTransfer count: %u\r\n"
            "\tHallState %d\r\n"
            "\tSineState: %d\r\n"
            "\tSineStart: %u\r\n"
            "\tPeriod: %u\r\n"
            "\tDMA_cnt: %u\r\n"
            "\tSampleEnded: %u\r\n"
            "\tDMA_int_cnt: %u\r\n"
            "\tDacValue_U: %u\r\n"
            "\tDacValue_V: %u\r\n"

            "\tADC_U_val: %d\r\n"
            "\tADC_V_val: %d\r\n"
            "\tADC_Volt: %d\r\n"
            "\tHall Port: %d\r\n"
            ,
            BLDC_DMA_V.TRFCNT,
            BLDC_status.hall_state,
            BLDC_status.sine_state.V,
            (BLDC_status.sine_state.sample_start_V - SineTable),
            BLDC_status.period.PER,
            dma_get_cnt(&BLDC_DMA_U),
            BLDC_status.sine_state.sample_ended,
            BLDC_status.intr.dma,
            BLDC_DAC.CH0DATA,
            BLDC_DAC.CH1DATA,

            me_adc_get_result(&ADC_Res_U),
            me_adc_get_result(&ADC_Res_V),
            me_adc_get_result(&ADC_Res_Volt),
            hs_get_hall()
            );
}

#define START_PER 200
int main(void)
{
    /* Currently the internal 32MHz clock is used. */
    setClockTo32MHz();
    /* Delay timer for serial communication. */
    InitDelayTimer();
    /* Initialize serial communication to Host PC */
    InitSerial();

    /* Initialize the measurement ADC. */
    me_adc_init();

    /* Debugging LED's */
    PORTE.DIRSET = PIN0_bm;
    PORTE.OUTSET = PIN0_bm;

    /*
     * BLDC inits
     */
    /* Initialize the sine table used for the DAC control signal. */
    sinew_generate(&SineTable, BLDC_SINE_SAMPLES, 750, 2048);
    /* Setup the DMA transfer to move the sine values to the DAC when needed. */
    ut_update_dac_dma_init(SineTable, BLDC_SINE_SAMPLES);
    /* Initialize the hall sensors for determining the rotor location. */
    hs_hall_init();
    hs_init_w();

    /* Enable the needed interrupt levels. */
    PMIC.CTRL |= PMIC_HILVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_LOLVLEN_bm;
    /* Enable the actual interrupts. */
    sei();

#ifdef START_BLDC
    /* Set the preferred direction of the BLDC motor. */
    hs_set_direction(BLDC_BACKWARD);
    /* Compute the next hall state. */
    db_next_hall_state();
    /* Determine the sine table part needed for the next hall state. */
    hs_comp_sine_state();
    /* Set the actual sine start positions. */
    hs_comp_sine_start(SineTable);
    /* Set the starting period (force start). */
    BLDC_status.period.PER = START_PER;
    /* Also set the start period in the DAC. */
    ut_set_dac_update_per(START_PER);
    /* Set the source for DMA (the sine table portion needed for this state. */
    hs_set_dma_source();
#endif

    _delay_ms(1000);
    DEBUG_PRINTF("\r\n\r\nStarted the program\r\n");
    print_stats();
    _delay_ms(1000);
    //hs_set_w_hi();

#ifdef START_BLDC
    /* Update the W signal (this one is 1 or 0 depending on the other states). */
    hs_update_w();
    /* Start the DMA transfer to the DAC with the set update cycle. */
    ut_dma_start();
    /* Start the update timer for the DAC */
    ut_update_timer_start();
#endif

#if 0
    /*
     * Can be used to check the DMA, DAC and Sine generator.
     * You might want to delete this, is just for showing you a test example.
     */

    /* Fill up a sine table */
    DEBUG_PRINTF("Setting up the sine table\r\n");
    sinew_generate(&SineTable, 512, 1500, 2048);
    DEBUG_PRINTF("First couple sine values\r\n %u, %u, %u, %u\r\n", SineTable[0], SineTable[1], SineTable[2], SineTable[3]);

    DEBUG_PRINTF("Setting the dac to: %d\r\n", 2048);
    dac_single_init(&DACB, DAC_REFSEL_AVCC_gc, DAC_REFRESH_32CLK_gc);
    dac_set_value(&DACB, 2048);

    DEBUG_PRINTF("Enable DMA transfer to DAC\r\n");
    ut_update_dac_dma_init(SineTable, 512);
    ut_update_timer_start();
    ut_set_dac_update_per(30000);
#endif

    while(1) {
        DEBUG_PRINTF("In the loop\r\n");
        print_stats();

/* Manually set values for debugging. */
#if 0
        hs_set_w_hi();
        _delay_ms(5000);
        hs_set_w_lo();
        _delay_ms(5000);
#endif
        _delay_ms(2000);
    }
    return 0;
}

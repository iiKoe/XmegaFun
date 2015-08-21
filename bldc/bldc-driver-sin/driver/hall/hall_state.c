/*
 * Driver for the BLDC hall states.
 * Calculating and predicting the sine zero points.
 *
 * By: Vito
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>

#include "bldc_config.h"
#include "lib/dma/dma.h"
#include "driver/timer/update_timer.h"
#include "driver/hall/hall_state.h"
#include "driver/debug/debug.h"
#include "hall_state.h"

/* Global BLDC status */
struct sBLDC_stat BLDC_status;

volatile static uint16_t PerCnt;

/*
 * Initialize the hall sensor PIN's.
 */
void hs_hall_init(void)
{
#if 1
    HALL_PORT.DIRCLR = HALL_MASK;
    HALL_PORT.INTCTRL = HALL_INT_LVL;
    HALL_PORT.HALL_INT_MASK = HALL_MASK;
    HALL_PORT.HALL_PIN0C = HALL_PIN_CONF;
    HALL_PORT.HALL_PIN1C = HALL_PIN_CONF;
    HALL_PORT.HALL_PIN2C = HALL_PIN_CONF;
#endif
}

/*
 * Set a BLDC direction.
 */
void hs_set_direction(enum eRunningState state)
{
    BLDC_status.running_state = state;
}

/*
 * Change the hall state to the next one.
 */
void hs_update_hall_state(void)
{
    hall_t hall_val;

    /* Read the hall port */
    hall_val = hs_get_hall();

    switch (hall_val) {
        case HSTATE1:
           BLDC_status.hall_state = BLDC_HALL_1;
           break;
        case HSTATE2:
           BLDC_status.hall_state = BLDC_HALL_2;
           break;
        case HSTATE3:
           BLDC_status.hall_state = BLDC_HALL_3;
           break;
        case HSTATE4:
           BLDC_status.hall_state = BLDC_HALL_4;
           break;
        case HSTATE5:
           BLDC_status.hall_state = BLDC_HALL_5;
           break;
        case HSTATE6:
           BLDC_status.hall_state = BLDC_HALL_6;
           break;

        default:
           BLDC_status.hall_state = BLDC_HALL_UNKNOWN;
           break;
    }
}

/*
 * Update sine according to rotation
 */
int hs_comp_sine_state(void)
{
    if (BLDC_status.running_state == BLDC_FORWARD) {
        switch (BLDC_status.hall_state) {
            case BLDC_HALL_1:
                BLDC_status.sine_state.U = SINE_RISING_ZERO;
                BLDC_status.sine_state.V = SINE_FALLING_N_PEAK;
                BLDC_status.sine_state.W = BLDC_W_POS;
                break;
            case BLDC_HALL_2:
                BLDC_status.sine_state.U = SINE_RISING_P_PEAK;
                BLDC_status.sine_state.V = SINE_RISING_N_PEAK;
                BLDC_status.sine_state.W = BLDC_W_GO_NEG;
                break;
            case BLDC_HALL_3:
                BLDC_status.sine_state.U = SINE_FALLING_P_PEAK;
                BLDC_status.sine_state.V = SINE_RISING_ZERO;
                BLDC_status.sine_state.W = BLDC_W_NEG;
                break;
            case BLDC_HALL_4:
                BLDC_status.sine_state.U = SINE_FALLING_ZERO;
                BLDC_status.sine_state.V = SINE_RISING_P_PEAK;
                BLDC_status.sine_state.W = BLDC_W_NEG;
                break;
            case BLDC_HALL_5:
                BLDC_status.sine_state.U = SINE_FALLING_N_PEAK;
                BLDC_status.sine_state.V = SINE_FALLING_P_PEAK;
                BLDC_status.sine_state.W = BLDC_W_GO_POS;
                break;
            case BLDC_HALL_6:
                BLDC_status.sine_state.U = SINE_RISING_N_PEAK;
                BLDC_status.sine_state.V = SINE_FALLING_ZERO;
                BLDC_status.sine_state.W = BLDC_W_POS;
                break;

            default:
                BLDC_status.sine_state.U = SINE_UNKNOWN;
                BLDC_status.sine_state.V = SINE_UNKNOWN;
                BLDC_status.sine_state.W = BLDC_W_UNKNOWN;
                break;
        }

    } else if (BLDC_status.running_state == BLDC_BACKWARD) {
#if 1
        switch (BLDC_status.hall_state) {
            case BLDC_HALL_1:
                BLDC_status.sine_state.U = SINE_FALLING_P_PEAK;
                BLDC_status.sine_state.V = SINE_FALLING_N_PEAK;
                BLDC_status.sine_state.W = BLDC_W_GO_POS;
                break;
            case BLDC_HALL_2:
                BLDC_status.sine_state.U = SINE_FALLING_ZERO;
                BLDC_status.sine_state.V = SINE_RISING_N_PEAK;
                BLDC_status.sine_state.W = BLDC_W_POS;
                break;
            case BLDC_HALL_3:
                BLDC_status.sine_state.U = SINE_FALLING_N_PEAK;
                BLDC_status.sine_state.V = SINE_RISING_ZERO;
                BLDC_status.sine_state.W = BLDC_W_POS;
                break;
            case BLDC_HALL_4:
                BLDC_status.sine_state.U = SINE_RISING_N_PEAK;
                BLDC_status.sine_state.V = SINE_RISING_P_PEAK;
                BLDC_status.sine_state.W = BLDC_W_GO_NEG;
                break;
            case BLDC_HALL_5:
                BLDC_status.sine_state.U = SINE_RISING_ZERO;
                BLDC_status.sine_state.V = SINE_FALLING_P_PEAK;
                BLDC_status.sine_state.W = BLDC_W_NEG;
                break;
            case BLDC_HALL_6:
                BLDC_status.sine_state.U = SINE_RISING_P_PEAK;
                BLDC_status.sine_state.V = SINE_FALLING_ZERO;
                BLDC_status.sine_state.W = BLDC_W_NEG;
                break;

            default:
                BLDC_status.sine_state.U = SINE_UNKNOWN;
                BLDC_status.sine_state.V = SINE_UNKNOWN;
                BLDC_status.sine_state.W = BLDC_W_UNKNOWN;
                break;
        }
#endif

    } else {
        /* No direction specified. */
        return 0;
    }

    return 1;
}

/*
 * Compute the new update period based on the previous ones and the error.
 */
void hs_comp_new_update_period(void)
{
    static uint16_t period_avg;
    double sample_error;
    uint16_t per;

    sample_error = BLDC_SINE_SAMPLES_PER_COMP / BLDC_status.sine_state.sample_ended;
    //DEBUG_PRINTF("Sample error %u\r\n", (uint16_t)sample_error);
    per = (uint16_t)((double)BLDC_status.period.PER * sample_error);

    //period_avg = ((period_avg * (BLDC_NUMB_AVERIGE - 1)) + per) / BLDC_NUMB_AVERIGE;
    period_avg = per;

    BLDC_status.period.PER_new = period_avg;
    BLDC_status.period.PER_diff = BLDC_status.period.PER_new - BLDC_status.period.PER;
}


#define HS_SINE_START_SWITCH(_state, _base, _data) do {         \
    switch (_state) {                                           \
        case SINE_RISING_ZERO:                                  \
            _data = _base + (BLDC_SINE_SAMPLES_4_6);            \
            break;                                              \
        case SINE_RISING_P_PEAK:                                \
            _data = _base + (BLDC_SINE_SAMPLES_5_6);            \
            break;                                              \
        case SINE_FALLING_P_PEAK:                               \
            _data = _base + (BLDC_SINE_SAMPLES_0_6);            \
            break;                                              \
        case SINE_FALLING_ZERO:                                 \
            _data = _base + (BLDC_SINE_SAMPLES_1_6);            \
            break;                                              \
        case SINE_FALLING_N_PEAK:                               \
            _data = _base + (BLDC_SINE_SAMPLES_2_6);            \
            break;                                              \
        case SINE_RISING_N_PEAK:                                \
            _data = _base + (BLDC_SINE_SAMPLES_3_6);            \
            break;                                              \
        default:                                                \
            _data = NULL;                                       \
            break;                                              \
    }                                                           \
} while(0)

/*
 * Get the sine start (offset from the start of the table).
 */
int hs_comp_sine_start(uint16_t *data_base)
{
    uint16_t *data_u, *data_v;

    HS_SINE_START_SWITCH(BLDC_status.sine_state.U, data_base, data_u);
    HS_SINE_START_SWITCH(BLDC_status.sine_state.V, data_base, data_v);

    if (!data_u || !data_v)
        return 0;

    BLDC_status.sine_state.sample_start_U = data_u;
    BLDC_status.sine_state.sample_start_V = data_v;

    return 1;
}

/* For hall testing TODO*/
void db_next_hall_state(void);

/*
 * Function to update the sine and DMA transfer.
 * This is called in the Hall sensor interrupt routine.
 */
int hs_update_sine(void)
{
    /* Stop DMA */
    hs_stop_dma();

    /* Stop DAC update timer */
    ut_update_timer_stop();

    /* Keep DAC as last value? (hope not) TODO */

    /* Save the current DMA count */
    hs_update_dma_sample_ended();

    /* Get the current HALL state */
    //hs_update_hall_state(); TODO change for real
    db_next_hall_state();
    //DEBUG_PRINTF("Hall sensor state: %u\r\n", BLDC_status.hall_state);

    /* Update the sinus state */
    hs_comp_sine_state();
    if (!hs_comp_sine_start(SineTable))
        return 0;

    /* Compute new DAC update period */
    hs_comp_new_update_period();

    /* Apply the computed sine data */
    hs_set_dma_source();

    /* Update the DAC update period */
    hs_set_dac_timer_per();

    /* Check for W update */
    hs_update_w();

    /* Start it all again */
    ut_dma_start();
    ut_update_timer_start();

}

/* Test function */
void db_next_hall_state(void)
{
    switch(BLDC_status.hall_state) {
        case BLDC_HALL_1:
            BLDC_status.hall_state = BLDC_HALL_2;
            break;
        case BLDC_HALL_2:
            BLDC_status.hall_state = BLDC_HALL_3;
            break;
        case BLDC_HALL_3:
            BLDC_status.hall_state = BLDC_HALL_4;
            break;
        case BLDC_HALL_4:
            BLDC_status.hall_state = BLDC_HALL_5;
            break;
        case BLDC_HALL_5:
            BLDC_status.hall_state = BLDC_HALL_6;
            break;
        case BLDC_HALL_6:
            BLDC_status.hall_state = BLDC_HALL_1;
            break;

        default:
            BLDC_status.hall_state = BLDC_HALL_1;
            break;
    }
}


/*
 * ISR's
 */

/*
 * The ISR triggerd by a hall sensor state change.
 */
ISR(HALL_INT_VECT)
{
    BLDC_status.intr.hall += 1;
    BLDC_status.period_offset.offset_state = BLDC_PER_TO_LONG;

    hs_update_sine();

    PORTE.OUTTGL = PIN0_bm;
}

/*
 * Interrupt for W fase and PER short
 */
ISR(BLDC_W_TIMER_INT_VECT)
{
    ++PerCnt;

    if (BLDC_status.period_offset.offset_state == BLDC_PER_TO_SHORT) {
       if (PerCnt == BLDC_MAX_SLO_SAMPLES) {
           /* This is bad, the motor is not reaching the hall sensors */
       }

    } else if (PerCnt == BLDC_SINE_SAMPLES_PER_COMP) {
        switch (BLDC_status.sine_state.W) {
            case BLDC_W_GO_NEG:
                hs_set_w_lo();
                hs_stop_w_update_timer();
                PerCnt = 0;
                break;
            case BLDC_W_GO_POS:
                hs_set_w_hi();
                hs_stop_w_update_timer();
                PerCnt = 0;
                break;

            default:
                break;
        }
    }
}

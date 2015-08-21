/*
 * Driver for the BLDC hall states.
 * Calculating and predicting the sine zero points.
 *
 * By: Vito 
 */
#ifndef __HALL_STATE_H__
#define __HALL_STATE_H__

#include "bldc_config.h"
#include "driver/timer/update_timer.h"

#define BLDC_SINE_SAMPLES_BYTES (BLDC_SINE_SAMPLES * 2)

#define BLDC_SINE_SAMPLES_0_6 0
#define BLDC_SINE_SAMPLES_1_6 ((uint16_t)(BLDC_SINE_SAMPLES / 6.0))
#define BLDC_SINE_SAMPLES_2_6 ((uint16_t)(BLDC_SINE_SAMPLES / 6.0 * 2.0))
#define BLDC_SINE_SAMPLES_3_6 ((uint16_t)(BLDC_SINE_SAMPLES / 6.0 * 3.0))
#define BLDC_SINE_SAMPLES_4_6 ((uint16_t)(BLDC_SINE_SAMPLES / 6.0 * 4.0))
#define BLDC_SINE_SAMPLES_5_6 ((uint16_t)(BLDC_SINE_SAMPLES / 6.0 * 5.0))

#define BLDC_SINE_SAMPLES_ZERO_1 ((uint16_t)(BLDC_SINE_SAMPLES / 4))
#define BLDC_SINE_SAMPLES_ZERO_2 ((uint16_t)(BLDC_SINE_SAMPLES / 4 * 3))

#define BLDC_SINE_SAMPLES_PER_COMP (BLDC_SINE_SAMPLES_1_6)

#define HALL_PORT       PORTF
#define HALL_MASK       0b111
#define HALL_PIN0C      PIN0CTRL
#define HALL_PIN1C      PIN1CTRL
#define HALL_PIN2C      PIN2CTRL
#define HALL_INT_LVL    PORT_INT0LVL_MED_gc
#define HALL_INT_MASK   INT0MASK
#define HALL_PIN_CONF   (PORT_ISC_BOTHEDGES_gc | PORT_OPC_PULLUP_gc)
#define HALL_INT_VECT   PORTF_INT0_vect

#define HSTATE1 0b011
#define HSTATE2 0b010
#define HSTATE3 0b110
#define HSTATE4 0b100
#define HSTATE5 0b101
#define HSTATE6 0b001

#define BLDC_W_PORT             PORTD
#define BLDC_W_PIN              PIN7_bm
#define BLDC_W_TIMER            DAC_UPDATE_TIMER
#define BLDC_W_TIMER_INT_LVL    TC_OVFINTLVL_MED_gc
#define BLDC_W_TIMER_INT_VECT   TCC1_OVF_vect

#define BLDC_NUMB_AVERIGE       16

#define BLDC_MAX_SLO_SAMPLES    65000

typedef uint8_t hall_t;

enum eHallState {
    BLDC_HALL_UNKNOWN,
    BLDC_HALL_1,
    BLDC_HALL_2,
    BLDC_HALL_3,
    BLDC_HALL_4,
    BLDC_HALL_5,
    BLDC_HALL_6
};

enum eRunningState {
    BLDC_OFF,
    BLDC_ALIGN,
    BLDC_FORWARD,
    BLDC_BACKWARD
};

enum eSineState {
    SINE_UNKNOWN,
    SINE_RISING_ZERO,
    SINE_RISING_P_PEAK,
    SINE_FALLING_P_PEAK,
    SINE_FALLING_ZERO,
    SINE_FALLING_N_PEAK,
    SINE_RISING_N_PEAK
};

enum eWstate {
    BLDC_W_UNKNOWN,
    BLDC_W_GO_NEG,
    BLDC_W_NEG,
    BLDC_W_GO_POS,
    BLDC_W_POS,
};

struct sSineState {
    enum eSineState U;
    enum eSineState V;
    enum eWstate W;

    uint16_t sample_ended;

    uint16_t *sample_start_U;
    uint16_t *sample_start_V;
};

struct sBLDC_speed {
    uint16_t RPM;
    uint16_t RPS;
};

enum eBLDC_perOffState {
    BLDC_PER_OK,
    BLDC_PER_TO_SHORT,
    BLDC_PER_TO_LONG
};

struct sBLDC_perOff {
    int16_t error;
    enum eBLDC_perOffState offset_state;
};

struct sBLDC_per {
    uint16_t PER;
    uint16_t PER_new;

    int16_t PER_diff;
};

struct sBLDC_interrupt {
    uint16_t dma;
    uint16_t hall;
};


struct sBLDC_stat {
    /* General BLDC State */
    enum eRunningState running_state;
    struct sBLDC_speed speed;

    struct sBLDC_interrupt intr;

    /* U and V */
    enum eHallState hall_state;
    struct sBLDC_perOff period_offset;
    struct sBLDC_per period;
    struct sSineState sine_state;
};

/* The BLDC struct */
extern struct sBLDC_stat BLDC_status;
extern uint16_t *SineTable;

static inline hall_t hs_get_hall(void)
{
    return(HALL_PORT.IN & HALL_MASK);
}

static inline void hs_update_dma_sample_ended(void)
{
    uint16_t dma_cnt;

    dma_cnt = dma_get_cnt(&BLDC_DMA_U);
    dma_cnt /= 2;
    if (dma_cnt == 0 || dma_cnt == BLDC_SINE_SAMPLES_PER_COMP)
        dma_cnt = 1;

    BLDC_status.sine_state.sample_ended = (BLDC_SINE_SAMPLES_PER_COMP - dma_cnt) + 1;
}

static inline void hs_set_dac_timer_per(void)
{
    ut_set_dac_update_per(BLDC_status.period.PER_new);
    BLDC_status.period.PER = BLDC_status.period.PER_new;
}

static inline void hs_set_dma_source(void)
{
    ut_dma_new_source_u(BLDC_status.sine_state.sample_start_U);
    ut_dma_new_source_v(BLDC_status.sine_state.sample_start_V);
}

static inline void hs_resume_dma(void)
{
    dma_resume(&BLDC_DMA_U);
    dma_resume(&BLDC_DMA_V);
}

static inline void hs_stop_dma(void)
{
    dma_stop(&BLDC_DMA_U);
    dma_stop(&BLDC_DMA_V);
}

static inline void hs_start_per_short_timer(void)
{
    DAC_UPDATE_TIMER.INTCTRLA |= BLDC_W_TIMER_INT_LVL;
}

static inline void hs_stop_per_short_timer(void)
{
    DAC_UPDATE_TIMER.INTCTRLA &= ~TC0_OVFINTLVL_gm;
}

static inline void hs_init_w(void)
{
    /* Init port */
    BLDC_W_PORT.DIRSET = BLDC_W_PIN;
    BLDC_W_PORT.OUTCLR = BLDC_W_PIN;
}

static inline void hs_set_w_lo(void)
{
    /* Init pin */
    BLDC_W_PORT.OUTCLR = BLDC_W_PIN;
}

static inline void hs_set_w_hi(void)
{
    BLDC_W_PORT.OUTSET = BLDC_W_PIN;
}

static inline void hs_start_w_update_timer(void)
{
    /* Enable interrupt for update timer */
    DAC_UPDATE_TIMER.INTCTRLA |= BLDC_W_TIMER_INT_LVL;
}

static inline void hs_stop_w_update_timer(void)
{
    /* Disable interrupt for update timer */
    DAC_UPDATE_TIMER.INTCTRLA &= ~TC0_OVFINTLVL_gm;
}

static inline void hs_update_w(void)
{
    switch(BLDC_status.sine_state.W) {
        case BLDC_W_GO_NEG:
            /* Start timer for 0 point update */
            hs_start_w_update_timer();
            break;
        case BLDC_W_NEG:
            /* Set W LO */
            hs_set_w_lo();
            break;
        case BLDC_W_GO_POS:
            /* Start timer for 0 point update */
            hs_start_w_update_timer();
            break;
        case BLDC_W_POS:
            /* Set W HI */
            hs_set_w_hi();
            break;

        default:
            break;
    }
}


/* Prototypes */
void hs_hall_init(void);
void hs_set_direction(enum eRunningState state);
void hs_update_hall_state(void);
int hs_comp_sine_state(void);
void hs_comp_new_update_period(void);
int hs_comp_sine_start(uint16_t *data_base);
int hs_update_sine(void);

#endif /*__HALL_STATE_H__*/

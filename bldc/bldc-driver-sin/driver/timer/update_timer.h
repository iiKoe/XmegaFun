/*
 * Driver for setting the DAC update period.
 * The update period is set, and at that rate values from a sine table
 * are transfers into the dac at that period using DMA.
 *
 * By: Vito 
 */
#ifndef __UPDATE_TIMER_H__
#define __UPDATE_TIMER_H__

#include "lib/dma/dma.h"

#define DAC_UPDATE_TIMER        TCC1
#define DAC_UPDATE_TIMER_PSCL   TC_CLKSEL_DIV256_gc

#define BLDC_DAC_EV_MUX         CH1MUX
#define BLDC_TIMER_EV_TRIG      EVSYS_CHMUX_TCC1_OVF_gc;

#define BLDC_DAC                DACB
#define BLDC_DAC_REF            DAC_REFSEL_AREFA_gc
#define BLDC_DAC_EVENT          DAC_EVSEL_1_gc
#define BLDC_DAC_EV_TRIG_U      DAC_CH0TRIG_bm
#define BLDC_DAC_EV_TRIG_V      DAC_CH1TRIG_bm
#define BLDC_DAC_REFRESH        DAC_REFRESH_32CLK_gc

#define BLDC_DMA_U              DMA.CH0
#define BLDC_DMA_V              DMA.CH1
#define BLDC_DMA_U_TRIGSRC      DMA_CH_TRIGSRC_DACB_CH0_gc
#define BLDC_DMA_V_TRIGSRC      DMA_CH_TRIGSRC_DACB_CH1_gc
#define BLDC_DAC_DMA_PRI        DMA_PRIMODE_RR0123_gc
#define BLDC_DMA_SRC_RELOAD     DMA_CH_SRCRELOAD_TRANSACTION_gc
#define BLDC_DMA_SRC_ADDRM      DMA_CH_SRCDIR_INC_gc
#define BLDC_DMA_DEST_RELOAD    DMA_CH_DESTRELOAD_BURST_gc
#define BLDC_DMA_DEST_ADDRM     DMA_CH_DESTDIR_INC_gc
#define BLDC_DMA_BURSTLEN       DMA_CH_BURSTLEN_2BYTE_gc

#define BLDC_DMA_TRN_INT_LVL    DMA_CH_TRNINTLVL_MED_gc
#define BLDC_DMA_ERR_INT_LVL    DMA_CH_ERRINTLVL_OFF_gc
#define DMA_INT_U_VECT          DMA_CH0_vect
#define DMA_INT_V_VECT          DMA_CH1_vect

static inline void ut_set_dac_update_per(uint16_t per)
{
    DAC_UPDATE_TIMER.PERBUF = per;
}

static inline void ut_update_timer_start(void)
{
    DAC_UPDATE_TIMER.CTRLA  = DAC_UPDATE_TIMER_PSCL;
}

static inline void ut_update_timer_stop(void)
{
    DAC_UPDATE_TIMER.CTRLA  = TC_CLKSEL_OFF_gc;
    DAC_UPDATE_TIMER.CNT    = 0x00;
}

static inline void ut_dma_new_source_u(uint16_t *data)
{
    dma_source_dest(&BLDC_DMA_U, data, &BLDC_DAC.CH0DATA);
}

static inline void ut_dma_new_source_v(uint16_t *data)
{
    dma_source_dest(&BLDC_DMA_V, data, &BLDC_DAC.CH1DATA);
}

static inline void ut_dma_start(void)
{
    dma_start(&BLDC_DMA_U, true, false, true, BLDC_DMA_BURSTLEN);
    dma_start(&BLDC_DMA_V, true, false, true, BLDC_DMA_BURSTLEN);
}

void ut_update_dac_dma_init(uint16_t *data, uint16_t size);

#endif /*__UPDATE_TIMER_H__*/

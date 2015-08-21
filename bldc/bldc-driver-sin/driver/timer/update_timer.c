/*
 * Driver for setting the DAC update period.
 * The update period is set, and at that rate values from a sine table
 * are transferd into the dac at that period using DMA.
 *
 * By: Vito
 */
#include <avr/io.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <stdbool.h>

#include "lib/dac/dac.h"
#include "lib/dma/dma.h"
#include "update_timer.h"
#include "driver/hall/hall_state.h"
#include "driver/debug/debug.h"


/*
 * Update the DMA channels with new data to send to the DAC's
 */
void ut_update_dac_dma_init(uint16_t *data, uint16_t size)
{
    /* Setup Event for timer */
    EVSYS.BLDC_DAC_EV_MUX = BLDC_TIMER_EV_TRIG;

    /* Setup DAC */
    //dac_single_init(&BLDC_DAC, BLDC_DAC_REF, BLDC_DAC_REFRESH);
    dac_dual_init(&BLDC_DAC, BLDC_DAC_REF, BLDC_DAC_REFRESH);
    dac_set_event(&BLDC_DAC, BLDC_DAC_EVENT, BLDC_DAC_EV_TRIG_U | BLDC_DAC_EV_TRIG_V);

    /* Setup DMA */
    dma_enable(BLDC_DAC_DMA_PRI);

    dma_address_ctrl(&BLDC_DMA_U,
            BLDC_DMA_SRC_RELOAD, BLDC_DMA_SRC_ADDRM,
            BLDC_DMA_DEST_RELOAD, BLDC_DMA_DEST_ADDRM);
    dma_address_ctrl(&BLDC_DMA_V,
            BLDC_DMA_SRC_RELOAD, BLDC_DMA_SRC_ADDRM,
            BLDC_DMA_DEST_RELOAD, BLDC_DMA_DEST_ADDRM);

    dma_trigger_src(&BLDC_DMA_U, BLDC_DMA_U_TRIGSRC);
    dma_trigger_src(&BLDC_DMA_V, BLDC_DMA_V_TRIGSRC);

    dma_transfer_cnt(&BLDC_DMA_U, (size / 6 * 2));
    dma_transfer_cnt(&BLDC_DMA_V, (size / 6 * 2));

    dma_set_ch_int_lvl(&BLDC_DMA_U, BLDC_DMA_ERR_INT_LVL, BLDC_DMA_TRN_INT_LVL);
    //dma_set_ch_int_lvl(&BLDC_DMA_V, BLDC_DMA_ERR_INT_LVL, BLDC_DMA_TRN_INT_LVL);

    //dma_source_dest(&BLDC_DMA_V, data, &BLDC_DAC_V.CH0DATA);
    //dma_start(&BLDC_DMA_V, true, false, true, BLDC_DMA_BURSTLEN);
}


/*
 * Interrupt called if the DMA transfer is done.
 */
ISR(DMA_INT_U_VECT)
{
    /* Clear interrupt bit */
    BLDC_DMA_U.CTRLB |= DMA_CH_TRNIF_bm;
    BLDC_DMA_U.CTRLB |= DMA_CH_ERRIF_bm;
    PORTE.OUTTGL = PIN0_bm;
    BLDC_status.intr.dma += 1;

    /* Hall test */
    //hs_update_sine(); // Voor hall state update in DMA

    /* To slow timer */
    BLDC_status.period_offset.offset_state = BLDC_PER_TO_SHORT;
    hs_start_per_short_timer();

}


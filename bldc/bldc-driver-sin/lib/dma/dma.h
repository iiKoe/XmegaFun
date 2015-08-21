/*
 * Library for using the Xmega DMA
 * By: Vito
 */
#ifndef __DMA_H__
#define __DMA_H__

#include <stdbool.h>

static inline void dma_enable(DMA_PRIMODE_t priority)
{
    DMA.CTRL = DMA_ENABLE_bm | priority;
}

static inline void dma_address_ctrl(volatile DMA_CH_t *dma_ch,
        DMA_CH_SRCRELOAD_t s_reload, DMA_CH_SRCDIR_t s_addrm,
        DMA_CH_DESTRELOAD_t d_reload, DMA_CH_DESTDIR_t d_addrm)
{
    dma_ch->ADDRCTRL = s_reload | s_addrm | d_reload | d_addrm;
}

static inline void dma_trigger_src(volatile DMA_CH_t *dma_ch, DMA_CH_TRIGSRC_t src)
{
    dma_ch->TRIGSRC = src;
}

static inline void dma_transfer_cnt(volatile DMA_CH_t *dma_ch, uint16_t count)
{
    dma_ch->TRFCNT = count;
}

static inline void dma_start(volatile DMA_CH_t *dma_ch,
        bool enable, bool repeat, bool single, DMA_CH_BURSTLEN_t burst)
{
    dma_ch->CTRLA = (enable ? DMA_CH_ENABLE_bm : (0 << DMA_CH_ENABLE_bp)) |
                    (repeat ? DMA_CH_REPEAT_bm : (0 << DMA_CH_REPEAT_bp)) |
                    (single ? DMA_CH_SINGLE_bm : (0 << DMA_CH_SINGLE_bp)) |
                    burst;
}

static inline void dma_stop(volatile DMA_CH_t *dma_ch)
{
    dma_ch->CTRLA &= ~DMA_CH_ENABLE_bp;
}

static inline void dma_resume(volatile DMA_CH_t *dma_ch)
{
    dma_ch->CTRLA |= DMA_CH_ENABLE_bp;
}

static inline uint16_t dma_get_cnt(volatile DMA_CH_t *dma_ch)
{
    return dma_ch->TRFCNT;
}

static inline void dma_set_ch_int_lvl(volatile DMA_CH_t *dma_ch,
        DMA_CH_ERRINTLVL_t err_int, DMA_CH_TRNINTLVL_t trn_int)
{
    dma_ch->CTRLB &= ~(DMA_CH_TRNINTLVL_gm | DMA_CH_ERRINTLVL_gm);
    dma_ch->CTRLB |= err_int | trn_int;
}

void dma_source_dest(volatile DMA_CH_t *dma_ch, volatile void *source, volatile void *dest);

#endif /*__DMA_H__*/

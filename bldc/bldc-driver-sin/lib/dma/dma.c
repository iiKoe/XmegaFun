/*
 * Library for using the Xmega DMA
 * By: Vito
 */
#include <avr/io.h>
#include <inttypes.h>
#include <stdbool.h>

#include "dma.h"

/*
 * Set the source and destination of the DMA channel.
 */
void dma_source_dest(volatile DMA_CH_t *dma_ch, volatile void *source, volatile void *dest)
{
    uint32_t source32 = (uintptr_t)source, dest32 = (uintptr_t)dest;

    dma_ch->SRCADDR0 = (uint8_t)((source32 >>  0) & 0xFF);
    dma_ch->SRCADDR1 = (uint8_t)((source32 >>  8) & 0xFF);
    dma_ch->SRCADDR2 = (uint8_t)((source32 >> 16) & 0xFF);

    dma_ch->DESTADDR0 = (uint8_t)((dest32 >>  0) & 0xFF);
    dma_ch->DESTADDR1 = (uint8_t)((dest32 >>  8) & 0xFF);
    dma_ch->DESTADDR2 = (uint8_t)((dest32 >> 16) & 0xFF);
}


#include <avr/io.h>
#include <stdbool.h>
#include <avr/interrupt.h>


/*! Size of one memory block. */
#define MEM_BLOCK_SIZE  (1024)

 /*! Memory block count. */
#define MEM_BLOCK_COUNT (1)

/*! Total block size. Note that MEM_BLOCK_SIZE * MEM_BLOCK_COUNT must not
 *  exceed 64k for this demo application.
 */
#define MEM_BLOCK ( MEM_BLOCK_SIZE * MEM_BLOCK_COUNT )

/*! Memory blocks A and B for testing. */
uint8_t memoryBlockA[MEM_BLOCK];
uint8_t memoryBlockB[MEM_BLOCK];

/*! Global declared status for interrupt routine. */
volatile bool gInterruptDone;
volatile bool gStatus=true;


/*! \brief This function enable the DMA module.
 *
 *  \note Each individual DMA channel must be enabled separately
 *        using the DMA_EnableChannel() function.
 */
#define DMA_Enable()    ( DMA.CTRL |= DMA_ENABLE_bm )

/*! \brief This function configures the necessary registers for a block transfer.
 *
 *  \note The transfer must be manually triggered or a trigger source
 *        selected before the transfer starts. It is possible to reload the
 *        source and/or destination address after each data transfer, block
 *        transfer or only when the entire transfer is complete.
 *        Do not change these settings after a transfer has started.
 *
 *  \param  channel        The channel to configure.
 *  \param  srcAddr        Source memory address.
 *  \param  srcReload      Source address reload mode.
 *  \param  srcDirection   Source address direction (fixed, increment, or decrement).
 *  \param  destAddr       Destination memory address.
 *  \param  destReload     Destination address reload mode.
 *  \param  destDirection  Destination address direction (fixed, increment, or decrement).
 *  \param  blockSize      Block size in number of bytes (0 = 64k).
 *  \param  burstMode      Number of bytes per data transfer (1, 2, 4, or 8 bytes).
 *  \param  repeatCount    Number of blocks, 0x00 if you want to repeat at infinitum.
 *  \param  useRepeat      True if reapeat should be used, false if not.
 */
void DMA_SetupBlock( volatile DMA_CH_t * channel,
                     const void * srcAddr,
                     DMA_CH_SRCRELOAD_t srcReload,
                     DMA_CH_SRCDIR_t srcDirection,
                     void * destAddr,
                     DMA_CH_DESTRELOAD_t destReload,
                     DMA_CH_DESTDIR_t destDirection,
                     uint16_t blockSize,
                     DMA_CH_BURSTLEN_t burstMode,
                     uint8_t repeatCount,
                     bool useRepeat )
{
	channel->SRCADDR0 = (( (uint32_t) srcAddr) >> 0*8 ) & 0xFF;
	channel->SRCADDR1 = (( (uint32_t) srcAddr) >> 1*8 ) & 0xFF;
	channel->SRCADDR2 = (( (uint32_t) srcAddr) >> 2*8 ) & 0xFF;

	channel->DESTADDR0 = (( (uint32_t) destAddr) >> 0*8 ) & 0xFF;
	channel->DESTADDR1 = (( (uint32_t) destAddr) >> 1*8 ) & 0xFF;
	channel->DESTADDR2 = (( (uint32_t) destAddr) >> 2*8 ) & 0xFF;

	channel->ADDRCTRL = (uint8_t) srcReload | srcDirection |
	                              destReload | destDirection;
	channel->TRFCNT = blockSize;
	channel->CTRLA = ( channel->CTRLA & ~( DMA_CH_BURSTLEN_gm | DMA_CH_REPEAT_bm ) ) |
	                  burstMode | ( useRepeat ? DMA_CH_REPEAT_bm : 0);

	if ( useRepeat ) {
		channel->REPCNT = repeatCount;
	}
}



/*! \brief This function enables one DMA channel sub module.
 *
 *  \note A DMA channel will be automatically disabled
 *        when a transfer is finished.
 *
 *  \param  channel  The channel to enable.
 */
void DMA_EnableChannel( volatile DMA_CH_t * channel )
{
	channel->CTRLA |= DMA_CH_ENABLE_bm;
}



/*! \brief This function sends a manual transfer request to the channel.
 *
 *  The bit will automatically clear when transfer starts.
 *
 *  \param  channel  The channel to request a transfer for.
 */
void DMA_StartTransfer( volatile DMA_CH_t * channel )
{
	channel->CTRLA |= DMA_CH_TRFREQ_bm;
}



/*! \brief This function configures the interrupt levels for one DMA channel.
 *
 *  \note  The interrupt level parameter use the data type for channel 0,
 *         regardless of which channel is used. This is because we use the
 *         same function for all channel. This method relies upon channel
 *         bit fields to be located this way: CH3:CH2:CH1:CH0.
 *
 *  \param  channel      The channel to configure.
 *  \param  transferInt  Transfer Complete Interrupt Level.
 *  \param  errorInt     Transfer Error Interrupt Level.
 */
void DMA_SetIntLevel( volatile DMA_CH_t * channel,
                      DMA_CH_TRNINTLVL_t transferInt,
                      DMA_CH_ERRINTLVL_t errorInt )
{
	channel->CTRLB = (channel->CTRLB & ~(DMA_CH_ERRINTLVL_gm | DMA_CH_TRNINTLVL_gm)) |
			 transferInt | errorInt;
}



bool BlockMemCopy( const void * src,
				void * dest,
				uint16_t blockSize,
				volatile DMA_CH_t * dmaChannel )
{
	DMA_EnableChannel( dmaChannel );

	DMA_SetupBlock( dmaChannel,
		src,
		DMA_CH_SRCRELOAD_NONE_gc,
		DMA_CH_SRCDIR_INC_gc,
		dest,
		DMA_CH_DESTRELOAD_NONE_gc,
		DMA_CH_DESTDIR_INC_gc,
		blockSize,
		DMA_CH_BURSTLEN_1BYTE_gc,
		0,
		false
	 );

	DMA_StartTransfer( dmaChannel );

	return true;
}


int main(void)
{
	
	
	uint32_t index;

	volatile DMA_CH_t * Channel;
	Channel = &DMA.CH0;

	DMA_Enable();

	/*  Test 2: Copy using single block mode and use interrupt. Perform
	 *  second test only if first test succeeded.
	 */
	if ( gStatus ) {

		/*  Enable LO interrupt level for the complete transaction and
		 *  error flag on DMA channel 0.
		 */
		DMA_SetIntLevel( Channel, DMA_CH_TRNINTLVL_LO_gc, DMA_CH_ERRINTLVL_LO_gc );
		PMIC.CTRL |= PMIC_LOLVLEN_bm;
		sei();

		/* Fill memory block A with example data again. */
		for ( index = 0; index < MEM_BLOCK; ++index ) {
			memoryBlockA[index] = 0xff - ( (uint8_t) index & 0xff );
		}

		/* Set intDone to false to know when it has been executed. */
		gInterruptDone = false;

		/* Copy data using channel 0. */
		gStatus = BlockMemCopy( memoryBlockA,
		                        memoryBlockB,
		                        MEM_BLOCK,
		                        Channel);

		do {
			/* Do something here while waiting for the
			 * interrupt routine to signal completion.
			 */
		} while ( gInterruptDone == false );

		/* Compare memory blocks. */
		if ( gStatus ) {
			for ( index = 0; index < MEM_BLOCK; ++index ) {
				if (memoryBlockA[index] != memoryBlockB[index]) {
					gStatus = false;
					break;
				}
			}
		}
	}

	if ( gStatus ) {
		do {
			/* Completed with success. */
		} while (1);
	} else {
		do {
			/* Completed with failure. */
		} while (1);
	}
}



/*! DMA CH0 Interrupt service routine. Clear interrupt flags after check. */
ISR(DMA_CH0_vect)
{
	if (DMA.CH0.CTRLB & DMA_CH_ERRIF_bm) {
		DMA.CH0.CTRLB |= DMA_CH_ERRIF_bm;
		gStatus = false;
	} else {
		DMA.CH0.CTRLB |= DMA_CH_TRNIF_bm;
		gStatus = true;
	}
	gInterruptDone = true;
}

/******************************************************************************
 *                      Task 3: USART using DMAC
 *
 * This task will show how to set up the USART with the DMA controller. 
 * SetupTransmitChannel and SetupReceiveChannel are using the dma_driver.c
 * The driver provides a fast setup of the DMAC. SetupTransmitChannel 
 * is setting up the Tx_Buf buffer to feed the USART data transfer register. 
 * At the receiver, another DMAC channel will receive the bytes and 
 * start filling the Rx_Buf buffer.
 *****************************************************************************/
#define F_CPU 32000000UL
#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "spi.h"
#include "dma_driver.h"

#define TEST_CHARS  1024
volatile bool success;

#define DMA_USPI_SEND_CHANNEL &DMA.CH0

// Buffers to send/receive
static char SPI_BUFF[TEST_CHARS];
static volatile char testBuff[TEST_CHARS] = { [0 ... 1023] = 'A' };

void setClockTo32MHz(void) {
	CCP = CCP_IOREG_gc;						// disable register security for oscillator update
	OSC.CTRL = OSC_RC32MEN_bm;				// enable 32MHz oscillator
	while(!(OSC.STATUS & OSC_RC32MRDY_bm)); // wait for oscillator to be ready
	CCP = CCP_IOREG_gc;						// disable register security for clock update
	CLK.CTRL = CLK_SCLKSEL_RC32M_gc;		// switch to 32MHz clock
}


void SetupTransmitChannel( void )
{
   DMA_SetupBlock( 
                    DMA_USPI_SEND_CHANNEL,
                    testBuff,
                    DMA_CH_SRCRELOAD_TRANSACTION_gc, 
                    DMA_CH_SRCDIR_INC_gc,
                    (void *) &USPI.DATA,
                    DMA_CH_DESTRELOAD_NONE_gc, 
                    DMA_CH_DESTDIR_FIXED_gc,
                    TEST_CHARS, 
                    DMA_CH_BURSTLEN_1BYTE_gc, 
                    0, // Perform once
                    false
                   );
    DMA_EnableSingleShot( DMA_USPI_SEND_CHANNEL );
    DMA_SetTriggerSource( DMA_USPI_SEND_CHANNEL, DMA_CH_TRIGSRC_USARTD0_DRE_gc ); 
}




int main(void)
{
	setClockTo32MHz();
    volatile uint16_t i = 0;   

	
	
    USPI_master_setup();
    
    // Initialise DMAC
    DMA_Enable();
    SetupTransmitChannel();
	DMA_SetIntLevel(DMA_USPI_SEND_CHANNEL, DMA_CH_TRNINTLVL_MED_gc, DMA_CH_ERRINTLVL_MED_gc);
	
	
	sei();
	PMIC.CTRL = PMIC_MEDLVLEN_bm;			// Enable MEDIUM interrupts
    
    // Initialise transmit buffer
    for(i=0; i < TEST_CHARS; i++)
    {
        // filling in a,b,c,...,t
        SPI_BUFF[i] = 'a'+ i;
    }
    
    // Assume that everything is OK.
    success = true;
    

    // Start sending the data bits from Tx_Buf on the
    // transmit dma channel.
    // Note that DRE is considered empty at this point,
    // thus triggering the initial transfer.

	
	
    while(1)
    {
	   if (success == true)
	   {
		   _delay_ms(10);
		   success = false;
		   USPI_SS_CLR();
		   DMA_EnableChannel( DMA_USPI_SEND_CHANNEL);
	   }
	}   
}


ISR(DMA_CH0_vect)
{
	USPI_SS_SET();
	success = true;
	DMA.CH0.CTRLB |= DMA_CH_ERRIF_bm | DMA_CH_TRNIF_bm;
	
}
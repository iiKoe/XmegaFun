/*
 * spi_simple.c
 *
 * Created: 1-2-2013 10:25:11
 *  Author: Vito
 */ 


#include <avr/io.h>
#define F_CPU 32000000UL
#include <util/delay.h>

#define SPI_MOSI	PIN5_bm
#define SPI_MISO	PIN6_bm
#define SPI_SS		PIN4_bm
#define SPI_CLK		PIN7_bm


void setClockTo32MHz(void) 
{
	CCP = CCP_IOREG_gc;							// disable register security for oscillator update
	OSC.CTRL = OSC_RC32MEN_bm;					// enable 32MHz oscillator
	while(!(OSC.STATUS & OSC_RC32MRDY_bm));		// wait for oscillator to be ready
	CCP = CCP_IOREG_gc;							// disable register security for clock update
	CLK.CTRL = CLK_SCLKSEL_RC32M_gc;			// switch to 32MHz clock
}

void SPI_master_setup(void)
{
	PORTC.DIR |= SPI_MOSI | SPI_SS | SPI_CLK;
	PORTC.OUT |= SPI_SS;
	SPIC.CTRL = SPI_ENABLE_bm | SPI_MASTER_bm | SPI_MODE_3_gc | SPI_PRESCALER_DIV4_gc | SPI_CLK2X_bm;
	SPIC.INTCTRL = SPI_INTLVL_OFF_gc;
}


void SPI_master_write_data(uint8_t data)
{
	//PORTC.OUT &= ~ SPI_SS;
	SPIC.DATA = data;
	while(!(SPIC.STATUS & SPI_IF_bm));
	data = SPIC.DATA;
	//PORTC.OUT |= SPI_SS;
}

uint8_t SPI_master_read_data(void)
{
	SPIC.DATA = 0x00;
	while(!(SPIC.STATUS & SPI_IF_bm));
	return SPIC.STATUS;
}

uint8_t SPI_master_transceive(uint8_t data)
{
	SPI_master_write_data(data);
	return SPIC.DATA;
}

int main(void)
{
	setClockTo32MHz();
	SPI_master_setup();
	char data [] = "Hello";
	
	PORTE.DIR |= PIN0_bm;
	
    while(1)
    {
		int i;
		for(i=0; i<255; i++)
		{
			//PORTC.OUT &= ~ SPI_SS;
			SPI_master_write_data(i);
			//PORTC.OUT |= SPI_SS;
		}			
    }
}
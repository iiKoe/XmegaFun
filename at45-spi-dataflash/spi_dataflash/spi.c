/************************************************************************************
 Title	:   C file for the xmega spi "library" (spi.h)
 Author:    Vito https://github.com/iiKoe
 File:	    spi.c, version 1.0, 05/02/2013
 Software:  AVR-GCC 4.6.2
 Hardware:  anny AVR XMEGA

 DESCRIPTION
       Bacic "library" for sending and receiving SPI data.
	   This version supports both Hardware SPI and UART based SPI

 USAGE
       See the header file (spi.h) for a description of each function

***********************************************************************************/

#include <avr/io.h>
#include "spi.h"


/************************************************
*
*		HARDWARE SPI MODE
*
*************************************************/



/*************************************************************************
Setup hardware SPI in Master mode
Input:    none
Returns:  none
*************************************************************************/
void SPI_master_setup(void)
{
    SPI_PORT.DIR	|= SPI_MOSI | SPI_SS | SPI_CLK;
    SPI_SS_SET();
    SPI.CTRL	 = SPI_ENABLE_bm | SPI_MASTER_bm | SPI_MODE_3_gc | SPI_PRESCALER_DIV4_gc | SPI_CLK2X_bm;
    SPI.INTCTRL = SPI_INTLVL_OFF_gc;
}



/*************************************************************************
Write 8-bit data to the connected slave
Input:    8-bit data
Returns:  none
*************************************************************************/
inline void SPI_master_write_data(uint8_t data)
{
    SPI.DATA = data;
    while(!(SPI.STATUS & SPI_IF_bm));
    data = SPI.DATA;
}



/*************************************************************************
Read 8-bit data from the connected slave
Input:    none
Returns:  8-bit data
*************************************************************************/
inline uint8_t SPI_master_read_data(void)
{
    SPI.DATA = 0x00;
    while(!(SPI.STATUS & SPI_IF_bm));
    return SPI.DATA;
}


/************************************************
*
*		USART SPI MODE
*
*************************************************/



/*************************************************************************
Setup UART SPI in Master mode
Input:    none
Returns:  none
*************************************************************************/
void USPI_master_setup(void)
{
    USPI_PORT.DIRSET  = USPI_MOSI | USPI_CLK;
    USS_PORT.DIRSET   = USPI_SS;
    USPI_SS_SET();

    USPI.BAUDCTRLA		= 0;
    USPI.BAUDCTRLB		= 0;
    USPI.CTRLB			= USART_RXEN_bm | USART_TXEN_bm;
    USPI.CTRLC			= USART_CMODE_MSPI_gc | 0x02;		// UCPHA = 0x02
    USPI_PORT.PIN1CTRL	= PORT_INVEN_bm;					// INVEN = 1
    USPI.BAUDCTRLA		= 0x00;
    USPI.BAUDCTRLB		= 0x00;
}



/*************************************************************************
Write 8-bit data to the connected slave
Input:    8-bit data
Returns:  none
*************************************************************************/
inline void USPI_master_write_data(uint8_t data)
{
    while (!(USPI.STATUS & USART_DREIF_bm));
    USPI.DATA = data;
    while (!(USPI.STATUS & USART_RXCIF_bm));
    data = USPI.DATA;
}



/*************************************************************************
Read 8-bit data from the connected slave
Input:    none
Returns:  8-bit data
*************************************************************************/
inline uint8_t USPI_master_read_data(void)
{
    while (!(USPI.STATUS & USART_DREIF_bm));
    USPI.DATA = 0x00;
    while (!(USPI.STATUS & USART_RXCIF_bm));
    return USPI.DATA;
}
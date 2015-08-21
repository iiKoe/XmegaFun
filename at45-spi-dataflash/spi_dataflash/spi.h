/************************************************************************************
 Title	:   Header file for the xmega spi library (spi.c)
 Author:    Vito https://github.com/iiKoe
 File:	    oled.h, version 1.0, 05/02/2013
 Software:  AVR-GCC 4.6.2
 Hardware:  any AVR XMEGA
 Code:		include spi.h in main code
************************************************************************************/


/**
 DESCRIPTION
	This basic "library" makes it easeyer to use HW SPI or UART SPI
*/


#ifndef SPI_H_
#define SPI_H_


/************************************************
*
*	HARDWARE SPI MODE
*
*************************************************/



/**
 *  @name     Port decelerations for the HW-SPI
 */
#define SPI_PORT	PORTC
#define SPI			SPIC



/**
 *  @name     Pin decelerations for the HW-SPI
 */
#define SPI_MOSI	PIN5_bm
#define SPI_MISO	PIN6_bm
#define SPI_SS		PIN4_bm
#define SPI_CLK		PIN7_bm

#define SPI_SS_SET()	(SPI_PORT.OUTSET = SPI_SS)
#define SPI_SS_CLR()	(SPI_PORT.OUTCLR = SPI_SS)



/**
 @brief    Setup Hardware SPI in Master mode
 @param    none
 @return   none
*/
void SPI_master_setup(void);



/**
 @brief    Write 8-bit SPI data using HW-SPI
 @param    8-bit data
 @return   none
*/
void SPI_master_write_data(uint8_t data);



/**
 @brief    Read 8-bit SPI data using HW-SPI
 @param    none
 @return   8-bit data
*/
uint8_t SPI_master_read_data(void);



/************************************************
*
*	USART SPI MODE
*
*************************************************/

/**
 *  @name     Port decelerations for the UART-SPI
 */
#define USS_PORT	PORTC
#define USPI_PORT	PORTC
#define USPI		USARTC0



/**
 *  @name     Pin decelerations for the UART-SPI
 */
#define USPI_MOSI	PIN3_bm			//TX
#define USPI_MISO	PIN2_bm			//RX
#define USPI_SS		PIN4_bm			//SS
#define USPI_CLK	PIN1_bm			//XCK0

#define USPI_SS_SET()	(USS_PORT.OUTSET = USPI_SS)
#define USPI_SS_CLR()	(USS_PORT.OUTCLR = USPI_SS)



/**
 @brief    Setup UART SPI in Master mode
 @param    none
 @return   none
*/
void USPI_master_setup(void);



/**
 @brief    Write 8-bit SPI data using UART-SPI
 @param    8-bit data
 @return   none
*/
void USPI_master_write_data(uint8_t data);



/**
 @brief    Read 8-bit SPI data using UART-SPI
 @param    none
 @return   8-bit data
*/
uint8_t USPI_master_read_data(void);



#endif /* SPI_H_ */
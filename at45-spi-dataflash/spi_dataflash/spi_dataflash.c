/*
 * spi_dataflash.c
 *
 * Created: 5-2-2013
 *  Author: Vito
 */

#define F_CPU 32000000UL
#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>
#include <util/delay.h>
#include <string.h>
#include <stdbool.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include "dataflash_board_config.h"
#include "spi.h"
#include "dma_driver.h"
#include "at45_flash.h"
#include "flash_protocol.h"
#include "usart/usart_driver.h"


void setClockTo32MHz(void)
{
    CCP = CCP_IOREG_gc;							// disable register security for oscillator update
    OSC.CTRL = OSC_RC32MEN_bm;					// enable 32MHz oscillator
    while(!(OSC.STATUS & OSC_RC32MRDY_bm));		// wait for oscillator to be ready
    CCP = CCP_IOREG_gc;							// disable register security for clock update
    CLK.CTRL = CLK_SCLKSEL_RC32M_gc;			// switch to 32MHz clock
}


int main(void)
{
    setClockTo32MHz();
    BC_SET_LEDS();
    BC_SWITCH_PULLDOWN();
    BC_RISING_INT_SW1();
    BC_RISING_INT_SW2();
    USPI_master_setup();
    USART_Init();
    PMIC.CTRL |= PMIC_MEDLVLEN_bm;
    sei();
	//FLP_print_menu();
    FLP_init_flash_protocol();
    _delay_ms(1000);



/*
    uint16_t x;
    for (x=0; x<10; x++) {
        _delay_ms(10);
        FLP_write_string("# Alfa Betha Gamma Praxis black&decker $"); // schrijf naar de shit
    }*/
	//FLP_write_string("$DIt is een test#");
    _delay_ms(1000);

    //FLP_read_flash_to_buffer_npages(3); // lees 5 pagina's


    while (1) {

        //printf("Status: %x\n",at45_status_read()); //lees de status (niet nodig)
        _delay_ms(2000);
        FLP_check_commands();	// Kijk naar comando's via uUSB, moet gewoon als er tijd is gedraait worden
    }
}

//printf("%x",at45_status_read());
/*
		uint8_t buffer = 'B';
		buffer = at45_read_one_byte(1,2);
		printf("%c",buffer);
		_delay_ms(500);
		at45_write_data("Test",1,0);
		_delay_ms(500);*/


/*
		PORTF.OUTTGL = PIN1_bm;
		strcpy(at45_buffer, "Hello World!");
		USART_Puts("Send data: ");
		USART_Puts(at45_buffer);
		at45_write_data(at45_buffer, 1, 0);
		_delay_ms(100);
		at45_read_data(1, 0, 12);
		USART_Puts("\nReceived data: ");
		USART_Puts(at45_buffer);
		_delay_ms(1000);

*/
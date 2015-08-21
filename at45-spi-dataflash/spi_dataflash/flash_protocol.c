/*
 * flash_protocol.c
 *
 * Created: 18-4-2013 11:37:37
 *  Author: Vito
 */

#define F_CPU 32000000UL
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "dma_driver.h"
#include "spi.h"
#include "at45_flash.h"
#include "flash_protocol.h"
#include "usart/usart_driver.h"
#include "dataflash_board_config.h"

static uint8_t dataFlash_Buffer1[BUFFER_SIZE] = {[0 ... 1023] '0'};
static uint8_t dataFlash_Buffer2[BUFFER_SIZE] = {[0 ... 1023] '0'};
static uint8_t *dataFlash_Buffer_ptr = dataFlash_Buffer1;
static uint16_t g_page_number = 0;

static volatile bool g_dma_ready		= true;
static volatile bool g_dma_failed		= false;
static volatile bool g_force_push_dma	= false;

static volatile bool g_command_enable	= false;
static volatile bool g_command_first	= false;


uint16_t FLP_search_beginning_session(uint8_t *session_number)
{
    uint8_t session_buffer = 0;
	uint16_t numb_pages = 0;
	*session_number = 1;
    for(g_page_number=0; g_page_number<=FLASH_MAX_SIZE; g_page_number++) {
        session_buffer = at45_read_one_byte(g_page_number, 0);
        //printf("\nSession: %u \n", (uint16_t)session_number);
        if(session_buffer == 0xFF)
            return numb_pages;
        else {
            *session_number = session_buffer;
			numb_pages++;
		}			
    }
    return numb_pages;
}


void FLP_init_flash_protocol(void)
{
    uint8_t session_number = FLP_BEGIN_SESSION;
    //FLP_search_beginning_session(&session_number);

    dataFlash_Buffer1[SESSION_BYTE] = 42;
    dataFlash_Buffer2[SESSION_BYTE] = 42;

    DMA_Enable();
    DMA_SetupBlock(
        DMA_USPI_SEND_CHANNEL,
        (void *) dataFlash_Buffer_ptr,
        DMA_CH_SRCRELOAD_NONE_gc,
        DMA_CH_SRCDIR_INC_gc,
        (void *) &USPI.DATA,
        DMA_CH_DESTRELOAD_NONE_gc,
        DMA_CH_DESTDIR_FIXED_gc,
        DMA_SEND_SIZE,
        DMA_CH_BURSTLEN_1BYTE_gc,
        0, // Perform once
        false
    );
    DMA_EnableSingleShot(DMA_USPI_SEND_CHANNEL);
    DMA_SetTriggerSource(DMA_USPI_SEND_CHANNEL, DMA_CH_TRIGSRC_USARTC0_DRE_gc );
    DMA_SetIntLevel(DMA_USPI_SEND_CHANNEL, DMA_CH_TRNINTLVL_MED_gc, DMA_CH_ERRINTLVL_MED_gc);
}


bool FLP_write_data_flash_buffer(uint8_t data)
{
    static bool flash_full = false;
    static uint8_t current_buffer = BUFFER_ONE;	// Buffer 1 of 2
    static uint16_t buffer_byte_counter = BUFFER_BEGIN_BYTE;

    if (!g_dma_failed && !flash_full) {
        dataFlash_Buffer_ptr[buffer_byte_counter++]= data;
        if ((buffer_byte_counter>=BUFFER_SIZE && g_dma_ready) || g_force_push_dma) {
            g_dma_ready = false;
            if(g_force_push_dma) {
                for (; buffer_byte_counter<BUFFER_SIZE; buffer_byte_counter++) {
                    dataFlash_Buffer_ptr[buffer_byte_counter]= 0x00;
                }
                g_force_push_dma = false;
            }

            buffer_byte_counter = BUFFER_BEGIN_BYTE;
            if (current_buffer == BUFFER_ONE) {
                printf("\nPageBufferOne: %d\n",g_page_number);
                dataFlash_Buffer_ptr = dataFlash_Buffer2;
                current_buffer = BUFFER_TWO;
                CHANGE_DMA_SOURCE(dataFlash_Buffer1);
                SEND_BUFFER_FLASH_DMA_BUF1();
            } else {
                printf("\nPageBufferTwo: %d\n",g_page_number);
                dataFlash_Buffer_ptr = dataFlash_Buffer1;
                current_buffer = BUFFER_ONE;
                CHANGE_DMA_SOURCE(dataFlash_Buffer2);
                SEND_BUFFER_FLASH_DMA_BUF2();
            }

            if(++g_page_number >= FLASH_MAX_SIZE)
                flash_full = true;
        }
        return true;
    } else {
        LEDPORT.OUTSET = LED_ORANGE;
    }
    return false;
}

void FLP_write_string(const char *data)
{
    while (*data)
        FLP_write_data_flash_buffer(*data++);
}

void FLP_read_flash_to_buffer(void)
{
    uint8_t page_num = 0;
    USPI_SS_CLR();
    USPI_master_write_data(AT45_CONTAR_READ_LOF);
    at45_send_address(0,0);
    for (page_num=0; page_num<2; page_num++) {
        uint16_t byte_num;
        for(byte_num=0; byte_num<BUFFER_SIZE; byte_num++) {
            printf("%c",USPI_master_read_data());
            _delay_us(100);
        }
        printf("\n\n");
    }
    USPI_SS_SET();
}

void FLP_read_flash_to_buffer_npages(uint16_t pages)
{
	uint8_t page_num = 0;
	USPI_SS_CLR();
	USPI_master_write_data(AT45_CONTAR_READ_LOF);
	at45_send_address(0,0);
	for (page_num=0; page_num<pages; page_num++) {
		uint16_t byte_num;
		for(byte_num=0; byte_num<BUFFER_SIZE; byte_num++) {
			printf("%c",USPI_master_read_data());
			_delay_us(100);
		}
		printf("\n\n");
	}
	USPI_SS_SET();
}


void FLP_read_flash_to_buffer_pagen(uint16_t page)
{
	uint8_t page_num = 0;
	USPI_SS_CLR();
	USPI_master_write_data(AT45_CONTAR_READ_LOF);
	at45_send_address(page,0);
	uint16_t byte_num;
	for(byte_num=0; byte_num<BUFFER_SIZE; byte_num++) {
		printf("%c",USPI_master_read_data());
		_delay_us(100);
	}
	printf("\n\n");
	USPI_SS_SET();
}




void FLP_check_commands(void)
{
    if(g_command_enable) {
        static char RX_command_data_buffer[RX_COMMAND_SIZE+1];
        static uint8_t RX_command_data_index = 0;
        char *command_pt;

        if(g_command_first) {
            g_command_first = false;
            while (USART_RXBufferData_Available(&USART_data))
                USART_RXBuffer_GetByte(&USART_data);
			FLP_print_menu();	
        }

        while (USART_RXBufferData_Available(&USART_data)) {
            RX_command_data_buffer[RX_command_data_index++] = USART_RXBuffer_GetByte(&USART_data);
            if(RX_command_data_index > RX_COMMAND_SIZE)
                RX_command_data_index = 0;
        }

        if ((strstr(RX_command_data_buffer,"$RD_ALL" ))!=NULL) {
            printf("Read All\n");
			uint16_t pages = FLP_search_beginning_session(0x00);
			FLP_read_flash_to_buffer_npages(pages);
			printf("\n Done reading %d pages", pages);
			for (; RX_command_data_index>0; RX_command_data_index--)
				RX_command_data_buffer[RX_command_data_index] = 0;
        } 
		else if ((command_pt=strstr(RX_command_data_buffer,"$RD_P"))!=NULL) {
            printf("Read Page:");
            command_pt+=5;
			uint16_t rpage = atoi(command_pt);
			printf("%d\n",rpage);
            for (; RX_command_data_index>0; RX_command_data_index--)
                RX_command_data_buffer[RX_command_data_index] = 0;
        } 
		else if((command_pt=strstr(RX_command_data_buffer,"$RD_nP"))!=NULL){
			command_pt+=6;
			int pages = atoi(command_pt);
			printf("Read Page 0 - %d\n",pages);
			FLP_read_flash_to_buffer_npages(pages+1);
			printf("\n Done \n");
			for (; RX_command_data_index>0; RX_command_data_index--)
				RX_command_data_buffer[RX_command_data_index] = 0;
			
		} 
		else if ((strstr(RX_command_data_buffer,"$CLR_ALL" ))!=NULL) {
            printf("\nClear All, wait for +/- 10 seconds (long...)\n");
            at45_chip_erase();
            while((at45_status_read() & 0x80) != 0x80) {
                printf("...");
                _delay_ms(1000);
            }
            printf("\nDone!\n");
            for (; RX_command_data_index>0; RX_command_data_index--)
                RX_command_data_buffer[RX_command_data_index] = 0;
		 } 
		 else if ((command_pt=strstr(RX_command_data_buffer,"$WR_DATA "))!=NULL) {
			 printf("Send data to buffer:");
			 command_pt+=9;
			 printf("%s\n",command_pt);
			 FLP_write_string(command_pt);
			 for (; RX_command_data_index>0; RX_command_data_index--)
				RX_command_data_buffer[RX_command_data_index] = 0;
		 } 
		 else if((command_pt=strstr(RX_command_data_buffer,"$FLUSH_BUFFER"))!=NULL) {
			 	printf("Flush the buffer\n");
				 g_force_push_dma = true;
				 FLP_write_data_flash_buffer(0x00);
				 for (; RX_command_data_index>0; RX_command_data_index--)
					RX_command_data_buffer[RX_command_data_index] = 0;		
		} 
		else if((command_pt=strstr(RX_command_data_buffer,"help"))!=NULL) {
			FLP_print_menu();
			for (; RX_command_data_index>0; RX_command_data_index--)
			RX_command_data_buffer[RX_command_data_index] = 0;
		}			
		else {
			printf("\nEnter  help  to see the command list\n");
		}
	}
}		

void FLP_print_menu(void)
{
	_delay_ms(100);
	printf(" \n\n\n"\
	 " _ \n"\
	 "| | \n"\
	 "| |     ___   __ _  __ _  ___ _ __ \n"\
	 "| |    / _ \\ / _` |/ _` |/ _ \\ '__| \n"\
	 "| |___| (_) | (_| | (_| |  __/ | \n"\
	 "\\_____/\\___/ \\__, |\\__, |\\___|_| \n"\
	 "              __/ | __/ | \n"\
	 "             |___/ |___/ \n"\
	);
	printf("Version 1.0 for H2A\n\n\n");
	_delay_ms(10);
	printf("To enter a command, command mode must be enabled by pressing button one (near PDI connector) \n\n\n");
	printf("COMMAND LIST: (not functional)\n\n");
	_delay_ms(10);
	printf("* $CLR_ALL         - Erase the whole flash chip\n");
	printf("* $RD_ALL          - Read the whole flash chip\n");
	_delay_ms(10);
	printf("* $RD_nP           - Read until page n\n"); 
	printf("* $WR_DATA data    - Write data to buffer, follow with data to be send\n");
	printf("* $FLUSH_BUFFER    - Write buffer data to flash\n");
	printf("\n\n\n\n\n");
	_delay_ms(10);
}



ISR(DMA_CH0_vect)
{
    DELAY_20_CYCLE();
    USPI_SS_SET();
    if (DMA.INTFLAGS & DMA_CH0TRNIF_bm) {
        DMA.INTFLAGS = DMA_CH0TRNIF_bm;
        g_dma_ready = true;
    } else {
        DMA.INTFLAGS = DMA_CH0ERRIF_bm;
        g_dma_failed = true;
    }
    USPI.DATA;
    USPI.DATA;
    USPI.DATA;
}


ISR(PORTB_INT0_vect)
{
    if (SWITCHPORT.IN & SWITCH_ONE) {
        g_command_enable ^=true;
        g_command_first = true;
        LEDPORT.OUTTGL = LED_GREEN;
    } else if (SWITCHPORT.IN & SWITCH_TWO && g_force_push_dma == false) {
        g_force_push_dma = true;
        LEDPORT.OUTSET = LED_ORANGE;
        FLP_write_data_flash_buffer(0x00);
        LEDPORT.OUTCLR = LED_ORANGE;
    } else {
        //none
    }

}
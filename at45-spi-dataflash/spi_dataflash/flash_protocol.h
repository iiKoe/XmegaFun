/*
 * flash_protocol.h
 *
 * Created: 18-4-2013 11:37:47
 *  Author: Vito
 */


#ifndef FLASH_PROTOCOL_H_
#define FLASH_PROTOCOL_H_


#define BUFFER_ONE			1
#define BUFFER_TWO			2
#define BUFFER_BEGIN_BYTE	1
#define BUFFER_SIZE			1024	// Voor sessie nummer
#define SESSION_BYTE		0
#define FLP_BEGIN_SESSION	2

#define DMA_SEND_SIZE		1024
#define DMA_USPI_SEND_CHANNEL &DMA.CH0

#define FLASH_MAX_SIZE		8190

#define RX_COMMAND_SIZE		32

#define SEND_BUFFER_FLASH_DMA_BUF1() {\
	USPI_SS_CLR();\
	USPI_master_write_data(AT45_MMPPROG_TRBUFF1);\
	at45_send_address(g_page_number, 0);\
	DMA_EnableChannel( DMA_USPI_SEND_CHANNEL);\
}

#define SEND_BUFFER_FLASH_DMA_BUF2() {\
	USPI_SS_CLR();\
	USPI_master_write_data(AT45_MMPPROG_TRBUFF2);\
	at45_send_address(g_page_number, 0);\
	DMA_EnableChannel( DMA_USPI_SEND_CHANNEL);\
}

#define CHANGE_DMA_SOURCE(_srcAddr) {\
	((DMA_CH_t *)DMA_USPI_SEND_CHANNEL)->SRCADDR0 = (( (uint32_t)(intptr_t) _srcAddr) >> 0*8 ) & 0xFF;\
	((DMA_CH_t *)DMA_USPI_SEND_CHANNEL)->SRCADDR1 = (( (uint32_t)(intptr_t) _srcAddr) >> 1*8 ) & 0xFF;\
	((DMA_CH_t *)DMA_USPI_SEND_CHANNEL)->SRCADDR2 = (( (uint32_t)(intptr_t) _srcAddr) >> 2*8 ) & 0xFF;\
}

#define DELAY_20_CYCLE() {\
	asm volatile ("nop");\
	asm volatile ("nop");\
	asm volatile ("nop");\
	asm volatile ("nop");\
	asm volatile ("nop");\
	asm volatile ("nop");\
	asm volatile ("nop");\
	asm volatile ("nop");\
	asm volatile ("nop");\
	asm volatile ("nop");\
	asm volatile ("nop");\
	asm volatile ("nop");\
	asm volatile ("nop");\
	asm volatile ("nop");\
	asm volatile ("nop");\
	asm volatile ("nop");\
	asm volatile ("nop");\
	asm volatile ("nop");\
	asm volatile ("nop");\
	asm volatile ("nop");\
}

uint16_t FLP_search_beginning_session (uint8_t *session_number);
void FLP_init_flash_protocol(void);
bool FLP_write_data_flash_buffer(uint8_t data);
void FLP_write_string(const char *data);
void FLP_read_flash_to_buffer(void);
void FLP_read_flash_to_buffer_npages(uint16_t pages);
void FLP_check_commands(void);
void FLP_print_menu(void);



#endif /* FLASH_PROTOCOL_H_ */
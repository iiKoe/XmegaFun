/*
 * at45_flash.h
 *
 * Created: 5-2-2013 12:27:52
 *  Author: Vito
 */


#ifndef AT45_FLASH_H_
#define AT45_FLASH_H_

/* PROGRAM DEFINES */
#define MAX_BUFFER_SIZE			1024

volatile uint8_t at45_buffer[5];

/* READ COMMANDS */
#define AT45_MMPAGE_READ		0xD2
#define	AT45_CONTAR_READ_LC		0xE8
#define AT45_CONTAR_READ_LOF	0x03
#define AT45_CONTAR_READ		0x0B
#define	AT45_BUFF1_READ_LOF		0xD1
#define	AT45_BUFF2_READ_LOF		0xD3
#define	AT45_BUFF1_READ			0xD4
#define AT45_BUFF2_READ			0xD6

/* PROGRAM AND ERASE COMMANDS */
#define AT45_BUFF1_WRITE		0x84
#define	AT45_BUFF2_WRITE		0x87
#define AT45_BUFF1_MMPPROG		0x83
#define	AT45_BUFF2_MMPPROG		0x86
#define	AT45_BUFF1_MMPPROG_WOE	0x88
#define AT45_BUFF2_MMPPROG_WOE	0x89
#define AT45_PAGE_ERASE			0x81
#define AT45_BLCOK_ERASE		0x50
#define AT45_SECTOR_ERASE		0x7C
#define	AT45_CHIP_ERASE_B1		0xC7
#define	AT45_CHIP_ERASE_B2		0x94
#define	AT45_CHIP_ERASE_B3		0x80
#define	AT45_CHIP_ERASE_B4		0x9A
#define AT45_MMPPROG_TRBUFF1	0x82
#define AT45_MMPPROG_TRBUFF2	0x85

/* ADDITIONAL COMMANDS */
#define AT45_MMP_BUFF1_TRANS	0x53
#define	AT45_MMP_BUFF2_TRANS	0x55
#define AT45_MMP_BUFF1_COMP		0x60
#define AT45_MMP_BUFF2_COMP		0x61
#define AT45_AUTOP_RW_BUFF1		0x58
#define AT45_AUTOP_RW_BUFF2		0x59
#define AT45_DEEP_POWERDOWN		0xB9
#define AT45_RESUME_POWERDOWN	0xAB
#define AT45_STATUSR_READ		0xD7
#define AT45_MAN_DEVICEID_READ	0x9F
#define AT45_POWEROFTWO_B1		0x3D
#define AT45_POWEROFTWO_B2		0x2A
#define AT45_POWEROFTWO_B3		0x80
#define AT45_POWEROFTWO_B4		0xA6





void at45_send_address(uint16_t page, uint16_t b_buffer);
void at45_set_poweroftwo(void);
void at45_readid(uint8_t *idBuffer);
void at45_readstatus(uint8_t *statusBuffer);
void at45_chip_erase(void);
void at45_page_erase(uint16_t page);
uint8_t at45_status_read(void);
void at45_write_data(const char *s, uint16_t page, uint16_t b_buffer);
void at45_write_data_bf1(const char *s, uint16_t page, uint16_t b_buffer);
void at45_write_data_bf2(const char *s, uint16_t page, uint16_t b_buffer);
void at45_read_data(uint16_t page, uint16_t b_buffer, uint8_t numb_bytes, uint8_t *save_buffer);
uint8_t at45_read_one_byte(uint16_t page, uint16_t b_buffer);




#endif /* AT45_FLASH_H_ */
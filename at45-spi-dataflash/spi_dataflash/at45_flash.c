/*
 * at45_flash.c
 *
 * Created: 5-2-2013 20:15:47
 *  Author: Vito
 *	test
 */

#include <avr/io.h>
#include "at45_flash.h"
#include "SPI.h"

//static uint16_t g_page, g_bbuffer = 0;

void at45_send_address(uint16_t page, uint16_t b_buffer)
{
    uint8_t address[3];
    uint8_t i;

    address[0] = page>>6;
    address[1] = (page<<2 | b_buffer>>8);
    address[2] = b_buffer;

    for(i=0; i<=2; i++)
        USPI_master_write_data(address[i]);
}

void at45_set_poweroftwo(void)
{
    USPI_SS_CLR();
    USPI_master_write_data(AT45_POWEROFTWO_B1);
    USPI_master_write_data(AT45_POWEROFTWO_B2);
    USPI_master_write_data(AT45_POWEROFTWO_B3);
    USPI_master_write_data(AT45_POWEROFTWO_B4);
    USPI_SS_SET();
}

void at45_readid(uint8_t *idBuffer)
{
    USPI_SS_CLR();
    USPI_master_write_data(AT45_MAN_DEVICEID_READ);
    uint8_t i;
    for (i=0; i<2; i++)
        idBuffer[i] = USPI_master_read_data();
    idBuffer[i]	= '\0';
    USPI_SS_SET();
}

void at45_readstatus(uint8_t *statusBuffer)
{
    USPI_SS_CLR();
    USPI_master_write_data(AT45_STATUSR_READ);
    int i;
    for (i=0; i<2; i++)
        statusBuffer[i] = USPI_master_read_data();
    statusBuffer[i]	= '\0';
    USPI_SS_SET();
}


void at45_chip_erase(void)
{
    USPI_SS_CLR();
    USPI_master_write_data(AT45_CHIP_ERASE_B1);
    USPI_master_write_data(AT45_CHIP_ERASE_B2);
    USPI_master_write_data(AT45_CHIP_ERASE_B3);
    USPI_master_write_data(AT45_CHIP_ERASE_B4);
    USPI_SS_SET();
}

void at45_page_erase(uint16_t page)
{
    if (page <= 8192) {
        USPI_SS_CLR();
        USPI_master_write_data(AT45_PAGE_ERASE);
        at45_send_address(page, 0x00);
        USPI_SS_SET();
    }
}

uint8_t at45_status_read(void)
{
    uint8_t data = 0;
    USPI_SS_CLR();
    USPI_master_write_data(AT45_STATUSR_READ);
    data = USPI_master_read_data();
    USPI_SS_SET();
    return data;
}


void at45_write_data(const char *s, uint16_t page, uint16_t b_buffer)
{
    if ((page <= 8192) || (b_buffer<= 1024)) {
        USPI_SS_CLR();
        USPI_master_write_data(AT45_MMPPROG_TRBUFF1);
        at45_send_address(page, b_buffer);
        while (*s)
            USPI_master_write_data(*s++);
        USPI_SS_SET();
    }
}

void at45_write_data_bf1(const char *s, uint16_t page, uint16_t b_buffer)
{
    if ((page <= 8192) || (b_buffer<= 1024)) {
        USPI_SS_CLR();
        USPI_master_write_data(AT45_MMPPROG_TRBUFF1);
        at45_send_address(page, b_buffer);
        while (*s)
            USPI_master_write_data(*s++);
        USPI_SS_SET();
    }
}

void at45_write_data_bf2(const char *s, uint16_t page, uint16_t b_buffer)
{
    if ((page <= 8192) || (b_buffer<= 1024)) {
        USPI_SS_CLR();
        USPI_master_write_data(AT45_MMPPROG_TRBUFF2);
        at45_send_address(page, b_buffer);
        while (*s)
            USPI_master_write_data(*s++);
        USPI_SS_SET();
    }
}


void at45_read_data(uint16_t page, uint16_t b_buffer, uint8_t numb_bytes, uint8_t *save_buffer)
{
    if ((page <= 8192) || (b_buffer<= 1024)) {
        uint16_t i;
        USPI_SS_CLR();
        USPI_master_write_data(AT45_CONTAR_READ_LOF);
        at45_send_address(page, b_buffer);
        for(i=0; i< numb_bytes; i++)
            save_buffer[i] = USPI_master_read_data();
        USPI_SS_SET();
    }
}

uint8_t at45_read_one_byte(uint16_t page, uint16_t b_buffer)
{
    if ((page <= 8192) || (b_buffer<= 1024)) {
        uint8_t data = 0;
        USPI_SS_CLR();
        USPI_master_write_data(AT45_CONTAR_READ_LOF);
        at45_send_address(page, b_buffer);
        data = USPI_master_read_data();
        USPI_SS_SET();
        return data;
    }
    return 0;
}





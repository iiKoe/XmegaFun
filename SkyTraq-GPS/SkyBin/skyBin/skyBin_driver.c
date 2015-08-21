/*
 * skyBin_driver.c
 *
 * Created: 12-3-2013 15:57:03
 *  Author: Vito
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include "skyBin_driver.h"
#include "../UART/usart_driver.h"

static inline uint8_t skyBin_checkOutpStart(const uint8_t c);
static inline uint8_t skyBin_checkOutpStop(const uint8_t c);
static uint8_t skyBin_checkValid(const uint8_t *s);
static void skyBin_calcSpeed(void);
static inline void skyBin_decode(uint8_t *s);
static void skyBin_toString (void);

static inline uint8_t skyBin_checkOutpStart(const uint8_t c)
{
    static bool CharValid = false;
    if (c==0xa1 && CharValid==true) {
        CharValid = false;
        return 1;
    } else if (c==0xa0) CharValid = true;
    else CharValid = false;
    return 0;
}

static inline uint8_t skyBin_checkOutpStop(const uint8_t c)
{
    static bool CharValid = false;
    if (c==0x0a && CharValid==true) {
        CharValid = false;
        return 1;
    } else if (c==0x0d) CharValid = true;
    else CharValid = false;
    return 0;
}


void skyBin_fillBuffer(void)
{
    uint8_t data_gps;
    static uint8_t i=0;
    static bool gps_inData = false;
    static uint8_t skyBinBuffer[100];

    if (USART_RXBufferData_Available(&USART_data)) {
        data_gps = USART_RXBuffer_GetByte(&USART_data);

        if (skyBin_checkOutpStart(data_gps) && gps_inData==false)
            gps_inData = true;
        else if (skyBin_checkOutpStop(data_gps) && gps_inData==true) {
            gps_inData = false;
            i=0;
            if (skyBin_checkValid(skyBinBuffer)) {
                PORTE.OUTTGL =PIN0_bm;
                skyBin_decode(skyBinBuffer);
                skyBin_toString();
                skyBin_calcSpeed();
                printAll_UART();
            }
        } else if (gps_inData == true)
            skyBinBuffer[i++] = data_gps;

        else {
            //USART_Puts("Else in loop: ");
        }
    }
}

static uint8_t skyBin_checkValid(const uint8_t *s)
{
    uint8_t i=0;
    uint16_t payLoad = 0;
    uint8_t checksumByte = 0;
    uint8_t checksumCalc = 0;

    SKYBIN_COMBINE_16(payLoad, s[0], s[1]);
    for(i=PAYLOAD_BEGIN; i<payLoad+PAYLOAD_BEGIN; i++)
        checksumCalc ^= s[i];
    checksumByte = s[i];
    if ((checksumCalc == checksumByte) && checksumByte>0) return 1;
    else return 0;
}

static void skyBin_calcSpeed(void)
{
    double vx, vy, vz = 0;

    vx = skyBin.ecef_vx;
    vx = vx/100;
    vy = skyBin.ecef_vy;
    vy = vy/100;
    vz = skyBin.ecef_vz;
    vz = vz/100;

    skyBin_calc.gps_speed = (3.6*(sqrt((square(vx)+square(vy)+square(vz)))));

}


static inline void skyBin_decode(uint8_t *s)
{
    SKYBIN_COMBINE_8 (skyBin.fix_mode,  s[BC]);
    SKYBIN_COMBINE_8 (skyBin.number_sv, s[BC+1]);
    SKYBIN_COMBINE_16(skyBin.gps_week,  s[BC+2],  s[BC+3]);
    SKYBIN_COMBINE_32(skyBin.gps_tow,   s[BC+4],  s[BC+5],  s[BC+6],  s[BC+7]);
    SKYBIN_COMBINE_32(skyBin.latitude,  s[BC+8],  s[BC+9],  s[BC+10], s[BC+11]);
    SKYBIN_COMBINE_32(skyBin.longitude, s[BC+12], s[BC+13], s[BC+14], s[BC+15]);
    SKYBIN_COMBINE_32(skyBin.eps_alt,   s[BC+16], s[BC+17], s[BC+18], s[BC+19]);
    SKYBIN_COMBINE_32(skyBin.sea_alt,   s[BC+20], s[BC+21], s[BC+22], s[BC+23]);
    SKYBIN_COMBINE_16(skyBin.gdop,      s[BC+24], s[BC+25]);
    SKYBIN_COMBINE_16(skyBin.pdop,      s[BC+26], s[BC+27]);
    SKYBIN_COMBINE_16(skyBin.hdop,      s[BC+28], s[BC+29]);
    SKYBIN_COMBINE_16(skyBin.vdop,      s[BC+30], s[BC+31]);
    SKYBIN_COMBINE_16(skyBin.tdop,      s[BC+32], s[BC+33]);
    SKYBIN_COMBINE_32(skyBin.ecef_x,    s[BC+34], s[BC+35], s[BC+36], s[BC+37]);
    SKYBIN_COMBINE_32(skyBin.ecef_y,    s[BC+38], s[BC+39], s[BC+40], s[BC+41]);
    SKYBIN_COMBINE_32(skyBin.ecef_z,    s[BC+42], s[BC+43], s[BC+44], s[BC+45]);
    SKYBIN_COMBINE_32(skyBin.ecef_vx,   s[BC+46], s[BC+47], s[BC+48], s[BC+49]);
    SKYBIN_COMBINE_32(skyBin.ecef_vy,   s[BC+50], s[BC+51], s[BC+52], s[BC+53]);
    SKYBIN_COMBINE_32(skyBin.ecef_vz,   s[BC+54], s[BC+55], s[BC+56], s[BC+57]);
}

static void skyBin_toString (void)
{
    uint8_t strlen_buffer=0;
    SKYBIN_TOSTR_UE0(fix_mode);
    SKYBIN_TOSTR_UE0(number_sv);
    SKYBIN_TOSTR_UE0(gps_week);
    SKYBIN_TOSTR_UE2_32(gps_tow);
    SKYBIN_TOSTR_DE7_32(latitude);
    SKYBIN_TOSTR_DE7_32(longitude);
    SKYBIN_TOSTR_UE2_32(eps_alt);
    SKYBIN_TOSTR_UE2_32(sea_alt);
    SKYBIN_TOSTR_UE2_16(gdop);
    SKYBIN_TOSTR_UE2_16(pdop);
    SKYBIN_TOSTR_UE2_16(hdop);
    SKYBIN_TOSTR_UE2_16(vdop);
    SKYBIN_TOSTR_UE2_16(tdop);
    SKYBIN_TOSTR_DE2_32(ecef_x);
    SKYBIN_TOSTR_DE2_32(ecef_y);
    SKYBIN_TOSTR_DE2_32(ecef_z);
    SKYBIN_TOSTR_DE2_32(ecef_vx);
    SKYBIN_TOSTR_DE2_32(ecef_vy);
    SKYBIN_TOSTR_DE2_32(ecef_vz);
}

void printAll_UART(void)
{
    //USART_Puts("fix Mode= "); USART_Puts(skyBin_str.fix_mode);
    //USART_Puts(", Number sv= "); USART_Puts(skyBin_str.number_sv);
    USART_Puts(" Gps Week= "); USART_Puts(skyBin_str.gps_week);
    USART_Puts(" Gps tow= ");
    USART_Puts(skyBin_str.gps_tow);
/*
    USART_Puts(", Lat= ");
    USART_Puts(skyBin_str.latitude);
    USART_Puts(", Long= ");
    USART_Puts(skyBin_str.longitude);*/
    //USART_Puts(", Eps Alt= "); USART_Puts(skyBin_str.eps_alt);
    USART_Puts(" Sea Alt= "); USART_Puts(skyBin_str.sea_alt);
    //USART_Puts(", gdop= "); USART_Puts(skyBin_str.gdop);
    //USART_Puts(", pdop= "); USART_Puts(skyBin_str.pdop);
    //USART_Puts(", hdop= "); USART_Puts(skyBin_str.hdop);
    //USART_Puts(", vdop= "); USART_Puts(skyBin_str.vdop);
    //USART_Puts(", tdop= "); USART_Puts(skyBin_str.tdop);
    //USART_Puts(", ecef_x= "); USART_Puts(skyBin_str.ecef_x);
    //USART_Puts(", ecef_y= "); USART_Puts(skyBin_str.ecef_y);
    //USART_Puts(", ecef_z= "); USART_Puts(skyBin_str.ecef_z);
    //USART_Puts(", ecef_vx= "); USART_Puts(skyBin_str.ecef_vx);
    //USART_Puts(", ecef_vy= "); USART_Puts(skyBin_str.ecef_vy);
    USART_Puts(", ecef_vz= "); USART_Puts(skyBin_str.ecef_vz);
    //USART_Puts(", SPEED km/u= "); USART_Puts(skyBinSeedBuffer);
    USART_Puts("\n");
}
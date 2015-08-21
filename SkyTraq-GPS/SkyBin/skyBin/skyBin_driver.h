/*
 * skyBin_driver.h
 *
 * Created: 12-3-2013 15:57:22
 *  Author: Vito
 */


#ifndef SKYBIN_DRIVER_H_
#define SKYBIN_DRIVER_H_

typedef struct {
    /* structs for binary gps data */
    uint8_t  fix_mode;  // [1] 0-3
    uint8_t  number_sv; // [1] 0-12
    uint16_t gps_week;  // [2] week number
    uint32_t gps_tow;   // [4] 1/100 sec.
    int32_t  latitude;  // [4] 1/1e-7 degree
    int32_t  longitude; // [4] 1/1e-7 degree
    uint32_t eps_alt;   // [4] 1/100 meter
    uint32_t sea_alt;   // [4] 1/100 meter
    uint16_t gdop;      // [2] 1/100
    uint16_t pdop;      // [2] 1/100
    uint16_t hdop;      // [2] 1/100
    uint16_t vdop;      // [2] 1/100
    uint16_t tdop;      // [2] 1/100
    int32_t  ecef_x;    // [4] 1/100 meter
    int32_t  ecef_y;    // [4] 1/100 meter
    int32_t  ecef_z;    // [4] 1/100 meter
    int32_t  ecef_vx;   // [4] 1/100 m/s
    int32_t  ecef_vy;   // [4] 1/100 m/s
    int32_t  ecef_vz;   // [4] 1/100 m/s

} t_skyBin;
t_skyBin skyBin;



typedef struct {
    /* structs for post calculated gps data */
    double gps_speed;

} t_skyBin_calc;
t_skyBin_calc skyBin_calc;


#define STRLEN_U8    3   //12 is het hoogst getal + '\0'
#define STRLEN_U16   7   // 5 getallen + komma + '\0'
#define STRLEN_U32  12   //10 getallen + komma + '\0'
#define STRLEN_16    8   // 5 getallen + '+/-' + komma + '\0'
#define STRLEN_32   13   //10 getallen + '+/-' + komma + '\0'

typedef struct {
    /* structs for binary gps data */
    char fix_mode[STRLEN_U8];
    char number_sv[STRLEN_U8];
    char gps_week[STRLEN_U16];
    char gps_tow[STRLEN_U32];
    char latitude[STRLEN_32];
    char longitude[STRLEN_32];
    char eps_alt[STRLEN_U32];
    char sea_alt[STRLEN_U32];
    char gdop[STRLEN_U16];
    char pdop[STRLEN_U16];
    char hdop[STRLEN_U16];
    char vdop[STRLEN_U16];
    char tdop[STRLEN_U16];
    char ecef_x[STRLEN_32];
    char ecef_y[STRLEN_32];
    char ecef_z[STRLEN_32];
    char ecef_vx[STRLEN_32];
    char ecef_vy[STRLEN_32];
    char ecef_vz[STRLEN_32];

} t_skyBin_str;
t_skyBin_str skyBin_str;


#define BC 3
#define PAYLOADB(_payloadb) (_payloadb+1)
#define PAYLOAD_BEGIN 2

#define SKYBIN_COMBINE_8(_struct, _val1)                        (_struct = _val1)
#define SKYBIN_COMBINE_16(_struct, _val1, _val2)                (_struct = ((uint16_t)_val1<<8)|((uint16_t)_val2<<0))
#define SKYBIN_COMBINE_32(_struct, _val1, _val2, _val3, _val4)  (_struct = ((uint32_t)_val1<<24)|((uint32_t)_val2<<16)|((uint32_t)_val3<<8)|((uint32_t)_val4<<0))


#define ROOM_TO_MAKE        1
#define STRLEN_DEC_100(_struct) (strlen(_struct)-2)
#define STRLEN_DEC_E7(_struct)  (strlen(_struct)-7)



#define SKYBIN_TOSTR_UE0(_struct)   (sprintf(skyBin_str._struct, "%02u", skyBin._struct))

#define SKYBIN_TOSTR_UE2_16(_struct)	{   \
											sprintf(skyBin_str._struct, "%04u", skyBin._struct); \
											strlen_buffer = STRLEN_DEC_100(skyBin_str._struct);\
											memmove(skyBin_str._struct+ROOM_TO_MAKE+strlen_buffer,\
													skyBin_str._struct+strlen_buffer,\
													strlen_buffer+3-(ROOM_TO_MAKE+strlen_buffer));\
											skyBin_str._struct[strlen_buffer] = '.';\
										}

#define SKYBIN_TOSTR_UE2_32(_struct)	{   \
											sprintf(skyBin_str._struct, "%04lu", skyBin._struct); \
											strlen_buffer = STRLEN_DEC_100(skyBin_str._struct);\
											memmove(skyBin_str._struct+ROOM_TO_MAKE+strlen_buffer,\
													skyBin_str._struct+strlen_buffer,\
													strlen_buffer+3-(ROOM_TO_MAKE+strlen_buffer));\
											skyBin_str._struct[strlen_buffer] = '.';\
										}


#define SKYBIN_TOSTR_DE2_32(_struct)	{   \
											sprintf(skyBin_str._struct, "%04ld", skyBin._struct);\
											strlen_buffer = STRLEN_DEC_100(skyBin_str._struct);\
											memmove(skyBin_str._struct+ROOM_TO_MAKE+strlen_buffer,\
													skyBin_str._struct+strlen_buffer,\
													strlen_buffer+3-(ROOM_TO_MAKE+strlen_buffer));\
											skyBin_str._struct[strlen_buffer] = '.';\
										}

#define SKYBIN_TOSTR_DE7_32(_struct)	{  \
											sprintf(skyBin_str._struct, "%04ld", skyBin._struct);\
											strlen_buffer = STRLEN_DEC_E7(skyBin_str._struct);\
											memmove(skyBin_str._struct+ROOM_TO_MAKE+strlen_buffer,\
													skyBin_str._struct+strlen_buffer,\
													strlen_buffer+8-(ROOM_TO_MAKE+strlen_buffer));\
											skyBin_str._struct[strlen_buffer] = '.';\
										}

void printAll_UART(void);
void skyBin_fillBuffer(void);


#endif /* SKYBIN_DRIVER_H_ */
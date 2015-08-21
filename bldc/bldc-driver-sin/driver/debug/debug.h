/*
 * Debug related settings for BLDC driver
 *  It's not deeply implemented, but the base is there.
 * By: Vito
 */
#ifndef __DEBUG_H__
#define __DEBUG_H__

#include "bldc_config.h"

#define DEBUG_OUT gCtrl_IO

#define INFO_MSG        "#INF "
#define STD_MSG         "#STD "
#define DEBUG_MSG       ">DBG "
#define ERROR_MSG       "*ERR "
#define PANNIC_MSG      "+PAN "
#define FATAL_MSG       "+FTL "

#ifdef BLDC_DEBUG
int printf_debug(const char *format, ...);
int printf_debug_OPC(const char *opcode, const char *format, ...);
#define DEBUG_PRINTF    printf_debug
#define MSG_PRINTF      printf_debug_OPC
#else
#define DEBUG_PRINTF    (void)
#endif /*BLDC_DEBUG*/

#endif /*__DEBUG_H__*/

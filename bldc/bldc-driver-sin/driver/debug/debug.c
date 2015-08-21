/*
 * Debug code for BLDC driver
 *
 * By: Vito
 */

#include <stdio.h>
#include <stdarg.h>
#include "debug.h"

/*
 * Print function for debug.
 */
int printf_debug(const char *format, ...)
{
    int res;
    va_list args;

    va_start(args, format);
    res = vfprintf(&DEBUG_OUT, format, args);
    va_end(args);

    return res;
}

/*
 * Print function for debug with specified opcode.
 */
int printf_debug_OPC(const char *opcode, const char *format, ...)
{
    int res;
    va_list args;

    fprintf(&DEBUG_OUT, "%s", opcode);

    va_start(args, format);
    res = vfprintf(&DEBUG_OUT, format, args);
    va_end(args);

    return res;
}

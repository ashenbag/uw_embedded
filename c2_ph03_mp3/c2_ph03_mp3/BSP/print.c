/*
 * MODULE
 *
 *   print.c
 *
 * DESCRIPTION
 *
 * Very lightweight print functions.
 *
****************************************************************************/
#define __PRINT_C__

//#include <stdio.h>
//???#include <stdint.h>
#include "includes.h"
#include "print.h"
#include "lpc2378.h"
#include "uarts.h"

void printHex(INT32U u32)
{
    INT32U   u32Mask  = 0xF0000000;
    INT8U    u32Shift = 32;
    INT32U   u32Char;

    do {
        u32Shift -= 4;
        u32Char = (u32 & u32Mask) >> u32Shift;
        u32Mask >>= 4;
        if (u32Char >= 0xA)
        {
            PUTC('A' + (u32Char - 10));
        }
        else
        {
            PUTC('0' + u32Char);
        }
    } while (u32Shift > 0);
}

void print_uint32(uint32_t u)
{
    INT32U    buffer_length = 20;
    char      buffer[buffer_length];
    char      *p = &buffer[buffer_length - 1];

    *p = '\0';
    do {
        p--;
        *p = (u % 10) + '0';
        u /= 10;
    } while (u > 0);

    printString(p);
}

void printStringHex(char *ptr, uint32_t u32Hex)
{
    printString(ptr);
    printHex(u32Hex);
    printChar('\n');
}

void printChar(const INT8U letter)
{
    if (0 == letter)
    {
        return;
    }
    if ('\n' == letter)
    {
        PUTC('\n');
        PUTC('\r');
        return;
    }
    PUTC(letter);
    return;
}


/*
 * NAME
 *
 *  printString
 *
 * DESCRIPTION
 *
 *  Standard printString function
 *
 * PARAMETERS
 *
 *  none...
 *
 * EXAMPLE
 *
 *  printString("hello embedded world");
 *
 * NOTES
 *
 *  intercepts '\n' and adds '\r'  (LF+CR)
 *
 */

void printString(char *ptr)
{
    if (ptr==0 || *ptr==0) return;

    do
    {
      if (*ptr=='\n')
      {
      PUTC(*ptr++);
      PUTC('\r');
      }
      else
      {
      PUTC(*ptr++);
      }
    }
    while (*ptr!=0);
}
/*** EOF ***/

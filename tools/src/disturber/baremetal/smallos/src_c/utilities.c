/******************************************************************************

  Copyright (c) 2001-2009, Intel Corporation
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

  1. Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.

  2. Redistributions in binary form must reproduce the above copyright
  notice, this list of conditions and the following disclaimer in the
  documentation and/or other materials provided with the distribution.

  3. Neither the name of the Intel Corporation nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.

 ******************************************************************************/
/*
 * Utility support functions including video console operations.
 */
#include <param.h>
#include <utilities.h>
#include <stdarg.h>
#include <stdlib.h>
#include <console.h>
#include <spinlock.h>
#include <lwrte_config.h>

extern spinlock_t apPrintLock;

/*
 * Global variables.
 */
#ifdef CONSOLE_PRINT
u8 scr_row = 0;
u8 scr_col = 0;
u8 def_bg = CLR_BLACK;
u8 def_fg = CLR_WHITE;
u16 *video = (u16 *)0xB8000;

extern struct lwConsole defConsole;
u8 int_buf[32];
u8 tmp_buf[32];
#endif

void outb(u16 port, u8 val)
{
    asm volatile("outb %1, %0" : : "dN" (port), "a" (val));
}

u8 inb(u16 port)
{
    u8 ret;

    asm volatile("inb %1, %0" : "=a" (ret) : "dN" (port));
    return ret;
}

void outw(u16 port, u16 val)
{
    asm volatile("outw %1, %0" : : "dN" (port), "a" (val));
}

u16 inw(u16 port)
{
    u16 ret;

    asm volatile ("inw %1, %0" : "=a" (ret) : "dN" (port));

    return ret;
}

void outl(u16 port, u32 val)
{
    asm volatile ("outl %1, %0" :: "dN"(port), "a"(val));

} /* End of outl */

u32 inl(u16 port)
{
    u32 ret;

    asm volatile ("inl %1, %0" : "=a" (ret): "dN"(port));
    return ret;
}

void memset(u8 *dst, u8 val, u32 size)
{
    u8 *tmp;
    u32 indx;

    for (indx = 0; indx < size; indx++)
        *dst++ = val;
}

int memcmp(u8 *src, u8 *dst, u32 size)
{
    while (size)
    {
        if (*src == *dst)
        {
            src++;
            dst++;
        }
        else
        {
            if (*src > *dst)
                return (1);
            else
                return (-1);
        }
        size--;
    }

    return (0);
} 

void memcpy(u8 *dst, u8 *src, u32 size)
{
    u32 indx;

    for (indx = 0; indx < size; indx++)
    {
        *dst++ = *src++;
    }
}

unsigned char lw_itoa(unsigned long val, u8 base)
{
    if (base == 10)
    {
        if ((val >= 0)  && (val <= 9))
        {
            return ('0' + val);
        }
    }
    if (base == 16)
    {
        if ((val >= 0) && (val <= 9))
        {
            return ('0' + val);
        }
        if ((val >= 10) && (val <= 15))
        {
            return ('a' + (val - 10));
        }
    }
    return ('X');
}

/* Convert the integer D to a string and save the string in BUF. If
   BASE is equal to 'd', interpret that D is decimal, and if BASE is
   equal to 'x', interpret that D is hexadecimal.  */
void itoa (u8 *buf, int base, int d)
{
    u8 *p;
    u8 *p1, *p2;
    unsigned long ud = d;
    int divisor = 10;
    u8  tmp_buf[32];
    u32 offset;

    p = tmp_buf;
    /* If %d is specified and D is minus, put `-' in the head.  */
    if (base == 'd' && d < 0)
    {
        *p++ = '-';
        buf++;
        ud = -d;
    }
    else if (base == 'x')
        divisor = 16;

    /* Divide UD by DIVISOR until UD == 0.  */
    do
    {
        int remainder = ud % divisor;

        *p++ = (remainder < 10) ? remainder + '0' : remainder + 'a' - 10;
    }
    while (ud /= divisor);

    /* Terminate BUF.  */
    *p = 0;

    buf = strrev(tmp_buf, buf);
}

/*
 * strlen
 * Need to move these functions to a different file
 */
u32 strlen (u8 *instr)
{
    u32 indx;

    indx = 0;
    while (*instr++)
        indx++;
    return indx;

} /* end of strlen */


/*
 * strncpy 
 * 
 * Need to fix the function name, but we want to copy to destination
 * starting at a particular point.
 *
 */
u8 * strncpy(u8 *dst, u8 *src, u32 offset)
{
    u32 indx;

    indx = offset;
    while (*src)
    {
        dst[indx++] = *src;
        src++;
    }
    dst[indx] = '\0';
    return dst;
}

/*
 * Copy source to destintation reversing it start copying at the 
 * specified offset. Destination is expected to contain sufficient
 * space to hold the source. If not we are screwed.
 */

u8 *strrev(u8 *src, u8 *dst)
{
    u8 *retptr;
    u32 len;
    u32 dIndx = 0;
    u32 sIndx = 0;

    len = strlen(src);
    sIndx = len-1;
    retptr = dst;
    while (len)
    {
        dst[dIndx] = src[sIndx];
        sIndx--;
        dIndx++;
        len--;
    }
    dst[dIndx] = '\0';
    return retptr;
}

/*
 * convert from ascii to integer.
 * We assume we don't need to skip white spaces.
 */

unsigned int atoi(u8 *str, int base)
{
    int val;
    int indx;
    int len;
    int negFlag = 1;
    u8  ch;
    u8  tval;

    /*
     * If we have a leading -, + or a 0x we will skip them.
     */
    len = strlen(str);
    val = 0;
    indx = 0;
    if ((str[0] == '0') && ((str[1] == 'x') || (str[1] == 'X')))
    {
        indx += 2;
    }
    if (str[0] == '-') 
    {
        negFlag = -1;
        indx += 1;
    }
    if (str[1] == '+')
    {
        indx += 1;
    }

    for (;indx < len; indx++)
    {

        ch = str[indx];
        if (IS_NUM(ch))
        {
            tval = ch - '0';
        }
        else
        {
            if (IS_HEX_UPPER(ch))
            {
                tval = (ch - 'A') + 10;
            }
            else
            {
                if (IS_HEX_LOWER(ch))
                {
                    tval = (ch - 'a') + 10;
                }
            }

        }
        val *= base;
        val += tval;
    }

    return (val * negFlag);
}

#ifdef CONSOLE_PRINT
void printStr(u8 *str)
{
    while(*str)
    {
        defConsole.putchar(defConsole.lwConsoleHndle, *str);
        str++;
    }
}

void console_printf (u8 *fmt, va_list args)
{
    va_list ap = args;
    u8 *fmt_str;
    u8 ch;
    u32 intval;
    u8  int_base;
    u8 *pStr;
    u8  *str;
    u8 leadFill = 0;
    u8 leadZero = 0;

    u32 slen;

    spinlock_lock (&apPrintLock);

    fmt_str = fmt;
    leadFill = 0;
    leadZero = FALSE;

    while (*fmt_str)
    {
        if (*fmt_str != '%')
        {
            defConsole.putchar(defConsole.lwConsoleHndle, *fmt_str);
        }
        else
        {
pickNextChar:    
            ch  = *++fmt_str;
            switch (ch)
            {
                case 'd':
                case 'i':
                case 'x':
                    // Pick the next argument.
                    intval = va_arg(ap, int);
                    if (leadZero == TRUE)
                    {
                        leadZero = FALSE;
                        memset(int_buf, '0', 32);
                    }
                    itoa(tmp_buf, (int)ch, intval);
                    /* If we have to put in leading zeros 
                       then we copy over to a new buffer.
                       */
                    pStr = tmp_buf;
                    slen = strlen(tmp_buf);
                    if (leadFill > slen)
                    {
                        strncpy(int_buf, tmp_buf, leadFill - slen);
                        leadFill = 0;
                        pStr = int_buf;
                    }
                    printStr(pStr);
                    break;
                case 'c':
                    ch = (u8 )va_arg(ap, int);
                    defConsole.putchar(defConsole.lwConsoleHndle, ch);
                    break;
                case 's':
                    str = va_arg(ap, u8 *);
                    printStr(str);
                    break;
                case '0':
                    leadZero = TRUE;
                    goto pickNextChar;
                case '2':
                    leadZero = TRUE;
                    leadFill = 2;
                    goto pickNextChar;
                case '4':
                    leadZero = TRUE;
                    leadFill = 4;
                    goto pickNextChar;
                case '8':
                    leadZero = TRUE;
                    leadFill = 8;
                    goto pickNextChar;
                default:
                    defConsole.putchar(defConsole.lwConsoleHndle, *str);
                    break;

            }

        }
        fmt_str++;

    } /* End of while fmt_str */
    spinlock_unlock(&apPrintLock);
}
#endif


void kprintf(char * fmt, ...)
{
    va_list args;
    va_start(args,fmt);
#ifdef SERIAL_PRINT
    serial_printf(fmt,args);
#endif
#ifdef CONSOLE_PRINT
    console_printf(fmt, args);
#endif
    va_end(args);
}

typedef unsigned long long ticks;

__inline__ ticks getticks(void)
{
     unsigned a, d;
     asm volatile("rdtsc" : "=a" (a), "=d" (d));

     return (((ticks)a) | (((ticks)d) << 32));
}

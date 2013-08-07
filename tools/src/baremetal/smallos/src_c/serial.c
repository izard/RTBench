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
 * ========================================================================
 *
 *       Filename:  serial.c
 *
 *    Description:  Initialize serial communication
 *
 *        Version:  1.0
 *        Created:  04/22/2010 11:54:51 PM
 *       Revision:  none
 *       Compiler:  icc
 *
 *        Company:  Intel
 *
 * ========================================================================
 */
#include <serial.h>
#include <spinlock.h>
#include <lwrte_config.h>

extern spinlock_t apPrintLock;
u8 int_buf[32];
u8 tmp_buf[32];

void init_serial() {
    outb(PORT + 1, 0x00);    // Disable all interrupts
    outb(PORT + 3, 0x80);    // Enable DLAB (set baud rate divisor)
    outb(PORT + 0, 0x03);    // Set divisor to 3 (lo byte) 38400 baud
    outb(PORT + 1, 0x00);    //                  (hi byte)
    outb(PORT + 3, 0x03);    // 8 bits, no parity, one stop bit
    outb(PORT + 2, 0xC7);    // Enable FIFO, clear them, with 14-byte threshold
    outb(PORT + 4, 0x0B);    // IRQs enabled, RTS/DSR set
    kprintf("\r\nSmallOS v%s\r\n",SMALLOS_VERSION);
}

static int serial_received() {
    return inb(PORT + 5) & 1;
}

char read_serial() {
    while (serial_received() == 0);

    return inb(PORT);
}

static int is_transmit_empty() {
    return inb(PORT + 5) & 0x20;
}

static void serial_putchar(char a) {
    while (is_transmit_empty() == 0);
    if(a == '\n')
        outb(PORT,'\r');
    outb(PORT,a);
}

static void printStr(u8 *str)
{
    u8 * ptr = str;

    while(*ptr)
    {
        serial_putchar(*ptr);
        ptr++;
    }
}

static void printHex(u32 n)
{
    s32int tmp;

    char noZeroes = 1;

    int i;
    for (i = 28; i > 0; i -= 4)
    {
        tmp = (n >> i) & 0xF;
        if (tmp == 0 && noZeroes != 0)
        {
            continue;
        }
    
        if (tmp >= 0xA)
        {
            noZeroes = 0;
            serial_putchar(tmp-0xA+'a');
        }
        else
        {
            noZeroes = 0;
            serial_putchar(tmp+'0');
        }
    }
  
    tmp = n & 0xF;
    if (tmp >= 0xA)
    {
        serial_putchar(tmp-0xA+'a');
    }
    else
    {
        serial_putchar(tmp+'0');
    }

}

static void printDec(u32 n)
{

    if (n == 0)
    {
        serial_putchar('0');
        return;
    }

    s32int acc = n;
    u8 c[32];
    int i = 0;
    while (acc > 0)
    {
        c[i] = '0' + acc%10;
        acc /= 10;
        i++;
    }
    c[i] = 0;

    u8 c2[32];
    c2[i--] = 0;
    int j = 0;
    while(i >= 0)
    {
        c2[i--] = c[j++];
    }
    printStr(c2);
}

/* Kernel printf */
#if 0
void serial_printf (u8 *fmt, va_list ap)
{
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
            serial_putchar(*fmt_str);
        }
        else
        {
pickNextChar:
            ch  = *++fmt_str;
            switch(ch)
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
                    serial_putchar(ch);
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
                    serial_putchar(*str);
                    break;
            }
        }
        fmt_str++;
    } /* End of while fmt_str */

    spinlock_unlock(&apPrintLock);
}
#endif 

void serial_printf (u8 *fmt, va_list ap)
{
    u8 *fmt_str;
    u8 ch;
    u32 intval;
    u8  *str;
    u32 slen;

    spinlock_lock (&apPrintLock);

    fmt_str = fmt;

    while (*fmt_str)
    {
        if (*fmt_str != '%')
        {
            serial_putchar(*fmt_str);
        }
        else
        {
            ch  = *++fmt_str;
            if(ch == 'd')
            {
                intval = va_arg(ap,int);
                printDec(intval);
            }
            else if(ch == 'x')
            {
                intval = va_arg(ap,int);
                printHex(intval);
            }
            else if(ch == 'c')
            {
                ch = (u8) va_arg(ap, int);
                serial_putchar(ch);
            }
            else if(ch == 's')
            {
                str = va_arg(ap, u8 *);
                printStr(str);
            }
        }
        fmt_str++;
    } /* End of while fmt_str */

    spinlock_unlock(&apPrintLock);
}

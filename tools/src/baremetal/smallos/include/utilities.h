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
#ifndef __UTILITIES_H__
#define __UTILITIES_H__

#define COLS  80   
#define ROWS  25
#define VIDEO_MEM  0xB8000
#define ATTRIBUTE  0x0F00

/* Screen colors.
 */
#define CLR_BLACK    0 
#define CLR_BLUE     1
#define CLR_GREEN    2
#define CLR_CYAN     3
#define CLR_RED      4
#define CLR_MAGENTA  5
#define CLR_BROWN    6
#define CLR_LGREY    7
#define CLR_DGREY    8
#define CLR_LBLUE    9
#define CLR_LGREEN   10
#define CLR_LCYAN    11
#define CLR_LRED     12
#define CLR_LMAGENTA 13
#define CLR_LBROWN   14
#define CLR_WHITE    15

#define inc_cursor(x,y) y++;\
  if (y > 80){ y = 0;x++;} \
  if (x > 25){ x = 0;y=0;} 
  

#define inc_row(x) if (x<25) x++; else x = 0;
#define inc_col(x) if (x<80) x++; else x = 0;
#define dec_row(x) if (x> 0) x--; else x = 0;
#define dec_col(x) if (x> 0) x--; else x = 0;

#define SCR_HEXSIZE  32

void outb(u16 port, u8 val);
u8 inb(u16 port);
u16 inw(u16 port);
void memset(u8 *dst, u8 val, u32 size);
void memcpy(u8 *dst, u8 *src, u32 size);
static void putchar (int c);
void printf (const char *format, ...);
void itoa (u8 *buf, int base, int d);
void outl(u16 port, u32 val);
u32 inl(u16 port);
void outw(u16 port, u16 val);
void move_cursor (u8 cX, u8 cY);

u8 *strncpy(u8 *dst, u8 *src, u32 offset);
unsigned int strlen (u8 *instr);
u8 *strrev(u8 *src, u8 *dst);
void printStr(u8 *str);

unsigned int atoi(u8 *str, int base);
int memcmp(u8 *src, u8 *dst, u32 size);

#define CHK_BIT_SET(bitval,bit) ((bitval) & (1 << (bit)))

#endif /* __UTILITIES_H__ */

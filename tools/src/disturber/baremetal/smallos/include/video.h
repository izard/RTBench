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
#ifndef __VIDEO_H__
#define __VIDEO_H__

struct vga_console
{
  int row;              // Current Row
  int col;              // Current col
  unsigned char attr;   // current attribute
  u16 *video;
};

#define VGA_COLS  80   
#define VGA_ROWS  25
#define VIDEO_MEM  0xB8000
#define VGA_ATTRIBUTE  0x0F

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
  

#define VALID_ROW(x) ((x>=0 && (x) < VGA_ROWS))
#define VALID_COL(y) ((y>=0 && (y) < VGA_COLS))

#define inc_row(x) if (x<25) x++; else x = 0;
#define inc_col(x) if (x<80) x++; else x = 0;
#define dec_row(x) if (x> 0) x--; else x = 0;
#define dec_col(x) if (x> 0) x--; else x = 0;

#define SCR_HEXSIZE  32

#define VIDEO_ADDR_REG 0x3D4
#define VIDEO_DATA_REG 0x3D5
#define VIDEO_CURSOR_LOC_HIGH 0x0E
#define VIDEO_CURSOR_LOC_LOW  0x0F
#define VIDEO_CURSOR_START_REG 0x0A
#define VIDEO_CURSOR_END_REG   0x0B


void vga_movecursor(void *cons, int row, int col);
void vga_scroll (void  *cons, u8 scrollAt);
void vga_putchar(void *cons, 
  	 unsigned char ch);
void vga_clear(void *cons);
int vga_getxpos(void *cons);
int vga_getypos(void *cons);
int vga_getnumrows (void *cons);
int vga_getnumcols (void *cons);
void initVgaConsStruct (struct lwConsole *cons);
void vga_update_pos(struct vga_console *vga, u8 nlFlg);

#endif /* __VIDEO_H__ */

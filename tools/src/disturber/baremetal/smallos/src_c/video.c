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
 * This file implements the video console for standalone baremetal boot.
 */

#include <param.h>
#include <console.h>
#include <video.h>
#include <utilities.h>

struct vga_console vgaConsole;

void vga_movecursor(void  *cons, int row, int col)
{
  unsigned int curLoc;
  struct vga_console *vga;


  vga = (struct vga_console *)cons;

  /*
   * check whether the row is valid. if not we wrap around 
   * to row 0, similarly for column. 
   */

  vga->row = row;
  vga->col = col;

  if (!VALID_ROW(vga->row))
    vga->row = 0;
  if (!VALID_COL(vga->col))
    vga->col = 0;
  curLoc = vga->row * VGA_COLS + vga->col;

  outb(VIDEO_ADDR_REG, VIDEO_CURSOR_LOC_HIGH);
  outb(VIDEO_DATA_REG, curLoc >> 8);
  outb(VIDEO_ADDR_REG, VIDEO_CURSOR_LOC_LOW);
  outb(VIDEO_DATA_REG, curLoc & 0x00FF);
} /* end of vga_movecursor */



void vga_scroll (void  *cons, u8 scrollAt)
{
  u8 attrB = (CLR_BLACK << 4) | (CLR_WHITE & 0x0F);
  u16 blank = 0x20 | attrB << 8;
  u16 indx;
  struct vga_console *vga;

  

  vga = (struct vga_console *)cons;

  if (scrollAt == 0)
    return;
  
  for (indx = 0; indx < (scrollAt -1) * 80; indx++)
    {
      vga->video[indx] = vga->video[indx+80];
    }
  
  for (indx = (scrollAt - 1) * 80; indx < scrollAt * 80 ; indx++)
    {
      vga->video[indx] = blank;
    }
}

void vga_putchar(void  *cons, 
  	 u8 ch)

{
  unsigned int indx;
  struct vga_console *vga;


  vga = (struct vga_console *)cons;

  if ((ch == '\n') || (ch == '\r'))
    {
      vga_update_pos(vga, 1);
      return;
    }

  indx = vga->row * VGA_COLS + vga->col;

  vga->video[indx] = (u16) (vga->attr << 8) | ch;

  vga_update_pos(vga, 0);

  //  inc_cursor(vga->row, vga->col);


} /* vga_putchar */

void vga_clear(void *cons)
{
  u8 attrB = (0 << 4) | (15 & 0x0F);
  u16 blank = 0x20 | attrB;
  u16 indx;
  struct vga_console *vga;


  vga = (struct vga_console *)cons;

  for (indx = 0; indx < 80*25; indx++)
    {
      vga->video[indx] = blank;
    }
  
  vga->row = 0;
  vga->col = 0;
  vga_movecursor (cons, vga->row, vga->col);
}




int vga_getxpos(void *cons)
{
  
  struct vga_console *vga;

  vga = (struct vga_console *)cons;

  return (vga->row);
}


int vga_getypos(void *cons)
{
  struct vga_console *vga;

  vga = (struct vga_console *)cons;

  return (vga->col);

}

int vga_getnumrows (void *cons)
{
  return VGA_ROWS;

}

int vga_getnumcols (void *cons)
{

  return VGA_COLS;
}

void vga_init (void *cons)
{

  vgaConsole.row = 0;
  vgaConsole.col = 0;
  vgaConsole.attr = VGA_ATTRIBUTE;
  vgaConsole.video = (u16 *)VIDEO_MEM;
  outb(VIDEO_ADDR_REG, VIDEO_CURSOR_START_REG);
  outb(VIDEO_DATA_REG, 0x20);
  outb(VIDEO_ADDR_REG, VIDEO_CURSOR_END_REG);
  outb(VIDEO_DATA_REG, 0x1F);
}

void initVgaConsStruct (struct lwConsole *cons)
{
  
  
  cons->clear = &vga_clear;
  cons->putchar = &vga_putchar;
  cons->scroll = &vga_scroll;
  cons->move_cursor = &vga_movecursor;
  cons->get_xpos = &vga_getxpos;
  cons->get_ypos = &vga_getypos;
  cons->get_numrows = &vga_getnumrows;
  cons->get_numcols = &vga_getnumcols;
  cons->init = &vga_init;
  cons->lwConsoleHndle = &vgaConsole;
}
 

/*
 * Update the vga position.
 * depending on the current position we will scroll the screen up.
 */
void vga_update_pos(struct vga_console *vga, u8 nlFlg)
{
    if (nlFlg)
    {
        if (vga->row == 24)
        {
            vga->col = 0;
            vga_scroll((void *)vga, 25);
            vga_movecursor((void *)vga, vga->row, vga->col);
            return;
        }
        else
        {
            vga->col = 0;
            vga->row++;
            vga_movecursor((void *)vga, vga->row, vga->col);
            return;
        }
    }

  /*
   * If we are the last row, column, scroll up by one row.
   * Otherwise we just increment the row and col.
   */
  

  if ((vga->row == 24) && (vga->col == 79))
  {
    vga_scroll ((void *)vga, 25);
    vga->col = 0;
    return;
  }

  if (vga->col == 79)
    {
      vga->col = 0;
      vga->row++;
      return;
    }

  vga->col ++;
  vga_movecursor((void *)vga, vga->row, vga->col);
  return;
}

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
#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#define KEY_BUFFER_SIZE 4096
#define KEY_BUFFER_MASK (KEY_BUFFER_SIZE-1)


/*
 * create a generic structure for consoles. We will abstract this out so 
 * we can use the same interface for serial port as well as for video.
 *
 */

struct lwConsole
{
  u8 shiftState;
  u8 escCode;
  u8 numLock;
  u8 *keyBuffer;
  u32 keyBufferInIndx;
  u32 keyBufferOutIndx;
  void (*clear)(void *cons);
  void (*putchar)(void *cons, u8 ch);
  void (*scroll)(void *cons, u8 scrollAt);
  void (*move_cursor)(void *cons, int row, int col);
  int (*get_xpos)(void *cons);
  int (*get_ypos)(void *cons);
  int (*get_numrows)(void *cons);
  int (*get_numcols)(void *cons);
  void (*init)(void *cons);
  void *lwConsoleHndle;
};

#endif /* __CONSOLE_H__ */

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
#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

/*
 * We ideally need a proper or half decent terminal structure
 * etc. But no, we won't have that. We will live with this crap.
 */
#define SHIFT_RANGE_START 0x01
#define SHIFT_RANGE_END   0x3A


#define KEYBOARD_PORT  0x60
#define ESC_CODE       0x1B
#define BACK_SPACE     0x10
#define TAB            0x11
#define BACK_SLASH     0x5C
#define ENTER_KEY      0x1C
#define LEFT_CTRL      0x1D
#define LEFT_SHIFT     0x2A
#define LEFT_SHIFT_UNPRESSED 0x2A | 0x80
#define BACK_QUOTE     0x60
#define QUOTE          0x27
#define RIGHT_SHIFT    0x36
#define RIGHT_SHIFT_UNPRESSED 0x36 | 0x80
#define PRINT_SCREEN   0x37
#define LEFT_ALT       0x38
#define CAPS_LOCK      0x3A
#define CAPS_LOCK_PRESSED 0x3A
#define CAPS_LOCK_UNPRESSED 0x3A|0x80
#define F_1            0x3B
#define F_2            0x3C
#define F_3            0x3D
#define F_4            0x3E
#define F_5            0x3F
#define F_6            0x40
#define F_7            0x41
#define F_8            0x42
#define F_9            0x43
#define F_10           0x44
#define NUM_LOCK_PRESSED 0x45
#define NUM_LOCK_UNPRESSED 0xC5
#define SCROLL_LOCK 0x46
#define KEYPAD_HOME_7    0x47
#define KEYPAD_UP_8      0x48
#define KEYPAD_PGUP_9    0x49
#define KEYPAD_MINUS     0x4a
#define KEYPAD_LEFT_4    0x4b
#define KEYPAD_5         0x4C
#define KEYPAD_RIGHT_6   0x4D
#define KEYPAD_PLUS      0x4E
#define KEYPAD_END_1     0x4F
#define KEYPAD_DOWN_2    0x50
#define KEYPAD_PGDN_3    0x51
#define KEYPAD_INS_0     0x52
#define KEYPAD_DEL_DOT   0x53
#define F_11         0x57
#define F_12         0x58
#define DBL_QUOTE    0x22
#define ESCAPE_CODE  0xE0
#define SHIFT_BREAK  0xAA

#define SHIFT_PRESSED 0x01
#define ESCAPE_CODE_SET 0x01
#define SHIFT_UNPRESSED 0x00
#define KBD_BREAK_MASK  0x80
#define KBD_ERROR       0x00

#define SPACE           ' '
#define NEWLINE         '\n'
#define BS              0x08

#define KEYBOARD_BUFFER_SZ 4096
#define KEYBOARD_MASK      (KEYBOARD_BUFFER_SZ-1)

#define INC_KEYINDX(indx) indx=(indx+1)&KEYBOARD_MASK



#define KP_UP             0x80
#define KP_DOWN           0x81
#define KP_LEFT           0x82
#define KP_RIGHT          0x83
#define KP_INS            0x84
#define KP_HOME           0x85
#define KP_END            0x86
#define KP_PGUP           0x87
#define KP_PGDN           0x88
#define KP_DELETE         0x89
#define KP_ALT            0x8A
#define KP_LFT_WINDOW     0x8B
#define KP_RT_WINDOW      0x8C
#define KP_KEY_CTRL       0x8D
#define KP_KEY_ALT        0x8E


void setKeyboardBuffer (struct lwConsole *console);
void kbcISR(registers_t regs);
void kbcInit();

#endif /* __KEYBOARD_H__ */

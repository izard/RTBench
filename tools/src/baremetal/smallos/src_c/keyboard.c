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
 * This file contains the keyboard related functions.
 *
 * We maintain a circular buffer for keyboard input.
 * When a thread requests data we just make data available in the 
 * circular buffer to whoever requests it.
 *
 */

#include <param.h>
#include <multiboot.h>
#include <utilities.h>
#include <segments.h>
#include <page.h>
#include <console.h>
#include <video.h>
#include <mp.h>
#include <keyboard.h>

extern struct lwConsole defConsole;
volatile u8 keyBoardBuffer[KEYBOARD_BUFFER_SZ];
volatile u32 keyIndx;
volatile u8 escapePressed;
volatile u32 numCharsInBuffer;
volatile u8 shiftPressed;
volatile u8 capsLockPressed;
volatile u8 numLockPressed;

/*
 * We will provide two functions for user entities to interface 
 * a getchar () and get_keypress ();
 * 
 * Both of these will 

/*
 * These are the scan codes.
 */ 
                        
u8 no_shift_scan_map[] = {0x00,ESC_CODE,'1','2','3','4','5','6','7','8','9','0',
  		  '-','=',BS,'\t','q','w','e','r','t','y','u',
			  'i','o','p','[',']',NEWLINE,LEFT_CTRL,'a','s',
			  'd','f','g','h','j','k','l',';',QUOTE,BACK_QUOTE,
			  LEFT_SHIFT,BACK_SLASH,'z','x','c','v','b','n',
			  'm',',','.','/',RIGHT_SHIFT,PRINT_SCREEN,
			  LEFT_ALT,' ',CAPS_LOCK};


u8 shift_scan_map[] = {0x00,ESC_CODE,'!','@','#','$','%','^','&','*','(',')',
		       '_','+',BS,'\t','Q','W','E','R','T','Y','U',
		       'I','O','P','{','}',NEWLINE,LEFT_CTRL,'A','S',
		       'D','F','G','H','J','K','L',':',DBL_QUOTE,BACK_QUOTE,
		       LEFT_SHIFT,BACK_SLASH,'Z','X','C','V','B','N',
		       'M','<','>','?',RIGHT_SHIFT,PRINT_SCREEN,
		       LEFT_ALT,' ',CAPS_LOCK};


/*
 * MAP the numlock elements to what we need. 
 */
u8 numKBDScan[] = {'7','8','9','-','4','5','6','+','1','2','3','0','.'};
u8 numKBDScanNoLock[] = {KP_HOME, KP_UP, KP_PGUP,'-',
			KP_LEFT, '5',KP_RIGHT, '+',
			KP_END, KP_DOWN, KP_PGDN, KP_INS, KP_DELETE};


/*
 * In the ISR we grab all the data we can get in the buffer.
 * All scan codes are put in. We don't capture break codes.
 *
 * It will be up to the processing code to translate this to appropriate
 * keypress codes. Whether they consist of single or multiple key strokes.
 * We also store escape codes, but we don't store escape codes for key release.
 */


void kbcISR(registers_t regs)
{
  u8 scan_code;

  scan_code = inb(KEYBOARD_PORT);

  /*
   * The general process is some to look at the input code
   * and determine what to do.
   */
  switch (scan_code)
    {
    case KBD_ERROR:
      break;
    case ESCAPE_CODE:
      if (escapePressed==FALSE)
	{
	  escapePressed = TRUE;
	  keyBoardBuffer[keyIndx] = scan_code;
	  INC_KEYINDX(keyIndx);
	  numCharsInBuffer++;
	}
      else
	{
	  escapePressed = FALSE;
	}
      break;
    case NUM_LOCK_PRESSED:
    case NUM_LOCK_UNPRESSED:
    case CAPS_LOCK_PRESSED:
    case CAPS_LOCK_UNPRESSED:
    case LEFT_SHIFT_UNPRESSED:
    case RIGHT_SHIFT_UNPRESSED:
      keyBoardBuffer[keyIndx] = scan_code;
      INC_KEYINDX(keyIndx);
      numCharsInBuffer++;
      break;
    default:
      if (!(scan_code & KBD_BREAK_MASK))
	{
	  keyBoardBuffer[keyIndx] = scan_code;
	  INC_KEYINDX(keyIndx);
	  numCharsInBuffer++;
	}
      break;
    }
  return;
  
} /* kbcISR */


void kbcInit()
{

  keyIndx = 0;
  escapePressed = FALSE;
  numCharsInBuffer = 0;
  shiftPressed = FALSE;
  capsLockPressed = FALSE;
}


/*
 * Set the keyboard buffer.
 */

void setKeyboardBuffer (struct lwConsole *console)
{

  console->keyBuffer = keyBoardBuffer;

}

/*
 * Ideally we want to create a proper keyboard map that will return the 
 * right set of characters.
 *
 * In short a keyboard state machine that will iterate and produce the 
 * right keycodes as necessary.
 *
 * But right now we shall not do that.
 * So we will just make a simple state machine that will check the following
 * - Key shift
 * - Capslock
 * - Numlock 
 *
 * We will also return a limited number of control codes 
 * Ctrl-C, Ctrl-D, Ctrl-X, Ctrl-A, Ctrl-k, Ctrl-L 
 * 
 */


unsigned char kbd_getchar (struct lwConsole *cons)
{
  
  u8 nxtChar;

  /*
   * First check whether we have any more character to read
   * If not we return 0x00.
   */
  //  if (numCharsInBuffer <= 0)
  // {
  //  return 0x00;
  //}

  while (numCharsInBuffer <= 0);

  nxtChar = cons->keyBuffer[cons->keyBufferOutIndx];
  INC_KEYINDX(cons->keyBufferOutIndx);
  numCharsInBuffer--;

  switch (nxtChar)
    {
    case LEFT_SHIFT:
    case RIGHT_SHIFT:
      if (capsLockPressed)
	{
	  shiftPressed = SHIFT_UNPRESSED;
	}
      else
	shiftPressed = SHIFT_PRESSED;
      break;
    case LEFT_SHIFT_UNPRESSED:
    case RIGHT_SHIFT_UNPRESSED:
      shiftPressed = SHIFT_UNPRESSED;
      break;
    case ESCAPE_CODE:
      if (escapePressed == TRUE)
	escapePressed = FALSE;
      else
	{
	  escapePressed = TRUE;

	  if (numCharsInBuffer <=0)
	    {
	      return 0x00;
	    }
	  numCharsInBuffer--;
	  nxtChar = cons->keyBuffer[cons->keyBufferOutIndx];
	  INC_KEYINDX(cons->keyBufferOutIndx);
	}
      break;
    case NUM_LOCK_PRESSED:
      numLockPressed = TRUE;
      break;
    case NUM_LOCK_UNPRESSED:
      numLockPressed = FALSE;
      break;
    case CAPS_LOCK_PRESSED:
      capsLockPressed = TRUE;
      break;
    case CAPS_LOCK_UNPRESSED:
      capsLockPressed = FALSE;
      break;
    default:
      /* Here we deal with the translations.
       * As said before what we really nead is a state machine that just
       * steps through the key combinations and returns the right key codes.
       * But for now we will have this ugly hack,
       *
       * Wo what we do is to check which map array we need to pick and return
       * the key from there. Otherwise we just return a 0x00.
       */
      /*
       * If the keys are in the shift range then map it against the shift maps.
       */
      if ((nxtChar >= SHIFT_RANGE_START) && (nxtChar <= SHIFT_RANGE_END))
	{
	  if (shiftPressed == SHIFT_PRESSED)
	    {
	      return (shift_scan_map[nxtChar]);
	    }
	  else
	    return (no_shift_scan_map[nxtChar]);
	}
      /* If we are in the numlock range, then lookup the numlok map.
       */
      if ((nxtChar >= KEYPAD_HOME_7) && (nxtChar <= KEYPAD_DEL_DOT))
	{
	  if (numLockPressed)
	    {
	      return (numKBDScan[nxtChar - KEYPAD_HOME_7]);
	    }
	  else
	    {
	      return (numKBDScanNoLock[nxtChar-KEYPAD_HOME_7]);
	    }

	}
    } /* End of switch */
  return 0x00;

} /* kbd_getchar */

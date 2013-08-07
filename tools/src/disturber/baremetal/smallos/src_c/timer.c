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
#include <param.h>
#include <multiboot.h>
#include <utilities.h>
#include <segments.h>
#include <timer.h>

u32 tick_cnt = 0;
void timer_callback(registers_t regs)
{
  tick_cnt++;
  kprintf ("Timer Tick %d\n", tick_cnt);
  return;
}

void setup_pit(u32 pitHz)
{
  u32 divisor;
 
  divisor = PIT_FREQ/pitHz;
 
  outb(PIT_REG_MODE, 0x34);
  uSleep(10);
  //  outb(PIT_REG_CH0, PIT_LATCH & 0xff);
  outb(PIT_REG_CH0, (u8)(divisor & 0xFF));
  outb(PIT_REG_CH0, (u8)((divisor >> 8) & 0xFF));
  //  uSleep(10);
  //  outb(PIT_REG_CH0, PIT_LATCH >> 8);
}

unsigned int get_pit_count(void)
{
    unsigned int count;

    outb(PIT_REG_MODE, 0x00);
    count  = (unsigned int)inb(PIT_REG_CH0);
    count |= (unsigned int)inb(PIT_REG_CH0) << 8;

    return count;
}

void wait_pit_wrap(void)
{
    unsigned int current;
    unsigned int previous;

    current = get_pit_count();
    do {
        previous = current;
        current = get_pit_count();
    } while (previous >= current);
}


void init_timer (u32 frequency)
{

}

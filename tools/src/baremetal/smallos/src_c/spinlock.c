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
#include <page.h>
#include <console.h>
#include <video.h>
#include <pci.h>
#include <mp.h>
#include <spinlock.h>
#include <timer.h>



void inline spinlock_lock(spinlock_t *sl)
{
    __asm__ __volatile__ ("mov $1, %%eax\n"
  		  "l1:\n"
			  "xchg %0, %%eax\n"
			  "test %%eax, %%eax\n"
			  "jz l3\n"
			  "l2:\n"
			  "cmp $0, %0\n"
			  "jnz l2\n"
			  "jmp l1\n"
			  "l3:\n" : "=m" (sl->locked) : : "memory", "%eax");
}

void inline spinlock_unlock(spinlock_t *sl)
{
    __asm__ __volatile__ ("xor %%eax, %%eax\n"
			  "xchg %0, %%eax\n" :
			  "=m" (sl->locked) : : "memory", "%eax");
}

int spinlock_trylock(spinlock_t *sl)
{
    int oldval = 0;
    __asm__ __volatile__ ("mov $1, %%eax\n"
			  "xchg %0, %%eax\n"
			  "mov %%eax, %1" : "=m" (sl->locked), "=r" (oldval)
			  : : "memory", "%eax");
    return (0 == oldval);
}

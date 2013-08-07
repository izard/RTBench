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
 * =====================================================================================
 *
 *       Filename:  lwrte.c
 *
 *    Description:  Common functions for lwrte
 *
 *        Version:  1.0
 *        Created:  11/11/2010 05:16:25 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Yen-Ting Lu (yen-ting.p.lu@intel.com)
 *        Company:  Intel
 *
 * =====================================================================================
 */

#include <lwrte.h>

void writeMsr(u32 msr, u64 value)
{
    u32 low, high;
    high = value >> 32;
    low = (u32) value;
 
    __asm__ __volatile__("wrmsr" : : "c" (msr), "a"(low), "d" (high) : "memory");
}
 
u64 readMsr(u32 msr)
{
    u64 val = 0;
    u32 val1, val2;
    val1 = 0;
    val2 = 0;
    __asm__ __volatile__("rdmsr" : "=A" (val) : "c" (msr));
    //__asm__ __volatile__("rdmsr" : "=a" (val1), "=d" (val2) : "c" (msr));
    //val = val1; 
    return val;
}

/* 
 * The code is borrowed from msrmod-v0.5.1
 */
u32 getCPU_multiplier()
{
    return (u32) ((readMsr(MSR_REG_CPU) & 0x1f00) >> 8);
}

u64  __timestamp()
{
   u32 lo=0,hi=0;
   __asm__ __volatile__ ("xorl %%eax, %%eax \n  cpuid"
           ::: "%rax", "%rbx", "%rcx", "%rdx");
   __asm__ __volatile__("rdtsc" : "=a" (lo), "=d" (hi));
   return ((u64) hi << 32) | lo;
}

void __monitor(const void *eax, unsigned long ecx, unsigned long edx)
{
    /* "monitor %eax, %ecx, %edx;" */
    asm volatile(".byte 0x0f, 0x01, 0xc8;"
        ::"a" (eax), "c" (ecx), "d"(edx));
}

void __mwait(unsigned long eax, unsigned long ecx)
{
    /* "mwait %eax, %ecx;" */
    asm volatile(".byte 0x0f, 0x01, 0xc9;"
        ::"a" (eax), "c" (ecx));
}
 
void setup_apic_timer(int periodic, int vector, int divider)
{
    asm volatile ("cli");
    u32 sprIntVect;

    /* Enable LAPIC */
    sprIntVect = apicRead(APIC_SPUR_INT_VECT_REG(APIC_BASE));
    sprIntVect |= SET_APIC_ENABLE;
    apicWrite(APIC_SPUR_INT_VECT_REG(APIC_BASE), sprIntVect);
    
    /* Set LAPIC timer interrupt either periodic or oneshot */
    if(periodic)
        apicWrite(APIC_LVT_TMR_REG(APIC_BASE),
                  (APIC_LVTT_TMR_PERIODIC | vector));
    else
        apicWrite(APIC_LVT_TMR_REG(APIC_BASE),
                  (APIC_LVTT_TMR_ONESHOT | vector));

    /* Set timer divider */
    apicWrite(APIC_LVT_DIV_CFG_REG(APIC_BASE),divider);
}

/*
 * max ticks = (CPU clock frequency / CPU_multiplier)
 */
void enable_apic_timer(u32 ticks)
{
    asm volatile ("sti");

    apicWrite(APIC_INIT_COUNT_REG(APIC_BASE),ticks);
}

void disable_apic_timer()
{
    enable_apic_timer(0);
}

volatile u32 apicRead(u32 *addr)
{
    return (*addr);
}

void apicWrite(u32 *addr, u32 data)
{
    *addr = data;
}

/*-
 * Copyright (c) <2010,2013>, Intel Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in
 *   the documentation and/or other materials provided with the
 *   distribution.
 *
 * - Neither the name of Intel Corporation nor the names of its
 *   contributors may be used to endorse or promote products derived
 *   from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*
 * =====================================================================================
 *
 *       Filename:  mwaitTester.c
 *
 *    Description:  example app to interact with mwait
 *
 *        Version:  1.0
 *        Created:  10/08/2010 11:46:36 AM
 *       Revision:  none
 *       Compiler:  icc
 *
 *         Author:  Patrick Lu (yen-tingx.p.lu@intel.com)
 *        Company:  Intel
 *
 * =====================================================================================
 */

#include "mwaitTester.h"
#include <utilities.h>

volatile u32 * monAddr;
u32 * monAddrArg;
static u32 * count;
static u32 * count2;
static u64 * timePtr;
static u32 * cstate; 

void mwait_run()
{
  //  kprintf("starting mwait at %x\n", monAddr);
    if (*monAddr == TESTVALUE)
    {
        __monitor((const void *)monAddr, 0, 0);
        if (*monAddr == TESTVALUE)
        {
            //1 treat interrupt as break event
            __mwait(*cstate, 0);
 //   kprintf("after mwait\n");
            if (*monAddr != TESTVALUE)
            {
                (*count)++;
            }
            else
                (*count2)++;
        }
    }
}

#define rdtscll(val)    __asm__ __volatile__("lfence;rdtsc" : "=A" (val))

void mwaitLoop()
{
    static u64 t1 = 0, t2 = 0;
    volatile int i;
 //mov %%ecx, 0x10; rdmsr; add %%eax, 50;wrmsr
//asm("rdtsc; mov %%ecx, 0x10;wrmsr; add %%eax, 100" : : : "%eax", "%ecx", "%edx" );

    while(1)
    {
        *monAddr = TESTVALUE;
//#if NO_MWAIT == TRUE
        mwait_run();
//#endif
#if LOOP_LOG == TRUE       
        if(timePtr < (u64*) (SCRATCH_PTR+(DATASET*sizeof(u64))))
        {
            *timePtr = __timestamp();
            t2 = *timePtr;
            timePtr++;
        }
#endif
#if LWRTE_MP == FALSE
//        kprintf("Wake up from MWAIT\n");
#endif
//if (*count % 10000000 == 0)
  //              kprintf("mon addr %x is %d\n", monAddr, *monAddr);
        switch(*monAddr)
        {
            case CMD_HALT:
                kprintf("Halting CPU\n");
                kprintf("Disable interrupt");
                asm volatile("cli");
                asm volatile("nop");
                asm volatile("nop");
                asm volatile("nop");
                asm volatile("nop");
                asm volatile("nop");
                u32 * ptr = (u32 *) 0x1100;
                *ptr = 0xdead;
                asm volatile("hlt");
                break;
            case CMD_501:
  	{
		unsigned long long int t0,t1;
		*((unsigned int*)monAddr + 10) = 0;
		t0 = main_501();
//                kprintf("test 24 %d done \n", t0);
		*((unsigned int*)monAddr + 10) = t0;
		}
                break;
            case CMD_502:
		{
		unsigned long long int t0,t1;
		*((unsigned int*)monAddr + 10) = 0;
		t0 = main_502();
//                kprintf("test 25 %d done \n", t0);
		*((unsigned int*)monAddr + 10) = t0;
		}
                break;
            case CMD_503_1:
		{
		unsigned long long int t0,t1;
		*((unsigned int*)monAddr + 10) = 0;
		t0 = main_503_1();
//                kprintf("test 26 %d done \n", t0);
		*((unsigned int*)monAddr + 10) = t0;
		}
                break;
            case CMD_503_2:
		{
//                kprintf("test 261\n");
		main_503_2();
		}
                break;
            case CMD_503_3:
		{
//                kprintf("test 262\n");
		main_503_3();
		}
                break;
            default:
#if LWRTE_MP == FALSE
//                kprintf("nothing happened...\n");
//                kprintf("Hex:%x,Dec:%d\n",*monAddr,*monAddr);
//                kprintf("Dec:%d\n",(t2-t1));
		t1 = t2;
#endif
                break;
        }
    }
}

void mwaitInit(u32 * mAddr, u32 * mAddrArg, u32 * cntWakeUpFromVarChange, u32 * cntWakeUpFromOtherEvent) 
{
    monAddr = mAddr;
    monAddrArg = mAddrArg;
    *monAddrArg = 0;
    timePtr = (u64*) SCRATCH_PTR;
    count = cntWakeUpFromVarChange;
    *count = 0;
    count2 = cntWakeUpFromOtherEvent;
    *count2 = 0;

    cstate = (u32 *) CSTATE_PTR;
    *cstate = C1;

//    kprintf("monitoring %x\n", monAddr);
    mwaitLoop();
}

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


#include <lwrte_config.h>
#include <spinlock.h>
#include "mwaitTester.h"

typedef void (*FPtr)(void);

extern FPtr CTORS_ADDR;

void do_ctors(FPtr *ptr)
{
  while (*ptr) (*ptr++)();
}

void ctors_init(void)
{
  do_ctors(&CTORS_ADDR);
};


spinlock_t apPrintLock;

/* LWRTE_AMP C main function */
int ap_main(unsigned long magic, unsigned long mboot_ptr)
{
    ctors_init();
    /* LWRTE Init Routine Begin */
    SPINLOCK_INIT(&apPrintLock);
    /* Initialize serial port */
    init_serial();
    /* Print welcome message to serial console */
    kprintf("Init serial done!\n");
//    kprintf("0x20:%d,0xA0:%d\n",inb(0x20),inb(0xA0));
    /* Reload gdt/idt table in higher memory region 
     * Comments: Initial (temporary) gdt table is located at 0x1000p, 
     *           installed by the boot.s.
     *           After we jump to our entry_main routine, we are in 
     *           the higher memory region, and we should install another
     *           gdt/idt tables in this region.
     */
    init_descriptor_tables();
    /* LWRTE Init Routine End */

    /* You can call your application from here 
     * Comments: you should include necessary headers in main to make 
     *           your functions visible by main.
     */

    mwaitInit(MWAIT_CMD,MWAIT_CMD_ARG,MWAIT_CNT,MWAIT_CNT+0x10);

    return 0xfeedbaba;
}

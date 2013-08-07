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
 * Main kernel entry point for eal386
 *
 * Code structure follows various examples on the net
 * including FreeBSD 7.2 code base.
 */

#include <param.h>
#include <multiboot.h>
#include <utilities.h>
#include <segments.h>
#include <page.h>
#include <console.h>
#include <video.h>
#include <mp.h>
#include <spinlock.h>
#include <timer.h>
#include "mwaitTester.h"
#define rdtscll(val)    __asm__ __volatile__("lfence;rdtsc" : "=A" (val))

extern u32 end;
void IdentifyCPU();
u32 phys_addr_len;
extern page_directory_t *lwrtePageDir;
extern int mwait2,mwait3;
struct lwConsole defConsole;
void testVideo();
void kbcISR(registers_t regs);
u32 chkMemAddr;
struct mp_config_table *glblMpCfgPtr;
spinlock_t apPrintLock;
volatile u8 apReadyToGo;

extern void tmrEvntGenerator(registers_t regs);

void scan_memory_map (struct mb_info *mbi);

/*
 * This is the global array of memory map information.
 */
struct memory_map glblMemMapInfo[NUM_OF_MEM_MAP];
u32 glblNumEntries = 0;

void mp_main(unsigned long magic, unsigned long mboot_ptr)
{

    struct mb_info *mbi;
    struct memory_map *mmap;
    volatile int indx;
    u16 vid;
    u16 did;
    u32 *mpTabStart;
    u32 *biosEBDA;
    u8  myChar;

    /* Test debug LED */
    outb(0x80,0x77);

    /*
     *  Initialize serial port first 
     */
    init_serial();

    /*
     * Initialize our console first.
     * then we get to print stuff.
     */
#ifndef PINEVIEW
    initVgaConsStruct (&defConsole);
    kbcInit();
    defConsole.keyBufferInIndx = 0;
    defConsole.keyBufferOutIndx = 0;
    defConsole.init(defConsole.lwConsoleHndle);
    setKeyboardBuffer(&defConsole);
    defConsole.clear(defConsole.lwConsoleHndle);
    kprintf ("LWRTE MP 386\n");
    defConsole.move_cursor(defConsole.lwConsoleHndle, 1,0);
#endif 

    mbi = (struct mb_info *) mboot_ptr;
    chkMemAddr = mboot_ptr;

    scan_memory_map(mbi);

    IdentifyCPU ();
    init_descriptor_tables ();
    SPINLOCK_INIT(&apPrintLock);

    //register_interrupt_handler(IRQ1, &kbcISR);
    //  register_interrupt_handler(IRQ0, &tmrEvntGenerator);
    //register_interrupt_handler(IRQ0, &timer_callback);
    /*
     * Look for MP Information
     */
    doMPSetup ();
    //doMoorestownMPSetup();

    lwrte_eu0();
}


/*
 * IdentifyCPU
 *
 * This function is called to identify the cpu.
 * We follow a bootup sequence very similar to FreeBSD.
 * Once we have control we first identify what the CPU is.
 * We don't necessarily do much with that information, we just try to 
 * print the info out.
 */
void IdentifyCPU()
{
    u32 funcCode;
    u32 res1, res2, res3, res4;


    funcCode = 0;
    asm("mov %4, %%eax;"
            "cpuid;"
            "mov %%eax, %0;"
            "mov %%ebx, %1;"
            "mov %%edx, %2;"
            "mov %%ecx, %3;"
            :"=r"(res1),"=r"(res2),"=r"(res3),"=r"(res4)
            :"r"(funcCode)
       );

    kprintf ("CPUID: 0x%8x 0x%8x 0x%8x 0x%8x", res1, res2, res3, res4);
    kprintf ("%c%c%c%c%c%c%c%c%c%c%c%c", 
            (u8)(res2 & 0x000000FF),
            (u8)((res2>>8) & 0x000000FF),
            (u8)((res2>>16) & 0x000000FF),
            (u8)((res2>>24) & 0x000000FF),
            (u8)(res3 & 0x000000FF),
            (u8)((res3>>8) & 0x000000FF),
            (u8)((res3>>16) & 0x000000FF),
            (u8)((res3>>24) & 0x000000FF),
            (u8)(res4 & 0x000000FF),
            (u8)((res4>>8) & 0x000000FF),
            (u8)((res4>>16) & 0x000000FF),
            (u8)((res4>>24) & 0x000000FF));

    funcCode = 1;

    asm("mov %1, %%eax;"
            "cpuid;"
            "mov %%edx, %0;"
            :"=r"(res1)
            :"r"(funcCode)
       ); 

    /*
     * Print out the stepping id and model number, family code
     */

    kprintf ("Stepping ID: 0x%x\n", res1 & 0x0000000F);
    kprintf ("Model Number: 0x%x\n", (res1 & 0x000000F0)>>4);
    kprintf ("Family Code: 0x%x\n", (res1 & 0x00000F00) >> 8);

}


void testVideo()
{
    int i;

    for (i = 0; i < 25; i++)
        kprintf ("This is line %d\n", i);

    kprintf ("Next line, we should see scroll up\n");

}

void testPrintf ()
{
    u8 str[64];

    strncpy(str,"This is a test string.\n", 0);
    defConsole.putchar(defConsole.lwConsoleHndle, '\n');
    //  kprintf ("This is the format string.\n");
    defConsole.move_cursor(defConsole.lwConsoleHndle, 1, 0);
    kprintf ("Test %s", str);
    kprintf ("This is a number %d\n", 10);
    kprintf ("This is a number %d\n", 10000);
    kprintf ("This is a number %d\n", 1010);
    kprintf ("This is a number %d\n", 2000010);
    kprintf ("Number %d followed by %s\n", 1002, str);
    kprintf ("Number %x followed by %s\n", 1002, str);
    kprintf ("Number %8x followed by %s\n", 1002, str);

} /* end of testPrintf */


void scan_memory_map (struct mb_info *mbi)
{

    struct memory_map *mmap;

    if (CHK_BIT_SET(mbi->flags, 0))
    {
        kprintf ("Mem Lower: %dKB, mem_upper: %dKB\n",
                (u32) mbi->mem_low, (u32)mbi->mem_high);
    }

    if (CHK_BIT_SET(mbi->flags, 6))
    {
        kprintf ("mmap addr: 0x%x, mmap length: 0x%x\n",
                mbi->mmap_addr, mbi->mmap_len);

        for (mmap = (struct memory_map *)mbi->mmap_addr;
                (u32) mmap < mbi->mmap_addr + mbi->mmap_len;
                mmap = (struct memory_map *)((u32) mmap +
                    mmap->size + sizeof (mmap->size)))
        {
            glblMemMapInfo[glblNumEntries].size = mmap->size;
            glblMemMapInfo[glblNumEntries].base_addr_low = mmap->base_addr_low;
            glblMemMapInfo[glblNumEntries].base_addr_high = mmap->base_addr_high;
            glblMemMapInfo[glblNumEntries].len_low = mmap->len_low;
            glblMemMapInfo[glblNumEntries].len_high = mmap->len_high;
            glblMemMapInfo[glblNumEntries].type = mmap->type;
            glblNumEntries ++;
#if 0
            kprintf (" size: 0x%x base_addr: 0x%x 0x%x length: 0x%x 0x%x type: 0x%x\n",
                    mmap->size, mmap->base_addr_low, mmap->base_addr_high,
                    mmap->len_low, mmap->len_high, mmap->type);
#endif    
            if ((mmap->type == 0x1) && (mmap->base_addr_low == 0x100000))
            {
                phys_addr_len = mmap->len_low;
            }
        }
    }
} /* End of scan_memory_map */

void run_rt_tests()
{
	unsigned int i = 0, avg = 0, max = 0, t;
	for (i = 0; i < 10; i++) {
		uSleep(300000);
		*((unsigned int*)(MWAIT_CMD_EU(3))) = CMD_501;
		uSleep(800000);
		t = *( (unsigned int *)MWAIT_CMD_EU(3)+ 10);
		if (t > max) max = t;
                avg += t;
	}
        kprintf("501.motioncontrol test: Average: %d, maximum %d clockticks\n", avg/10, max);
	uSleep(5000000);

	avg = 0; max = 0;
	for (i = 0; i < 10; i++) {
		uSleep(300000);
		*((unsigned int*)(MWAIT_CMD_EU(3))) = CMD_502;
		uSleep(800000);
		t = *( (unsigned int *)MWAIT_CMD_EU(3)+ 10);
		if (t > max) max = t;
                avg += t;
	}
        kprintf("502.plc test: Average: %d, maximum %d clockticks\n", avg/10, max);
}

void lwrte_eu0(void)
{
    volatile u32 i;
    u32 *monAddr, *ptr; 
    monAddr = (u32 *) MWAIT_CMD_EU(1);
    char ch;
    //cache line size is 64 bytes, 
    //we should have our monitor addresses in each threads at least 64 bytes away
    //mwaitInit((MWAIT_CMD+128*0), MWAIT_CMD_ARG, (MWAIT_CNT+0x00), (MWAIT_CNT+0x10));

    run_rt_tests();
    while(1)
    {
        ch = read_serial();
        switch(ch)
        {
            case 'h':
                kprintf("h:help\n");
                kprintf("r:re-run tests\n");
                break;
            case 'r':
		run_rt_tests();
                break;
        }
    }
}
void lwrte_eu1(void)
{
//    kprintf("I'm in eu1 %x\n", (MWAIT_CMD_EU(1)));
    //cache line size is 64 bytes, 
    //we should have our monitor addresses in each threads at least 64 bytes away
    mwaitInit((MWAIT_CMD_EU(1)), MWAIT_CMD_ARG, MWAIT_CNT_EU(1,1), MWAIT_CNT_EU(1,2));
    while(1);
}
void lwrte_eu2(void)
{
//    kprintf("I'm in eu2 %x\n", (MWAIT_CMD_EU(2)));
    mwaitInit((MWAIT_CMD_EU(2)), MWAIT_CMD_ARG, MWAIT_CNT_EU(2,1), MWAIT_CNT_EU(2,2));
    while(1);
}
void lwrte_eu3(void)
{
//    kprintf("I'm in eu3 %x\n", (MWAIT_CMD_EU(3)));
    mwaitInit((MWAIT_CMD_EU(3)), MWAIT_CMD_ARG, MWAIT_CNT_EU(3,1), MWAIT_CNT_EU(3,2));
    while(1);
}

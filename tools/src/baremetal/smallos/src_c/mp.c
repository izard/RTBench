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
 * This file contains the MP protocol initialization
 * and management routines.
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
#include <spinlock.h>
#include <timer.h>

#define MAIL_BOX 0x9000
//#define VERBOSE_BOOT

/*
 */
struct mp_proc_info_entry mpCpuInfo[MP_MAX_PROCS];
// Thus array maintains the reverse map from APIC IDs to
// cpu number.
u8 cpu_apic_id_map[MP_MAX_PROCS];
int glblNumCpus;
static i32 g_cpuBusFreq = 333000000; /* assume 333 MHz to begin with */
extern struct mp_config_table *glblMpCfgPtr;
static struct mp_config_table dummyMpCfgPtr;
extern u32 ap_entry;
extern u32 ap_entry_end;
extern spinlock_t apPrintLock;
extern volatile u8 apReadyToGo;

extern void downLinkHandler();
extern void fwdingLoop ();

void calibrate_time(void)
{
    u64 before;
    u64 after;
    u32 lvtt;
    u32 apic1;
    u32 apic2;
    u32 result;
    int ii;

    apicWrite (APIC_LVT_DIV_CFG_REG(glblMpCfgPtr->memAddrLAPIC),
               APIC_TMR_DIV_BY16);

    lvtt = APIC_LVTT_VECTOR | APIC_LVTT_MASKED;
    apicWrite(APIC_LVT_TMR_REG(glblMpCfgPtr->memAddrLAPIC), lvtt);

    /*
     * Write to the counter register.
     */
    apicWrite(APIC_INIT_COUNT_REG(glblMpCfgPtr->memAddrLAPIC),
              1000000000);

    wait_pit_wrap();
    before = __timestamp();
    for (ii = 0; ii < PIT_LOOPS; ii++) {
        wait_pit_wrap();
    }
    after = __timestamp();

    wait_pit_wrap();
    apic1 = apicRead(APIC_LVT_CUR_COUNT_REG(glblMpCfgPtr->memAddrLAPIC));
    for (ii = 0; ii < PIT_LOOPS; ii++) {
        wait_pit_wrap();
    }
    apic2 = apicRead(APIC_LVT_CUR_COUNT_REG(glblMpCfgPtr->memAddrLAPIC));

    result = (apic1 - apic2) * 16 / PIT_LOOPS;

/*    kprintf("CPU clock speed is %d.%04d MHz.\n",
            ((long)(after - before) / PIT_LOOPS) / (1000000 / PIT_HZ),
            ((long)(after - before) / PIT_LOOPS) % (1000000 / PIT_HZ));

    kprintf("Bus clock speed is %d.%04d MHz.\n",
            result / (1000000 / PIT_HZ),
            result % (1000000 / PIT_HZ));
*/
    g_cpuBusFreq = result * PIT_HZ;
}

/*
 * Search for mp floating point structure.
 *
 */
u32 *mp_search_flt_sig(u32 *base, u32 *limit)
{

    u32 sigdata;
    u32 maxLimit;
    u32 *startPtr;
    struct mp_ioapic_entry *ioapic;


    for (startPtr = base; startPtr < limit ; startPtr++)
    {
        sigdata = *startPtr;
        if (sigdata == MP_FLT_PTR_SIG)
        {
            return startPtr;

        }

    }

    return (NULL);

} /* mp_search_flt_sig */

void printMPFloatStruct (struct mp_float_ptr *mflt)
{
    kprintf ("Signature: %s\n", (u8 *)mflt->mpFPSig);
    kprintf ("PhyAddrPtr: 0x%8x\n", mflt->physAddrPtr);
    kprintf ("Length: 0x%2x\n", mflt->length);
    kprintf ("SpecRev: 0x%2x\n", mflt->specRev);
    kprintf ("Checksum: 0x%2x\n", mflt->checksum);
    kprintf ("MP Feature Byte: 0x%2x\n", mflt->mpFeatureByte1);
    kprintf ("MP Feature Byte: 0x%8x\n", mflt->mpFeatureBytes2_5);


} /* printMPFloatStruct */

void printMPConfigTable (struct mp_config_table *mpCfg)
{
    unsigned int indx;

    kprintf ("MP Config Signature: 0x%x\n", mpCfg->mpSig);
    kprintf ("Base Table Length: 0x%4x\n", mpCfg->baseTblLen);
    kprintf ("Spec Revision: 0x%2x\n", mpCfg->specRev);
    kprintf ("Checksum: 0x%2x\n", mpCfg->checksum);
    for (indx = 0; indx < 8; indx++)
    {
        kprintf ("%c", mpCfg->oemIDStr[indx]);
    }
    kprintf ("\n");
    for (indx = 0; indx < 12; indx ++)
    {

        kprintf ("%c", mpCfg->prodIdStr[indx]);
    }
    kprintf ("\n");
    kprintf ("OEM Table Ptr: 0x%8x\n", mpCfg->oemTblPtr);
    kprintf ("OEM Table Size: 0x%8x\n", mpCfg->oemTblSize);
    kprintf ("Entry count: 0x%4x\n", mpCfg->entryCnt);
    kprintf ("LAPIC Addr: 0x%8x\n", mpCfg->memAddrLAPIC);
    kprintf ("Extended Table Length: 0x%4x\n", mpCfg->extTblLen);
    kprintf ("Extended Table Checksum: 0x%2x\n", mpCfg->extTblChecksum);

} /* printMPConfigTable */


/*
 * Parse out the MP Configuration Table.
 */
void mp_parse_config_table (struct mp_config_table *mpCfg)
{
    int numOfEntries;
    int localApicBase;
    u8 *entryType;
    int numOfCpus;
    struct mp_proc_entry *procEnt;


    numOfCpus = 0;
    kprintf ("Local APIC Base Address: 0x%8x\n", mpCfg->memAddrLAPIC);

    entryType = (u8 *)((u8 *)mpCfg+sizeof(struct mp_config_table));
    for (numOfEntries = 0; numOfEntries < mpCfg->entryCnt; numOfEntries++)
    {
        switch (*entryType)
        {

            case MP_CONFIG_PROCESSOR:
                //        kprintf ("Found a CPU.\n");
                procEnt = (struct mp_proc_entry *)entryType;
                if (procEnt->cpuFlags & MP_PROC_ENABLE)
                {
                    /*
                     * Copy over the CPU information into the
                     * proc entry stucture.
                     */
                    mpCpuInfo[numOfCpus].cpuId = numOfCpus;
                    cpu_apic_id_map[numOfCpus] = procEnt->lapicID;
                    mpCpuInfo[numOfCpus].lapicID = procEnt->lapicID;
                    mpCpuInfo[numOfCpus].lapicVer = procEnt->lapicVer;
                    mpCpuInfo[numOfCpus].cpuSig = procEnt->cpuSig;
                    mpCpuInfo[numOfCpus].cpuFlags = procEnt->cpuFlags;
                    mpCpuInfo[numOfCpus].featureFlags = procEnt->featureFlgs;
                    numOfCpus++;
                }
                entryType += 20;
                break;
            case MP_CONFIG_BUS:
                //        kprintf ("Found a BUS\n");
                entryType += 8;
                break;
            case MP_CONFIG_IOAPIC:
                //        kprintf ("Found IOAPIC Entry\n");
                entryType += 8;
                break;
            case MP_CONFIG_IOINTASSIGN:
                //        kprintf ("Found IO Assign\n");
                entryType += 8;
                break;
            case MP_CONFIG_LOCINTASSIGN:
                //        kprintf ("Found local API Assignment.\n");
                entryType += 8;
                break;

        } /* End of switch */

    } /* End of for loop */

    glblNumCpus = numOfCpus;
    if (numOfCpus)
        kprintf ("Number of CPUs: %d\n", numOfCpus);
    else
        kprintf ("No CPU Entries in mp configuration? \n");

    kprintf ("Attempt to read APIC ID: 0x%8x\n",
             apicRead(APIC_ID(mpCfg->memAddrLAPIC)));

} /* end of mp_parse_config_table */


/*
 * smp_scan
 *
 * This function will scan the memory for
 * the float point structure and fill up our SMP information structure.
 */

struct mp_float_ptr *mp_scan(void)
{
    struct mp_float_ptr *mploc;

    mploc = (struct mp_float_ptr *)mp_search_flt_sig((u32 *)BIOS_EBDA,
                                                     (u32 *)(BIOS_EBDA+MP_SEARCH_SZ));
    if (mploc == NULL)
    {
        mploc = (struct mp_float_ptr *)mp_search_flt_sig((u32 *)BIOS_BASE,
                                                         (u32 *)(BIOS_BASE+MP_SEARCH_SZ));
        if (mploc == NULL)
        {
            mploc = (struct mp_float_ptr *)mp_search_flt_sig((u32 *)BIOS_BASE2,
                                                             (u32 *)(BIOS_BASE2+MP_SEARCH_SZ));
        }
    }

    return mploc;

} /* End of mp_scan */

#define offsetof(TYPE,FIELD) \
    ((unsigned long)(&((TYPE *)0)->FIELD))

int ACPIScan(void)
{
    sRSDPtr_t *pRSD = NULL;
    int numcpus = 0;
    unsigned long ptr;
    u32 val = 0;
    int ii;
    sRSDTPtr_t *pRSDT = NULL;
    sMADTPtr_t *pMADT = NULL;
    sAPICStr_t *pAPICs = NULL;
    int numheaders = 0;
    int offset = 0;
    int structlen = 0;

    __asm__ __volatile__("xor %%eax, %%eax\n"
                         "inc %%eax\n"
                         "cpuid" : "=a" (val));

    memset((u8 *)&dummyMpCfgPtr, 0, sizeof(dummyMpCfgPtr));

    /* first scan for the RSD struct */
    for (ptr = BIOS_EBDA; ptr < (BIOS_EBDA + MP_SEARCH_SZ); ptr += 16) {
        if (!memcmp((void *)ptr, ACPI_ANKR_RSDP_STR, ACPI_ANKR_RSDP_LEN)) {
            /* found it! */
            pRSD = (sRSDPtr_t *)ptr;
            break;
        }
    }

    for (ptr = 0xE0000; pRSD == NULL && ptr < 0xFFFFF; ptr += 16) {
        if (!memcmp((void *)ptr, ACPI_ANKR_RSDP_STR, ACPI_ANKR_RSDP_LEN)) {
            /* found it! */
            pRSD = (sRSDPtr_t *)ptr;
            break;
        }
    }

    if (pRSD) {
        pRSDT = (sRSDTPtr_t *)pRSD->rsdt_address;
#ifdef VERBOSE_BOOT
        kprintf("Found ACPI table at: %x\n", pRSD);
        kprintf("RSDT at: %x\n", pRSD->rsdt_address);
#endif /* VERBOSE_BOOT */

        if (pRSDT && !memcmp(pRSDT->signature,
                             ACPI_ANKR_RSDT_STR, ACPI_ANKR_RSDT_LEN)) {
            numheaders =
                (pRSDT->len - offsetof(sRSDTPtr_t, headers)) >> 2;
#ifdef VERBOSE_BOOT
      kprintf("There are %d headers!\n", numheaders);
#endif /* VERBOSE_BOOT */

            for (ii = 0; ii < numheaders; ii++) {
                pMADT = (sMADTPtr_t *)pRSDT->headers[ii];

#ifdef VERBOSE_BOOT
		kprintf("> %d  > %x\n", ii, pRSDT->headers[ii]);
#endif /* VERBOSE_BOOT */

                if (pMADT &&
                    !memcmp(pMADT->signature,
                            ACPI_ANKR_MADT_STR, ACPI_ANKR_MADT_LEN)) {
                    structlen =
                        pMADT->len - offsetof(sMADTPtr_t, structdata);
#ifdef VERBOSE_BOOT
                    kprintf("Found MADT at: %x\n", pRSDT->headers[ii]);
#endif /* VERBOSE_BOOT */
                    dummyMpCfgPtr.memAddrLAPIC = (void *)(pMADT->lapicaddress);
                    for (offset = 0;
                         numcpus < MP_MAX_PROCS && offset < structlen;) {
                        pAPICs = (sAPICStr_t *)&pMADT->structdata[offset];
                        if (ACPI_APIC_TYPE_LAPIC == pAPICs->type) {
#ifdef VERBOSE_BOOT
                            kprintf("CPU %d (APIC ID:%d) %sabled\n",
                                    pAPICs->acpiprocid, pAPICs->apicid,
                                    (pAPICs->flags & ACPI_APIC_FLAG_ENABLED)?
                                    "en":"dis");
#endif /* VERBOSE_BOOT */
                            if (pAPICs->flags & ACPI_APIC_FLAG_ENABLED) {
                                memset((u8 *)&mpCpuInfo[numcpus], 0x00,
                                       sizeof(struct mp_proc_entry));
                                mpCpuInfo[numcpus].cpuId = pAPICs->acpiprocid;
                                mpCpuInfo[numcpus].lapicID = pAPICs->apicid;
                                mpCpuInfo[numcpus].lapicVer =
                                    apicRead(APIC_VER(pMADT->lapicaddress));
                                mpCpuInfo[numcpus].cpuSig = val;
                                cpu_apic_id_map[numcpus] = pAPICs->apicid;
                                numcpus++;
                            }
                        }
                        offset += pAPICs->len;
                    }
                    break;
                }
            }
        } else {
            kprintf("Invalid RSDT!\n");
        }
    }

    glblNumCpus = numcpus;
    if (numcpus) {
        glblMpCfgPtr = &dummyMpCfgPtr;
        kprintf ("Number of CPUs: %d\n", numcpus);
    }

    return numcpus;
}

/*
 * doMPSetup
 *
 * This is our main entry point for MP initialization
 *
 */

void doMPSetup ()
{
    struct mp_float_ptr *mploc;
    u32 sprIntVect;
    u32 tpr;
    u32 lvtErr;
    volatile u32 *apMailBox;
    u8 *apCodeStore;
    u32 cIndx;
    u8 *biosAddr;
    u16 *bAddr16;
    int ii;

    apReadyToGo = 0;

    //    apPrintLock = 0;
    SPINLOCK_INIT(&apPrintLock);

    if (!ACPIScan()) {
        glblMpCfgPtr = NULL;
        mploc = mp_scan();

        if (mploc != NULL)
        {
            glblMpCfgPtr = (struct mp_config_table *)mploc->physAddrPtr;
            kprintf ("Found MP Floating Pointer structure 0x%8x\n", mploc);
            kprintf ("Initializing SMP...\n");
            mp_parse_config_table((struct mp_config_table *)mploc->physAddrPtr);
        } else {
            kprintf ("MP Floating Pointer structure not found!\n");
            return;
        }
    }


    // spurious interrupt vector.
    sprIntVect = apicRead(APIC_SPUR_INT_VECT_REG(glblMpCfgPtr->memAddrLAPIC));
    // Enable the apic, and enable - we don't change the vector. It is already
    // the way we want it to be.
    sprIntVect |= SET_APIC_ENABLE;
    apicWrite(APIC_SPUR_INT_VECT_REG(glblMpCfgPtr->memAddrLAPIC), sprIntVect);
    sprIntVect = apicRead(APIC_SPUR_INT_VECT_REG(glblMpCfgPtr->memAddrLAPIC));

    /* Set task priority to be 0,0 */
    tpr = apicRead(APIC_TASK_PRI_REG(glblMpCfgPtr->memAddrLAPIC));
    tpr |= 0xffffff00;
    apicWrite(APIC_TASK_PRI_REG(glblMpCfgPtr->memAddrLAPIC), tpr);

    apicRead(APIC_SPUR_INT_VECT_REG(glblMpCfgPtr->memAddrLAPIC));

    apicWrite(APIC_EOI_REG(glblMpCfgPtr->memAddrLAPIC), 0);

    // Not sure whether we need to set this up.
    // Need to cross verify with BSD.

    lvtErr = apicRead(APIC_LVT_ERR_REG(glblMpCfgPtr->memAddrLAPIC));
    lvtErr &= 0xFFFFFFFE;
    apicWrite(APIC_LVT_ERR_REG(glblMpCfgPtr->memAddrLAPIC), lvtErr);
    apicWrite(APIC_LVT_ERR_REG(glblMpCfgPtr->memAddrLAPIC), lvtErr);

    //Very important, do not disable it. 
    //Or Clock and Bus speed will detect incorrectly and
    //result in slow boot up.
    setup_pit(1000);
    calibrate_time();

    apMailBox = (u32 *)MAIL_BOX;

    *apMailBox = 0x00000000;
#ifdef VERBOSE_BOOT
    kprintf ("ap_entry: 0x%8x ap_entry_end: 0x%8x\n",
             (u32 )&ap_entry,
             (u32) &ap_entry_end);
#endif /* VERBOSE_BOOT */

    /*
     * Copy this over to where we need to place the code.
     */
    apCodeStore = (u8 *)0x00001000;
    for (cIndx = 0; cIndx < ((u32 )&ap_entry_end - (u32)&ap_entry +1);
         cIndx++)
    {
        apCodeStore[cIndx] = ((unsigned char *)&ap_entry)[cIndx];

    }

    biosAddr = (unsigned char *)0xf;
    *biosAddr = 0xa;
    bAddr16 = (u16 *)0x467;
    *bAddr16 = 0x1000;
    biosAddr = (unsigned char *)0x469;
    *biosAddr = 0;

    kprintf("%s - %s\n", __DATE__, __TIME__);

    //    uSleep(10000000);
    for (ii = 0; ii < MP_MAX_PROCS && ii < glblNumCpus; ii++) {
        if (mpCpuInfo[ii].lapicID ==
            apicRead(APIC_ID(glblMpCfgPtr->memAddrLAPIC))) {
            mpCpuInfo[ii].cpuFlags |= MP_PROC_BP;
            continue;
        }

#ifdef VERBOSE_BOOT
        kprintf ("Trying to boot CPU %d\n", mpCpuInfo[ii].lapicID);
        kprintf ("LAPIC Version: 0x%2x\n", mpCpuInfo[ii].lapicVer);
#else
	kprintf ("/");
#endif /* VERBOSE_BOOT */

        apMailBox[0] = mpCpuInfo[ii].lapicID;
        apMailBox[1] = 0x100000 + ((ii + 1) << 12);

        /*
         * We need to send a INIT followed by two SIPI with a 200us delay
         * between the two.
         */

        mpSendIPI(mpCpuInfo[ii].lapicID, APIC_TRIG_MODE_LEVEL | APIC_LVL_ASSERT | APIC_IPI_INIT);
        uSleep(10000);
        mpSendIPI(mpCpuInfo[ii].lapicID, APIC_TRIG_MODE_LEVEL | APIC_IPI_INIT);
        uSleep(300);
        mpSendIPI(mpCpuInfo[ii].lapicID, APIC_IPI_STARTUP | ((unsigned long)apCodeStore >> 12));
        uSleep(300);
        mpSendIPI(mpCpuInfo[ii].lapicID, APIC_IPI_STARTUP | ((unsigned long)apCodeStore >> 12));

        // Sleep for more time now.
        uSleep(100000);

        while(*apMailBox != 0xffffffff);
#ifdef VERBOSE_BOOT
        kprintf("Successfully booted CPU %d\n\n", mpCpuInfo[ii].lapicID);
#else
	kprintf("\\");
#endif /* VERBOSE_BOOT */
    }

#ifndef VERBOSE_BOOT
    kprintf("\n");
#endif /* !VERBOSE_BOOT */

    //    uSleep(10000000);

    /*XXX: rethink the logic here.*/
    /* 
     * Once everything is completed, disable the LAPIC interrupt
     */
    // spurious interrupt vector.
    sprIntVect = apicRead(APIC_SPUR_INT_VECT_REG(glblMpCfgPtr->memAddrLAPIC));
    // Enable the apic, and enable - we don't change the vector. It is already
    // the way we want it to be.
    sprIntVect &= ~SET_APIC_ENABLE;
    apicWrite(APIC_SPUR_INT_VECT_REG(glblMpCfgPtr->memAddrLAPIC), sprIntVect);
    sprIntVect = apicRead(APIC_SPUR_INT_VECT_REG(glblMpCfgPtr->memAddrLAPIC));

} /* doMPSetup */


i32 cpu_bus_clock_freq(void)
{
    return (g_cpuBusFreq);
} /* end of cpu_bus_clock_freq */

/*
 * The following code is from openblt.
 * Need to clean it up.
 */

void apic_set_timer (int value)
{
    unsigned long lvtt;
    long ticks_per_us;

    ticks_per_us = cpu_bus_clock_freq () / 1000000;
    apicWrite (APIC_LVT_DIV_CFG_REG(glblMpCfgPtr->memAddrLAPIC),
               APIC_TMR_DIV_BY1);

    /*
     * Configure timer.
     */

    lvtt = apicRead(APIC_LVT_TMR_REG(glblMpCfgPtr->memAddrLAPIC));
    lvtt &= ~(APIC_LVTT_VECTOR | APIC_LVTT_DLV_STAT_PEND | APIC_LVTT_MASKED | APIC_LVTT_TMR_PERIODIC) | APIC_LVTT_MASKED | APIC_LVTT_VECTOR;
    apicWrite(APIC_LVT_TMR_REG(glblMpCfgPtr->memAddrLAPIC), lvtt);

    /*
     * Write to the counter register.
     */
    apicWrite(APIC_INIT_COUNT_REG(glblMpCfgPtr->memAddrLAPIC),
              value * ticks_per_us);


}


int apic_read_timer(void)
{
    // Just return the current count register.
    // Should just make this into a macro.

    return (apicRead(APIC_LVT_CUR_COUNT_REG(glblMpCfgPtr->memAddrLAPIC)));
}


/*
 * This is just a tight loop for now.
 */
void uSleep(u32 timeInUSecs)
{
    volatile u32 count;

    apic_set_timer(timeInUSecs);

    //    while (count = apicRead(APIC_LVT_CUR_COUNT_REG(glblMpCfgPtr->memAddrLAPIC)));
    do 
      {
	count = apicRead(APIC_LVT_CUR_COUNT_REG(glblMpCfgPtr->memAddrLAPIC));
      } while (count);

    return;

} /* uSleep */

/*
 * mpSendIPI
 *
 * Function to send an IPI to a target.
 */
void mpSendIPI(u32 targetID, u32 ipiNum)
{
    u32 icr64;
    u32 icr32;

    icr64 = targetID << 24;
    apicWrite(APIC_ICR_REG32(glblMpCfgPtr->memAddrLAPIC), icr64);
    icr32 = ipiNum;
    apicWrite(APIC_ICR_REG0(glblMpCfgPtr->memAddrLAPIC), icr32);
}

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

void smp_idle_loop()
{
	volatile u32 *ptr = (u32 *)MAIL_BOX;
	u32 myInt = 0;
    static int inc = 0;
    ctors_init();

	init_descriptor_tables();
  
#ifdef VERBOSE_BOOT
	kprintf("%d: Booted AP! (SP:%08x) stack:%x\n", ptr[0], ptr[1], &myInt);
#endif /* VERBOSE_BOOT */
  
	switch(ptr[0])
	{
		case 2:
			ptr[0] = 0xffffffff;
			lwrte_eu1();
			break;
		case 4:
			ptr[0] = 0xffffffff;
			lwrte_eu2();
			break;
		case 6:
			ptr[0] = 0xffffffff;
			lwrte_eu3();
			break;
    }
}

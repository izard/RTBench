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
#ifndef __MP_H__
#define __MP_H__

#include <param.h>
#include <apic.h>

#define MP_CONFIG_PROCESSOR    0
#define MP_CONFIG_BUS          1
#define MP_CONFIG_IOAPIC       2
#define MP_CONFIG_IOINTASSIGN  3
#define MP_CONFIG_LOCINTASSIGN 4

/*
 * We will support max of 32 for now.
 */
#define MP_MAX_PROCS           32

/*
 * Memory address for probing.
 *
 * According to the multiprocessor spec there are several
 * places where we could search for the MP Floating Ptr structure.
 *
 */
#define BIOS_EBDA  0x0009FC00 // This can be read from the BIOS Data area
#define BIOS_BASE  0x000F0000 // Per Spec
#define BIOS_BASE2 0x000E0000 // DragonFly BSD ....
#define MP_SEARCH_SZ 0x00010000

#define MP_FLT_PTR_SIG  0x5F504D5F /*_MP_*/
#define MP_CFG_TABLE_SIZE 0x504D4350 /* PCMP */

struct mp_float_ptr
{
  u32 mpFPSig;
  u32 physAddrPtr;
  u8  length;
  u8  specRev;
  u8  checksum;
  u8  mpFeatureByte1;
  u32 mpFeatureBytes2_5;
};

/*
 * MP Configuration Table Header
 */
struct mp_config_table
{
  u32 mpSig;
  u16 baseTblLen;
  u8  specRev;
  u8  checksum;
  u8  oemIDStr[8];
  u8  prodIdStr[12];
  void *oemTblPtr;
  u16 oemTblSize;
  u16 entryCnt;
  void *memAddrLAPIC;
  u16 extTblLen;
  u8  extTblChecksum;
  u8  rsvd;
};

/*
 * Processor entry in MP configuration table
 */

struct mp_proc_entry
{
  u8 entryType;
  u8 lapicID;
  u8 lapicVer;
#define MP_PROC_ENABLE  0x01
#define MP_PROC_BP      0x02
  u8 cpuFlags;
#define MP_SIG_STEPPING(xx) (((xx) >> 0) & 0xf)
#define MP_SIG_MODEL(xx)    (((xx) >> 4) & 0xf)
#define MP_SIG_FAMILY(xx)   (((xx) >> 8) & 0xf)
  u32 cpuSig;

#define MP_FF_FPU       0x00000001
#define MP_FF_MCE       0x00000080
#define MP_FF_CX8       0x00000100
#define MP_FF_APIC      0x00000200
  u32 featureFlgs;
  u32 rsvd1;
  u32 rsvd2;
};



struct mp_bus_entry
{
  u8 entryType;
  u8 busID;
  u8 busTypeString[6];
};

struct mp_ioapic_entry
{
  u8 entryType;
  u8 ioApicID;
  u8 ioApicVer;
#define MP_IOAPIC_EN   0x01
  u8 ioApicFlags;
  u32 ioApicAddr;
};


struct mp_iointassign_entry
{
  u8 entryType;
  u8 intType;
  u16 ioIntFlag;
  u8 srcBusId;
  u8 srcBusIRQ;
  u8 destIOApicID;
  u8 destIOApicInt;
};


struct mp_localassign_entry
{
  u8 entryType;
  u8 intType;
  u16 localIntFlag;
  u8 srcBusId;
  u8 srcBusIRQ;
  u8 destLocalApicID;
  u8 lesLocalApicInt;
};


/*
 * Following are the extended MP Table structure.
 */
struct mp_ext_sysaddr
{
  u8 entryType;
  u8 entryLen;
  u8 busId;
#define MP_EXT_ADDRTYPE_IO   0
#define MP_EXT_ADDRMEM       1
#define MP_EXT_ADDR_PREFETCH 2
  u8 addrType;
  u32 addrBase;
  u32 addrLen;
};

struct mp_ext_bus_hd_entry
{
  u8 entryType;
  u8 entrylen;
  u8 busId;
  u8 busInfo;
#define MP_EXT_PARENT_MASK 0x000000FF
  u32 parentBus;
};


struct mp_ext_bus_addr_mod_entry
{
  u8 entryType;
  u8 entryLen;
  u8 busId;
#define MP_EXT_ADDR_MOD_MASK 0x01
  u8 addrMod;
  u32 preDefRangeList;

};

struct mp_proc_info_entry
{
  u8 cpuId;
  u8 lapicID;
  u8 lapicVer;
  u8 cpuFlags;
  u32 cpuSig;
  u32 featureFlags;
};

/* ACPI Tables defines */

/*- RSD -*/
#define ACPI_ANKR_RSDP_STR "RSD PTR "
#define ACPI_ANKR_RSDP_LEN 8
typedef struct
{
    u8  signature[ACPI_ANKR_RSDP_LEN];
    u8  cs;
    u8  oemid[6];
    u8  revision;
    u32 rsdt_address;
    u32 len;
    u64 xsdt_address;
    u8  ex_cs;
    u8  rsv[3];
} sRSDPtr_t;

/*- RSDT -*/
#define ACPI_ANKR_RSDT_STR "RSDT"
#define ACPI_ANKR_RSDT_LEN 4
typedef struct
{
    u8  signature[ACPI_ANKR_RSDT_LEN];
    u32 len;
    u8  rev;
    u8  cs;
    u8  oemid[6];
    u8  oemtblid[8];
    u32 oemrev;
    u8  creatorid[4];
    u32 creatorrev;
    u32 headers[1];
} sRSDTPtr_t;

/*- RSDT -*/
#define ACPI_ANKR_MADT_STR "APIC"
#define ACPI_ANKR_MADT_LEN 4
typedef struct
{
    u8  signature[ACPI_ANKR_MADT_LEN];
    u32 len;
    u8  rev;
    u8  cs;
    u8  oemid[6];
    u8  oemtblid[8];
    u32 oemrev;
    u8  creatorid[4];
    u32 creatorrev;
    u32 lapicaddress;
    u32 flags;
    u8  structdata[1];
} sMADTPtr_t;

#define ACPI_APIC_TYPE_LAPIC     0
#define ACPI_APIC_TYPE_IOAPIC    1
#define ACPI_APIC_TYPE_ISO       2
#define ACPI_APIC_TYPE_NMI       3
#define ACPI_APIC_TYPE_LAPIC_NMI 4
#define ACPI_APIC_TYPE_LAPIC_AO  5
#define ACPI_APIC_TYPE_IOSAPIC   6
#define ACPI_APIC_TYPE_LSAPIC    7
#define ACPI_APIC_TYPE_PIS       8
#define ACPI_APIC_TYPE_PLx2APIC  9

#define ACPI_APIC_FLAG_ENABLED   1

typedef struct
{
    u8  type;
    u8  len;
    u8  acpiprocid;
    u8  apicid;
    u32 flags;
} sAPICStr_t;


u32 *mp_sarch_flt_sig(u32 *base, u32 *limit);
void printMPFloatStruct (struct mp_float_ptr *mflt);
void printMPConfigTable (struct mp_config_table *mpCfg);
void mp_parse_config_table (struct mp_config_table *mpCfg);
struct mp_float_ptr *mp_scan(void);
i32 cpu_clock_freq(void);
void apic_set_timer (int value);
int apic_read_timer(void);
void uSleep(u32 timeInUSecs);
void mpSendIPI(u32 targetID, u32 ipiNum);
#endif /* __MP_H__ */

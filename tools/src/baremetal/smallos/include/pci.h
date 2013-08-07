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
#ifndef __PCI_H__
#define __PCI_H__

#define PCI_CONFIG_ADDR 0xCF8
#define PCI_CONFIG_DATA 0xCFC

#define PCI_ERROR_RETURN 0xFFFFFFFF


/*
 * This is the type 00 pci header. 
 * We don't build a list of PCI devices. We fill up a structure
 * with the required values.
 */
typedef struct pci_header_type00
{
  u16 vendorID;
  u16 deviceID;
  u16 command;
  u16 status;
  u8  revId;
  u8  progIF;
  u8  subClass;
  u8  classCode;
  u8  cacheLineSize;
  u8  latencyTimer;
  u8  headerType;
  u8  bist;
  u32 bar0;
  u32 bar1;
  u32 bar2;
  u32 bar3;
  u32 bar4;
  u32 bar5;
  u32 cardBusCISPtr;
  u16 subSystemVendorID;
  u16 subSystemID;
  u32 expROMBaseAddr;
  u32 capabilitiesPtr;
  u32 rsvd;
  u8  intLine;
  u8  intPin;
  u8  minGrant;
  u8  maxLatency;
} pci0_header;


/*
 * PCI Defines. 
 */
/*
 * These are offsets into the PCI Header. 
 */
#define PCI_VENDOR_ID    0x00
#define PCI_DEVICE_ID    0x02
#define PCI_COMMAND      0x04
#define PCI_STATUS       0x06
#define PCI_REVISION_ID  0x08
#define PCI_PROG_IF      0x09
#define PCI_SUB_CLASS    0x0A
#define PCI_CLASS_CODE   0x0B
#define PCI_CACHE_LINE_SIZE 0x0C
#define PCI_LATENCY_TIMER   0x0D
#define PCI_HEADER_TYPE     0x0E
#define PCI_BIST            0x0F
#define PCI_BAR0            0x10
#define PCI_BAR1            0x14
#define PCI_BAR2            0x18
#define PCI_BAR3            0x1C
#define PCI_BAR4            0x20
#define PCI_BAR5            0x24
#define PCI_CARDBUS_PTR     0x28
#define PCI_SUBSYS_VID      0x2C
#define PCI_SUBSYS_ID       0x2E
#define PCI_ROM_ADDR        0x30
#define PCI_CAP_PTR         0x34
#define PCI_INT_LINE        0x3C
#define PCI_INT_PIN         0x3D
#define PCI_MIN_GRANT       0x3E
#define PCI_MAX_LATENCY     0x3F

#define PCI_MAX_BUS         0xFF
#define PCI_MAX_DEVICES     0x1F
#define PCI_MAX_FUNCTIONS   0x07


u32 pci_read_config_reg(u16 bus, u16 device, u16 function,
  		u16 reg, u16 bytes);

void pci_write_config_reg(u16 bus, u16 device, u16 function,
			  u16 reg, u32 data, u16 bytes);

pci0_header *pci_get_header(pci0_header *pcihdr,
			    u16 vendor_id,
			    u16 device_id);

void pci_scan_bus(u16 bus, u16 dev, u16 func, u16 *vid, u16 *did);
void print_pci_info ();

#endif /* __PCI_H__ */

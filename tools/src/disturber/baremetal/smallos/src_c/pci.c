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
 * This file contains the PCI configuration mechanisms.
 *
 * We use configuration mechanism #1 to get the PCI address.
 *
 */
#include "param.h"
#include "utilities.h"
#include "pci.h"


/*
 * Read configuration information
 *
 */
u32 pci_read_config_reg(u16 bus, u16 device, u16 function,
  		u16 reg, u16 bytes)
{
  u32 address;
  u32 retval;


  /*
   * We can read 1, 2 or 4 bytes.
   */
  if (bytes == 3)
    return PCI_ERROR_RETURN;
  
  
  /*
   * Create the address we need to use. 
   */

  address = (((unsigned long)bus)<<16) | (((unsigned long)device) << 11)
    | (((unsigned long) function) << 8) | (reg &~0x03) | 0x80000000;


  
  // Write thisut to the configuration register.
  outl (PCI_CONFIG_ADDR, address);
  // Niw read the data back.

  switch (bytes)
    {
    case 1:
      return (inb(PCI_CONFIG_DATA+(reg&0x03)));
    case 2:
      return (inw(PCI_CONFIG_DATA+(reg&0x03)));
    case 4:
      return (inl(PCI_CONFIG_DATA+(reg&0x03)));

    } /* switch */


} /* end of pci_read_config_reg */


/*
 * pci_write_config_reg
 *
 * 
 */
void pci_write_config_reg(u16 bus, u16 device, u16 function,
			  u16 reg, u32 data, u16 bytes)
{
  u32 address;
  
  if (bytes == 3)
    return;

  /*
   * make the address.
   */
  address = (((unsigned long)bus)<<16) | (((unsigned long)device) << 11)
    | (((unsigned long) function) << 8) | (reg &~0x03) | 0x80000000;
  outl(PCI_CONFIG_ADDR, address);


  /*
   * Write the data to the configuration register.
   */
  switch(bytes)
    {
    case 1:
      outb((PCI_CONFIG_DATA+(reg&0x3)), (u8)data);
      break;
    case 2:
      outw((PCI_CONFIG_DATA+(reg&0x3)), (u16)data);
      break;
    case 4:
      outl((PCI_CONFIG_DATA+(reg&0x3)), (u32)data);
      break;
    }

}



/*
 * pci_get_header
 *
 * We use this function to get header information of a 
 * particular vendor_id/device_id combination. 
 *
 * We are really that bothered about efficiency here considering that 
 * we should be doing PCI scan only a few times at most.
 *
 */

pci0_header *pci_get_header(pci0_header *pcihdr,
			    u16 vendor_id,
			    u16 device_id)
{
  u16 myBus;
  u16 myDev;
  u16 myFunc;
  u16 vid;
  u16 did;

  /*
   * Start from bus 0 and loop through all the max.
   */

  for (myBus = 0; myBus < PCI_MAX_BUS; myBus++)
    {
      for (myDev = 0; myDev < PCI_MAX_DEVICES; myDev++)
	{
	  for (myFunc = 0; myFunc < PCI_MAX_FUNCTIONS; myFunc++)
	    {
	      //	      kprintf ("Checking Bus: 0x%x Device: 0x%x, Function: 0x%x\n", 
	      //		      myBus, myDev, myFunc);

	      vid = pci_read_config_reg(myBus, myDev, myFunc, PCI_VENDOR_ID, 2);
	      did = pci_read_config_reg(myBus, myDev, myFunc, PCI_DEVICE_ID, 2);
	      if ((vid == vendor_id) && (did == device_id))
		{

		  pcihdr->vendorID = vid;
		  pcihdr->deviceID = did;
		  pcihdr->command = pci_read_config_reg(myBus, myDev, myFunc, PCI_COMMAND,2);
		  pcihdr->status = pci_read_config_reg(myBus, myDev, myFunc, PCI_STATUS, 2);
		  pcihdr->revId = pci_read_config_reg(myBus, myDev, myFunc, PCI_REVISION_ID, 1);
		  pcihdr->progIF = pci_read_config_reg(myBus, myDev, myFunc, PCI_PROG_IF, 1); 
		  pcihdr->subClass = pci_read_config_reg(myBus, myDev, myFunc, PCI_SUB_CLASS, 1);
		  pcihdr->classCode = pci_read_config_reg(myBus, myDev, myFunc, PCI_CLASS_CODE, 1);
		  pcihdr->cacheLineSize = pci_read_config_reg(myBus, myDev, myFunc,
							      PCI_CACHE_LINE_SIZE, 1);
		  pcihdr->latencyTimer = pci_read_config_reg(myBus, myDev, myFunc,PCI_LATENCY_TIMER,1);
		  pcihdr->headerType = pci_read_config_reg(myBus, myDev, myFunc, PCI_HEADER_TYPE, 1);
		  pcihdr->bist = pci_read_config_reg(myBus, myDev, myFunc,PCI_BIST, 1);
		  pcihdr->bar0 = pci_read_config_reg(myBus, myDev, myFunc, PCI_BAR0, 4);
		  pcihdr->bar1 = pci_read_config_reg(myBus, myDev, myFunc, PCI_BAR1, 4);
		  pcihdr->bar2 = pci_read_config_reg(myBus, myDev, myFunc, PCI_BAR2, 4);
		  pcihdr->bar3 = pci_read_config_reg(myBus, myDev, myFunc, PCI_BAR3, 4);
		  pcihdr->bar4 = pci_read_config_reg(myBus, myDev, myFunc, PCI_BAR4, 4);
		  pcihdr->bar5 = pci_read_config_reg(myBus, myDev, myFunc, PCI_BAR5, 4);
		  pcihdr->cardBusCISPtr = pci_read_config_reg(myBus, myDev, myFunc,PCI_CARDBUS_PTR, 4);
		  pcihdr->subSystemVendorID = pci_read_config_reg(myBus, myDev, myFunc, 
								  PCI_SUBSYS_VID, 2);
		  pcihdr->subSystemID = pci_read_config_reg(myBus, myDev, myFunc,PCI_SUBSYS_ID, 2);
		  pcihdr->expROMBaseAddr = pci_read_config_reg(myBus, myDev, myFunc, PCI_ROM_ADDR, 4);
		  pcihdr->capabilitiesPtr = pci_read_config_reg(myBus, myDev, myFunc,PCI_CAP_PTR, 4);
		  pcihdr->intLine = pci_read_config_reg(myBus, myDev, myFunc,PCI_INT_LINE, 1);
		  pcihdr->intPin = pci_read_config_reg(myBus, myDev, myFunc, PCI_INT_PIN, 1);
		  pcihdr->minGrant = pci_read_config_reg(myBus, myDev, myFunc,PCI_MIN_GRANT, 1);
		  pcihdr->maxLatency = pci_read_config_reg(myBus, myDev, myFunc,PCI_MAX_LATENCY, 1);
		  return (pcihdr);
		}

	    }

	}
      

    }

  return NULL;

} /* End of pci_get_header. */




/*
 *
 * pci_scan_bus
 *
 * This function is used to scan the PCI bus and print out the 
 * bus and device id.
 *
 * If we ever get to a command line interface this would be a good function
 * for debugging.
 */
void pci_scan_bus(u16 bus, u16 dev, u16 func, u16 *vid, u16 *did)
{
  *vid = pci_read_config_reg(bus, dev, func, 
			     PCI_VENDOR_ID, 2);
  *did = pci_read_config_reg(bus, dev, func, 
			     PCI_DEVICE_ID, 2);
  return;

} /* pci_scan_bus */

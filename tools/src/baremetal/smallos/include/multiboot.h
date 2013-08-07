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
#ifndef __multiboot_h__
#define __multiboot_h__

struct mb_header
{
  unsigned long magic;
  unsigned long flags;
  unsigned long checksum;
  unsigned long hdr_addr;
  unsigned long load_addr;
  unsigned long load_end_addr;
  unsigned long bss_end;
  unsigned long entry_addr;
} __attribute__ ((packed));

struct elf_header
{
  u32 num;
  u32 size;
  u32 addr;
  u32 shndx;
} __attribute__((packed));

struct mb_info
{
  u32 flags;
  u32 mem_low;
  u32 mem_high;
  u32 boot_dev;
  u32 cmd_line;
  u32 mods_cnt;
  u32 mods_addr;
  struct elf_header elf_hdr;
  u32 mmap_len;
  u32 mmap_addr;
} __attribute__((packed));


struct memory_map
{
  u32 size;
  u32 base_addr_low;
  u32 base_addr_high;
  u32 len_low;
  u32 len_high;
  u32 type;
} __attribute__((packed));

#define NUM_OF_MEM_MAP 64

#endif /* __multiboot_h__ */

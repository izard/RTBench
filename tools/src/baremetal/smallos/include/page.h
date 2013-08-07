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
#ifndef __page_h__
#define __page_h__

#define PG_TB_ENTRIES 1024
#define PD_TB_ENTRIES 1024

#define ALIGN_MASK  0xFFFFF000
#define PAGE_SIZE_4KB   0x1000
#define FRAME_ALL_BITS 0xFFFFFFFF

/*
 * Access macros for frame bit map.
 */
#define FRM_VECTOR_SZ    32
#define FRAME_INDEX(frm) (frm/FRM_VECTOR_SZ)
#define FRAME_OFFSET(frm) (frm%FRM_VECTOR_SZ)

/*
 * Format of a 32 bit paget table entry that maps a 4KB page.
 * programmers manual vol3A table 4-6
 */
typedef struct pte
{
  u32 pres:1;   /* set to map to a 4KB page */
  u32 rw:1;     /* 0: no writes allowed. */
  u32 us:1;     /* User: 1, Supervisor : 0 */
  u32 pwt:1;    /* Page level write through */
  u32 pcd:1;    /* Page level cache disable. */
  u32 access: 1; /* Bit set if page was accessed. */
  u32 dirty:1;   /* Page set if software has written to the entry. */
  u32 pat: 1;    
  u32 global: 1; /* indicates whether the translation is global. */
  u32 rsrvd: 3;  /* Reserved. */
  u32 frame:20;  /* Frame address */
} pte_t;

/*
 * Page directory entry.
 * Format of a 32 bit page directory entry that references a page table.
 * table 4-5
 */

typedef struct pde
{
  u32 pres:1;   /* 1 if refers to a page table. */
  u32 rw: 1;    /* if 0 no writes allowed. */
  u32 us: 1;    /* If 0 page is supervisor only */
  u32 pwt: 1;   /* page level write through. */
  u32 pcd: 1;   /* page level cache disable */
  u32 access:1; /* Set if the entry was accessed. */
  u32 rsvd1: 1; /* Reserved bit */
  u32 psbit: 1; /* if cr4.pse = 1, then should be 0 for 4KB 1 for 4 MB */
  u32 rsvd2: 4; /* Reserved bits */
  u32 frame:20; /* physical address of the 4 KB page table. */
} pde_t;

/*
 * Page Table 
 *
 */

typedef struct page_table
{
  pte_t pages[PG_TB_ENTRIES];

} page_table_t;


/*
 * Page Table Directory
 * We maintain physical address to be equal to virtual address.
 * So paging doesn't do much in our case. 
 */
typedef struct page_directory
{
  pde_t page_dir[PD_TB_ENTRIES];
  page_table_t   *pageTableAddr[PD_TB_ENTRIES];  
  u32   pdPhysAddr;           
} page_directory_t;

pte_t *get_page(page_directory_t *pgDir, u32 addr);
void set_page_dir(page_directory_t *pgDir);


#endif /* __page_h__ */

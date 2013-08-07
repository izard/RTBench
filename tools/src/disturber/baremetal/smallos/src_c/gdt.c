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
 * File contains gdt/idt initialization routines.
 */

#include <param.h>
#include <utilities.h>
#include <segments.h>


// Lets us access our ASM functions from our C code.
extern void gdt_flush(u32);
extern void idt_flush(u32);

// Internal function prototypes.
static void init_gdt();
static void init_idt();
static void gdt_set_gate(s32,u32,u32,u8,u8);
static void idt_set_gate(u8,u32,u16,u8);

gdt_entry_t gdt_entries[NUM_OF_GDT_ENTRIES];
//gdt_ptr_t   gdt_ptr;
idt_entry_t idt_entries[NUM_OF_IDT_ENTRIES];
//idt_ptr_t   idt_ptr;

struct region_descriptor gdt_ptr;
struct region_descriptor idt_ptr;

// Initialisation routine - zeroes all the interrupt service routines,
// initialises the GDT and IDT.
void init_descriptor_tables()
{

    // Initialise the global descriptor table.
    init_gdt();
    // Initialise the interrupt descriptor table.
    init_idt();

}

static void init_gdt()
{
    gdt_ptr.limit = (sizeof(gdt_entry_t) * NUM_OF_GDT_ENTRIES) - 1;
    gdt_ptr.base  = (u32)&gdt_entries;

    gdt_set_gate(0, 0, 0, 0, 0);                // Null segment
    gdt_set_gate(1, 0x0, 0xFFFFFFFF,
            DESC_SEG_PRSNT|DESC_PRIV_LEVEL_0|SEG_CODE_OR_DATA|SEG_CODE_EXEC_RD,
            GRAN_4KCHUNK|DEV_OP_MODE_32|GRAN_NOT_64BIT|GRAN_NOT_AVL_SYS);
    gdt_set_gate(2, 0x0, 0xFFFFFFFF,
            DESC_SEG_PRSNT|DESC_PRIV_LEVEL_0|SEG_CODE_OR_DATA|SEG_DATA_RW,
            GRAN_4KCHUNK|DEV_OP_MODE_32|GRAN_NOT_64BIT|GRAN_NOT_AVL_SYS);
    gdt_set_gate(3, 0x0, 0xFFFFFFFF,
            DESC_SEG_PRSNT|DESC_PRIV_LEVEL_3|SEG_CODE_OR_DATA|SEG_CODE_EXEC_RD,
            GRAN_4KCHUNK|DEV_OP_MODE_32|GRAN_NOT_64BIT|GRAN_NOT_AVL_SYS);
    gdt_set_gate(4, 0x0, 0xFFFFFFFF,
            DESC_SEG_PRSNT|DESC_PRIV_LEVEL_3|SEG_CODE_OR_DATA|SEG_DATA_RW,
            GRAN_4KCHUNK|DEV_OP_MODE_32|GRAN_NOT_64BIT|GRAN_NOT_AVL_SYS);

    //while(1);
    gdt_flush((u32)&gdt_ptr);
}

// Set the value of one GDT entry.
static void gdt_set_gate(s32 num, u32 base, u32 limit, u8 access, u8 gran)
{
    gdt_entries[num].base_low    = (base & 0xFFFF);
    gdt_entries[num].base_middle = (base >> 16) & 0xFF;
    gdt_entries[num].base_high   = (base >> 24) & 0xFF;

    gdt_entries[num].limit_low   = (limit & 0xFFFF);
    gdt_entries[num].granularity = (limit >> 16) & 0x0F;

    gdt_entries[num].granularity |= gran & 0xF0;
    gdt_entries[num].access      = access;
}

static void init_idt()
{
    u8 seg_selector;
    u8 intr_flags;

    idt_ptr.limit = sizeof(idt_entry_t) * NUM_OF_IDT_ENTRIES -1;
    idt_ptr.base  = (u32)&idt_entries;

    memset((u8*)&idt_entries, 0, sizeof(idt_entry_t)*NUM_OF_IDT_ENTRIES);


    // Remap the irq table.
    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    outb(0x21, 0x20);
    outb(0xA1, 0x28);
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    outb(0x21, 0x0);
    outb(0xA1, 0x0);

    seg_selector = GDT_CS_SEG_P0 * sizeof (gdt_entry_t);
    intr_flags = DESC_SEG_PRSNT | DESC_PRIV_LEVEL_0|INTR_GATE_SZ_32|INTR_GATE_DEF;

    idt_set_gate( 0, (u32)isr0 , seg_selector, intr_flags);
    idt_set_gate( 1, (u32)isr1 , seg_selector, intr_flags);
    idt_set_gate( 2, (u32)isr2 , seg_selector, intr_flags);
    idt_set_gate( 3, (u32)isr3 , seg_selector, intr_flags);
    idt_set_gate( 4, (u32)isr4 , seg_selector, intr_flags);
    idt_set_gate( 5, (u32)isr5 , seg_selector, intr_flags);
    idt_set_gate( 6, (u32)isr6 , seg_selector, intr_flags);
    idt_set_gate( 7, (u32)isr7 , seg_selector, intr_flags);
    idt_set_gate( 8, (u32)isr8 , seg_selector, intr_flags);
    idt_set_gate( 9, (u32)isr9 , seg_selector, intr_flags);
    idt_set_gate(10, (u32)isr10, seg_selector, intr_flags);
    idt_set_gate(11, (u32)isr11, seg_selector, intr_flags);
    idt_set_gate(12, (u32)isr12, seg_selector, intr_flags);
    idt_set_gate(13, (u32)isr13, seg_selector, intr_flags);
    idt_set_gate(14, (u32)isr14, seg_selector, intr_flags);
    idt_set_gate(15, (u32)isr15, seg_selector, intr_flags);
    idt_set_gate(16, (u32)isr16, seg_selector, intr_flags);
    idt_set_gate(17, (u32)isr17, seg_selector, intr_flags);
    idt_set_gate(18, (u32)isr18, seg_selector, intr_flags);
    idt_set_gate(19, (u32)isr19, seg_selector, intr_flags);
    idt_set_gate(20, (u32)isr20, seg_selector, intr_flags);
    idt_set_gate(21, (u32)isr21, seg_selector, intr_flags);
    idt_set_gate(22, (u32)isr22, seg_selector, intr_flags);
    idt_set_gate(23, (u32)isr23, seg_selector, intr_flags);
    idt_set_gate(24, (u32)isr24, seg_selector, intr_flags);
    idt_set_gate(25, (u32)isr25, seg_selector, intr_flags);
    idt_set_gate(26, (u32)isr26, seg_selector, intr_flags);
    idt_set_gate(27, (u32)isr27, seg_selector, intr_flags);
    idt_set_gate(28, (u32)isr28, seg_selector, intr_flags);
    idt_set_gate(29, (u32)isr29, seg_selector, intr_flags);
    idt_set_gate(30, (u32)isr30, seg_selector, intr_flags);
    idt_set_gate(31, (u32)isr31, seg_selector, intr_flags);
    idt_set_gate(32, (u32)irq0, seg_selector, intr_flags);
    idt_set_gate(33, (u32)irq1, seg_selector, intr_flags);
    idt_set_gate(34, (u32)irq2, seg_selector, intr_flags);
    idt_set_gate(35, (u32)irq3, seg_selector, intr_flags);
    idt_set_gate(36, (u32)irq4, seg_selector, intr_flags);
    idt_set_gate(37, (u32)irq5, seg_selector, intr_flags);
    idt_set_gate(38, (u32)irq6, seg_selector, intr_flags);
    idt_set_gate(39, (u32)irq7, seg_selector, intr_flags);
    idt_set_gate(40, (u32)irq8, seg_selector, intr_flags);
    idt_set_gate(41, (u32)irq9, seg_selector, intr_flags);
    idt_set_gate(42, (u32)irq10, seg_selector, intr_flags);
    idt_set_gate(43, (u32)irq11, seg_selector, intr_flags);
    idt_set_gate(44, (u32)irq12, seg_selector, intr_flags);
    idt_set_gate(45, (u32)irq13, seg_selector, intr_flags);
    idt_set_gate(46, (u32)irq14, seg_selector, intr_flags);
    idt_set_gate(47, (u32)irq15, seg_selector, intr_flags);
    idt_set_gate(48, (u32)isr48, seg_selector, intr_flags);
    idt_set_gate(49, (u32)isr49, seg_selector, intr_flags);
    idt_set_gate(50, (u32)isr50, seg_selector, intr_flags);

    idt_flush((u32)&idt_ptr);

}

static void idt_set_gate(u8 num, u32 base, u16 sel, u8 flags)
{
    idt_entries[num].base_lo = base & 0xFFFF;
    idt_entries[num].base_hi = (base >> 16) & 0xFFFF;

    idt_entries[num].sel     = sel;
    idt_entries[num].always0 = 0;
    idt_entries[num].flags   = flags;
}

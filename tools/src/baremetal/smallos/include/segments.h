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
#ifndef __segments_h__
#define __segments_h__

#define NUM_OF_GDT_ENTRIES  5
#define NUM_OF_IDT_ENTRIES  256

#define SEG_DATA_RO             0x00
#define SEG_DATA_RO_ACC         0x01
#define SEG_DATA_RW             0x02
#define SEG_DATA_RW_ACC         0x03
#define SEG_DATA_RO_EXP_DWN     0x04
#define SEG_DATA_RO_EXP_ACC     0x05
#define SEG_DATA_RW_EXP_DWN     0x06
#define SEG_DATA_RW_EXP_DWN_ACC 0x07

#define SEG_CODE_EXEC           0x08
#define SEG_CODE_EXEC_ACC       0x09
#define SEG_CODE_EXEC_RD        0x0A
#define SEG_CODE_EXEC_RD_ACC    0x0B
#define SEG_CODE_EXEC_CFM       0x0C
#define SEG_CODE_EXEC_CFM_ACC   0x0D
#define SEG_CODE_EXEC_CFM_RD    0x0E
#define SEG_CODE_EXEC_CFM_RD_AC 0x0F


#define DESC_PRIV_LEVEL_0       0x00
#define DESC_PRIV_LEVEL_1       0x20
#define DESC_PRIV_LEVEL_2       0x40
#define DESC_PRIV_LEVEL_3       0x60

#define DESC_SEG_NOT_PRSNT      0x00
#define DESC_SEG_PRSNT          0x80

#define SEG_SYSTEM              0x00
#define SEG_CODE_OR_DATA        0x10

#define DEF_OP_MODE_16          0x00
#define DEV_OP_MODE_32          0x40

#define GRAN_BYTE               0x00
#define GRAN_4KCHUNK            0x80
#define GRAN_NOT_64BIT          0x00
#define GRAN_64BIT_MODE         0x20
#define GRAN_AVL_SYSTEM         0x10
#define GRAN_NOT_AVL_SYS        0x00

#define INTR_GATE_SZ_32         0x08
#define INTR_GATE_SZ_16         0x00

#define INTR_GATE_DEF           0x06

#define GDT_NULL_ENT            0x00
#define GDT_CS_SEG_P0           0x01
#define GDT_DS_SEG_P0           0x02
#define GDT_CS_SEG_P3           0x03
#define GDT_DS_SEG_P3           0x04

/*
 * Exception list
 */
#define INTR_DIV_BY_ZERO        0
#define INTR_RSVD               1
#define INTR_NMI                2
#define INTR_BRKPT              3
#define INTR_OVRFLOW            4
#define INTR_BOUNT_RANGE        5
#define INTR_INVOPCODE          6
#define INTR_DEV_NOT_AVAIL      7
#define INTR_DFAULT             8
#define INTR_SEG_OVRRUN         9
#define INTR_INV_TSS            10
#define INTR_SEG_NOTPRSNT       11
#define INTR_STK_SEG_FAULT      12
#define INTR_GNRL_PROT          13
#define INTR_PFAULT             14
#define INTR_RSVD1              15
#define INTR_X87                16
#define INTR_ALIGNCHK           17
#define INTR_MACHCHK            18
#define INTR_SIMD_EXCP          19

/*
 * Table 3-2 of System Programming Manual.
 */
#define SEG_DSCR_TYPE         0   /* Reserved */
#define SEG_DSCR_16BIT_TSS    1   /* 16 Bit TSS */
#define SEG_DSCR_LDT          2   /* LDT */
#define SEG_DSCR_16BIT_TSSB   3   /* 16 Bit TSS Busy */
#define SEG_DSCR_16BIT_CALLGT 4   /* 16 Bit Call Gate */
#define SEG_DSCR_TASK_GATE    5   /* Task Gate */
#define SEG_DSCR_16BIT_INTR   6   /* 16 Bit Interrupt Gate */
#define SEG_DSCR_16BIT_TRAP   7   /* 16 Bit Trap Gate */
#define SEG_DSCR_RESERVED     8   /* Reserved */
#define SEG_DSCR_32BIT_TSS    9   /* 32 Bit TSS */
#define SEG_DSCR_RESERVED_2   10  /* Reserved */
#define SEG_DSCR_32BIT_TSSB   11  /* 32 Bit TSS Busy */
#define SEG_DSCR_32BIT_CALLGT 12  /* 32 Bit Call Gate */
#define SEG_DSCR_RESERVED_3   13  /* Reserved */
#define SEG_DSCR_32BIT_INTR   14  /* 32 Bit Interrupt */
#define SEG_DSCR_32BIT_TRAP   15  /* 32 Bit Trap */


void init_descriptor_tables();


// This structure contains the value of one GDT entry.
// We use the attribute 'packed' to tell GCC not to change
// any of the alignment in the structure.
struct gdt_entry_struct
{
    u16 limit_low;           // The lower 16 bits of the limit.
    u16 base_low;            // The lower 16 bits of the base.
    u8  base_middle;         // The next 8 bits of the base.
    u8  access;              // Access flags, determine what ring this segment can be used in.
    u8  granularity;
    u8  base_high;           // The last 8 bits of the base.
} __attribute__((packed));

typedef struct gdt_entry_struct gdt_entry_t;

struct region_descriptor
{
  u16 limit;
  u32 base;
} __attribute__ ((packed));

// A struct describing an interrupt gate.
struct idt_entry_struct
{
    u16 base_lo;             // The lower 16 bits of the address to jump to when this interrupt fires.
    u16 sel;                 // Kernel segment selector.
    u8  always0;             // This must always be zero.
    u8  flags;               // More flags. See documentation.
    u16 base_hi;             // The upper 16 bits of the address to jump to.
} __attribute__((packed));

typedef struct idt_entry_struct idt_entry_t;

typedef struct registers
{
    u32 ds;                  // Data segment selector
    u32 edi, esi, ebp, esp, ebx, edx, ecx, eax; // Pushed by pusha.
    u32 int_no, err_code;    // Interrupt number and error code (if applicable)
    u32 eip, cs, eflags, useresp, ss; // Pushed by the processor automatically.
} registers_t;


#define IRQ0 32
#define IRQ1 33
#define IRQ2 34
#define IRQ3 35
#define IRQ4 36
#define IRQ5 37
#define IRQ6 38
#define IRQ7 39
#define IRQ8 40
#define IRQ9 41
#define IRQ10 42
#define IRQ11 43
#define IRQ12 44
#define IRQ13 45
#define IRQ14 46
#define IRQ15 47

typedef void (*isr_t)(registers_t);

// These extern directives let us access the addresses of our ASM ISR handlers.
extern void isr0 ();
extern void isr1 ();
extern void isr2 ();
extern void isr3 ();
extern void isr4 ();
extern void isr5 ();
extern void isr6 ();
extern void isr7 ();
extern void isr8 ();
extern void isr9 ();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();
extern void isr48();
extern void isr49();
extern void isr50();

extern void irq0 ();
extern void irq1 ();
extern void irq2 ();
extern void irq3 ();
extern void irq4 ();
extern void irq5 ();
extern void irq6 ();
extern void irq7 ();
extern void irq8 ();
extern void irq9 ();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();


void register_interrupt_handler(u8int n, isr_t handler);

#endif /* __segments_h__ */

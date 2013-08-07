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
/*
 * =====================================================================================
 *
 *       Filename:  lwrte.h
 *
 *    Description:  Header files for lwrte.c
 *
 *        Version:  1.0
 *        Created:  11/11/2010 05:44:29 AM
 *       Revision:  none
 *       Compiler:  icc
 *
 *         Author:  Yen-Ting Lu (yen-ting.p.lu@intel.com)
 *        Company:  Intel
 *
 * =====================================================================================
 */

#ifndef _LWRTE_H_
#define _LWRTE_H_

#include <param.h>
#include <mp.h>
#include <apic.h>

/* Assume LAPIC_BASE is at 0xFEE00000 */
#define APIC_BASE 0xFEE00000

/* definition for setup_apic_timer functions */
/* first parameter */
#define LAPIC_TMR_PERIODIC  1
#define LAPIC_TMR_ONESHOT   0

#define MSR_REG_CPU         0x198

void writeMsr(u32 msr, u64 value);
u64 readMsr(u32 msr);
u32 getCPU_multiplier();
u64  __timestamp();
void __monitor(const void *eax, unsigned long ecx, unsigned long edx);
void __mwait(unsigned long eax, unsigned long ecx);
void setup_apic_timer(int periodic, int vector, int divider);
void enable_apic_timer(u32 ticks);

#endif

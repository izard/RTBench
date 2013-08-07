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
 *       Filename:  apic.h
 *
 *    Description:  APIC related header
 *
 *        Version:  1.0
 *        Created:  11/11/2010 05:53:47 AM
 *       Revision:  none
 *       Compiler:  icc
 *
 *        Company:  Intel
 *
 * =====================================================================================
 */

#ifndef __APIC_H__
#define __APIC_H__

#include <param.h>

/*
 * APIC Registers MAP
 * Table 10-1 of the system programmers manual
 */

#define APIC_ID(addr)               ((u32 *)(addr + 0x0020))
#define APIC_VER(addr)              ((u32 *)(addr + 0x0030))
#define APIC_TASK_PRI_REG(addr)     ((u32 *)(addr + 0x0080))
#define APIC_ARB_PRI_REG(addr)      ((u32 *)(addr + 0x0090))
#define APIC_PROC_PRI_REG(addr)     ((u32 *)(addr + 0x00A0))
#define APIC_EOI_REG(addr)          ((u32 *)(addr + 0x00B0))
#define APIC_REM_READ_REG(addr)     ((u32 *)(addr + 0x00C0))
#define APIC_LGCL_DEST_REG(addr)    ((u32 *)(addr + 0x00D0))
#define APIC_DEST_FORMAT_REG(addr)  ((u32 *)(addr + 0x00E0))
#define APIC_SPUR_INT_VECT_REG(addr) ((u32 *)(addr + 0x00F0))
#define APIC_IN_SRV_REG0(addr)      ((u32 *)(addr + 0x0100))
#define APIC_IN_SRV_REG32(addr)     ((u32 *)(addr + 0x0110))
#define APIC_IN_SRV_REG64(addr)     ((u32 *)(addr + 0x0120))
#define APIC_IN_SRV_REG96(addr)     ((u32 *)(addr + 0x0130))
#define APIC_IN_SRV_REG128(addr)    ((u32 *)(addr + 0x0140))
#define APIC_IN_SRV_REG160(addr)    ((u32 *)(addr + 0x0150))
#define APIC_IN_SRV_REG192(addr)    ((u32 *)(addr + 0x0160))
#define APIC_IN_SRV_REG224(addr)    ((u32 *)(addr + 0x0170))
#define APIC_TMR_REG0(addr)         ((u32 *)(addr + 0x0180))
#define APIC_TMR_REG32(addr)        ((u32 *)(addr + 0x0190))
#define APIC_TMR_REG64(addr)        ((u32 *)(addr + 0x01A0))
#define APIC_TMR_REG96(addr)        ((u32 *)(addr + 0x01B0))
#define APIC_TMR_REG128(addr)       ((u32 *)(addr + 0x01C0))
#define APIC_TMR_REG160(addr)       ((u32 *)(addr + 0x01D0))
#define APIC_TMR_REG192(addr)       ((u32 *)(addr + 0x01E0))
#define APIC_TMR_REG224(addr)       ((u32 *)(addr + 0x01F0))
#define APIC_IRR_REG0(addr)         ((u32 *)(addr + 0x0200))
#define APIC_IRR_REG32(addr)        ((u32 *)(addr + 0x0210))
#define APIC_IRR_REG64(addr)        ((u32 *)(addr + 0x0220))
#define APIC_IRR_REG96(addr)        ((u32 *)(addr + 0x0230))
#define APIC_IRR_REG128(addr)       ((u32 *)(addr + 0x0240))
#define APIC_IRR_REG160(addr)       ((u32 *)(addr + 0x0250))
#define APIC_IRR_REG192(addr)       ((u32 *)(addr + 0x0260))
#define APIC_IRR_REG224(addr)       ((u32 *)(addr + 0x0270))

#define APIC_ERR_REG(addr)          ((u32 *)(addr + 0x0280))
#define APIC_LVT_REG(addr)          ((u32 *)(addr + 0x02F0))
#define APIC_ICR_REG0(addr)         ((u32 *)(addr + 0x0300))
#define APIC_ICR_REG32(addr)        ((u32 *)(addr + 0x0310))
#define APIC_LVT_TMR_REG(addr)      ((u32 *)(addr + 0x0320))
#define APIC_LVT_THRM_REG(addr)     ((u32 *)(addr + 0x0330))
#define APIC_LVT_PRF_REG(addr)      ((u32 *)(addr + 0x0340))
#define APIC_LVT_INT0_REG(addr)     ((u32 *)(addr + 0x0350))
#define APIC_LVT_INT1_REG(addr)     ((u32 *)(addr + 0x0360))
#define APIC_LVT_ERR_REG(addr)      ((u32 *)(addr + 0x0370))
#define APIC_INIT_COUNT_REG(addr)   ((u32 *)(addr + 0x0380))
#define APIC_LVT_CUR_COUNT_REG(addr) ((u32 *)(addr + 0x0390))
#define APIC_LVT_DIV_CFG_REG(addr)   ((u32 *)(addr + 0x03E0))


#define SET_APIC_ENABLE  0x00000100  /* Fig 10-31 of S/W Dev Man 3A */
#define APIC_TMR_DIV_BY2 0x00
#define APIC_TMR_DIV_BY4 0x01
#define APIC_TMR_DIV_BY8 0x02
#define APIC_TMR_DIV_BY16 0x03
#define APIC_TMR_DIV_BY32 0x08
#define APIC_TMR_DIV_BY64 0x09
#define APIC_TMR_DIV_BY128 0x0A
#define APIC_TMR_DIV_BY1 0x0B


#define APIC_LVTT_VECTOR 0x000000FF
#define APIC_LVTT_TMR_ONESHOT 0x00000000
#define APIC_LVTT_TMR_PERIODIC 0x0020000
#define APIC_LVTT_MASKED 0x00010000
#define APIC_LVTT_NOT_MASKED 0x00000000
#define APIC_LVTT_DLV_STAT_IDLE 0x00000000
#define APIC_LVTT_DLV_STAT_PEND 0x00001000

#define APIC_IPI_FIXED    0x00000000
#define APIC_IPI_LOWPRI   0x00000100
#define APIC_IPI_SMI      0x00000200
#define APIC_IPI_RSVD     0x00000300
#define APIC_IPI_NMI      0x00000400
#define APIC_IPI_INIT     0x00000500
#define APIC_IPI_STARTUP  0x00000600
#define APIC_IPI_RSVD1    0x00000700

#define APIC_DEST_MODE_PHYS 0x00000000
#define APIC_DEST_MODE_LOG  0x00000800
#define APIC_DLV_STS_IDLE   0x00000000
#define APIC_DLV_STS_PEND   (1 << 12)
#define APIC_LVL_ASSERT     (1 << 14)
#define APIC_TRIG_MODE_LEVEL (1<<15)
#define APIC_DST_SHORT_NONE  0
#define APIC_DST_SHORT_SELF  (0x01 << 18)
#define APIC_DST_ALL_SELF    (0x02 << 18)
#define APIC_DST_ALL_BUT_ME  (0x03 << 18)

volatile u32 apicRead(u32 *addr);
void apicWrite(u32 *addr, u32 data);

#endif

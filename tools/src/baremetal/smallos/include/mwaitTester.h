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
#ifndef __MWAITTESTER_H__
#define __MWAITTESTER_H__

#include <param.h>
#include <mp.h>
#include <segments.h>
#include <lwrte_config.h>
#include <lwrte.h>
//#include "apps/rubicon.h"

/* conditional enabled functions */
//#define LOOP_LOG                    TRUE
#define NO_MWAIT                    TRUE


/* MWAIT Monitor address */
#ifdef LWRTE_CPU2
#define MWAIT_CMD                   0x19000000
#endif
#ifdef LWRTE_CPU4
 #define MWAIT_CMD  	    0x19000040
#endif
#ifdef LWRTE_CPU6
 #define MWAIT_CMD		    0x19000080
#endif

#ifdef LWRTE_MP
 #define MWAIT_CMD		    0x19000080
#endif

#define MWAIT_CMD_ARG               0x19003000
#define MWAIT_CNT                   0x19005000
#define MWAIT_CMD_EU(x)             (MWAIT_CMD + 0x128*x)
#define MWAIT_CMD_ARG_EU(x)         (MWAIT_CMD_ARG + 0x10*x)
#define MWAIT_CNT_EU(x,y)           (MWAIT_CNT + (x*2+y) * 0x10)

/* define ISR vector numbers for latency related functions */
#define MWAIT_ISR 48

/* mwaitTester.c */
#define TESTVALUE                   0x12345678
#define CSTATE_PTR                  0x17000000
#define SCRATCH_PTR                 0x25000000
#define DATASET                     10000
/* These C-state values are for Nehalem family */
#define C1  0x00
#define C1E 0x01
#define C3  0x20
#define C6  0x50
/* Command lists */
#define CMD_HALT                        17
#define CMD_501                         24
#define CMD_502                         25
#define CMD_503_1                       26
#define CMD_503_2                       27
#define CMD_503_3                       28
#define CMD_504                         29
/* End Command lists */
void mwait_run();
void mwaitLoop();
void mwaitInit();

#endif

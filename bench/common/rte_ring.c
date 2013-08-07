/*-
 * Copyright (c) <2010,2011>, Intel Corporation
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
 * Derived from FreeBSD's bufring.c
 *
 **************************************************************************
 *
 * Copyright (c) 2007,2008 Kip Macy kmacy@freebsd.org
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. The name of Kip Macy nor the names of other
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ***************************************************************************/

#ifndef __baremetal__
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <errno.h>
#include <sys/queue.h>
#else
#include "/usr/include/sys/queue.h"
#define NULL 0
#endif

#include "rte_ring.h"

TAILQ_HEAD(rte_ring_list, rte_ring);
/* global list of ring (used for debug/dump) */
static struct rte_ring_list ring_list = TAILQ_HEAD_INITIALIZER(ring_list);

/* true if x is a power of 2 */
#define POWEROF2(x) ((((x)-1) & (x)) == 0)

#ifdef __baremetal__
struct test_ring
{
  struct rte_ring ring;
	unsigned char filler[32768 * 6 * sizeof(void *)];
};

// FIXME AK hack static allocator
static struct test_ring* baremetal_rings =  (struct test_ring *)(0x19000000 + 1000);
static int *baremetal_rings_allocated = 0x19000100;
#endif

/* create the ring */
struct rte_ring *
rte_ring_create(const char *name, unsigned count, 
		unsigned flags)
{
	char mz_name[4];
	struct rte_ring *r;
	const struct rte_memzone *mz;
	unsigned int ring_size;
	int mz_flags = 0;

	/* compilation-time checks */
//	RTE_BUILD_BUG_ON((sizeof(struct rte_ring) &
//			  63) != 0);
//	RTE_BUILD_BUG_ON((offsetof(struct rte_ring, cons) &
//			  63) != 0);
//	RTE_BUILD_BUG_ON((offsetof(struct rte_ring, prod) &
//			  63) != 0);
#ifdef RTE_LIBRTE_RING_DEBUG
	RTE_BUILD_BUG_ON((sizeof(struct rte_ring_debug_stats) &
			  CACHE_LINE_MASK) != 0);
	RTE_BUILD_BUG_ON((offsetof(struct rte_ring, stats) &
			  CACHE_LINE_MASK) != 0);
#endif

	/* count must be a power of 2 */
	if (!POWEROF2(count)) {
		//printf("Requested size is not a power of 2\n");
		return NULL;
	}

	//snprintf(mz_name, sizeof(mz_name), "RG_%s", name);
	ring_size = count * sizeof(void *) + sizeof(struct rte_ring);

	/* reserve a memory zone for this ring */
/*	mz = rte_memzone_reserve(mz_name, ring_size, socket_id, mz_flags);
	if (mz == NULL) {
		printf("Cannot reserve memory\n");
		return NULL;
	}*/

#ifndef __baremetal__
	r = malloc(ring_size);//mz->addr;
#else
	r = (struct test_ring*)baremetal_rings + *baremetal_rings_allocated;
	(*baremetal_rings_allocated)++;
//kprintf("allocating %x\n", *baremetal_rings_allocated);
#endif

	/* init the ring structure */
	memset(r, 0, sizeof(*r));
//	snprintf(r->name, sizeof(r->name), "%s", name);
	r->flags = flags;
	r->prod.bulk_default = r->cons.bulk_default = 1;
	r->prod.watermark = count;
	r->prod.sp_enqueue = !!(flags & RING_F_SP_ENQ);
	r->cons.sc_dequeue = !!(flags & RING_F_SC_DEQ);
	r->prod.size = r->cons.size = count;
	r->prod.mask = r->cons.mask = count-1;
	r->prod.head = r->cons.head = 0;
	r->prod.tail = r->cons.tail = 0;

	TAILQ_INSERT_TAIL(&ring_list, r, next);
	return r;
}

/*
 * change the high water mark. If *count* is 0, water marking is
 * disabled
 */
int
rte_ring_set_water_mark(struct rte_ring *r, unsigned count)
{
	if (count >= r->prod.size)
		return -1;

	/* if count is 0, disable the watermarking */
	if (count == 0)
		count = r->prod.size;

	r->prod.watermark = count;
	return 0;
}

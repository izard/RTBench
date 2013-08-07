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

// Do not include anything in baremetal, keep to the minimum in Linux !
#ifndef __baremetal__
#include <pthread.h>
#include <stdio.h>
#include <sched.h>
#include <stdarg.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <errno.h>
#include <sys/queue.h>
#define likely(x) __builtin_expect((x),1)
#define unlikely(x) __builtin_expect((x),0)
#else
#include "/usr/include/sys/queue.h"
#endif

#include "rte_ring.h"
//#define DEBUG
// "Canned" timed messages being delivered to apps from emulated IO
struct message
{
    unsigned int message_id;
    unsigned int recv_app;
    unsigned int payload1;
    unsigned char padding[52];
    unsigned char payload2[128];
};

// Allocate memory statically! 1M >> L2
struct rte_ring *app1, *app2, *app3, *app4;
struct rte_ring *out1, *out2;

// We have only 3 bare metal threads (4 cores, 1 core is running Linux)
// 1 pinned app thread
// [to keep architecture extensible to bigger number of threads!] 
#define N 40
int app_func(void *arg)
{
    struct rte_ring *ring1, *ring2;
    static struct message *msg1, *msg2;
    static struct message *msg3, *msg4;
    static struct message out_msg1, out_msg2;
    unsigned long long int t1 = 0, t2 = 0;
    int i = 0;
    out_msg1.message_id = 2;
    out_msg1.payload1 = 0;
    out_msg2.message_id = 2;
    out_msg2.payload1 = 0;

#ifndef __baremetal__
cpu_set_t cpuset;
CPU_ZERO(&cpuset);
CPU_SET(1, &cpuset);
pthread_t current_thread = pthread_self();
pthread_setaffinity_np(current_thread, sizeof(cpu_set_t), &cpuset);
#endif

    if (*(int*)arg == 0) {
        ring1 = app1; ring2 = app2; // Only this branch is used for now, for 3 threads
    } else {
        ring1 = app3; ring2 = app3;
    }

    // Simple round robin message parser/scheduler
    for (i = 0; i < N; i++)
    {
        int received1 = 0, received2 = 0;
        int received3 = 0, received4 = 0;
        // Scan the command queue, barrier
        while (!(received1 && received2 && received3 && received4))
        {
/*#ifdef DEBUG
#ifdef __baremetal__
kprintf
#else
printf
#endif
("app spin %d %d %d %d\n", received1, received2, received3, received4);
#endif*/
          if(!received1)
          {
            if(!rte_ring_mc_dequeue(ring1, (void **)&msg1))
	    {
#ifdef DEBUG
#ifdef __baremetal__
kprintf
#else
printf
#endif
("packet from io ring 1 %d %d\n", msg1->message_id, msg1->payload1);
#endif
                received1 = -1;
            }
          }
          if(!received2)
          {
            if(!rte_ring_mc_dequeue(ring2, (void **)&msg2))
            {
                received2 = -1;
#ifdef DEBUG
#ifdef __baremetal__
kprintf
#else
printf
#endif
("packet from io ring 2 %d %d\n", msg2->message_id, msg2->payload1);
#endif
            }
	  }
          if(!received3)
          {
            if(!rte_ring_mc_dequeue(ring1, (void **)&msg3))
            {
                received3 = -1;
#ifdef DEBUG
#ifdef __baremetal__
kprintf
#else
printf
#endif
("packet from io ring 12 %d %d\n", msg3->message_id, msg3->payload1);
#endif
            }
	  }
          if(!received4)
          {
            if(!rte_ring_mc_dequeue(ring2, (void **)&msg4))
            {
                received4 = -1;
#ifdef DEBUG
#ifdef __baremetal__
kprintf
#else
printf
#endif
("packet from io ring 22 %d %d\n", msg4->message_id, msg4->payload1);
#endif
            }
          }
        }
    	        if (t1 == 0) t1 = getticks();
		else
		{
#ifdef DEBUG
#ifdef __baremetal__
kprintf
#else
printf
#endif
("next iteration %d\n", (unsigned int)(getticks() - t1));
#endif
		}
        // Send stuff back, use mp function in case
        out_msg1.payload1++;
        int sent = rte_ring_mp_enqueue(out1, &out_msg1);
        out_msg2.payload1++;
        sent     = rte_ring_mp_enqueue(out2, &out_msg2);
    }
    t2 = getticks();
#ifndef __baremetal__
    printf
#else
    kprintf
#endif
    ("%d\n", ((unsigned int)(t2 - t1))/N);

#ifndef __baremetal__
    return 0;
#else
    return ((unsigned int)(t2 - t1))/N;
#endif
}

// 2 pinned comms threads, processing 2out and 4in buffers.
int io_func(void *arg)
{
    struct rte_ring *ring1, *ring2;
    static struct message *out_msg1, *out_msg2;
    static struct message msg1, msg2;
#ifndef __baremetal__
cpu_set_t cpuset;
CPU_ZERO(&cpuset);
pthread_t current_thread = pthread_self();
#endif
    msg1.payload1 = 0;
    msg2.payload1 = 0;

    if (*(int*)arg == 0) {
        msg1.message_id = 1;
        ring1 = out1;
#ifndef __baremetal__
CPU_SET(2, &cpuset);
pthread_setaffinity_np(current_thread, sizeof(cpu_set_t), &cpuset);
#endif
    } else {
        msg2.message_id = 3;
        ring1 = out2;
#ifndef __baremetal__
CPU_SET(3, &cpuset);
pthread_setaffinity_np(current_thread, sizeof(cpu_set_t), &cpuset);
#endif
    }

    // Simple round robin message sender/receiver
    while (1)
    {
        int received1 = 0, received2 = 0;
        unsigned long long t1 = getticks();
        // Try sending
        msg1.payload1++;
        int sent = rte_ring_mp_enqueue(app1, &msg1);

        if (-1 == sent)
        {// FIXME to handle the error
        }
        msg2.payload1++;
        sent = rte_ring_mp_enqueue(app2, &msg2);

        // receive output, barrier
        while (!received1)
        {
/*#ifdef DEBUG
#ifdef __baremetal__
kprintf
#else
printf
#endif
("io spin %d %d \n", received1, received2);
#endif*/
          if(!received1)
          {
            if(!rte_ring_mc_dequeue(ring1, (void **)&out_msg1))
            {
#ifdef DEBUG
#ifdef __baremetal__
kprintf
#else
printf
#endif
("packet from app ring 1 %d %d\n", out_msg1->message_id, out_msg1->payload1);
#endif
		if (out_msg1->payload1 == N)
			return 0;
                received1 = -1;
            }
          }
        }
    }

    return 0;
}

void create_rings()
{
	// Create ring queues
	app1 = rte_ring_create("app1", 32768, 0);
	app2 = rte_ring_create("app2", 32768, 0);
	app3 = rte_ring_create("app3", 32768, 0);
	app4 = rte_ring_create("app4", 32768, 0);

	out1 = rte_ring_create("out1", 32768, 0);
	out2 = rte_ring_create("out2", 32768, 0);
}

#ifndef __baremetal__
int main()
{
	create_rings();
	// start app thread
	pthread_t app_thread1, app_thread2;
	int arg1 = 0;
	pthread_create(&app_thread1, NULL, app_func, (void*)&arg1);

	// start io 2x threads
	pthread_t io_thread1, io_thread2;
	int arg3 = 0, arg4 = 1;
	pthread_create(&io_thread1, NULL, io_func, (void*)&arg3);
	pthread_create(&io_thread2, NULL, io_func, (void*)&arg4);

	// wait until senders are out of packets
	pthread_join(app_thread1, NULL);
}
#endif

#if defined __baremetal__
static int *baremetal_rings_allocated = 0x19000100;
static struct rte_ring **baremetal_rings_pointers = 0x19000200;
void set_rings_addresses()
{
	app1 = *baremetal_rings_pointers++;
	app2 = *baremetal_rings_pointers++;
	app3 = *baremetal_rings_pointers++;
	app4 = *baremetal_rings_pointers++;
	out1 = *baremetal_rings_pointers++;
	out2 = *baremetal_rings_pointers++;
}

int main_503_1()
{
	*baremetal_rings_allocated = 0;
	create_rings();
	*baremetal_rings_pointers++ = app1;
	*baremetal_rings_pointers++ = app2;
	*baremetal_rings_pointers++ = app3;
	*baremetal_rings_pointers++ = app4;
	*baremetal_rings_pointers++ = out1;
	*baremetal_rings_pointers++ = out2;
	int arg1 = 0;
	app_func((void*)&arg1);
}

int main_503_2()
{
	int arg3 = 0;
	set_rings_addresses();
	io_func((void*)&arg3);
}

int main_503_3()
{
	int arg4 = 1;
	set_rings_addresses();
	io_func((void*)&arg4);
}
#endif


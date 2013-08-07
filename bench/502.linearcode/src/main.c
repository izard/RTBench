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

#if defined __baremetal__
#elif defined __linux__
#include <stdlib.h>
#include <sys/mman.h>
#endif

#define DATA_SIZE 65536
// 512K of function calls, 8k of functions
#define FUNCTIONS_SIZE 512*1024
#define FUNCTIONS_NUMBER 8192
#define CALLS_SIZE 1024*1024*16
#define CALLS_NUM (CALLS_SIZE/10)
typedef void (*func)(void);

static volatile int i = 0;

// Use data for D-Cache references
unsigned char data[DATA_SIZE] = {};

// Simple function w/o parameters with single memory write inside
static unsigned char myproc[] =
{
0x48, 0xc7, 0xc1, 0x11, 0x11, 0x11, 0x11, 0xc3,
};

static unsigned char functions[FUNCTIONS_SIZE] __attribute__((aligned(0x1000)));

// 8MB of linear code
static unsigned char calls[CALLS_SIZE] __attribute__((aligned(0x1000)));

void build_calls(int len, int N, int A, int B)
{
 int C = B;
 int i;
 int offset = - (unsigned int)calls + (unsigned int)functions;

 for (i = 0; i < len; i++)
 {
  *(calls + i * 10) = 0xe8;

  // Set offset for the relative call
  *((int *)(calls + i * 10 + 1)) = (int)(offset + C*64 - 5 - 10*i);

  // Add EAX,IMM
  *((unsigned char *)(calls + i * 10 + 5)) = 0x05;
  *((unsigned char *)(calls + i * 10 + 6)) = 0xb3;
  *((unsigned char *)(calls + i * 10 + 7)) = 0x74;
  *((unsigned char *)(calls + i * 10 + 8)) = 0xa4;
  *((unsigned char *)(calls + i * 10 + 9)) = 0x02;


  C = (A*C+B) % N;
 }

*(calls + len * 10) = 0xc3;
}

#if defined __baremetal__
int main_502()
#elif defined __gnu_linux__
int main()
#endif
{
unsigned long long int t1,t2;
int pagelen = 4096;

#ifndef __baremetal__
if(mprotect(functions, pagelen*128, PROT_READ | PROT_WRITE | PROT_EXEC) != 0)
return -1;
if(mprotect(calls , pagelen*4096, PROT_READ | PROT_WRITE | PROT_EXEC) != 0)
return -2;
#endif

for (i = 0; i < FUNCTIONS_NUMBER; i++)
{
// One function per cache line, we won't be mean!
  memcpy((unsigned char*)functions + i*64 , myproc, sizeof(myproc));
// Add one memory access to the function, also single cache line
  *((unsigned int*)((unsigned char*)functions + 3 + i*64)) = (unsigned int)(data + i * 8);
}
//((func)functions)();

// build 10MB of linear code
build_calls(1024*1024, 8192, 509, 509);

t1 = getticks();
((func)calls)();
t2 = getticks();

#ifndef __baremetal__
printf("%i\n", ((unsigned int)(t2 - t1))/CALLS_NUM);
#endif

#ifndef __baremetal__
return 0;
#else
return ((unsigned int)(t2 - t1)/CALLS_NUM);
#endif
}

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
 * physical memory access utility
 *
 * Last modified date: 2011-10-21
 * Last modified by: Lu, Patrick <patrick.lu@intel.com>
 *
 * Contributor(s): Shah, Shrikant M <shrikant.m.shah@intel.com> 
 */
typedef unsigned long ulong;

#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/io.h>
#include <sys/mman.h>

#define SPAN (1<<10)  	// 1KB
#define CACHE_ALIGN 0xFFF
#define VERSION 1.0

ulong mem_span = SPAN;
ulong phy_addr; 
ulong page_offset = 0x0L;
ulong *mem_ptr;
ulong *logical_addr;
unsigned int lines_per_pages=256;

void fill_memory_with_user_input(ulong *start_addr, ulong data, unsigned int length);
void fill_mem_with_data_equal_address(ulong *start_addr, ulong phy_addr, unsigned int length);
void read_pattern(ulong *start_addr, unsigned int length);
void usage();

int main(int argc, char* argv[])
{
    int cmd;
    int fd;					/* The file descriptor for opening /dev/mem */
    unsigned long long data;
    ulong map_len;
    int dataflag = 0;
    int writeflag = 0;
    int init_memory_flag = 0;

    while((cmd = getopt(argc, argv, "l:p:d:wfn:h")) != EOF) 
    {
        switch(cmd) 
        {
            case 'l':
                mem_span = strtoul(optarg, (char**)NULL, 0);
                printf("length : 0x%x\n", mem_span);
                break;
            case 'p':
                phy_addr = strtoul(optarg, (char**)NULL, 0);
                printf("Physical address : 0x%llx\n", phy_addr);
                break;
            case 'w':
                writeflag = 1;
                printf("Memory Write operation\n");
                break;
            case 'd':
                dataflag = 1;
                data = strtoull(optarg, (char**)NULL, 0);
                printf("Data : 0x%llx\n", (unsigned long long) data);
                break;	
            case 'f':
                init_memory_flag = 1;
                printf("Make memory content equals to last 32-bit address\n");
                break;
            case 'n':
                lines_per_pages = strtoul(optarg, (char**)NULL, 0);
                printf("Number of lines per page: %d\n",lines_per_pages);
                break;
            case 'h':
            default:
                usage(argv[0]);
                return 0;
        }
    }

    if(phy_addr == 0)
    {
        usage(argv[0]);
        return -1;
    }

    if(writeflag && init_memory_flag)
    {
        printf("-w flag is set, ignoring -f operation\n");
        init_memory_flag = 0;
    }

    //make sure we are mapping at least one 4k page of memory
    if(mem_span >= 0x1000)
        map_len = mem_span;
    else
    {
        printf("adjust span size to 4K\n");
        map_len = 0x1000;
    }

    if(lines_per_pages < 0x10)
        lines_per_pages = 0x10;

    lines_per_pages &= ~0xF;

    //make sure we have root permissions
    if(iopl(3))
    {
        printf("Cannot get I/O permissions (being root helps)\n");
        return -1;
    }

    //open the main memory file
    fd = open ("/dev/mem", O_RDWR);

    //save the page offset
    page_offset = phy_addr & CACHE_ALIGN;

    //mask off the address to a page aligned address
    //0xfffff000 or 0xfffffffffffff000
    phy_addr &= ~CACHE_ALIGN;

    //perform the memory map
    ulong tempaddress;
    tempaddress = (ulong) mmap(NULL, map_len, PROT_READ|PROT_WRITE, MAP_SHARED, fd, (off_t)phy_addr);
    //check if it worked
    if(tempaddress == (ulong) MAP_FAILED)
    {
        perror("Mapping memory for absolute memory access failed.\n");
        return -3;
    }
    tempaddress |= page_offset;
    logical_addr = (ulong *) tempaddress;
    phy_addr |= page_offset;

    //check if we are doing a read or write
    if(writeflag)
    {
        switch(mem_span)
        {
            case 1:	//write a single byte
                printf("Writing 0x%x to location 0x%lx\n", data, phy_addr);
                *(char*)logical_addr = (char) data;
                break;
            case 2: //write a word
                printf("Writing 0x%x to location 0x%lx\n", data, phy_addr);
                *(short*)logical_addr = (short) data;
                break;
            case 4: //write a double word
                printf("Writing 0x%x to location 0x%lx\n", data, phy_addr);
                *(int*)logical_addr = (int) data;
                break;
            case 8: //write a quad word
                printf("Writing 0x%x to location 0x%lx\n", data, phy_addr);
                *(unsigned long long*)logical_addr = (unsigned long long) data;
                break;
            default: //write the data pattern repeating
                fill_memory_with_user_input(logical_addr,data, mem_span);	
        }	
    }
    else if(init_memory_flag)
    {
        fill_mem_with_data_equal_address(logical_addr,phy_addr+page_offset,mem_span);
    }
    else
    {
        printf("Memory locations : \n");
        read_pattern(logical_addr, mem_span);
    }
    printf("\nUnmap memory..\n");
    tempaddress &= CACHE_ALIGN;
    munmap((ulong *)tempaddress, mem_span);
    close(fd);
    return 0;
}

void usage(char * prog)
{
    printf("+++++++++++++++++++\n");
    printf("phymem version:%1.1f\n",VERSION);
    printf("+++++++++++++++++++\n");
    printf("Usage: %s -p <Physical Address> [-l <Length>] [-w <-d <data>>|-f] [-n <lines per page>]\n\n", prog);
    printf("%s is a utility to view and modify physical memory\n", prog);
    printf("Flags explanation:\n");
    printf("-p physical address where read or write operation should perform\n");
    printf("-l the length data should read or write\n");
    printf("-w write flag to enable write operation\n");
    printf("-d actual data will be write to the physical memory. Must be using with -w flag\n");
    printf("-f fill address with content being its address\n");
    printf("-n lines per page, each line contains 16 bytes\n");
}

/*
 * This function will fill memory with its last 32-bit address
 */
void fill_mem_with_data_equal_address(ulong *start_addr, ulong phy_addr, unsigned int length)
{
    int * tmp_ptr = (int*) start_addr;
    int i;
    printf("Filling memory with content equal to last 32-bit address for [%d] Bytes\n", length);
    printf("[");
    for(i=0;i<length;i+=4)
    {
        *tmp_ptr++ = (int) (phy_addr+i);
        printf(".");
        fflush(stdout);
    }
    printf("]\n");
    fflush(stdout);
}

void fill_memory_with_user_input(ulong *start_addr, ulong data, unsigned int length)
{
    int * tmp_ptr = (int*) start_addr;
    printf("Filling memory with user input [0x%x] for [%d] Bytes\n",data,length);
    printf("[");
    while(length > 0) 
    {
        *tmp_ptr++ = (int) data;
        printf(".");
        fflush(stdout);
        length -= sizeof(int);
    }
    printf("]\n");
    fflush(stdout);
}

void read_pattern(ulong *start_addr, unsigned int length)
{
    int * tmp_ptr = (int*) start_addr;
    ulong i,j,c;	
    j = (ulong)start_addr - (ulong)logical_addr + phy_addr;
    for(i=0; i<(length); i+=sizeof(int),j+=sizeof(int))
    {
        if(!(i%lines_per_pages)) c=getchar();
        if(c=='q') break;
        if(sizeof(j) == 8)
        {
            if(i%(16) == 0) printf("%llx:", j);
            printf(" %#010llx", *(tmp_ptr));
        }
        else
        {
            if(i%(16) == 0) printf("%x:", j);
            printf(" %#010x", *(tmp_ptr));
        }
        if((i+4)%(16) == 0) printf("\n");
        tmp_ptr++;
    }
}

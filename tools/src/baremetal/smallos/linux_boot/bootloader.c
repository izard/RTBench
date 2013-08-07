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

#include <fcntl.h>
#include <sys/io.h>
#include <sys/mman.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#define BUFLEN 100
#define FILELEN 80

typedef unsigned int u32;

int main(int argc, const char *argv[])
{

    FILE *fp;
    char buf[BUFLEN];
    char addr[10], origin[10],fourbyte[10], c;
    int  map_len, i, j, bytes, parsed[4], fd; 
    int  sleep_now=0, new_map=0;
    long hex_buf;
    unsigned long data, tempaddress, physicaladdress, *log_phy_addr, pageoffset=0x0L;
    unsigned long icr_address, pageoffset_icr_low, pageoffset_icr_high;
    char filename[FILELEN];
    unsigned long apid;
    unsigned long bAddr = 0x9F;
    int silent = 0;

    if(argc != 4)
    {
        printf("Usage:%s <file.bin> <0xAPID> <0xboot code addr>\n", argv[0]);
        exit(-1);
    }
    else if(strlen(argv[1])>FILELEN)
    {
        printf("Filename needs to be less than %d chars long\n", FILELEN);
        exit(-2);
    }
    else 
    {
        char *p;
        errno = 0;
        apid = strtoul(argv[2],&p,10);
        if(errno != 0 || *p != 0 || p == argv[2] || apid > 8 || apid == 0)
        {
            printf("argv2:%s\n",argv[2]);
            printf("APID:%lu incorrect! You should choose between 1 to 8\n",apid); 
            exit(-3);
        }
        errno = 0;
        bAddr = strtoul(argv[3],&p,16);
        if(errno != 0 || *p != 0 || p == argv[3] || bAddr > 0xFF)
        {
            printf("argv3:%s\n",argv[2]);
            printf("boot code addr:%lu incorrect! You should use < 0xFF\n",bAddr); 
            exit(-4);
        }
        strncpy(filename,argv[1],FILELEN);
        printf("Filename:%s\tAPID:0x%08x\tBoot Addr:0x%02x\n",filename, (apid<<24), bAddr);
    }

    //make sure we have root permissions
    if(iopl(3))
    {
        printf("Cannot get I/O permissions (being root helps)\n");
        return -1;
    }

    //open the main memory file
    fd = open ( "/dev/mem", O_RDWR);
    if ((fp = fopen(filename, "r")) == NULL) 
    {
        printf ("File not found\n");
        return -1;
    }else 
    {
        while(!(fgets(buf, 100, fp) == NULL))  //get a line
        {
            strncpy(origin, buf, 7);        
            if ((strncmp(origin, "/origin", 7)) == 0) 
            {
                if (new_map == 1)
                {
                    printf("Unmapping 'old' page\n");
                    tempaddress &= 0xfffff000;
                    munmap((void *)tempaddress, map_len);
                }

                strncpy(addr, buf+8, 8); //got the origin, lets get the physical address
                physicaladdress = strtoul(addr, NULL, 16);
                printf("Press key to start writing for this origin address 0x%x\n",physicaladdress);
                printf("Press y to auto load all data\n");
                if(!silent)
                {
                    c=getchar();
                    if(c == 'y')
                        silent=1;
                }

                map_len = 0x1000;  // 4K page
                pageoffset = physicaladdress & 0x00000fff; //save offset, need it later to map to address from 4K alignment
                physicaladdress = physicaladdress & 0xfffff000; //mask off to 4k aligned address

                tempaddress = (unsigned long) mmap(NULL, map_len, PROT_READ|PROT_WRITE, MAP_SHARED, fd, (off_t)physicaladdress);   

                if(tempaddress == (unsigned long)MAP_FAILED) //check if it worked
                {  
                    perror("Mapping memory for absolute memory access failed.\n");
                    return -3;
                }
                tempaddress |= pageoffset;
                log_phy_addr = (unsigned long *) tempaddress;
                physicaladdress |= pageoffset;
                new_map = 1;

            } else 
            {
                bytes = 0;
                for(i = 0; i < 4; i++) // get the whole line
                {
                    for(j=0; j < 4; j++) //get the 4 bytes
                    {
                        strncpy(fourbyte, buf + bytes, 2);
                        hex_buf = strtol(fourbyte, NULL, 16);
                        //printf("hex_buf: %x\n", hex_buf);
                        bytes += 3;
                        parsed[j] = hex_buf;
                    }
                    //data = parsed[0] << 24;
                    //data += parsed[1] << 16;
                    //data += parsed[2] << 8;
                    //data += parsed[3];
                    data = parsed[0] << 0;
                    data += parsed[1] << 8;
                    data += parsed[2] << 16;
                    data += parsed[3] << 24;
                    //printf("opcodes + data: %x\n", data);

                    //write now
                    *log_phy_addr = data;
                    log_phy_addr++;
                    data = 0;
                } 
            }
        } 
    } //end while
    //  fclose(fp);
    printf ("End of File\n");

    printf("Opcodes loaded in memory, ready to send INIT/SIPI sequence\n");
    physicaladdress = 0xfee00000;
    pageoffset_icr_low = 0x00000300;
    pageoffset_icr_high = 0x00000310;

    printf("Assuming APIC physical base: %lx \n", physicaladdress);
    tempaddress = (unsigned long) mmap(NULL, map_len, PROT_READ|PROT_WRITE, MAP_SHARED, fd, (off_t)physicaladdress);

    if(tempaddress == (unsigned long)MAP_FAILED) //check if it worked
    {
        perror("Mapping memory for absolute memory access failed.\n");
        return -3;
    }

    printf("Hit enter for INIT / SIPI / SIPI sequence\n");
    c=getchar();

    //prep ICR high APIC ID 01
    icr_address = tempaddress;
    icr_address |= pageoffset_icr_high;
    log_phy_addr = (unsigned long *) icr_address;
    *log_phy_addr = apid << 24;

    //prep ICR low Send INIT and wait
    icr_address = tempaddress;
    icr_address |= pageoffset_icr_low;
    log_phy_addr = (unsigned long *) icr_address;
    *log_phy_addr = 0x004500;
    sleep_now = usleep( 10000 ); //sleep 10 ms

    //prep for SIPI X2, prep ICR high
    icr_address = tempaddress;
    icr_address |= pageoffset_icr_high;
    log_phy_addr = (unsigned long *) icr_address;
    *log_phy_addr = apid << 24;

    icr_address = tempaddress;
    icr_address |= pageoffset_icr_low;
    log_phy_addr = (unsigned long *) icr_address;
    *log_phy_addr = 0x004600 | bAddr;  //02 = 2000h startup vector
    sleep_now = usleep( 500 ); //sleep 500 usec 

    icr_address = tempaddress;
    icr_address |= pageoffset_icr_high;
    log_phy_addr = (unsigned long *) icr_address;
    *log_phy_addr = apid << 24;

    icr_address = tempaddress;
    icr_address |= pageoffset_icr_low;
    log_phy_addr = (unsigned long *) icr_address;
    *log_phy_addr = 0x004600 | bAddr;  //02 = 2000h startup vector
    sleep_now = usleep( 500 ); //sleep 500 usec 

    printf("Unmapping APIC Base page\n");
    tempaddress &= 0xfffff000;
    munmap((void *)tempaddress, map_len);

    return 0;
}


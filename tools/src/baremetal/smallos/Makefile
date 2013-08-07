#;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
# 
#  Copyright (c) 2001-2009, Intel Corporation
#  All rights reserved.
# 
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions are met:
# 
#   1. Redistributions of source code must retain the above copyright notice,
#      this list of conditions and the following disclaimer.
# 
#   2. Redistributions in binary form must reproduce the above copyright
#      notice, this list of conditions and the following disclaimer in the
#      documentation and/or other materials provided with the distribution.
# 
#   3. Neither the name of the Intel Corporation nor the names of its
#      contributors may be used to endorse or promote products derived from
#      this software without specific prior written permission.
# 
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
#  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
#  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
#  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
#  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
#  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
#  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
#  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
#  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
#  POSSIBILITY OF SUCH DAMAGE.

PWD           := $(shell pwd)
MAIN_DIR       = $(PWD)/src_c
TESTS_DIR      = $(PWD)/../../../bench
ASM_DIR        = $(PWD)/src_asm
SCRIPTS_DIR    = $(PWD)/scripts
BOOTLOADER_DIR = $(PWD)/linux_boot
APP_DIR        = $(PWD)/apps

TEST_OBJS := $(shell ls ../../../../bench/5*/bin/*.o)

C_SRC_COMMON=$(MAIN_DIR)/isr.o $(MAIN_DIR)/gdt.o $(MAIN_DIR)/serial.o \
			 $(MAIN_DIR)/utilities.o $(MAIN_DIR)/pci.o $(MAIN_DIR)/lwrte.o \
			 $(MAIN_DIR)/spinlock.o $(MAIN_DIR)/cppsupport.o
ASM_SRC=$(ASM_DIR)/boot.o $(ASM_DIR)/asm_support.o
#ifneq ($(LWRTE), SMP)
APP_SRC=$(APP_DIR)/mwaitTester.o
#endif
HLT_SRC=$(ASM_DIR)/halt.o

ifeq ($(LWRTE), SMP)
C_SRC=$(C_SRC_COMMON) $(MAIN_DIR)/mp_main.o $(MAIN_DIR)/keyboard.o \
	  $(MAIN_DIR)/video.o $(MAIN_DIR)/timer.o $(MAIN_DIR)/mp.o \

#	  $(SH_SRC)
CONSOLE  = -D LWRTE_MP
AS_FLAG  = -D LWRTE_MP
ASM_SRC += $(ASM_DIR)/mp_boot.o
LDFLAGS  = -Ttlink_mp.ld
else
C_SRC    = $(C_SRC_COMMON) $(MAIN_DIR)/ap_main.o
ifeq ($(LWRTE_CPU2), 1)
LDFLAGS  = -Ttlink_ap.ld
endif
ifeq ($(LWRTE_CPU4), 1)
LDFLAGS  = -Ttlink_ap4.ld
endif
ifeq ($(LWRTE_CPU6), 1)
LDFLAGS  = -Ttlink_ap6.ld
endif
endif

SOURCES=$(C_SRC) $(ASM_SRC) $(APP_SRC) $(TEST_OBJS) ../../../../bench/common/rte_ring.o

INC_DIR   = -I$(PWD)/include
CFLAGS    = -nostdlib -nostdinc -fno-builtin -fno-stack-protector -m32 -fpack-struct
ASFLAGS   = $(AS_FLAG) -felf
# icc also works
CC        = gcc
CPP       = g++
CFLAGS   += $(CONSOLE) $(INC_DIR) -O3 -D__baremetal__
CPPFLAGS   += $(CONSOLE) $(INC_DIR) -m32 -O3
ifeq ($(LWRTE_CPU2), 1)
IMG_NAME = smallOS.bin
CFLAGS   += -DLWRTE_CPU2
CPPFLAGS   += -DLWRTE_CPU2
ASFLAGS   += -DLWRTE_CPU2
endif
ifeq ($(LWRTE_CPU4), 1)
IMG_NAME = smallOS4.bin
CFLAGS   += -DLWRTE_CPU4
CPPFLAGS   += -DLWRTE_CPU4
ASFLAGS   += -DLWRTE_CPU4
endif
ifeq ($(LWRTE_CPU6), 1)
IMG_NAME = smallOS6.bin
CFLAGS   += -DLWRTE_CPU6
CPPFLAGS   += -DLWRTE_CPU6
ASFLAGS   += -DLWRTE_CPU6
endif
#-xHost
LD        = ld

ifeq ($(LWRTE), SMP)
all: $(SOURCES) link
else
all: $(SOURCES) link hlt xxd
endif
	@make -C linux_boot

clean:
	@rm -f $(ASM_DIR)/*.o 
	@rm -f $(MAIN_DIR)/*.o 
	@rm -f $(SH_DIR)/*.o 
	@rm -f $(APP_DIR)/*.o 
	@rm -f kernel halt *.bin
#	rm -f $(BOOTLOADER_DIR)/*.bin 

link:
	@$(LD) $(LDFLAGS) -m elf_i386 -o kernel $(SOURCES)
ifeq ($(LWRTE), SMP)
	@cp kernel /boot
endif

%.o:    %.c
	@$(CC) $(CFLAGS) -c $< -o $@

%.o:    %.cpp
	@$(CPP) $(CPPFLAGS) -c $< -o $@

.s.o:
	@nasm $(ASFLAGS) $<

xxd:
	@cat kernel | xxd > kernel.bin
	@$(SCRIPTS_DIR)/parser.py -i kernel -o $(BOOTLOADER_DIR)/$(IMG_NAME) >/dev/null 2>&1

hlt:	$(HLT_SRC) link_hlt dis_hlt

link_hlt:
	@$(LD) $(LDFLAGS) -m elf_i386 -o halt $(HLT_SRC)

dis_hlt:
	@$(SCRIPTS_DIR)/parser.py -i halt -o $(BOOTLOADER_DIR)/halt.bin >/dev/null 2>&1


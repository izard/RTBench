
ifndef TARGET
$(error TARGET must be set)
endif

CC=gcc
CPP=g++

ifeq ($(TARGET), __linux__)
CFLAGS=-O3 -I../common -D__linux__
CCFLAGS=-O3 -I../common -D__linux__ -fno-exceptions -fno-rtti
endif

ifeq ($(TARGET), __baremetal__)
CFLAGS=-O3 -nostdlib -nostdinc -fno-builtin -fno-stack-protector -m32 -fpack-struct -c -I../common -D__baremetal__
CCFLAGS=-O3 -I../common -D__linux__ -fno-exceptions -fno-rtti -D__baremetal__ -m32
endif


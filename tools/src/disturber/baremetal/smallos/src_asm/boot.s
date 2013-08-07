;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; 
;  Copyright (c) 2001-2009, Intel Corporation
;  All rights reserved.
; 
;  Redistribution and use in source and binary forms, with or without
;  modification, are permitted provided that the following conditions are met:
; 
;   1. Redistributions of source code must retain the above copyright notice,
;      this list of conditions and the following disclaimer.
; 
;   2. Redistributions in binary form must reproduce the above copyright
;      notice, this list of conditions and the following disclaimer in the
;      documentation and/or other materials provided with the distribution.
; 
;   3. Neither the name of the Intel Corporation nor the names of its
;      contributors may be used to endorse or promote products derived from
;      this software without specific prior written permission.
; 
;  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
;  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
;  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
;  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
;  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
;  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
;  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
;  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
;  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
;  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
;  POSSIBILITY OF SUCH DAMAGE.
; 

%ifdef LWRTE_MP
    MULTIBOOT_MAGIC_HEADER  equ 0x1BADB002
    MULTIBOOT_BLDR_MAGIC    equ 0x2BADB002
    MULTIBOOT_HEADER_FLAGS  equ 0x00000003
    MULTIBOOT_CHECKSUM      equ -(MULTIBOOT_MAGIC_HEADER + MULTIBOOT_HEADER_FLAGS)
    LWRTE_STACK equ 0x101000
    [BITS 32]
    [global mboot]
    [EXTERN code]
    [EXTERN bss]
    [EXTERN end]
    [SECTION .boot]
    mboot:
        dd MULTIBOOT_MAGIC_HEADER
        dd MULTIBOOT_HEADER_FLAGS
        dd MULTIBOOT_CHECKSUM
        dd LWRTE_STACK
        dd mboot
        dd code
        dd bss
        dd end
        dd start
    [SECTION .text]
    [global start]
    [EXTERN mp_main]
    start:
        mov esp, LWRTE_STACK
        sub esp, 0x80
        push ebx
        push eax
        call mp_main

    dead_loop:
        jmp dead_loop
%else

%ifdef LWRTE_CPU4
    LWRTE_STACK equ 0x18002000
%endif


%ifdef LWRTE_CPU6
    LWRTE_STACK equ 0x18003000
%endif

%ifdef LWRTE_CPU2
   LWRTE_STACK equ 0x18001000
%endif

    [BITS 16]
    BASE equ 0x0
    [extern ap_main]
    [global start]

    [section .setup]
    start:
        cli
        xor ax, ax
        mov ds, ax
        mov ss, ax
        mov si, 0x1100 
        mov ax, 0xbaba
        mov [si], ax

        lgdt [GDTR]
    ; ---------------------------------------------------
    ; Goto protected mode
    ; ---------------------------------------------------

        xor ax, ax
        inc ax
        inc ax
        inc ax
        lmsw ax         ;set CR0 bit 0 to '1' enable the proteced mode, set CR0 bit 1 to '1' for coprocessor monitoring
        mov eax, 0x600
        mov cr4, eax 


        jmp dword 0x08:(do_pm-BASE)


    [BITS 32]   ; All code from now on will be 32-BITS
    [section .text]
    do_pm:
        ; jmp $  ; Debug barrier
        mov esp, LWRTE_STACK
        ; reload segment registers to enable new GDT
        mov ax, 0x10
        mov ds, ax
        mov es, ax
        mov fs, ax
        mov gs, ax
        mov ss, ax

        call ap_main
        jmp $  

    [section .gdt]
    GDTR:
        dw boot_gdt_end - boot_gdt_start - 1
        dd boot_gdt_start
    boot_gdt_start:
        dd 0x00000000, 0x00000000
        dd 0x0000FFFF, 0x00CF9B00   ;start CS at 0x18000000 = ~402MB
        dd 0x0000FFFF, 0x00CF9300   ;start DS at 0x00000000 
    boot_gdt_end:

    small_end:
%endif

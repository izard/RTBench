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
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Some compiler directives
; [ORG 7C00h]   ; BIOS puts us here

MAILBOX equ 0x9000

[GLOBAL ap_entry]
[GLOBAL ap_entry_end]
[EXTERN smp_idle_loop]
[EXTERN gdt_ptr]
[BITS 16]     ; The instructions below are 16-bit

ap_entry:
  cli

	xor ax, ax
	mov ds, ax
	mov ss, ax

	mov eax, (ap_entry_end - ap_entry + 0x1000)
	mov [eax], word (boot_gdt_end - boot_gdt_start - 1)
	mov [eax + 2], dword (boot_gdt_start - ap_entry + 0x1000)
    lgdt [eax]

; -------------------------------------------------------
; Goto protected mode
; -------------------------------------------------------

	xor eax, eax
	inc eax
	lmsw ax

    jmp dword 0x8:(do_pm - ap_entry + 0x1000)

[BITS 32]    ; All code from now on will be 32-bit
do_pm:
    	mov ax, 0x10      ; 0x10 is the offset in the GDT to our data segment
    	mov ds, ax        ; Load all data segment selectors
    	mov es, ax
    	mov fs, ax
    	mov gs, ax
    	mov ss, ax
    	jmp dword 0x8:(fflush - ap_entry + 0x1000)
fflush:
	mov eax, MAILBOX
	mov esp, [eax + 4]
	mov eax, smp_idle_loop
	jmp eax
mloop:
	hlt
	jmp mloop

boot_gdt_start:
	dd 0x00000000, 0x00000000
	dd 0x0000FFFF, 0x00CF9B00
	dd 0x0000FFFF, 0x00CF9300
boot_gdt_end:

ap_entry_end:

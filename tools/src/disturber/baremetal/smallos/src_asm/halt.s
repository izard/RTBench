[global start]
[BITS 16]

[SECTION .halt]
start:
    cli
    xor ax, ax
    mov ds, ax
    mov ss, ax
    mov si, 0x1100 
    mov ax, 0xdead
    mov [si], ax
    nop
    nop
    nop
    hlt

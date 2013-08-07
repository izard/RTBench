ENTRY(start)
SECTIONS
{
  .gdt 0x2000:
  {
    data = .; _data = .; __data = .;
    *(.gdt)
    . = ALIGN(4096);
  }

  .halt 0x9f000:
  {
    code = .; _code = .; __code = .;
    *(.halt)
    . = ALIGN(4096);
  }

  .setup 0x9000:
  {
    code = .; _code = .; __code = .;
    *(.setup)
    . = ALIGN(4096);
  }

  .text 0x18500000:
  {
    code = .; _code = .; __code = .;
    *(.text)
    . = ALIGN(4096);
  }

  .ctors :
  {
    KEEP( * (.ctors) )
    LONG(0)
  }

  .dtors :
  {
    KEEP( * (.dtors) )
    LONG(0)
  }

  .data 0x18700000:
  {
     data = .; _data = .; __data = .;
     *(.data)
     *(.rodata)
     . = ALIGN(4096);
  }

  .bss :
  {
    bss = .; _bss = .; __bss = .;
    *(.bss)
    . = ALIGN(4096);
  }

  end = .; _end = .; __end = .;
} 

CTORS_ADDR   = ADDR(.ctors); 

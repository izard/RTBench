ENTRY(start)
SECTIONS
{
  .boot 0x101000 :
  {
    code = .; _code = .; __code = .;
    *(.boot)
    . = ALIGN(4096);
  }

  .text 0x120000 :
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

  .data :
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


OUTPUT_FORMAT("elf32-littleriscv")
OUTPUT_ARCH( "riscv" )
ENTRY(_start)

SECTIONS
{
  . = 0x80000000;
  .text.init : { *(.text.init) }
  . = ALIGN(0x1000);
  .tohost : { *(.tohost) }
  . = ALIGN(0x1000);
  .text : { *(.text) }
  . = 0x80010000;
  .data : { *(.data) }
  .bss : { *(.bss) }
  _end = .;
}


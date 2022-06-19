OUTPUT_ARCH("riscv")
ENTRY(post_pp_entry)
SECTIONS
{
  . = 0x0; /* actually PIC */
  .text : {
    *(.text.entry)
    *(.text)
    *(.text*)
  }
  .rodata : {
    *(.rodata)
    *(.rodata*)
  }
  .data : {
    *(.data)
    *(.data*)
    *(.sdata)
    . = ALIGN(4);
    *(.bss_len)
  }
  _bss_start = .;
  .bss : {
    *(.bss)
    *(.sbss*)
  }
  . = ALIGN(4);
  _bss_end = .;
  kfd_list = .;

  /DISCARD/ : {
    *(.debug*)
    *(.comment)
  }
  .got : {
    *(.got)
    *(.got*)
  }
}

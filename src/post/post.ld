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
    *(.bss_len)
  }
  _bss_start = .;
  .bss : {
    *(.bss)
    *(.sbss*)
  }
  _bss_end = .;
}

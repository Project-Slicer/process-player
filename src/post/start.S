  .text
  .globl start_process
start_process:
  # TODO

  .section .bss_len
  .align 2
  .word _bss_end - _bss_start

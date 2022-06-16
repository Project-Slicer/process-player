#include "shared/riscv.h"

  .text
  # Restores the floating-point registers.
  # Arguments:
  #   - fpregs: address of the `fpregs_t` structure
  .globl restore_fpregs
restore_fpregs:
  FLOAD f0, 8(a0)
  FLOAD f1, 16(a0)
  FLOAD f2, 24(a0)
  FLOAD f3, 32(a0)
  FLOAD f4, 40(a0)
  FLOAD f5, 48(a0)
  FLOAD f6, 56(a0)
  FLOAD f7, 64(a0)
  FLOAD f8, 72(a0)
  FLOAD f9, 80(a0)
  FLOAD f10, 88(a0)
  FLOAD f11, 96(a0)
  FLOAD f12, 104(a0)
  FLOAD f13, 112(a0)
  FLOAD f14, 120(a0)
  FLOAD f15, 128(a0)
  FLOAD f16, 136(a0)
  FLOAD f17, 144(a0)
  FLOAD f18, 152(a0)
  FLOAD f19, 160(a0)
  FLOAD f20, 168(a0)
  FLOAD f21, 176(a0)
  FLOAD f22, 184(a0)
  FLOAD f23, 192(a0)
  FLOAD f24, 200(a0)
  FLOAD f25, 208(a0)
  FLOAD f26, 216(a0)
  FLOAD f27, 224(a0)
  FLOAD f28, 232(a0)
  FLOAD f29, 240(a0)
  FLOAD f30, 248(a0)
  FLOAD f31, 256(a0)
  lw    a0, 4(a0)
  csrw  fcsr, a0
  ret

  .section .bss_len
  .align 2
  .word _bss_end - _bss_start

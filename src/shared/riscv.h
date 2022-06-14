#ifndef PP_SHARED_RISCV_H_
#define PP_SHARED_RISCV_H_

#define RISCV_PGSHIFT 12
#define RISCV_PGSIZE (1 << RISCV_PGSHIFT)

#if __riscv_xlen == 64
#define REGBYTES 8
#define LOAD ld
#else
#define REGBYTES 4
#define LOAD lw
#endif

#ifdef __riscv_flen
#if __riscv_flen == 64
#define FLOAD fld
#elif __riscv_flen == 32
#define FLOAD flw
#else
#error "Unsupported floating point length"
#endif
#endif

#endif  // PP_SHARED_RISCV_H_

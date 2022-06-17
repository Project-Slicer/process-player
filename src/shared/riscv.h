#ifndef PP_SHARED_RISCV_H_
#define PP_SHARED_RISCV_H_

#define RISCV_PGSHIFT 12
#define RISCV_PGSIZE (1 << RISCV_PGSHIFT)

#ifdef __riscv_flen
#if __riscv_flen == 64
#define FLOAD fld
#elif __riscv_flen == 32
#define FLOAD flw
#else
#error "Unsupported floating point length"
#endif
#endif

// page table entry (PTE) fields
#define PTE_R 0x002  // Read
#define PTE_W 0x004  // Write
#define PTE_X 0x008  // Execute

#endif  // PP_SHARED_RISCV_H_

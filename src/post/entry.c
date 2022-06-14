#include "post/mem.h"
#include "shared/dump.h"
#include "shared/utils.h"

// functions defined in `start.S`
extern void restore_fpregs(fpregs_t *fpregs);
extern void start_process(trapframe_t *tf) __attribute__((noreturn));

void __attribute__((section(".text.entry")))
post_pp_entry(int dirfd, int log_fd) {
  // initialize utils
  utils_init(dirfd, log_fd);

  // restore memory
  restore_memory();

  // restore floating point registers
#ifdef __riscv_flen
  fpregs_t fpregs;
  int fp_fd = openr_assert("fpregs");
  read_assert(fp_fd, &fpregs, sizeof(fpregs));
  close_assert(fp_fd);
  restore_fpregs(&fpregs);
#endif

  // restore trapframe and start process
  trapframe_t tf;
  int tf_fd = openr_assert("tf");
  read_assert(tf_fd, &tf, sizeof(tf));
  close_assert(tf_fd);
  start_process(&tf);
}

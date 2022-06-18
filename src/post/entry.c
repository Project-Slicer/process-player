#include <signal.h>
#include <sys/ptrace.h>
#include <unistd.h>

#include "post/mem.h"
#include "shared/dump.h"
#include "shared/syscall.h"
#include "shared/utils.h"

// defined in `fpregs.S`
extern void restore_fpregs(fpregs_t *fpregs);

static void __attribute__((noreturn)) pp_start(trapframe_t *tf) {
  register long a7 __asm__("a7") = SYS_PP_START;
  register long a0 __asm__("a0") = (long)tf;
  __asm__ volatile("ecall\n\t" : : "r"(a7), "r"(a0) : "memory");
  __builtin_unreachable();
}

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
  ASSERT(!ptrace(PTRACE_TRACEME) && !kill(getpid(), SIGSTOP));
  pp_start(&tf);
}

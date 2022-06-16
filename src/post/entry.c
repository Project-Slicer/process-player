#include <signal.h>
#include <sys/ptrace.h>
#include <unistd.h>

#include "post/mem.h"
#include "shared/dump.h"
#include "shared/syscall.h"
#include "shared/utils.h"

// defined in `fpregs.S`
extern void restore_fpregs(fpregs_t *fpregs);

static void pp_start(trapframe_t *tf) __attribute__((noreturn)) {
  register long a7 asm("a7") = SYS_PP_START;
  register long a0 asm("a0") = (long)tf;
  asm volatile("ecall\n\t" : : "r"(a7), "r"(a0) : "memory");
}

void post_pp_entry(int dirfd, int log_fd)
    __attribute__((section(".text.entry"))) {
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

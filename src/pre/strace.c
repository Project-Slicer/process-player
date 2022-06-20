#include "pre/strace.h"

#include <elf.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/ptrace.h>
#include <sys/uio.h>
#include <sys/wait.h>
#include <unistd.h>

#include "shared/dump.h"
#include "shared/syscall.h"
#include "shared/utils.h"

struct user_regs_struct {
  unsigned long pc;
  unsigned long ra;
  unsigned long sp;
  unsigned long gp;
  unsigned long tp;
  unsigned long t0;
  unsigned long t1;
  unsigned long t2;
  unsigned long s0;
  unsigned long s1;
  unsigned long a0;
  unsigned long a1;
  unsigned long a2;
  unsigned long a3;
  unsigned long a4;
  unsigned long a5;
  unsigned long a6;
  unsigned long a7;
  unsigned long s2;
  unsigned long s3;
  unsigned long s4;
  unsigned long s5;
  unsigned long s6;
  unsigned long s7;
  unsigned long s8;
  unsigned long s9;
  unsigned long s10;
  unsigned long s11;
  unsigned long t3;
  unsigned long t4;
  unsigned long t5;
  unsigned long t6;
};

int fuzzy_check_strace;  // set by --fuzzy-strace flag

static void wait_for_syscall(pid_t child) {
  int status;
  for (;;) {
    ASSERT(!ptrace(PTRACE_SYSCALL, child, 0, 0));
    ASSERT(waitpid(child, &status, 0) == child);
    if (LIKELY(WIFSTOPPED(status))) {
      if (LIKELY(WSTOPSIG(status) & 0x80)) return;
      raise(WSTOPSIG(status));
    }
    if (UNLIKELY(WIFEXITED(status))) exit(WEXITSTATUS(status));
  }
}

static int check_syscall(int strace_fd, pid_t child,
                         struct user_regs_struct *regs) {
  strace_t strace;
  ssize_t len = read(strace_fd, &strace, sizeof(strace));
  if (len == 0) {
    kill(child, SIGKILL);
    exit(0);
  } else if (len != sizeof(strace)) {
    return 1;
  } else {
    DBG("strace:");
    DBG("\tactual   num = %d, pc = %p, a0 = %d", (int)regs->a7, regs->pc - 4,
        (int)regs->a0);
    DBG("\texpected num = %d, pc = %p, a0 = %d", (int)strace.args[6],
        strace.epc, (int)strace.args[0]);
    if (!fuzzy_check_strace) {
      if (strace.args[0] != regs->a0) return 1;
      if (strace.args[1] != regs->a1) return 1;
      if (strace.args[2] != regs->a2) return 1;
      if (strace.args[3] != regs->a3) return 1;
      if (strace.args[4] != regs->a4) return 1;
      if (strace.args[5] != regs->a5) return 1;
    }
    if (strace.args[6] != regs->a7 || strace.epc != regs->pc - 4) return 1;
  }
  return 0;
}

static void restore_trapframe(pid_t child, struct user_regs_struct *regs) {
  // read trapframe from the child
  trapframe_t tf;
  for (size_t i = 0; i < sizeof(tf); i += sizeof(long)) {
    errno = 0;
    long val = ptrace(PTRACE_PEEKDATA, child, regs->a0 + i, 0);
    ASSERT(errno == 0);
    *((long *)&tf + i / sizeof(long)) = val;
  }

  // fill regs structure
  regs->pc = tf.epc;
  regs->ra = tf.gpr[1];
  regs->sp = tf.gpr[2];
  regs->gp = tf.gpr[3];
  regs->tp = tf.gpr[4];
  regs->t0 = tf.gpr[5];
  regs->t1 = tf.gpr[6];
  regs->t2 = tf.gpr[7];
  regs->s0 = tf.gpr[8];
  regs->s1 = tf.gpr[9];
  regs->a0 = tf.gpr[10];
  regs->a1 = tf.gpr[11];
  regs->a2 = tf.gpr[12];
  regs->a3 = tf.gpr[13];
  regs->a4 = tf.gpr[14];
  regs->a5 = tf.gpr[15];
  regs->a6 = tf.gpr[16];
  regs->a7 = tf.gpr[17];
  regs->s2 = tf.gpr[18];
  regs->s3 = tf.gpr[19];
  regs->s4 = tf.gpr[20];
  regs->s5 = tf.gpr[21];
  regs->s6 = tf.gpr[22];
  regs->s7 = tf.gpr[23];
  regs->s8 = tf.gpr[24];
  regs->s9 = tf.gpr[25];
  regs->s10 = tf.gpr[26];
  regs->s11 = tf.gpr[27];
  regs->t3 = tf.gpr[28];
  regs->t4 = tf.gpr[29];
  regs->t5 = tf.gpr[30];
  regs->t6 = tf.gpr[31];
}

int trace_syscall(pid_t child) {
  // open the system call trace file
  int strace_fd = openr_assert("strace");

  // setup regs structure
  struct user_regs_struct regs;
  struct iovec iov;
  iov.iov_len = sizeof(regs);
  iov.iov_base = &regs;

  // wait for the child to stop
  int status;
  ASSERT(waitpid(child, &status, 0) == child);
  if (WIFEXITED(status)) return WEXITSTATUS(status);
  if (WIFSIGNALED(status)) raise(WTERMSIG(status));

  // start tracing
  ASSERT(!ptrace(PTRACE_SETOPTIONS, child, 0, PTRACE_O_TRACESYSGOOD));
  for (;;) {
    // wait for the child to enter a system call
    wait_for_syscall(child);

    // read registers
    ASSERT(!ptrace(PTRACE_GETREGSET, child, NT_PRSTATUS, &iov));

    // check the system call
    if (LIKELY(regs.a7 != SYS_PP_START) &&
        UNLIKELY(check_syscall(strace_fd, child, &regs))) {
      kill(child, SIGKILL);
      log_error("system call trace mismatch\n");
      return 1;
    }

    // wait for the child to exit the system call
    wait_for_syscall(child);

    // handle system call `pp_start`
    if (UNLIKELY(regs.a7 == SYS_PP_START)) {
      restore_trapframe(child, &regs);
      ASSERT(!ptrace(PTRACE_SETREGSET, child, NT_PRSTATUS, &iov));
      DBG("process started");
    } else {
#ifndef NDEBUG
      ASSERT(!ptrace(PTRACE_GETREGSET, child, NT_PRSTATUS, &iov));
      DBG("\treturned %p", regs.a0);
#endif
    }
  }

  return 0;
}

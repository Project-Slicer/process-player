#include "strace.h"

#include <elf.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ptrace.h>
#include <sys/uio.h>
#include <sys/wait.h>
#include <unistd.h>

#include "dump.h"

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
    ptrace(PTRACE_SYSCALL, child, 0, 0);
    waitpid(child, &status, 0);
    if (WIFSTOPPED(status) && WSTOPSIG(status) & 0x80) return;
    if (WIFEXITED(status)) exit(WEXITSTATUS(status));
  }
}

static int check_syscall(int strace_fd, pid_t child,
                         struct user_regs_struct *regs) {
  strace_t strace;
  ssize_t len = read(strace_fd, &strace, sizeof(strace));
  if (len == 0) {
    kill(child, SIGKILL);
    return 0;
  } else if (len != sizeof(strace)) {
    return 1;
  } else {
    if (!fuzzy_check_strace) {
      if (strace.args[0] != regs->a0) return 1;
      if (strace.args[1] != regs->a1) return 1;
      if (strace.args[2] != regs->a2) return 1;
      if (strace.args[3] != regs->a3) return 1;
      if (strace.args[4] != regs->a4) return 1;
      if (strace.args[5] != regs->a5) return 1;
    }
    if (strace.args[6] != regs->a7 || strace.epc != regs->pc) return 1;
  }
  return 0;
}

int trace_syscall(const char *checkpoint_dir, pid_t child) {
  // open the system call trace file
  int dirfd = open(checkpoint_dir, O_DIRECTORY);
  int strace_fd = openat(dirfd, "strace", O_RDONLY);
  if (strace_fd < 0) {
    perror("failed to open strace file");
    return 1;
  }
  close(dirfd);

  // wait for the child to stop
  int status;
  if (waitpid(child, &status, 0) != child) {
    perror("failed to wait for child");
    return 1;
  }
  if (WIFEXITED(status)) return WEXITSTATUS(status);

  // start tracing
  ptrace(PTRACE_SETOPTIONS, child, 0, PTRACE_O_TRACESYSGOOD);
  for (;;) {
    // wait for the child to enter a system call
    wait_for_syscall(child);

    // read registers
    struct user_regs_struct regs;
    struct iovec iov;
    iov.iov_len = sizeof(regs);
    iov.iov_base = &regs;
    int ret = ptrace(PTRACE_GETREGSET, child, NT_PRSTATUS, &iov);
    if (ret < 0) {
      perror("failed to read registers of child");
      return 1;
    }

    // check the system call
    if (check_syscall(strace_fd, child, &regs)) {
      kill(child, SIGKILL);
      fprintf(stderr, "system call trace mismatch\n");
      return 1;
    }

    // wait for the child to exit the system call
    wait_for_syscall(child);
  }

  return 0;
}

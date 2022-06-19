#ifndef PP_POST_TINYLIBC_H_
#define PP_POST_TINYLIBC_H_

#include <stdarg.h>
#include <sys/types.h>

// ptrace
#define PTRACE_TRACEME 0

// signals
#define SIGABRT 6
#define SIGSTOP 17

// mmap
#define PROT_READ 0x1
#define PROT_WRITE 0x2
#define PROT_EXEC 0x4
#define MAP_PRIVATE 0x2
#define MAP_FIXED 0x10
#define MAP_ANONYMOUS 0x20
#define MAP_POPULATE 0x8000

// fcntl
#define O_RDONLY 00000000

// system call wrappers
#define SYSCALL0(number)                                           \
  ({                                                               \
    long _sys_result;                                              \
    register long __a7 asm("a7") = number;                         \
    register long __a0 asm("a0");                                  \
    asm volatile("ecall\n\t" : "=r"(__a0) : "r"(__a7) : "memory"); \
    _sys_result = __a0;                                            \
    _sys_result;                                                   \
  })
#define SYSCALL1(number, arg0)                                     \
  ({                                                               \
    long _sys_result;                                              \
    long _arg0 = (long)(arg0);                                     \
    register long __a7 asm("a7") = number;                         \
    register long __a0 asm("a0") = _arg0;                          \
    asm volatile("ecall\n\t" : "+r"(__a0) : "r"(__a7) : "memory"); \
    _sys_result = __a0;                                            \
    _sys_result;                                                   \
  })
#define SYSCALL2(number, arg0, arg1)                                          \
  ({                                                                          \
    long _sys_result;                                                         \
    long _arg0 = (long)(arg0);                                                \
    long _arg1 = (long)(arg1);                                                \
    register long __a7 asm("a7") = number;                                    \
    register long __a0 asm("a0") = _arg0;                                     \
    register long __a1 asm("a1") = _arg1;                                     \
    asm volatile("ecall\n\t" : "+r"(__a0) : "r"(__a7), "r"(__a1) : "memory"); \
    _sys_result = __a0;                                                       \
    _sys_result;                                                              \
  })
#define SYSCALL3(number, arg0, arg1, arg2)         \
  ({                                               \
    long _sys_result;                              \
    long _arg0 = (long)(arg0);                     \
    long _arg1 = (long)(arg1);                     \
    long _arg2 = (long)(arg2);                     \
    register long __a7 asm("a7") = number;         \
    register long __a0 asm("a0") = _arg0;          \
    register long __a1 asm("a1") = _arg1;          \
    register long __a2 asm("a2") = _arg2;          \
    asm volatile("ecall\n\t"                       \
                 : "+r"(__a0)                      \
                 : "r"(__a7), "r"(__a1), "r"(__a2) \
                 : "memory");                      \
    _sys_result = __a0;                            \
    _sys_result;                                   \
  })
#define SYSCALL6(number, arg0, arg1, arg2, arg3, arg4, arg5)              \
  ({                                                                      \
    long _sys_result;                                                     \
    long _arg0 = (long)(arg0);                                            \
    long _arg1 = (long)(arg1);                                            \
    long _arg2 = (long)(arg2);                                            \
    long _arg3 = (long)(arg3);                                            \
    long _arg4 = (long)(arg4);                                            \
    long _arg5 = (long)(arg5);                                            \
    register long __a7 asm("a7") = number;                                \
    register long __a0 asm("a0") = _arg0;                                 \
    register long __a1 asm("a1") = _arg1;                                 \
    register long __a2 asm("a2") = _arg2;                                 \
    register long __a3 asm("a3") = _arg3;                                 \
    register long __a4 asm("a4") = _arg4;                                 \
    register long __a5 asm("a5") = _arg5;                                 \
    asm volatile("ecall\n\t"                                              \
                 : "+r"(__a0)                                             \
                 : "r"(__a7), "r"(__a1), "r"(__a2), "r"(__a3), "r"(__a4), \
                   "r"(__a5)                                              \
                 : "memory");                                             \
    _sys_result = __a0;                                                   \
    _sys_result;                                                          \
  })

// system call numbers
#define SYS_openat 56
#define SYS_close 57
#define SYS_read 63
#define SYS_write 64
#define SYS_ptrace 117
#define SYS_kill 129
#define SYS_getpid 172
#define SYS_mmap 222

static inline int openat(int dirfd, const char *path, int flags) {
  return SYSCALL3(SYS_openat, dirfd, path, flags);
}

static inline int close(int fd) { return SYSCALL1(SYS_close, fd); }

static inline ssize_t read(int fd, void *buf, size_t count) {
  return SYSCALL3(SYS_read, fd, buf, count);
}

static inline long ptrace(int request) { return SYSCALL1(SYS_ptrace, request); }

static inline int kill(pid_t pid, int sig) {
  return SYSCALL2(SYS_kill, pid, sig);
}

static inline pid_t getpid() { return SYSCALL0(SYS_getpid); }

static inline void *mmap(void *addr, size_t length, int prot, int flags, int fd,
                         off_t offset) {
  return (void *)SYSCALL6(SYS_mmap, addr, length, prot, flags, fd, offset);
}

static inline void __attribute__((noreturn)) abort() {
  kill(getpid(), SIGABRT);
}

int vdprintf(int fd, const char *fmt, va_list ap);

#endif  // PP_POST_TINYLIBC_H_

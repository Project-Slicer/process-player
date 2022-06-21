#ifndef PP_PRE_STRACE_H_
#define PP_PRE_STRACE_H_

#include <sys/types.h>

extern int fuzzy_check_strace;
extern int print_cycles;

// Traces the syscall of the given pid.
// Returns the exit code of the process.
int trace_syscall(pid_t child);

#endif  // PP_PRE_STRACE_H_

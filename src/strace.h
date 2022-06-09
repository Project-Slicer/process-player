#ifndef PP_STRACE_H_
#define PP_STRACE_H_

#include <sys/types.h>

extern int fuzzy_check_strace;

// Traces the syscall of the given pid.
// Returns the exit code of the process.
int trace_syscall(const char *checkpoint_dir, pid_t child);

#endif  // PP_STRACE_H_

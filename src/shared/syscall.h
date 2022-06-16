#ifndef PP_SHARED_SYSCALL_H_
#define PP_SHARED_SYSCALL_H_

// System call number of the custom system call `pp_start`.
// `pp_start` will restore the trapframe and start the process.
// Arguments:
//   tf: trapframe_t*
#define SYS_PP_START 19980130

#endif  // PP_SHARED_SYSCALL_H_

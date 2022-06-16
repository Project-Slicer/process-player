#ifndef PP_PRE_FILE_H_
#define PP_PRE_FILE_H_

// Restores file descriptors.
// Returns kernel file descriptor list, which the first element is the number of
// kernel file descriptors. The list should be freed by `free()`.
int *restore_fds();

#endif  // PP_PRE_FILE_H_

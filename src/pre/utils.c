#include "shared/utils.h"

#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <sys/resource.h>
#include <unistd.h>

static int dirfd, log_fd = STDERR_FILENO;

// defined in `post.S`
extern void __attribute__((noreturn))
call_post_pp(int dirfd, int log_fd, uintptr_t sp, uintptr_t entry);

void utils_init(const char *checkpoint_dir) {
  // open the checkpoint directory
  dirfd = open(checkpoint_dir, O_DIRECTORY);
  PANIC_IF(dirfd < 0, "failed to open checkpoint directory %s", checkpoint_dir);

  // get the maximum number of open files
  struct rlimit rlim;
  ASSERT(!getrlimit(RLIMIT_NOFILE, &rlim));
  int new_dirfd = rlim.rlim_cur - 1;
  int new_stderr = rlim.rlim_cur - 2;

  // dup2 to new_dirfd
  ASSERT(dup2(dirfd, new_dirfd) == new_dirfd);
  close_assert(dirfd);
  dirfd = new_dirfd;

  // dup2 to new_stderr
  ASSERT(dup2(STDERR_FILENO, new_stderr) == new_stderr);
  log_fd = new_stderr;
}

void utils_post_init(uintptr_t sp, uintptr_t entry) {
  call_post_pp(dirfd, log_fd, sp, entry);
}

void log_error(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vdprintf(log_fd, fmt, args);
  va_end(args);
}

int openat_dir(const char *path, int flags) {
  return openat(dirfd, path, flags);
}

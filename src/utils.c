#include "utils.h"

#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <sys/resource.h>
#include <unistd.h>

static int dirfd;
static FILE *log_file;

void utils_init(const char *checkpoint_dir) {
  // open the checkpoint directory
  dirfd = open(checkpoint_dir, O_DIRECTORY);
  ASSERT(dirfd >= 0);

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
  ASSERT(log_file = fdopen(new_stderr, "w"));
}

void log_error(const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vfprintf(log_file, fmt, args);
  va_end(args);
  fflush(log_file);
}

int openr_assert(const char *path) {
  int fd = openat(dirfd, path, O_RDONLY);
  if (fd < 0) PANIC("failed to open %s", path);
  return fd;
}

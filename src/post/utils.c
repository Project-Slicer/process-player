#include "shared/utils.h"

#include <stdarg.h>

#include "post/tinylibc.h"

static int dirfd, log_fd;

void utils_init(int dirfd_, int log_fd_) {
  dirfd = dirfd_;
  log_fd = log_fd_;
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

#ifndef PP_UTILS_H_
#define PP_UTILS_H_

#include <stdlib.h>
#include <unistd.h>

#define PANIC(fmt, ...)                           \
  do {                                            \
    log_error("Panic: " fmt "\n", ##__VA_ARGS__); \
    abort();                                      \
  } while (0)

#define ASSERT(cond)                                   \
  do {                                                 \
    if (!(cond)) PANIC("Assertion failed: %s", #cond); \
  } while (0)

void utils_init(const char *checkpoint_dir);
void log_error(const char *fmt, ...);
int openr_assert(const char *path);

static inline void read_assert(int fd, void *buf, size_t size) {
  size_t nread = read(fd, buf, size);
  if (nread != size) PANIC("failed to read from fd %d", fd);
}

static inline void close_assert(int fd) {
  if (close(fd) < 0) PANIC("failed to close fd %d", fd);
}

#endif  // PP_UTILS_H_

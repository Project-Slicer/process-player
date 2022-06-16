#ifndef PP_SHARED_UTILS_H_
#define PP_SHARED_UTILS_H_

#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#define LIKELY(x) __builtin_expect((x), 1)
#define UNLIKELY(x) __builtin_expect((x), 0)

#define PANIC(fmt, ...)                           \
  do {                                            \
    log_error("Panic: " fmt "\n", ##__VA_ARGS__); \
    abort();                                      \
  } while (0)

#define ASSERT(cond)                                             \
  do {                                                           \
    if (UNLIKELY(!(cond))) PANIC("Assertion failed: %s", #cond); \
  } while (0)

#define ROUNDUP(a, b) ((((a)-1) / (b) + 1) * (b))

#ifndef PP_POST
void utils_init(const char *checkpoint_dir);
void utils_post_init(uintptr_t sp, uintptr_t entry) __attribute__((noreturn));
#else
void utils_init(int dirfd, int log_fd);
#endif

void log_error(const char *fmt, ...);
int openr(const char *path);

static inline int openr_assert(const char *path) {
  int fd = openr(path);
  if (fd < 0) PANIC("failed to open %s", path);
  return fd;
}

static inline void read_assert(int fd, void *buf, size_t size) {
  size_t nread = read(fd, buf, size);
  if (nread != size) PANIC("failed to read from fd %d", fd);
}

static inline void close_assert(int fd) {
  if (close(fd) < 0) PANIC("failed to close fd %d", fd);
}

#endif  // PP_SHARED_UTILS_H_

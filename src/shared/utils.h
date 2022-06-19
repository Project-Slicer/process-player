#ifndef PP_SHARED_UTILS_H_
#define PP_SHARED_UTILS_H_

#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#define LIKELY(x) __builtin_expect((x), 1)
#define UNLIKELY(x) __builtin_expect((x), 0)
#define ROUNDUP(a, b) ((((a)-1) / (b) + 1) * (b))

#define PANIC(fmt, ...)                           \
  do {                                            \
    log_error("Panic: " fmt "\n", ##__VA_ARGS__); \
    abort();                                      \
  } while (0)

#define PANIC_IF(cond, fmt, ...)                   \
  do {                                             \
    if (UNLIKELY(cond)) PANIC(fmt, ##__VA_ARGS__); \
  } while (0)

#define ASSERT(cond) PANIC_IF(!(cond), "Assertion failed: %s", #cond)

#ifdef NDEBUG
#define DBG(fmt, ...) \
  do {                \
  } while (0)
#else
#define DBG(fmt, ...)                             \
  do {                                            \
    log_error("Debug: " fmt "\n", ##__VA_ARGS__); \
  } while (0)
#endif

#ifndef PP_POST
void utils_init(const char *checkpoint_dir);
void __attribute__((noreturn)) utils_post_init(uintptr_t sp, uintptr_t entry);
#else
void utils_init(int dirfd, int log_fd);
#endif

void log_error(const char *fmt, ...);
int openat_dir(const char *path, int flags);

static inline int openr(const char *path) { return openat_dir(path, O_RDONLY); }

static inline int openr_assert(const char *path) {
  int fd = openr(path);
  PANIC_IF(fd < 0, "failed to open %s", path);
  return fd;
}

static inline void read_assert(int fd, void *buf, size_t size) {
  PANIC_IF(read(fd, buf, size) != size, "failed to read from fd %d", fd);
}

static inline void close_assert(int fd) {
  PANIC_IF(close(fd) < 0, "failed to close fd %d", fd);
}

#endif  // PP_SHARED_UTILS_H_

#ifndef PP_UTILS_H_
#define PP_UTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define PANIC(fmt, ...)                                 \
  do {                                                  \
    fprintf(stderr, "Panic: " fmt "\n", ##__VA_ARGS__); \
    abort();                                            \
  } while (0)

#define ASSERT(cond)                                   \
  do {                                                 \
    if (!(cond)) PANIC("Assertion failed: %s", #cond); \
  } while (0)

static inline void read_assert(int fd, void *buf, size_t size) {
  size_t nread = read(fd, buf, size);
  if (nread != size) PANIC("failed to read from fd %d", fd);
}

#endif  // PP_UTILS_H_

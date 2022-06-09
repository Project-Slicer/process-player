#ifndef PP_UTILS_H_
#define PP_UTILS_H_

#include <stdio.h>
#include <stdlib.h>

#define PANIC(fmt, ...)                                 \
  do {                                                  \
    fprintf(stderr, "Panic: " fmt "\n", ##__VA_ARGS__); \
    abort();                                            \
  } while (0)

#define ASSERT(cond)                                   \
  do {                                                 \
    if (!(cond)) PANIC("Assertion failed: %s", #cond); \
  } while (0)

#endif  // PP_UTILS_H_

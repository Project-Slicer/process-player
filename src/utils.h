#ifndef PP_UTILS_H_
#define PP_UTILS_H_

#include <stdio.h>
#include <stdlib.h>

#define PP_ASSERT(cond)                                 \
  do {                                                  \
    if (!(cond)) {                                      \
      fprintf(stderr, "Assertion failed: %s\n", #cond); \
      abort();                                          \
    }                                                   \
  } while (0)

#endif  // PP_UTILS_H_

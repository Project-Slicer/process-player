#ifndef PP_POST_UNCOMPRESS_H_
#define PP_POST_UNCOMPRESS_H_

#include <stdint.h>

typedef void (*uncompress_callback_t)(uint8_t byte);

// Uncompresses the given file.
int uncompress(int fd, uncompress_callback_t write_byte);

#endif  // PP_POST_UNCOMPRESS_H_

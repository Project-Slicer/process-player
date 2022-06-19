#include "post/tinylibc.h"

#include <stdbool.h>

static inline ssize_t write(int fd, const void *buf, size_t count) {
  return SYSCALL3(SYS_write, fd, buf, count);
}

static int vsnprintf(char *out, size_t n, const char *s, va_list vl) {
  bool format = false;
  size_t pos = 0;
  for (; *s; s++) {
    if (format) {
      switch (*s) {
        case 'p': {
          if (++pos < n) out[pos - 1] = '0';
          if (++pos < n) out[pos - 1] = 'x';
          long num = va_arg(vl, long);
          for (int i = 2 * sizeof(long) - 1; i >= 0; i--) {
            int d = (num >> (4 * i)) & 0xf;
            if (++pos < n) out[pos - 1] = (d < 10 ? '0' + d : 'a' + d - 10);
          }
          format = false;
          break;
        }
        case 'd': {
          int num = va_arg(vl, int);
          if (num < 0) {
            num = -num;
            if (++pos < n) out[pos - 1] = '-';
          }
          int digits = 1;
          for (int nn = num; nn /= 10; digits++)
            ;
          for (int i = digits - 1; i >= 0; i--) {
            if (pos + i + 1 < n) out[pos + i] = '0' + (num % 10);
            num /= 10;
          }
          pos += digits;
          format = false;
          break;
        }
        case 's': {
          const char *s2 = va_arg(vl, const char *);
          while (*s2) {
            if (++pos < n) out[pos - 1] = *s2;
            s2++;
          }
          format = false;
          break;
        }
        default:
          break;
      }
    } else if (*s == '%') {
      format = true;
    } else if (++pos < n) {
      out[pos - 1] = *s;
    }
  }
  if (pos < n) {
    out[pos] = 0;
  } else if (n) {
    out[n - 1] = 0;
  }
  return pos;
}

int vdprintf(int fd, const char *fmt, va_list ap) {
  char buf[256];
  int n = vsnprintf(buf, sizeof(buf), fmt, ap);
  return write(fd, buf, n);
}

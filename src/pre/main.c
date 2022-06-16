#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include "pre/file.h"
#include "pre/strace.h"
#include "shared/dump.h"
#include "shared/riscv.h"
#include "shared/utils.h"

static const char *checkpoint_dir;

// post PP related stuffs
#define POST_PP_STACK_SIZE 2048
extern char post_pp_begin, post_pp_end;

static void help(const char *progname) {
  printf("Process Player\n\n");
  printf("Usage: %s [OPTIONS] <CHECKPOINT_DIR>\n", progname);
  printf("Options:\n");
  printf("  -h, --help        Show this help message\n");
  printf("  --fuzzy-strace    Fuzzy check system call trace\n");
}

static void parse_args(int argc, const char *argv[]) {
  if (argc < 2) {
    help(argv[0]);
    exit(1);
  } else if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
    help(argv[0]);
    exit(0);
  } else if (strcmp(argv[1], "--fuzzy-strace") == 0) {
    fuzzy_check_strace = 1;
    checkpoint_dir = argv[2];
  } else {
    checkpoint_dir = argv[1];
  }
}

static bool check_platinfo() {
  platinfo_t platinfo;
  int fd = openr_assert("platinfo");
  read_assert(fd, &platinfo, sizeof(platinfo));
  close_assert(fd);

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
  const int endian = 0;
#else
  const int endian = 1;
#endif

  if (platinfo.magic[0] != 'p' || platinfo.magic[1] != 'i') return false;
  if (platinfo.endian != endian) return false;
  if (platinfo.ptr_size != sizeof(void *)) return false;
  if (platinfo.page_size != RISCV_PGSIZE) return false;
  return platinfo.major <= PLATINFO_MAJOR && platinfo.minor <= PLATINFO_MINOR;
}

static char *get_post_pp_base(size_t len) {
  // TODO
}

int main(int argc, const char *argv[]) {
  // parse command line arguments
  parse_args(argc, argv);

  // initialize utils
  utils_init(checkpoint_dir);

  // check platform information
  if (!check_platinfo()) {
    PANIC("invalid checkpoint, platform information mismatch");
  }

  // start the system call tracer
  pid_t child = fork();
  if (child != 0) return trace_syscall(checkpoint_dir, child);

  // restore file descriptors
  restore_fds();

  // scan for address space holes
  size_t bss_len = (size_t)(*((uint32_t *)&post_pp_end - 1));
  size_t post_pp_len =
      ROUNDUP(&post_pp_end - &post_pp_begin + bss_len + POST_PP_STACK_SIZE, 16);
  char *post_pp_base = get_post_pp_base(post_pp_len);

  // load the post PP and initialize
  if (mmap(post_pp_base, post_pp_len, PROT_READ | PROT_WRITE | PROT_EXEC,
           MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED, -1, 0) != post_pp_base) {
    PANIC("failed to map the post PP");
  }
  memcpy(post_pp_base, &post_pp_begin, &post_pp_end - &post_pp_begin);
  memset(post_pp_base + (&post_pp_end - &post_pp_begin), 0, bss_len);

  // call the post part of PP
  uintptr_t post_pp_sp = (uintptr_t)(post_pp_base + post_pp_len);
  utils_post_init(post_pp_sp, (uintptr_t)post_pp_base);
}

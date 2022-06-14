#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "pre/file.h"
#include "pre/strace.h"
#include "shared/dump.h"
#include "shared/riscv.h"
#include "shared/utils.h"

static const char *checkpoint_dir;

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
  const int endianess = 0;
#else
  const int endianess = 1;
#endif

  if (platinfo.magic[0] != 'p' || platinfo.magic[1] != 'i') return false;
  if (platinfo.endian != endianess) return false;
  if (platinfo.ptr_size != sizeof(void *)) return false;
  if (platinfo.page_size != RISCV_PGSIZE) return false;
  return platinfo.major <= PLATINFO_MAJOR && platinfo.minor <= PLATINFO_MINOR;
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

  // TODO

  return 0;
}

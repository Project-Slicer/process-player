#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "file.h"
#include "strace.h"

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

int main(int argc, const char *argv[]) {
  // parse command line arguments
  parse_args(argc, argv);

  // start the system call tracer
  pid_t child = fork();
  if (child != 0) return trace_syscall(checkpoint_dir, child);

  // restore file descriptors
  restore_fds(checkpoint_dir);

  // TODO

  return 0;
}

#include "file.h"

#include <fcntl.h>
#include <stdint.h>
#include <sys/resource.h>
#include <unistd.h>

#include "utils.h"

#define MAX_FILES 128

// file object map
static struct { int kfd, fd; } files[MAX_FILES];

static void restore_files(int dirfd) {
  uint32_t length;
  int obj = openat(dirfd, "file/obj", O_RDONLY);
  read_assert(obj, &length, sizeof(length));
  if (length > MAX_FILES) PANIC("file object array length exceeds MAX_FILES");

  for (size_t i = 0; i < MAX_FILES; i++) {
    if (i < length) {
      uint32_t refcnt;
      read_assert(obj, &files[i].kfd, sizeof(files[i].kfd));
      read_assert(obj, &refcnt, sizeof(refcnt));
      if (!refcnt) {
        files[i].kfd = -1;
      } else {
        files[i].fd = 0;
      }
    } else {
      files[i].kfd = -1;
    }
  }

  close(obj);
}

void restore_fds(const char *checkpoint_dir) {
  // open the checkpoint directory
  int dirfd = open(checkpoint_dir, O_DIRECTORY);
  ASSERT(dirfd >= 0);
  struct rlimit rlim;
  ASSERT(!getrlimit(RLIMIT_NOFILE, &rlim));
  ASSERT(dup2(dirfd, rlim.rlim_cur - 1) == rlim.rlim_cur - 1);
  close(dirfd);
  dirfd = rlim.rlim_cur - 1;

  // restore file objects
  restore_files(dirfd);

  // restore file descriptors
  // TODO

  // close the checkpoint directory
  close(dirfd);
}

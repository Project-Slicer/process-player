#include "file.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "utils.h"
#include "dump.h"

typedef struct {
  int kfd, fd;
} file_t;

static char kfd_dump_path[sizeof("file/kfd/0123456789")];

static file_t *read_files(uint32_t *length) {
  int obj = openr_assert("file/obj");
  read_assert(obj, length, sizeof(*length));
  file_t *files = malloc(*length * sizeof(file_t));

  for (size_t i = 0; i < *length; i++) {
    uint32_t refcnt;
    read_assert(obj, &files[i].kfd, sizeof(files[i].kfd));
    read_assert(obj, &refcnt, sizeof(refcnt));
    if (!refcnt) {
      files[i].kfd = -1;
    } else {
      files[i].fd = -1;
    }
  }

  close_assert(obj);
  return files;
}

static uint32_t *read_fds(uint32_t *length) {
  int fd = openr_assert("file/fd");
  read_assert(fd, length, sizeof(*length));
  uint32_t *fds = malloc(*length * sizeof(uint32_t));
  read_assert(fd, fds, *length * sizeof(uint32_t));
  close_assert(fd);
  return fds;
}

static const char *get_kfd_dump_path(int kfd) {
  int ret = snprintf(kfd_dump_path, sizeof(kfd_dump_path), "file/kfd/%d", kfd);
  ASSERT(ret > 0 && (size_t)ret < sizeof(kfd_dump_path));
  return kfd_dump_path;
}

static void restore_fd(file_t *file, int fd) {
  if (file->fd >= 0) {
    ASSERT(dup2(file->fd, fd) == fd);
  } else {
    // open kfd dump file
    int kfd_dump = openr(get_kfd_dump_path(file->kfd));
    if (kfd_dump < 0) {
      // must be stdin, stdout, or stderr
      ASSERT(file->kfd >= 0 && file->kfd <= 2);
    } else {
      kfd_t data;
      char kfd_path[128];
      read_assert(kfd_dump, &data, sizeof(data));
      read_assert(kfd_dump, kfd_path, data.path_len);
      close_assert(kfd_dump);

      kfd_path[data.path_len] = '\0';
      file->kfd = open(kfd_path, data.flags);
      ASSERT(file->kfd >= 0);
      lseek(file->kfd, data.offset, SEEK_SET);
    }

    // dup2 to fd
    if (file->kfd != fd) {
      ASSERT(dup2(file->kfd, fd) == fd);
      close_assert(file->kfd);
    }

    // update fd
    file->fd = fd;
  }
}

void restore_fds() {
  // read file objects
  uint32_t files_len;
  file_t *files = read_files(&files_len);

  // read file descriptors
  uint32_t fds_len;
  uint32_t *fds = read_fds(&fds_len);

  // restore file descriptors
  for (size_t i = 0; i < fds_len; i++) {
    if (fds[i] == -1) continue;
    uint32_t index = fds[i];
    ASSERT(index < files_len && files[index].kfd >= 0);
    restore_fd(&files[index], (int)i);
  }

  // free memory
  free(files);
  free(fds);
}

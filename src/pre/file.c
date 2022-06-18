#include "pre/file.h"

#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "shared/dump.h"
#include "shared/utils.h"

typedef struct {
  int kfd, fd;
} file_t;

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

static int open_kfd(int kfd) {
  // get the path to the kfd dump file
  char path[sizeof("file/kfd/0123456789")];
  int ret = snprintf(path, sizeof(path), "file/kfd/%d", kfd);
  ASSERT(ret > 0 && (size_t)ret < sizeof(path));

  // open kfd dump file
  int kfd_dump = openr(path);
  if (kfd_dump < 0) {
    // must be stdin, stdout, or stderr
    ASSERT(kfd >= 0 && kfd <= 2);
  } else {
    kfd_t data;
    char kfd_path[128];
    read_assert(kfd_dump, &data, sizeof(data));
    read_assert(kfd_dump, kfd_path, data.path_len);
    close_assert(kfd_dump);

    kfd_path[data.path_len] = '\0';
    kfd = openat_dir(kfd_path, data.flags);
    PANIC_IF(kfd < 0, "failed to open kfd %s", kfd_path);
    lseek(kfd, data.offset, SEEK_SET);
  }
  return kfd;
}

static void restore_fd(file_t *file, int fd) {
  if (file->fd >= 0) {
    ASSERT(dup2(file->fd, fd) == fd);
  } else {
    int kfd = open_kfd(file->kfd);
    if (kfd != fd) {
      ASSERT(dup2(kfd, fd) == fd);
      close_assert(kfd);
    }
    file->fd = fd;
  }
}

int *restore_fds() {
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

  // construct kfd list
  int *kfd_list = malloc((files_len + 1) * sizeof(int));
  kfd_list[0] = (int)files_len;
  for (size_t i = 0; i < files_len; i++) {
    if (files[i].kfd >= 0) {
      if (files[i].fd < 0) {
        kfd_list[i + 1] = open_kfd(files[i].kfd);
      } else {
        int fd = dup(files[i].fd);
        ASSERT(fd >= 0);
        kfd_list[i + 1] = fd;
      }
    } else {
      kfd_list[i + 1] = -1;
    }
  }

  // free memory
  free(files);
  free(fds);

  return kfd_list;
}

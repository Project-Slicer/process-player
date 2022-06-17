#include "post/mem.h"

#include <stdint.h>
#include <sys/mman.h>
#include <unistd.h>

#include "post/uncompress.h"
#include "shared/riscv.h"
#include "shared/utils.h"

// kernel file descriptor list
extern int kfd_list[];

static int pmap_fd;
static size_t bytes_written;
static void *current_page;

static void *map_page(size_t vaddr_type) {
  uintptr_t vaddr = vaddr_type & ~(RISCV_PGSIZE - 1);
  int prot = 0;
  if (vaddr_type & PTE_R) prot |= PROT_READ;
  if (vaddr_type & PTE_W) prot |= PROT_WRITE;
  if (vaddr_type & PTE_X) prot |= PROT_EXEC;
  void *page =
      mmap(vaddr, RISCV_PGSIZE, prot,
           MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED | MAP_POPULATE, -1, 0);
  PANIC_IF((uintptr_t)page != vaddr, "failed to map page");
  return page;
}

static void write_page(uint8_t byte) {
  if (bytes_written == 0) {
    size_t vaddr_type;
    read_assert(pmap_fd, &vaddr_type, sizeof(vaddr_type));
    current_page = map_page(vaddr_type);
  }
  ((uint8_t *)current_page)[bytes_written++] = byte;
  if (bytes_written == RISCV_PGSIZE) bytes_written = 0;
}

static void restore_pages() {
  pmap_fd = openr_assert("mem/pmap");
  int page_fd = openr_assert("mem/page");

  // check if the page dump was compressed
  uint8_t compressed;
  read_assert(page_fd, &compressed, sizeof(compressed));
  if (compressed) {
    bytes_written = 0;
    PANIC_IF(uncompress(page_fd, write_page), "failed to uncompress page dump");
  } else {
    ssize_t n;
    size_t vaddr_type;
    while ((n = read(pmap_fd, &vaddr_type, sizeof(vaddr_type))) ==
           sizeof(vaddr_type)) {
      void *page = map_page(vaddr_type);
      read_assert(page_fd, page, RISCV_PGSIZE);
    }
    PANIC_IF(n != 0, "failed to read physical memory map");
  }

  close_assert(pmap_fd);
  close_assert(page_fd);
}

void restore_memory() {
  restore_pages();
  // TODO
}

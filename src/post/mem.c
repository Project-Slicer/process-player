#include "post/mem.h"

#include <stdint.h>

#include "post/tinylibc.h"
#include "post/uncompress.h"
#include "shared/dump.h"
#include "shared/riscv.h"
#include "shared/utils.h"

static int pmap_fd;
static size_t bytes_written;
static void *current_page;

#define MAX_VMRS 128
static vmr_data_t vmrs[MAX_VMRS];
static size_t vmrs_count;

static inline int *get_kfd_list() {
  int *kfds;
  asm volatile("lla %0, kfd_list\n\t" : "=r"(kfds));
  return kfds;
}

static void *map_page(size_t vaddr_type) {
  uintptr_t vaddr = vaddr_type & ~(RISCV_PGSIZE - 1);
  int prot = PROT_WRITE;
  if (vaddr_type & PTE_R) prot |= PROT_READ;
  if (vaddr_type & PTE_X) prot |= PROT_EXEC;

  void *page =
      mmap((void *)vaddr, RISCV_PGSIZE, prot,
           MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED | MAP_POPULATE, -1, 0);
  PANIC_IF((uintptr_t)page != vaddr, "failed to map page");
  DBG("mapped page %p", page);
  return page;
}

static void map_vmr(vmap_record_t *record, size_t len) {
  PANIC_IF(record->id >= vmrs_count, "invalid VMR object index");
  vmr_data_t *vmr = &vmrs[record->id];

  int flags = MAP_PRIVATE | MAP_FIXED;
  if (vmr->file == -1) flags |= MAP_ANONYMOUS;

  void *page = mmap((void *)record->vaddr, len, vmr->prot, flags, vmr->file,
                    record->vaddr - vmr->addr + vmr->offset);
  PANIC_IF((uintptr_t)page != record->vaddr, "failed to map VMR object");
  DBG("mapped VMR %d at %p, len = %d", record->id, page, (int)len);
}

static void write_page(uint8_t byte) {
  if (bytes_written == 0) {
    size_t vaddr_type;
    read_assert(pmap_fd, &vaddr_type, sizeof(vaddr_type));
    current_page = map_page(vaddr_type);
  }
  ((uint8_t *)current_page)[bytes_written++] = byte;
  if (bytes_written == RISCV_PGSIZE) {
    DBG("wrote page at %p", current_page);
    bytes_written = 0;
  }
}

static void restore_pages() {
  pmap_fd = openr_assert("mem/pmap");
  int page_fd = openr_assert("mem/page");
  DBG("pmap_fd = %d, page_fd = %d", pmap_fd, page_fd);

  // check if the page dump was compressed
  uint8_t compressed;
  read_assert(page_fd, &compressed, sizeof(compressed));
  DBG("compressed = %d", (int)compressed);
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
      DBG("restored page at %p", page);
    }
    PANIC_IF(n != 0, "failed to read physical memory map");
  }

  close_assert(pmap_fd);
  close_assert(page_fd);
}

static void restore_vmrs() {
  int vmr_fd = openr_assert("mem/vmr");
  int *kfds = get_kfd_list();

  for (vmrs_count = 0;; vmrs_count++) {
    size_t i = vmrs_count;
    ssize_t n = read(vmr_fd, &vmrs[i], sizeof(vmr_data_t));
    PANIC_IF(n < 0, "failed to read VMR object");
    if (n == 0) break;

    if (vmrs[i].file != -1) {
      PANIC_IF(vmrs[i].file >= kfds[0], "invalid file object index");
      vmrs[i].file = kfds[vmrs[i].file + 1];
    }
  }

  close_assert(vmr_fd);
}

static void restore_vmr_map() {
  int vmap_fd = openr_assert("mem/vmap");

  vmap_record_t last_record = {.vaddr = 0};
  size_t last_len = 0;
  for (;;) {
    vmap_record_t record;
    ssize_t n = read(vmap_fd, &record, sizeof(record));
    PANIC_IF(n < 0, "failed to read VMR mapping");
    if (n == 0) break;

    if (record.vaddr == last_record.vaddr + last_len &&
        record.id == last_record.id) {
      last_len += RISCV_PGSIZE;
    } else if (last_len > 0) {
      map_vmr(&last_record, last_len);
      last_len = 0;
    } else {
      last_record = record;
      last_len = RISCV_PGSIZE;
    }
  }

  if (last_len > 0) map_vmr(&last_record, last_len);
  close_assert(vmap_fd);
}

void restore_memory() {
  DBG("restoring pages...");
  restore_pages();
  DBG("restoring VMRs...");
  restore_vmrs();
  DBG("restoring VMR mapping...");
  restore_vmr_map();

  // close kernel file descriptors
  int *kfds = get_kfd_list();
  for (int i = 1; i <= kfds[0]; i++) {
    if (kfds[i] >= 0) close_assert(kfds[i]);
  }
}

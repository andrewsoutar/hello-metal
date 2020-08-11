#include <stddef.h>
#include <stdnoreturn.h>

#include "bootloader.h"
#include "hlt.h"
#include "multiboot.h"
#include "term.h"

static void real_main(uint32_t magic, uint32_t info_) {
  struct bootloader *bootloader = NULL;
  if (magic == MULTIBOOT2_BOOTLOADER_MAGIC)
    bootloader = &multiboot_bootloader;
  if (bootloader == NULL)
    return;

  if (bootloader->early_init(info_) != 0 ||
      bootloader->term_init() != 0)
    return;
  if (bootloader->physmem_init() != 0) {
    term_print("Unable to initialize physical memory manager, bailing out\n");
    return;
  }

  if (bootloader->init() == 0)    
    /* Hello, metal! */
    term_print("Hello, bare metal!\n");
}

noreturn void main(uint32_t magic, uint32_t info_) {
  real_main(magic, info_);
  for (;;) hlt();
}

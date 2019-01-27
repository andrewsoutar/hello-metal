#include "hlt.h"
#include "multiboot.h"
#include "term.h"

void main(uint32_t multiboot_magic, struct multiboot_info *info) {
  if (multiboot_magic != MBOOT_REG_MAGIC)
    return;

  if (!(info->flags & MBOOT_INFO_FRAMEBUFFER) ||
      info->framebuffer.type != 2)
    goto err;

  term_init((void *) (uintptr_t) info->framebuffer.addr);

  /* Hello, metal! */
  term_print("Hello, bare metal!\n");

 err:
  for (;;) hlt();
}

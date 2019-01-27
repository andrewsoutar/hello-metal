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

  if (info->flags & MBOOT_INFO_MODS) {
    uint32_t n_mods = info->mods.count;
    term_print("Have ");
    term_print_u32(n_mods);
    term_print(" modules\n");
    const struct multiboot_mod *mods = (const void *) (uintptr_t) info->mods.addr;
    for (uint32_t i = 0; i < n_mods; ++i) {
      term_print("Module ");
      term_print_u32(i);
      term_print(": ");
      term_print((const char *) (uintptr_t) mods[i].string);
      term_print(" (");
      term_print_u32(mods[i].start);
      term_print(":");
      term_print_u32(mods[i].end);
      term_print(")\n");
      for (uint32_t p = mods[i].start; p < mods[i].end; ++p) {
        term_put_char(*(const char *) (uintptr_t) p);
      }
      term_print("\n");
    }
  } else {
    term_print("No modules found\n");
  }

 err:
  for (;;) hlt();
}

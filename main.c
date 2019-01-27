#include "hlt.h"
#include "multiboot.h"
#include "term.h"

void main(uint32_t multiboot_magic, struct multiboot_data *multiboot_data) {
  if (multiboot_magic != MBOOT_REG_MAGIC)
    return;

  term_clr();

  /* Hello, metal! */
  term_print("Hello, bare metal!\n");

  for (;;) hlt();
}

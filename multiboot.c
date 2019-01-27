#include "multiboot.h"

/* The multiboot header for the binary */
const struct multiboot_header multiboot_header
/*
 * The header must go in the ".multiboot" section so that the linker
 * script puts it at the beginning of the kernel file
 */
__attribute__((section(".multiboot"))) =
{
  .magic = MBOOT_MAGIC,

  /* Ask the bootloader to give us a memory map */
  .flags = MBOOT_MEM | MBOOT_GFX,

  /* Calculate the checksum */
  .checksum = MBOOT_SUM(MBOOT_MEM | MBOOT_GFX),

  /* Text mode */
  .mode_type = 1,
  .width = 80,
  .height = 25,
  .depth = 0

  /*
   * We do not need to populate the other fields because we did not
   * enable the flags that require them
   */
};

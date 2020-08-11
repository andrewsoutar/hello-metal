#ifndef HELLO_METAL_BOOTLOADER_H
#define HELLO_METAL_BOOTLOADER_H

#include <stdint.h>

struct bootloader {
  int (*early_init)(uint32_t);
  int (*term_init)();
  int (*physmem_init)();
  int (*init)();
};

extern struct bootloader multiboot_bootloader;

#endif

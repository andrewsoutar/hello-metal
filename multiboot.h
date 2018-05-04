#ifndef HELLO_METAL_MULTIBOOT_H__
#define HELLO_METAL_MULTIBOOT_H__

#include <stddef.h>
#include <stdint.h>

/*
 * This magic number must be in the "magic" field of the multiboot
 * header
 */
#define MBOOT_MAGIC (0x1BADB002u)

/*
 * This magic number will be in the register eax when the kernel is
 * booted using multiboot
 */
#define MBOOT_REG_MAGIC (0x2BADB002u)

/*
 * The layout of the multiboot header - see Multiboot Specification,
 * version 0.6.96
 */
struct multiboot_header {
  uint32_t magic;
  uint32_t flags;
  uint32_t checksum;
  uint32_t header_addr;
  uint32_t load_addr;
  uint32_t load_end_addr;
  uint32_t bss_end_addr;
  uint32_t entry_addr;
  uint32_t mode_type;
  uint32_t width;
  uint32_t height;
  uint32_t depth;
} __attribute__ ((packed));

/* These flags are for the "flags" field of the multiboot header */
#define MBOOT_ALIGN (1 << 0)
#define MBOOT_MEM (1 << 1)
#define MBOOT_GFX (1 << 2)
#define MBOOT_ADDR (1 << 16)

/*
 * Automatically calculate the multiboot checksum field for the
 * specified flags
 */
#define MBOOT_SUM(FLAGS) (-(MBOOT_MAGIC + FLAGS))

#endif /* HELLO_METAL_MULTIBOOT_H */

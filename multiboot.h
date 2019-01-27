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
#define MBOOT_SUM(FLAGS) (-(MBOOT_MAGIC + (FLAGS)))

/* The multiboot boot information structure */
struct multiboot_info {
  uint32_t flags;
#define MBOOT_INFO_MEM (1u << 0)
#define MBOOT_INFO_BOOT_DEVICE      (1u << 1)
#define MBOOT_INFO_CMDLINE          (1u << 2)
#define MBOOT_INFO_MODS             (1u << 3)
#define MBOOT_INFO_A_OUT            (1u << 4)
#define MBOOT_INFO_ELF              (1u << 5)
#define MBOOT_INFO_MMAP             (1u << 6)
#define MBOOT_INFO_DRIVES           (1u << 7)
#define MBOOT_INFO_CONFIG_TABLE     (1u << 8)
#define MBOOT_INFO_BOOT_LOADER_NAME (1u << 9)
#define MBOOT_INFO_APM_TABLE        (1u << 10)
#define MBOOT_INFO_VBE              (1u << 11)
#define MBOOT_INFO_FRAMEBUFFER      (1u << 12)
  struct { uint32_t lower, upper; } mem;
  uint32_t boot_device;
  uint32_t cmdline;
  struct { uint32_t count, addr; } mods;
  union {
    struct { uint32_t tabsize, strsize, addr, rsvd; } a_out;
    struct { uint32_t num, size, addr, shndx; } elf;
  };
  struct { uint32_t length, addr; } mmap, drives;
  uint32_t config_table;
  uint32_t boot_loader_name;
  uint32_t apm_table;
  struct {
    uint32_t control_info, mode_info;
    uint16_t mode, interface_seg, interface_off, interface_len;
  } vbe;
  struct {
    uint64_t addr;
    uint32_t pitch, width, height;
    uint8_t bpp, type, color_info[6];
  } framebuffer;
} __attribute__((packed));

struct multiboot_mod {
  uint32_t start, end, string, rsvd;
};

#endif /* HELLO_METAL_MULTIBOOT_H */

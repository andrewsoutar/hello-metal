#ifndef HELLO_METAL_MULTIBOOT_H__
#define HELLO_METAL_MULTIBOOT_H__

#include <stddef.h>
#include <stdint.h>

#define MULTIBOOT2_BOOTLOADER_MAGIC 0x36d76289

struct multiboot2_info {
  uint32_t total_size;
  uint32_t reserved;
  char tags[];
} __attribute__((packed));

struct multiboot2_info_tag {
  uint32_t type;
  uint32_t size;
} __attribute__((packed));

#define MULTIBOOT2_INFO_END 0

#define MULTIBOOT2_INFO_CMDLINE 1
struct multiboot2_info_cmdline {
  struct multiboot2_info_tag tag;
  char string[];
} __attribute__((packed));

#define MULTIBOOT2_INFO_MODULE 3
struct multiboot2_info_module {
  struct multiboot2_info_tag tag;
  uint32_t start;
  uint32_t end;
  char string[];
} __attribute__((packed));

#define MULTIBOOT2_INFO_FRAMEBUFFER 8
struct multiboot2_info_framebuffer {
  struct multiboot2_info_tag tag;
  uint64_t addr;
  uint32_t pitch, width, height;
  uint8_t bpp;

  uint8_t type;
#define MULTIBOOT2_FRAMEBUFFER_INDEXED_COLOR 0
#define MULTIBOOT2_FRAMEBUFFER_RGB 1
#define MULTIBOOT2_FRAMEBUFFER_TEXT 2

  uint8_t reserved;
  char color_info[];
} __attribute__((packed));

#define MULTIBOOT2_INFO_ACPI_OLD 14
struct multiboot2_info_acpi_old {
  struct multiboot2_info_tag tag;
  char rsdp[];
} __attribute__((packed));

#define MULTIBOOT2_INFO_ACPI_NEW 15
struct multiboot2_info_acpi_new {
  struct multiboot2_info_tag tag;
  char rsdp[];
} __attribute__((packed));

#endif /* HELLO_METAL_MULTIBOOT_H */

  .section ".multiboot"

  .align 8
multiboot_header:
  # multiboot2 magic value
#define MAGIC 0xE85250D6
  # 0 = x86, 4 = 32-bit MIPS
#define ARCH 0
#define LENGTH (multiboot_end - multiboot_header)
#define CHECKSUM (-(MAGIC + ARCH + LENGTH))
  .long MAGIC
  .long ARCH
  .long LENGTH
  .long CHECKSUM

#define TAG(type, flags)  \
0:                       ;\
  .align 8               ;\
0:                       ;\
  .word type             ;\
  .word flags            ;\
  .long (0f - 0b)

#define TAG_OPTIONAL 0
#define TAG_REQUIRED 1

  TAG(1, TAG_REQUIRED)          # Information request
  .long 1                       # Command-line string
  .long 3                       # Modules
  .long 6                       # Memory map
  .long 8                       # Framebuffer
  .long 15                      # ACPI RSDP v2

  TAG(4, TAG_REQUIRED)          # Console flags
  .long 0x3                     # Bit 0 = console required, bit 1 = text mode

  TAG(6, TAG_REQUIRED)          # Modules must be page-aligned

  TAG(0, 0)                     # End of tags
0:
multiboot_end:

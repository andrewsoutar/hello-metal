#include "gdt.h"

#include "interrupt.h"

void lgdt(const struct gdt *gdt, size_t int_masked) {
  if (!int_masked)
    cli();
  asm volatile("lgdt (%[gdt])" :: [gdt] "r" (gdt));
  if (!int_masked)
    sti();
}

void lgdt_and_jmp(const struct gdt *gdt, size_t int_masked, uint16_t cs) {
  lgdt(gdt, int_masked);

  /*
   * The only way to switch to a non-compile-time-constant code segment
   * is via a long return. Here, we populate the stack with a long
   * return address, which has a 32-bit address followed by a 32-bit
   * code segment. We set the address to be the next instruction after
   * the return, so that we don't actually jump to new code.
   */
  asm volatile("pushl %[cs]\n\t"
               "pushl $0f\n\t"
               "lret\n\t"
               "0:"
               :: [cs] "irm" ((uint32_t) cs));
}


/* A flat 32-bit-mode global descriptor table */
static const struct gdt_entry gdt_entries[] = {
  {0},                          /* First segment is unused */
  {                             /* 32-bit code segment */
    .limit_low = 0xFFFFu,
    .base_low  = 0x0000u,
    .base_mid  = 0x00u,
    .access    = GDT_RW | GDT_DC | GDT_EX | GDT_S | GDT_R0 | GDT_P,
    .limit_high_flags = 0xFu | GDT_DB | GDT_G,
    .base_high = 0x00u
  },
  {                             /* 32-bit data segment */
    .limit_low = 0xFFFFu,
    .base_low  = 0x0000u,
    .base_mid  = 0x00u,
    .access    = GDT_RW | GDT_S | GDT_R0 | GDT_P,
    .limit_high_flags = 0xFu | GDT_DB | GDT_G,
    .base_high = 0x00u
  },
  {                             /* 16-bit code segment */
    .limit_low = 0xFFFFu,
    .base_low  = 0x0000u,
    .base_mid  = 0x00u,
    .access    = GDT_RW | GDT_DC | GDT_EX | GDT_S | GDT_R0 | GDT_P,
    .limit_high_flags = 0xFu,
    .base_high = 0x00u
  },
  {                             /* 16-bit data segment */
    .limit_low = 0xFFFFu,
    .base_low  = 0x0000u,
    .base_mid  = 0x00u,
    .access    = GDT_RW | GDT_S | GDT_R0 | GDT_P,
    .limit_high_flags = 0xFu,
    .base_high = 0x00u
  }
};
static const struct gdt gdt = {sizeof(gdt_entries)-1, (uint32_t) (void const *) gdt_entries};

static const struct gdt_entry low_gdt_entries[]
__attribute__((section(".low.rodata"))) = {
  {0},                          /* First segment is unused */
  {                             /* 32-bit code segment */
    .limit_low = 0xFFFFu,
    .base_low  = 0x0000u,
    .base_mid  = 0x00u,
    .access    = GDT_RW | GDT_DC | GDT_EX | GDT_S | GDT_R0 | GDT_P,
    .limit_high_flags = 0xFu | GDT_DB | GDT_G,
    .base_high = 0x00u
  },
  {                             /* 32-bit data segment */
    .limit_low = 0xFFFFu,
    .base_low  = 0x0000u,
    .base_mid  = 0x00u,
    .access    = GDT_RW | GDT_S | GDT_R0 | GDT_P,
    .limit_high_flags = 0xFu | GDT_DB | GDT_G,
    .base_high = 0x00u
  },
  {                             /* 16-bit code segment */
    .limit_low = 0xFFFFu,
    .base_low  = 0x0000u,
    .base_mid  = 0x00u,
    .access    = GDT_RW | GDT_DC | GDT_EX | GDT_S | GDT_R0 | GDT_P,
    .limit_high_flags = 0xFu,
    .base_high = 0x00u
  },
  {                             /* 16-bit data segment */
    .limit_low = 0xFFFFu,
    .base_low  = 0x0000u,
    .base_mid  = 0x00u,
    .access    = GDT_RW | GDT_S | GDT_R0 | GDT_P,
    .limit_high_flags = 0xFu,
    .base_high = 0x00u
  }
};
extern const struct gdt low_gdt __attribute__((section(".low.rodata")));
const struct gdt low_gdt = {sizeof(low_gdt_entries)-1, (uint32_t) (void const *) low_gdt_entries};

void gdt_init(void) {
  lgdt_and_jmp(&gdt, 1, 0x08u);
}

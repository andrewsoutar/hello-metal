#include "gdt.h"

/* A flat 64-bit-mode global descriptor table */
static const struct gdt_entry gdt_entries[] = {
  {0},                          /* Null segment */
  {                             /* 64-bit code segment */
    .limit_low = 0xFFFFu,
    .base_low  = 0x0000u,
    .base_mid  = 0x00u,
    .access    = GDT_RW | GDT_DC | GDT_EX | GDT_S | GDT_R0 | GDT_P,
    .limit_high_flags = 0xFu | GDT_L | GDT_G,
    .base_high = 0x00u
  },
  {                             /* 32/64-bit data segment */
    .limit_low = 0xFFFFu,
    .base_low  = 0x0000u,
    .base_mid  = 0x00u,
    .access    = GDT_RW | GDT_S | GDT_R0 | GDT_P,
    .limit_high_flags = 0xFu | GDT_DB | GDT_G,
    .base_high = 0x00u
  }
};
const struct gdt gdt = { sizeof(gdt_entries)-1, (uintptr_t) gdt_entries };

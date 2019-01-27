#ifndef HELLO_METAL_GDT_H
#define HELLO_METAL_GDT_H

#include <stddef.h>
#include <stdint.h>

/* Access flags */
#define GDT_AC (1 << 0) /* Accessed */
#define GDT_RW (1 << 1) /* Readable (for code)/Writable (for data) */
#define GDT_DC (1 << 2) /* Direction (for data)/Conforming (for code) */
#define GDT_EX (1 << 3) /* Executable */
#define GDT_S  (1 << 4) /* 0 = system, 1 = code/data */
#define GDT_R0 (0 << 5) /* Ring 0 */
#define GDT_R1 (1 << 5) /* Ring 1 */
#define GDT_R2 (2 << 5) /* Ring 2 */
#define GDT_R3 (3 << 5) /* Ring 3 */
#define GDT_P  (1 << 7) /* Present */

/* Other flags */
#define GDT_AVL (1 << 4)      /* Available for use by system software */
#define GDT_L  (1 << 5)       /* Long (64-bit) mode */
#define GDT_DB (1 << 6)       /* 0=16 bit, 1=32 bit */
#define GDT_G  (1 << 7)       /* Granularity (0=byte, 1=page) */

struct gdt_entry {
  uint16_t limit_low;
  uint16_t base_low;
  uint8_t base_mid;
  uint8_t access;
  uint8_t limit_high_flags;
  uint8_t base_high;
} __attribute__((packed));

struct gdt {
  uint16_t size;
  uint64_t entries;
} __attribute__((packed));

#endif /* HELLO_METAL_GDT_H */

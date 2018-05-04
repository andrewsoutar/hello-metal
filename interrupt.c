#include "interrupt.h"

#include <stddef.h>
#include <stdint.h>

/* #include "apic.h" */
#include "cpuio.h"
#include "pic.h"
#include "string.h"

typedef char interrupt_stub_t[6]; /* Indirect call is always 6 bytes on x86 */
extern interrupt_stub_t handler_stub;

static void(*interrupt_handlers[256])(uint32_t,uint32_t);
static interrupt_stub_t interrupt_stubs[256];

static struct idt_entry {
  uint16_t offs_low;
  uint16_t cs;
  uint8_t zero;
  uint8_t flags;
#define TASK32 (0x5)
#define INT16  (0x6)
#define TRAP16 (0x7)
#define INT32  (0xE)
#define TRAP32 (0xF)
#define IDT_S  (1u<<4)
#define IDT_R0 (0u<<5)
#define IDT_R1 (1u<<5)
#define IDT_R2 (2u<<5)
#define IDT_R3 (3u<<5)
#define IDT_P  (1u<<7)
  uint16_t offs_hi;
} __attribute__((packed)) idt_entries[256] = {{0}};

static const struct idt {
  uint16_t limit;
  uint32_t base;
} __attribute__((packed)) idt = {
  .limit = sizeof(idt_entries)-1,
  .base = (uint32_t) (void const *) idt_entries
};

extern const struct idt real_mode_idt __attribute__((section(".low.rodata")));
const struct idt real_mode_idt = {
  .limit = 0x03FF,
  .base  = 0x00000000
};

void cli(void) {
  asm volatile("cli");
}

void sti(void) {
  asm volatile("sti");
}

static int using_apic;
void interrupt_init(void) {
  /* if ((using_apic = apic_present())) */
  /*   apic_init(); */
  /* else */
  /*   pic_init(); */
  using_apic = 0;
  pic_init();
  for (size_t i = 0; i < 256; ++i) {
    interrupt_handlers[i] = NULL;
    memcpy(interrupt_stubs[i], &handler_stub, sizeof(handler_stub));
    idt_entries[i].offs_low = (uint16_t) ((uintptr_t) (void const *) interrupt_stubs[i] & 0xFFFF);
    idt_entries[i].cs = 0x08;
    idt_entries[i].zero = 0;
    idt_entries[i].flags = INT32 | IDT_R0 | IDT_P;
    idt_entries[i].offs_hi = (uint16_t) ((uintptr_t) (void const *) interrupt_stubs[i] >> 16);
  }

  asm volatile("lidt (%[idt])" :: [idt] "r" (&idt));

  sti();
}

void eoi(uint8_t irq) {
  /* if (using_apic) */
  /*   apic_eoi(); */
  /* else */
  pic_eoi(irq);
}

void add_handler(uint32_t interrupt, void (*handler)(uint32_t, uint32_t)) {
  interrupt_handlers[interrupt] = handler;
}

int interrupt_handler_c(uint32_t, uint32_t);
int interrupt_handler_c(uint32_t stub_addr, uint32_t err) {
  uint8_t interrupt = ((interrupt_stub_t const *) stub_addr - interrupt_stubs) - 1;
  if (interrupt_handlers[interrupt])
    interrupt_handlers[interrupt](interrupt, err);
  if (interrupt < 0x20)
    return 1;
  eoi(interrupt);
  return 0;
}

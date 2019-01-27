#include "term.h"

#include "cpuio.h"
#include "gdt.h"
#include "string.h"

/* These variables represent symbols populated by the linker script */
extern char _low_start, _low_end, _low_addr;

static size_t x, y;
static uint8_t color;
static volatile uint16_t *buf;

static void put_char_at(char chr, uint8_t color, size_t x, size_t y) {
  buf[y * 80 + x] = (uint16_t) chr | ((uint16_t) color << 8);
}

void term_clr() {
  x = 0; y = 0;
  color = 0x0F;
  for (size_t i = 0; i < 25 * 80; ++i)
    term_put_char(' ');
}

void term_init(void) {
  /* Copy the real-mode init code into place */
  memcpy(&_low_addr, &_low_start, &_low_end - &_low_start);

  /* Switch into 16-bit protected mode and call the copied code */
  asm volatile("lcall $0x18, $_asm_term_init" ::: "memory", "cc");
  /* Put the real GDT back */
  gdt_init();

  buf = (uint16_t *) 0x000B8000;

  term_clr();

  /* Turn on the cursor */
  outb(0x3D4, 0x0A);
  outb(0x3D5, (inb(0x3D5) & 0xC0) | 14);
  outb(0x3D4, 0x0B);
  outb(0x3D5, (inb(0x3D5) & 0xE0) | 15);
}

void term_put_char(char chr) {
  if (chr == '\n') {
    ++y;
    x = 0;
  } else if (chr == '\b') {
    if (x) x--;
    put_char_at(' ', color, x, y);
  } else {
    put_char_at(chr, color, x, y);
    ++x;
  }
  if (x == 80) {
    x = 0;
    ++y;
  }
  if (y == 25)
    y = 0;

  uint16_t curs = y * 80 + x;
  outb(0x3D4, 0x0E);
  outb(0x3D5, curs >> 8);
  outb(0x3D4, 0x0F);
  outb(0x3D5, curs & 0xFF);
}

void term_move_x(int offs) {
  x += offs;
  if (x >= 80) {
    y += x / 80;
    x %= 80;
    if (y >= 25)
      y %= 25;
  }

  uint16_t curs = y * 80 + x;
  outb(0x3D4, 0x0E);
  outb(0x3D5, curs >> 8);
  outb(0x3D4, 0x0F);
  outb(0x3D5, curs & 0xFF);
}

void term_print(char const *str) {
  while (*str != '\0')
    term_put_char(*str++);
}

void term_print_u32(uint32_t n) {
  for (size_t i = 0; i < 8; ++i) {
    unsigned char num = (n << (4 * i)) >> 28;
    if (num < 0xA) {
      term_put_char(num + '0');
    } else {
      term_put_char(num - 0xA + 'A');
    }
  }
}

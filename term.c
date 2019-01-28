#include "term.h"

#include <stddef.h>

#include "cpuio.h"

static size_t x, y;
static uint8_t color;
static volatile uint16_t *buf;

void term_init(volatile uint16_t *buf_) {
  buf = buf_;
  term_clr();
}

static void put_char_at(char chr, uint8_t color, size_t x, size_t y) {
  buf[y * 80 + x] = (uint16_t) chr | ((uint16_t) color << 8);
}

void term_clr() {
  x = 0; y = 0;
  color = 0x0F;
  for (size_t i = 0; i < 25 * 80; ++i)
    term_put_char(' ');
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

void term_print_num(unsigned long long n) {
  unsigned long long copy = n, pos = 1;
  while (copy /= 10) pos *= 10;
  do term_put_char('0' + (n / pos) % 10); while (pos /= 10);
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

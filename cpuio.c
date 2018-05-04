#include "cpuio.h"

void io_wait(void) {
  outb(0x80, 0);
}

void outb(uint16_t port, uint8_t data) {
  asm volatile("outb %[data], %[port]" :: [data] "a"(data), [port] "Nd"(port));
}
uint8_t inb(uint16_t port) {
  uint8_t data;
  asm volatile("inb %[port], %[data]" : [data] "=a"(data) : [port] "Nd"(port));
  return data;
}

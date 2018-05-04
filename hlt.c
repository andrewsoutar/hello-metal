#include "hlt.h"

void hlt(void) {
  asm volatile("hlt");
}

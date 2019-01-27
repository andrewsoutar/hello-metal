#include "msr.h"

uint64_t rdmsr(uint32_t msr) {
  uint32_t edx, eax;
  asm("rdmsr" : "=d"(edx), "=a"(eax) : "c"(msr));
  return ((uint64_t) edx << 32) | eax;
}

void wrmsr(uint32_t msr, uint64_t value) {
  asm volatile("wrmsr"
               :
               : "c"(msr),
                 "d"((uint32_t) (value >> 32)),
                 "a"((uint32_t) (value & 0xFFFFFFFF)));
}

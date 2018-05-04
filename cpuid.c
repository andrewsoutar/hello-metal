#include "cpuid.h"

int cpuid(struct cpuid *out, uint32_t leaf, uint32_t subleaf) {
  if (leaf > 0 && leaf <= cpuid_leaf_max()) {
    asm("cpuid"
        : "=a"(leaf), "=c"(subleaf)
        : "a"(out->eax), "b"(out->ebx), "c"(out->ecx), "d"(out->edx));
    return 0;
  } else {
    return -1;
  }
}

uint32_t cpuid_leaf_max(void) {
  static int found = 0;
  static uint32_t leaf_max;
  if (!found) {
    struct cpuid cpuid_;
    cpuid(&cpuid_, 0, 0);
    leaf_max = cpuid_.eax;
    found = 1;
  }
  return leaf_max;
}

#include "vmx.h"

#include "cpuid.h"

int has_vmx() {
  struct cpuid cpuid_;
  return (cpuid(&cpuid_, 1, 0) == 0) && (cpuid_.ecx & (1 << 5));
}

void vmxon() {
  uint64_t tmp;
  asm volatile("movq %%cr4, %[tmp]\n\t"
               "orq $(1 << 13), %[tmp]\n\t"
               "movq %[tmp], %%cr4\n\t"
               "vmxon"
               : [tmp]  "=r" (tmp));
}

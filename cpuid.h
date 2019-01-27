#ifndef HELLO_METAL_CPUID_H
#define HELLO_METAL_CPUID_H

#include <stdint.h>

struct cpuid {uint32_t eax,ebx,edx,ecx;};
int cpuid(struct cpuid *out, uint32_t leaf, uint32_t subleaf);
uint32_t cpuid_leaf_max(void);
uint32_t cpuid_ext_leaf_max(void);

/* CPUID.01H:EDX */
#define CPUID_APIC (1u<<9)

/* CPUID.80000001H:EDX */
#define CPUID_EM64T (1u<<29)

#endif /* HELLO_METAL_CPUID_H */

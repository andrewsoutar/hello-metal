#ifndef HELLO_METAL_MSR_H
#define HELLO_METAL_MSR_H

#include <stdint.h>

uint64_t rdmsr(uint32_t);
void wrmsr(uint32_t, uint64_t);

#endif

#ifndef HELLO_METAL_STRING_H
#define HELLO_METAL_STRING_H

#include <stddef.h>

#define memcpy __builtin_memcpy
void *memcpy(void *restrict s1, void const *restrict s2, size_t n);

#define memcmp __builtin_memcmp
int memcmp(void const *s1, void const *s2, size_t n);

#endif /* HELLO_METAL_STRING_H */

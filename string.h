#ifndef HELLO_METAL_STRING_H
#define HELLO_METAL_STRING_H

#include <stddef.h>

int memcmp(const void *, const void *, size_t);
#define memcmp __builtin_memcmp

void *memset(void *, int, size_t);
#define memset __builtin_memset

void *memcpy(void *restrict, const void *restrict, size_t);
#define memcpy __builtin_memcpy

void *memmove(void *, const void *, size_t);
#define memmove __builtin_memmove

#endif /* HELLO_METAL_STRING_H */

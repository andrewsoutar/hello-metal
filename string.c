#include "string.h"

void *memcpy(void *restrict s1, void const *restrict s2, size_t n) {
  while (n--) ((unsigned char *) s1)[n] = ((unsigned char const *) s2)[n];
}

int memcmp(void const *s1_, void const *s2_, size_t n) {
  unsigned char const *s1 = s1_, *s2 = s2_;
  while (n--) if (*s1 != *s2) return *s1 - *s2;
  return 0;
}

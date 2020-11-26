#include "string.h"

#undef memcmp
int memcmp(const void *s1_, const void *s2_, size_t n_) {
  register const void *s1 asm("rsi") = s1_;
  register const void *s2 asm("rdi") = s2_;
  register const size_t n asm("rcx") = n_;
  signed char ret;
  asm("repz cmpsb; setab %[ret]; sbbb $0, %[ret]"
      : [ret] "=r" (ret)
      : "r" (s1), "r" (s2), "r" (n) : "cc");
  return (signed int) ret;
}

#undef memset
void *memset(void *s_, int c_, size_t n_) {
  register const void *s asm("rdi") = s_;
  register unsigned char c asm("al") = (unsigned char) c_;
  register size_t n asm("rcx") = n_;
  asm volatile("rep stosb" : "+r" (s), "+r" (n) : "r" (c) : "memory");
  return s_;
}

#undef memcpy
void *memcpy(void *restrict dest_, const void *restrict src_, size_t n_) {
  register void *dest asm("rdi") = dest_;
  register const void *src asm("rsi") = src_;
  register size_t n asm("rcx") = n_;
  asm volatile("rep movsb" : "+r" (dest), "+r" (src), "+r" (n) :: "memory");
  return dest_;
}

#undef memmove
void *memmove(void *dest_, const void *src_, size_t n_) {
  register void *dest asm("rdi") = dest_;
  register const void *src asm("rsi") = src_;
  register size_t n asm("rcx") = n_;
  if ((uintptr_t) dest_ - (uintptr_t) src_ < n_) {
    dest += n_;
    src += n_;
    asm volatile("std; rep movsb; cld" : "+r" (dest), "+r" (src), "+r" (n) :: "memory");
  } else {
    asm volatile("rep movsb" : "+r" (dest), "+r" (src), "+r" (n) :: "memory");
  }
  return dest_;
}

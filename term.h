#ifndef HELLO_METAL_TERM_H
#define HELLO_METAL_TERM_H

#include <stddef.h>
#include <stdint.h>

void term_init(volatile uint16_t *, uint32_t, uint32_t);
void term_put_char(char);
void term_move_x(int);
void term_clr(void);
void term_print(char const *);
void term_printn(char const *, size_t);
void term_print_num(unsigned long long);
void term_print_u32(uint32_t);

#endif /* HELLO_METAL_TERM_H */

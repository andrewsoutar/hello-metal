#ifndef HELLO_METAL_TERM_H
#define HELLO_METAL_TERM_H

void term_init(void);
void term_put_char(char);
void term_move_x(int);
void term_clr(void);
void term_print(char const *);

#endif /* HELLO_METAL_TERM_H */

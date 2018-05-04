#include "gdt.h"
#include "interrupt.h"
#include "kbd.h"
#include "multiboot.h"
#include "term.h"

void main(uint32_t multiboot_magic, struct multiboot_data *multiboot_data) {
  if (multiboot_magic != MBOOT_REG_MAGIC)
    return;
  /* Set up a basic flat, 32-bit global descriptor table */
  gdt_init();

  /* Put the screen into the proper mode so that I can write to it */
  term_init();

  /* Hello, metal! */
  term_print("Hello, bare metal!\n");

  /* Initialize the Programmable Interrupt Controller & add interrupt handlers */
  interrupt_init();
  /* Initialize the PS/2 controller and keyboard */
  kbd_init();

  /* LOL just for fun */
  term_print("nqnsminishell> ");
  return;
}

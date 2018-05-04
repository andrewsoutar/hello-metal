  .section .low.text, "ax"
  .globl _asm_term_init
  .type _asm_term_init, @function
_asm_term_init:
  .code16

  // Initialize data segment registers
  movw $0x0020, %ax
  movw %ax, %ds
  movw %ax, %es
  movw %ax, %fs
  movw %ax, %gs
  movw %ax, %ss

  // Back up the old stack pointer
  movl %esp, %ecx

  // Load the BIOS interrupt descriptor table
  lidt real_mode_idt

  // Unset the protected-mode flag
  movl %cr0, %eax
  andl $(~0x1), %eax
  movl %eax, %cr0
  jmpl $0x0000, $real_mode
real_mode:
  // Initialize data segment registers
  movw $0x0000, %ax
  movw %ax, %ds
  movw %ax, %es
  movw %ax, %fs
  movw %ax, %gs

  // Set up a stack
  movw $0x1000, %ax
  movw %ax, %ss
  movw $0xFFFF, %sp

  // Enable interrupts
  sti

  // Save our saved stack pointer
  pushl %ecx

  // Int 0x10, AX=3: switch to VGA text mode
  movw $0x0003, %ax
  int $0x10

  // Restore old stack pointer
  popl %esp

  // Switch back to protected mode
  cli
  lgdt low_gdt
  movl %cr0, %eax
  orl $0x1, %eax
  movl %eax, %cr0
  jmpl $0x0008, $prot_mode
prot_mode:
  // Whew! We made it.
  .code32
  movw $0x0010, %ax
  movw %ax, %ds
  movw %ax, %es
  movw %ax, %fs
  movw %ax, %gs
  movw %ax, %ss
  lret

  /*
   * The global entry point for the operating system
   *
   * Sets up a stack and enables long mode, then jumps to the main function
   */

  .code32
  .section .text
  .globl _start
  .type _start,@function
_start:
  /*
   * _stack_end is defined by the linker script to be the top of the
   * stack; since the stack grows downward on x86, _stack_end is where we
   * start
   */
  movl $_stack_end, %esp

  /*
   * In the x64 ABI, RDI and RSI are the first and second arguments. Save EAX
	 * and EBX into the parameter registers now so we can reuse those registers.
   */
  movl %eax, %edi
  movl %ebx, %esi

  /* Set the 5th bit of CR4 to enable PAE */
  movl %cr4, %eax
  orl $(1 << 5), %eax
  movl %eax, %cr4

  /* Set the 8th bit of EFER (MSR 0xC0000080) to enable long mode */
  movl $0xC0000080, %ecx
  rdmsr
  orl $(1 << 8), %eax
  wrmsr

  /* Put the page table address into CR3 */
  movl $early_pml4, %eax
  movl %eax, %cr3

  /* Set bit 31 of CR0 to enable paging */
  movl %cr0, %eax
  orl $(1 << 31), %eax
  movl %eax, %cr0

  /* Load a GDT with a 64-bit code descriptor */
  lgdt gdt

  /*
   * The stack is already aligned, so we can just jump to main in the new code
	 * segment
   */
  ljmp $0x08, $main

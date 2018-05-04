  /*
   * The global entry point for the operating system
   *
   * Simply sets up a stack, then jumps to the main function
   */

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
   * Push arguments to the main function: multiboot magic (from eax),
   * multiboot info structure (from ebx). Arguments are pushed
   * right-to-left to comply with GCC's x86 ABI. Stack must be aligned
   * to 16 bytes.
   */
  subl $4, %esp                 // Take 4 bytes...
  pushl %ebx                    // ...and 4 bytes...
  pushl %eax                    // ...and another 4...
  call main                     // ...and 4 for the return address, to
                                // keep our 16-byte alignment
  // If main returns, fall through to our deaths

  .globl _halt
  .type _halt,@function
_halt:
  hlt                           // Try to pause the CPU
  jmp _halt                     // If we fail, keep trying

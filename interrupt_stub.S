  .section .data
handler_mem:
  .long handler

  .text
  .globl handler_stub
handler_stub:
  call *handler_mem

handler:
  cld
  // Grab the return address, simultaneously saving %edx
  xchgl %edx, (%esp)

  // Save %ecx, then grab what might be an error code
  pushl %ecx
  movl 0xC(%esp), %ecx

  // %eax is also caller-saved
  pushl %eax

  // Align the stack
  movl %esp, %eax
  andl $0xFFFFFFF0, %esp
  pushl %eax

  // Push arguments (stub address, error code)
  pushl %ecx
  pushl %edx
  call interrupt_handler_c

  // Restore stack pointer
  addl $0x8, %esp
  popl %esp

  // Did we return 0 or 1? (0 for no error code, 1 for error code)
  testl %eax, %eax

  // Restore registers
  popl %eax
  popl %ecx
  popl %edx

  // If we didn't return 0, take the error code off the stack
  jz ret
  addl $0x4, %esp
ret:
  iret

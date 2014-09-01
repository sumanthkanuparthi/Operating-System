#ifndef PAGEFAULT_H
#define PAGEFAULT_H

#include<defs.h>
struct pagefault_registers
  {
      uint64_t r15,r14,r13,r12,r11,r10;
      uint64_t r9,r8,rdi,rsi,rdx,rcx;
      uint64_t rbx,rax,rbp,errorCode;
      uint64_t rip,cs,rflags,rsp,ss;
  }__attribute((packed));
        
void pagefaulthandler(struct pagefault_registers *regvals);
             
#endif

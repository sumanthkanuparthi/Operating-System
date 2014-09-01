#ifndef SYSTEMCALL_H
#define SYSTEMCALL_H
#include<defs.h>
struct registers_t
  {
      uint64_t r15,r14,r13,r12,r11,r10;
      uint64_t r9,r8,rdi,rsi,rdx,rcx;
      uint64_t rbx,rax,rbp;
      uint64_t rip,cs,rflags,rsp,ss;
  }__attribute((packed));

#endif

#include<sys/systemcall.h>
#include <sys/contextswitch.h>
#include "stdio.h"
#include <sys/MManager.h>

void write();
 void memMap();
 void read();
 void w_opendir();
 void w_open();
 void w_readdir();
 void GetCurrentPath();
 void UpdateEnvPath();
 void UpdateCurrentPath();
 void listProcesses();
 void w_closedir();
 void w_close();
  void ulimit();
 /*struct registers_t
{
  uint64_t r15,r14,r13,r12,r11,r10;
  uint64_t r9,r8,rdi,rsi,rdx,rcx;
  uint64_t rbx,rax;
}__attribute((packed));
*/

struct registers_t* g_regVals;
static void *syscalls[21] =
 {
       &schedule,&ExitProcess,&Fork,&write,&GetCurrentProcessId,&memMap,&read,&ExecSysCall,&initSleep,&WaitSysCall,&WaitPidSysCall,&w_open,&w_opendir,&w_readdir,&GetCurrentPath,&UpdateCurrentPath,&UpdateEnvPath,&listProcesses,&w_closedir,&w_close,&ulimit
};

void syscall_handler(struct registers_t *regs)
{
     g_regVals = regs;
     int a = regs->rax;
     if(a == 17)
     {
       a =a;
     }
    void *location = syscalls[regs->rax];
          __asm volatile (
                   "call *%0; \n\t" 
                       :: "r" (location));
}


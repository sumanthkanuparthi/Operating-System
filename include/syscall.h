#ifndef _SYSCALL_H
#define _SYSCALL_H
#include <defs.h>
#include<sys/dirent.h>
#define SYSCALL_YIELD 0x0
#define SYSCALL_EXIT 0x1
#define SYSCALL_FORK 0x2
#define SYSCALL_WRITE 0x3
#define SYSCALL_GETPID 0x4
#define SYSCALL_MMAP 0x5
#define SYSCALL_READ 0x6
#define SYSCALL_EXEC 0x7
#define SYSCALL_SLEEP 0x8
#define SYSCALL_WAIT 0x9
#define SYSCALL_WAITPID 0xA
#define SYSCALL_OPEN 0xB
#define SYSCALL_OPENDIR 0xC
#define SYSCALL_READDIR 0xD
#define SYSCALL_CURRENTPATH 0xE
#define SYSCALL_UPDATEPATH 0xF
#define SYSCALL_UPDATE_ENV 0x10
#define SYSCALL_PS 0x11
#define SYSCALL_CLOSEDIR 0x12
#define SYSCALL_CLOSE 0x13
#define SYSCALL_ULIMIT 0x14
#define SYSCALL_PROTO(n) static __inline uint64_t __syscall##n

SYSCALL_PROTO(0)(uint64_t n) {
/*uint64_t ret;
__asm volatile("pushq %rax");
__asm volatile( "movq %0, %%rax\n\t"
                   ::"g"(n):"memory");
	__asm volatile("int $0x80;" : "=a" (ret));
__asm volatile("popq %rax");  
  return ret;
*/
  uint64_t ret;
    __asm volatile("movq %1,%%rax;"
                   "int $0x80;"
                   "movq %%rax,%0;"
                   :"=r"(ret)
                   :"r"(n)
                   :"rax"
                   );
           return ret;
  }

SYSCALL_PROTO(1)(uint64_t n, uint64_t a1) {
/*uint64_t ret=0;
__asm volatile("pushq %rax");
__asm volatile("pushq %rbx");

__asm volatile( "movq %0, %%rax\n\t"
                "movq %1,%%rbx\n\t"
                   ::"r"(n),"r"(a1):"memory");
	__asm volatile("int $0x80;" : "=a" (ret));
  __asm volatile("popq %rbx\n\t");
  __asm volatile("popq %rax\n\t");
  return ret;
*/
    uint64_t ret;
         __asm volatile("movq %1,%%rax;"
                        "movq %2, %%rbx;"
                        "int $0x80;"
                        "movq %%rax,%0;"
                        :"=r"(ret)
                        :"r"(n),"r"(a1)
                        :"rax","rbx"
                        );
                   return ret;
  }



SYSCALL_PROTO(2)(uint64_t n, uint64_t a1, uint64_t a2) {
/*uint64_t ret;
__asm volatile("pushq %rax\n\t"
               "pushq %rbx\n\t"
               "pushq %rcx\n\t");

  __asm volatile( "movq %0,%%rax\n\t"
                "movq %1,%%rbx\n\t"
                "movq %2,%%rcx\n\t"
                ::"r"(n),"r"(a1),"r"(a2):"memory");
  __asm volatile("INT $0x80":"=r"(ret)::"rax");
 
  __asm volatile("popq %rcx\n\t"
                 "popq %rbx\n\t"
                 "popq %rax\n\t");
  return ret;
  */
    uint64_t ret;
         __asm volatile("movq %1,%%rax;"
                        "movq %2, %%rbx;"
                        "movq %3, %%rcx;"
                        "int $0x80;"
                        "movq %%rax,%0;"
                        :"=r"(ret)
                        :"r"(n),"r"(a1),"r"(a2)
                        :"rax","rbx","rcx"
                       );
                   return ret;
}

SYSCALL_PROTO(3)(uint64_t n, uint64_t a1, uint64_t a2, uint64_t a3) {
  /*__asm volatile( "movq %0,%%rax\n\t"
                  "movq %1,%%rbx\n\t"
                  "movq %2,%%rcx\n\t"
                  "movq %3,%%rdx\n\t"
                  ::"r"(n),"r"(a1),"r"(a2),"r"(a3):"memory");
  __asm volatile("INT $0x80");
  return 0;
  uint64_t ret =0;
       __asm volatile("movq %0,%%rax;"
                      "movq %1, %%rbx;"
                      "movq %2, %%rcx;"
                      "movq %3, %%rdx;"
                      "int $0x80;"
                      ::"r"(n),"r"(a1),"r"(a2),"r"(a3)
                      :"rax","rbx","rcx","rdx"
                      );
                 return ret;

                */
uint64_t ret;
     __asm volatile("movq %1,%%rax;"
                    "movq %2, %%rbx;"
                    "movq %3, %%rcx;"
                    "movq %4, %%rdx;"
                    "int $0x80;"
                    "movq %%rax,%0;"
                    :"=r"(ret)
                    :"r"(n),"r"(a1),"r"(a2),"r"(a3)
                    :"rax","rbx","rcx","rdx","memory"
                    );
          return ret;
}

SYSCALL_PROTO(4)(uint64_t n, uint64_t a1, uint64_t a2, uint64_t a3, uint64_t a4) {
	return 0;
}
void yield();
int  write(int filedescriptor,char *buffer,int size); 
int read(int fd, char* buffer,int size);
int fork();
int GetProcessId();
void * mmap(void * addr,uint64_t size);
void *malloc(uint64_t nbytes);
void sleep(int limit);
int execvpe(char *fileName,char *arguements[],char *env[]);
void wait();
void waitpid(int pid);
void exit(int status);
int  parse(char *line, char **argv);
int atoi(char *nptr);
void ps();
dirent * readdir(DIR* dir);
int open(const char *pathname, int flags);
DIR * opendir(const char *fullpath);
void  getPath(char* buffer); 
void  updatePath(char* buffer); 
void updateEnvPath(char* buffer);
void ulimit(uint64_t size);
#endif

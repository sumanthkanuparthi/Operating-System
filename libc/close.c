#include<syscall.h>    
int close(int fd)
{
  uint64_t a = fd;
  int ret= __syscall1(SYSCALL_CLOSE,a);
  return ret;
}

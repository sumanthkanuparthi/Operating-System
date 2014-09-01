#include<syscall.h>   
#include<stdio.h>  

int execvpe(char *fileName,char *arguements[],char *env[])
{
  uint64_t a = (uint64_t)fileName;
  uint64_t b = (uint64_t)arguements;
  uint64_t c = (uint64_t)env;

  int ret=0;

  ret = __syscall3(SYSCALL_EXEC,a,b,c);
  return ret;
}

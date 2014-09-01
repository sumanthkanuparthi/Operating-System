#include<syscall.h>
#include <stdlib.h>



void  getPath(char* buffer)                                             
  {
    uint64_t a = (uint64_t)buffer;
    __syscall1(SYSCALL_CURRENTPATH,a);
 }



void  updatePath(char* buffer)                                             
  {
    uint64_t a = (uint64_t)buffer;
    __syscall1(SYSCALL_UPDATEPATH,a);
 }


void updateEnvPath(char* buffer)
  {
    uint64_t a = (uint64_t)buffer;
    __syscall1(SYSCALL_UPDATE_ENV,a);
 }

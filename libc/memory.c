#include <stdio.h>
#include <syscall.h>
#define BLOCK 4096
void * mmap(void * address,uint64_t size)
{
  uint64_t ret;
  uint64_t a =(uint64_t)address;
  uint64_t b = (uint64_t)size;
ret =   __syscall2(SYSCALL_MMAP,a,b);
return (void*)ret;
}


void *malloc(uint64_t nbytes)
{
 static uint64_t count =0;
 static uint64_t ptr = 0;
 
 uint64_t temp=0;
 if(nbytes <= count)
 {

     temp = ptr;
     ptr += nbytes;
     count -= nbytes;


 }
 else
 {
      uint64_t noPages = 1;
      uint64_t size = 4096;
      while(nbytes > size)
	{
      	   size = 4096* (++noPages);
       }

     void *p = mmap(NULL,noPages);

     if(p == (void*)-1)
	return (void*)-1;

     count = count + noPages*4096;
     

     if(ptr == 0)
	{
        ptr = (uint64_t)p;
	}

     temp = ptr;
     ptr += nbytes;
     count -= nbytes;
    

 }

    return (void*)temp;
}






#include<syscall.h>

void ulimit(uint64_t size)
{
__syscall1(SYSCALL_ULIMIT,size);

}


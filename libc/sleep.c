

#include<syscall.h>

void sleep(int limit)
{
	__syscall1(SYSCALL_SLEEP,limit);
}
#include<stdlib.h>
#include<syscall.h>

void exit(int status) 
{
	__syscall1(SYSCALL_EXIT,status);
}

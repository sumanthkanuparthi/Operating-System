#include<syscall.h>


void wait()
{
	 __syscall0(SYSCALL_WAIT);
}

void waitpid(int pid)
{
	 __syscall1(SYSCALL_WAITPID,pid);
}


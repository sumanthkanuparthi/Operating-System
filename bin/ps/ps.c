#include <syscall.h>

#include<stdio.h>
int main()
{
    printf("\nNAME            PID\n");
    printf("-----------------------\n");
    __syscall0(SYSCALL_PS);
     return 0;
}


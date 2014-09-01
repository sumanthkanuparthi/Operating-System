#include<syscall.h>
void  yield()
{

     __syscall0(SYSCALL_YIELD);



}

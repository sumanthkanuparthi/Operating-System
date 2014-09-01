#include <syscall.h>

int fork()
{
  return __syscall0(SYSCALL_FORK);
}

int GetProcessId()
{
  return __syscall0(SYSCALL_GETPID);
}

//To signa; process is Background process
void SetPGID(int parent)
{

}




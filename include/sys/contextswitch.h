#ifndef CONTEXTSWITCH_H
#define CONTEXTSWITCH_H

#include <defs.h>
void firstSwitch();
#define STACK_SIZE 256
typedef struct
{
    int8_t processId;
    uint64_t stack[STACK_SIZE];
    uint64_t sp;
    uint64_t cr3;
    struct mm_struct * memMap;
    struct task_struct* parentTask;
    uint64_t sleepTime;
    uint64_t sleepLimit;
    int8_t waitId;
	
}task_struct;

void schedule();
void startingPoint();
void ExitProcess();
int Fork();
void initSleep();
void ExecSysCall();
void WaitSysCall();
void WaitPidSysCall();
void GetCurrentProcessId();
#endif 

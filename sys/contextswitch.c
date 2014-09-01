#include <sys/contextswitch.h>
#include "pagetable.h"
#include "stdio.h"
#include <sys/gdt.h>
#include <sys/MManager.h>
#include <sys/queue.h>
#include <sys/systemcall.h>
#include <sys/pmap.h>
#include<sys/TARFSReader.h>
#include<sys/string.h>
queue activeProcesses;  //Creating a Queue
queue waitingProcesses;
extern char currentPath[100];
extern char envPaths[200];
int8_t bskipAddingTask = 0;
int g_nProcessId = 0;   //Initializing processId to ZERO

task_struct proc1, proc2;
extern uint64_t COW;

task_struct *currentTask = 0;

task_struct * newPro = 0;

uint64_t *stack;

uint64_t userStackAddr = 0xFFFFFE0000000000UL;
uint64_t stackHardLimit= 0xFFFEFF0000000000UL;
uint64_t stackSoftLimit;
//uint64_t userStackAddr = 0x0000000FFFF00000UL;
//uint64_t userStackAddr = 0x50000;
uint64_t argEntries = 0xFFFFFE0000002000UL;
uint64_t envEntries = 0xFFFFFE0000004000UL;

#define stackLimit 0xFFFFFF0007800000

uint64_t curStack;

extern struct registers_t* g_regVals;
extern uint32_t g_seconds;



void ulimit()
{
  int size = g_regVals->rbx;
if(userStackAddr - size >= stackHardLimit)
   stackSoftLimit = userStackAddr -size;
else
{
  printf("Stack size exceeds the Hard Limit");
}



}







void initSleep()
{
	currentTask->sleepLimit = g_regVals->rbx;
	currentTask->sleepTime = g_seconds;
	schedule();

}


void WaitSysCall()
{
	int waitQueueSize = activeProcesses.count;
	int bAdd = 0;

		while(waitQueueSize--)
		{
			task_struct *task = dequeue(&activeProcesses);
			task_struct *parent = (task_struct*)task ->parentTask;

			if(parent != NULL)
			{

				if(parent->processId == currentTask->processId )
				{
					bAdd = 1;
				}
			}
			enqueue(&activeProcesses,task );
		}

	if(bAdd == 0)
		return;
	enqueue(&waitingProcesses,currentTask);
	currentTask->waitId = -1;
	bskipAddingTask = 1;
	schedule();
}

void WaitPidSysCall()
{

	int waitQueueSize = activeProcesses.count;
	int bAdd = 0;

	int waitId = (int)g_regVals->rbx;



		while(waitQueueSize--)
		{
			task_struct *task = dequeue(&activeProcesses);
			task_struct *parent = (task_struct*)task ->parentTask;

			if(parent != NULL)
			{
				if(parent->processId == currentTask->processId && task->processId == waitId)
				{
					bAdd = 1;
				}
			}
			enqueue(&activeProcesses,task );
		}

	if(bAdd == 0)
	{
		return;
	}
	enqueue(&waitingProcesses,currentTask);
	currentTask->waitId = waitId;
	bskipAddingTask = 1;
	schedule();
}


void GetCurrentProcessId()
{
	if(currentTask != NULL)
		g_regVals->rax =  currentTask->processId;
	else
		g_regVals->rax = -1; 	
}


extern uint64_t virtualBaseAddr;
int FindFullPath(char *fullPath)
{

	char* path = envPaths;

	while(*path)
	{
		char temp[100] = "";
		char *tempPtr = temp;
		while(*path != ':' && *path)
		{
			*tempPtr++ =  *path++;
		}
		char *fileName =(char*)g_regVals->rbx;
		while(*fileName != '\0')
			*tempPtr ++ = *fileName++;

		int ripVal = IsValidFile(temp);

		if(ripVal != -1)
		{
			char *ptr = temp;
			while(*ptr)
				*fullPath++ = *ptr++;
			return 1;
		 }
		path = path+1;;
	}

	
	return -1;
}
//updates the current task based on the arguements passed from the system call
void ExecSysCall()
{


  char name[100] = "";
  char *namePtr = name;

 char* path = currentPath;

  while(*path != '\0')
	*namePtr++ = *path ++;
  char *fileName =(char*)g_regVals->rbx;


  while(*fileName != '\0')
	*namePtr++ = *fileName++;


  int ripVal = IsValidFile(name);

  if(ripVal == -1) //did not find binary in current path
  {
	int i=0;
	for(i=0;i<100;i++)
		name[i] = '\0';
	
	if(FindFullPath(name) == -1)
	{
		g_regVals->rax = -1;
		return;
	}
  }

   char **argsStart=(char**)g_regVals->rcx;
   char **envStart=(char**)g_regVals->rcx;

  char *args =  (char*)AssignPhysicalToVirtual(argEntries,1,7);

   int m=0;
   while(*argsStart[m]!= '\0')
   {
	printf("\n Arg = %s",argsStart[m]);
     int j=0;
     while(argsStart[m][j]!= '\0')
	{
	*args++ = argsStart[m][j];
	j++;
 	}
      *args++ = ' ';
       m++;
   }

   if(strlen(argsStart[m-1])==1)
 	if(argsStart[m-1][0]=='&')
 		currentTask->memMap->isBgProcess = 1;
//Setting name to new process
   strcpy(currentTask->memMap->name,name);
   *args ='\0'; 


 //char **envs =  (char**)AssignPhysicalToVirtual(envEntries,1,7);

 envStart = envStart;







  struct vm_area_struct* prevVmas = currentTask->memMap->head;

  	while(prevVmas != NULL)
	{
		uint64_t temp = (uint64_t)prevVmas->vm_start ;
		prevVmas = prevVmas ->vm_next;
		uint64_t taskvma = GetMappedPhysicalAddress(temp);
	 	 map_page((void*)0x0,(uint64_t*)temp,0);
		if((COW&taskvma)  != COW)
		{
			taskvma = ~COW&taskvma ;
			unFreePage(taskvma);
			//memset((void*)((uint64_t)taskvma|virtualBaseAddr),0,0x1000);
		}		
	}


 	//freeing the vmas itself 

	struct vm_area_struct *vma = currentTask->memMap->head;

	while(vma != NULL)
	{
		uint64_t taskvma = GetMappedPhysicalAddress((uint64_t)vma);
		unFreePage(taskvma);
		vma = vma->vm_next;
	}

	

  currentTask->memMap->head = NULL;


  uint64_t rip = ReadElf(currentTask->memMap,name);

  if(rip == -1)
  {
	printf("\n%s : Command not found!!!",name);
	ExitProcess();
  }


  stack =  (uint64_t*)AssignPhysicalToVirtual(userStackAddr ,1,7);


  	//creating new Stack Vma
  	struct vm_area_struct* newVma  = (struct vm_area_struct *)AllocateVirtualMemory();
  	newVma->vm_start = userStackAddr  ;
  	newVma->sectionOffset= -1;
  	newVma->vm_end = userStackAddr + 0x1000;
  	newVma->vm_next = currentTask->memMap->head;
	newVma->permFlags = 3;

  	currentTask->memMap->head = newVma;


	int j=0;
	for(j=0;j<STACK_SIZE;j++)
		currentTask->stack[j] =0;

	uint64_t strTop = (uint64_t)&(currentTask->stack[STACK_SIZE-1]);

	__asm volatile( "movq %0, %%rsp ": : "m"(strTop) : "memory" );


	__asm volatile("pushq $0x23\n\t"
				"pushq %0\n\t"
				"pushq $0x200\n\t"
				"pushq $0x1b\n\t"
				"pushq %1\n\t"
				: :"c"((uint64_t)(&stack[500])),"d"(rip) :"memory");



	__asm volatile("pushq $0x0\n\t"
				"pushq $0x0\n\t"
				"pushq $0x0\n\t"
				"pushq $0x0\n\t"
				"pushq $0x0\n\t"
				"pushq %1\n\t"
				"pushq %0\n\t"
				"pushq $0x0\n\t"
				"pushq $0x0\n\t"
				"pushq $0x0\n\t"
				"pushq $0x0\n\t"
				"pushq $0x0\n\t"
				"pushq $0x0\n\t"
				"pushq $0x0\n\t"
				"pushq $0x0\n\t"
				::"m"(argEntries),"m"(envEntries):"memory");
   


	
	__asm volatile("movq %%rsp, %0;":"=g"(currentTask->sp));

	schedule();



}

void  CreateProcess(char fileName[100])
{

	newPro  = (task_struct *)AllocateVirtualMemory();

	newPro->parentTask = NULL;	
	newPro->sleepLimit = -1;
	newPro->sleepTime= -1;

	currentTask = newPro;
	newPro->processId = g_nProcessId++;
	newPro->cr3 =  (uint64_t)SetUpMemoryForNewProcess();

	struct PML4 *current  = (struct PML4 *)getCr3Value();
       SetCr3Value((struct PML4 *)newPro ->cr3);

	newPro->memMap = (struct mm_struct *)AllocateVirtualMemory();
	newPro->memMap->isBgProcess = 0;

if((strcmp(fileName,"bin/shell")==0))
   {
      strcpy(newPro->memMap->name,"SBUsh");
  }




	__asm volatile("movq %%rsp, %0;":"=g"(curStack));


	uint64_t strTop = (uint64_t)&(newPro->stack[STACK_SIZE-1]);

	__asm volatile( "movq %0, %%rsp ": : "m"(strTop) : "memory" );

	
	uint64_t rip = ReadElf(newPro->memMap,fileName);



	

	stack =  (uint64_t*)AssignPhysicalToVirtual(userStackAddr ,1,7);


	//stack =  (uint64_t*)userStackAddr;

	//creating new Stack Vma
	struct vm_area_struct* newVma  = (struct vm_area_struct *)AllocateVirtualMemory();
	newVma->vm_start = userStackAddr  ;
	newVma->sectionOffset= -1;
	newVma->vm_end = userStackAddr + 0x1000;
	newVma->vm_next = newPro->memMap->head;
	newVma->permFlags = 3;

	newPro->memMap->head = newVma;
	enqueue(&activeProcesses,newPro);


	__asm volatile("pushq $0x23\n\t"
				"pushq %0\n\t"
				"pushq $0x200\n\t"
				"pushq $0x1b\n\t"
				"pushq %1\n\t"
				: :"c"((uint64_t)(&stack[500])),"d"(rip) :"memory");



	__asm volatile("pushq $0x0\n\t"
				"pushq $0x0\n\t"
				"pushq $0x0\n\t"
				"pushq $0x0\n\t"
				"pushq $0x0\n\t"
				"pushq $0x0\n\t"
				"pushq $0x0\n\t"
				"pushq $0x0\n\t"
				"pushq $0x0\n\t"
				"pushq $0x0\n\t"
				"pushq $0x0\n\t"
				"pushq $0x0\n\t"
				"pushq $0x0\n\t"
				"pushq $0x0\n\t"
				"pushq $0x0\n\t"
				: : :"memory");


	
	__asm volatile("movq %%rsp, %0;":"=g"(newPro->sp));

	__asm volatile( "movq %0, %%rsp ": : "m"(curStack) : "memory" );

	SetCr3Value(current);

	return;

}

int Fork()
{
	//creating new PCB
	newPro  = (task_struct *)AllocateVirtualMemory();
	
	//setting current Task as parent Task
	newPro->parentTask = (struct task_struct *)currentTask;
	
	newPro->sleepLimit = -1;
	newPro->sleepTime= -1;


	//setting new ID
	newPro->processId = g_nProcessId++;

	//copy Page Tables
	newPro->cr3 = (uint64_t)CopyPageTables();

	struct PML4 *current  = (struct PML4 *)getCr3Value();

	SetCr3Value((struct PML4 *)newPro ->cr3);

	newPro->memMap = (struct mm_struct *)AllocateVirtualMemory();

	//parent is background, child is also a background process
	newPro->memMap->isBgProcess = currentTask->memMap->isBgProcess;

	CopyVmaInfo(newPro->memMap);

	stack =  (uint64_t*)userStackAddr;

	//creating new Stack Vma
	struct vm_area_struct* newVma  = (struct vm_area_struct *)AllocateVirtualMemory();
	newVma->vm_start = userStackAddr  ;
	newVma->sectionOffset= -1;
	newVma->vm_end = userStackAddr + 0x1000;
	newVma->vm_next = newPro->memMap->head;
	newVma->permFlags = 3;

	newPro->memMap->head = newVma;


	__asm volatile("movq %%rsp, %0;":"=g"(curStack));


	uint64_t strTop = (uint64_t)&(newPro->stack[STACK_SIZE-1]);

	__asm volatile( "movq %0, %%rsp ": : "m"(strTop) : "memory" );


	enqueue(&activeProcesses,newPro);


	__asm volatile("pushq $0x23\n\t"
				"pushq %0\n\t"
				"pushq $0x200\n\t"
				"pushq $0x1b\n\t"
				"pushq %1\n\t"
				: :"m"(g_regVals->rsp),"m"(g_regVals->rip) :"memory");

	__asm volatile("pushq %0\n\t"
				"pushq $0x0\n\t"
				"pushq %2\n\t"
				"pushq %3\n\t"
				"pushq %4\n\t"
				"pushq %5\n\t"
				"pushq %6\n\t"
				"pushq %7\n\t"
				"pushq %8\n\t"
				"pushq %9\n\t"
				"pushq %10\n\t"
				"pushq %11\n\t"
				"pushq %12\n\t"
				"pushq %13\n\t"
				"pushq %14\n\t"
				: :"m"(g_regVals->rbp),"m"(g_regVals->rax),"m"(g_regVals->rbx),"m"(g_regVals->rcx),"m"(g_regVals->rdx),"m"(g_regVals->rsi),"m"(g_regVals->rdi),"m"(g_regVals->r8),"m"(g_regVals->r9),"m"(g_regVals->r10),"m"(g_regVals->r11),"m"(g_regVals->r12),"m"(g_regVals->r13),"m"(g_regVals->r14),"m"(g_regVals->r15):"memory");

	g_regVals->rax =  newPro->processId;

	__asm volatile("movq %%rsp, %0;":"=g"(newPro->sp));

	__asm volatile( "movq %0, %%rsp ": : "m"(curStack) : "memory" );

	SetCr3Value(current);

	return 0;
}


void schedule()
{
	task_struct * task =0;

	if(bskipAddingTask != 1)
	{
		//adding back to the queue
		activeProcesses.last = (activeProcesses.last+1) % QUEUESIZE;
       	activeProcesses.q[ activeProcesses.last ] = currentTask;    
       	activeProcesses.count = activeProcesses.count + 1;
	}
	else
	{
		bskipAddingTask = 0;
	}

	//getting new task from the queue - round robin
	 task  = activeProcesses.q[ activeProcesses.first ];
        activeProcesses.first = (activeProcesses.first+1) % QUEUESIZE;
        activeProcesses.count = activeProcesses.count - 1;

	
	 
	while(task->sleepLimit != -1)
	{
		if((g_seconds - task->sleepTime) < task->sleepLimit)
		{
			//adding back to the queue
			activeProcesses.last = (activeProcesses.last+1) % QUEUESIZE;
       		activeProcesses.q[ activeProcesses.last ] = task;    
       		activeProcesses.count = activeProcesses.count + 1;

			//getting new task from the queue - round robin
	 		task  = activeProcesses.q[ activeProcesses.first ];
        		activeProcesses.first = (activeProcesses.first+1) % QUEUESIZE;
        		activeProcesses.count = activeProcesses.count - 1;
		}
		else
		{
			task->sleepTime = -1;
			task->sleepLimit = -1;
		}
	
	}

	
	while(task->cr3 == 0)
	{
			//adding back to the queue
			activeProcesses.last = (activeProcesses.last+1) % QUEUESIZE;
       		activeProcesses.q[ activeProcesses.last ] = task;    
       		activeProcesses.count = activeProcesses.count + 1;

			//getting new task from the queue - round robin
	 		task  = activeProcesses.q[ activeProcesses.first ];
        		activeProcesses.first = (activeProcesses.first+1) % QUEUESIZE;
        		activeProcesses.count = activeProcesses.count - 1;

	}

	currentTask = task ;

	
	SetCr3Value((struct PML4 *)currentTask->cr3);
	//switching page tables
	__asm volatile("movq %0, %%cr3":: "a"(task ->cr3));
	


	//settign tss to point to top of stack
	tss.rsp0 = (uint64_t)&(task ->stack[STACK_SIZE-1]);
	
	//switcing stack to the new task stack
	__asm volatile( "movq %0, %%rsp ": : "m"(task ->sp) : "memory" );	

	__asm volatile("popq %%r15\n\t"
				"popq %%r14\n\t"
				"popq %%r13\n\t"
				"popq %%r12\n\t"
				"popq %%r11\n\t"
				"popq %%r10\n\t"
				"popq %%r9\n\t"
				"popq %%r8\n\t"
				"popq %%rdi\n\t"
				"popq %%rsi\n\t"
				"popq %%rdx\n\t"
				"popq %%rcx\n\t"
				"popq %%rbx\n\t"
				"popq %%rax\n\t"
				"popq %%rbp\n\t"
				: : :"memory");

	__asm volatile("iretq");	

}



void firstSwitch()
{
	
	//getting new task from the queue - round robin
	currentTask = dequeue(&activeProcesses);


	SetCr3Value((struct PML4 *)currentTask->cr3);
	//switching page tables
	__asm volatile("movq %0, %%cr3":: "a"(currentTask->cr3));

	

	tss.rsp0 = (uint64_t)&(currentTask->stack[STACK_SIZE-1]);

       int a = 0x28;
	__asm volatile("movq %0,%%rax;\n\t"
				"ltr (%%rax);"::"r"(&a));

	
	//switcing stack to the new task stack
	__asm volatile( "movq %0, %%rsp ": : "m"(currentTask->sp) : "memory" );	



	__asm volatile("popq %%r15\n\t"
				"popq %%r14\n\t"
				"popq %%r13\n\t"
				"popq %%r12\n\t"
				"popq %%r11\n\t"
				"popq %%r10\n\t"
				"popq %%r9\n\t"
				"popq %%r8\n\t"
				"popq %%rdi\n\t"
				"popq %%rsi\n\t"
				"popq %%rdx\n\t"
				"popq %%rcx\n\t"
				"popq %%rbx\n\t"
				"popq %%rax\n\t"
				"popq %%rbp\n\t"
				: : :"memory");

	__asm volatile("iretq");	
	
}


void startingPoint()
{
	__asm volatile ("cli");

	init_queue(&activeProcesses);
	init_queue(&waitingProcesses);

  	//CreateProcess("bin/fork");
  	//CreateProcess("bin/hello");
	CreateProcess("bin/idle");
	CreateProcess("bin/shell");
	
	
	firstSwitch();
	
}


void ExitProcess()
{
	
	task_struct *tasktobeDeleted = currentTask;

	
	//first freeing the entries stored in the VMAs 
	struct vm_area_struct* prevVmas = tasktobeDeleted->memMap->head;
  	while(prevVmas != NULL)
	{
		uint64_t temp = (uint64_t)prevVmas->vm_start ;
		prevVmas = prevVmas ->vm_next;
		uint64_t taskvma = GetMappedPhysicalAddress(temp);
		//MapPhysicaltoVirtual(current,(uint64_t)temp,0x0,0);
		if((COW&taskvma)  != COW)
		{
			taskvma = ~COW&taskvma ;
			unFreePage(taskvma);
			//memset((void*)((uint64_t)taskvma|virtualBaseAddr),0,0x1000);
		}		
	}

	

	
	//freeing the vmas itself 

	struct vm_area_struct *vma = tasktobeDeleted->memMap->head;

	while(vma != NULL)
	{
		uint64_t taskvma = GetMappedPhysicalAddress((uint64_t)vma);
		unFreePage(taskvma);
		vma = vma->vm_next;
	}

	

	//freeing mm struct
	uint64_t taskmms = GetMappedPhysicalAddress((uint64_t)tasktobeDeleted->memMap);
	unFreePage(taskmms);

	
	//CRASHING BECAUSE OF THIS!!..DEBUG LATER!!!..
	//freeing cr3
	uint64_t taskcr3 = GetMappedPhysicalAddress((uint64_t)tasktobeDeleted->cr3);
	unFreePage(taskcr3);	

	

	//check if parent process is waiting
	task_struct *task = (task_struct*)tasktobeDeleted->parentTask;
	
	if(task != NULL)
	{

		int8_t parentId = task->processId;

		int waitQueueSize = waitingProcesses.count;

		while(waitQueueSize--)
		{
			task_struct *parentTask = dequeue(&waitingProcesses);

			if(parentTask->processId == parentId )
			{
				if(parentTask->waitId == -1 || parentTask->waitId == tasktobeDeleted->processId)
				{
					parentTask->waitId = -1;
					enqueue(&activeProcesses,parentTask);
					break;
				}
			}
			enqueue(&waitingProcesses,parentTask);
		}

	}
	
	
	//freeing PCB
	uint64_t taskPCB = GetMappedPhysicalAddress((uint64_t)tasktobeDeleted);
	unFreePage(taskPCB );


	bskipAddingTask  = 1;

	schedule();



}

void listProcesses()
{
    task_struct *tempTask = currentTask;
    char absoluteName[100];
    char *fullname =  tempTask->memMap->name;
    int len = strlen(fullname);
    int i = lastIndexOf (fullname,"/");
    char *shortname;
    char name[50];
    shortname = name;
    substring(shortname,fullname,i+1,len);
    //Adding current task to the queue
                    
    int apCount = activeProcesses.count;        
    while(apCount--)
    {                    
     printf("%s               %d\n", name,tempTask->processId);              
     IDLE:
        enqueue(&activeProcesses,tempTask);
        tempTask  = dequeue(&activeProcesses);
                           
     //Idle case
     if(tempTask->processId ==0)
     goto IDLE;                  
     int k=0;
     for(k=0;k<100;k++)
        absoluteName[k] = '\0'; 
      for(k=0;k<50;k++)
        name[k] = '\0';
         
      fullname = absoluteName;
      fullname = tempTask->memMap->name;
      len = strlen(fullname);
      i = lastIndexOf (fullname,"/");
      if(i==-1)
      {
      strcpy(shortname,fullname);
      }
      else
      {
      shortname = name;
      substring(shortname,fullname,i+1,len);
      }
      }
     int wpCount = waitingProcesses.count;
       
         while(wpCount>0)
       { 
         tempTask  = dequeue(&waitingProcesses);
              int k=0;
               for(k=0;k<100;k++)
               absoluteName[k] = '\0';

               for(k=0;k<50;k++)
                     name[k] = '\0';
                 
              fullname = absoluteName;
              fullname = tempTask->memMap->name;
              len = strlen(fullname);
              i = lastIndexOf ("/", fullname);
              if (i==-1)
              {
                strcpy(shortname,fullname);
              }
              else
              {
              shortname = name;
              substring(shortname,fullname,i+1,len);
              }        
              printf("%s               %d\n", name,tempTask->processId); 
              enqueue(&waitingProcesses,tempTask);
       wpCount--; 
       }
        ExitProcess();
}

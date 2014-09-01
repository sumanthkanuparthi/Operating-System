#include "stdio.h"
#include <sys/pagefault.h>
#include <sys/MManager.h>
#include "pagetable.h"
#include <sys/contextswitch.h>

extern uint64_t COW;
extern task_struct *currentTask;
extern uint64_t stackHardLimit;
void pagefaulthandler(struct pagefault_registers *regvals)
{
	uint64_t faultAddr;
    	__asm volatile("mov %%cr2, %0" : "=r" (faultAddr));
	printf("\nPage Fault accessing : %p",faultAddr);

	switch(regvals->errorCode)
	{
		case 0:
		CopyCodeData(faultAddr,regvals->errorCode);
		break;
		case 1:
		ExitProcess();
		break;
		case 2:
		CopyCodeData(faultAddr,regvals->errorCode);
		break;
		case 3:
			ExitProcess();
			break;
		case 4:
		case 6:
      if(regvals->rsp - 0x8 == faultAddr)
      {
        if(regvals->rsp == stackHardLimit)
        {
          printf("Stack Overflow!!!");
          ExitProcess();
        }
        else
        {
          //Auto Growing Stack
          struct vm_area_struct * temp = currentTask->memMap->head;
          while(temp->vm_next != NULL)
            temp = temp->vm_next;
           
          struct vm_area_struct * newVMA=(struct vm_area_struct*)AllocateVirtualMemory();
           newVMA->vm_start = regvals->rsp+0x8-0x1000;
           newVMA->vm_start = regvals->rsp & 0xFFFFFFFFFFFFF000;
           newVMA->vm_end = newVMA->vm_start+0x1000;                                                                                                                      
           newVMA->vm_next = NULL;
           newVMA->sectionOffset = -1;
           newVMA->permFlags = 3;
           temp->vm_next = newVMA;
        }

      }

			CopyCodeData(faultAddr,regvals->errorCode);
			break;
		case 5:
			//User process tried to read a present page and caused a protection fault
			ExitProcess();
			break;
		case 7:;
			uint64_t COW = 0x10000000000000UL;
			uint64_t PTentry = GetMappedPhysicalAddress(faultAddr);
			
			if((COW&PTentry)  == COW)
			{
				CopyVmaData(faultAddr);
			}
			else
			{
				ExitProcess();
			}
			break;
		default :
			break;
	};
}

#include <sys/MManager.h>
#include "pagetable.h"
#include <sys/TARFSReader.h>
#include <sys/ELFReader.h>
#include <sys/contextswitch.h>



extern task_struct * currentTask;
extern uint64_t virtualBaseAddr;
extern uint64_t COW;
extern void ExitProcess();
void CopyVmaInfo(struct mm_struct *memStr)
{
	memStr->fileStartPointer = currentTask->memMap->fileStartPointer;

	struct vm_area_struct* current = currentTask->memMap->head;
	
	struct vm_area_struct* prev = NULL;

	while(current!= NULL)
	{
		struct vm_area_struct* newVma  = (struct vm_area_struct *)AllocateVirtualMemory();
		if(prev != NULL)
			{
				prev->vm_next = newVma;
			}
			else
			{
				memStr->head = newVma;
			}	
		newVma->vm_start = current->vm_start ;
		newVma->sectionOffset= current->sectionOffset;
		newVma->vm_end = current->vm_end;
		newVma->vm_next = NULL;
		prev = newVma ;
		newVma->permFlags =  current->permFlags;
		
		current = current->vm_next;
	}
}

uint64_t ReadElf(struct mm_struct *memStr,char *fileName)
{

	char *start = ReadFile(fileName);
	uint64_t rip= -1;

	struct Exe_Format exeFormat;
	memStr->fileStartPointer = (uint64_t)start;
	rip= Parse_ELF_Executable( start , &exeFormat);
    	if (rip!= -1) 
	{
		int i =0;

		struct vm_area_struct* prev = NULL;
		for(i=0;i<exeFormat.numSegments;i++)
		{
			if(exeFormat.segmentList[i].sizeInMemory > 0)
			{
			 struct vm_area_struct* newVma  = (struct vm_area_struct *)AllocateVirtualMemory();
			 if(prev != NULL)
				{
					prev->vm_next = newVma;
				}
				else
				{
					memStr->head = newVma;
				}	
			 newVma->vm_start = exeFormat.segmentList[i].startAddress;
			 newVma->sectionOffset= exeFormat.segmentList[i].offsetInFile;
                      newVma->copySize =  exeFormat.segmentList[i].lengthInFile;
			 newVma->vm_end = newVma->vm_start +  exeFormat.segmentList[i].sizeInMemory;
			 newVma->permFlags = exeFormat.segmentList[i].protFlags;
			 newVma->vm_next = NULL;
			 prev = newVma ;
			}

		}
	}
	
/*
	char* addr = (char*)AssignPhysicalToVirtual(0x0000000000400000,4);
	int n = 12374;

	 while(n--)
        	*addr ++ = *start++;
*/	
	return rip;
}

int CopyCodeData(uint64_t virtAddr,int perms)
{
	struct mm_struct *memStr = (struct mm_struct *)currentTask->memMap;
	int success = 0;
	if(memStr != NULL)
	{
		struct vm_area_struct* curVma = memStr->head;

		while(curVma != NULL)
		{
			if(curVma->vm_start<= virtAddr && virtAddr < curVma->vm_end)
			{
				{

				int totSize = curVma->copySize;
				uint64_t noPages = 1;
      				uint64_t size = 4096;
      				while(totSize > size)
				{
      	   				size = 4096* (++noPages);
       			}

				int flags = -1;

				if((curVma->permFlags & 0x2) == 0x2)
				{
					flags =7;
				}
				else
				{
					flags = 5;

					if((perms & 0x2) == 0x2)// if user is trying to write into a read section  such as .text
					{
						ExitProcess();
					}
				}


				//based on VMA size we need to change the number of pages beign allocated below
				AssignPhysicalToVirtual(curVma->vm_start,noPages,flags);
				
				if(curVma->sectionOffset != -1)
				{
					
					char* phyStart = (char*)(memStr->fileStartPointer + curVma->sectionOffset);
					char* addr = (char*)curVma->vm_start;
					while(totSize--)
        					*addr ++ = *phyStart++;
				}
				else
				{
					printf("for stack");
				}	

				success = 1;
				break;
				}			
			}
			curVma = curVma ->vm_next;

		}
	}

	if(success == 0)// if user is trying to write into a section that has no VMA
	{
		ExitProcess();
	}

	
	return success;
}


void memcpy(void *from, void *to,int size)
{
		char* phyStart = (char*)from;
		char* addr = (char*)to;
		while(size--)
        		*addr ++ = *phyStart++;

}


int CopyVmaData(uint64_t virtAddr)
{
  uint64_t base = virtualBaseAddr;
	struct mm_struct *memStr = (struct mm_struct *)currentTask->memMap;
	int success = 0;
	if(memStr != NULL)
	{
		struct vm_area_struct* curVma = memStr->head;

		while(curVma != NULL)
		{
			if(curVma->vm_start <= virtAddr && virtAddr <= curVma->vm_end)
			{
				uint64_t nbytes = curVma->vm_end - curVma->vm_start;
				uint64_t noPages = 1;
      				uint64_t size = 4096;
      				while(nbytes > size)
				{
      	   				size = 4096* (++noPages);
       			}

				int flags = -1;

				if((curVma->permFlags & 0x2) == 0x2)
				{
					flags =7;
				}
				else
				{
					flags = 5;
				}

				uint64_t PTentry = GetMappedPhysicalAddress(curVma->vm_start);

				//Create a new Physical Page fr this VMA into which we will copy the contents from the old physical address it was pointing at
				AssignPhysicalToVirtual(curVma->vm_start,noPages,flags);

				memcpy((void *)(base|(PTentry&~COW)),(void *)(curVma->vm_start),4096*noPages);

				success = 1;
				break;			
			}
      curVma = curVma->vm_next;
		}
	}
	return success;
}
uint64_t HeapStart = 0xFFF0000;
uint64_t sbrk = 0xFFF0000;


void * mmap(void *addr,uint64_t noPages)
{
  /*uint8_t noPages = 1; //no of pages
  uint64_t size = PAGE_SIZE;  
    while(len > size)
    {
      size = PAGE_SIZE * (++noPages);
    }*/
   
    struct vm_area_struct * temp = currentTask->memMap->head;
    while(temp->vm_next != NULL)
      temp = temp->vm_next;
    
    struct vm_area_struct * newVMA=(struct vm_area_struct*)AllocateVirtualMemory();

newVMA->vm_start = sbrk;
newVMA->vm_end = sbrk + noPages*PAGE_SIZE;                                                                                                                                   
newVMA->vm_next = NULL;
newVMA->sectionOffset = -1; 
newVMA->permFlags = 3;
temp->vm_next = newVMA;

    sbrk  = newVMA->vm_end;
 
    return (void*)newVMA->vm_start;

}



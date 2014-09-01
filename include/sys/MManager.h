#ifndef MMANAGER_H
#define MMANAGER_H

#include<defs.h>

struct vm_area_struct
{
	uint64_t vm_start;
	uint64_t vm_end;
	uint64_t copySize;
	uint64_t sectionOffset;
	uint8_t permFlags;
	
	struct vm_area_struct *vm_next;
};



struct mm_struct 
{
	struct vm_area_struct * head;
	struct vm_area_struct * current;
	uint64_t fileStartPointer;
	char name[100];
	uint8_t isBgProcess;
};

#define PAGE_SIZE 4096


uint64_t ReadElf(struct mm_struct *memStr,char *fileName);
int CopyCodeData(uint64_t virtAddr,int perms);
void CopyVmaInfo(struct mm_struct *memStr);
int CopyVmaData(uint64_t virtAddr);
void * mmap(void *addr,uint64_t len);
#endif

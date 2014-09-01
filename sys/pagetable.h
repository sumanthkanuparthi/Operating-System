#ifndef PAGETABLE_H
#define PAGETABLE_H

#include<defs.h>
#define PR 0x1        //Present
#define RW 0x2       //Read-0/Write-1
#define US 0x4       //User -1/Supervisor -0
#define PWT 0x8      //PageWriteThrough
#define PCD 0x10     //PageCahceDisable
#define A 0x20       //Accessed
#define DI 0x40       //Dirty
#define PAT 0x80     //PageAttributeTable
#define G 0x100      //Global
#define NX 0x8000000000000 //NoExecute

struct PML4{
      uint64_t  Entries[512];
};

struct PDPT{
      uint64_t Entries[512];
};

struct PDT{
      uint64_t Entries[512];
};

struct PT{
      uint64_t Entries[512];
};

 void setupPageTables(uint64_t physbase,uint64_t physfree);
 void setupcr3();
 void physical_address_mapping();
 void* AllocateVirtualMemory();
 uint64_t getCr3Value();
void* AssignPhysicalToVirtual(uint64_t virtAddr,int pages,int flags);
 void* SetUpMemoryForNewProcess();
 void SetCr3Value(struct PML4 * current);
 void* CopyPageTables();
 uint64_t GetMappedPhysicalAddress(uint64_t virtAddr);
 void map_page(void * physaddr, void * virtualaddr, unsigned int flags);
#endif

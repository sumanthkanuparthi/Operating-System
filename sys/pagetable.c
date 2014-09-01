#include "pagetable.h"
#include<sys/pmap.h>
#include "stdio.h"

#define UFLAGS 1|2|4
#define KFLAGS 1|2
extern uint64_t DISPLAY;
uint64_t COW =  0x10000000000000UL;
uint64_t ReadOnly = ~RW; 
 

uint16_t GetPTIndex(uint64_t VAddress)
{
      return VAddress >> 12 & 0x1FF;
}

uint16_t GetPDIndex(uint64_t VAddress)
{
      return VAddress >> 21 & 0x1FF;
}

uint16_t GetPDPIndex(uint64_t VAddress)
{
      return VAddress >> 30 & 0x1FF;
}

uint16_t GetPML4Index(uint64_t VAddress)
{
      return VAddress >> 39 & 0x1FF;
}
struct PML4 *pml4;

extern char kernmem;
/*void setupPageTables(uint64_t physbase,uint64_t physfree)
    {
      pml4 = (struct PML4 *)getFreePage();
       struct PDPT *pdpt = (struct PDPT *)getFreePage();
       struct PDT *pdt = (struct PDT *)getFreePage();
       struct PT *pt = (struct PT *)getFreePage();
       pml4->Entries[GetPML4Index((uint64_t)&kernmem)] = ((uint64_t)pdpt)|PR|RW|US;
       pdpt->Entries[GetPDPIndex((uint64_t)&kernmem)] = ((uint64_t)pdt)|PR|RW|US;
       pdt->Entries[GetPDIndex((uint64_t)&kernmem)] = ((uint64_t)pt)|PR|RW|US;
       uint64_t virAddr= (uint64_t)&kernmem;
        uint64_t virAddrStart = 0xfffffffff0000000 & virAddr;
        uint64_t tempStart = physbase;

        while(tempStart < (physfree+0x2000))
          {
            uint64_t tempVirt = virAddrStart | tempStart;
            pt->Entries[GetPTIndex(tempVirt)] = tempStart | PR | RW | US;
            tempStart+=0x1000;

          }
        //Mapping video memory
        DISPLAY = virAddrStart | tempStart;
        pt->Entries[GetPTIndex(DISPLAY)] = 0xB8000 | PR | RW | US;
      printf("PT entry index is %d",GetPTIndex(DISPLAY));
    }
*/

void setupPageTables(uint64_t physbase,uint64_t physfree)
{
  pml4 = (struct PML4 *)getFreePage();
   uint64_t virAddr= (uint64_t)&kernmem;
  uint64_t virAddrStart = 0xfffffffff0000000 & virAddr;
  uint64_t tempStart = physbase; 
  while(tempStart < (physfree+0x2000))
  {
    uint64_t tempVirt = virAddrStart | tempStart;
    map_page((uint64_t*)tempStart,(uint64_t*)tempVirt,3);
    tempStart+=0x1000;
  }
  DISPLAY = virAddrStart | tempStart;
 map_page((uint64_t*)0xB8000,(uint64_t*)DISPLAY,3);
}

void setupcr3()
{
  uint64_t pml4addr =(uint64_t)pml4;
 __asm volatile("mov %0, %%cr3":: "b"(pml4addr));
}
void SetCr3Value(struct PML4 * current)
{
  uint64_t pml4addr =(uint64_t)current;
    pml4 = current;
  __asm volatile("mov %0, %%cr3":: "b"(pml4addr));
}

uint64_t getCr3Value()
{
    uint64_t cr3Value;
    __asm volatile("mov %%cr3, %0" : "=r" (cr3Value));
    return cr3Value;
}
 extern uint64_t virtualBaseAddr;

void map_page(void * physaddr, void * virtualaddr, unsigned int flags)
{
    	   struct PML4 * current = pml4;
          unsigned long pml4index = GetPML4Index((uint64_t)virtualaddr);
          unsigned long pdpindex = GetPDPIndex((uint64_t)virtualaddr);
          unsigned long pdindex = GetPDIndex((uint64_t)virtualaddr);
          unsigned long ptindex = GetPTIndex((uint64_t)virtualaddr);
          current = (struct PML4*)(virtualBaseAddr|(uint64_t)current );
          uint64_t pml4entry = current ->Entries[pml4index];       
          struct PDPT *pdpt;
          if ((pml4entry & PR)!=PR)
          {
            pdpt = (struct PDPT*)getFreePage();
            //pml4->Entries[pml4index] = ((uint64_t)pdpt)|PR|RW|US;
            current ->Entries[pml4index] = ((uint64_t)pdpt)|flags;
          } 
          else
          {
            pdpt = (struct PDPT *)(pml4entry & 0xFFFFFFFFFFFFF000);
          }
          
          pdpt = (struct PDPT*)(virtualBaseAddr+(uint64_t)pdpt);
            uint64_t pdpentry = pdpt->Entries[pdpindex];
          struct PDT *pdt;
          if ((pdpentry & PR)!=PR)
           {
              pdt = (struct PDT*)getFreePage();
              //pdpt->Entries[pdpindex] = ((uint64_t)pdt)|PR|RW|US;
              pdpt->Entries[pdpindex] = ((uint64_t)pdt)|flags;
           } 
           else
           {
              pdt = (struct PDT *)(pdpentry & 0xFFFFFFFFFFFFF000);
           }

          pdt = (struct PDT*)(virtualBaseAddr+(uint64_t)pdt);
          
           uint64_t pdtentry = pdt->Entries[pdindex];
           struct PT *pt;
           if((pdtentry & PR)!=PR)
           {
             pt = (struct PT *)getFreePage();
             //pdt->Entries[pdindex] = ((uint64_t)pt)|PR|RW|US;
             pdt->Entries[pdindex] = ((uint64_t)pt)|flags;
           }
           else
           {
             pt = (struct PT *)(pdtentry & 0xFFFFFFFFFFFFF000);
           }

          pt = (struct PT*)(virtualBaseAddr+(uint64_t)pt);
           //pt->Entries[ptindex] = (uint64_t)physaddr|PR|RW|US;
           pt->Entries[ptindex] = (uint64_t)physaddr|flags;

}
extern uint64_t physical_memory_size;
extern char * bitMap;
//extern uint64_t virtualBaseAddr;


void physical_address_mapping()
{
  uint64_t virtualaddr = 0xFFFFFFFFF0000000UL;
  uint64_t physaddr =0x0UL;
  while(physaddr<=physical_memory_size)
  {
    map_page((uint64_t*)physaddr,(uint64_t*)virtualaddr,3);
    physaddr = physaddr+0x1000UL;
    virtualaddr = virtualaddr+0x1000UL;
  }
virtualBaseAddr = 0xFFFFFFFFF0000000UL;
bitMap =(char*)(0xFFFFFFFF80000000UL|(uint64_t)bitMap);

}






uint64_t lastUsedAddress = 0xFFFFFFFFFA000000;
void* AllocateVirtualMemory()
{
    uint64_t phyPage =  (uint64_t)getFreePage();
    lastUsedAddress += 0x1000;
        
    struct PML4 *newpml4 = (struct PML4 *)getCr3Value();
     pml4 = newpml4;
          map_page((void*)phyPage,(void*)lastUsedAddress-0x1000,3);
          return (void*)(lastUsedAddress-0x1000);
}

//Setting up PageTables for a new Process
void* SetUpMemoryForNewProcess()
{
    struct PML4 *pml4New = (struct PML4 *) getFreePage();
      
    struct PML4 *pml4Current = (struct PML4 *)getCr3Value();
        
        //Mapping Kernel & Identity Mapping

    uint64_t newpml4 = (uint64_t)pml4New + virtualBaseAddr;
    uint64_t prevpml4 = (uint64_t)pml4Current + virtualBaseAddr;
    ((struct PML4 *)(newpml4))->Entries[511] = ((struct PML4 *)(prevpml4))->Entries[511];
    //((struct PML4 *)((uint64_t)pml4New |virtualBaseAddr))->Entries[511] = ((struct PML4 *)((uint64_t)pml4Current |virtualBaseAddr))->Entries[511];
    return (void*)pml4New ;
}


// Mapping Particular VirtualAddress to Physical Page which is aquired from getfreepage()

void* AssignPhysicalToVirtual(uint64_t virtAddr,int pages,int flags)
{
	int cnt=0;

	if(flags == -1)
		flags = UFLAGS;

	uint64_t temp = virtAddr;
	for(cnt =0;cnt<pages;cnt++)
	{
		uint64_t phyPage =  (uint64_t)getFreePage();
		map_page((void*)phyPage,(void *)temp,flags);
		temp +=0x1000;
	}
	return (void*)(virtAddr);
} 



extern char kernmem;

void* CopyPageTables()
{
	struct PML4 *pml4New = (struct PML4 *)getFreePage();
	
	struct PML4 *pml4Current = (struct PML4 *)getCr3Value();
	

	uint64_t virAddrStart = (long)&kernmem;

	uint64_t pml4UserMax = GetPML4Index(virAddrStart);

  uint64_t base = virtualBaseAddr;
	int pmlIndex =0;
	//below the kernel all needs to be copied
	for(pmlIndex=0;pmlIndex<512;pmlIndex++)
	{
		if(pmlIndex >= pml4UserMax) //part of kernel , clone...
		{
			((struct PML4 *)((uint64_t)pml4New |base))->Entries[pmlIndex] = ((struct PML4 *)((uint64_t)pml4Current |base))->Entries[pmlIndex];
			continue;
		}
		
		
		uint64_t pdpBaseAddr = ((struct PML4 *)((uint64_t)pml4Current |base))->Entries[pmlIndex];

		if(( pdpBaseAddr & PR)  == PR)
		{
			struct PDPT *pdpNew = (struct PDPT *)getFreePage();
			((struct PML4 *)((uint64_t)pml4New |base))->Entries[pmlIndex] = (uint64_t)((uint64_t)pdpNew| UFLAGS);
			
			struct PDPT *pdpCurrent = (struct PDPT *)( pdpBaseAddr & 0xFFFFFFFFFFFFF000);

			int pdpIndex = 0;
			for(pdpIndex =0;pdpIndex <512;pdpIndex ++)
			{
				uint64_t pdBaseAddr =   ((struct PDPT *)((uint64_t)pdpCurrent |base))->Entries[pdpIndex];

				if((pdBaseAddr & PR)  == PR)
				{
					struct PDT *pdNew = (struct PDT *) getFreePage();
					((struct PDPT *)((uint64_t)pdpNew |base))->Entries[pdpIndex] = (uint64_t)((uint64_t)pdNew | UFLAGS);
			
					struct PDT *pdCurrent = (struct PDT *)( pdBaseAddr & 0xFFFFFFFFFFFFF000);
					
					int pdIndex = 0;
					for(pdIndex =0;pdIndex <512;pdIndex ++)
					{
						uint64_t ptBaseAddr =   ((struct PDT *)((uint64_t)pdCurrent |base))->Entries[pdIndex ];

						if((ptBaseAddr & PR)  == PR)
						{
							struct PT *ptNew = (struct PT *) getFreePage();
							((struct PDT *)((uint64_t)pdNew |base))->Entries[pdIndex] = (uint64_t)((uint64_t)ptNew | UFLAGS);
			
							struct PT *ptCurrent = (struct PT *)( ptBaseAddr & 0xFFFFFFFFFFFFF000);

							int ptIndex = 0;
							for(ptIndex =0;ptIndex <512;ptIndex ++)
							{
								uint64_t pageAddr =   ((struct PT *)((uint64_t)ptCurrent |base))->Entries[ptIndex];

								ptNew = ((struct PT *)((uint64_t)ptNew |base));
								ptCurrent = ((struct PT *)((uint64_t)ptCurrent |base));


								if((pageAddr & PR)  == PR)
								{

									//setting COW bit & read only bit	
									pageAddr  = ((pageAddr | COW) & ReadOnly );

									//same physical address the new PT
									ptNew->Entries[ptIndex] = pageAddr;
									
									//updating address in old PT
									ptCurrent->Entries[ptIndex] = pageAddr;	

									//Need to add ref count code			
			
								}		
							}							
			
						}		
					}
			
				}		
			}
		}		
	}

	return (void*)pml4New ;
}



uint64_t GetMappedPhysicalAddress(uint64_t virtAddr)
{
    struct PML4 *pml4= (struct PML4 *)getCr3Value();
    uint64_t physAddr = 0;
    struct PDPT *pdp = NULL;
    struct PDT *pd = NULL;
    struct PT *pt = NULL;
    uint64_t base = virtualBaseAddr;              
    uint64_t pdpBaseAddr =   ((struct PML4 *)((uint64_t)pml4|base))->Entries[GetPML4Index(virtAddr)];
    if((pdpBaseAddr & PR) == PR)
      {
        pdp =(struct PDPT *)( pdpBaseAddr & 0xFFFFFFFFFFFFF000);
      }
      pdp = ((struct PDPT *)((uint64_t)pdp |base));
      uint64_t pdBaseAddr =   pdp->Entries[GetPDPIndex(virtAddr)];
      if((pdBaseAddr & PR) == PR)
      {
       pd =(struct PDT *)( pdBaseAddr & 0xFFFFFFFFFFFFF000);
      }
                        
      pd = ((struct PDT *)((uint64_t)pd |base));
      uint64_t ptBaseAddr =   pd->Entries[GetPDIndex(virtAddr)];
      if((ptBaseAddr & PR) == PR)
      {
      pt = (struct PT *)(ptBaseAddr & 0xFFFFFFFFFFFFF000);
      }
      if(pt  != NULL)
      {
       pt = ((struct PT *)((uint64_t)pt |base));
       physAddr = pt->Entries[GetPTIndex(virtAddr)]& 0xFFFFFFFFFFFFF000;
      }
      return physAddr ;
}



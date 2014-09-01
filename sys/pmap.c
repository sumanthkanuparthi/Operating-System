#include "stdio.h"
#include <defs.h>
#include <sys/pmap.h>


#define AVAILABLE 1
#define NOTAVAILABLE 0
#define MAXSIZECHARARRAY 8192

uint64_t physfree, physbase;
uint64_t virtualBaseAddr = 0x0;
char * bitMap;
      
void init_freeList(uint64_t physbase1, uint64_t physfree1) {
  physfree = physfree1;
  physbase = physbase1;
  bitMap = (char*)physfree;
  //bitMap = p;
  int i = 0;
  for(i = 0;i<MAXSIZECHARARRAY;i++){
    bitMap[i] = NOTAVAILABLE;
    }
  physfree += (int)(MAXSIZECHARARRAY/8);
  }


int isValid(uint64_t address){
  if(address < physfree){
    return 0;
  }
  return 1;
}

void addFreePages(uint64_t start, uint64_t end){
  int startingIndex = start/0x1000;
  int endingIndex = end/0x1000;
  int i = 0;

  for(i=startingIndex;i<endingIndex;i++){
    if(isValid(i*0x1000)){
       bitMap[(int)(i/8)] |=  1<<(i%8) ;   //Marking bit of page entry as available.
    }
  }
  
}
void * memset(void *,int,uint64_t);
void* getFreePage(){
  int i = 0;
// bitMap = (char*)((uint64_t)bitMap|virtualBaseAddr);
  while(!bitMap[i]) i++;
  char _bit_masks[] = {0x01, 0x02, 0x04, 0x08,  0x10, 0x20, 0x40, 0x80};
  int j = 0;
  for(j=0;j<8;j++)
    if(bitMap[i] & _bit_masks[j]){
            bitMap[i] &= ~(1<<j);
            uint64_t address = (i*0x1000*8)+0x1000*j;
            //address = virtualBaseAddr|address;
            memset((uint64_t *)(virtualBaseAddr|address),0,4096);
            return (void *)address;
    }
  return NULL;
}


void unFreePage(uint64_t pageAddress){
  int index = pageAddress/0x1000;
  bitMap[(int)(index/8)] &= ~1<<(index%8) ;
}

void * kmalloc()
{
uint64_t addr =(uint64_t )getFreePage();
addr = virtualBaseAddr|addr;
memset((uint64_t *)addr,0,4096);
return (void *)(addr);
}



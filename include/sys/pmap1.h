#ifndef _PMAP_H
#define _PMAP_H
#include <defs.h>
struct PageNode
{
  uint32_t address;
  struct PageNode *nextFree;
};
void CreateFreeLists(uint32_t,uint32_t,uint32_t*);
void print(); 
#endif
              

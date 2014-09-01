


#ifndef _PMAP_H
#define _PMAP_H

void init_freeList(uint64_t,uint64_t);

void addFreePages(uint64_t start, uint64_t end);

void unFreePage(uint64_t base);
void* getFreePage();
void* kmalloc();
#endif



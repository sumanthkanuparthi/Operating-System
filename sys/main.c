#include <defs.h>
#include <sys/gdt.h>
#include <sys/idt.h>
#include <sys/pic.h>
#include <sys/timer.h>
#include "stdio.h"
#include <sys/pmap.h>
#include "pagetable.h"
#include <sys/pagefault.h>
#include <sys/tarfs.h>   
#include <sys/dirent.h>
#include <sys/string.h>
#include <sys/file.h>
#include <sys/contextswitch.h>
uint64_t physical_memory_size;
extern struct PML4 *pml4;
extern char* buffer;

void start(uint32_t* modulep, void* physbase, void* physfree)
{
  clrscr();
	struct smap_t {
		uint64_t base, length;
		uint32_t type;
	}__attribute__((packed)) *smap;
	while(modulep[0] != 0x9001) modulep += modulep[1]+2;
  init_freeList((uint64_t)physbase,(uint64_t)physfree);
	for(smap = (struct smap_t*)(modulep+2); smap < (struct smap_t*)((char*)modulep+modulep[1]+2*4); ++smap) {
		if (smap->type == 1 /* memory */ && smap->length != 0) {
      addFreePages(smap->base,smap->base + smap->length );
      printf("Available Physical Memory [%x-%x]\n", smap->base, smap->base + smap->length);
      physical_memory_size = smap->base+smap->length; 
    }
	}
  setupPageTables((uint64_t)physbase,(uint64_t)physfree);
  physical_address_mapping();
  setupcr3();
	init_fdlist(); //Allocated space for fdtable. Must Run
	
  printf("The modules, physfree and physbase are %x, %x, %x\n",modulep ,physfree,physbase);
  //To create PageFault
/*uint64_t addr = ((uint64_t)pml4);
struct PML4  *test =(struct PML4*)((0xFFFFFFFFF0000000|(addr)));
uint64_t entry = test->Entries[511];
printf("entry:%p",entry);
*/
//Testing whther Identity mapping is working or not
//char *some = (char*)0xFFFFFFFFF0080000;
//printf("\nAddr:%p",some);
//*some ='a';
//printf("\nValue:%c",*some);
/*int a=0;
__asm volatile( "movq %0, %%rdi\n\t"
                "movq %1, %%rsi\n\t"
                "movq %2, %%rdx\n\t"
                "movq %3, %%rcx\n\t"
                "movq %4, %%r8\n\t"
                "movq %5, %%r9\n\t"
                "movq %6, %%rax\n\t"
                : : "m"(a),"m"(b),"m"(c),"m"(d),"m"(e),"m"(f),"m"(g) : "memory" );

__asm volatile( "movq %0, %%rax\n\t"::"m"(a):"memory");
__asm volatile("INT $0x80");

*/
//printf("enter any number");
//printf("wait");
  startingPoint();

printf("tarfs in [%p:%p]\n", &_binary_tarfs_start, &_binary_tarfs_end);
// kernel starts here
	while(1);
}

#define INITIAL_STACK_SIZE 4096
char stack[INITIAL_STACK_SIZE];
uint32_t* loader_stack;
extern char kernmem, physbase;
 struct tss_t tss;  

void boot(void)
{
	// note: function changes rsp, local stack variables can't be practically used
	//register char *temp1, *temp2;
	__asm__(
		"movq %%rsp, %0;"
		"movq %1, %%rsp;"
		:"=g"(loader_stack)
		:"r"(&stack[INITIAL_STACK_SIZE])
	);
  __asm volatile("cli");
	reload_gdt();
	setup_tss();
  PIC_remap(32,40);
  reload_idt();
  timer_phase(500);
  unmask_IRQs();
  IRQ_clear_mask(0);
  IRQ_clear_mask(1);
  __asm volatile("sti");
  
  //printf("kern: %p,%x",&kernmem,&kernmem);
	start(
		(uint32_t*)((char*)(uint64_t)loader_stack[3] + (uint64_t)&kernmem - (uint64_t)&physbase),
		&physbase,
		(void*)(uint64_t)loader_stack[4]
	);
/*	for(
		temp1 = "!!!!! start() returned !!!!!", temp2 = (char*)0xb8000;
		*temp1;
		temp1 += 1, temp2 += 2
	) *temp2 = *temp1;*/
	while(1);
}

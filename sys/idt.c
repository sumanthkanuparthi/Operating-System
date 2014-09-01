//copyright (C) 2013 by Sumanth Kumar and Jitender Kalra. This work is licensed under GPLv3, details in accompanying GPL/LICENSE file.



#include <sys/idt.h>
#include "stdio.h"

void idt_set_gate(uint16_t,uint64_t,uint16_t,int dpl);
idt_entry_t idt_entries[256];
static idt_ptr_t   idt_ptr;


void idt_set_gate(uint16_t num, uint64_t base, uint16_t sel,int dpl)
{
idt_entries[num].target_offset1 = (base & 0xFFFF);
idt_entries[num].target_offset2 = (base >> 16) & 0xFFFF;
idt_entries[num].target_offset3 = (base >> 32) & 0xFFFFFFFF;    
idt_entries[num].target_selector = sel;
idt_entries[num].res=0;
idt_entries[num].zero1=0;
idt_entries[num].type=0xE;
idt_entries[num].storage=0;
idt_entries[num].dpl=dpl;
idt_entries[num].p=1;      
idt_entries[num].reserved = 0; 
}

extern void _x86_64_asm_lidt(idt_ptr_t * idt_ptr);
extern void _isr0();
extern void isr1();
extern void _isr2();
extern void _isr13();
extern void _isr14();

extern void _isr128();
void reload_idt()
 {
   idt_ptr.limit = sizeof(idt_entries);
   idt_ptr.base  = (uint64_t)idt_entries;

   idt_set_gate(0, (uint64_t)&isr1, 0x08,0);
   idt_set_gate(1, (uint64_t)&isr1, 0x08,0);     
   idt_set_gate(2, (uint64_t)&isr1, 0x08,0);
   idt_set_gate(3, (uint64_t)&isr1, 0x08,0);
   idt_set_gate(4, (uint64_t)&isr1, 0x08,0);
   idt_set_gate(5, (uint64_t)&isr1, 0x08,0);
   idt_set_gate(6, (uint64_t)&isr1, 0x08,0);
   idt_set_gate(7, (uint64_t)&isr1, 0x08,0);
   idt_set_gate(8, (uint64_t)&isr1, 0x08,0);
   idt_set_gate(9, (uint64_t)&isr1, 0x08,0);
   idt_set_gate(10, (uint64_t)&isr1, 0x08,0);
   idt_set_gate(11, (uint64_t)&isr1, 0x08,0);
   idt_set_gate(12, (uint64_t)&isr1, 0x08,0);
   idt_set_gate(13, (uint64_t)&_isr13, 0x08,0);

   idt_set_gate(14, (uint64_t)&_isr14, 0x08,0);

   idt_set_gate(15, (uint64_t)&isr1, 0x08,0);
   idt_set_gate(16, (uint64_t)&isr1, 0x08,0);
   idt_set_gate(17, (uint64_t)&isr1, 0x08,0);
   idt_set_gate(18, (uint64_t)&isr1, 0x08,0);
   idt_set_gate(19, (uint64_t)&isr1, 0x08,0);
   idt_set_gate(20, (uint64_t)&isr1, 0x08,0);
   idt_set_gate(21, (uint64_t)&isr1, 0x08,0);
   idt_set_gate(22, (uint64_t)&isr1, 0x08,0);
   idt_set_gate(23, (uint64_t)&isr1, 0x08,0);
   idt_set_gate(24, (uint64_t)&isr1, 0x08,0);
   idt_set_gate(25, (uint64_t)&isr1, 0x08,0);
   idt_set_gate(26, (uint64_t)&isr1, 0x08,0);
   idt_set_gate(27, (uint64_t)&isr1, 0x08,0);
   idt_set_gate(28, (uint64_t)&isr1, 0x08,0);
   idt_set_gate(29, (uint64_t)&isr1, 0x08,0);
   idt_set_gate(30, (uint64_t)&isr1, 0x08,0);
   idt_set_gate(31, (uint64_t)&isr1, 0x08,0);


   idt_set_gate(32, (uint64_t)&_isr0, 0x08,0);
   idt_set_gate(33, (uint64_t)&_isr2, 0x08,0);
   
   idt_set_gate(128, (uint64_t)&_isr128,0x08,3);
    _x86_64_asm_lidt(&idt_ptr);
    
 }

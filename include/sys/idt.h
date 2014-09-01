//Copyright (C)  2013 by Sumanth Kumar and Jitender Kalra. This work is licensed under GPLv3, details in accompanying COPYING file.
#ifndef _IDT_H
#define _IDT_H
#include <defs.h>


struct idt_entry_struct{
  uint16_t target_offset1;
  uint16_t target_selector;
  unsigned res :3;
  unsigned zero1 :5;
  unsigned type :4;
  unsigned storage :1;
  unsigned dpl :2;
  unsigned p :1;
  uint16_t target_offset2;
  uint32_t target_offset3;
  uint32_t reserved;
} __attribute__((packed));
typedef struct idt_entry_struct idt_entry_t;

struct idt_ptr_struct
{
  uint16_t limit;
  uint64_t base;
} __attribute__((packed));
typedef struct idt_ptr_struct idt_ptr_t;

extern void reload_idt();

#endif

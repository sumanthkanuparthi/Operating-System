//copyright (C) 2013 by Sumanth Kumar and Jitender Kalra. This work is licensed under GPLv3, details in accompanying GPL/LICENSE file.

#ifndef _STDIO_H
#define _STDIO_H
#include <defs.h>
#define va_start(v,l)     __builtin_va_start(v,l)
#define va_arg(v,l)       __builtin_va_arg(v,l)
#define va_end(v)         __builtin_va_end(v)
#define va_copy(d,s)      __builtin_va_copy(d,s)
typedef __builtin_va_list va_list;

void io_wait(void);
unsigned char inb( unsigned short);
void outb( unsigned short, unsigned char);
void Putc(char);
int Puts(char*);
void itoa(unsigned long,int,char*);
int printf(const char*,...);
void update_cursor(int, int);
void clrscr();
 void * memset(void* , int, uint64_t);
#endif

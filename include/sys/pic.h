//Copyright (C)  2013 by Sumanth Kumar and Jitender Kalra. This work is licensed under GPLv3, details in accompanying COPYING file.
#ifndef PIC_H
#define PIC_H

#include<defs.h>
void  PIC_remap(int,int);
void  IRQ_clear_mask(unsigned char);
void unmask_IRQs();

#endif

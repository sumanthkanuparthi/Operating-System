//copyright (C) 2013 by Sumanth Kumar and Jitender Kalra. This work is licensed under GPLv3, details in accompanying GPL/LICENSE file.

#include "stdio.h"
#include <sys/idt.h>
#include <sys/timer.h>
#include <sys/contextswitch.h>
int i=0;
char *s ="";
int row=0,col = 0;
int hertz = 0;
int minutes,seconds,hours;
uint32_t g_seconds;
extern uint64_t DISPLAY;
extern task_struct *currentTask;
void putc(char c)
{
       char* p = (char*)DISPLAY + (col++)*2 + row * 80*2;
        *p++ = c;
        *p =0x0F;
}

void displaytime(char *str)
{
if(!*str) {
         return;
      }
      while(*str) {
          putc(*str);
          str++;
      }
        //int len = sizeof(str);
        //printf("<%d>",len);
}

void timer()
{
if(i++%hertz == 0)
  { 
    seconds++;
    g_seconds =seconds;
  }
if(seconds>0 && seconds%60==0)
{
        minutes++;
        seconds=0;
        if(minutes>0 && minutes%60==0){
                hours++;
                minutes=0;
                if(hours>0 && hours%24==0){
                        hours =0;
                        seconds=0;
                        minutes=0;
                }
        }
}
char time[8] = {'\0'};
time[0] = hours/10%10 + '0';
time[1] = hours%10 + '0';
time[2] = ':';
time[3] = minutes/10%10 + '0';
time[4] = minutes%10 + '0';
time[5] = ':';
time[6] = seconds/10%10 + '0';
time[7] = seconds%10 + '0';
time[8] = '\0';
row=24;
col=70;
displaytime(time);
outb(0x20,0x20);

if(currentTask != NULL)
    {
      schedule();   
    }
}


void timer_phase(int hz)
{
    int divisor = 1193180 / hz;       /* Calculate our divisor */
    outb(0x43, 0x36);             /* Set our command byte 0x36 */
    outb(0x40, divisor & 0xFF);   /* Set low byte of divisor */
    outb(0x40, divisor >> 8);     /* Set high byte of divisor */
    hertz = hz;
}


// Shift this code to common handler
void dividezero()
{
printf("Dividezero Exception");
while(1);
}

void gpf()
{
  printf("General Protection Fault");
  while(1);
}


// copyright (C) 2013 by Sumanth Kumar and Jitender Kalra. This work is licensed under GPLv3, details in accompanying GPL/LICENSE file.

#include "stdio.h"
#include <defs.h>
//#define DISPLAY   0xB8000
int xPos=0, yPos=0;
int color=0x0F;

uint64_t  DISPLAY = 0xB8000;
 void * memset(void *baseaddr, int c, uint64_t size)
 {
     unsigned char *us = baseaddr;
     while (size-- != 0)
         *us++ = (unsigned char)c;
     return baseaddr;
 }


void update_cursor(int row, int col)
 {
    unsigned short position=(row*80) + col;

    // cursor LOW port to vga INDEX register
    outb(0x3D4, 0x0F);
    outb(0x3D5, (unsigned char)(position&0xFF));
    // cursor HIGH port to vga INDEX register
    outb(0x3D4, 0x0E);
    outb(0x3D5, (unsigned char )((position>>8)&0xFF));
 }


void clrscr()
{
   int i=0;
   char blank = 0x20;
   xPos =0;
   yPos =0;
   for(i=0;i<80*25;i++)
    Putc(blank);
   xPos = 0;
   yPos = 0;
}



void outb( unsigned short port, unsigned char val )
{
    __asm volatile( "outb %0, %1"
                  : : "a"(val), "Nd"(port) );
}

unsigned char inb( unsigned short port )
{
    unsigned char ret;
    __asm volatile( "inb %1, %0"
                  : "=a"(ret) : "Nd"(port) );
    return ret;
}

// Scrolls the text on the screen up by one line.
void scroll()
{

   // Get a space character with the default colour attributes.
   //uint8_t attributeByte = (0 /*black*/ << 4) | (15 /*white*/ & color);
   //uint16_t blank = 0x20 /* space */ | (attributeByte << 8);
   // Row 25 is the end, this means we need to scroll up
   //int position = xPos * yPos;
   if(yPos  > 23)
   {

       // Move the current text chunk that makes up the screen
       // back in the buffer by a line
        char* p = (char*)DISPLAY;
       int i;
       for (i = 0; i < 23*80*2; i++)
       {
           *(p+i) = *(p+i+160);
       }

       // The last line should now be blank. Do this by writing
       // 80 spaces to it.
	   
       for (i = (23*80*2); i < 24*80*2; i+=2)
       {		
				
                *(p+i) = ' ';
              *(p+i+1) = color ;
       }
       // The cursor should now be on the last line.
       yPos = 23;
   }
}


void Putc(char c)
{

        if (c == '\n'||c=='\r') {   /* Handling New line and Carriage return */
                yPos+=1;
                xPos=0;
				scroll();
                update_cursor(yPos,xPos);
                return;
        }
        //Handling Backspace
        if(c == '\b' && (xPos!=0 || yPos!=0))
        {
			xPos-=1;
			char* p = (char*)DISPLAY + (xPos)*2 + yPos * 160;
			*p++ = ' ';
			*p =color;
			update_cursor(yPos,xPos);
			return;
        }


        if (xPos > 79) {             /*Setting Xpos back to 0 after the current row is finished  */
                yPos+=1;
                xPos=0;
                scroll();
        }

        /* Put the character to Display memory*/
        char* p = (char*)DISPLAY + (xPos++)*2 + yPos * 160;
        *p++ = c;
        *p =color;
update_cursor(yPos,xPos);
scroll();
}
void puts(char *str)
{
if(!*str) {
         return;
      }
      while(*str) {
          Putc(*str);
          str++;
      }
Putc('\n');
}

int Puts(char *str)
{
     int count =0;
      if(!*str) {
         return 0;
      }
      while(*str) {
          Putc(*str);
          str++;
          count++;
      }
     return count;
}
char tbuf[64];
char bchars[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
void itoa(unsigned long i,int base,char* buf) {
    if (base > 16) return;
   if (i < 0) {
      *buf++ = '-';
      i *= -1;
   }
    int pos = 0;
   int opos = 0;
   int top = 0;

   if (i == 0) {
      buf[0] = '0';
      buf[1] = '\0';
      return;
   }

   while (i != 0) {
      tbuf[pos] = bchars[i % base];
      pos++;
      i /= base;
   }
   top=pos--;
   int start =0;
   if (base ==16)
    {
    start = 2;
     buf[0] = '0';
     buf[1] = 'X'; 
    }
 
   for (opos=start; opos<top+start; pos--,opos++) {
      buf[opos] = tbuf[pos];
   }
   buf[opos] = 0;
}

int printf(const char* str,...)
{
va_list args;
va_start(args,str);
int NChars=0;
while(*str)
{
    if(*str == '%')
       {
        str++;
        int i=0;
       unsigned long l=0l;
        switch(*str++)
        {
         case 'c': Putc((char)va_arg(args,int));
                   NChars++;
                   break;
         case 'd':i = va_arg(args,int);
                  char str[32]={0};
                  itoa(i,10,str);
                  NChars+=Puts(str); 
                  break;
         case 'x':i = va_arg (args, int);
	          char str1[32]={0};
		  itoa(i,16,str1);
		  NChars+=Puts(str1);
		  break;
         case 's':NChars+=Puts(va_arg(args,char*));
                  break;
         case 'p':
		l = va_arg (args,long);
		char str3[64]={0};
		itoa(l,16,str3);
		NChars+=Puts(str3);
		break;
         default: break;
        }
       }
     else
     {
       Putc(*str++);
       NChars++;
     } 
}
va_end(args);
return NChars;
}

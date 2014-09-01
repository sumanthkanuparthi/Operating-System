//copyright (C) 2013 by Sumanth Kumar and Jitender Kalra. This work is licensed under GPLv3, details in accompanying GPL/LICENSE file.
#include <defs.h>
#include "stdio.h"
int shift=0;
extern uint64_t DISPLAY;
char kbdus[128] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',     /* 9 */
  '9', '0', '-', '=', '\b',     /* Backspace */
  '\t',                 /* Tab */
  'q', 'w', 'e', 'r',   /* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', /* Enter key */
    0,                  /* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',     /* 39 */
 '\'', '`',   0,                /* Left shift */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',                    /* 49 */
  'm', ',', '.', '/',   0,                              /* Right shift */
  '*',
    0,  /* Alt */
  ' ',  /* Space bar */
    0,  /* Caps lock */
    0,  /* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,  /* < ... F10 */
    0,  /* 69 - Num lock*/
    0,  /* Scroll Lock */
    0,  /* Home key */
    0,  /* Up Arrow */
    0,  /* Page Up */
  '-',
    0,  /* Left Arrow */
    0,
    0,  /* Right Arrow */
  '+',
    0,  /* 79 - End key*/
    0,  /* Down Arrow */
    0,  /* Page Down */
    0,  /* Insert Key */
    0,  /* Delete Key */
    0,   0,   0,
    0,  /* F11 Key */
    0,  /* F12 Key */
    0,  /* All other keys are undefined */
};

 char upperCaseKbdus[128] =
{
  0,  27, '!', '@', '#', '$', '%', '^', '&', '*',	/* 9 */
  '(', ')', '_', '+', '\b',	/* Backspace */
  '\t',			/* Tab */
  'Q', 'W', 'E', 'R',	/* 19 */
  'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',		/* Enter key */
  0,			/* 29   - Control */
  'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':',	/* 39 */
  '\"', '~',   0,		/* Left shift */
  '|', 'Z', 'X', 'C', 'V', 'B', 'N',			/* 49 */
  'M', '<', '>', '?',   0,					/* Right shift */
  '*',
  0,	/* Alt */
  ' ',	/* Space bar */
  0,	/* Caps lock */
  0,	/* 59 - F1 key ... > */
  0,   0,   0,   0,   0,   0,   0,   0,
  0,	/* < ... F10 */
  0,	/* 69 - Num lock*/
  0,	/* Scroll Lock */
  0,	/* Home key */
  0,	/* Up Arrow */
  0,	/* Page Up */
  '-',
  0,	/* Left Arrow */
  0,
  0,	/* Right Arrow */
  '+',
  0,	/* 79 - End key*/
  0,	/* Down Arrow */
  0,	/* Page Down */
  0,	/* Insert Key */
  0,	/* Delete Key */
  0,   0,   0,
  0,	/* F11 Key */
  0,	/* F12 Key */
  0,	/* All other keys are undefined */
};

//scanf code
char buf[256]="\0";
char *buffer = buf;
int8_t isNewline =0;
void keyboard()
{
unsigned char scancode;
scancode = inb(0x60);
if (scancode & 0x80)
{
  if ((scancode-0x80 == 42)||(scancode-0x80==54))
  shift = 0;
}
else
{
  if ((scancode== 42)||(scancode==54))
   {
     shift = 1;
outb(0x20,0x20);
     return;
   }
   
   

  if(shift == 0)
  {
     char* p = (char*)DISPLAY + 67*2 + (24 * 2 * 80);
     *p++ = kbdus[scancode];
	 	if(kbdus[scancode]== '\b' && (buffer - buf) > 0 )
		{
			*(--buffer) = '\0';
			outb(0x20,0x20);
			Putc(kbdus[scancode]);
			return;
		}
		 
		*buffer++ = kbdus[scancode];
		Putc(kbdus[scancode]);
      if(kbdus[scancode]=='\n')
      isNewline = 1;
      *p =0x0F;
    // }
     }
  else
  {
    char* p = (char*)DISPLAY + 67*2 + (24 * 2 * 80);
    *p++ = upperCaseKbdus[scancode];
    
     if(upperCaseKbdus[scancode]== '\b' && (buffer - buf) > 0 )
        {
              *(--buffer) = '\0';
              outb(0x20,0x20);
              Putc(upperCaseKbdus[scancode]);
              return;
       }

    //scanf code
    Putc(upperCaseKbdus[scancode]);
    *buffer++ = upperCaseKbdus[scancode];
    *p =0x0F;
    if(kbdus[scancode]=='\n')
    isNewline = 1; 
  }
}
outb(0x20,0x20);
}


char temp[150]={0};


char* gets()
 {
  int i=0,j=0;

  int k=0;
  for(k=0;k<150;k++)
	temp[k] = '\0';
   
  if(!isNewline)
  return NULL;
  else
   {
    while(buf[i]!='\n')
    {
       temp[i]=buf[i];
       i++;
    }
    isNewline =0;
    temp[i++]='\0';

  }

 while(buf[i])
  {
	buf[j++]=buffer[i++];
   }
 
for(i=j;i<256;i++)
    buf[i] = '\0';

 buffer = &buf[j];

   return temp;
 }

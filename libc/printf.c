#include<stdio.h>
#include<syscall.h>
#include<defs.h>
#define va_start(v,l)     __builtin_va_start(v,l)
#define va_arg(v,l)       __builtin_va_arg(v,l)
#define va_end(v)         __builtin_va_end(v)
#define va_copy(d,s)      __builtin_va_copy(d,s)




char * itoa1( uint64_t value, int base, char * str )
{
    char * rc;
    char * ptr;
    char * low;
    // Check for supported base.
    if ( base < 2 || base > 36 )
    {
        *str = '\0';
        return str;
    }
    rc = ptr = str;
    // Set '-' for negative decimals.
    if ( value < 0 && base == 10 )
    {
        *ptr++ = '-';
    }
    // Remember where the numbers start.
    low = ptr;
    // The actual conversion.
    do
    {
        // Modulo is negative for negative value. This trick makes abs() unnecessary.
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35 + value % base];
        value /= base;
    } while ( value );
    // Terminating the string.
    *ptr-- = '\0';
    // Invert the numbers.
    while ( low < ptr )
    {
        char tmp = *low;
        *low++ = *ptr;
        *ptr-- = tmp;
    }
    return rc;
}



void itoa(unsigned long i,int base,char *buf)
{

 char bchars[]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
 char tbuf[64];
  if(base >16)return;
   if(i<0){
     *buf++ ='-';
     i*=-1;
    }
   int pos = 0;
   int opos =0;
   int top =0;
   
  if(i==0){
    buf[0] = '0';
    buf[1] = '\0';
    return;
   }
  while(i!=0)
  {
    tbuf[pos] = bchars[i % base];
    pos++;
    i/=base;
  }
  top = pos--;
  int start =0;
  if (base ==16)
  {
    start =2;
    buf[0] = '0';
    buf[1] = 'X';
  }
  for(opos = start;opos<top+start;pos--,opos++)
  {
    buf[opos] = tbuf[pos];
  }
  buf[opos] = '\0';
}



int printf(const char *format, ...)
 {

typedef __builtin_va_list va_list;

va_list args;
va_start(args,format);
char sprocessed[256]={};
int i=0,j=0,k=0;
int temp=0;
while(format[i])
{
  if(format[i]!='%')
  {
    sprocessed[j] = format[i];
     i++;
     j++;
  }
  else
  {
    i++;
    switch(format[i])
    {
      case 'c':;
               int c = va_arg(args,int);
               sprocessed[j++] = c;
               break;
      case 's':;
               unsigned char *s = va_arg(args,unsigned char*);
               k=0;
               while(s[k])
                 sprocessed[j++]=s[k++];
               break;
      case 'd':;
               char buffer[16]="";
               temp = va_arg(args,int);
               itoa(temp,10,buffer);
               k=0;
               while(buffer[k])
                   sprocessed[j++] = buffer[k++];
               break;
      case 'x':;
               temp = va_arg(args,int);
               char buffer1[16]="";
               itoa(temp,16,buffer1);
               k=0; 
               while(buffer1[k])
                 sprocessed[j++] = buffer1[k++];
               break;
      case 'p':;
               unsigned long l = 0l;
               l = va_arg(args,long);
               char buffer2[64]={0};
               itoa(l,16,buffer2);
               k=0;
               while(buffer2[k])
                 sprocessed[j++]=buffer2[k++];
               break;
      default:break;
    }
    i++;
  }
}
va_end(args);
//char aa = sprocessed[0];
//aa =aa;
int noChars =write(1,sprocessed,j);
noChars = noChars;
return noChars;
}

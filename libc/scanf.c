#include<stdio.h>                                                           
#include<syscall.h>
#include<defs.h>
#define va_start(v,l)     __builtin_va_start(v,l)
#define va_arg(v,l)       __builtin_va_arg(v,l)
#define va_end(v)         __builtin_va_end(v)
#define va_copy(d,s)      __builtin_va_copy(d,s)

int atoi(char *nptr);
uint64_t atox(const char * s);

int read(int fd, char* buffer,int size)
 {
   uint64_t a = fd;
   uint64_t b = (uint64_t)buffer;
   uint64_t c = size;
   int ret;
     ret = __syscall3(SYSCALL_READ,a,b,c);
     return ret;
}

void read1(char *line,int size)
   {
    do
     {
      read(0,line,100);
      }while(*line == '\0');
  }


int scanf(const char * format,...)
{
  typedef __builtin_va_list va_list;
  va_list scan;
   char input[10]={0};
  va_start(scan,format);
  while(*format)
  {
    if(*format == '%')
      switch(*++format)
      {
        case 'd':;
                 int *num = va_arg(scan,int*);
                 read1(input,sizeof(int));
                 *num  = atoi(input);
                 break;
        case 's':;
                 char *str = va_arg(scan,char*);
                 read1(str,10);
                 break;
        case 'x':;
                 uint64_t *num1 = va_arg(scan,uint64_t*);
                 read1(input,sizeof(int));
                 *num1 =atox(input);          
          default:break;
      }
  format++;
  }

  return 0;
}

int isdigit(char c)
{
int a = c - 48;
if(a>=0 && a<=9)
 return 1;
else
  return 0;
}



int atoi(char *nptr)
{
    int base=1;
    int res=0;
    int i;

          /* Make sure all chars are numbers */
    for (i=0; *(nptr+i); ++i) {
      if(!isdigit(*(nptr+i)))
        return -1;
     }

    i=0;
    while(nptr[++i])
       base *= 10;

                /* Actual conversion. It works like this: for example, 123 is obtained with
                 *    1*100 + 2*10 + 3*1 */
    for (i=0; *(nptr+i); ++i ) {
       res += ((int)nptr[i] - 48) * base;
       base /= 10;
     }

  return res;
}

uint64_t atox(const char * s) {
uint64_t result = 0;
int c ;
if ('0' == *s && 'x' == *(s+1)) { s+=2;
  while (*s) {
    result = result << 4;
    if (c=(*s-'0'),(c>=0 && c <=9)) result|=c;
    else if (c=(*s-'A'),(c>=0 && c <=5)) result|=(c+10);
    else if (c=(*s-'a'),(c>=0 && c <=5)) result|=(c+10);
    else break;
    ++s;
    }
  }
      return result;
}


int  parse(char *line, char **argv)
{
    int len =0; 
    while (*line != '\0') 
    {
          len++;
          /* if not the end of line ....... */ 
          while (*line == ' ' || *line == '\t' || *line == '\n')
               *line++ = '\0';     /* replace white spaces with 0    */
          *argv++ = line;          /* save the argument position     */
          while (*line != '\0' && *line != ' ' && 
                 *line != '\t' && *line != '\n') 
               line++;             /* skip the argument until ...    */
     }
     *argv++ = "\0";                   /* mark the end of argument list  */ 
     return len;
}
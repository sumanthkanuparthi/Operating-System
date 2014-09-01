#include <sys/TARFSReader.h>
#include <sys/ELFReader.h>
#include<sys/tarfs.h>
#include<sys/dirent.h>

#include "stdio.h"
#include<sys/string.h>

/*
int strcmp(char *s1, char *s2)
{
     while (*s1 == *s2++) {
        if (*s1++ == 0) {
             return (0);
         }
     }
     return (*(unsigned char *) s1 - *(unsigned char *) --s2);
}*/


int power(int x, int y)
{
    if( y == 0)
        return 1;
    else if (y%2 == 0)
        return power(x, y/2)*power(x, y/2);
    else
        return x*power(x, y/2)*power(x, y/2);
 
}

int getSize(char *p) 
{
 int k = 0;
 while (*p) {
 k = (k<<3)+(k<<1)+(*p)-'0';
 p++;
 }


 int decimal=0, i=0, rem; 
 while (k!=0) 
 { 
  rem = k%10; 
  k/=10; 
  decimal += rem*power(8,i); 
  ++i; 
 }
 
 return decimal;
}



#define BLOCKSIZE 512

int8_t IsValidFile(char *fileName)
{
    char *start = ReadFile(fileName);
    int8_t rip= -1;
    struct Exe_Format exeFormat;
    rip= Parse_ELF_Executable( start , &exeFormat);
    if(rip != -1)
        return 1;
    else
        return -1;
}

char* ReadFile(char *fileName)
{
    struct posix_header_ustar * entry = (struct posix_header_ustar*)(&_binary_tarfs_start);

    //printf("tarfs in [2q%p:%p]\n", &_binary_tarfs_start, &_binary_tarfs_end);
    int padding =0;
	//printf("\nname - %s, size-%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s",entry ->name, entry->size, entry-> mode , entry-> uid, entry-> gid, entry-> mtime, entry-> checksum, entry-> typeflag, entry-> linkname, entry-> magic, entry-> version, entry-> uname, entry-> gname, entry-> devmajor, entry-> devminor , entry-> prefix  , entry-> pad);
    //while(strcmp(fileName,entry ->name) != 0 && (uint64_t)entry < (uint64_t)&_binary_tarfs_end)
    while(strcmp(fileName,entry ->name) != 0 &&(uint64_t)entry < (uint64_t)&_binary_tarfs_end)
        {
    	   int size = getSize(entry->size);
           
		   
			//printf("\nname - %s, size-%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s",entry ->name, entry->size, entry-> mode , entry-> uid, entry-> gid, entry-> mtime, entry-> checksum, entry-> typeflag, entry-> linkname, entry-> magic, entry-> version, entry-> uname, entry-> gname, entry-> devmajor, entry-> devminor , entry-> prefix  , entry-> pad);
           if(strcmp(entry->typeflag,"5")  == 0){
				//printf("  Directory -- %s, size-%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s",entry ->name, entry->size, entry-> mode , entry-> uid, entry-> gid, entry-> mtime, entry-> checksum, entry-> typeflag, entry-> linkname, entry-> magic, entry-> version, entry-> uname, entry-> gname, entry-> devmajor, entry-> devminor , entry-> prefix  , entry-> pad);
			//	printf("  Directory -- %s, size-%s, type %s ",entry ->name, entry->size, entry-> typeflag);
			}else if(strcmp(entry->typeflag,"0")==0){
				//printf("  file -- %s, size-%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s",entry ->name, entry->size, entry-> mode , entry-> uid, entry-> gid, entry-> mtime, entry-> checksum, entry-> typeflag, entry-> linkname, entry-> magic, entry-> version, entry-> uname, entry-> gname, entry-> devmajor, entry-> devminor , entry-> prefix  , entry-> pad);
			//	printf("  file -- %s, size-%s, type %s ",entry ->name, entry->size, entry-> typeflag);
			}else if(entry->typeflag == NULL){
			//	printf(" typeflag null");
			}else{
				//printf(" entry address : %p ,entry->typeflag %s, name %s",entry,entry->typeflag,entry->name);
			}
           //increasing by size of the structure + size of file
           entry = (struct posix_header_ustar *)((char*)entry + sizeof(struct posix_header_ustar) + size );
		   if(size > 0){
				padding = BLOCKSIZE - size%BLOCKSIZE;
				//printf("value - %d & padding - %d",(char*)&_binary_tarfs_end - (char*)entry, padding);
				if((char*)&_binary_tarfs_end - (char*)entry >=BLOCKSIZE)
				{
					entry = (struct posix_header_ustar *)((char*)entry + padding);
					//break;
				}
			 //printf(" ,address:%p",entry);
			 //add padding
			
			 
			}	
		} 

//		printf("\nName :%s Size:%s\n",entry ->name,entry->size);


		char *ptr = ((char*)(entry)+ sizeof(struct posix_header_ustar));
	
		return ptr;

}

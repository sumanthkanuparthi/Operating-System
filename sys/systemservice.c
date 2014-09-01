#include<sys/systemcall.h>
#include "stdio.h"
#include <sys/MManager.h>
#include <sys/file.h>
#include <sys/dirent.h>
#include <sys/contextswitch.h>
#include <sys/queue.h>
#include <sys/string.h>
char currentPath[100] = "";
char envPaths[200] = "";
char* lastPtr = envPaths;

#define FAILURE 0
#define SUCCESS 1
extern struct registers_t* g_regVals;
extern task_struct *currentTask;   // &
char * gets();

void write()
{
  int fd = g_regVals->rbx;
  char *str =(char*)g_regVals->rcx;
 long size = g_regVals->rdx;
  if (fd == 1 || fd == 2 )
  {
    char *temp = str;
  while((size--)> 0 && *str++);
  *str = '\0';
    Puts(temp);
  }
}


void read()
 {
  int fd = g_regVals->rbx;
   // bg check - can't get keyboard input
  if(currentTask->memMap->isBgProcess == 1)
  {
    printf("Kill the process");
    ExitProcess();
  }
    else
  {
  char *str = (char*)g_regVals->rcx;
   int size = g_regVals->rdx;
   int readBytes = 0;
   if(fd < 3){
	  char *str1 = gets();
	  if(str1 == 0){
			*str = '\0';
	  }else{
	   while(*str1){
		 *str++ = *str1++;
		 readBytes++;
		 }
	   *str = '\0';
	   readBytes++;
	  }
	}else{
		readBytes = t_read(fd, str, size);
	}
	g_regVals->rax = readBytes;
    }
	}

void w_open()
 {
  uint64_t fd;
  fd = (uint64_t) open((char*)g_regVals->rbx,(int)g_regVals->rcx);
  g_regVals->rax = fd;
 }
 
 void w_close()
 {
  uint64_t ret;
  ret = close((int)g_regVals->rbx);
  if(ret != -1){
	  g_regVals->rax = SUCCESS;
  }else{
	  g_regVals->rax = FAILURE;
  }
 }

void memMap()
{
  uint64_t retAddr ;
  retAddr = (uint64_t) mmap((void*)g_regVals->rbx,g_regVals->rcx);
  g_regVals->rax = retAddr;
}


void w_opendir()
 {
  DIR * directory1,* directory2;
  directory1 = (DIR *)g_regVals->rcx;
  directory2 = opendir((char*)g_regVals->rbx);
  if(directory2 != NULL){
	  strcpy(directory1->dirname, directory2->dirname);
	  strcpy(directory1->current.d_name, directory2->current.d_name);
	  directory1->current.offset = directory2->current.offset;
	  directory1->dirent_filled = directory2->dirent_filled;
	  g_regVals->rax = SUCCESS;
  }else{
	  g_regVals->rax = FAILURE;
  }
 }

//Have to handle null return cases 
 void w_readdir()
 {
  dirent * dirent1, * dirent2;
  dirent1 = (dirent *)g_regVals->rcx;
  dirent2 = readdir((DIR *)g_regVals->rbx);
  if(dirent2 != NULL){
	  strcpy(dirent1->d_name, dirent2->d_name);
	  dirent1->offset = dirent2->offset;
	  g_regVals->rax = SUCCESS;
  }else{
	  g_regVals->rax = FAILURE;
  }
 }

 void w_closedir()
 {
  int ret;
  ret = closedir((DIR *)g_regVals->rbx);
  if(ret != -1){
	  g_regVals->rax = SUCCESS;
  }else{
	  g_regVals->rax = FAILURE;
  }
 }
 


void GetCurrentPath()
{
    char *str = (char*)g_regVals->rbx;
    char *str1 = currentPath;
  
	*str++ = '~';
        while(*str1)
        	*str++ = *str1++;
	 *str++ = ']';
        *str = '\0';
}

void UpdateEnvPath()
{
	char *str = (char*)g_regVals->rbx;

	//only supporting PATH
	if(starts_with(str,"PATH=") == 1)
	{
		char temp[100]="";
		substring(temp,str,5,strlen(str));

		char *newEnv = temp;
		if(*newEnv  == '/')//should be full path
		{
		
			newEnv  +=1;
			DIR * dir =  opendir(newEnv);

	 		if(dir  != NULL)
	 		{
				int i=0;
				for(i=0;i<200;i++)
					envPaths[i] = '\0';

				lastPtr =envPaths;

				while(*newEnv)
					*lastPtr ++ = *newEnv ++;
			}
		}
		else if(starts_with(temp,"$PATH:") == 1)
		{
			char temp1[100]="";
			substring(temp1,temp,6,strlen(temp));

			char *newenv = temp1;
			if(*newenv == '/')//should be full path
			{
				newenv +=1;
				DIR * dir =  opendir(newenv);

	 			if(dir  != NULL)
	 			{
					if(lastPtr  == envPaths)//first one
					{
						while(*newenv)
							*lastPtr ++ = *newenv++;
					}
					else//new one being added
					{
						*lastPtr ++ = ':';
						while(*newenv)
							*lastPtr ++ = *newenv++;
					}
				}
			}
		}

	}

}
void UpdateCurrentPath()
{
    char *newPath = (char*)g_regVals->rbx;

    if(*newPath == '/')
    {
	newPath = newPath+1;
	char temp[100] = "";
	char *tempPath = temp;
	while(*newPath)
			*tempPath++ = *newPath++;

	
	DIR * dir =  opendir(temp);

	 if(dir  != NULL)
	 {
		int i=0;
		for(i=0;i<100;i++)
			currentPath[i] = '\0';
		
		char *str1 = currentPath;
		char *tempPath = temp;
		while(*tempPath )
			*str1++ = *tempPath++;

	 }
    }
    else if(*newPath == '.')
    {
	 if(*(newPath+1) == '.') //parent directory case
	 {
		char temp[100] = "";
		char parent[100] = "";
		strncpy(temp,currentPath,strlen(currentPath)-1);
		temp[strlen(currentPath)-1] = '\0';
		int index = lastIndexOf (temp, "/");
		
		if(index == -1 || index == strlen(currentPath)-1)
		{
			parent[0] = '\0';
		}

			substring(parent,currentPath, 0, index);
			char *tempPath = (parent+index);
			newPath = newPath+2;
			while(*newPath )
        			*tempPath++ = *newPath++;

			DIR * dir =  opendir(parent);

	 		if(dir  != NULL)
	 		{
				int i=0;
				for(i=0;i<100;i++)
					currentPath[i] = '\0';

				char *cur = currentPath;
      				char *tempPath = parent;

				while(*tempPath)
        			*cur++ = *tempPath++;
	 		}

	 }
	 else //current directory case
	 {
		char temp[100] = "";
		char *cur = currentPath;
      		char *tempPath = temp;
	
		while(*cur)
        		*tempPath++ = *cur++;

		newPath = newPath+2;

		while(*newPath )
        		*tempPath++ = *newPath++;

		DIR * dir =  opendir(temp);

	 	if(dir  != NULL)
	 	{
			int i=0;
			for(i=0;i<100;i++)
				currentPath[i] = '\0';

			char *cur = currentPath;
      			char *tempPath = temp;

			while(*tempPath)
        		*cur++ = *tempPath++;
	 	}
	 }
    }
    else
    {
      char temp[100] = "";
      char *cur = currentPath;
      char *tempPath = temp;
	
	while(*cur)
        	*tempPath++ = *cur++;
	
	while(*newPath)
			*tempPath++ = *newPath++;

	DIR * dir =  opendir(temp);

	 if(dir  != NULL)
	 {
		int i=0;
		for(i=0;i<100;i++)
			currentPath[i] = '\0';

		char *cur = currentPath;
      		char *tempPath = temp;

		while(*tempPath)
        	*cur++ = *tempPath++;
	 }	
    }
	}


#include <stdio.h>
#include<syscall.h>


int strlen(const char *str)
{
        const char *s;

        for (s = str; *s; ++s)
                ;
        return (s - str);
}


int strcmp(const char *s1,const char *s2)
{
       while (*s1 == *s2++) {
          if (*s1++ == 0){ 
               return (0);
                 }
            }
        return (*(unsigned char *) s1 - *(unsigned char *) --s2);
}


int readcommand(char *line,char* args[])
{
	do
	{
		read(0,line,100);
	}while(*line == '\0');

	return parse(line,args);

}


int main(int argc, char* argv[]) 
{

	char *env[] = {"PATH=C:\\TEST", NULL};
	char *args[] = {"bin/child", "arg1", "arg2", NULL};

	char path[100];

	while (1) 
	{
      char buff[50] = "";
	   getPath(path);

	    printf ("\n%s$",path);
	    int pid = 0;
	
	    int res = readcommand(buff,args);
		
	    
	    if( strcmp(args[0],"cd")== 0)
	    {
		printf("CD is getting %s",args[1]);
		updatePath(args[1]);
	    }
	    else if( strcmp(args[0],"setenv")== 0)
	    {
		updateEnvPath(args[1]);
	    }
      else if(strcmp(args[0],"ulimit")==0)
      {
        if(strcmp(args[1],"-s")==0)
        {
          printf("ulimit:%s,%s,%s",args[1],args[2],buff);
          int size = atoi(args[2]);
          ulimit(size);
        }
      }
      else
	    {    
		
		int isBgProcess = 0;
		if(strlen(args[res -1])==1)
 			if(args[res -1][0]=='&')
 				isBgProcess = 1;
           	pid = fork();

	    	if(pid  > 0)
	    	{
	       	if(isBgProcess != 1)
				waitpid(pid);

	    	}
	    	else
	    	{
            		int res = execvpe(args[0], args, env);

            		if(res == -1)
            		{
                		printf("\n%s : Command not found!!!!",args[0]);
                		exit(0);
            		}
	   	}
	    }
    	}

	return 0;
}


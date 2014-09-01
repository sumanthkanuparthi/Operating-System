#include <stdio.h>
#include<syscall.h>



int main(int argc, char* argv[]) 
{
	int a = fork();
	char *env[] = {"PATH=C:\\TEST", NULL};

	if(a > 0)
	{
		printf("\nParent ProcessID : %d\n",GetProcessId());

		waitpid(a);

		printf("\nAfter Child Process has exited");
			
		sleep(10);
			
		printf("\nParent : After Sleep");
		

	}
	else
	{
		printf("\nChild ProcessID : %d\n",GetProcessId());
		char *inp[] = {"bin/hello","a","b"};

		execvpe(inp[0],inp,env);
	}

	return 0;

}

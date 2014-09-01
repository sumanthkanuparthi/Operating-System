#include <stdlib.h>
#include<syscall.h>
#include<stdio.h>
#include<defs.h>
void _start(char *arguements,char *envs) {
	int argc = 1;
	char* argv[10];

	if(arguements != NULL)	
		parse(arguements,argv);	
	char* envp[10];
	int res;
	res = main(argc, argv, envp);
	exit(res);
}


#include <stdio.h>
#include<syscall.h>

int main(int argc, char* argv[]) {
	printf("Hello World!\n");
	const char* path= "etc/new.txt";
	printf("\nAddress of string %x\n",path);
	printf("\nFiles opened with descriptor %d, %d", open(path,8), open("bin/hello",7));
	DIR * directory = opendir("etc/");
	printf("\nDirent - %x", directory);
	printf("\nDirent - %s", readdir(directory)->d_name);
	printf("\nDirent - %s", readdir(directory)->d_name);
    printf("\nDirent - %x", readdir(directory));
	if(readdir(directory) == NULL)
		printf("\nNULL Returned Yaaaay");
	return 0;
}
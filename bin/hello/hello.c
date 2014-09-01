#include<sys/dirent.h>
#include<syscall.h>
#include<stdio.h>
#include<sys/file.h>



int main(int argc, char* argv[]) 
{
printf("Enter any number:\n");
int *a;
a = (int*)malloc(sizeof(int));
*a =24;
printf("Integer is %d,%x",*a,a);

/*  DIR *dir; 
    struct dirent *dirfile;
	char str1[50];
	
	//printf("%s",str);
	dir = opendir(str1);
    while((dirfile = readdir(dir)) != NULL)
    {
        printf(" %s\n", dirfile->d_name);
    }
    closedir(dir); */
	int fd = open("etc/new.txt",8); 
	char st[100];
	//int i = 0;
	//for(i = 0; i<20;i++){
	int readbytes =	read(fd,st,50);
		printf("%d -> %s",readbytes, st);
		readbytes =	read(fd,st,10);
		printf("%d -> %s",readbytes, st);
	//}
	if(fd >2){
		close(fd);
	}
return 0;
}

#include<syscall.h>   
#include<sys/dirent.h> 
#include<stdio.h>


int main(int argc, char* argv[])
{
    DIR *dir;
    struct dirent *dirfile;
	if(argv[1]!=NULL){
		dir = opendir(argv[1]);
	}else{
		dir = opendir("etc/");
	}
    while((dirfile = readdir(dir)) != NULL)
    {
        printf(" %s\n", dirfile->d_name);
    }
    closedir(dir);
}
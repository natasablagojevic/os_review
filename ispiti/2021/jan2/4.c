#define _XOPEN_SOURCE 700
#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<ftw.h>
#include<semaphore.h>
#include<sys/mman.h>
#include<sys/wait.h>
#include<pthread.h>
#include<time.h>
#include<string.h>
#include<math.h>
#include<ctype.h>
#include<stdbool.h>
#include<dirent.h>

#define UNUSED(X) (void)(X)
#define RD_END 0
#define WR_END 1

char *s = NULL;

void greska(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

char *substring(char *dest, char *src, int begin, int n)
{
    while (n > 0) {
        *dest = *(src + begin);

        dest++;
        src++;
        n--;
    }

    *dest = 0;
    
    return dest - (n-begin);
}

void f(char *path)
{
    struct stat sb;
    if (lstat(path, &sb) == -1)
        greska("lstat");
    
    
}

// a.out dir s
int main(int argc, char **argv)
{

/* 
    char *dest = malloc(100);

    int i = 0;
    substring(dest, "dir.bla", i++, strlen("bla"));

    while (strcmp(dest, "bla") != 0) 
        substring(dest, "dir.bla", ++i, strlen("bla"));
    printf("%s\n", dest);

 */   

    if (argc != 3)
        greska("args failed");

    s = malloc(strlen(argv[2]) + 1);
        if (s == NULL)
            greska("s malloc failed");
    
    strcpy(s, argv[2]);




    free(s);
    exit(EXIT_SUCCESS);
}
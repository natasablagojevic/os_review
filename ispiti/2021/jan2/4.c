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
#include<errno.h>

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
    
    if (!S_ISDIR(sb.st_mode))   
        return ;

    printf("%s:\n", path);

    DIR *dir = opendir(path);
        if (dir == NULL)
            greska("opendir failed");

    struct dirent *dirEntry = NULL;
    errno = 0;

    while ((dirEntry = readdir(dir)) != NULL) {
        char *nova = malloc(strlen(dirEntry->d_name) + strlen(path) + 2);
            if (nova == NULL)
                greska("nova malloc failed");

        strcpy(nova, path);
        strcat(nova, "/");
        strcat(nova, dirEntry->d_name);

        printf("\t%s\n", nova);

        if (strcmp(dirEntry->d_name, ".") != 0 && strcmp(dirEntry->d_name, "..") != 0) {
            if (stat(nova, &sb) == -1)
                greska("f:\tstat failed");

            // magija

            free(nova);
            errno = 0;
            continue;
        }

        f(nova);
        free(nova);
        errno = 0;
    }

    if (errno == EBADF)
        greska("readdir failed");

    if (closedir(dir) == -1)
        greska("closedir failed");
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

    f(argv[1]);


    free(s);
    exit(EXIT_SUCCESS);
}
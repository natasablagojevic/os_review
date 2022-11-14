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

#define UNUSED(X) (void)(X)
#define RD_END 0
#define WR_END 1

void greska(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

// a.out fajl
int main(int argc, char **argv)
{
    if (argc != 2)
        greska("args failed");

    struct stat sb;
    if (lstat(argv[1], &sb) == -1)
        greska("lstat failed");

    char *pom = NULL;
    char *real_file = realpath(argv[1], pom);
        if (real_file == NULL)
            greska("realpath failed");

    if (stat(real_file, &sb) == -1)
        greska("stat failed");

    char *ekst = strrchr(real_file, '.');
    /* 
        ekst == NULL ? -> fajl nemekstenziju
     */

    switch (sb.st_mode & S_IFMT) {
        case S_IFREG: printf("reguralan "); break;
        case S_IFDIR: printf("direktorijum "); break;
        case S_IFSOCK: printf("soket "); break;
        case S_IFLNK: printf("link "); break;
        case S_IFIFO: printf("pajp "); break;
        case S_IFBLK: printf("blok "); break;
        case S_IFCHR: printf("karakter "); break;
    }

    if (ekst == NULL) 
        printf("nema\n");
    else 
        printf("%s\n", ekst);


    exit(EXIT_SUCCESS);
}
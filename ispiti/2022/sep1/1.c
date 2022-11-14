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

void get_day(int dan)
{
    switch (dan) {
        case 0 : printf("nedelja\n"); break;
        case 1 : printf("ponedeljak\n"); break;
        case 2 : printf("utorak\n"); break;
        case 3 : printf("sreda\n"); break;
        case 4 : printf("cetvrtak\n"); break;
        case 5 : printf("petak\n"); break;
        case 6 : printf("subota\n"); break;
    } 
}

int f(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
    if (typeflag == FTW_F) {
        struct tm *vreme = localtime(&(sb->st_mtime));
            if (vreme == NULL)
                greska("localtime failed");

        printf("%s ", fpath + ftwbuf->base);
        get_day(vreme->tm_wday);
    }

    return 0;
}

// a.out dir
int main(int argc, char **argv)
{   
    if (argc != 2)
        greska("args failed");

    struct stat sb;
    if (stat(argv[1], &sb) == -1)
        greska("stat failed");

    if (!S_ISDIR(sb.st_mode))
        greska("nije dir");

    if (nftw(argv[1], f, 50, 0) == -1)
        greska("nije dir");



    exit(EXIT_SUCCESS);
}
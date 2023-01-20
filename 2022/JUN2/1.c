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

bool check_test_(const char *s)
{
    // 0 1 2 3 4  = 5
    // t e s t _
    char pom[6];
    int k = 0;
    for (int i = 0; i < 5; i++)
        pom[k++] = s[i];

    pom[k] = 0;

    return strcmp(pom, "test_") == 0 ? true : false;
}

/* 
    111 101 101
    000 010 010
    ------------ &
    000 000 000

 */

int f(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
    UNUSED(sb);
    if (typeflag == FTW_D) {
        mode_t grp = sb->st_mode & S_IWGRP;
        mode_t oth = sb->st_mode & S_IWOTH;
        if (check_test_(fpath + ftwbuf->base) && grp == 0 && oth == 0)
            printf("%s\n", fpath + ftwbuf->base);
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
        greska("nftw failed");


    exit(EXIT_SUCCESS);
}
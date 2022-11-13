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

bool pocinje_sa_test_(const char *s)
{
    /* 
        0 1 2 3 4 = 5
        t e s t _
     */

    char pom[5];
    int i;
    for (i = 0; i < 5; i++)
        pom[i] = s[i];

    pom[i] = 0;

    return strcmp(pom, "test_") == 0 ? true : false;
}

int f(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
    if (typeflag == FTW_D) {
        mode_t prava = sb->st_mode & (S_IWGRP | S_IWOTH);

        /* 
            110 010 000
            000 010 010
            111 101 101
            ------------ and
            110 000 000

         */

        if (prava == 0 && pocinje_sa_test_(fpath + ftwbuf->base))
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
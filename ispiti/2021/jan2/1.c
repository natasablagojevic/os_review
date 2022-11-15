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

//  a.out fajl1 fajl2
int main(int argc, char **argv)
{
    if (argc != 3)
        greska("args failed");

    struct stat sb;
    if (stat(argv[1], &sb) == -1)
        greska("stat failed");

    mode_t usr = sb.st_mode & S_IRWXU;
    mode_t grp = sb.st_mode & S_IRWXG;
    mode_t oth = sb.st_mode & S_IRWXO;

    if (chmod(argv[2], (grp << 03) | (oth << 03) | (usr >> 06)) == -1)
        greska("chmod failed");  


    exit(EXIT_SUCCESS);
}
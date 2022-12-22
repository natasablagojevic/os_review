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
#define MAX 256

void greska(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

bool check_rec(char *s)
{
    if (s[0] == '-')
        return false;

    if (strlen(s) != 4)
        return false;

    for (int i = 0; s[i] != 0; i++) 
        if (!isdidgit(s[i]))
            return false;

    return true;
}

int main(int argc, char **argv)
{
    if (argc != 2)
        greska("args failed");

    int fd = open(argv[1], O_RDWR);
        if (fd == -1)
            greska("open failed");

    FILE *f = fopen(argv[1], "r");
        if (f == NULL)
            greska("fopen malloc failed");

    struct flock lock;
    char *buf = NULL;

    while (fscanf(f, "%ms", &buf) == 1) {
        if (!check_rec(buf))
            continue;

        lock.l_type = F_RDLCK;
    }   



    fclose(f);
    close(f);
    exit(EXIT_SUCCESS);
}
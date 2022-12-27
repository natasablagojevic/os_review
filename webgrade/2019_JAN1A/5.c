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

// a.out fajl rec
int main(int argc, char **argv)
{
    if (argc != 3)
        greska("args failed");

    int fd = open(argv[1], O_RDWR);
        if (fd == -1)
            greska("open failed");

    FILE *f = fdopen(fd, "r+");
        if (f == NULL)
            greska("fdopen failed");

    char buf[MAX+1];
    struct flock lock;

    while (fscanf(f, "%s", buf) == 1) {
        if (strcmp(buf, argv[1])) {
            continue;
        }


        lock.l_type = F_UNLCK;
        lock.l_whence = SEEK_SET;
        lock.l_start = ftell(f);
        lock.l_len = -strlen(argv[2]);

        if (fcntl(fd, F_GETLK, &lock) == -1)
            greska("fcntl failed");

        switch (lock.l_type) {
            case F_RDLCK: printf("%ld r\n", ftell(f) - strlen(argv[2])); break;
            case F_WRLCK: printf("%ld w\n", ftell(f) - strlen(argv[2])); break;
        }
    }

    lock.l_type = F_UNLCK;


    fclose(f);
    exit(EXIT_SUCCESS);
}
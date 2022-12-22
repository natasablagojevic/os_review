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
#include<errno.h>

#define UNUSED(X) (void)(X)
#define RD_END 0
#define WR_END 1

void greska(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
    if (argc !=2)
        greska("args failed");

    int fd = open(argv[1], O_RDWR);
        if (fd == -1)
            greska("open failed");

    FILE *f = fdopen(fd, "rw");
        if (f == NULL)
            greska("fdopen failed");

    char *buf = NULL;
    struct flock lock;

    while (fscanf(f, "%ms", &buf) == -1) {
        lock.l_whence = SEEK_SET;
        lock.l_type = F_RDLCK;
        lock.l_start = ftell(f);
        lock.l_len = -strlen(buf);

        if (fcntl(fd, F_SETLK, &lock) == -1) {
            if (errno == EACCES || errno == EAGAIN)
                greska("fcntl failed");

                        
        }


    }

    fclose(f);
    close(f);
    exit(EXIT_SUCCESS);
}
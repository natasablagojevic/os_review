#define _XOPEN_SOURCE 700
#define _POSIX_C_SOURCE 200112L
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<ctype.h>
#include<unistd.h>
#include<ftw.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<semaphore.h>
#include<sys/mman.h>
#include<fcntl.h>
#include<signal.h>
#include<sys/wait.h>
#include<sys/epoll.h>
#include<poll.h>
#include<stdbool.h>
#include<string.h>
#include<errno.h>

#define UNUSED(X) (void)(X)

void greska(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

// a.out fajl rec1 rec2
int main(int argc, char **argv)
{
    if (argc != 4)
        greska("args failed");

    int fd = open(argv[1], O_RDWR);
        if (fd == -1)
            greska("open failed");

    FILE *f  = fdopen(fd, "rw");
        if (f == NULL)
            greska("fdopen failed");

    char *buf = NULL;
    struct flock lock;
    int broj_nezamenjenih = 0;

    while (fscanf(f, "%ms", &buf) == 1) {
        if (strcmp(buf, argv[2]) != 0)
            continue;

        lock.l_type = F_WRLCK;
        lock.l_whence = SEEK_SET;
        lock.l_start = ftell(f);
        lock.l_len = -strlen(buf);

        if (fcntl(fd, F_SETLK, &lock) == -1) {
            if (errno != EACCES && errno != EAGAIN)
                greska("lock failed");

            broj_nezamenjenih++;
        } else {
            if (fseek(f, -strlen(buf), SEEK_SET) == -1)
                greska("fseek failed");
            fprintf(f, "%s", argv[3]);
        }
    }

    lock.l_type = F_UNLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;

    if (fcntl(fd, F_SETLK, &lock) == -1)
        greska("fcntl failed");

    printf("%d\n", broj_nezamenjenih);
    
    fclose(f);
    close(fd);
    exit(EXIT_SUCCESS);
}
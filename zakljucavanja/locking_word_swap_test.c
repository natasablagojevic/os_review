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

#define UNUSED(X) (void)(X)

void greska(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

// a.out fajl
int main(int argc, char **argv)
{
    int fd = open(argv[1], O_RDONLY);
        if (fd == -1)
            greska("open failed");

    struct flock lock;
    lock.l_type = F_RDLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 8;
    lock.l_len = -5;

    if (fcntl(fd, F_SETLK, &lock) == -1)
        greska("fcntl failed (locking)");

    sleep(10);
    
    close(fd);
    exit(EXIT_SUCCESS);
}
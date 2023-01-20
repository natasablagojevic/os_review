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

#define UNUSED(X) (void)(X)

void greska(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

//  0     1   2 3
// a.out fajl a b
int main(int argc, char **argv)
{
    if (argc != 4)
        greska("args failed");

    // int fd = open(argv[1], O_RDWR);
    //     if (fd == -1)
    //         greska("open failed");

    FILE *f = fopen(argv[1], "rw");
        if (f == NULL)
            greska("fopen failed");

    int fd = fileno(f);
        if (fd == -1)
            greska("fileno failed");

    bool unlocked = false;
    bool shared_lock = false;
    bool exclusive_lock = false;

    bool read = false;
    bool write = false;

    int a = atoi(argv[2]);
    int b = atoi(argv[3]);

    struct flock lock;
    lock.l_type = F_RDLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = a;
    lock.l_len = b;

    if (fcntl(fd, F_GETLK, &lock) == -1)
        greska("fcntl");

    if (lock.l_type = F_UNLCK)
        read = true;

    lock.l_type = F_WRLCK;

    if (fcntl(fd, F_GETLK, &lock) == -1)
        greska("fcntl failed");

    if (lock.l_type = F_UNLCK)
        write = true;

    if (read && write)
        printf("unlocked\n");
    else if (read && !write)
        printf("shared lock\n");
    else 
        printf("exclusive lock\n");


    close(fd);
    exit(EXIT_SUCCESS);
}
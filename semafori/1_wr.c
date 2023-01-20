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
#include<float.h>

#define UNUSED(X) (void)(X)

typedef struct 
{
    sem_t ready;
    sem_t done;
    double points[1024];
    unsigned nPoints;
} OsInputData;

void greska(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

void *get_memory_block(char *path, unsigned size)
{
    int fd = shm_open(path, O_RDWR | O_CREAT, 0600);
        if (fd == -1)
            greska("shm_open failed");

    if (ftruncate(fd, size) == -1)
        greska("ftruncate failed");

    void *addr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if (addr == MAP_FAILED)
            greska("mmap failed");

    close(fd);
    return addr;
}

double rastojanje(double x1, double y1, double x2, double y2)
{
    return (x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2);
}

// a.out memsh
int main(int argc, char **argv)
{
    if (argc != 2)
        greska("args failed");

    OsInputData *p = get_memory_block(argv[1], sizeof(OsInputData));

    // if (sem_wait(&(p->ready)) == -1)
    //     greska("sem_wait failed");
    
    if (sem_init(&(p->ready), 1, 0) == -1)
        greska("sem_init failed");

    p->nPoints = 10;
    p->points[0] = -1.3;
    p->points[1] = 1.7;
    p->points[2] = 15.4;
    p->points[3] = -101.3;
    p->points[4] = -1.4;
    p->points[5] = 1.9;
    p->points[6] = 13.2;
    p->points[7] = -99.5;
    p->points[8] = -17;
    p->points[9] = 0;

    if (sem_post(&(p->done)) == -1)
        greska("sem_post failed");

    if (munmap(p, sizeof(OsInputData)) == -1)
        greska("munmap failed");

    // if (shm_unlink(argv[1]) == -1)
    //     greska("shm_unlink failed");


    exit(EXIT_SUCCESS);
}
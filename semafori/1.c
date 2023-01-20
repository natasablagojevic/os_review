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

void *get_memory_block(char *path, unsigned *size)
{
    int fd = shm_open(path, O_RDWR, 0600);
        if (fd == -1)
            greska("shm_open failed");

    struct stat sb;
    if (fstat(fd, &sb) == -1)
        greska("fstat failed");

    *size = sb.st_size;

    void *addr = mmap(NULL, *size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if (addr == MAP_FAILED)
            greska("mmap failed");

    close(fd);
    return addr;
}

double rastojanje(double x1, double y1, double x2, double y2)
{
    return sqrt((x1 - x2)*(x1 - x2) + (y1 - y2)*(y1 - y2));
}

// a.out memsh
int main(int argc, char **argv)
{
    if (argc != 2)
        greska("args failed");

    unsigned size = 0;
    OsInputData *p = get_memory_block(argv[1], &size);

    if (sem_wait(&(p->ready)) == -1)
        greska("sem_wait failed");
    
    // if (sem_init(&(p->ready), 1, 1) == -1)
    //     greska("sem_init failed");

    double min = DBL_MAX;
    double x = DBL_MAX;

    for (unsigned i = 0; i < p->nPoints; i += 2) {
        for (unsigned j = 0; i != j && j < p->nPoints; j += 2) {
            x = rastojanje(p->points[i], p->points[i+1], p->points[j], p->points[j+1]);
            // printf("x: %lf\n", x);
            if (x < min)
                min = x;

            // printf("\tmin: %lf\n", min);
        }
    }

    if (sem_post(&(p->done)) == -1)
        greska("sem_post failed");

    if (munmap(p, size) == -1)
        greska("munmap failed");

    printf("%lf\n", min);

    // if (shm_unlink(argv[1]) == -1)
    //     greska("shm_unlink failed");


    exit(EXIT_SUCCESS);
}
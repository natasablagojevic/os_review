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
#include<float.h>

#define UNUSED(X) (void)(X)
#define RD_END 0
#define WR_END 1

typedef struct 
{
    sem_t ready;
    sem_t done;
    double points[1024];
    unsigned nPoints;
} OsInputData;

/* 
    points:
    x0, y0, x1, y1, ...

 */

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

double kvadratno_rasotjanje(double x0, double y0, double x1, double y1)
{
    return (x0 - x1)*(x0 - x1) + (y0 - y1)*(y0 - y1);
}

int main(int argc, char **argv)
{
    if (argc != 2)
        greska("args failed");

    unsigned size = 0;
    OsInputData *p = get_memory_block(argv[1], &size);

    if (sem_wait(&(p->ready)) == -1)
        greska("sem_wait failed");

    double min = DBL_MAX;

    for (unsigned i = 0; i < p->nPoints; i += 2) {
        double rastojanje = -1;
        for (unsigned j = i+2; i < p->nPoints; j += 2) {
            if (j+1 > p->nPoints)
                break;

            rastojanje = kvadratno_rasotjanje(p->points[i], p->points[i+1], p->points[j], p->points[j+1]);

            if (min > rastojanje)
                min = rastojanje;
        }
    }

    printf("%lf\n", sqrt(min));

    if (sem_post(&(p->done)) == -1)
        greska("sem_post failed");

    if (munmap(p, size) == -1)
        greska("munmap failed");


    exit(EXIT_SUCCESS);
}
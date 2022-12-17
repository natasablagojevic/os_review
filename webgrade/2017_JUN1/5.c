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

typedef struct 
{
    sem_t inDataReady;
    float array[1024];
    unsigned arrayLen;
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

double mi(float *a, unsigned n)
{
    double sum = 0.0;
    for (unsigned i = 0; i < n; i++)
        sum += a[i];

    return sum*1.0/n;
}

double devijacija(float *a, unsigned n)
{
    double sum = 0.0;
    for (unsigned i = 0; i < n; i++) {
        sum += (pow(a[i] - mi(a, n), 2));
    }

    sum = sum*1.0/n;

    return sqrt(sum);
}

// a.out mem
int main(int argc, char **argv)
{
    if (argc != 2)
        greska("args failed");
    
    unsigned size = 0;
    OsInputData *p = get_memory_block(argv[1], &size);

    if (sem_wait(&(p->inDataReady)) == -1)
        greska("sem_wait failed");

    printf("%lf\n", devijacija(p->array, p->arrayLen));

    // if (sem_post(&(p->inDataReady)) == -1)
    //     greska("sem_post failed");

    if (munmap(p, size) == -1)
        greska("munmap failed");


    exit(EXIT_SUCCESS);
}
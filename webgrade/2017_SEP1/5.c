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
    int array[1024];
    unsigned arrayLen;
} OsInputData;

void greska(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

void *get_memory_blok(char *path, unsigned *size)
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

bool check(int broj)
{
    if (broj == 1)
        return true;

    int p = 1;
    while (p < broj) {
        p *= 3;
        // printf("p: %d\n", p);
    }

    return p == broj ? true : false;
}

int main(int argc, char **argv)
{
    if (argc != 2)
        greska("args failed");
    
    unsigned size = 0;
    OsInputData *p = get_memory_blok(argv[1], &size);

    if (sem_wait(&(p->inDataReady)) == -1)
        greska("sem_wait failed");

    for (unsigned i = 0; i < p->arrayLen; i++)
        if (check(p->array[i]))
            printf("%d ", p->array[i]);

    if (munmap(p, size) == -1)
        greska("munmap failed");

    // if (shm_unlink(argv[1]) == -1)
    //     greska("shm_unlink failed");


    exit(EXIT_SUCCESS);
}
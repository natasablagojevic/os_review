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
    sem_t dataProcessed;
    char str[1024];
} OsInputData;

void greska(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

void *create_memory_block(char *path, unsigned size)
{
    int fd = shm_open(path, O_RDWR | O_CREAT, 0600);
        if (fd == -1)
            greska("fd failed");

    if (ftruncate(fd, size) == -1)
        greska("ftruncate failed");

    void *addr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if (addr == MAP_FAILED)
            greska("mmap failed");

    close(fd);
    return addr;
}

int main(int argc, char **argv)
{
    if (argc != 2)
        greska("args failed");

    OsInputData *p = create_memory_block(argv[1], sizeof(OsInputData));

    if (sem_init(&(p->inDataReady), 1, 1) == -1)
        greska("sem_init failed");

    strcpy(p->str, "natasa");

    if (sem_post(&(p->dataProcessed)) == -1)
        greska("sem_post failed");

    if (munmap(p, sizeof(OsInputData)) == -1)
        greska("munmap failed");


    exit(EXIT_SUCCESS);
}
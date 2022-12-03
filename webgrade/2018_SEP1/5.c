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
    sem_t dataProcessingFinished;
    int array[1024];
    unsigned arrayLen;
} OsInputData;

void greska(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

int usr1 = 0;
int usr2 = 0;

void signal_heandler(int signum)
{
    switch (signum) {
        case SIGUSR1: usr1 = 1; break;
        case SIGUSR2: usr2 = 1; break;
        default : greska("wrong signal sent");
    }
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

void promeni_znak(int *array, unsigned n)
{
    for (unsigned i = 0; i < n; i++)
        array[i] *= (-1);
}

void dupliraj(int *array, unsigned n)
{
    for (unsigned i = 0; i < n; i++)
        array[i] *= 2;
}

void stampaj_na_stderr(int *array, unsigned n)
{
    for (unsigned i = 0; i < n; i++)
        fprintf(stderr, "%d ", array[i]);

    fprintf(stderr, "\n");
}

// a.out memshared
int main(int argc, char **argv)
{
    if (argc != 2)
        greska("args failed");

    unsigned size = 0;
    OsInputData *p = get_memory_block(argv[1], &size);

    if (signal(SIGUSR1, signal_heandler) == SIG_ERR)
        greska("signal user1 failed");

    if (signal(SIGUSR2, signal_heandler) == SIG_ERR)
        greska("signal user2 failed");

    pause();

    if (usr1) {
        promeni_znak(p->array, p->arrayLen);
        stampaj_na_stderr(p->array, p->arrayLen);
        usr1 = 0;
    }

    if (usr2) {
        dupliraj(p->array, p->arrayLen);
        stampaj_na_stderr(p->array, p->arrayLen);
        usr2 = 0;
    }

    if (sem_post(&(p->dataProcessingFinished)) == -1)
        greska("sem_post failed");

    if (munmap(p, size) == -1)
        greska("munmap failed");



    exit(EXIT_SUCCESS);
}
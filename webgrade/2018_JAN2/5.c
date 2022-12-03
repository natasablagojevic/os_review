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

void *get_memory_block(char *path, unsigned *size)
{
    int fd = shm_open(path, O_RDWE, 0600);
        if (fd == -1)
            greska("shm_open failed");

    struct stat sb;
    if (fstat(path, &sb) == -1)
        greska("fstat failed");

    *size = sb.st_size;

    void *addr = mmap(NULL, *size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if (addr == MAP_FAILED)
            greska("mmap failed");

    close(fd);
    return addr;
}

void reverse(char *str)
{
    int n = strlen(s);
    char *pom = malloc(n+1);
        if (pom == NULL)
            greska("pom malloc failed");

    int k = 0;
    for (int i = n-1; i >= 0; i--)
        pom[k++] = str[i];

    pom[k] = 0;

    strcpy(str, pom);
    free(pom);
}

int main(int argc, char **argv)
{
    if (argc != 2)
        greska("args failed");

    unsigned size = 0;
    OsInputData *p = get_memory_block(argv[1], &size);

    if (sem_init(&(p->inDataReady), 1, 1) == -1)
        greska("sem_init failed");

    reverse(p->str);
    fprintf(stderr, "%s\n", p->str);

    if (sem_post(&(p->dataProcessed)) == -1)
        greska("sem_post failed");

    if (shm_unlink(argv[1]) == -1)
        greska("shm_unlink failed");

    if (munmap(p, size) == -1)
        greska("munmap failed");


    exit(EXIT_SUCCESS);
}
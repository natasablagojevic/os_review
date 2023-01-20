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
#include<string.h>

#define UNUSED(X) (void)(X)

typedef struct  
{
    sem_t inDataready;
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
    int fd = shm_open(path, O_RDWR, 0600);
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

void obrni(char *str)
{
    int n = strlen(str);
    int k = 0;
    char pom[1024];

    for (int i = n-1; i >= 0; i--)
        pom[k++] = str[i];

    pom[k] = 0;

    strcpy(str, pom);
}

int main(int argc, char **argv)
{
    if (argc != 2)
        greska("args failed");

    OsInputData *p = create_memory_block(argv[1], sizeof(OsInputData));

    if (sem_init(&(p->inDataready), 1, 1) == -1)
        greska("sem_init failed");

    
    strcpy(p->str, "neka_niska");


    if (sem_post(&(p->dataProcessed)) == -1)
        greska("sem_post failed");

    if (munmap(p, sizeof(OsInputData)) == -1)
        greska("munmap failed");

    exit(EXIT_SUCCESS);
}
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

    unsigned size = 0;
    OsInputData *p = get_memory_block(argv[1], &size);

    if (sem_wait(&(p->inDataready)) == -1)
        greska("sem_wait failed");

    
    obrni(p->str);
    fprintf(stderr, "%s\n", p->str);


    if (sem_post(&(p->dataProcessed)) == -1)
        greska("sem_post failed");

    if (munmap(p, size) == -1)
        greska("munmap failed");

    exit(EXIT_SUCCESS);
}
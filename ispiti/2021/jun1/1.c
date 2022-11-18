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

void greska(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

// a.out epoha
int main(int argc, char **argv)
{   
    if (argc != 2)
        greska("args failed");

    long epoha = atol(argv[1]);
    struct tm *vreme = localtime(&epoha);
        if (vreme == NULL)
            greska("localtime failed");

    long sati = 0;
    sati += vreme->tm_mday * 24;
    sati += vreme->tm_hour;

    printf("%d\n", vreme->tm_mday);
    printf("sati: %d, minuti: %d, sekundi: %d\n", vreme->tm_hour, vreme->tm_min, vreme->tm_sec);
    printf("%ld\n", sati);


    exit(EXIT_SUCCESS);
}
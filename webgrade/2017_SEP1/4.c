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

void get_day(int day)
{
    switch (day) {
        case 0: printf("nedeljda\n"); break;
        case 1: printf("ponedeljak\n"); break;
        case 2: printf("utorak\n"); break;
        case 3: printf("sreda\n"); break;
        case 4: printf("cetvrtak\n"); break;
        case 5: printf("petak\n"); break;
        case 6: printf("subota\n"); break;
    }
}

// a.out epoha
int main(int argc, char **argv)
{
    if (argc != 2)
        greska("args failed");

    long epoha = atol(argv[1]);
    struct tm *vreme = localtime(&epoha);
        if (vreme == NULL)
            greska("localtime");

    get_day(vreme->tm_wday);


    exit(EXIT_SUCCESS);
}
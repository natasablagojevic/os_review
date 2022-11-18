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

void get_dan(int dan)
{
    switch (dan) {
        case 0 : printf("nedelja "); break;
        case 1 : printf("pondeljak "); break;
        case 2 : printf("utorak "); break;
        case 3 : printf("sreda "); break;
        case 4 : printf("cetvrtak "); break;
        case 5 : printf("petak "); break;
        case 6 : printf("subota "); break;
    }
}

//  a.out epoha godina
int main(int argc, char **argv)
{
    if (argc != 3)
        greska("args failed");

    long epoha = atol(argv[1]);
    int godina = atoi(argv[2]);

    struct tm *vreme = localtime(&epoha);
        if (vreme == NULL)
            greska("localtime failed");

    get_dan(vreme->tm_wday);

    epoha += godina * 31556952;

    vreme = localtime(&epoha);
        if (vreme == NULL)
            greska("localtime 2 failed");

    get_dan(vreme->tm_wday);
    printf("\n");

    exit(EXIT_SUCCESS);
}
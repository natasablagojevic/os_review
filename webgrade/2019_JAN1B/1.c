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

void get_meseci(int mesec)
{
    switch (mesec) {
        case 0 : printf("januar "); break;
        case 1 : printf("februar "); break;
        case 2 : printf("mart "); break;
        case 3 : printf("april "); break;
        case 4 : printf("maj "); break;
        case 5 : printf("jun "); break;
        case 6 : printf("jul "); break;
        case 7 : printf("avgust "); break;
        case 8 : printf("septembar "); break;
        case 9 : printf("oktobar "); break;
        case 10: printf("novembar "); break;
        case 11: printf("decembar "); break;
    }
}

// a.out epoha dani
int main(int argc, char **argv)
{
    if (argc != 3)
        greska("args failed");

    long epoha = atol(argv[1]);
    int dani = atoi(argv[2]);

    struct tm *vreme = localtime(&epoha);
        if (vreme == NULL)
            greska("localtime failed 1");

    get_meseci(vreme->tm_mon);

    epoha += dani * 60*60*24;

    vreme = localtime(&epoha);
        if (vreme == NULL)
            greska("localtime failed 2");

    get_meseci(vreme->tm_mon);
    printf("\n");


    exit(EXIT_SUCCESS);
}
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

void get_mesec(int mesec)
{
    switch (mesec) {
        case 0 : printf("januar\n"); break;
        case 1 : printf("februar\n"); break;
        case 2 : printf("mart\n"); break;
        case 3 : printf("april\n"); break;
        case 4 : printf("maj\n"); break;
        case 5 : printf("jun\n"); break;
        case 6 : printf("jul\n"); break;
        case 7 : printf("avgust\n"); break;
        case 8 : printf("septembar\n"); break;
        case 9 : printf("oktobar\n"); break;
        case 10: printf("novembar\n"); break;
        case 11: printf("decembar\n"); break;
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
            greska("localtime failed");

    get_mesec(vreme->tm_mon);


    exit(EXIT_SUCCESS);
}
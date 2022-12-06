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

// a.out epoha minuti
int main(int argc, char **argv)
{
    if (argc != 3)
        greska("args failed");

    long epoha = atol(argv[1]);
    int minuti = atoi(argv[2]);

    epoha += (minuti * 60);

    struct tm *vreme = localtime(&epoha);
        if (vreme == NULL)
            greska("localtime failed");

    char result[1024];
    if (strftime(result, 1024, "%d/%m/%Y %R", vreme) == 0)
        greska("strptime failed");

    // strptime(result, "%D", vreme);

    printf("%s\n", result);

    exit(EXIT_SUCCESS);
}
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

int main(int argc, char **argv)
{
    if (argc != 2)
        greska("args filed");

    long epoha = atol(argv[1]);
    char buf[1024];

    struct tm *vreme = localtime(&epoha);
        if (vreme == NULL)
            greska("loclatime failed");

    if (strftime(buf, 1024, "%R", vreme) == 0)
        greska("strftime failed");

    printf("%s\n", buf);

    exit(EXIT_SUCCESS);
}
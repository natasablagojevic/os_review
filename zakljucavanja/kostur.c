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
#include<stdbool.h>
#include<string.h>

#define UNUSED(X) (void)(X)

void greska(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{


    exit(EXIT_SUCCESS);
}
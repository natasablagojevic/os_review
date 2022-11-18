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
#include<dirent.h>
#include<errno.h>

#define UNUSED(X) (void)(X)
#define RD_END 0
#define WR_END 1

void greska(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

unsigned size = 0;

void f(char *putanja)
{
    struct stat sb;
    if (lstat(putanja, &sb) == -1)
        greska("lstat failed");

    size += sb.st_size;

    if (!S_ISDIR(sb.st_mode))
        return ;

    printf("ROOT: %s:\n", putanja);

    DIR *dir = opendir(putanja);
        if (dir == NULL)
            greska("opendir failed");

    struct dirent *dirEntry = NULL;   
    errno = 0;

    while ((dirEntry = readdir(dir)) != NULL) {
        char *nova = malloc(strlen(dirEntry->d_name) + strlen(putanja) + 1);
            if (nova == NULL)
                greska("nova malloc failed");

        strcpy(nova, putanja);
        strcat(nova, "/");
        strcat(nova, dirEntry->d_name);

        printf("\t%s\n", nova);

        if (strcmp(dirEntry->d_name, ".") != 0 && strcmp(dirEntry->d_name, "..") != 0) {
            if (stat(nova, &sb) == -1)
                greska("stat failed 2");

            size += sb.st_size;

            free(nova);
            errno = 0;
            continue;
        }

        f(nova);
        free(nova);
        errno = 0;
    }

    if (errno == EBADF)
        greska("readdir failed");

    if (closedir(dir) == -1)
        greska("closedir failed");
}

int main(int argc, char **argv)
{

    f(argv[1]);


    exit(EXIT_SUCCESS);
}
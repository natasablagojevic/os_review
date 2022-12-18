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
#include<fcntl.h>
#include<errno.h>

#define UNUSED(X) (void)(X)
#define RD_END 0
#define WR_END 1
#define MAX 256

void greska(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

//  0     1    2    3
// a.out fajl rec1 rec2
int main(int argc, char **argv)
{
    if (argc != 4)
        greska("args failed");

    FILE *f = fopen(argv[1], "r");
        if (f == NULL)
            greska("fopen failed");

    struct flock lock;
    char *buf = NULL;
    int fd = fileno(f);
        if (fd == -1)
            greska("fileno failed");

    int counter = 0;
    
    while (fscanf(f, "%ms", &buf) == 1) {
        lock.l_type = F_WRLCK;
        lock.l_whence = SEEK_SET;
        lock.l_start = ftell(f);
        lock.l_len = -strlen(buf);

        if (strcmp(buf, argv[2]) != 0)
            continue;

        if (fcntl(fd, F_SETLK, &lock) == -1) {
            if (errno == EACCES || errno == EAGAIN)
                greska("fcntl failed");
            counter++;
        } else {
            if (fseek(f, -strlen(buf), SEEK_CUR) == -1)
                greska("fseek failed");

            fprintf(f, "%s", argv[3]);
        }
    }

    lock.l_type = F_UNLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;

    printf("%d\n", counter);

    fclose(f);
    exit(EXIT_SUCCESS);
}
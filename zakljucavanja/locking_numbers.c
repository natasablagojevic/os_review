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
#include<string.h>
#include<stdbool.h>
#include<errno.h>

#define UNUSED(X) (void)(X)

void greska(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

bool je_broj(char *s)
{
    int n = strlen(s);
    // bool broj = false;

    for (int i = 0; i < n; i++)
        if (!isdigit(s[i]))
            return false;

    return true;
}

// a.out file
int main(int argc, char **argv)
{   
    if (argc != 2)
        greska("args failed");

    char rec[256];

    int fd = open(argv[1], O_RDONLY);
        if (fd == -1)
            greska("open failed");

    FILE *f = fdopen(fd, "r");
        if (f == NULL)
            greska("fdopen failed");

    struct flock lock;
    int uspesno = 0;

    while (fscanf(f, "%s", rec) == 1) {
        // printf("%s\n", je_broj(rec) == true ? "true" : "false");
        if (je_broj(rec)) {
            lock.l_type = F_RDLCK;
            lock.l_whence = SEEK_SET;
            lock.l_len = -strlen(rec);
            lock.l_start = ftell(f);

            if (fcntl(fd, F_SETLK, &lock) == -1) {
                if (errno == EACCES || errno == EAGAIN)
                    greska("fcntl failed");
            } else 
                uspesno++;
        }
    }

    lock.l_type = F_UNLCK;
    lock.l_start = 0;
    lock.l_len = 0;
    lock.l_whence = SEEK_SET;

    if (fcntl(fd, F_SETLK, &lock) == -1)
        greska("fcntl failed");

    printf("%d\n", uspesno);


    fclose(f);
    close(fd);
    exit(EXIT_SUCCESS);
}
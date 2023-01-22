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
#include<errno.h>

#define UNUSED(X) (void)(X)

void greska(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

bool je_pozitivan_cetvorocifren(char *s)
{
    if (s[0] == '-')
        return false;

    if (strlen(s) != 4)
        return false;

    int n = strlen(s);
    for (int i = 0; i < s; i++)
        if (!isdigit(s[i]))
            return false;

    return true;
}

// a.out fajl
int main(int argc, char **argv)
{
    if (argc != 2)
        greska("args failed");

    int fd = open(argv[1], O_RDWR);
        if (fd == -1)
            greska("open failed");

    FILE *f = fdopen(fd, "rw");
        if (f == NULL)
            greska("fdopen failed");

    char rec[256];
    struct flock lock;

    while (fscanf(f, "%s", rec) == 1) {
        if (je_pozitivan_cetvorocifren(rec)) {
            lock.l_type = F_WRLCK;
            lock.l_whence = SEEK_SET;
            lock.l_start = ftell(f);
            lock.l_len = -strlen(rec);

            if (fcntl(fd, F_SETLK, &lock) == -1) {
                if (errno == EACCES || errno == EAGAIN)
                    greska("fcntl failed");
            } else {
                if (fseek(f, -strlen(rec), SEEK_SET) == -1)
                    greska("fseek failed");

                fprintf(f, "####");
            }
        }
    }

    fclose(f);
    close(fd);
    exit(EXIT_SUCCESS);
}
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
#include<errno.h>

#define UNUSED(X) (void)(X)
#define RD_END 0
#define WR_END 1

void greska(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

void swap(char *s)
{
    int n = strlen(s);
    char *pom = malloc(strlen(s) + 1);
        if (pom == NULL)
            greska("pom malloc failed");

    int k = 0;
    for (int i = n-1; i >= 0; i--)
        pom[k++] = s[i];

    pom[k] = 0;
    strcpy(s, pom);
    free(pom);
}

// a.out file
int main(int argc, char **argv)
{
    if (argc != 2)
        greska("args failed");

    struct stat sb;
    if (stat(argv[1], &sb) == -1)
        greska("stat failed");

    int fd = open(argv[1], O_RDWR);
        if (fd == -1)
            greska("open failed");

    FILE *f = fdopen(fd, "r+");
        if (f == NULL)
            greska("fdopen failed");

    char *buf = NULL;
    struct flock lock;
    int uspeh = 0;
    int neuspeh = 0;

    while (fscanf(f, "%ms", &buf) == 1) {
        lock.l_type = F_WRLCK;
        lock.l_whence = SEEK_SET;
        lock.l_start = ftell(f);
        lock.l_len = -strlen(buf);

        if (fcntl(fd, F_SETLK, &lock) == -1) {
            if (errno != EACCES && errno != EAGAIN)
                greska("fcntl failed");

            neuspeh++;
            continue;
        } else {
            if (fseek(f, -strlen(buf), SEEK_SET) == -1)
                greska("fseek failed");

            swap(buf);

            fprintf(f, "%s", buf);
            lock.l_type = F_UNLCK;
            if (fcntl(fd, F_SETLK, &lock) == -1)
                greska("fcntl2 failed");

            uspeh++;
        }
    } 

    if (feof(f) == -1)
        greska("fscanf failed");

    lock.l_type = F_UNLCK;

    fprintf(stdout, "%d %d\n", uspeh, neuspeh);

    fclose(f);
    exit(EXIT_SUCCESS);
}
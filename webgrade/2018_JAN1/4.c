#define _XOPEN_SOURCE 700
#define _GNU_SOURCE
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
#include<poll.h>

#define UNUSED(X) (void)(X)
#define RD_END 0
#define WR_END 1

typedef struct
{
    char nazivi[1024];
} Nazivi;

void greska(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

long long procitano(char *s)
{
    long long brojac = 0;

    for (int i = 0; s[i] != 0; i++)
        brojac++;

    return brojac; 
}

// a.out f1 f2 f3 ...
int main(int argc, char **argv)
{
    if (argc <= 1)
        greska("args failed");

    int nFds = argc - 1;
    struct pollfd *fds = malloc(nFds * sizeof(struct pollfd));
        if (fds == NULL)
            greska("fds malloc failed");

    Nazivi *n = malloc(nFds * sizeof(Nazivi));
        if (n == NULL)
            greska("n malloc failed");

    for (int i = 0; i < nFds; i++) {
        char *slash = strrchr(argv[i+1], '/');
            if (slash == NULL) {
                strcpy(n[i].nazivi, argv[i+1]);
            } else {
                slash++;
                strcpy(n[i].nazivi, slash);
            }
    }

    for (int i = 0; i < nFds; i++) {
        int fd = open(argv[i+1], O_RDONLY | O_NONBLOCK);
            if (fd == -1)
                greska("open failed");

        fds[i].fd = fd;
        fds[i].events = POLLIN | POLLERR | POLLHUP;
        fds[i].revents = 0;
    }

    int activeFds = nFds;
    long long max = LONG_LONG_MIN;
    char max_ime[1024];

    while (activeFds) {
        int ret_val = poll(fds, nFds, -1);
            if (ret_val == -1)
                greska("poll failed");

        for (int i = 0; i < nFds; i++) {
            if (fds[i].revents & POLLIN) {
                FILE *f = fdopen(fds[i].fd, "r");
                    if (f == NULL)
                        greska("fdopen failed");

                long long br = 0;
                char *linija = NULL;
                size_t size = 0;

                while (getline(&linija, &size, f) != -1)
                    br += procitano(linija);
                

                if (br > max) {
                    max = br;
                    strcpy(max_ime, n[i].nazivi);
                }

                fds[i].revents = 0;
            }

            if (fds[i].revents & (POLLERR | POLLHUP)) {
                close(fds[i].fd);

                fds[i].fd = -1;
                fds[i].events = 0;
                fds[i].revents = 0;

                activeFds--;
            }
        }
    }

    printf("%s\n", max_ime);

    free(n);
    free(fds);
    exit(EXIT_SUCCESS);
}
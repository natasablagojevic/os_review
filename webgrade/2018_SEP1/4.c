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
#include<poll.h>

#define UNUSED(X) (void)(X)
#define RD_END 0
#define WR_END 1

void greska(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

int numbers_of_a(char *s)
{
    int counter = 0;
    for (int i = 0; s[i] != 0; i++)
        if (s[i] == 'a')
            counter++;

    return counter;
}

int main(int argc, char **argv)
{
    if (argc <= 1)
        greska("args failed");

    int nFds = argc - 1;
    struct pollfd *fds = malloc(nFds * sizeof(struct pollfd));
        if (fds == NULL)
            greska("fds malloc failed");

    for (int i = 0; i < nFds; i++) {
        int fd = open(argv[i+1], O_RDONLY | O_NONBLOCK);
            if (fd == -1)
                greska("open failed");

        fds[i].fd = fd;
        fds[i].events = POLLIN | POLLERR | POLLHUP;
        fds[i].revents = 0;
    }

    int activeFds = nFds;
    int max = INT_MIN;
    char naziv[1024];

    while (activeFds) {
        int ret_val = poll(fds, 10, -1);
            if (ret_val == -1)
                greska("poll failed");

        for (int i = 0; i < nFds; i++) {
            if (fds[i].events & POLLIN) {
                FILE *f = fdopen(fds[i].fd, "r");
                    if (f == NULL)
                        greska("fdopen failed");

                int brojac = 0;
                char *buf = NULL;

                printf("passed\n");
                while (fscanf(f, "%ms", &buf) == 1)
                    brojac += numbers_of_a(buf);

                free(buf);

                if (brojac > max) {
                    max = brojac;
                    char *slash = strrchr(argv[i+1], '/');
                        if (slash == NULL) {
                            strcpy(naziv, argv[i+1]);
                        } else {
                            slash++;
                            strcpy(naziv, slash);
                        }
                }
            }

            if (fds[i].events & (POLLERR | POLLHUP)) {
                close(fds[i].fd);
                fds[i].fd = -1;

                fds[i].events = 0;
                fds[i].revents = 0;

                activeFds--;
            }
        }
    }

    printf("%s %d\n", naziv, max);


    free(fds);
    exit(EXIT_SUCCESS);
}
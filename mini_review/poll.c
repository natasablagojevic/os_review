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
        fds[i].events = POLLIN | POLLHUP | POLLERR;
        fds[i].revents = 0;
    }

    int activeFds = nFds;

    while (activeFds) {
        int ret_val = poll(fds, nFds, -1);
            if (ret_val == -1)
                greska("poll failed");

        for (int i = 0; i < nFds; i++) {
            if (fds[i].revents & POLLIN) {
                FILE *f = fdopen(fds[i].fd, "r");
                    if (f == NULL)
                        greska("fdopen failed");

                char *linija = NULL;
                size_t size = 0;

                while (getline(&linija, &size, f) != -1)
                    printf("%s", linija);

                free(linija);
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
    

    free(fds);
    exit(EXIT_SUCCESS);
}
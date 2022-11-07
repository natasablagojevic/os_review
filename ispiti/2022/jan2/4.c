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
#include<sys/epoll.h>

#define UNUSED(X) (void)(X)
#define RD_END 0
#define WR_END 1

typedef struct
{
    int fd;
    char *ime;
} Podaci;

void greska(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

int get_broj_cifara(char *s)
{
    int brojac = 0;
    int n = strlen(s);
    for (int i = 0; i < n; i++)
        if (isdigit(s[i]))
            brojac++;

    return brojac;
}

//  a.out f1 f2 f3..
int main(int argc, char **argv)
{
    if (argc <= 1)
        greska("args failed");

    int nFds = argc - 1;
    int epoll_fd = epoll_create(nFds);
    struct epoll_event *fds = malloc(nFds * sizeof(struct epoll_event));
        if (fds == NULL)
            greska("fds malloc failed");

    for (int i = 0; i < nFds; i++) {
        int fd = open(argv[i+1], O_RDONLY | O_NONBLOCK);
            if (fd == -1)
                greska("open failed");

        Podaci *p = malloc(sizeof(Podaci));
            if (p == NULL)
                greska("p malloc failed");

        p->fd = fd;
        char *slash = strrchr(argv[i+1], '/');
            if (slash == NULL) {
                // nema '/'
                p->ime = malloc(strlen(argv[i+1]) + 1);
                    if (p->ime == NULL)
                        greska("p->ime malloc failed 1");

                strcpy(p->ime, argv[1]);
            } else {
                slash++;
                p->ime = malloc(strlen(slash) + 1);
                    if (p->ime == NULL)
                        greska("p->ime malloc failed 2");

                strcpy(p->ime, slash);
            }

        fds[i].events = EPOLLIN | EPOLLHUP | EPOLLERR;
        fds[i].data.ptr = p;

        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &fds[i]) == -1)
            greska("epoll_ctl ADD failed");
    }

    int activeFds = nFds;
    struct epoll_event activeEvents[10];

    int max = INT_MIN;
    char ime[1024];

    while (activeFds) {
        int numActive = epoll_wait(epoll_fd, activeEvents, 10, -1);
            if (numActive == -1)
                greska("epoll_wait failed");

        for (int i = 0; i < numActive; i++) {
            if (activeEvents[i].events & EPOLLIN) {
                Podaci *p = (Podaci *)activeEvents[i].data.ptr;

                FILE *f = fdopen(p->fd, "r");
                    if (f == NULL)
                        greska("fdopen failed");
                
                int brojac = 0;
                char pom[1024];

                while (fscanf(f, "%s", pom) == 1)
                    brojac += get_broj_cifara(pom);

                if (brojac > max) {
                    max = brojac;
                    strcpy(ime, p->ime);
                }

            }

            if (activeEvents[i].events & (EPOLLHUP | EPOLLERR)) {
                Podaci *p = (Podaci *)activeEvents[i].data.ptr;

                if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, p->fd, &activeEvents[i]) == -1)
                    greska("epoll_ctl DEL failed");

                close(p->fd);
                p->fd = -1;
                free(p->ime);
                free(p);
                activeEvents[i].events = 0;

                activeFds--;
            }
        }
    }

    printf("%s %d", ime, max);


    free(fds);
    close(epoll_fd);
    exit(EXIT_SUCCESS);
}
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
#include<signal.h>

#define UNUSED(X) (void)(X)
#define RD_END 0
#define WR_END 1

void greska(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

int usr1 = 0;
int usr2 = 0;
int stop = 0;

void signal_heandler(int signum)
{
    switch (signum) {
        case SIGUSR1 : usr1 = 1; break;
        case SIGUSR2 : usr2 = 1; break;
        case SIGQUIT : stop = 1; break;
    }
}

int palindrom(char *s)
{
    int n = strlen(s);

    for (int i = 0; i < n; i++)
        if (s[i] != s[n-1-i])
            return 0;

    return 1;
}

void obrni(char *s)
{
    int n = strlen(s);
    char pom[1024];
    int k = 0;

    for (int i = n-1; i >= 0; i--)
        pom[k++] = s[i];

    pom[k] = 0;

    printf("%s ", pom);
}

int main(int argc, char **argv)
{
    UNUSED(argc);
    UNUSED(argv);

    char rec[1024];

    while (true) {
        scanf("%s", rec);

        if (signal(SIGUSR1, signal_heandler) == SIG_ERR)
            greska("signal user1 failed");

        if (signal(SIGUSR2, signal_heandler) == SIG_ERR)
            greska("signal user2 failed");

        if (signal(SIGQUIT, signal_heandler) == SIG_ERR)
            greska("signal quit failed");

        pause();

        if (stop)
            exit(EXIT_SUCCESS);

        if (usr1) {
            printf("%d ", palindrom(rec));
            usr1 = 0;
        }

        if (usr2) {
            obrni(rec);
            usr2 = 0;
        }
    }


    exit(EXIT_SUCCESS);
}
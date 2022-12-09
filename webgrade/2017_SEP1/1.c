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

#define UNUSED(X) (void)(X)
#define RD_END 0
#define WR_END 1
#define MAX 63

void greska(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

int usr1 = 0;
int usr2 = 0;
int term = 0;

void signal_heandler(int signum)
{
    switch (signum) {
        case SIGUSR1: usr1 = 1; break;
        case SIGUSR2: usr2 = 1; break;
        case SIGTERM: term = 1; break;
    }
}

void obrni(char *str)
{
    int n = strlen(str);
    char pom[MAX];
    int k = 0;

    for (int i = n-1; i >= 0; i--)
        pom[k++] = str[i];

    pom[k] = 0;
    strcpy(str, pom);
}

void mala_u_velika(char *str)
{
    for (int i = 0; str[i] != 0; i++)
        if (islower(str[i]))
            str[i] = toupper(str[i]);
}

int main()
{
    char *rec = malloc(MAX);
        if (rec == NULL)
            greska("rec malloc failed");

    while (true) {
        scanf("%s", rec);

        if (signal(SIGUSR1, signal_heandler) == SIG_ERR)
            greska("signal user1 failed");

        if (signal(SIGUSR2, signal_heandler) == SIG_ERR)
            greska("signal user2 failed");

        if (signal(SIGTERM, signal_heandler) == SIG_ERR)
            greska("signal term failed");

        pause();

        if (term)
            exit(EXIT_SUCCESS);

        if (usr1) {
            obrni(rec);
            printf("%s\n", rec);
            usr1 = 0;
        }

        if (usr2) {
            mala_u_velika(rec);
            printf("%s\n", rec);
            usr2 = 0;
        }
    }


    free(rec);
    exit(EXIT_SUCCESS);
}
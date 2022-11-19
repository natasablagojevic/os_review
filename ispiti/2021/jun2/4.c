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

#define UNUSED(X) (void)(X)
#define RD_END 0
#define WR_END 1

void greska(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

// a.out fajl
int main(int argc, char **argv)
{
    if (argc != 2)
        greska("args failed");

    FILE *f = fopen(argv[1], "r");
        if (f == NULL)
            greska("fopen failed");

    char *linija = NULL;
    char komanda[1024];
    size_t velicina = 0;
    int max = INT_MIN;
    char komanda_max[1024];

    while (getline(&linija, &velicina, f) != -1) {
        sscanf(linija, "%s", komanda);

        printf("komanda: %s\n", komanda);

        int cld2Par[2];
        if (pipe(cld2Par) == -1)
            greska("pipe failed");

        pid_t childPid = fork();
            if (childPid == -1)
                greska("fork failed");

        if (childPid > 0) {
            /* PARENT */

            close(cld2Par[WR_END]);

            int read_bytes = 0;
            char buf[1025];

            while ((read_bytes = read(cld2Par[RD_END], buf, 1024)) > 0){

            }

            if (read_bytes == -1)
                greska("read failed");

            if (read_bytes > max) {
                max = read_bytes;
                strcpy(komanda_max, komanda);
            }

            close(cld2Par[RD_END]);
        } else {
            /* CHILD */

            close(cld2Par[RD_END]);

            if (dup2(cld2Par[WR_END], STDOUT_FILENO) == -1)
                greska("dup2 failed");

            if (execvp(komanda, &linija) == -1)
                greska("execvp failed");

            close(cld2Par[WR_END]);
            exit(EXIT_FAILURE);
        }

        int status = 0;

        if (wait(&status) == -1)
            greska("wait failed");

        if (WIFEXITED(status) && WEXITSTATUS(status) == EXIT_SUCCESS) 
            fprintf(stderr, "uspeh\n");
        else 
            fprintf(stderr, "neuspeh\n");

        close(cld2Par[WR_END]);
        close(cld2Par[RD_END]);
    }


    exit(EXIT_SUCCESS);
}
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

void greska(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

// a.out fajl1 fajl2 ...
int main(int argc, char **argv)
{
    if (argc <= 1)
        greska("args failed");

    int n = argc - 1;

    for (int i = 0; i < n; i++) {
        int cld2Par[2];
        if (pipe(cld2Par) == -1)
            greska("pipe failed");

        pid_t childPid = fork();
            if (childPid == -1)
                greska("fork failed");

        if (childPid > 0) {
            /* PARENT */

            close(cld2Par[WR_END]);

            FILE *f = fdopen(cld2Par[RD_END], "r");
                if (f == NULL)
                    greska("fdopen failed");

            char *buf = NULL;
            fscanf(f, "%ms", &buf);

            printf("%s ", buf);
            
            fclose(f);
            free(buf);
            close(cld2Par[RD_END]);
        } else {
            /* CHILD */

            close(cld2Par[RD_END]);

            if (dup2(cld2Par[WR_END], STDOUT_FILENO) == -1)
                greska("dup2 failed");

            struct stat sb;
            if (stat(argv[i+1], &sb) == -1)
                exit(EXIT_FAILURE);

            if (execlp("du", "du", "-sch", argv[i+1], NULL) == -1)
                greska("execlp failed");

            close(cld2Par[WR_END]);
            exit(EXIT_FAILURE);
        }

        int status = 0;
        if (wait(&status) == -1)
            greska("wait failed");

        if (WIFEXITED(status)) {
            if (WEXITSTATUS(status) == EXIT_FAILURE) 
                fprintf(stdout, "neuspeh ");
            else 
                fprintf(stderr, "*uspeh*\n");
        }

        close(cld2Par[RD_END]);
        close(cld2Par[WR_END]);
    }


    exit(EXIT_SUCCESS);
}
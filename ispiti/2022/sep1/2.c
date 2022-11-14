#define _XOPEN_SOURCE 700
#include<stdio.h>
#include<stdlib.h>
#include<limits.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<ftw.h>
#include<sys/mman.h>
#include<sys/wait.h>
#include<time.h>
#include<string.h>
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

// a.out n fajl
int main(int argc, char **argv)
{
    if (argc != 3)
        greska("args failed");

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

        int n = 0;
        int max = INT_MIN;
        char max_buf[1024];
        size_t linelen = 0;
        char *line = NULL;

        while ((n = getline(&line, &linelen, f)) != -1) {
            if (n > max) {
                max = n;
                strcpy(max_buf, line);
            }
        }

        printf("%s\n", max_buf);
        // free(max_buf);
        free(line);
        close(cld2Par[RD_END]);
    } else {
        /* CHILD */

        close(cld2Par[RD_END]);
        
        if (dup2(cld2Par[WR_END], STDOUT_FILENO) == -1)
            greska("dup2 failed");

        if (execlp("head", "head", "-n", argv[2], argv[1], NULL) == -1)
            greska("execlp failed");

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

    exit(EXIT_SUCCESS);
}
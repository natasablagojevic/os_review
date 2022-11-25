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

// a.out fajl a
int main(int argc, char **argv)
{
    if (argc != 3) {
        fprintf(stdout, "Neuspeh\n");
        greska("args failed");
    }
    
    struct stat sb;
    if (stat(argv[1], &sb) == -1){
        fprintf(stdout, "Neuspeh\n");
        // greska("stat");
        exit(EXIT_SUCCESS);
    }

    int cld2Par[2];
    if (pipe(cld2Par) == -1)
        greska("pipe failed");

    pid_t childPid = fork();
        if (childPid == -1)
            greska("fork failed");

    if (childPid == 0) {
        /* CHILD */
        close(cld2Par[RD_END]);

        if (dup2(cld2Par[WR_END], STDOUT_FILENO) == -1)
            greska("dup2 failed");

        if (execlp("tail", "tail", "-n", argv[2], argv[1], NULL) == -1)
            greska("execlp failed");

        close(cld2Par[WR_END]);
        exit(EXIT_FAILURE);
    } else {
        /* PARENT */
        close(cld2Par[WR_END]);

        FILE *f = fdopen(cld2Par[RD_END], "r");
            if (f == NULL)
                greska("fdopen failed");

        char *linija = NULL;
        size_t size = 0;

        while (getline(&linija, &size, f) != -1)
            printf("%s", linija);

        free(linija);
        fclose(f);
        close(cld2Par[RD_END]);
    }

    int status = 0;
    if (wait(&status) == -1)
        greska("wait failed");

    if (WIFEXITED(status)) {
        if (WEXITSTATUS(status) == EXIT_FAILURE)
            fprintf(stdout, "Neuspeh\n");
        else 
            fprintf(stderr, "uspeh\n");
    } else {
        fprintf(stdout, "Neuspeh\n");
    }


    exit(EXIT_SUCCESS);
}
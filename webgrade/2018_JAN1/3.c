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

// a.out fajl -w/-l/-c
int main(int argc, char **argv)
{
    if (argc != 3)
        greska("args failed");

    if (strcmp(argv[2], "-c") != 0 && strcmp(argv[2], "-l") != 0 && strcmp(argv[2], "-w") != 0)
        fprintf(stderr, "wrong command"), exit(EXIT_FAILURE);

    struct stat sb;
    if (stat(argv[1], &sb) == -1)
        exit(EXIT_FAILURE);

    int cld2Par[2];
    if (pipe(cld2Par) == -1)
        greska("pipe failed");

    pid_t childPid = fork();
        if (childPid == -1)
            greska("fork failed");

    // if (childPid > 0) {
        /* PARENT */
        close(cld2Par[WR_END]);

        FILE *f = fdopen(cld2Par[RD_END], "r");
            if (f == NULL)
                greska("fdopen failed");

        char *linija = NULL;
        size_t size = 0;
        
        if (getline(&linija, &size, f) == -1)
            greska("getline failed");

        char pompom[1024];
        int k = 0;

        for (int i = 0; linija[i] != 0; i++) {
            if (isspace(linija[i]))
                break;
            pompom[k++] = linija[i];
        }

        pompom[k] = 0;

        printf("%s\n", pompom);

        free(linija);
        close(cld2Par[RD_END]);

    // } else {
    if (childPid == 0){
        /* CHILD */

        close(cld2Par[RD_END]);

        // struct stat sb;
        // if (stat(argv[1], &sb) == -1)
        //    exit(EXIT_FAILURE);

        if (dup2(cld2Par[WR_END], STDOUT_FILENO) == -1)
            greska("dup2 failed");

        if (execlp("wc", "wc", argv[2], argv[1], NULL) == -1)
            greska("execlp failed");

        close(cld2Par[WR_END]);
        exit(EXIT_FAILURE);
    }   

    int status = 0;
    if (wait(&status) == -1)
        greska("wait failed");

    if (WIFEXITED(status)) {
        if (WEXITSTATUS(status) == EXIT_FAILURE)
            fprintf(stdout, "Neuspeh\n");
        else 
            fprintf(stderr, "Uspeh\n");
    }

    close(cld2Par[RD_END]);
    close(cld2Par[WR_END]);

    exit(EXIT_SUCCESS);
}
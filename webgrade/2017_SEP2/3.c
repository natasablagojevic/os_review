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

int main()
{
    char **command = malloc(3 * sizeof(char *));
        if (command == NULL)
            greska("command malloc failed");

    for (int i = 0; i < 3; i++) {
        command[i] = malloc(256 * sizeof(char));
            if (command[i] == NULL) {
                for (int j = 0; j < i; j++)
                    free(command[j]);

                free(command);
                greska("command[i] malloc failed");
            }
    }

    while (scanf("%s%s%s", command[0], command[1], command[2]) != EOF) {
        int cld2Par[2];
        if (pipe(cld2Par) == -1)
            greska("pipe failed");

        pid_t childPid = fork();
            if (childPid == -1)
                greska("fork failed");

        if (childPid == 0) {
            close(cld2Par[RD_END]);

            if (dup2(cld2Par[WR_END], STDOUT_FILENO) == -1)
                greska("dup2 failed");

            if (execlp("expr", "expr", command[0], command[2], command[1], NULL) == -1)
                exit(EXIT_FAILURE);

            close(cld2Par[WR_END]);
            exit(EXIT_FAILURE);
        } else {
            close(cld2Par[WR_END]);

            char *linija = NULL;
            size_t size = 0;

            FILE *f = fdopen(cld2Par[RD_END], "r");
                if (f == NULL)
                    greska("fdopen failed");

            if (getline(&linija, &size, f) == -1)
                greska("getline failed");
            
            // char *buf = NULL;
            // fscanf(f, "%ms", &buf);

            printf("%s", linija);
            
            fclose(f);
            // free(buf);
            close(cld2Par[RD_END]);
        }

        int status = 0;
        if (wait(&status) == -1)
            greska("wait failed");

        if (WIFEXITED(status)) {
            if (WEXITSTATUS(status) == EXIT_SUCCESS)
                fprintf(stderr, "uspeh\n");
            else 
                fprintf(stderr, "neuspeh\n");
        } else 
            fprintf(stderr, "neuspeh\n");
    }



    exit(EXIT_SUCCESS);
}
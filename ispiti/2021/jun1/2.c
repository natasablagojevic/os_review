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

// a.out putanja
int main(int argc, char **argv)
{   
    if (argc != 2)
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

        char *linija = NULL;
        size_t linijalen = 0;
        char pom[1024];
        char pompom[1024];

        while (getline(&linija, &linijalen, f) != -1) {
            // printf("%s", linija);

            sscanf(linija, "%s%s", pom, pompom);

            // printf("%s %s\n", pom, pompom);

            if (strcmp(pom, "Access:") == 0)
                break;
        }

        // printf("------------\n");
        //printf("%s %s\n", pom, pompom);

        int n = strlen(pompom);
        int k = 0;
        char result[1024];

        for (int i = 6; i < n-1; i++)
            result[k++] = pompom[i];

        result[k] = 0;
        printf("%s\n", result);


        close(cld2Par[WR_END]);
    } else {
        /* CHILD */
        close(cld2Par[RD_END]);
        
        if (dup2(cld2Par[WR_END], STDOUT_FILENO) == -1)
            greska("dup2 failed");

        if (execlp("stat", "stat", argv[1], NULL) == -1)
            greska("exelp failed");

        close(cld2Par[WR_END]);
        exit(EXIT_FAILURE);
    }   

    int status = 0;
    if (wait(&status) == -1)
        greska("wait failed");

    if (WIFEXITED(status) && WEXITSTATUS(status) == EXIT_SUCCESS)
        fprintf(stderr, "uspeh\n");
    else 
        fprintf(stdout, "neuspeh\n");

    


    close(cld2Par[RD_END]);
    close(cld2Par[WR_END]);
    exit(EXIT_SUCCESS);
}
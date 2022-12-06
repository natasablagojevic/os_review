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
#define MAX 256

void greska(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

// a.out putanja programa
// a.out ls -l -a
int main(int argc, char **argv)
{
    if (argc <= 1)
        greska("args failed");

    int n = argc - 1;
    char **komanda = malloc(n * sizeof(char *));
        if (komanda == NULL)
            greska("komanda malloc failed");

    for (int i = 0; i < n; i++) {
        komanda[i] = malloc(MAX);
            if (komanda[i] == NULL) {
                for (int j = 0; j < i; j++)
                    free(komanda[j]);
                free(komanda);
                greska("komanda[i] malloc failed");
            }
    }

    for (int i = 0; i < n; i++)
        strcpy(komanda[i], argv[i+1]);

    // printf("-----------------\n");
    // for (int i = 0; i < n; i++)
    //     printf("%s\n", komanda[i]);

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
        size_t size = 0;
        int brojac = 0;

        while (getline(&linija, &size, f) != -1)
            brojac++;

        printf("%d\n", brojac);

        free(linija);
        fclose(f);
        close(cld2Par[RD_END]);
    } else {
        /* CHILD */
        close(cld2Par[RD_END]);

        if (dup2(cld2Par[WR_END], STDOUT_FILENO) == -1)
            greska("dup2 failed");

        if (execvp(komanda[0], komanda) == -1)
            exit(EXIT_FAILURE); 


        close(cld2Par[WR_END]);
        exit(EXIT_FAILURE);
    }

    int status = 0;
    if (wait(&status) == -1)
        greska("wait failed");

    if (WIFEXITED(status)) {
        if (WEXITSTATUS(status) == EXIT_SUCCESS)
            fprintf(stderr, "uspeh");
        else 
            fprintf(stdout, "Neuspeh\n");
    }


    for (int i = 0; i < n; i++)
        free(komanda[i]);
    free(komanda);
    exit(EXIT_SUCCESS);
}
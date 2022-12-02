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

int number_of_lines(FILE *f)
{
    int brojac = 0;
    char *linija = NULL;
    size_t size = 0;

    while (getline(&linija, &size, f) != -1)
        brojac++;

    return brojac;
}

int main(int argc, char **argv)
{
    if (argc != 2)
        greska("args failed");

    struct stat sb;
    if (stat(argv[1], &sb) == -1) {
        fprintf(stdout, "Neuspeh\n");
        exit(EXIT_FAILURE);
    }

    FILE *f = fopen(argv[1], "r");
        if (f == NULL)
            greska("fopen failed");

    int n = number_of_lines(f);

    if (fseek(f, SEEK_SET, 0) == -1)
        greska("fseek failed");

    char **komande = malloc(n * sizeof(char *));
        if (komande == NULL)
            greska("komande malloc failed");

    for (int i = 0; i < n; i++) {
        komande[i] = malloc(256);
            if (komande[i] == NULL) {
                for (int j = 0; j < i; j++)
                    free(komande[j]);
                free(komande);
                greska("komande[i] malloc failed");
            }
    }    

    char **argumenti = malloc(n * sizeof(char *));
        if (argumenti == NULL) 
            greska("argukmenti malloc failed");

    for (int i = 0; i < n; i++) {
        argumenti[i] = malloc(256);
            if (argumenti[i] == NULL) {
                for (int j = 0; j < i; j++)
                    free(argumenti[j]);
                free(argumenti);
                greska("argumenti[i] malloc failed");
            }
    }

    for (int i = 0; i < n; i++) {
        if (fscanf(f, "%s%s", komande[i], argumenti[i]) != 2)
            greska("treba tacno dva");
    }

    // printf("-------\n");
    // for (int i = 0; i < n; i++)
    //     printf("%s %s\n", komande[i], argumenti[i]);

    FILE *error = fopen("error.txt", "a+");
        if (error == NULL)
            greska("error fopen failed");

    for (int i = 0; i < n; i++) {
        int cld2Par[2];
        if (pipe(cld2Par) == -1)
            greska("pipen failed");

        pid_t childPid = fork();
            if (childPid == -1)
                greska("fork failed");

        // char err[4094];

        if (childPid == 0) {
            /* CHILD */
            close(cld2Par[RD_END]);

            if (dup2(cld2Par[WR_END], STDERR_FILENO) == -1)
                greska("dup2 failed");

            if (execlp(komande[i], komande[i], argumenti[i], NULL) == -1)
                greska("execlp failed");

            close(cld2Par[WR_END]);
            exit(EXIT_SUCCESS);
        }
        // else {
        //     close(cld2Par[WR_END]);

        //     FILE *fpom = fdopen(cld2Par[RD_END], "r");
        //         if (fpom == NULL)
        //             greska("fdopedn failed");

        //     char *linija = NULL;
        //     size_t size = 0;

        //     while (getline(&linija, &size, f) != -1)
        //         strcat(err, )

        //     close(cld2Par[RD_END]);
        // }

        close(cld2Par[WR_END]);

        int status = 0;
        if (wait(&status) == -1)
            greska("wait failed");

        if (WIFEXITED(status)) {
            if (WEXITSTATUS(status) == EXIT_FAILURE) {
                FILE *fpom = fdopen(cld2Par[RD_END], "r");
                    if (fpom == NULL)
                        greska("fdopen failed");

                char *linija = NULL;
                size_t size = 0;

                while (getline(&linija, &size, fpom) != -1)
                    fprintf(error, "%s\n", linija);
            } else {
                fprintf(stderr, "USPEH\n");
            }
        }

        close(cld2Par[RD_END]); 
    }


    for (int i = 0; i < n; i++)
        free(komande[i]), free(argumenti[i]);
    free(komande);
    free(argumenti);
    exit(EXIT_SUCCESS);
}
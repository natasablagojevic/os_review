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

int get_numbers_of_lines(FILE *f)
{
    char *line = NULL;
    size_t size = 0;
    int counter = 0;

    while (getline(&line, &size, f) != -1)
        counter++;

    free(line);

    return counter;
}

// a.out fajl
int main(int argc, char **argv)
{
    if (argc != 2)
        greska("args failed");

    FILE *f = fopen(argv[1], "r");
        if (f == NULL)
            greska("fopen failed");


    int n = get_numbers_of_lines(f);
    
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
            greska("argumenti malloc failed");

    for (int i = 0; i < n; i++) {
        argumenti[i] = malloc(256);
            if (argumenti[i] == NULL) {
                for (int j = 0; j < i; j++)
                    free(argumenti[j]);
                free(argumenti);
                greska("argumenti[i] malloc failed");
            }
    }

    if (fseek(f, 0, SEEK_SET) == -1)
        greska("fseek failed");

    for (int i = 0; i < n; i++) {
        fscanf(f, "%s%s", komande[i], argumenti[i]);
    }

    // fscanf(f, "%s", komanda);
    // printf("komanda: %s\n%d\n", komanda, n);
    // for (int i = 0; i < n; i++)
    //     printf("%s %s\n", komande[i], argumenti[i]);

    int max = INT_MIN;
    char komanda[256];
    char argument[256];
    
    for (int i = 0; i < n; i++) {
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

            if (execlp(komande[i], komande[i], argumenti[i], NULL) == -1)
                greska("execlp failed"); 

            close(cld2Par[WR_END]);
            exit(EXIT_FAILURE);
        } else {
            /* PARENT */
            close(cld2Par[WR_END]);
            
            char buf[1024*1024+1];
            int read_bytes = read(cld2Par[RD_END], buf, 1024*1024);
                if (read_bytes == -1)
                    greska("read failed");

            if (read_bytes > max) {
                max = read_bytes;
                strcpy(komanda, komande[i]);
                strcpy(argument, argumenti[i]);
            }

            close(cld2Par[RD_END]);
        }

        int status = 0;
        if (wait(&status) == -1)
            greska("wait failed");

        if (WIFEXITED(status) && WEXITSTATUS(status) == EXIT_SUCCESS)
            fprintf(stderr, "upeh\n");
        else 
            fprintf(stderr, "neuspeh\n");   

        close(cld2Par[WR_END]);
        close(cld2Par[RD_END]);
    }

    printf("%s %s\n", komanda, argument);
    

    for (int i = 0; i < n; i++)
        free(komande[i]), free(argumenti[i]);
    free(komande);
    free(argumenti);
    fclose(f);
    exit(EXIT_SUCCESS);
}
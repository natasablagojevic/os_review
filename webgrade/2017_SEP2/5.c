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

void print_time(char *s)
{
    char *dri = NULL;
    char *nbr = NULL;
    char *user = NULL;
    char *group = NULL;
    char *size = NULL;
    char *month = NULL;
    char *day = NULL;
    char *time = NULL;
    char *file = NULL;

    sscanf(s, "%ms%ms%ms%ms%ms%ms%ms%ms%ms", &dri, &nbr, &user, &group, &size, &month, &day, &time, &file);

    printf("%s\n", dri);
    printf("%s\n", nbr);
    printf("%s\n", user);
    printf("%s\n", group);
    printf("%s\n", size);
    printf("%s\n", month);
    printf("%s\n", day);
    printf("%s\n", time);
    printf("%s\n", file);
    printf("--------------------------\n");

    free(dri), free(nbr), free(user);
    free(group), free(size), free(month), free(day);
    free(time), free(file);
}

int main()
{
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

        while (getline(&linija, &size, f) != -1) {
            print_time(linija);
        }

        free(linija);
        fclose(f);
        close(cld2Par[RD_END]);
    } else {
        /* CHILD */
        close(cld2Par[RD_END]);

        if (dup2(cld2Par[WR_END], STDOUT_FILENO) == -1)
            greska("dup2 failed");

        if (execlp("ls", "ls", "-l", NULL) == -1)
            exit(EXIT_FAILURE);

        close(cld2Par[WR_END]);
        exit(EXIT_FAILURE);
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


    exit(EXIT_SUCCESS);
}
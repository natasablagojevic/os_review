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

/* 
    0 1 2 = 3
    m o v
 */

bool mov_komanda(char *s)
{
    char pom[3];
    int i;
    for (i = 0; i < 3; i++)
        pom[i] = s[i];

    pom[i] = 0;

    return strcmp(pom, "mov") == 0 ? true : false;
}

//  a.out c-fajl
int main(int argc, char **argv)
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

        FILE *f = fopen("2.s", "r");
            if (f == NULL)
                greska("fopen failed");

        // char *line = NULL;
        char line[1025];
        // size_t linelen = 0;
        int brojac = 0;
        char pom[1024];

        // while (getline(&line, &linelen, f) != -1) {
        //     // printf("%s", line);
        //     sscanf(line, "mov%s", pom);
        //     printf("%s", pom);
        // }

        while (fgets(line, 1024, f) != NULL) {
            // printf("%s", line);

            sscanf(line, "%s", pom);

            if (mov_komanda(pom))
                brojac++;
        }

        printf("%d\n", brojac);

        close(cld2Par[RD_END]);
    } else {
        /* CHILD */

        close(cld2Par[RD_END]);

        if (dup2(cld2Par[WR_END], STDOUT_FILENO) == -1)
            greska("dup2 failed");

        if (execlp("gcc", "gcc", argv[1], "-S", "-o", "2.s", NULL) == -1)
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
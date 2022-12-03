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
    0 1 2 3 4 5 6 7 8 9
    - r w - r w - r - -
 */

void privilegije(char *linija)
{
    char *prava = NULL;
    char *count = NULL;
    char *user = NULL;
    char *group = NULL;
    char *size = NULL;
    char *month = NULL;
    char *date = NULL;
    char *_time = NULL;
    char *name = NULL;

    sscanf(linija, "%ms%ms%ms%ms%ms%ms%ms%ms%ms", &prava, &count, &user, &group, &size, &month, &date, &_time, &name);

    // printf("prava: %s\tfajl: %s\n", prava, name);

    if (prava[7] == 'r' && prava[8] == 'w' && prava[9] == 'x')
        printf("%s\n", name);

    free(prava), free(count), free(user), free(group);
    free(size), free(month), free(date), free(_time), free(name);
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
        char *buf = NULL;
        // int id = 0;

        while (getline(&linija, &size, f) != -1) {
            // if (id == 0){
            //     id = 1;
            //     continue;
            // }

            // sscanf(linija, "%ms", &buf);
            // printf("buf: %s\n", buf);

            privilegije(linija);
        }
        
        free(buf);
        free(linija);
        fclose(f);
        close(cld2Par[RD_END]);
    } else {
        /* CHILD */

        close(cld2Par[RD_END]);

        if (dup2(cld2Par[WR_END], STDOUT_FILENO) == -1)
            greska("dup2 failed");

        if (execlp("ls", "ls", "-l", NULL) == -1)
            greska("execlp failed");

        close(cld2Par[WR_END]);
        exit(EXIT_FAILURE); 
    }

    int status = 0;
    if (wait(&status) == -1)
        greska("wait failed");

    if (WIFEXITED(status)) {
        if (WEXITSTATUS(status) == EXIT_FAILURE)
            fprintf(stdout, "neuspeh\n");
        else 
            fprintf(stderr, "uspeh\n");
    }


    exit(EXIT_SUCCESS);
}
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

void dekomponuj(char *linija, char **result, int n)
{
    int i = 0;
    int k = 1;
    int alocirano = k;

    result = malloc(k * sizeof(char *));
        if (result == NULL)
            greska("result malloc failed");

    result[0] = malloc(MAX);
        if (result[0] == NULL)
            greska("result[0] malloc failed");

    char *buf = NULL;

    while (sscanf(linija, "%ms", &buf) != EOF)  {
        if (i == alocirano) {
            alocirano += k;

            result = realloc(result, alocirano * sizeof(char *));
                if (result == NULL)
                    greska("result realloc failed");

            for (int k = i; i < alocirano; k++) {
                result[k] = malloc(MAX);
                    if (result[k] == NULL) {
                        for (int j = 0; j < k; j++)
                            free(result[j]);
                        free(result);
                        greska("result[k] malloc failed");
                    }
            }
        }

        strcpy(result[i], buf);
        i++;
    }    

    n = i;

    free(buf);
}

// a.out fajl
int main(int argc, char **argv)
{
    if (argc != 2)
        greska("args failed");

    FILE *f = fopen(argv[1], "r");  
        if (f == NULL)
            greska("fopen failed");

    char *linija = NULL;
    size_t size = 0;
    int max = INT_MIN;
    char *max_ime = malloc(MAX);
        if (max_ime == NULL)
            greska("max_ime malloc failed");

    while (getline(&linija, &size, f) != -1) {
        // sscanf(linija, "%s", komanda);
        char **komande = NULL;
        int n = 0;
        dekomponuj(linija, komande, n);

        int cld2Par[2];
        if (pipe(cld2Par) == -1)
            greska("pipe failed");

        pid_t childPid = fork();
            if (childPid == -1)
                greska("fork failed");

        if (childPid == 0) {
            // CHILD

            close(cld2Par[RD_END]);

            if (dup2(open("/dev/null", O_WRONLY), STDOUT_FILENO) == -1)
                greska("dup2 failed");


            if (execvp(komande[0], komande) == -1)
                exit(EXIT_FAILURE);


            close(cld2Par[WR_END]);
            exit(EXIT_FAILURE);
        } else {
            // PARENT
            close(cld2Par[WR_END]);

            int read_bytes = 0;
            // if (open(cld2Par[RD_END], O_RDONLY) == -1)
            //     greska("open failed");

            int sum = 0;
            char buf[1024];
            while ((read_bytes = read(cld2Par[RD_END], buf, 1023)) > 0)
                sum += read_bytes;

            if (read_bytes == -1)
                greska("read failed");

            if (sum > max) {
                max = sum;
                strcpy(max_ime, komande[0]);
            }     

            for (int i = 0; i < n; i++)
                free(komande[i]);
            free(komande);

            close(cld2Par[RD_END]);
        }

        int status = 0;
        if (wait(&status) == -1)
            greska("wait failed");

        if (WIFEXITED(status)) {
            if (WEXITSTATUS(status) == EXIT_SUCCESS)
                fprintf(stderr, "Uspeh\n");
            else 
                fprintf(stderr, "Neuspeh\n");
        }
    }

    printf("%s %d\n", max_ime, max);

    // free(komanda);
    free(linija);
    free(max_ime);
    fclose(f);
    exit(EXIT_SUCCESS);
}
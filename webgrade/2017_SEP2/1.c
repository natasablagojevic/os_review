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
#include<errno.h>

#define UNUSED(X) (void)(X)
#define RD_END 0
#define WR_END 1

typedef struct 
{
    char *gen;
    int start;
    int end;
} InputData;

void greska(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

void check_pthread(int errNum, const char *msg)
{
    int _err = errNum;
    if (_err > 0) {
        errno = _err;
        greska(msg);
    }
}

int global_array[2048];
int N = 0;

void get_genom(char *sekvenca, char *result, int index)
{
    int n = strlen(sekvenca);
    int k = 0;
    for (int i = 0; i < n; i++) {
        if (k == 2)
            break;

        result[k++] = sekvenca[i];
    }

    result[k] = 0;
}

void *function(void *arg)
{
    InputData *p = (InputData *)arg;
    char gen[4];
    
    for (int i = p->start; i < p->end; i++) {
        get_genom(p->gen, gen);
        printf("gen: %s\n", gen);
    }


    return NULL;
}

// a.out numTids file
int main(int argc, char **argv)
{
    if (argc != 3)
        greska("args failed");

    int n = atoi(argv[1]);
    FILE *f = fopen(argv[2], "r");
        if (f == NULL)
            greska("fopen failed");

    struct stat sb;
    if (stat(argv[2], &sb) == -1)
        greska("stat failed");

    int M = sb.st_size;

    pthread_t *tids = malloc(n * sizeof(pthread_t));
        if (tids == NULL)
            greska("tids malloc failed");

    InputData *p = malloc(n * sizeof(InputData));
        if (p == NULL)
            greska("p malloc failed");

    char *buf = NULL;
    fscanf(f, "%ms", &buf);
    printf("%s\n", buf);

    for (int i = 0; i < n; i++) {
        // printf("%d:\tOVDE!\n", i);
        
        p[i].gen = malloc(strlen(buf) + 1);
            if (p[i].gen == NULL) {
                for (int j = 0; j < i; j++)
                    free(p[j].gen);
                free(p);
                greska("p[i].gen malloc failed");
            }

        strcpy(p[i].gen, buf);
        p[i].start = i * M/n;
        p[i].end = p[i].start + M/n;
    
        check_pthread(pthread_create(&tids[i], NULL, function, &p[i]), "pthread_create failed");
    }

    /*
        i = 0:  start = 0,      end = M/n
        i = 1:  start = M/n,    end = M/n + M/n = 2*M/n
        i = 2:  start = 2*M/n   end = 2*M/n + M/n = 3*M/n
    */

   for (int i = 0; i < n; i++) {
        ReturnValue *result = NULL;

        check_pthread(pthread_join(tids[i], (void **)&result), "pthread_join failed");

        free(result);
   }


    free(p);
    free(tids);
    fclose(f);
    exit(EXIT_SUCCESS);
}
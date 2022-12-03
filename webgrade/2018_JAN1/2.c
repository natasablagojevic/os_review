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
    int start;
    int end;
    int index;
    int num;
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

int sum = 0;
char *w = NULL;
int k, n;
char *file = NULL;

void *thread_function(void *arg)
{
    InputData *p = (InputData *)arg;
    int start = p->start;
    int end = p->end;

    FILE *f = fopen(file, "r");
        if (f == NULL)
            greska("fopen failed");

    if (fseek(f, start, SEEK_SET) == -1)
        greska("fseek failed");

    int w_length = strlen(w);

    for (int i = start; i < end; i++) {
        char c = fgetc(f);

        
    }


    return NULL;
}

// k - broj niti
// w - sekvenca karaktera
//  0     1   2 3
// a.out fajl w k
int main(int argc, char **argv)
{
    if (argc != 4)
        greska("args failed");

    w = malloc(strlen(argv[2]) + 1);
        if (w == NULL)
            greska("w malloc failed");

    strcpy(w, argv[2]);
    k = atoi(argv[3]);

    struct stat sb;
    if (stat(argv[1], &sb) == -1)
        greska("stat failed");

    n = sb.st_size;

    file = malloc(strlen(argv[1]) + 1);
        if (file == NULL)
            greska("file malloc failed");

    strcpy(file, argv[1]);

    InputData *p = malloc(k * sizeof(InputData));
        if (p == NULL)
            greska("p malloc failed");

    pthread_t *tids = malloc(k * sizeof(pthread_t));
        if (tids == NULL)
            greska("tids malloc failed");

    for (int i = 0; i < k; i++) {
        p[i].index = i;
        p[i].num = n/k;
        p[i].start = i * n/k;
        p[i].end = start + p[i].num;

        check_pthread(pthread_create(&tids[i], NULL, thread_function, &p[i]), "pthread_create failed");
    }

    for (int i = 0; i < k; i++)
        check_pthread(pthread_join(tids[i], NULL), "pthread_join failed");


    free(p);
    free(tids);
    free(w);
    exit(EXIT_SUCCESS);
}
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
    int n;
    int *array;
} Podaci;

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

int max = INT_MIN;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void *function(void *arg)
{
    Podaci *p = (Podaci *)arg;

    int local_max = INT_MIN;
    for (int i = 0; i < p->n; i++)
        if (p->array[i] > local_max)
            local_max = p->array[i];

    check_pthread(pthread_mutex_lock(&lock), "mutex_lock failed");
        if (local_max > max)
            max = local_max;
    check_pthread(pthread_mutex_unlock(&lock), "mutex_unlock failed");

    return NULL;
}

int main(int argc, char **argv)
{
    if (argc != 2)
        greska("args failed");

    FILE *f = fopen(argv[1], "r");  
        if (f == NULL)
            greska("fopen failed");

    int n;
    fscanf(f, "%d", &n);

    pthread_t *tids = malloc(n * sizeof(pthread_t));
        if (tids == NULL)
            greska("tids malloc failed");

    Podaci *a = malloc(n * sizeof(Podaci));
        if (a == NULL)
            greska("a malloc failed");

    for (int i = 0; i < n; i++) {
        fscanf(f, "%d", &a[i].n)
        a[i].array = malloc(a[i].n * sizeof(int));
            if (a[i].array == NULL) {
                for (int j = 0; j < i; j++)
                    free(a[j].array);
                free(a);
                greska("a malloc failed");
            }
        
        for (int j = 0; j < a[i].n; j++)
            fscanf(f, "%d", &a[i].array[j]);
    }

    for (int i = 0; i < n; i++)
        check_pthread(pthread_create(&tids[i], NULL, function, &a[i]), "pthread_create failed");

    for (int i = 0; i < n; i++)
        check_pthread(pthread_join(tids, NULL), "pthread_join failed");

    check_pthread(pthread_mutex_destroy(&lock), "mutex_destroy failed");

    printf("%d\n", max);

    for (int i = 0; i < n; i++)
        free(a[i].array);
    free(a);
    fclose(f);
    exit(EXIT_SUCCESS);
}
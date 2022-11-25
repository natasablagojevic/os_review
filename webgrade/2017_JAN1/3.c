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
#include<float.h>

#define UNUSED(X) (void)(X)
#define RD_END 0
#define WR_END 1

typedef struct  
{
    int n;
    double *array;
} Vektor;

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

double min = DBL_MAX;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void *function(void *arg)
{
    Vektor *p = (Vektor *)arg;
    double local_min = DBL_MAX;

    for (int i = 0; i < p->n; i++)
        if (p->array[i] < local_min)
            local_min = p->array[i];

    check_pthread(pthread_mutex_lock(&lock), "mutex_lock failed");
        if (local_min < min)
            min = local_min;
    check_pthread(pthread_mutex_unlock(&lock), "mutex_unlock failed");

    return NULL;
}

// a.out fajl
int main(int argc, char **argv)
{
    if (argc != 2)
        greska("args failed");

    FILE *f = fopen(argv[1], "r");
        if (f == NULL)
            greska("fopen failed");

    int n, m;
    fscanf(f, "%d%d", &m, &n);

    Vektor *a = malloc(m * sizeof(Vektor));
        if (a == NULL)
            greska("a malloc failed");

    for (int i = 0; i < m; i++) {
        a[i].n = n;
        a[i].array = malloc(n * sizeof(double));
            if (a[i].array == NULL) {
                for (int j = 0; j < i; j++)
                    free(a[j].array);

                free(a);
                greska("a[i].array malloc failed");
            }
    }

    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++)
            fscanf(f, "%lf", &a[i].array[j]);
    }

    // printf("\n");
    // for (int i = 0; i < m; i++) {
    //     for (int j = 0; j < n; j++)
    //         printf("%lf ", a[i].array[j]);
    //     printf("\n");
    // }

    pthread_t *tids = malloc(m * sizeof(pthread_t));
        if (tids == NULL)
            greska("tids malloc failed");

    for (int i = 0; i < m; i++) 
        check_pthread(pthread_create(&tids[i], NULL, function, &a[i]), "pthread_create failed");

    for (int i = 0; i < m; i++)
        check_pthread(pthread_join(tids[i], NULL), "pthread_join failed");

    check_pthread(pthread_mutex_destroy(&lock), "mutex_destroy");

    printf("%lf\n", min);


    for (int i = 0; i < m; i++)
        free(a[i].array);
    free(a);
    fclose(f);
    free(tids);
    exit(EXIT_SUCCESS);
}
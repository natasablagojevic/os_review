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
    float *array;
} Vector;

typedef struct 
{
    int idx;
} Indexes;

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
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
float p;
int n, m;
Vector *a;

double norma(Vector *a)
{
    double sum = 0.0;
    for (int i = 0; i < a->n; i++)
        sum += pow(a->array[i], p);

    return sum;
}

void *function(void *arg)
{
    // Indexes *trenutna = (Indexes *)arg;
    // double local_sum = 0;
    // local_sum += norma(a[trenutna->idx]);

    Vector *p = (Vector *)arg;

    check_pthread(pthread_mutex_lock(&lock), "mutex_lock failed");
        sum += norma(p);
    check_pthread(pthread_mutex_unlock(&lock), "mutex_unlock");

    return NULL;
}

int main(int argc, char **argv)
{
    scanf("%f%d%d", &p, &m, &n);

    a = malloc(m * sizeof(Vector));
        if (a == NULL)
            greska("a malloc failed");

    for (int i = 0; i < m; i++) {
        a[i].n = n;
        a[i].array = malloc(n * sizeof(float));
            if (a[i].array == NULL) {
                for (int j = 0; j < i; j++)
                    free(a[i].array);
                free(a);
                greska("a[i].array malloc failed");
            }
    }

    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            scanf("%f", &a[i].array[j]);

    // printf("--------------\n");
    // for (int i = 0; i < m; i++) {
    //     for (int j = 0; j < n; j++)
    //         printf("%lf ", a[i].array[j]);
    //     printf("\n");
    // }

    pthread_t *tids = malloc(m * sizeof(pthread_t));
        if (tids == NULL)
            greska("tids malloc failed");
    
    Indexes *idx = malloc(m * sizeof(Indexes));
        if (idx == NULL)
            greska("idx malloc failed");

    for (int i = 0; i < m; i++){
        idx[i].idx = i;

        check_pthread(pthread_create(&tids[i], NULL, function, &a[i]), "pthread_create failed");
    }
    
    for (int i = 0; i < m; i++)
        check_pthread(pthread_join(tids[i], NULL), "pthread_join failed");

    check_pthread(pthread_mutex_destroy(&lock), "mutex_destroy failed");

    printf("%f\n", pow(sum, 1/p));

    free(tids);
    free(idx);
    for (int i = 0; i < m; i++)
        free(a[i].array);
    free(a);
    exit(EXIT_SUCCESS);
}
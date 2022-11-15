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
    float *array;
} UlazniPodaci;

// typedef struct 
// {
//     float result;
// } IzlazniPodaci;

float max = FLT_MIN;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

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

void *function(void *arg)
{
    UlazniPodaci *p = (UlazniPodaci *)arg;

    float sum = 0.0;
    for (int i = 0; i < p->n; i++)
        sum += p->array[i];

    check_pthread(pthread_mutex_lock(&lock), "mutex_lock failed");
        if (sum > max)
            max = sum;
    check_pthread(pthread_mutex_unlock(&lock), "mutex_unlock failed");

    return NULL;
}

int main()
{
    int n, m;
    scanf("%d%d", &n, &m);

    UlazniPodaci *a = malloc(n * sizeof(UlazniPodaci));
        if (a == NULL)
            greska("a malloc failed");

    for (int i = 0; i < n; i++) {
        a[i].n = m;
        a[i].array = malloc(a[i].n * sizeof(float));
            if (a[i].array == NULL) {
                for (int j = 0; j < i; j++)
                    free(a[j].array);
                free(a);
                greska("a[i].array malloc failed");
            }
    }

    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            scanf("%f", &a[i].array[j]);

    printf("\n");
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++)
            printf("%f ", a[i].array[j]);
        printf("\n");
    }

    pthread_t *tids = malloc(n * sizeof(pthread_t));
        if (tids == NULL)
            greska("tids malloc failed");

    for (int i = 0; i < n; i++)
        check_pthread(pthread_create(&tids[i], NULL, function, &a[i]), "pthread_create failed");

    for (int i = 0; i < n; i++)
        check_pthread(pthread_join(tids[i], NULL), "pthread_join failed");

    check_pthread(pthread_mutex_destroy(&lock), "mutex_destroy");

    printf("%f\n", max);

    free(tids);
    for (int i = 0; i < n; i++)
        free(a[i].array);
    free(a);
    exit(EXIT_SUCCESS);
}
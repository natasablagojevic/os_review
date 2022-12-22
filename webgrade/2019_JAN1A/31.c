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
    int idx;
    int num;
} InputData;

typedef struct 
{
    int n;
    double *array;
} Vector;

typedef struct 
{
    double result;
    int idx;
} Result;

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

int m, n, k;
Vector *a;

double norma(Vector a)
{
    double sum = 0.0;
    for (int i = 0; i < a.n; i++)
        sum += (a.array[i]*a.array[i]);

    return sqrt(sum);
}

void *function(void *arg)
{
    InputData *p = (InputData *)arg;

    double local_max = DBL_MIN;
    int start = p->idx * p->num;
    int end = start + p->num;

    for (int i = start; i < end; i++) {
        double pom = norma(a[i]);

        if (pom > local_max)
            local_max = pom;
    }

    Result *r = malloc(sizeof(Result));
        if (r == NULL)
            greska("r malloc failed (function)");

    r->result = local_max;
    r->idx = p->idx;

    return r;
}

int main()
{
    scanf("%d%d%d", &m, &n, &k);

    a = malloc(m * sizeof(Vector));
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

    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            scanf("%lf", &a[i].array[j]);

    pthread_t *tids = malloc(k * sizeof(pthread_t));
        if (tids == NULL)
            greska("tids malloc failed");

    InputData *indeks = malloc(k * sizeof(InputData));
        if (indeks == NULL)
            greska("idneks malloc failed");

    for (int i = 0; i < k; i++) {
        indeks[i].idx = i;
        indeks[i].num = m/k;

        check_pthread(pthread_create(&tids[i], NULL, function, &indeks[i]), "pthread_create failed");
    }

    double max = DBL_MIN;
    int idx = -1;

    for (int i = 0; i < k; i++) {
        Result *r = NULL;

        check_pthread(pthread_join(tids[i], (void **)&r), "pthread_join failed");

        if (r->result > max) {
            max = r->result;
            idx = r->idx;
        }

        free(r);
    }

    printf("%d %lf\n", idx, max);


    for (int i = 0; i < m; i++)
        free(a[i].array);
    free(a);
    free(indeks);
    free(tids);
    exit(EXIT_SUCCESS);
}
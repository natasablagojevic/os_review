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
#include<errno.h>
#include<float.h>
#include<stdbool.h>

#define UNUSED(X) (void)(X)
#define RD_END 0
#define WR_END 1

typedef struct 
{
    float result;
} IzlazniPodaci;

typedef struct 
{
    int n;
    float *array;
} UlazniPodaci;

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

    float proi = 1.0;
    for (int i = 0; i < p->n; i++)
        proi *= p->array[i];

    IzlazniPodaci *out = malloc(sizeof(IzlazniPodaci));
        if (out == NULL)
            greska("out malloc failed");

    out->result = proi;

    return out;
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
                greska("a[i].array failed");
            }
    }

    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            scanf("%f", &a[i].array[j]);

/*    printf("\n");
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++)
            printf("%f ", a[i][j]);
        printf("\n");
    }
*/

    pthread_t *tids = malloc(n * sizeof(pthread_t));
        if (tids == NULL)
            greska("tids malloc failed");

    for (int i = 0; i < n; i++)
        check_pthread(pthread_create(&tids[i], NULL, function, &a[i]), "pthread_create failed");

    float max = FLT_MIN;
    IzlazniPodaci *p = malloc(sizeof(IzlazniPodaci));
        if (p == NULL)
            greska("p malloc failed");

    for (int i = 0; i < n; i++) {
        check_pthread(pthread_join(tids[i], (void **)&p), "pthread_join failed");

        if (p->result > max)
            max = p->result;
    }

    printf("%f\n", max);


    for (int i = 0; i < n; i++)
        free(a[i].array);
    free(a);
    free(tids);
    free(p);
    exit(EXIT_SUCCESS);
}
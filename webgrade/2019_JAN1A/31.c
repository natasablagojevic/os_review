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
} Vector;

typedef struct 
{
    int idx;
    int num;
} Input;

typedef struct 
{
    float max;
    int index;
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

float norma(Vector a)
{
    float sum = 0.0;
    for (int i = 0; i < a.n; i++)
        sum += pow(a.array[i], 2);

    return sqrt(sum);
}
int m, n, k;
Vector *a;

void *function(void *arg)
{
    Input *trenutna = (Input *)arg;

    int start = trenutna->idx;
    int end = start + trenutna->num;

    float Norma = 0.0;
    float max = FLT_MIN;
    int index = -1;

    for (int i = start; i < end; i++) {
        Norma = norma(a[i]);

        if (Norma > max) {
            max = Norma;
            index = i;
        }
    }

    /* ------------------------------------ */
    Result *result = malloc(sizeof(Result));
        if (result == NULL)
            greska("result malloc failed");

    result->index = trenutna->idx;
    result->max = max;

    return result; 
}

int main()
{
    scanf("%d%d%d", &m, &n, &k);

    a = malloc(m * sizeof(Vector));
        if (a == NULL)
            greska("a malloc failed");

    for (int i = 0; i < m; i++) {
        a[i].n = n;
        a[i].array = malloc(n * sizeof(float));
            if (a[i].array == NULL) {
                for (int j = 0; j < i; j++)
                    free(a[j].array);
                free(a);
                greska("a[i].array malloc failed");
            }
    }

    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            scanf("%f", &a[i].array[j]);


    // printf("------------------\n");
    // for (int i = 0; i < m; i++) {
    //     for (int j = 0; j < n; j++)
    //         printf("%f ", a[i].array[j]);
    //     printf("\n");
    // }

    Input *idx = malloc(k * sizeof(Input));
        if (idx == NULL)
            greska("idx malloc failed");

    pthread_t *tids = malloc(k * sizeof(pthread_t));
        if (tids == NULL)
            greska("tids malloc failed");

    for (int i = 0; i < k; i++) {
        idx[i].idx = i;
        idx[i].num = k;

        check_pthread(pthread_create(&tids[i], NULL, function, &idx[i]), "pthread_create failed");
    }

    int index = -1;
    float max = FLT_MIN;

    for (int i = 0; i < k; i++) {
        Result *result = NULL;

        check_pthread(pthread_join(tids[i], (void **)&result), "pthread_join failed");

        if (result->max > max) {
            max = result->max;
            index = result->index;
        }

        free(result);
    }

    printf("%d %f\n", index, max);


    free(tids);
    free(idx);
    for (int i = 0; i < m; i++)
        free(a[i].array);
    free(a);
    exit(EXIT_SUCCESS);
}
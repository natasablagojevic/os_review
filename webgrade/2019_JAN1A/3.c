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
    int index;
} Indexes;

typedef struct 
{
    float resutl;
    int index;
} Result;

void greska(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

void check_pthead(int errNum, const char *msg)
{
    int _err = errNum;
    if (_err > 0) {
        errno = _err;
        greska(msg);
    }
}

int m, n, k;
Indexes *idx;
Vector *a;

float norma(Vector a)
{
    float sum = 0.0;
    for (int i = 0; i < a.n; i++)
        sum += pow(a.array[i], 2);

    return sqrt(sum);
}

void *function(void *arg)
{
    Indexes *trenutni = (Indexes *)arg;
    Result *result = malloc(sizeof(Result));
        if (result == NULL)
            greska("result malloc failed");

    float locla_max = FLT_MIN;
    // int index_max = -1;

    for (int i = 0; i < m/k; i++) {
        float n = norma(a[i]);

        if (n > locla_max) {
            locla_max = n;
            // index_max = i;
        }
    }

    result->resutl = locla_max;
    result->index = trenutni->index;

    return result;

}

int main()
{
    scanf("%d%d%d", &m, &n, &k);

    a = malloc(m * sizeof(Vector));
        if (a == NULL)
            greska("a malloc failed");

    idx = malloc(m * sizeof(Indexes));
        if (idx == NULL)
            greska("idx malloc failed");

    for (int i = 0; i < m; i++) {
        a[i].n = n;
        idx[i].index = i;
        a[i].array = malloc(n * sizeof(float));
            if (a[i].array == NULL)
                greska("a[i].array malloc failed");
    }

    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            scanf("%f", &a[i].array[j]);

    // printf("------------\n");
    // for (int i = 0; i < m; i++) {
    //     for (int j = 0; j < n; j++)
    //         printf("%f ", a[i].array[j]);
    //     printf("\n");
    // }

    pthread_t *tids = malloc(k * sizeof(pthread_t));
        if (tids == NULL)
            greska("tids malloc failed");

    for (int i = 0; i < k; i++) {
        check_pthead(pthread_create(&tids[i], NULL, function, &idx[i]), "pthread_create");
    }

    // Result *result = malloc(sizeof(Result));
    //     if (result == NULL)
    //         greska("main: result malloc failed");

    float max = FLT_MIN;
    int index = -1;

    for (int i = 0; i < k; i++){
        Result *result = NULL;
        check_pthead(pthread_join(tids[i], (void **)&result), "pthread_join failed");

        if (result->resutl > max) {
            max = result->resutl;
            index = result->index;
        }

        free(result);
    }

    printf("%d %f\n", index, max);



    // free(result);
    for (int i = 0; i < m; i++)
        free(a[i].array);
    free(a);
    free(idx);
    exit(EXIT_SUCCESS);
}
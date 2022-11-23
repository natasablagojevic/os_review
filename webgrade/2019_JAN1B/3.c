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
    // int index;
} Vektor;

typedef struct
{
    int index;
} Index;

typedef struct 
{
    float result;
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

Vektor *a = NULL;
Vektor *p = NULL;
Index *idx = NULL;
float max = FLT_MIN;
int max_idx = -1;

void *function(void *arg)
{
    

    float local_max = FLT_MIN;
    int idx = -1;
    int threadNum =

    for (int i = )


    Result *r = malloc(sizeof(Result));
        if (r == NULL)
            greska("function -> r malloc failed");

    
    return r;
}

int main(int argc, char **argv)
{
    int m, n, k;
    scanf("%d%d%d", &m, &n, &k);

    a = malloc(m * sizeof(Vektor));
        if (a == NULL)
            greska("a malloc failed");

    p = malloc(sizeof(Vektor));
        if (p == NULL)
            greska("p malloc failed");

    idx = malloc((m+1) * sizeof(Index));
        if (i == NULL)
            greska("i malloc failed");

    for (int i = 0; i < m; i++) {
        a[i].n = n;
        // idx[i].index = i;

        a[i].array = malloc(n * sizeof(float));
            if (a[i].array == NULL) {
                for (int j = 0; j < i; j++)
                    free(a[j].array);

                free(a);
                greska("a[i].array malloc failed");
            }
    }

    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++)
            scanf("%f", &a[i].array[j]);
    }

    p[0].n = n;
    for (int i = 0; i < n; i++)
        scanf("%f", &p[0].array[i]);

    // printf("------------\n");
    // for (int i = 0; i < m+1; i++) {
    //     for (int j = 0; j < n; j++)
    //         printf("%f ", a[i].array[j]);
    //     printf("\n");
    // }

    pthread_t *tids = malloc(k * sizeof(pthread_t));
        if (tids == NULL)
            greska("tids malloc failed");

    for (int i = 0; i < k; i++) {
        idx[i].index = i;
        check_pthread(pthread_create(&tids[i], NULL, function, &idx[i]), "pthread_create failed");
    }

    Result *r = malloc(sizeof(Result));
        if (r == NULL)
            greska("r malloc failed");

    for (int i = 0; i < k; i++) {
        check_pthread(pthread_join(tids[i], (void **)&r), "pthread_join failed");
    }
    


    for (int i = 0; i < m+1; i++)
        free(a[i].array);
    free(a);
    free(idx);
    free(tids);
    free(p[0].array);
    free(p);
    free(r);
    exit(EXIT_SUCCESS);
}
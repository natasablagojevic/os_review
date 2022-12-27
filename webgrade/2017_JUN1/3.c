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
    int idx;
    int n;
    int k;
} Indeks;

typedef struct 
{
    int result;
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

int max = INT_MIN;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
int n, m, k;
int **A = NULL;
int **B = NULL;

void *function(void *arg)
{
    Indeks *p = (Indeks *)arg;

    int sum = 0;

    printf("%d %d\n", p->n, p->k);

    // A: i = p->n fiksirano
    // B: j = p->k fiksirano

    for (int k = 0; k < m; k++) {
        sum += A[p->n][k] * B[k][p->k];
        // printf("i:%d\tA[%d][%d] * B[%d][%d]\n", p->idx, p->n, k, k, p->k);
    }
    
    check_pthread(pthread_mutex_lock(&lock), "mutex_lock failed");
        if (sum > max) 
            max = sum;
    check_pthread(pthread_mutex_unlock(&lock), "mutex_unlock failed");

    Result *result = malloc(sizeof(Result));
        if (result == NULL)
            greska("result malloc failed");

    result->result = sum;

    return result;
}

int main()
{
    scanf("%d%d", &n, &m);

    A = malloc(n * sizeof(int *));
        if (A == NULL)
            greska("A malloc failed");

    for (int i = 0; i < n; i++) {
        A[i] = malloc(m * sizeof(int));
            if (A[i] == NULL) {
                for (int j = 0; j < i; j++)
                    free(A[j]);
                free(A);
                greska("A[i] malloc failed");
            }
    }

    for (int i = 0; i < n; i++)
        for (int j = 0; j < m; j++)
            scanf("%d", &A[i][j]);

    // printf("----------------------------\n");
    // printf("MATRICA A:\n");
    // for (int i = 0; i < n; i++) {
    //     for (int j = 0; j < m; j++)
    //         printf("%d ", A[i][j]);
    //     printf("\n");
    // }
    // printf("----------------------------\n");
 
    scanf("%d%d", &m, &k);

    B = malloc(m * sizeof(int *));
        if (B == NULL)
            greska("B malloc failed");

    for (int i = 0; i < m; i++) {
        B[i] = malloc(k * sizeof(int));
            if (B[i] == NULL) {
                for (int j = 0; j < i; j++)
                    free(B[j]);
                free(B);
                greska("B[i] malloc failed");
            }
    }

    for (int i = 0; i < m; i++)
        for (int j = 0; j < k; j++)
            scanf("%d", &B[i][j]);

    // printf("----------------------------\n");
    // printf("MATRICA B:\n");
    // for (int i = 0; i < m; i++) {
    //     for (int j = 0; j < k; j++)
    //         printf("%d ", B[i][j]);
    //     printf("\n");
    // }
    printf("----------------------------\n");
    
    pthread_t **tids = malloc(n * sizeof(pthread_t *));
        if (tids == NULL)
            greska("tids malloc failed");

    for (int i = 0; i < n; i++) {
        tids[i] = malloc(k * sizeof(pthread_t));
            if (tids[i] == NULL) {
                for (int j = 0; j < i; j++)
                    free(tids[j]);
                free(tids);
                greska("tids[i] malloc failed");
            }
    }

    Indeks *idx = malloc(sizeof(Indeks));
        if (idx == NULL)
            greska("idx malloc failed");

    // printf("n: %d, k: %d\n", n, k);

    for (int i = 0; i < n; i++) {
        idx->idx = i;
        idx->n = i;
        for (int j = 0; j < k; j++) {
            idx->k = j;
            // printf("%d %d\n", i, j);
            check_pthread(pthread_create(&tids[i][j], NULL, function, idx), "pthread_create failed");
        }
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < k; j++) {
            Result *r = NULL;

            check_pthread(pthread_join(tids[i][j], (void **)&r), "pthread_join failed");

            printf("%d ", r->result);
        }

        printf("\n");
    }

    check_pthread(pthread_mutex_destroy(&lock), "mutex_destroy failed");

    printf("%d\n", max);
    

    free(idx);
    for (int i = 0; i < n; i++)
        free(tids[i]);
    free(tids);
    for (int i = 0; i < n; i++)
        free(A[i]);
    free(A);
    for (int i = 0; i < m; i++)
        free(B[i]);
    free(B);
    exit(EXIT_SUCCESS);
}
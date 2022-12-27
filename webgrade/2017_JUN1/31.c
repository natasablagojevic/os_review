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
    int i;
    int j;
} Index;

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

int n, m1, m2, k;
int **a = NULL;
int **b = NULL;
int max = INT_MIN;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void *function(void *arg)
{
    Index *p = (Index *)arg;
    int sum = 0;

    for (int k = 0; k < m1; k++)
        sum += a[p->i][k] * b[k][p->j];

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
    scanf("%d%d", &n, &m1);

    a = malloc(n * sizeof(int *));
        if (a == NULL)
            greska("a malloc failed");

    for (int i = 0; i < n; i++) {
        a[i] = malloc(m1 * sizeof(int));
            if (a[i] == NULL) {
                for (int j = 0; j < i; j++)
                    free(a[j]);
                free(a);
                greska("a[i] malloc failed");
            }
    }

    for (int i = 0; i < n; i++)
        for (int j = 0; j < m1; j++)
            scanf("%d", &a[i][j]);

    scanf("%d%d", &m2, &k);

    if (m1 != m2)
        greska("nije moguce pomnoziti matrice");

    b = malloc(m1 * sizeof(int *));
        if (b == NULL)
            greska("b malloc failed");

    for (int i = 0; i < m1; i++) {
        b[i] = malloc(k * sizeof(int));
            if (b[i] == NULL) {
                for (int j = 0; j < i; j++)
                    free(b[j]);
                free(b);
                greska("b[i] malloc failed");
            }
    }

    for (int i = 0; i < m2; i++)
        for (int j = 0; j < k; j++)
            scanf("%d", &b[i][j]);

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
    Index **idx = malloc(n * sizeof(Index *));
        if (idx == NULL)
            greska("idx malloc failed");

    for (int i = 0; i < n; i++) {
        idx[i] = malloc(k * sizeof(Index));
            if (idx[i] == NULL) {
                for (int j = 0; j < i; j++)
                    free(idx[j]);
                free(idx);
                greska("idx[i] malloc failed");
            }
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < k; j++) {
            idx[i][j].i = i;
            idx[i][j].j = j;
        }
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < k; j++) 
            check_pthread(pthread_create(&tids[i][j], NULL, function, &idx[i][j]), "pthread_create failed");
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < k; j++) {
            Result *r = NULL;
            check_pthread(pthread_join(tids[i][j], (void **)&r), "pthread_join failed");

            printf("%d ", r->result);
            free(r);
        }
        printf("\n");
    }

    check_pthread(pthread_mutex_destroy(&lock), "destroy failed");

    printf("%d\n", max);

    
    // for (int i = 0; i < n; i++) {
    //     for (int j = 0; j < k; j++)
    //         printf("i:%d, j:%d\t%d %d\n", i, j, idx[i][j].i, idx[i][j].j);
    // }



    for (int i = 0; i < n; i++)
        free(idx[i]);
    free(idx);
    for (int i = 0; i < n; i++)
        free(tids[i]);
    free(tids);
    for (int i = 0; i < m2; i++)
        free(b[i]);
    free(b);
    for (int i = 0; i < n; i++)
        free(a[i]);
    free(a);
    exit(EXIT_SUCCESS);
}
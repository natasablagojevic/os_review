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
    int index;
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

int **matrica;
int n;
int max = INT_MIN;
int index = -1;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void *function(void *arg)
{
    Indexes *idx = (Indexes *)arg;
    int counter = 0;

    for (int i = 0; i < n; i++)
        if (matrica[i][idx->index])
            counter++;

    check_pthread(pthread_mutex_lock(&lock), "mutex_lock");
        if (counter > max) {
            max = counter;
            index = idx->index;
        }
    check_pthread(pthread_mutex_unlock(&lock), "mutex_unlock");

    return NULL;
}

int main()
{
    scanf("%d", &n);

    matrica = malloc(n * sizeof(int *));
        if (matrica == NULL)
            greska("matrica malloc failed");

    for (int i = 0; i < n; i++) {
        matrica[i] = malloc(n * sizeof(int));
            if (matrica[i] == NULL) {
                for (int j = 0; j < i; j++)
                    free(matrica[j]);
                free(matrica);
                greska("matrica[i] malloc failed");
            }
    }

    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            scanf("%d", &matrica[i][j]);

    Indexes *idx = malloc(n * sizeof(Indexes));
        if (idx == NULL)
            greska("idx malloc failed");

    pthread_t *tids = malloc(n * sizeof(pthread_t));
        if (tids == NULL)
            greska("tids malloc failed");

    for (int i = 0; i < n; i++) {
        idx[i].index = i;
        check_pthread(pthread_create(&tids[i], NULL, function, &idx[i]), "pthread_create failed");
    }

    for (int i = 0; i < n; i++)
        check_pthread(pthread_join(tids[i], NULL), "pthread_join");

    check_pthread(pthread_mutex_destroy(&lock), "mutex_destroy");

    printf("%d\n", index);

    free(tids);
    free(idx);
    for (int i = 0; i < n; i++)
        free(matrica[i]);
    free(matrica);
    exit(EXIT_SUCCESS);
}
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
    float x;
    float y;
} Tacka;

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

Tacka *niz = NULL;
Indexes *idx = NULL;
int n;
float min = FLT_MAX;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void *function(void *arg)
{
    Indexes *trenutna = (Indexes *)arg;
    float local_min = FLT_MAX;

    Tacka pom = niz[trenutna->idx];

    for (int i = 0; i < n; i++) {
        if (trenutna->idx == i)
            continue;
        
        float r = (pom.x - niz[i].x) * (pom.x - niz[i].x) + (pom.y - niz[i].y) * (pom.y - niz[i].y);

        r = sqrt(r);

        if (r < local_min)
            local_min = r;
    }

    check_pthread(pthread_mutex_lock(&lock), "mutex_lock failed");
        if (min > local_min)
            min = local_min;
    check_pthread(pthread_mutex_unlock(&lock), "mutex_unlock");

    return NULL;
}

int main()
{
    int i = 0;
    float x, y;

    int k = 1;
    int alocirano = k;
    
    niz = malloc(k * sizeof(Tacka));
        if (niz == NULL)
            greska("niz malloc failed");

    while (scanf("%f%f", &x, &y) != EOF) {
        if (i == k) {
            alocirano += k;

            niz = realloc(niz, alocirano * sizeof(Tacka));
            if (niz == NULL)
                greska("niz realloc failed");
        }    

        niz[i].x = x;
        niz[i].y = y;

        i++;
    }

    n = i;

    // for (int i = 0; i < n; i++)
    //     printf("%f %f\n", niz[i].x, niz[i].y);

    idx = malloc(n * sizeof(Indexes));
        if (idx == NULL)
            greska("idx malloc faied");

    pthread_t *tids = malloc(n * sizeof(pthread_t));
        if (tids == NULL)
            greska("tids malloc failed");

    for (int i = 0; i < n; i++) {
        idx[i].idx = i;

        check_pthread(pthread_create(&tids[i], NULL, function, &idx[i]), "pthread_create failed");
    }

    for (int i = 0; i < n; i++)
        check_pthread(pthread_join(tids[i], NULL), "pthread_join failed");

    check_pthread(pthread_mutex_destroy(&lock), "mutex_destroy");

    printf("%f\n", min);


    free(niz);
    free(tids);
    free(idx);
    exit(EXIT_SUCCESS);
}
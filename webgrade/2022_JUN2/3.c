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
    int resutl;
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

int broj_cifara(int broj)
{
    int brojac = 0;
    while (broj) {
        broj /= 10;
        brojac++;
    }

    return brojac;
}


// 1234  5
void obrni(int *broj)
{
    int n = broj_cifara(*broj);

    int *brojevi = malloc(n * sizeof(int));
        if (brojevi == NULL)
            greska("brojevi malloc failed");
    
    // brojevi[0] * 10^0 + brojevi[1] * 10^1

    int x = 0;
    int pom = *broj;
    int i = 0;

    while (pom) {
        x = pom % 10;
        pom /= 10;

        brojevi[i] = x;
        i++;
    }

    int tmp = brojevi[0];
    brojevi[0] = brojevi[n-1];
    brojevi[n-1] = tmp;

    int sum = 0;
    for (int i = 0; i < n; i++) {
        sum += (brojevi[i] * pow(10, i));
    }

    *broj = sum;

    free(brojevi);
}

int sum = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void *function(void *arg)
{
    int *broj = (int *)arg;

    obrni(broj);

    check_pthread(pthread_mutex_lock(&lock), "muetx_lock");
        sum += *broj;
    check_pthread(pthread_mutex_unlock(&lock), "mutex_unlcok");

    Result *result = malloc(sizeof(Result));
        if (result == NULL)
            greska("result malloc failed");

    result->resutl = *broj;

    return result;
}

int main()
{
    int n;
    scanf("%d", &n);

    int *a = malloc(n * sizeof(int));
        if (a == NULL)
            greska("a malloc failed");

    for (int i = 0; i < n; i++)
        scanf("%d", &a[i]);

    // for (int i = 0; i < n; i++)
    //     printf("%d\n", a[i]);

    pthread_t *tids = malloc(n * sizeof(pthread_t));
        if (tids == NULL)
            greska("tids malloc failed");
    
    for (int i = 0; i < n; i++)
        check_pthread(pthread_create(&tids[i], NULL, function, &a[i]), "pthread_create failed");

    for (int i = 0; i < n; i++){
        Result *r = NULL;
        check_pthread(pthread_join(tids[i], (void **)&r), "pthread_join");

        printf("%d ", r->resutl);

        free(r);
    }

    check_pthread(pthread_mutex_destroy(&lock), "mutex_destroy");

    printf("\n%d\n", sum);
    
    free(a);
    free(tids);
    exit(EXIT_SUCCESS);
}
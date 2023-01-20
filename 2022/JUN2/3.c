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

int broj_cifara(int broj)
{
    int brojac = 0;
    while (broj) {
        brojac++;
        broj /= 10;
    }

    return brojac;
}

void zameni(int *broj)
{
    int n = broj_cifara(*broj);
    int *brojevi = malloc(n * sizeof(int));
        if (brojevi == NULL)
            greska("brojevi malloc failed");

    int pom = *broj;
    int i = 0;

    while (pom && i < n) {
        brojevi[i] = pom % 10;
        pom /= 10;
        i++;
    }

    pom = brojevi[0];
    brojevi[0] = brojevi[n-1];
    brojevi[n-1] = pom;
    int sum = 0;

    for (i = 0; i < n; i++)
        sum += (brojevi[i] * pow(10, i));

    *broj = sum;
    free(brojevi);
}

int suma = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void *f(void *arg)
{
    int *broj = (int *)arg;

    zameni(broj);

    check_pthread(pthread_mutex_lock(&lock), "mutex_lock failed");
        suma += *broj;
    check_pthread(pthread_mutex_unlock(&lock), "mutex_unlock failed");

    Result *res = malloc(sizeof(Result));
        if (res == NULL)
            greska("res malloc failed");

    res->result = *broj;
    
    return res;
}

int main(int argc, char **argv)
{
    UNUSED(argc), UNUSED(argv);

    int n;
    scanf("%d", &n);

    int *a = malloc(n * sizeof(int));
        if (a == NULL)
            greska("a malloc failed");

    for (int i = 0; i < n; i++)
        scanf("%d", &a[i]);

    pthread_t *tids = malloc(n * sizeof(pthread_t));
        if (tids == NULL)
            greska("tids malloc failed");

    for (int i = 0; i < n; i++)
        check_pthread(pthread_create(&tids[i], NULL, f, &a[i]), "pthread_create failed");

    for (int i = 0; i < n; i++) {
        Result *result = NULL;

        check_pthread(pthread_join(tids[i], (void **)&result), "pthread_join failed");

        printf("%d ", result->result);

        free(result);
    }

    check_pthread(pthread_mutex_destroy(&lock), "mutex_destroy failed");

    printf("\n%d\n", suma);


    free(tids);
    free(a);
    exit(EXIT_SUCCESS);
}
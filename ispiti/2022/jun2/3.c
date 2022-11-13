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
#include<stdbool.h>

#define UNUSED(X) (void)(X)
#define RD_END 0
#define WR_END 1

typedef struct 
{
    int result;
} Podaci;

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

void zameni(int *B)
{
    int n = broj_cifara(*B);
    int *cifre = malloc(n * sizeof(int));
        if (cifre == NULL)
            greska("cifre malloc failed");

    int x;
    int pom = *B;
    for (int i = 0; pom != 0; i++) {
        x = pom % 10;
        pom /= 10;
        cifre[i] = x;
    }

    x = cifre[0];
    cifre[0] = cifre[n-1];
    cifre[n-1] = x;

    x = 0;
    for (int i = 0; i < n; i++)
        x += (cifre[i] * pow(10, i));

    *B = x;

    free(cifre);
}

int sum = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void *f(void *arg)
{
    int *broj = (int *)arg;

    zameni(broj);

    check_pthread(pthread_mutex_lock(&lock), "mutex_lock failed");
        sum += *broj;
    check_pthread(pthread_mutex_unlock(&lock), "mutex_unlock failed");

    Podaci *p = malloc(sizeof(Podaci));
        if (p == NULL)
            greska("f -> p malloc failed");

    p->result = *broj;

    return p;
}

// a.out 
int main()
{
    int n;
    scanf("%d", &n);

    int *brojevi = malloc(n * sizeof(int));
        if (brojevi == NULL)
            greska("brojevi malloc failed");

    for (int i = 0; i < n; i++)
        scanf("%d", &brojevi[i]);

    pthread_t *tids = malloc(n * sizeof(pthread_t));
        if (tids == NULL)
            greska("tids malloc failed");

    for (int i = 0; i < n; i++)
        check_pthread(pthread_create(&tids[i], NULL, f, &brojevi[i]), "pthread_create failed");

    Podaci *p = malloc(sizeof(Podaci));
        if (p == NULL)
            greska("main -> p malloc failed");

    for (int i = 0; i < n; i++) {
        check_pthread(pthread_join(tids[i], (void **)&p), "pthread_join failed");

        printf("%d ", p->result);
    }

    check_pthread(pthread_mutex_destroy(&lock), "mutex_destroy failed");

    printf("\n%d\n", sum);

    free(brojevi);
    free(tids);
    free(p);
    exit(EXIT_SUCCESS);
}
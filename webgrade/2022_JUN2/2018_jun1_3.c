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

int max = INT_MIN;
char slovo;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
char *file = NULL;

void sve_u_mala(char *s)
{
    for (int i = 0; s[i] != 0; i++)
        s[i] = tolower(s[i]);
}

int prebroj(char *s, int id)
{
    sve_u_mala(s);

    int brojac = 0;
    for (int i = 0; s[i] != 0; i++) 
        if ((s[i] - 'a') == id)
            brojac++;

    return brojac;
}

void *function(void *arg)
{
    Index *idx = (Index *)arg;

    FILE *f = fopen(file, "r");
        if (f == NULL)
            greska("fopen failed");

    char *buf = NULL;
    int broj = 0;
    while (fscanf(f, "%ms", &buf) == 1)
        broj += prebroj(buf, idx->idx);

    check_pthread(pthread_mutex_lock(&lock), "mutex_lock");
        if (broj > max) {
            max = broj;
            slovo = idx->idx + 'a';
        }
    check_pthread(pthread_mutex_unlock(&lock), "mutex_unlock");

    Result *r = malloc(sizeof(Result));
        if (r == NULL)
            greska("r malloc failed");

    r->result = broj;

    fclose(f);

    return r;
}

// a.out fajl
int main(int argc, char **argv)
{
    if (argc != 2)
        greska("args failed");

    file = malloc(strlen(argv[1]) + 1);
        if (file == NULL)
            greska("file malloc failed");

    strcpy(file, argv[1]);

    Index *idx = malloc(26 * sizeof(Index));
        if (idx == NULL)
            greska("idx malloc failed");

    pthread_t *tids = malloc(26 * sizeof(pthread_t));
        if (tids == NULL)
            greska("tids malloc failed");

    for (int i = 0; i < 26; i++) {
        idx[i].idx = i;
        
        check_pthread(pthread_create(&tids[i], NULL, function, &idx[i]), "pthread_create failed");
    }

    for (int i = 0; i < 26; i++) {
        Result *r = NULL;

        check_pthread(pthread_join(tids[i], (void **)&r), "pthread_join");

        printf("%d ", r->result);

        free(r);
    }

    check_pthread(pthread_mutex_destroy(&lock), "mutex_destroy");

    printf("\n%c\n", slovo);

    free(file);
    free(idx);
    exit(EXIT_SUCCESS);
}
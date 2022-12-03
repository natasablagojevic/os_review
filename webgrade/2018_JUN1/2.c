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
    int indeks;
} Indeksi;

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

char *filename = NULL;
char max_slovo;
int max_pojavljivanje = -1;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void sve_u_mala(char *rec)
{
    for (int i = 0; rec[i] != 0; i++)
        rec[i] = tolower(rec[i]);
}

int prebroj_slovo(char *rec, int id)
{   
    sve_u_mala(rec);
    int n = strlen(rec);
    int brojac = 0;

    for (int i = 0; i < n; i++)
        if ((rec[i] - 'a') == id)
            brojac++;

    return brojac;
}

void *function(void *arg)
{
    Indeksi *idx = (Indeksi *)arg;

    FILE *f = fopen(filename, "r");
        if (f == NULL)
            greska("fopen failed");

    Result *r = malloc(sizeof(Result));
        if (r == NULL)
            greska("r malloc failed");

    int locale_conter = 0;
    char *buf = NULL;

    while (fscanf(f, "%ms", &buf) == 1) {
        locale_conter += prebroj_slovo(buf, idx->indeks);
    }

    r->result = locale_conter;

    check_pthread(pthread_mutex_lock(&lock), "mutex_lock failed");
        if (locale_conter > max_pojavljivanje) {
            max_pojavljivanje = locale_conter;
            max_slovo = idx->indeks + 'a';
        }
    check_pthread(pthread_mutex_unlock(&lock), "mutex_unlock failed");
    
    free(buf);

    return r;
}

int main(int argc, char **argv)
{   
    if (argc != 2)
        greska("args failed");

    filename = malloc(strlen(argv[1]) + 1);
        if (filename == NULL)
            greska("filename malloc failed");

    strcpy(filename, argv[1]);

    pthread_t *tids = malloc(26*sizeof(pthread_t));
        if (tids == NULL)
            greska("tids malloc failed");

    Indeksi *idx = malloc(26 * sizeof(Indeksi));
        if (idx == NULL)
            greska("idx malloc failed");

    for (int i = 0; i < 26; i++) {
        idx[i].indeks = i;

        check_pthread(pthread_create(&tids[i], NULL, function, &idx[i]), "pthread_create failed");
    }

    for (int i = 0; i < 26; i++) {
        Result *r = NULL;

        check_pthread(pthread_join(tids[i], (void **)&r), "pthread_join failed");

        printf("%d ", r->result);

        free(r);
    }

    check_pthread(pthread_mutex_destroy(&lock), "mutex_destory failed");

    printf("\n%c\n", max_slovo);

    free(idx);
    free(tids);
    free(filename);
    exit(EXIT_SUCCESS);
}
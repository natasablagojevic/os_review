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
} Vektor;

typedef struct 
{
    float result;
    int index;
} PovratnaVrednost;

float **a = NULL;

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

float norma(int n, float *niz)
{
    float sum = 0.0;
    for (int i = 0; i < n; i++)
        sum += niz[i]*niz[i];

    return sqrt(sum);
}   

void *function(void *arg)
{

}

int main()
{
    int m, n, k;
    scanf("%d%d%d", &m, &n, &k);

    /* Vektor *v = malloc(m * sizeof(Vektor));
        if (v == NULL)
            greska("v malloc failed");

    for (int i = 0; i < m; i++) {
        v[i].n = n;
        v[i].array = malloc(n * sizeof(float));
            if (v[i].array == NULL) {
                for (int j = 0; j < i; j++)
                    free(v[j].array);
                free(v);
                greska("v[i].array malloc failed");
            }
    } */



    /* for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            scanf("%f", &v[i].array[j]);
 */
    

/*
    printf("\n");
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) 
            printf("%f ", v[i].array[j]);

        printf("\n");
    }
*/

    a = malloc(m * sizeof(float *));
        if (a == NULL)
            greska("a malloc failed");

    for (int i = 0; i < m; i++) {
        a[i] = malloc(n * sizeof(float));
            if (a[i] == NULL) {
                for (int j = 0; j < i; j++)
                    free(a[j]);
                free(a);
                greska("a[i] malloc failed");
            }
    }

    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            scanf("%f", &a[i][j]);

    pthread_t *tids = malloc(k * sizeof(pthread_t));
        if (tids == NULL)
            greska("tids malloc failed");

    for (int i = 0; i < k; i++)
        check_pthread(pthread_create(&tids[i], NULL, function, &a[i]), "pthread_create failed");

    PovratnaVrednost *p = malloc(sizeof(PovratnaVrednost));
        if (p == NULL)
            greska("p malloc failed");

    float max = FLT_MIN;
    int index = -1;

    for (int i = 0; i < k; i++) {
        check_pthread(pthread_join(tids[i], (void **)&p), "pthread_join failed");

        if (p->result > max) {
            max = p->result;
            index = p->index;
        }
    }

    for (int i = 0; i < m; i++)
        free(v[i].array);
    free(v);
    free(tids);
    free(p);
    exit(EXIT_SUCCESS);
}
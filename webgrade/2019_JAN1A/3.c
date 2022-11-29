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

#define UNUSED(X) (void)(X)
#define RD_END 0
#define WR_END 1

typedef struct 
{
    int n;
    float *array;
} Vector;

void greska(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

void check

int main()
{
    int m, n, k;
    scanf("%d%d%d", &m, &n, &k);

    Vector *a = malloc(m * sizeof(Vector));
        if (a == NULL)
            greska("a malloc failed");

    for (int i = 0; i < m; i++) {
        a[i].n = n;
        a[i].array = malloc(n * sizeof(float));
            if (a[i].array == NULL)
                greska("a[i].array malloc failed");
    }

    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            scanf("%f", &a[i].array[j]);

    printf("------------\n");
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++)
            printf("%f ", a[i].array[j]);
        printf("\n");
    }


    exit(EXIT_SUCCESS);
}
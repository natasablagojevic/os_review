#include<stdio.h>
#include<stdlib.h>
#include<math.h>

void greska()
{
    fprintf(stderr, "-1\n");
    exit(EXIT_FAILURE);
}

double mi(float *a, int n)
{
    double sum = 0.0;
    for (int i = 0; i < n; i++)
        sum += a[i];

    return sum*1.0/n;
}

double devijacija(float *a, int n)
{
    double sum = 0.0;
    for (int i = 0; i < n; i++) {
        sum += (pow(a[i] - mi(a, n), 2));
    }

    sum = sum*1.0/n;

    return sqrt(sum);
}


int main()
{
    int n;
    scanf("%d", &n);

    float *a = malloc(n * sizeof(float));
        if (a == NULL)
            greska();

    for (int i = 0; i < n; i++)
        scanf("%f", &a[i]);

    printf("%lf\n", devijacija(a, n));

    free(a);
    return 0;
}
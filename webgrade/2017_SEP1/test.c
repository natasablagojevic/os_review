#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>

void greska()
{
    fprintf(stderr, "-1\n");
    exit(EXIT_FAILURE);
}

bool check(int broj)
{
    if (broj == 1)
        return true;

    int p = 1;
    while (p < broj) {
        p *= 3;
        printf("p: %d\n", p);
    }

    return p == broj ? true : false;
}

int main()
{
    int x;
    scanf("%d", &x);

    printf("broj: %d\n", x);

    printf("%d\n", check(x));


    return 0;
}
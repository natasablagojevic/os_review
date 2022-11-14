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

int p, k;
char *ekst;
int brojac = 0;

void greska(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

int f(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
    if (typeflag == FTW_F) {
        char *ekstenzija = strrchr(fpath + ftwbuf->base, '.');
            if (ekstenzija == NULL)
                greska("strrchr failed");

        if (strcmp(ekst, ekstenzija) == 0 && ftwbuf->level >= p && ftwbuf->level <= k)
            brojac++;
    }

    return 0;
}

// a.out dir ekst p k
int main(int argc, char **argv)
{
    if (argc != 5)
        greska("args failed");

    struct stat sb;
    if (stat(argv[1], &sb) == -1)
        greska("stat failed");

    if (!S_ISDIR(sb.st_mode))
        greska("nije dir");

    p = atoi(argv[3]);
    k = atoi(argv[4]);

    ekst = malloc(strlen(argv[2]) + 1);
        if (ekst == NULL)
            greska("ekst malloc failed");

    strcpy(ekst, argv[2]);

    if (nftw(argv[1], f, 50, 0) == -1)
        greska("nftw failed");

    printf("%d\n", brojac);

    free(ekst);
    exit(EXIT_SUCCESS);
}
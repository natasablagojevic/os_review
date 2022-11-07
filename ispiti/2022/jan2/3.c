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
#include<pwd.h>
#include<grp.h>

#define UNUSED(X) (void)(X)
#define RD_END 0
#define WR_END 1

void greska(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

//  a.out fajl
int main(int argc, char **argv)
{
    if (argc != 2)
        greska("args failed");

    struct stat sb;
    if (lstat(argv[1], &sb) == -1)
        greska("lstat failed");

    char *pom = NULL;
    char *path = realpath(argv[1], pom);
        if (path == NULL)
            greska("realpath failed");
    // printf("%s\n", path);

    if (stat(path, &sb) == -1)
        greska("stat failed");

    struct passwd *pwd = getpwuid(sb.st_uid);
        if (pwd == NULL)
            greska("getpwuid failed");

    struct group *grp = getgrgid(sb.st_gid);
        if (grp == NULL)
            greska("getgrgid failed");

    printf("%s %s\n", pwd->pw_name, grp->gr_name);


    exit(EXIT_SUCCESS);
}
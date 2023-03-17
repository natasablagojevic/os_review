#define _XOPEN_SOURCE 700
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/mman.h>
#include<semaphore.h>
#include<pthread.h>
#include<ftw.h>
#include<fcntl.h>
#include<sys/epoll.h>
#include<unistd.h>
#include<limits.h>
#include<time.h>

#define UNUSED(X) (void)(X)

void greska(const char *msg)
{
  perror(msg);
  exit(EXIT_FAILURE);
}

char *ekstenzija = NULL;
time_t now;
long velicina = 0;

int f(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
  if (typeflag == FTW_F) {
    //printf("\t%s\n", fpath+ftwbuf->base);
    //const char *p = fpath+ftwbuf->base;
    char *dot = strrchr((fpath + ftwbuf->base), '.');
      if (dot == NULL)
	greska("strrchr failed");
      
      time_t diff = abs(now - sb->st_mtime);
      
      //printf("\tdiff: %ld\n", diff); 
      //printf("dot: %s\n", dot);
      //printf("mtime: %ld\n", diff);
    
      if (strcmp(dot, ekstenzija) == 0) {
      // ekstenzije se poklapaju

      if ((diff/(24*60*60)) <= 7)
	velicina += sb->st_size;
    }
  }
  
  return 0;
}

// a.out dir .ekstenzija
int main(int argc, char **argv)
{
  if (argc != 3)
    greska("args failed");
  
  struct stat sb;
  if (stat(argv[1], &sb) == -1)
    greska("stat failed");
  
  if (!S_ISDIR(sb.st_mode))
    greska("nije direktorijum");
  
  ekstenzija = malloc(strlen(argv[2]) + 1);
    if (ekstenzija == NULL)
      greska("ekstenzija malloc failed");
    
  if (time(&now) == -1)
    greska("time failed");
  
  //printf("%ld\n", now);
  
  if (nftw(argv[1], f, 50, 0) == -1)
    greska("nftw failed");
  
  printf("%ld\n", velicina);
  
  free(ekstenzija);
  exit(EXIT_SUCCESS);
}

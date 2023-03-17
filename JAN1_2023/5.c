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
#include<errno.h>

#define UNUSED(X) (void)(X)

void greska(const char *msg)
{
  perror(msg);
  exit(EXIT_FAILURE);
}

// a.out fajl
int main(int argc, char **argv)
{
  if (argc != 2)
    greska("args failed");
  
  int fd = open(argv[1], O_RDWR);
    if (fd == -1)
      greska("open failed");
    
  FILE *f = fdopen(fd, "rw");
    if (f == NULL)
      greska("fd open failed");
    
  char *buf = NULL;
  struct flock lock;
  
  while (fscanf(f, "%ms", &buf) == 1) {
    lock.l_type = F_UNLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = ftell(f);
    lock.l_len = -strlen(buf);
    
    if (fcntl(fd, F_GETLK, lock) == -1) {
      if (errno != EAGAIN && errno != EACCES)
	greska("fcntl");
    }
    
    if (lock.l_type == F_RDLCK) {
      printf("%s r\n", buf);
    } else if (lock.l_type == F_WRLCK) {
      printf("%s w\n", buf);
    } else 
      continue;
    
    /*lock.l_type = F_WRLCK;
    lock.l_wheence = SEEK_SET;
    lock.l_start = ftell(f);
    lock.l_len = -strlen(buf);
    
    if (fcntl(fd, F_SETLK, &lock) == -1) {
      if (errno != EAGAIN && errno != EACCES)
	greska("fcntl 2");
    }
    
    if (lock.l_type == F_WRLCK) {
      printf("%s w", buf);
    }
    
    lock.l_type = F_UNLCK;
    lock.l_wheence = SEEK_SET;
    lock.l_start = ftell(f);
    lock.l_len = -strlen(buf);
    */
    
    free(buf);
  }
  /*
    lock.l_type = F_UNLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;  
    
    if (fcntl(fd, F_SETLK, &lock) == -1) {
      if (errno != EACCES && errno != EAGAIN)
	greska("fcntl failed");
    }
   */ 
  exit(EXIT_SUCCESS);
}

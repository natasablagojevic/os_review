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
#include<limits.h>
#include<unistd.h>

#define UNUSED(X) (void)(X)

typedef struct 
{
  int fd;
  char *ime;
} Podaci;

void greska(const char *msg)
{
  perror(msg);
  exit(EXIT_FAILURE);
}

// a.out f1 f2 f3..
int main(int argc, char **argv)
{
  if (argc <= 1)
    greska("args failed");
  
  int nFds = argc - 1;
  int epoll_fd = epoll_create(nFds);
    if (epoll_fd == -1)
      greska("epoll_create failed");
  struct epoll_event *fds = malloc(nFds * sizeof(struct epoll_event));
    if (fds == NULL)
      greska("fds malloc failed");
   
  for (int i = 0; i < nFds; i++) {
    int fd = open(argv[i+1], O_RDONLY | O_NONBLOCK);
      if (fd == -1)
	greska("open failed");
      
    Podaci *p = malloc(sizeof(Podaci));
      if (p == NULL)
	greska("p malloc failed");
      
    p->fd = fd;
    char *slash = strrchr(argv[i+1], '/');
      if (slash == NULL) {
	// nema '/'
	p->ime = malloc(strlen(argv[i+1]) + 1);
	  if (p->ime == NULL)
	    greska("p->ime malloc failed1");
	strcpy(p->ime, argv[i+1]);
      } else {
	slash++;
	p->ime = malloc(strlen(slash) + 1);
	  if (p->ime == NULL)
	    greska("p->ime malloc failed 2");
	  
	strcpy(p->ime, slash);
      }
      
      fds[i].events = EPOLLIN | EPOLLHUP | EPOLLERR;
      fds[i].data.ptr = p;
      
      if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &fds[i]) == -1)
	greska("epoll_ctl ADD failed");
  }
  
  int activeFds = nFds;
  struct epoll_event activeEvents[10];
  
  int max = INT_MIN;
  char max_ime[1024];
  
  while (activeFds) {
    int numActive = epoll_wait(epoll_fd, activeEvents, nFds, -1);
      if (numActive == -1)
	greska("epoll_wait failed");
      
    for (int i = 0; i < numActive; i++) {
      if (activeEvents[i].events & EPOLLIN) {
	Podaci *p = (Podaci *)activeEvents[i].data.ptr;
	FILE *f = fdopen(p->fd, "r");
	  if (f == NULL)
	    greska("fdopen failed");
	  
	int x;
	
	while (fscanf(f, "%d", &x) == 1) {
	  if (x > max) {
	    max = x;
	    strcpy(max_ime, p->ime);
	  }
	}
      }
      
      if (activeEvents[i].events & (EPOLLHUP | EPOLLERR)) {
	Podaci *p = (Podaci *)activeEvents[i].data.ptr;
	
	if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, p->fd, &activeEvents[i]) == -1)
	  greska("epoll_ctl DEL failed");
	
	close(p->fd);
	free(p->ime);
	activeEvents[i].events = 0;
	
	activeFds--;
      }
    }
  }
    
  printf("%d %s\n", max, max_ime);
      
      
  free(fds);
  close(epoll_fd);
  exit(EXIT_SUCCESS);
}

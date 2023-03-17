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
#include<ctype.h>

#define UNUSED(X) (void)(X)

typedef struct 
{
  int brojac;
  char slovo;
} Result;

void greska(const char *msg)
{
  perror(msg);
  exit(EXIT_FAILURE);
}

typedef struct 
{
  int id;
} Index;

void check_pthread(int errNum, const char *msg)
{
  int _err = errNum;
  
  if (_err > 0) {
    errno = _err;
    greska(msg);
  }
}

//FILE *f = NULL;
char *file = NULL;

void sve_u_mala(char *s)
{
  for (int i = 0; s[i] != 0; i++)
    s[i] = tolower(s[i]);
}

int max = INT_MIN;
char max_slovo;
int SUMA = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

int prebroj_slova(char *s, int id)
{
  sve_u_mala(s);
  
  int brojac = 0;
  for (int i = 0; s[i] != 0; i++)
    if ((s[i]-'a') == id)
      brojac++;
  
  return brojac;
}

void *function(void *arg)
{
  Index *p = (Index *)arg;
  int id = p->id;
  
  //printf("\tid: %d\n", id);
  
  char *buf = NULL;
  int suma = 0;	// pojavljivanje
  
  FILE *f = fopen(file, "r");
    if (f == NULL)
      greska("fopen failed");
  
  while (fscanf(f, "%ms", &buf) == 1) {
    suma += prebroj_slova(buf, id);
    free(buf);
  }
  
  // printf("\t%c %d\n", id+'a', suma);
  
  //free(buf);
  
  check_pthread(pthread_mutex_lock(&lock), "mutex_lock");
    SUMA += suma;
    if (suma > max) {
      max = suma;
      max_slovo = id + 'a';
      //printf("\tmax_slovo: %c, %d\n", max_slovo, max);
    }
  check_pthread(pthread_mutex_unlock(&lock), "mutex_unlock");
  
  Result *r = malloc(sizeof(Result));
    if (r == NULL)
      greska("r malloc failed (function)");
    
  r->brojac = suma;
  r->slovo = id + 'a';
  
  fclose(f);
  
  return r;  
}

// a.out fajl
int main(int argc, char **argv)
{
  if (argc != 2)
    greska("args failed");
  
  file = malloc(strlen(argv[1]) + 1);
    if (file == NULL)
      greska("file malloc failed");
    
  strcpy(file, argv[1]);
  
  int n = 26;  
  pthread_t *tids = malloc(n * sizeof(pthread_t));
    if (tids == NULL)
      greska("tids malloc failed");
    
  Index *id = malloc(n * sizeof(Index));
    if (id == NULL)
      greska("id malloc failed");
    
  for (int i = 0; i < n; i++)
    id[i].id = i;
  
  for (int i = 0; i < n; i++) {
    check_pthread(pthread_create(&tids[i], NULL, function, &id[i]), "pthread_create failed");
  }
 
  int m = INT_MIN;
  char sl = -1;
  
  for (int i = 0; i < n; i++) {
    Result *r = NULL;
    
    check_pthread(pthread_join(tids[i], (void **)&r), "pthread_join failed");
    
    //printf("%c %d\n", r->slovo, r->brojac);
    
    if (r->brojac > m) {
      m = r->brojac;
      sl = r->slovo;
    }
    
    free(r);
  }
  
  check_pthread(pthread_mutex_destroy(&lock), "mutex_destroy failed");
  
  printf("%d %c %d\n", SUMA, sl, m);
  
  free(tids);
  free(file);
  free(id);
  exit(EXIT_SUCCESS);
}

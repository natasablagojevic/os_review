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
#include<stdbool.h>
#include<pwd.h>

#define UNUSED(X) (void)(X)

void greska(const char *msg)
{
  perror(msg);
  exit(EXIT_FAILURE);
}

void get_korisnicko_ime(char *file, char *korisnik)
{
  struct stat sb;
  if (stat(file, &sb) == -1)
    greska("stat failed");
  
  struct passwd *pwd = getpwuid(sb.st_uid);
    if (pwd == NULL)
      greska("getpwuid failed");
    
  strcpy(korisnik, pwd->pw_name);
}

// a.out fajl1 fajl2
int main(int argc, char **argv)
{
  if (argc <= 1)
    greska("args failed");
  
  int n = argc - 1;
  struct stat *sb = malloc(n * sizeof(struct stat));
    if (sb == NULL)
      greska("sb malloc failed");
    
  bool *id = calloc(n, n * sizeof(bool));
    if (id == NULL)
      greska("id malloc failed");
  
  for (int i = 0; i < n; i++) {
    if (stat(argv[i+1], &sb[i]) == -1)
      greska("stat failed");
    
    if (!S_ISREG(sb[i].st_mode))
      id[i] = true;
  }
    
  int br = 0;
  for (int i = 0; i < n; i++)
    if (id[i])
      br++;
    
  if (br == n) {
    printf("neuspeh\n");
    exit(EXIT_SUCCESS);
  }
  
  int max = INT_MIN;
  char max_file[1024];
  char korisnik[1024];
  
  for (int i = 0; i < n; i++) {
    if (!S_ISREG(sb[i].st_mode))
      continue;
    
    if (sb[i].st_size > max) {
      max = sb[i].st_size;
      struct passwd *pwd = getpwuid(sb[i].st_uid);
	if (pwd == NULL)
	  greska("getpwuid failed");
    
      strcpy(korisnik, pwd->pw_name);
      
      char *slash = strrchr(argv[i+1], '/');
	if (slash == NULL) {
	  // nema '/'
	  strcpy(max_file, argv[i+1]);
	} else {
	  slash++;
	  strcpy(max_file, slash);
	}
    }
  }
  
  for (int i = 0; i < n; i++) {
    if (sb[i].st_size == max) {
      struct passwd *pwd = getpwuid(sb[i].st_uid);
	if (pwd == NULL)
	  greska("getpwuid failed");
	
      strcpy(korisnik, pwd->pw_name);
      
        char *slash = strrchr(argv[i+1], '/');
	if (slash == NULL) {
	  // nema '/'
	  strcpy(max_file, argv[i+1]);
	} else {
	  slash++;
	  strcpy(max_file, slash);
	}
      
      break;
    }
  }
  
  printf("%s %d %s\n", korisnik, max, max_file);
  
  
  free(id);
  free(sb);
  exit(EXIT_SUCCESS);
}

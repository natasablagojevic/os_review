#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <semaphore.h>
#include <unistd.h>

#define check(expr, msg) \
	do { \
		if(!(expr)){ \
			perror(msg); \
			exit(EXIT_FAILURE); \
		} \
	}while(0)

#define ARRAY_MAX (256)

typedef struct{
	sem_t inDataReady;
	sem_t dataProcessed;
	char str[ARRAY_MAX];
}osInputData;

void *getMemoryBlock(const char *path, unsigned *size);

int main(int argc, char **argv)
{
	check(argc == 2, "argc");
	
	unsigned size = 0;
	osInputData *data = getMemoryBlock(argv[1], &size);
	
	check(sem_wait(&(data->inDataReady)) != -1, "sem_wait");

	char *copy = data->str;
	for(int i = 0; copy[i] != '\0'; i++)
	{
		if(islower(copy[i]))
			copy[i] = toupper(copy[i]);
		else if(isupper(copy[i]))
			copy[i] = tolower(copy[i]);
	}
	//printf("%s", copy);
	
	check(sem_post(&(data->dataProcessed)) != -1, "sem_post");
	
	check(munmap(data, size) != -1, "munmap");
	
	//check(shm_unlink(argv[1]) != -1, "shm_unlink");
	
	return 0;
}

void *getMemoryBlock(const char *path, unsigned *size)
{
	int memFd = shm_open(path, O_RDWR, 0600);
	check(memFd != -1, "shm_open");
	
	struct stat fInfo;
	check(fstat(memFd, &fInfo) != -1, "fstat");
	*size = fInfo.st_size;
	
	void *addr;
	check((addr = mmap(0, *size, PROT_READ | PROT_WRITE, MAP_SHARED, memFd, 0)) != MAP_FAILED, "mmap");
	
	close(memFd);
	return addr;
}
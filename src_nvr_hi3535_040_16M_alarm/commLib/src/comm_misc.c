
#include "commLib.h"
static pthread_mutex_t g_malloc_free_mutex;

void CommLib_InitLib(void)
{
	pthread_mutexattr_t mutexattr;
	pthread_mutexattr_init(&mutexattr);
	pthread_mutexattr_settype(&mutexattr, PTHREAD_MUTEX_RECURSIVE_NP);
	pthread_mutex_init(&g_malloc_free_mutex, &mutexattr);
	pthread_mutexattr_destroy(&mutexattr);
}

void CommLib_deinitLib(void)
{
	pthread_mutex_destroy(&g_malloc_free_mutex);
}
void *my_calloc(size_t nmemb, size_t size)
{
	void *p;
	pthread_mutex_lock(&g_malloc_free_mutex);
	p = calloc(nmemb, size);	
	pthread_mutex_unlock(&g_malloc_free_mutex);
	if(p==NULL){printf("Fatal error my_calloc().\n");exit(0);}
	return(p);
}

void *my_malloc(size_t size)
{
	void *p;
	pthread_mutex_lock(&g_malloc_free_mutex);
	p = malloc(size);
	pthread_mutex_unlock(&g_malloc_free_mutex);
	if(p==NULL){printf("Fatal error my_calloc().\n");exit(0);}
	return(p);
}

void my_free(void *ptr)
{
	pthread_mutex_lock(&g_malloc_free_mutex);
	free(ptr);
	pthread_mutex_unlock(&g_malloc_free_mutex);
}

void *my_realloc(void *ptr, size_t size)
{
	void *p;
	pthread_mutex_lock(&g_malloc_free_mutex);
	p = realloc(ptr, size);	
	pthread_mutex_unlock(&g_malloc_free_mutex);
	if(p==NULL){printf("Fatal error my_calloc().\n");exit(0);}
	return(p);
}

char *my_strsep(char **stringp, const char *delim)
{
	char *p;

	while (1)
	{
		p = strsep(stringp, delim);
		if (p == NULL)
		{
			return NULL;
		}
		else
		{
			if (p[0] == '\0')
				continue;
			else
				break;
		}
	}
	return p;
}

int CommLib_getRand(int min, int max)
{
	struct timeval tv;

	gettimeofday(&tv, NULL);
	srand(tv.tv_usec);
	return(min+(int) ((double)(max-min+1)*rand()/(RAND_MAX*1.0)));
}

int CommLib_semInit(sem_t *sem, int value)
{
	return(sem_init(sem, 0, value));
}

int CommLib_semDestroy(sem_t *sem)
{
	return(sem_destroy(sem));
}

int CommLib_semGetValue(sem_t *sem, int *value)
{
	return(sem_getvalue(sem, value));
}

int CommLib_semPost(sem_t *sem)
{
	return(sem_post(sem));
}

int CommLib_semBPost(sem_t *sem)
{
	int ret;
	int val;

	ret = sem_getvalue(sem, &val);
	if (val == 0)
		ret = sem_post(sem);

	return ret;
}

int CommLib_semWait(sem_t *sem)
{
	return(sem_wait(sem));
}

int CommLib_semTimedWait(sem_t *sem, struct timespec *ts)
{
	return(sem_timedwait(sem, ts));
}

int CommLib_semTrywait(sem_t *sem)
{
	return(sem_trywait(sem));
}

void CommLib_netConvertToAddr(const unsigned char *ipbuf, char *addr)
{
        if (addr == NULL)return;
            sprintf(addr, "%d.%d.%d.%d", ipbuf[0], ipbuf[1], ipbuf[2], ipbuf[3]);
}

void CommLib_netConvertFromAddr(unsigned char *ipbuf, const char *addr)
{
    char    buf[16];
    int tmp[4];
    if (ipbuf == NULL) return;
    strcpy(buf, addr);
    sscanf(buf, "%d.%d.%d.%d", &tmp[0], &tmp[1], &tmp[2], &tmp[3]);
    printf("tmp %d.%d.%d.%d\n", tmp[0], tmp[1], tmp[2], tmp[3]);
    ipbuf[0] = tmp[0];
    ipbuf[1] = tmp[1];
    ipbuf[2] = tmp[2];
    ipbuf[3] = tmp[3];
    printf("ipbuf %d.%d.%d.%d\n", ipbuf[0], ipbuf[1], ipbuf[2], ipbuf[3]);
}


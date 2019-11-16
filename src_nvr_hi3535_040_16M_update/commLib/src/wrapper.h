/*
 * Copyright   : General public 
 * Description : Linux API and GLIBC API wrapper
 * Created     : zhang ping 
 */
#ifndef __WRAPPER_H__
#define __WRAPPER_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <utime.h>
#include <pthread.h>
#include <semaphore.h>
#include <assert.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <net/if_arp.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <ctype.h>
#include <math.h>
#include <inttypes.h>
#include <sched.h>
#include <sys/timeb.h>
#include <netdb.h>
#include <signal.h>
#include <stdarg.h>

/*debug console*/
#define DPFL stderr,__FILE__,__LINE__ 
void debug_Printf(FILE *fp,const char *filename,int lineno,const char *fmt,...);
void debug_SleepPrintf(FILE *fp,const char *filename,int lineno,const char *fmt,...);
void Pthread_mutex_lock(pthread_mutex_t *mutex);
void Pthread_mutex_unlock(pthread_mutex_t *mutex);
const char *Inet_ntop(int af,const void *src,char *dst,socklen_t cnt);
int Pthread_detach(pthread_t thid);
int Close(int fd);
int Pthread_join(pthread_t thread,void **value_ptr);
int Pthread_create(pthread_t * ,const pthread_attr_t * ,void *(func)(void *), void *);
int Socket(int domain, int type, int protocol);
int Bind(int sockfd, const struct sockaddr *my_addr, socklen_t addrlen);
int Listen(int sockfd, int backlog);
int Pthread_mutex_init(pthread_mutex_t *mutex,const pthread_mutexattr_t *attr);
void *Malloc(size_t size);
void *Calloc(size_t nmemb, size_t size);
time_t time_HumanToUnix(int year,int month,int day,int hour,int minute,int second);
void time_UnixToHuman(time_t utc,int *pyear,int *pmonth,int *pday,int *phour,int *pminute,int *psecond);

ssize_t writen(int fd, const void *vptr, size_t n);
ssize_t readn(int fd, void *vptr, size_t n);
ssize_t readline(int fd, void *vptr, size_t maxlen);

ssize_t Writen(int fd, const void *vptr, size_t n);
ssize_t Readn(int fd, void *vptr, size_t n);
ssize_t Readline(int fd, void *vptr, size_t maxlen);

int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);

int tty_reset(int fd);
int tty_cbreak(int fd);
int tty_raw(int fd);

int Atexit(void(*function)(void));

typedef	void Sigfunc(int);/* for signal handlers */
Sigfunc *Signal(int signo, Sigfunc *func);

int Inet_pton(int af, const char *src, void *dst);
int Connect(int sockfd, const struct sockaddr *serv_addr, socklen_t addrlen);
int Setsockopt(int s,int level, int optname, const void *optval, socklen_t optlen);

#endif 


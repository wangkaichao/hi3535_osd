/*
 * Copyright   : General public 
 * Description : Linux API and GLIBC API wrapper
 * Created     : zhang ping 
 */
#include "wrapper.h"
#include <stdarg.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
static struct termios save_termios;
static int ttysavefd = -1;
static enum {RESET, RAW, CBREAK} ttystate = RESET;
static int g_hdLog_enable = 0;
static int g_hdLog_flag = 0;
static int g_debug_printfd = -1;
static FILE * g_mem_file = NULL;
int CommLib_openPrintFile()
{
	g_mem_file = fopen("/tmp/log.txt", "w+");
	return 0;
}
int CommLib_closePrintFile()
{
	fclose(g_mem_file);
	g_mem_file = NULL;
	return 0;
}

void CommLib_setHdLogFile(void)
{
	g_hdLog_enable = 1;
}
void CommLib_openHdLogFile(void)
{
	g_hdLog_flag = g_hdLog_enable&&0x1;
}

void CommLib_closeHdLogFile(void)
{
	if((g_hdLog_enable==1)&&(g_debug_printfd>0))
	{
		printf("fdisk or foramt hd, close log file .");
		close(g_debug_printfd);
		g_debug_printfd = -1;
		g_hdLog_flag=0;
	}
}

void debug_SleepPrintf(FILE *fp,const char *filename,int lineno,const char *fmt,...)
{
	char buffer[2048];
	char buffer1[2048];
	va_list ap;
	int i;

	va_start(ap,fmt);
	vsnprintf(buffer,sizeof(buffer)-2,fmt,ap);
	va_end(ap);
	if(fp!=NULL)
	{
		fprintf(fp,"%s %d:%s\n",filename,lineno,buffer);
		fflush(fp);
	}
	if(g_mem_file!=NULL)
	{
		fprintf(g_mem_file,"%s %d:%s\n",filename,lineno,buffer);
		fflush(g_mem_file);
	}
	if(g_debug_printfd>=0)
	{
		int n,nleft;
		char *ptr;
		snprintf(buffer1,sizeof(buffer1),"%s %d:%s\r\n",filename,lineno,buffer);
		nleft=strlen(buffer1);ptr=buffer1;
		while(nleft>0)
		{
			n=write(g_debug_printfd,ptr,nleft);
			if(n>0)
			{
				nleft-=n;
				ptr+=n;
			}
			else break;
		}
	}
	for(i=0; i<1000;i++)usleep(1000);
}

void Pthread_mutex_lock(pthread_mutex_t *mutex)
{
	int ret;
	ret = pthread_mutex_lock(mutex);
	if (ret != 0)
	{
		printf("Fatal error: Pthread_mutex_lock %s", strerror(ret));
		assert(0);
	}
}

void Pthread_mutex_unlock(pthread_mutex_t *mutex)
{
	int ret;
	ret = pthread_mutex_unlock(mutex);
	if (ret != 0)
	{
		printf("Fatal error: Pthread_mutex_unlock %s", strerror(ret));
		assert(0);
	}
}

const char *Inet_ntop(int af,const void *src,char *dst,socklen_t cnt)
{
	const char * ret;
	ret=inet_ntop(af,src,dst,cnt);
	if(ret==NULL)
	{
		printf("Fatal error: Inet_ntop %s",strerror(errno));
		assert(0);
	}
	return ret;
}
	
int Pthread_detach(pthread_t thid)
{
	int ret=pthread_detach(thid);
	if(ret!=0)
	{
		printf("Fatal error: Pthread_detach %s",strerror(ret));
		assert(0);
	}
	return 0;
}

int Close(int fd)
{
	int ret=close(fd);
	if(ret<0)
	{
		printf("Fatal error:Close fd[%d] %s",fd,strerror(errno));
		assert(0);
	}
	return 0;
}

int Pthread_join(pthread_t thread,void **value_ptr)
{
	int ret=pthread_join(thread,value_ptr);
	if(ret!=0)
	{
		printf("Fatal error: Pthread_join %s ",strerror(ret));
		assert(0);
	}
	return 0;
}

int Pthread_create(pthread_t *thread,const pthread_attr_t *attr,void *(*start_routine)(void *),void *arg)
{
	int ret;
	ret=pthread_create(thread,attr,start_routine,arg);
	if(ret!=0)
	{
		printf("Fatal error: Pthread_create %s ",strerror(ret));
		assert(0);
	}
	return 0;
}

int Socket(int domain, int type, int protocol)
{
	int ret=socket(domain,type,protocol);
	if(ret<0)
	{
		printf("Fatal error: socket %s ",strerror(errno));
		assert(0);
	}
	return ret;
}

int Bind(int sockfd, const struct sockaddr *my_addr, socklen_t addrlen)
{
	int ret;
	ret=bind(sockfd,my_addr,addrlen);
	if(ret<0)
	{
		printf("Fatal error: bind %s ",strerror(errno));
		assert(0);
	}
	return 0;
}

int Listen(int sockfd, int backlog)
{
	int ret;
	ret=listen(sockfd,backlog);
	if(ret<0)
	{
		printf("Fatal error: listen %s ",strerror(errno));
		assert(0);
	}
	return ret;
}

int Pthread_mutex_init(pthread_mutex_t * mutex,const pthread_mutexattr_t * attr)
{
	int ret=pthread_mutex_init(mutex,attr);
	if(ret!=0)
	{
		printf("Fatal error: pthread_mutex_init %s ",strerror(ret));
		assert(0);
	}
	return ret;
}

void *Malloc(size_t size)
{
	void *ret;
	ret =malloc(size);
	assert(ret != NULL);
	return ret;
}

void *Calloc(size_t nmemb, size_t size)
{
	void *ret;
	ret= calloc(nmemb,size);
	assert(ret != NULL);
	return ret;
}

/*Time utitlities */
time_t time_HumanToUnix(int year,int month,int day,int hour,int minute,int second)
{
	time_t ret;
	struct tm t;
	t.tm_year=year-1900;
	t.tm_mon=month-1;
	t.tm_mday=day;
	t.tm_hour=hour;
	t.tm_min=minute;
	t.tm_sec=second;
	ret=mktime(&t);
	if(ret<0) 
	{
		printf("Invalid human time,do not set time before 1970-1-1 8:00:00.");
		return 0;
	}
	return ret;
}

void time_UnixToHuman(time_t utc,int *pyear,int *pmonth,int *pday,int *phour,int *pminute,int *psecond)
{
	struct timeval tv;
	struct tm tm,*ret;
	if(utc<0)
	{
		printf("Invalid UTC time,do not set time before 1970-1-1 00:00:00.");
		utc=0;
	}
	tv.tv_sec=utc;
	tv.tv_usec=0;
	ret=localtime_r(&tv.tv_sec,&tm);
	assert(ret!=NULL);
	if(pyear!=NULL)*pyear = tm.tm_year+1900;
	if(pmonth!=NULL)*pmonth = tm.tm_mon+1;
	if(pday!=NULL) *pday=tm.tm_mday;
	if(phour!=NULL) *phour=tm.tm_hour;
	if(pminute!=NULL) *pminute=tm.tm_min;
	if(psecond!=NULL) *psecond=tm.tm_sec;
}

/* Write "n" bytes to a descriptor. */
ssize_t writen(int fd, const void *vptr, size_t n)
{
	if (fd < 0)
	{
		return -1;
	}
	
	size_t nleft;
	ssize_t nwritten;
	const char *ptr;

	ptr = vptr;
	nleft = n;
	int count = 0;
	while (nleft > 0) 
	{
		if (fd < 0)
		{
			return -1;
		}
		if ( (nwritten = write(fd, ptr, nleft)) <= 0) 
		{
			if (errno == EINTR)
			{
				nwritten = 0; /* and call write() again */
			}				 
			else if (errno == EAGAIN)   
			{  
				count++;
				usleep(10*1000);
				if (count >= 50)
				{
					 printf("========================errno==EAGAIN================================\n");
					 count = 0;
					 return(-1);
				}
				nwritten = 0;
				continue;  
			} 
			else
				return(-1); /* error */
		}
		if (errno == 5)
			return(-1);
		nleft -= nwritten;
		ptr += nwritten;
	}
	return(n);
}

ssize_t Writen(int fd, const void *vptr, size_t n)
{
	ssize_t nret = writen(fd,vptr,n);
	assert(nret == n);
	return nret;
}

/* end writen */

/* Read "n" bytes from a descriptor. */
ssize_t readn(int fd, void *vptr, size_t n)
{
	size_t nleft;
	ssize_t nread;
	char *ptr;

	ptr = vptr;
	nleft = n;
	while (nleft > 0) 
	{
		if ( (nread = read(fd, ptr, nleft)) < 0) 
		{
			if (errno == EINTR)
				nread = 0; /* and call read() again */
			else
				return(-1);
		} 
		else if (nread == 0)
		{
			break; /* EOF */
		}
		nleft -= nread;
		ptr += nread;
	}
	
	return(n - nleft); /* return >= 0 */
}

ssize_t Readn(int fd, void *vptr, size_t n)
{
	ssize_t nret;
	nret = readn(fd, vptr, n);
	if(nret <0)
	{
		printf("Readn Fatal error. %s",strerror(errno));
		assert(0);
	}
	return nret;
}

/* end readn */

ssize_t readline(int fd, void *vptr, size_t maxlen)
{
	ssize_t n, rc;
	char c, *ptr;

	ptr = vptr;
	for (n = 1; n < maxlen; n++)
	{
	again:
		if ((rc = read(fd, &c, 1)) == 1)
		{
			*ptr++ = c;
			if (c == '\n')
				break; /* newline is stored, like fgets() */
		}
		else if (rc == 0)
		{
			*ptr = 0;
			return(n - 1); /* EOF, n-1 bytes were read */
		}
		else
		{
			if (errno == EINTR)
				goto again;
			return(-1);
		}
	}
	*ptr = 0; /* null terminate like fgets() */
	return(n);
}

ssize_t Readline(int fd, void *vptr, size_t maxlen)
{
	ssize_t nret;
	nret = readline(fd,vptr,maxlen);
	assert(nret >= 0);
	return nret;
}

int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen)
{
	int ret;
	while(1)
	{
		ret = accept(sockfd, addr, addrlen); 
		if(ret <0 )
		{
			if(errno == EINTR || errno == ECONNABORTED) continue;
			else if(errno == EAGAIN || errno == EWOULDBLOCK) return ret;
		}
		break;
	}
	return ret;
}

/*----------- tty RAW CBREAK RESET mode setting ------------------*/

int tty_cbreak(int fd)
{
	int err;
	struct termios buf;
	if(ttystate != RESET)
	{
		errno = EINVAL;
		return -1;
	}
	if(tcgetattr(fd, &buf) < 0)
		return -1;
	save_termios = buf;

	buf.c_lflag &= ~(ECHO | ICANON);
	buf.c_cc[VMIN]= 1;
	buf.c_cc[VTIME]= 0;
	if(tcsetattr(fd,TCSAFLUSH, &buf)<0)
	{
		err = errno;
		tcsetattr(fd, TCSAFLUSH, &save_termios);
		errno = err;
		return -1;
	}
	if( (buf.c_lflag & (ECHO | ICANON)) || buf.c_cc[VMIN]!= 1 || buf.c_cc[VTIME]!= 0 )
	{
		tcsetattr(fd, TCSAFLUSH, &save_termios);
		errno = EINVAL;
		return -1;
	}
	ttystate = CBREAK;
	ttysavefd = fd;
	return 0;
}

int tty_raw(int fd)
{
	int err;
	struct termios buf;
	if(ttystate != RESET)
	{
		errno = EINVAL;
		return -1;
	}
	if(tcgetattr(fd, &buf)<0)
	{
		return -1;
	}
	save_termios = buf;
	buf.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
	buf.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
	buf.c_cflag &= ~(CSIZE | PARENB);
	buf.c_cflag |= CS8;
	buf.c_oflag &= ~(OPOST);
	buf.c_cc[VMIN] = 1;
	buf.c_cc[VTIME]= 0;
	if(tcsetattr(fd,TCSAFLUSH, &buf)<0)
	{
		return -1;
	}
	if(tcgetattr(fd,&buf)<0)
	{
		err= errno;
		tcsetattr(fd, TCSAFLUSH, &save_termios);
		errno = err;
		return -1;
	}
	if( (buf.c_lflag &(ECHO | ICANON | IEXTEN | ISIG)) ||
		(buf.c_iflag & (BRKINT| ICRNL | INPCK | ISTRIP |IXON)) || 
		(buf.c_cflag &(CSIZE | PARENB | CS8))!= CS8 || 
		(buf.c_oflag & (OPOST)) || 
		buf.c_cc[VMIN]!= 1 || 
		buf.c_cc[VTIME]!= 0 )
	{
		tcsetattr(fd, TCSAFLUSH, &save_termios);
		errno = EINVAL;
		return -1;
	}
	ttystate = RAW;
	ttysavefd = fd;
	return 0;
}

int tty_reset(int fd)
{
	if(ttystate == RESET) 
		return 0;
	if(tcsetattr(fd, TCSAFLUSH, &save_termios)<0)
		return -1;
	ttystate = RESET;
	return 0;
}

void tty_atexit(void)
{
	if(ttysavefd >= 0 )
	{
		tty_reset(ttysavefd);
	}
}

Sigfunc *Signal(int signo, Sigfunc *func)
{
	struct sigaction act, oact;

	act.sa_handler = func;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	if (signo == SIGALRM) {
#ifdef	SA_INTERRUPT
		act.sa_flags |= SA_INTERRUPT;	/* SunOS 4.x */
#endif
	} else {
#ifdef	SA_RESTART
		act.sa_flags |= SA_RESTART;	/* SVR4, 44BSD */
#endif
	}
	if (sigaction(signo, &act, &oact) < 0) //return(SIG_ERR);
	{
		printf("Register signal handler error.");
		exit(0);
	}
	return(oact.sa_handler);
}

int Atexit(void (*function)(void))
{
	int ret;
	ret = atexit(function);
	assert(ret == 0);
	return ret;
}


int Inet_pton(int af, const char *src, void *dst)
{
	int ret;
	ret = inet_pton(af, src, dst);
	if(ret <0)
	{
		printf("Error Inet_ntop %s", strerror(errno));
		exit(0);
	}
	else if(ret == 0)
	{
		printf("Error Inet_ntop : invalid ip address string.");
		exit(0);
	}
	return ret;
}

int Connect(int sockfd, const struct sockaddr *serv_addr, socklen_t addrlen)
{
	int ret;
	while(1)
	{
		ret = connect(sockfd, serv_addr, addrlen);
		if(ret < 0)
		{
			if(errno == EINTR) continue;
			else 
			{
				printf("Error connect to server:%s",strerror(errno));
				exit(0);
			}
		}
		break;
	}
	return ret;
}



int Setsockopt(int s,int level, int optname, const void *optval, socklen_t optlen)
{
	int ret; 
	ret = setsockopt(s, level, optname, optval, optlen);
	if(ret <0)
	{
		printf("error setsockopt %s.",strerror(errno));
		exit(0);
	}
	return ret;
}

ssize_t Recv(int fd, void *vptr, size_t n, int flags)
{
	int ret=-1;
	ret = recv(fd, vptr, n, flags); 
	if(ret==0)
		return -1;
	if (ret<0)
	{
		printf( "Recv error. [%d] : %s\n", errno, strerror(errno));
	//	if((errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN))
		if((errno == EINTR))
		{
			return 0;
		}
	}
	return ret;
}

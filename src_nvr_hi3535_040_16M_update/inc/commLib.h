

#ifndef _COMMLIB_H_
#define _COMMLIB_H_

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

//#include "commLib.h"
//#include "dev_type.h"
typedef unsigned char uchar_t;
typedef unsigned short ushort_t;
typedef unsigned int uint_t;

#define logErr(...) do {\
  printf("[%s:%d]", __FUNCTION__, __LINE__);\
  printf(__VA_ARGS__);\
  printf("\n");\
  } while(0)

#define logDbg(...) do {\
  printf("[%s:%d]", __FUNCTION__, __LINE__);\
  printf(__VA_ARGS__);\
  printf("\n");\
  } while(0)

/*Filesystem*/
int CommLib_isFileExist(const char *pathname);
int CommLib_getFileLength(const char *pathname);
int CommLib_getFileName(const char *pathname, char *file, size_t len);
int CommLib_getFilePath(const char *pathname, char *path, size_t len);
time_t CommLib_getFileAtime(const char *pathname);
time_t CommLib_getFileMtime(const char *pathname);
int CommLib_isFileRead(const char *pathname);
int CommLib_isFileWrite(const char *pathname);
int CommLib_isFileExec(const char *pathname);
int CommLib_isLinkFile(const char *pathname);
int CommLib_isDeviceFile(const char *pathname);
int CommLib_isRegularFile(const char *pathname);
int CommLib_copyFile(const char *oldpath, const char *newpath, int flag);
int CommLib_rmFile(const char *pathname);
int CommLib_renameFile(const char *oldpath, const char *newpath);
int CommLib_setFileLength(const char *pathname, int newlen);
int CommLib_setFileMode(const char *pathname, mode_t mode);
int CommLib_readLock(int fd, off_t offset, int whence, off_t len);
int CommLib_readWLock(int fd, off_t offset, int whence, off_t len);
int CommLib_writeLock(int fd, off_t offset, int whence, off_t len);
int CommLib_writeWLock(int fd, off_t offset, int whence, off_t len);
int CommLib_unLock(int fd, off_t offset, int whence, off_t len);
off_t CommLib_seek(int fd, off_t offset, int whence);
off_t CommLib_seekToBegin(int fd);
off_t CommLib_seekToEnd(int fd);
int CommLib_setUTime(const char *pathname, const struct timeval *times);
int CommLib_fsync(int fd);
int CommLib_fflush(FILE *stream);

int CommLib_isDirExist(const char *pathname);
int CommLib_isDir(const char *pathname);
int CommLib_getDirName(const char *pathname, char *dirname, size_t len);
int CommLib_getAbsoluteDir(const char *pathname, char *dirname, size_t len);
int CommLib_isDirEmpty(const char *pathname);
long long CommLib_getDirSize(const char *pathname);
int CommLib_getDirFileCount(const char *pathname);
int CommLib_mkDir(const char *pathname, mode_t mode);
int CommLib_setDirMode(const char *pathname, mode_t mode);
int CommLib_copyDir(const char *oldpath, const char *newpath, int flag, long sleeptime);
int CommLib_renameDir(const char *oldpath, const char *newpath);
int CommLib_rmDir(const char *pathname);
int CommLib_getDelFileCount(void);
int CommLib_clearDelFileCount(void);

/*Network*/
int CommLib_getIfindex(int *ifindex);
int CommLib_getMacAddr(char *macaddr, size_t len);
int CommLib_getIpAddr(char *ipaddr, size_t len);
int CommLib_getWifiIpAddr(char *ipaddr, size_t len);
int CommLib_getPPPIpAddr(char *ipaddr, size_t len);
int CommLib_getNetMask(char *netmask, size_t len);
int CommLib_getNetAddr(char *netaddr, size_t len);
int CommLib_getBroadCast(char *broadcast, size_t len);
int CommLib_getGateWay(char *gateway, size_t len);
int CommLib_getGateWay_ex(char *gateway, size_t len);
int CommLib_getDns(char *firstdns, char *seconddns, size_t len);
int CommLib_setMacAddr(const char *macaddr);
int CommLib_setIpAddr(const char *ipaddr);
int CommLib_setNetMask(const char *netmask);
int CommLib_setNetAddr(const char *netaddr);
int CommLib_setBroadCast(const char *broadcast);
int CommLib_delGateWay(const char *gateway);
int CommLib_setGateWay(const char *gateway);
int CommLib_setDns(const char *firstdns, const char *seconddns);
int CommLib_setAutoDns(void);
int CommLib_getLinkStatus(void);

int CommLib_getEth1Ifindex(int *ifindex);
int CommLib_getEth1MacAddr(char *macaddr, size_t len);
int CommLib_getEth1IpAddr(char *ipaddr, size_t len);
int CommLib_getEth1PPPIpAddr(char *ipaddr, size_t len);
int CommLib_getEth1NetMask(char *netmask, size_t len);
int CommLib_getEth1NetAddr(char *netaddr, size_t len);
int CommLib_getEth1BroadCast(char *broadcast, size_t len);
int CommLib_getEth1GateWay(char *gateway, size_t len);
int CommLib_getEth1Dns(char *firstdns, char *seconddns, size_t len);
int CommLib_setEth1MacAddr(const char *macaddr);
int CommLib_setEth1IpAddr(const char *ipaddr);
int CommLib_setEth1NetMask(const char *netmask);
int CommLib_setEth1NetAddr(const char *netaddr);
int CommLib_setEth1BroadCast(const char *broadcast);
int CommLib_delEth1GateWay(const char *gateway);
int CommLib_setEth1GateWay(const char *gateway);
int CommLib_setEth1Dns(const char *firstdns, const char *seconddns);
int CommLib_getEth1LinkState(void);

int CommLib_convertMacToInt(const char *macaddr, int *p);
int CommLib_checkIpAddress(const char *ipaddr);
int CommLib_checkMacAddress(const char *macaddr);

/*WirelessInfo*/
void CommLib_joinAP_wep(char *essid,char safety_option,char passwd_format,char key_numb,char *password);
void CommLib_joinAP_psk();

/*SystemInfo*/
time_t CommLib_makeTime(int year, int month, int day, int hour, int minute, int second);
time_t CommLib_getTime(time_t *t);
int CommLib_getYear(time_t t);
int CommLib_getMonth(time_t t);
int CommLib_getDay(time_t t);
int CommLib_getHour(time_t t);
int CommLib_getMinute(time_t t);
int CommLib_getSecond(time_t t);
int CommLib_getWeekday(time_t t);
int CommLib_getWeekdayOfDay(int year, int mouth, int day);
int CommLib_getDateStr(time_t t, char *date, size_t len);
int CommLib_getTimeStr(time_t t, char *time, size_t len);
int CommLib_getDateTimeStr(time_t t, char *datetime, size_t len);
int CommLib_getWeekDayStr(time_t t, char *weekday, size_t len);
int CommLib_setTime(time_t t);
int CommLib_setTimeStr(const char *time);
int CommLib_getTimeOfDay(struct timeval *tv);
int CommLib_setTimeOfDay(const struct timeval *tv);
int CommLib_getTimeSpan(struct timeval *spantv, const struct timeval *oldtv, const struct timeval *newtv);
long CommLib_getTotalDaysSpan(const struct timeval *spantv);
long CommLib_getTotalHoursSpan(const struct timeval *spantv);
long CommLib_getTotalMinutesSpan(const struct timeval *spantv);
long CommLib_getTotalSecondsSpan(const struct timeval *spantv);
long long CommLib_getTotalMicroSecondsSpan(const struct timeval *spantv);
int CommLib_getNumberOfDays(int year, int month);
int CommLib_getHostName(char *hostname, size_t len);
int CommLib_setHostName(const char *hostname);
int CommLib_System(const char *cmdbuff);
/*Misc*/
void CommLib_InitLib(void);
void CommLib_deinitLib(void);

ssize_t readn(int fd, void *vptr, size_t n);
ssize_t writen(int fd, const void *vptr, size_t n);
ssize_t readline(int fd, void *vptr, size_t maxlen);
void *my_calloc(size_t nmemb, size_t size);
void *my_malloc(size_t size);
void my_free(void *ptr);
void *my_realloc(void *ptr, size_t size);
char *my_strsep(char **stringp, const char *delim);

int CommLib_createSem(int value);
void CommLib_destorySem(int sem_id);
void CommLib_giveBSem(int sem_id);
void CommLib_giveSem(int sem_id);
void CommLib_takeSem(int sem_id);

int CommLib_semInit(sem_t *sem, int value);
int CommLib_semDestroy(sem_t *sem);
int CommLib_semGetValue(sem_t *sem, int *value);
int CommLib_semPost(sem_t *sem);
int CommLib_semBPost(sem_t *sem);
int CommLib_semWait(sem_t *sem);

int CommLib_semTimedWait(sem_t *sem, struct timespec *ts);
int CommLib_semTrywait(sem_t *sem);

int CommLib_getRand(int min, int max);

//int to string addr
void CommLib_netConvertToAddr(const unsigned char *ipbuf, char *addr);
//string to int addr
void CommLib_netConvertFromAddr(unsigned char *ipbuf, const char *addr);
/*Error*/
int CommLib_getErrorNum(void);
int CommLib_getErrorStr(int comm_errno, char *buf, size_t n);
int CommLib_printErrorStr(const char *prefix);
int CommLib_setErrorNum(int comm_errno);

/*zp for compiling warning. */
int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int type);

/*debug console*/
#define DPFL stderr,__FILE__,__LINE__
/* #define DPFL stderr,__FILE__,__LINE__, __FUNCTION__ */
void debug_Printf(FILE *fp,const char *filename,int lineno,const char *fmt,...);
void debug_SleepPrintf(FILE *fp,const char *filename,int lineno,const char *fmt,...);
void Pthread_mutex_lock(pthread_mutex_t *mutex);
void Pthread_mutex_unlock(pthread_mutex_t *mutex);
const char *Inet_ntop(int af,const void *src,char *dst,socklen_t cnt);
int Pthread_detach(pthread_t thid);
int Close(int fd);
int Pthread_join(pthread_t thread,void **value_ptr);
typedef void *(*pthread_create_start_routine)(void *);
int Pthread_create(pthread_t * ,const pthread_attr_t * ,pthread_create_start_routine, void *);
int Socket(int domain, int type, int protocol);
int Bind(int sockfd, const struct sockaddr *my_addr, socklen_t addrlen);
int Listen(int sockfd, int backlog);
int Pthread_mutex_init(pthread_mutex_t *mutex,const pthread_mutexattr_t *attr);
void *Malloc(size_t size);
void *Calloc(size_t nmemb, size_t size);
time_t time_HumanToUnix(int year,int month,int day,int hour,int minute,int second);
void time_UnixToHuman(time_t utc,int *pyear,int *pmonth,int *pday,int *phour,int *pminute,int *psecond);
ssize_t Writen(int fd, const void *vptr, size_t n);
ssize_t Readn(int fd, void *vptr, size_t n);
ssize_t Readline(int fd, void *vptr, size_t maxlen);
ssize_t Recv(int fd, void *vptr, size_t n, int flags);

//gb2312
int unicode_to_gb2312(long int unicode, char *r, char *c);
long int gb2312_to_unicode(char r, char c);
void UTF_8ToGB2312(char *pOut, char *pText, int pLen);
void GB2312ToUTF_8(char *pOut,char *pText, int pLen);

//print info
void CommLib_setHdLogFile(void);
void CommLib_openHdLogFile(void);
void CommLib_closeHdLogFile(void);

#endif /*_COMMLIB_H_*/


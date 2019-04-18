#include "commLib.h"

int CommLib_System(const char *cmdbuff)
{
	FILE *fp_read = 0;
	
	fp_read = popen(cmdbuff, "r");
	
	if (fp_read == NULL)
	{
		printf("%s failure\n", cmdbuff);
	}
	else
	{
		printf("%s success\n", cmdbuff);
		pclose(fp_read);
	}

	return (fp_read != NULL);
}

time_t CommLib_makeTime(int year, int month, int day, int hour, int minute, int second)
{
    time_t t;
    struct tm ptr;

    ptr.tm_sec = second;
    ptr.tm_min = minute;
    ptr.tm_hour = hour;
    ptr.tm_mday = day;
    ptr.tm_mon = month - 1;
    ptr.tm_year = year - 1900;
    ptr.tm_isdst = -1;
    if ((t = mktime(&ptr)) < 0)
    {
        CommLib_setErrorNum(-100);
        return -1;
    }
    
    return t;
}

time_t CommLib_getTime(time_t *t)
{
	time_t tt;

	if (time(&tt) < 0)
	{
		CommLib_setErrorNum(-100);
		return -1;
	}
	if (t == NULL)
		return tt;
	else
		*t = tt;

	return *t;
}

int CommLib_getYear(time_t t)
{
	struct tm ptr;
	time_t tt = t;
	
	if (tt == 0) 
	{
		if (CommLib_getTime(&tt) < 0)
			return(-1);
	}
	if (localtime_r(&tt, &ptr) == NULL)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}

	return(ptr.tm_year+1900);
}

int CommLib_getMonth(time_t t)
{
	struct tm ptr;
	time_t tt = t;
	
	if (tt == 0) 
	{
		if (CommLib_getTime(&tt) < 0)
			return(-1);
	}

	if (localtime_r(&tt, &ptr) == NULL)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}

	return(ptr.tm_mon+1);
}

int CommLib_getDay(time_t t)
{
	struct tm ptr;
	time_t tt = t;
	
	if (tt == 0) 
	{
		if (CommLib_getTime(&tt) < 0)
			return(-1);
	}

	if (localtime_r(&tt, &ptr) == NULL)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}

	return(ptr.tm_mday);
}

int CommLib_getHour(time_t t)
{
	struct tm ptr;
	time_t tt = t;
	
	if (tt == 0) 
	{
		if (CommLib_getTime(&tt) < 0)
			return(-1);
	}

	if (localtime_r(&tt, &ptr) == NULL)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}

	return(ptr.tm_hour);
}

int CommLib_getMinute(time_t t)
{
	struct tm ptr;
	time_t tt = t;
	
	if (tt == 0) 
	{
		if (CommLib_getTime(&tt) < 0)
			return(-1);
	}

	if (localtime_r(&tt, &ptr) == NULL)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}

	return(ptr.tm_min);
}

int CommLib_getSecond(time_t t)
{
	struct tm ptr;
	time_t tt = t;
	
	if (tt == 0) 
	{
		if (CommLib_getTime(&tt) < 0)
			return(-1);
	}

	if (localtime_r(&tt, &ptr) == NULL)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}

	return(ptr.tm_sec);
}

int CommLib_getWeekday(time_t t)
{
	struct tm ptr;
	time_t tt = t;
	
	if (tt == 0) 
	{
		if (CommLib_getTime(&tt) < 0)
			return(-1);
	}

	if (localtime_r(&tt, &ptr) == NULL)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}

	return(ptr.tm_wday);
}

int CommLib_getWeekdayOfDay(int year, int month, int day)
{
	time_t t;
	struct tm ptr;	

	ptr.tm_sec = 1;
	ptr.tm_min = 1;
	ptr.tm_hour = 1;
	ptr.tm_mday = day;
	ptr.tm_mon = month - 1;
	ptr.tm_year = year - 1900;
	if ((t = mktime(&ptr)) < 0)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}
	return(CommLib_getWeekday(t));
}

int CommLib_getDateStr(time_t t, char *date, size_t len)
{
	struct tm ptr;

	if (localtime_r(&t, &ptr) == NULL)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}

	snprintf(date, len, "%d/%d/%d", (ptr.tm_year+1900), ptr.tm_mon+1, ptr.tm_mday);
	
	return(0);
}

int CommLib_getTimeStr(time_t t, char *time, size_t len)
{
	struct tm ptr;

	if (localtime_r(&t, &ptr) == NULL)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}

	snprintf(time, len, "%d:%d:%d", ptr.tm_hour, ptr.tm_min, ptr.tm_sec);
	
	return(0);
}

int CommLib_getDateTimeStr(time_t t, char *datetime, size_t len)
{
	ctime_r(&t, datetime);
	
	return(0);
}

int CommLib_getWeekDayStr(time_t t, char *weekday, size_t len)
{
	struct tm ptr;

	if (localtime_r(&t, &ptr) == NULL)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}
	if (strftime(weekday, len, "%A", &ptr) < 0)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}

	return(0);
}

int CommLib_setTime(time_t t)
{
	struct timeval tv;

	tv.tv_sec = t;
	tv.tv_usec = 0;

	if (settimeofday(&tv, NULL) < 0)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}
	
	return(0);
}

int CommLib_setTimeStr(const char *time)
{
	struct tm mtime;
	struct timeval tv;
	
	mtime.tm_mon = (time[0]-48)*10 + (time[1]-48);
	mtime.tm_mday = (time[2]-48)*10 + (time[3]-48);
	mtime.tm_hour = (time[4]-48)*10 + (time[5]-48);
	mtime.tm_min = (time[6]-48)*10 + (time[7]-48);
	mtime.tm_year = (time[8]-48)*1000 + (time[9]-48)*100 + (time[10]-48)*10 + (time[11]-48) - 1900;
	mtime.tm_sec = 0;

	tv.tv_sec = mktime(&mtime);
	tv.tv_usec = 0;
	if (settimeofday(&tv, NULL) < 0)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}

	return(0);
}

int CommLib_getTimeOfDay(struct timeval *tv)
{
	if (gettimeofday(tv, NULL) < 0)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}

	return(0);
}

int CommLib_setTimeOfDay(const struct timeval *tv)
{
	if (settimeofday(tv, NULL) < 0)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}

	return(0);
}

int CommLib_getTimeSpan(struct timeval *spantv, const struct timeval *oldtv, const struct timeval *newtv)
{
	if ((newtv->tv_sec > oldtv->tv_sec) && (newtv->tv_usec > oldtv->tv_usec))
	{
		spantv->tv_sec = newtv->tv_sec - oldtv->tv_sec;
		spantv->tv_usec = newtv->tv_usec - oldtv->tv_usec;
	}
	else if ((newtv->tv_sec > oldtv->tv_sec) && (newtv->tv_usec < oldtv->tv_usec))
	{
		spantv->tv_sec = newtv->tv_sec - oldtv->tv_sec - 1;
		spantv->tv_usec = 1000000 + newtv->tv_usec - oldtv->tv_usec;
	}
	else if ((newtv->tv_sec < oldtv->tv_sec) && (newtv->tv_usec > oldtv->tv_usec))
	{
		spantv->tv_sec = oldtv->tv_sec - newtv->tv_sec - 1;
		spantv->tv_usec = 1000000 + oldtv->tv_usec - newtv->tv_usec;
	}
	else if ((newtv->tv_sec < oldtv->tv_sec) && (newtv->tv_usec < oldtv->tv_usec))
	{
		spantv->tv_sec = oldtv->tv_sec - newtv->tv_sec;
		spantv->tv_usec = oldtv->tv_usec - newtv->tv_usec;
	}
	else
	{
		spantv->tv_sec = 0;
		spantv->tv_usec = (oldtv->tv_usec > newtv->tv_usec) ? 
					(oldtv->tv_usec - newtv->tv_usec) : (newtv->tv_usec - oldtv->tv_usec);
	}

	return(0);
}

long CommLib_getTotalDaysSpan(const struct timeval *spantv)
{
	return(spantv->tv_sec/(24*60*60)); /*one day has 24*60*60 seconds->	*/
}

long CommLib_getTotalHoursSpan(const struct timeval *spantv)
{
	return(spantv->tv_sec/(60*60));
}

long CommLib_getTotalMinutesSpan(const struct timeval *spantv)
{
	return(spantv->tv_sec/(60));
}

long CommLib_getTotalSecondsSpan(const struct timeval *spantv)
{
	return(spantv->tv_sec);
}

long long CommLib_getTotalMicroSecondsSpan(const struct timeval *spantv)
{
	long long int sec, usec;

	sec = spantv->tv_sec;
	usec = sec * 1000000 + spantv->tv_usec;

	return(usec); /* 1 second = 1000000 microsecond.*/
}

int CommLib_getNumberOfDays(int year, int month)
{
	switch (month)
	{
	case 1:
	case 3:
	case 5:
	case 7:
	case 8:
	case 10:
	case 12:
		return(31);	
	case 4:
	case 6:
	case 9:
	case 11:
		return(30);
	case 2:
		if (year%4 == 0)
			return(29);
		else
			return(28);
	default:
		CommLib_setErrorNum(-104);
		return(-1);
	}	
}


int CommLib_getHostName(char *hostname, size_t len)
{
	if (gethostname(hostname, len) < 0)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}

	return(0);
}

int CommLib_setHostName(const char *hostname)
{
	if (sethostname(hostname, strlen(hostname) + 1) < 0)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}

	return(0);
}

/*---------------------------- END ------------------------------*/
#define COMM_BUFSIZE 8192

static int _CommLib_getDirSize(const char *pathname, long long *dirsize);
static int _CommLib_getDirFileCount(const char *pathname, int *filecount);
static int lock_reg(int fd, int cmd, int type, off_t offset, int whence, off_t len);
static int g_del_file_count = 0;

/*Filesystem*/
int CommLib_isFileExist(const char *pathname)
{	
	int fd;
	struct stat buf;

	if ((fd = open(pathname, O_RDONLY)) < 0)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}
	if (fstat(fd, &buf) < 0)
	{
		CommLib_setErrorNum(-100);
		close(fd);
		return(-1);
	}
	close(fd);

	if (S_ISREG(buf.st_mode))
		return(1);

	return(0); /*not a file, maybe dir/link/fifo etc.*/
}

int CommLib_getFileLength(const char *pathname)
{
	int fd;
	struct stat buf;

	if ((fd = open(pathname, O_RDONLY)) < 0)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}
	if (fstat(fd, &buf) < 0)
	{
		CommLib_setErrorNum(-100);
		close(fd);
		return(-1);
	}
	close(fd);

	if (S_ISREG(buf.st_mode))
		return(buf.st_size);

	CommLib_setErrorNum(-102);
	return(-1);
}

int CommLib_getFileName(const char *pathname, char *file, size_t len)
{
	int fd;
	struct stat buf;

	if ((fd = open(pathname, O_RDONLY)) < 0)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}
	if (fstat(fd, &buf) < 0)
	{
		CommLib_setErrorNum(-100);
		close(fd);
		return(-1);
	}
	close(fd);

	if (S_ISREG(buf.st_mode))
	{
		int i, newpos = -1;

		for (i = 0; *(pathname+i) != '\0'; i++)
		{
			if (*(pathname+i) == '/')
				newpos = i;
		}
		if (newpos == -1)
		{
			if (strlen(pathname) > len-1)
			{
				CommLib_setErrorNum(-101);
				return(-1);
			}
			strncpy(file, pathname, len);
		}
		else
		{
			if (strlen(pathname+newpos+1) > len-1)
			{
				CommLib_setErrorNum(-101);
				return(-1);
			}
			strncpy(file, pathname+newpos+1, len);
		}

		return(0);
	}

	CommLib_setErrorNum(-102);
	return(-1);
}
int CommLib_getFilePath(const char *pathname, char *path, size_t len)
{
	int fd;
	struct stat buf;

	if ((fd = open(pathname, O_RDONLY)) < 0)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}
	if (fstat(fd, &buf) < 0)
	{
		CommLib_setErrorNum(-100);
		close(fd);
		return(-1);
	}
	close(fd);

	if (S_ISREG(buf.st_mode))
	{
		int i;
		int pathlen;
		char tmppath[PATH_MAX];
		const char *ptr = pathname;

		pathlen = strlen(pathname);
		i = pathlen;
		while (pathname[i-1] != '/')
			i--;
		if (pathname[0] == '/')
		{
			strncpy(tmppath, pathname, i-1);
		}
		else if (pathname[0] == '.' && pathname[1] != '.')
		{
			getcwd(tmppath, PATH_MAX);
			strncat(tmppath, ptr+1, i-2);
		}
		else if (pathname[0] == '.' && pathname[1] == '.')
		{
			char oldpath[PATH_MAX];

			getcwd(oldpath, PATH_MAX);
			chdir("..");
			getcwd(tmppath, PATH_MAX);
			chdir(oldpath);
			strncat(tmppath, ptr+2, i-2);
		}
		else
		{
			getcwd(tmppath, PATH_MAX);
			strcat(tmppath, "/");
			strncat(tmppath, pathname, i-1);
		}
		if (strlen(tmppath) > len-1)
		{
			CommLib_setErrorNum(-101);
			return(-1);
		}
		strncpy(path, tmppath, len);
		return(0);
	}

	CommLib_setErrorNum(-102);
	return(-1);
}

time_t CommLib_getFileAtime(const char *pathname)
{
	int fd;
	struct stat buf;

	if ((fd = open(pathname, O_RDONLY)) < 0)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}
	if (fstat(fd, &buf) < 0)
	{
		CommLib_setErrorNum(-100);
		close(fd);
		return(-1);
	}
	close(fd);

	if (S_ISREG(buf.st_mode))
		return(buf.st_atime);

	CommLib_setErrorNum(-102);
	return(-1);
}

time_t CommLib_getFileMtime(const char *pathname)
{
	int fd;
	struct stat buf;

	if ((fd = open(pathname, O_RDONLY)) < 0)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}
	if (fstat(fd, &buf) < 0)
	{
		CommLib_setErrorNum(-100);
		close(fd);
		return(-1);
	}
	close(fd);

	if (S_ISREG(buf.st_mode))
		return(buf.st_mtime);

	CommLib_setErrorNum(-102);
	return(-1);
}

int CommLib_isFileRead(const char *pathname)
{
	int fd;
	struct stat buf;

	if ((fd = open(pathname, O_RDONLY)) < 0)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}
	if (fstat(fd, &buf) < 0)
	{
		CommLib_setErrorNum(-100);
		close(fd);
		return(-1);
	}
	close(fd);

	if (S_ISREG(buf.st_mode))
	{
		if ((S_IRUSR & buf.st_mode) == S_IRUSR)
			return(1);
		else
			return(0);
	}

	CommLib_setErrorNum(-102);
	return(-1);
}

int CommLib_isFileWrite(const char *pathname)
{
	int fd;
	struct stat buf;

	if ((fd = open(pathname, O_RDONLY)) < 0)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}
	if (fstat(fd, &buf) < 0)
	{
		CommLib_setErrorNum(-100);
		close(fd);
		return(-1);
	}
	close(fd);

	if (S_ISREG(buf.st_mode))
	{
		if ((S_IWUSR & buf.st_mode) == S_IWUSR)
			return(1);
		else
			return(0);
	}

	CommLib_setErrorNum(-102);
	return(-1);
}

int CommLib_isFileExec(const char *pathname)
{
	int fd;
	struct stat buf;

	if ((fd = open(pathname, O_RDONLY)) < 0)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}
	if (fstat(fd, &buf) < 0)
	{
		CommLib_setErrorNum(-100);
		close(fd);
		return(-1);
	}
	close(fd);

	if (S_ISREG(buf.st_mode))
	{
		if ((S_IXUSR & buf.st_mode) == S_IXUSR)
			return(1);
		else
			return(0);
	}

	CommLib_setErrorNum(-102);
	return(-1);
}

int CommLib_isLinkFile(const char *pathname)
{
	struct stat buf;

	if (lstat(pathname, &buf) < 0)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}

	if (S_ISLNK(buf.st_mode))
		return(1);
	else
		return(0);
}

int CommLib_isDeviceFile(const char *pathname)
{
	struct stat buf;

	if (lstat(pathname, &buf) < 0)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}

	if (S_ISCHR(buf.st_mode) || S_ISBLK(buf.st_mode))
		return(1);
	else
		return(0);
}

int CommLib_isRegularFile(const char *pathname)
{
	struct stat buf;

	if (lstat(pathname, &buf) < 0)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}
	if (S_ISREG(buf.st_mode))
		return(1);
	else
		return(0);
}

/*flag:0, cover; 1, not cover.*/
int CommLib_copyFile(const char *oldpath, const char *newpath, int flag)
{
	int fdin, fdout;
	int copylen = 0, leftlen = 0;
	char buf[COMM_BUFSIZE];
	struct stat oldbuf, newbuf;	

	if (flag == 1)
	{
		int oldlen, newlen;

		if (CommLib_isFileExist(newpath) == 1) /*file is exist.*/
		{
			oldlen = CommLib_getFileLength(oldpath);
			newlen = CommLib_getFileLength(newpath);
			if (oldlen < 0 || newlen < 0)
				return(-1);
			if (oldlen == newlen) /*length is same*/
				return(0); /*return.*/
		}
	} /*if size not same or file not exist, copy will go on.*/

	if ((fdin = open(oldpath, O_RDONLY)) < 0) 
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}
	if (fstat(fdin, &oldbuf) < 0) 
	{
		CommLib_setErrorNum(-100);
		close(fdin);
		return(-1);
	}
	if ((fdout = open(newpath, O_RDONLY)) > 0) 
	{
		if (fstat(fdout, &newbuf) == 0)
		{
			if (oldbuf.st_ino == newbuf.st_ino) /*oldbuf and newbuf is the same file.*/
			{
				close(fdin);
				close(fdout);
				return(0);
			}
		}
		close(fdout);
	}
	if ((fdout = open(newpath, O_RDWR | O_CREAT | O_TRUNC, oldbuf.st_mode)) < 0) 
	{
		CommLib_setErrorNum(-100);
		close(fdin);
		return(-1);
	}

	leftlen = oldbuf.st_size;
	while (1)
	{
		if (leftlen > COMM_BUFSIZE)
			copylen = COMM_BUFSIZE;
		else
			copylen = leftlen;
		if (readn(fdin, buf, copylen) < 0)
		{
			CommLib_setErrorNum(-100);
			close(fdout);
			close(fdin);
			return(-1);
		}
		if (writen(fdout, buf, copylen) < 0)
		{
			CommLib_setErrorNum(-100);
			close(fdout);
			close(fdin);
			return(-1);
		}
		leftlen = leftlen - copylen;
		if (leftlen <= 0)
			break;
	}

	close(fdin);
	close(fdout);

	return(0);		
}

int CommLib_rmFile(const char *pathname)
{
	if (unlink(pathname) < 0)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}

	return(0);
}

int CommLib_renameFile(const char *oldpath, const char *newpath)
{
	if (rename(oldpath, newpath) < 0)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}

	return(0);
}

int CommLib_setFileLength(const char *pathname, int newlen)
{
	if (truncate(pathname, newlen) < 0)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}

	return(0);
}

int CommLib_setFileMode(const char *pathname, mode_t mode)
{
	if (chmod(pathname, mode) < 0)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}

	return(0);
}

int CommLib_readLock(int fd, off_t offset, int whence, off_t len)
{
	if (lock_reg(fd, F_SETLK, F_RDLCK, offset, whence, len) < 0)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}

	return(0);
}

int CommLib_readWLock(int fd, off_t offset, int whence, off_t len)
{
	if (lock_reg(fd, F_SETLKW, F_RDLCK, offset, whence, len) < 0)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}

	return(0);
}

int CommLib_writeLock(int fd, off_t offset, int whence, off_t len)
{
	if (lock_reg(fd, F_SETLK, F_WRLCK, offset, whence, len) < 0)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}

	return(0);
}

int CommLib_writeWLock(int fd, off_t offset, int whence, off_t len)
{
	if (lock_reg(fd, F_SETLKW, F_WRLCK, offset, whence, len) < 0)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}

	return(0);
}

int CommLib_unLock(int fd, off_t offset, int whence, off_t len)
{
	if (lock_reg(fd, F_SETLKW, F_UNLCK, offset, whence, len) < 0)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}

	return(0);
}

off_t CommLib_seek(int fd, off_t offset, int whence)
{
	if (lseek(fd, offset, whence) < 0)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}

	return(0);
}

off_t CommLib_seekToBegin(int fd)
{
	if (lseek(fd, 0, SEEK_SET) < 0)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}

	return(0);
}

off_t CommLib_seekToEnd(int fd)
{
	if (lseek(fd, 0, SEEK_END) < 0)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}

	return(0);
}

int CommLib_setUTime(const char *pathname, const struct timeval *times)
{
	if (utimes(pathname, times) < 0)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}

	return(0);
}

int CommLib_fsync(int fd)
{
	if (fsync(fd) < 0)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}

	return(0);
}

int CommLib_fflush(FILE *stream)
{
	if (fflush(stream) < 0)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}

	return(0);
}

int CommLib_isDirExist(const char *pathname)
{
	int fd;
	struct stat buf;

	if ((fd = open(pathname, O_RDONLY)) < 0)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}
	if (fstat(fd, &buf) < 0)
	{
		CommLib_setErrorNum(-100);
		close(fd);
		return(-1);
	}
	close(fd);

	if (S_ISDIR(buf.st_mode))
		return(1);

	return(0); /*not a dir, maybe file/fifo etc.*/
}

int CommLib_isDir(const char *pathname)
{
	int fd;
	struct stat buf;

	if ((fd = open(pathname, O_RDONLY)) < 0)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}
	if (fstat(fd, &buf) < 0)
	{
		CommLib_setErrorNum(-100);
		close(fd);
		return(-1);
	}
	close(fd);

	if (S_ISDIR(buf.st_mode))
		return(1);
	else
		return(0);
}


int CommLib_getDirName(const char *pathname, char *dirname, size_t len)
{
	return(0);
}

int CommLib_getAbsoluteDir(const char *pathname, char *dirname, size_t len)
{
	int fd;
	struct stat buf;

	if ((fd = open(pathname, O_RDONLY)) < 0)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}
	if (fstat(fd, &buf) < 0)
	{
		CommLib_setErrorNum(-100);
		close(fd);
		return(-1);
	}
	close(fd);

	if (S_ISDIR(buf.st_mode))
	{
		char tmppath[PATH_MAX];
		const char *ptr = pathname;

		if (pathname[0] == '/')
		{
			strncpy(tmppath, pathname, PATH_MAX);
		}
		else if (pathname[0] == '.' && pathname[1] != '.')
		{
			getcwd(tmppath, PATH_MAX);
			strncat(tmppath, ptr+1, PATH_MAX);
		}
		else if (pathname[0] == '.' && pathname[1] == '.')
		{
			char oldpath[PATH_MAX];

			getcwd(oldpath, PATH_MAX);
			chdir("..");
			getcwd(tmppath, PATH_MAX);
			chdir(oldpath);
			strncat(tmppath, ptr+2, PATH_MAX);
		}
		else
		{
			getcwd(tmppath, PATH_MAX);
			strcat(tmppath, "/");
			strncat(tmppath, pathname, PATH_MAX);
		}
		if (strlen(tmppath) > len-1)
		{
			CommLib_setErrorNum(-101);
			return(-1);
		}
		strncpy(dirname, tmppath, len);
		return(0);
	}

	CommLib_setErrorNum(-103);
	return(-1);
}

int CommLib_isDirEmpty(const char *pathname)
{
	DIR *dp;
	struct dirent *dirp;
	int flag = 1;

	if ((dp = opendir(pathname)) == NULL)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}
	while ((dirp = readdir(dp)) != NULL)
	{
		if (strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0)
			continue;
		else
		{
			flag = 0; /*dir is not empty.*/
			break;
		}
	}
	if (closedir(dp) < 0)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}

	return(flag);
}

long long CommLib_getDirSize(const char *pathname)
{
	long long dirsize = 0;

	if (_CommLib_getDirSize(pathname, &dirsize) < 0)
		return(-1); /*comm_errno has set in _CommLib_getDirSize().*/

	return(dirsize);	
}

int CommLib_getDirFileCount(const char *pathname)
{
	int filecount = 0;	

	if (_CommLib_getDirFileCount(pathname, &filecount) < 0)
		return(-1);

	return(filecount);
}

int CommLib_mkDir(const char *pathname, mode_t mode)
{
	if (mkdir(pathname, mode) < 0)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}

	return(0);
}

int CommLib_setDirMode(const char *pathname, mode_t mode)
{
	if (chmod(pathname, mode) < 0)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}

	return(0);
}

int CommLib_copyDir(const char *oldpath, const char *newpath, int flag, long sleeptime)
{
	int fdold, fdnew;
	char srcbuf[PATH_MAX], dstbuf[PATH_MAX];
	DIR *dp;
	struct dirent *ep;
	struct stat oldbuf, newbuf;

	/*judge the oldpath and newpath is the same dir.*/
	if ((fdold = open(oldpath, O_RDONLY)) < 0)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}
	if (fstat(fdold, &oldbuf) != 0) 
	{
		CommLib_setErrorNum(-100);
		close(fdold);
		return(-1);
	}
	if ((fdnew = open(newpath, O_RDONLY)) >= 0)
	{
		if (stat(newpath, &newbuf) == 0)
		{
			if (oldbuf.st_ino == newbuf.st_ino)	
			{
				close(fdold);
				close(fdnew);
				return(0);
			}
		}
		close(fdnew);
	}
	close(fdold);

	mkdir(newpath, oldbuf.st_mode); /*if newpath exist, ignore; else create.*/
	if ((dp = opendir(oldpath)) == NULL)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}
	while ((ep = readdir(dp)) != NULL) 
	{
		snprintf(srcbuf, PATH_MAX, "%s/%s", oldpath, ep->d_name);
		snprintf(dstbuf, PATH_MAX, "%s/%s", newpath, ep->d_name);
		if (strcmp(ep->d_name, ".") == 0 || strcmp(ep->d_name, "..") == 0)
			continue;
		if (lstat(srcbuf, &oldbuf) != 0) 
		{
			closedir(dp);
			CommLib_setErrorNum(-100);
			return(-1);
		}
		if (S_ISDIR(oldbuf.st_mode))
		{
			if (CommLib_copyDir(srcbuf, dstbuf, flag, sleeptime) < 0)
			{
				closedir(dp);
				return(-1);
			}
		}
		else if (S_ISREG(oldbuf.st_mode)) 
		{
			if (CommLib_copyFile(srcbuf, dstbuf, flag) < 0)
			{
				closedir(dp);
				return(-1);
			}
			if (sleeptime != 0)
				usleep(sleeptime);
		}
		else if (S_ISLNK(oldbuf.st_mode)) 
		{
			char tmpbuf[PATH_MAX];
			int len;

			if ((len = readlink(srcbuf, tmpbuf, PATH_MAX)) < 0)	
			{
				closedir(dp);
				CommLib_setErrorNum(-100);
				return(-1);
			}	
			tmpbuf[len] = '\0';
			unlink(dstbuf); /*ignore old link is exist, unlink and relink it.*/
			if (symlink(tmpbuf, dstbuf) < 0)
			{
				closedir(dp);
				CommLib_setErrorNum(-100);
				return(-1);
			}
		}
		else
			; /*others is ignored.*/
	}

	if (closedir(dp) < 0)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}
	return(0);
}

int CommLib_renameDir(const char *oldpath, const char *newpath)
{
	if (rename(oldpath, newpath))
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}

	return(0);
}

int CommLib_rmDir(const char *pathname)
{
	char dirbuf[PATH_MAX];
	DIR *dp;
	struct dirent *ep;
	struct stat buf;

	if ((dp = opendir(pathname)) == NULL)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}
	while ((ep = readdir(dp)) != NULL)
	{
		snprintf(dirbuf, PATH_MAX, "%s/%s", pathname, ep->d_name);
		if (strcmp(ep->d_name, ".") == 0 || strcmp(ep->d_name, "..") == 0)
			continue;
		if (lstat(dirbuf, &buf) != 0)
		{
			closedir(dp);
			CommLib_setErrorNum(-100);
			return(-1);
		}
		if (S_ISDIR(buf.st_mode))
		{
			if (CommLib_rmDir(dirbuf) < 0)
			{
				closedir(dp);
				return(-1);
			}
		}
		else
		{
			if (unlink(dirbuf) < 0)
			{
				closedir(dp);
				CommLib_setErrorNum(-100);
				return(-1);
			}
			g_del_file_count++;
			usleep(200*1000);
		}
	}
	if (closedir(dp) < 0)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}
	if (rmdir(pathname) < 0)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}

	return(0);
}

int CommLib_getDelFileCount(void)
{
	return(g_del_file_count);
}

int CommLib_clearDelFileCount(void)
{
	g_del_file_count = 0;
	return(0);
}

/*some tools function, not in the commlib.*/
static int lock_reg(int fd, int cmd, int type, off_t offset, int whence, off_t len)
{
	struct flock lock;

	lock.l_type = type; /* F_RDLCK, F_WRLCK, F_UNLCK */
	lock.l_start = offset; /* byte offset, relative to l_whence */
	lock.l_whence = whence; /* SEET_SET, SEEK_CUR, SEEK_END */
	lock.l_len = len; /* #bytes (0 means to EOF) */

	return(fcntl(fd, cmd, &lock));
}

static int _CommLib_getDirSize(const char *pathname, long long *dirsize)
{
	char dirbuf[PATH_MAX];
	DIR *dp;
	struct dirent *ep;
	struct stat buf;

	if ((dp = opendir(pathname)) == NULL)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}
	while ((ep = readdir(dp)) != NULL)
	{
		snprintf(dirbuf, PATH_MAX, "%s/%s", pathname, ep->d_name);
		if (strcmp(ep->d_name, ".") == 0 || strcmp(ep->d_name, "..") == 0)
			continue;
		if (lstat(dirbuf, &buf) != 0)
		{
			closedir(dp);
			CommLib_setErrorNum(-100);
			return(-1);
		}
		if (S_ISDIR(buf.st_mode))
		{
			if (_CommLib_getDirSize(dirbuf, dirsize) < 0)
			{
				closedir(dp);
				return(-1);
			}
		}
		else if (S_ISREG(buf.st_mode))
			*dirsize = *dirsize + buf.st_size;
	}
	if (closedir(dp) < 0)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}
	*dirsize = *dirsize + 3*4096; /*dir's size is 4KB, each dir has ".", ".." and self, so 12KB.*/

	return(0);
}

static int _CommLib_getDirFileCount(const char *pathname, int *filecount)
{
	char dirbuf[PATH_MAX];
	DIR *dp;
	struct dirent *ep;
	struct stat buf;

	if ((dp = opendir(pathname)) == NULL)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}
	while ((ep = readdir(dp)) != NULL)
	{
		snprintf(dirbuf, PATH_MAX, "%s/%s", pathname, ep->d_name);
		if (strcmp(ep->d_name, ".") == 0 || strcmp(ep->d_name, "..") == 0)
			continue;
		if (lstat(dirbuf, &buf) != 0)
		{
			closedir(dp);
			CommLib_setErrorNum(-100);
			return(-1);
		}
		if (S_ISDIR(buf.st_mode))
		{
			if (_CommLib_getDirFileCount(dirbuf, filecount) < 0)
			{
				closedir(dp);
				return(-1);
			}
		}
		else if (S_ISREG(buf.st_mode))
			(*filecount) = (*filecount) + 1;
	}
	if (closedir(dp) < 0)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}
	return(0);
}
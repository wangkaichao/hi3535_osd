

#include "commLib.h"

#define ERRORNUM_MAX 100
#define ERRORSTR_LEN 100
#define ERROR_START -100
#define ERROR_STOP -199
static const char g_comm_errorstr[ERRORNUM_MAX][ERRORSTR_LEN] = {
	"Commlib error",
	"No enough buffer size",
	"Not a file",
	"Not a directory",
	"Bad month",
	"Not found gateway",
	"Not found dns",
	"Bad mac address",
	"Bad ip address",
};
static int g_comm_errno;

int CommLib_setErrorNum(int comm_errno)
{
	g_comm_errno = comm_errno;
	return(0);
}

int CommLib_getErrorNum(void)
{
	return(g_comm_errno);
}

int CommLib_getErrorStr(int comm_errno, char *buf, size_t n)
{
	if ((comm_errno > ERROR_START) || (comm_errno < ERROR_STOP))/*no such error code*/
		return(-1);
	if ((strlen(g_comm_errorstr[(-comm_errno) + ERROR_START]) + 1) > n)/*not enough buffer size*/
		return(-1);
	if (comm_errno == ERROR_START)
		strncpy(buf, strerror(errno), n);
	else
		strncpy(buf, g_comm_errorstr[(-comm_errno) + ERROR_START], n);

	return(0);
}

int CommLib_printErrorStr(const char *prefix)
{
	char err_buf[ERRORSTR_LEN];

	if (CommLib_getErrorStr(CommLib_getErrorNum(), err_buf, ERRORSTR_LEN) < 0)
		return(-1);

	printf("%s:comm_errno(%d) %s\n", prefix ? prefix:"ERROR", CommLib_getErrorNum(), err_buf);

	return(0);
}


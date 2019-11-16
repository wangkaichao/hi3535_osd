#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <error.h>
#include <signal.h>

#include "netLib.h"
#include "paramLib.h"
#include "commLib.h"

static void signal_stop(int signo)
{
	printf("[%s]- %d: ~~~~\n", __FUNCTION__, __LINE__);
	exit(1);
}

int main(int argc, char **argv)
{
	//signal(SIGILL, signal_stop);  // 非法指令，堆栈溢出
	//signal(SIGQUIT, signal_stop); // Ctrl-\ and Core
	signal(SIGTERM, signal_stop); // kill hi3535
	signal(SIGINT, signal_stop);  // Ctrl-C
	signal(SIGPIPE, SIG_IGN);     // 在对关闭的socket 进行写操作的时候会发送子进程终止信号。

	printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
	printf("$$$$$$$$$       HI3535-NVR Init           $$$$$$$$$$\n");
	printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
	
	CommLib_InitLib();

	// 初始化参数库
	ParamLib_InitParamLib();
	
	// 启动监听参数改变线程
	ParamLib_StartParamService();
	
    //初始化网络库
	NetLib_InitNetLib();
	NetLib_StartNetService();

	while (1) {
		//printf("run ...\n");
		usleep(5 * 1000 * 1000);
	}
	
	return 0;
}


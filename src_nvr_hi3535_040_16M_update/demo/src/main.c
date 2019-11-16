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
	//signal(SIGILL, signal_stop);  // �Ƿ�ָ���ջ���
	//signal(SIGQUIT, signal_stop); // Ctrl-\ and Core
	signal(SIGTERM, signal_stop); // kill hi3535
	signal(SIGINT, signal_stop);  // Ctrl-C
	signal(SIGPIPE, SIG_IGN);     // �ڶԹرյ�socket ����д������ʱ��ᷢ���ӽ�����ֹ�źš�

	printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
	printf("$$$$$$$$$       HI3535-NVR Init           $$$$$$$$$$\n");
	printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
	
	CommLib_InitLib();

	// ��ʼ��������
	ParamLib_InitParamLib();
	
	// �������������ı��߳�
	ParamLib_StartParamService();
	
    //��ʼ�������
	NetLib_InitNetLib();
	NetLib_StartNetService();

	while (1) {
		//printf("run ...\n");
		usleep(5 * 1000 * 1000);
	}
	
	return 0;
}


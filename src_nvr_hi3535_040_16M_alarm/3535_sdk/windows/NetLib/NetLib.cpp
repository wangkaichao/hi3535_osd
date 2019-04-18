// NetLib.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include <Winsock2.h>
#include <Windows.h>

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h> 
#include "NetLib.h"

#define USE_DEBUG	(0)
#if USE_DEBUG

#define logErr(...) do {\
  printf("[%s:%d]", __FUNCTION__, __LINE__); \
  printf(__VA_ARGS__); \
  printf("\n"); \
} while (0)

#define logDbg(...) do {\
  printf("[%s:%d]", __FUNCTION__, __LINE__); \
  printf(__VA_ARGS__); \
  printf("\n"); \
} while (0)

#else

#define logErr(...)	
#define logDbg(...) 

#endif

#define NET_ID                      (0x5555AAAA)
#define NET_DATA_SIZE               (200 * 1024)

//NET CMD
#define NET_CMD_SEARCH              (WM_USER + 101)
#define NET_CMD_SETNET              (WM_USER + 102)
#define NET_CMD_UPDATE              (WM_USER + 103)
#define NET_CMD_HEART               (WM_USER + 104)
#define NET_CMD_EXIT                (WM_USER + 105)
#define NET_CMD_ALARM               (WM_USER + 106)
#define NET_CMD_REBOOT              (WM_USER + 107)
#define NET_CMD_SETALARM            (WM_USER + 108) // 设置告警结构体
#define NET_CMD_GETALARM            (WM_USER + 109) // 获取告警结构体

//Search Port
#define SEARCH_RCV_PORT             (23452)
#define SEARCH_SND_PORT             (23451)

typedef struct {
  int m_id;
  int m_type;
  int m_status;
  int m_size;
} net_msg_head_st;

typedef struct {
  net_msg_head_st m_head;
  unsigned char m_data[NET_DATA_SIZE];
} net_msg_st;

typedef struct {
  SOCKET sock;
  int enRsp;

  HANDLE rcvEvtCrt;
  HANDLE sndEvtCrt;

  int rcvRun;
  int rcvStatus;
  DWORD rcvThreadID;
  HANDLE rcvThreadHandle;
  HANDLE rcvEvt;
  net_msg_st rcvMsg;

  int sndRun;
  int sndStatus;
  DWORD sndThreadID;
  HANDLE sndThreadHandle;
  HANDLE sndEvt;
  net_msg_st sndMsg;

} net_sdk_st;


typedef struct
{
  int port_no;  //监听端口
  int net_type; //网络类型
  int rtspport; //rtst端口
  int web_port; //web 端口
  char password[32];
  char username[32];
  int channelnum;
  char mac_addr[24];
  char ip_addr[16];
  char host_name[24];  //设备名
  char version[24];    //版本日期
  char device_type[24];//产品型号	 
} msg_broadcast_st;

typedef struct network_config_t
{
  unsigned short 	      eth_type;
  unsigned short 	      eth_protocol;
  char	                eth_mac[32];
  unsigned short        eth_ipmode;
  char                  eth_ipaddr[32];
  char                  eth_subnet_mask[32];
  char                  eth_gateway[32];
  char                  dns_server1[32];
  char                  dns_server2[32];
  unsigned short        dns_enable;
  unsigned short        dhcp_flag;
} network_config_t;

static int isIp(const char *p);
static int isPort(short port);
static int net_msg_snd(int sock, const net_msg_st *pmsg);
static int snd(int fd, const char *vptr, int n, int flag);
static int net_msg_rcv(int sock, net_msg_st *pmsg);
static int rcv(int fd, char *vptr, int n, int flag);
static int procRcvMsg(long handle);

static DWORD WINAPI rcvThreadProc(LPVOID param);
static DWORD WINAPI sndThreadProc(LPVOID param);

static int isIp(const char *p)
{
  return 1;
}

static int isPort(short port)
{
  return (port > 0 && port < 65535);
}

static int net_msg_snd(int sock, const net_msg_st *pmsg)
{
  fd_set wset;
  FD_ZERO(&wset);
  FD_SET((unsigned int)sock, &wset);

  struct timeval tval;
  tval.tv_sec = 1;
  tval.tv_usec = 0;
  if (select(sock + 1, 0, &wset, 0, &tval) <= 0) {
    logErr("select err");
    return -1;
  }

  int ret = snd(sock, (const char *)pmsg, sizeof(pmsg->m_head) + pmsg->m_head.m_size, 0);

  if (ret != sizeof(pmsg->m_head) + pmsg->m_head.m_size) {
    logErr("snd ret(%d), %d + %d", ret, sizeof(pmsg->m_head), pmsg->m_head.m_size);
    return -1;
  }

  return 0;
}

static int snd(int fd, const char *vptr, int n, int flag)
{
  int  nleft;
  int	 nwritten;
  const char *ptr;

  ptr = vptr;
  nleft = n;
  while (nleft > 0)
  {
    if ((nwritten = send(fd, ptr, nleft, flag)) < 0)
    {
      DWORD dwError = GetLastError();
      if (dwError == WSAEINTR)
      {
        logDbg("~~~~~~1");
        nwritten = 0;
      }
      else if (dwError == WSAEWOULDBLOCK)
      {
        logDbg("~~~~~~2");
        Sleep(1);
        continue;
      }
      else
      {
        logErr("");
        return -1;
      }
    }
    else if (nwritten == 0)
    {
      logErr("");
      break;  //EOF
    }

    nleft -= nwritten;
    ptr += nwritten;
  }

  return (n - nleft);
}

static int net_msg_rcv(int sock, net_msg_st *pmsg)
{
  int ret = 0;

  memset(pmsg, 0, sizeof(net_msg_st));

  ret = rcv(sock, (char *)&pmsg->m_head, sizeof(pmsg->m_head), 0);
  if (ret <= 0) {
    logErr("err: rcv=%d", ret);
    return -1;
  }

  if (pmsg->m_head.m_id != NET_ID) {
    logErr("err(%d)", pmsg->m_head.m_id);
    return -1;
  }

  if (pmsg->m_head.m_size < 0 || pmsg->m_head.m_size > NET_DATA_SIZE) {
    logErr("err: msg_size=%d", pmsg->m_head.m_size);
    return -1;
  }

  if (pmsg->m_head.m_size == 0)
    return 0;

  ret = rcv(sock, (char *)pmsg->m_data, pmsg->m_head.m_size, 0);
  if (ret <= 0 || ret != pmsg->m_head.m_size) {
    logErr("err: rcv=%d", ret);
    return -1;
  }

  return 0;
}

static int rcv(int fd, char *vptr, int n, int flag)
{
  int nleft, nread;
  char * ptr;

  ptr = vptr;
  nleft = n;
  while (nleft > 0)
  {
    if ((nread = recv(fd, ptr, nleft, flag)) < 0)
    {
      DWORD dwError = GetLastError();
      if (dwError == WSAEINTR)
      {
        nread = 0;
      }
      else if (dwError == WSAEWOULDBLOCK)
      {
        Sleep(1);
        continue;
      }
      else
      {
        logErr("err");
        return -1;
      }
    }
    else if (nread == 0)
    {
      logErr("EOF");
      break;  //EOF
    }

    nleft -= nread;
    ptr += nread;
  }

  return (n - nleft);
}

int procRcvMsg(long handle)
{
  net_sdk_st *p = (net_sdk_st *)handle;

  switch (p->rcvMsg.m_head.m_type) {

    case NET_CMD_HEART:
    case NET_CMD_REBOOT:
    case NET_CMD_SETNET:
    case NET_CMD_ALARM:
    case NET_CMD_UPDATE:
    case NET_CMD_SETALARM:
    case NET_CMD_GETALARM:
    {
      if (p->enRsp) {
        p->enRsp = 0;
        SetEvent(p->rcvEvt);
      }
      break;
    }
    default:
    {
      break;
    }
  }

  return 0;
}

static DWORD WINAPI rcvThreadProc(LPVOID param)
{
  logDbg("start~~~~~~");

  net_sdk_st *p = (net_sdk_st *)param;
  SetEvent(p->rcvEvtCrt);

  while (p->rcvRun) {
    fd_set rset;
    FD_ZERO(&rset);
    FD_SET((unsigned int)p->sock, &rset);

    struct timeval tval;
    tval.tv_sec = 0;
    tval.tv_usec = 100*1000;
    
    int ret = select(p->sock + 1, &rset, 0, 0, &tval);
    if (ret == 0) {
      continue;
    }
    else if (ret == SOCKET_ERROR) {
      logErr("select fail");
      p->rcvStatus = -1;
      p->rcvRun = 0;
    }
    else {
      int ret = net_msg_rcv(p->sock, &p->rcvMsg);
      if (ret < 0) {
        p->rcvStatus = -1;
        p->rcvRun = 0;
      }
      else {
        procRcvMsg((long)p);
      }
    }
  }

  if (p->enRsp) {
    p->enRsp = 0;
    SetEvent(p->rcvEvt);
  }
  return NULL;
}

static DWORD WINAPI sndThreadProc(LPVOID param)
{
  logDbg("start~~~~~~");
  net_sdk_st *p = (net_sdk_st *)param;
  SetEvent(p->sndEvtCrt);

  MSG msg;
  PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);

  while (p->sndRun) {
    SetEvent(p->sndEvt);

    BOOL bRet;
    if ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0)
    {
      if (bRet == -1)
        continue;
      else
      {
        switch (msg.message)
        {
          case NET_CMD_EXIT:
          {
            p->sndRun = 0;
            break;
          }
          case NET_CMD_HEART:
          case NET_CMD_REBOOT:
          case NET_CMD_SETNET:
          case NET_CMD_ALARM:
          case NET_CMD_UPDATE:
          case NET_CMD_SETALARM:
          case NET_CMD_GETALARM:
          {
            if (net_msg_snd(p->sock, &p->sndMsg) < 0)
            {
              p->sndStatus = -1;
              p->sndRun = 0;
            }
            break;
          }
          default:
          {
            break;
          }
        }
      }
    } //if
  }

  return NULL;
}


NETLIB_API int NetLib_Init(void)
{
  WORD wVersionRequested = MAKEWORD(2, 2);
  WSADATA wsaData;
  int err = WSAStartup(wVersionRequested, &wsaData);
  if (err != 0)
  {
    /* Tell the user that we could not find a usable */
    /* WinSock DLL.                                  */
    return -1;
  }

  if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
  {
    /* Tell the user that we could not find a usable */
    /* WinSock DLL.                                  */
    WSACleanup();
    return -1;
  }

  return 0;
}

NETLIB_API int NetLib_DeInit(void)
{
  WSACleanup();
  return 0;
}

/************************************************************************/
/* 广播搜索设备    udp广播搜索                                            */
/* search_st *plist: 用户申请空间，获取搜索到的设备                        */
/* int *plen: 用户申请空间，获取设备个数                                  */
/* int maxnum : 但搜索个数大于maxnum时，只返回maxnum个                    */
/* int missecond : 搜索等待时间ms                                        */
/************************************************************************/
NETLIB_API int NetLib_Search(search_st *plist, int *plen, int maxnum, int missecond)
{
  int ret = 0;

  if (plist == NULL || maxnum <= 0 || missecond <= 0) {
    logErr("intput param err");
    return -1;
  }

  net_msg_st snd_net_msg;
  net_msg_st rev_net_msg;
  
  *plen = 0;
  
  //udp broadcast
  SOCKET sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd == INVALID_SOCKET) {
    logErr("socket fail");
    return -1;
  }
  
  int opt_val = 1;
  if (SOCKET_ERROR == setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, (const char*)&opt_val, sizeof(opt_val))) {
    logErr("setsockopt SO_BROADCAST fail");
    closesocket(sockfd);
    return -1;
  }
  
  struct sockaddr_in LocalAddr;
  LocalAddr.sin_family = AF_INET;
  LocalAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  LocalAddr.sin_port = htons(SEARCH_RCV_PORT);

  if (bind(sockfd, (struct sockaddr *)&LocalAddr, sizeof(LocalAddr)) < 0) {
    logErr("bind fail");
    return -1;
  }

  struct sockaddr_in RemoteAddr;
  RemoteAddr.sin_family = AF_INET;
  RemoteAddr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
  RemoteAddr.sin_port = htons(SEARCH_SND_PORT);

  snd_net_msg.m_head.m_id = NET_ID;
  snd_net_msg.m_head.m_type = NET_CMD_SEARCH;
  snd_net_msg.m_head.m_status = 0;
  snd_net_msg.m_head.m_size = 0;
  
  if (SOCKET_ERROR == sendto(sockfd, (char *)&snd_net_msg.m_head, sizeof(snd_net_msg.m_head), MSG_DONTROUTE, (struct sockaddr *)&RemoteAddr, sizeof(RemoteAddr))) {
    logErr("sendto fail");
    closesocket(sockfd);
    return -1;
  }

  msg_broadcast_st *pbroadcast = (msg_broadcast_st *)malloc(sizeof(msg_broadcast_st)* maxnum);
  if (pbroadcast == NULL) {
    logErr("malloc err");
    closesocket(sockfd);
    return -1;
  }
  memset(pbroadcast, 0, sizeof(msg_broadcast_st)* maxnum);

  while (1) {
    struct timeval tval;
    fd_set rset;
    FD_ZERO(&rset);
    FD_SET(sockfd, &rset);

    tval.tv_sec = missecond / 1000;
    tval.tv_usec = missecond * 1000;
    if (select(sockfd + 1, &rset, 0, 0, &tval) <= 0) {
      logErr("select timeout or net disconnect");
      //搜索完成，超时退出
      break;
    }

    unsigned int len = recvfrom(sockfd, (char *)&rev_net_msg, sizeof(net_msg_st), 0, NULL, NULL);
    if (len != sizeof(rev_net_msg.m_head) + sizeof(msg_broadcast_st)) {
      logErr("recvfrom len %d != %d", len, sizeof(rev_net_msg.m_head) + sizeof(msg_broadcast_st));
      ret = -1;
      break;
    }

    memcpy(pbroadcast + *plen, (char *)rev_net_msg.m_data, sizeof(msg_broadcast_st));
    strcpy(plist[*plen].ip_addr, pbroadcast[*plen].ip_addr);
    strcpy(plist[*plen].mac_addr, pbroadcast[*plen].mac_addr);
    strcpy(plist[*plen].version, pbroadcast[*plen].version);
    (*plen)++;

    if (*plen >= maxnum)
      break;
  }

  closesocket(sockfd);
  free(pbroadcast);

  return ret;
}

/************************************************************************/
/* 连接设备，TCP长连接                                                   */
/************************************************************************/
NETLIB_API int NetLib_Open(long *handle, const char * ip, short port)
{
  if (handle == NULL || !isIp(ip) || !isPort(port)) {
    logErr("");
    return -1;
  }
  
  SOCKET cli = socket(PF_INET, SOCK_STREAM, 0);
  int opt = 1;
  if (setsockopt(cli, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt)) == -1) {
    logErr("");
    goto ERR1;
  }

  int size = 512 * 1024;
  if (setsockopt(cli, SOL_SOCKET, SO_SNDBUF, (char *)&size, sizeof(size)) == SOCKET_ERROR) {
    logErr("");
    goto ERR1;
  }

  if (setsockopt(cli, SOL_SOCKET, SO_RCVBUF, (char *)&size, sizeof(size)) == SOCKET_ERROR) {
    logErr("");
    goto ERR1;
  }


  int timeout = 3;
  if (setsockopt(cli, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout)) == SOCKET_ERROR) {
    logErr("");
    goto ERR1;
  }

  if (setsockopt(cli, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) == SOCKET_ERROR) {
    logErr("");
    goto ERR1;
  }

  int flag = 1;
  if (setsockopt(cli, IPPROTO_TCP, 0x0001, (char *)&flag, sizeof(flag)) < 0) {
    logErr("");
    goto ERR1;
  }

  struct sockaddr_in server;
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = inet_addr(ip);
  server.sin_port = htons(port);

  u_long mode = 1;
  ioctlsocket(cli, FIONBIO, &mode);

  if (connect(cli, (struct sockaddr *)&server, sizeof(struct sockaddr)) < 0) {
    fd_set set;
    FD_ZERO(&set);
    FD_SET(cli, &set);

    struct timeval tm;
    tm.tv_sec = 0;
    tm.tv_usec = 100*1000;

    if (select(-1, NULL, &set, NULL, &tm) <= 0)
      goto ERR1;
    else
    {
      int error = -1;
      int optLen = sizeof(int);
      getsockopt(cli, SOL_SOCKET, SO_ERROR, (char*)&error, &optLen);
      if (error != 0)
        goto ERR1;
    }
  }

  mode = 0;
  ioctlsocket(cli, FIONBIO, &mode);

  net_sdk_st *p = (net_sdk_st *)malloc(sizeof(net_sdk_st));
  memset(p, 0, sizeof(net_sdk_st));
  p->sock = cli;

  p->rcvEvtCrt = CreateEvent(NULL, FALSE, FALSE, NULL);
  p->sndEvtCrt = CreateEvent(NULL, FALSE, FALSE, NULL);

  p->rcvEvt = CreateEvent(NULL, FALSE, FALSE, NULL);
  p->sndEvt = CreateEvent(NULL, FALSE, FALSE, NULL);

  p->rcvRun = 1;
  p->rcvThreadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)rcvThreadProc, p, 0, &p->rcvThreadID);
  WaitForSingleObject(p->rcvEvtCrt, INFINITE);

  p->sndRun = 1;
  p->sndThreadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)sndThreadProc, p, 0, &p->sndThreadID);
  WaitForSingleObject(p->sndEvtCrt, INFINITE);

  *handle = (long)p;

  logDbg("~~~~~~ok");
  return 0;

ERR1:
  closesocket(cli);
  return -1;
}

/************************************************************************/
/* 断开设备                                                              */
/************************************************************************/
NETLIB_API int NetLib_Close(long *handle)
{
  if (!handle || !(*handle))
    return -1;

  net_sdk_st *p = (net_sdk_st *)(*handle);

  p->rcvRun = 0;
  WaitForSingleObject(p->rcvThreadHandle, INFINITE);
  CloseHandle(p->rcvThreadHandle);
  CloseHandle(p->rcvEvtCrt);
  CloseHandle(p->rcvEvt);

  if (p->sndRun) {
    if (!PostThreadMessage(p->sndThreadID, NET_CMD_EXIT, 0, 0))
      logErr("post message failed,errno:%d\n", GetLastError());
  }
  WaitForSingleObject(p->sndThreadHandle, INFINITE);
  CloseHandle(p->sndThreadHandle);
  CloseHandle(p->sndEvtCrt);
  CloseHandle(p->sndEvt);

  closesocket(p->sock);
  free(p);

  *handle = 0;
  logDbg("~~~~~~ok");
  return 0;
}

/************************************************************************/
/* 网络心跳命令 可以用于侦测网络连接是否断开                                */
/************************************************************************/
NETLIB_API int NetLib_HeartBeat(long *handle)
{
  if (!handle || !(*handle))
    return -1;

  net_sdk_st *p = (net_sdk_st *)(*handle);

  if (!p->rcvRun || !p->sndRun)
    return -1;

  if (WaitForSingleObject(p->sndEvt, 3000) != WAIT_OBJECT_0)
    return -1;

  memset(&p->sndMsg, 0, sizeof(net_msg_st));
  p->sndMsg.m_head.m_id = NET_ID;
  p->sndMsg.m_head.m_type = NET_CMD_HEART;
  p->sndMsg.m_head.m_status = 0;
  p->sndMsg.m_head.m_size = 0;

  p->enRsp = 1; //需要回复
  if (!PostThreadMessage(p->sndThreadID, NET_CMD_HEART, 0, 0)) {
    logErr("post message failed,errno:%d\n", GetLastError());
    return -1;
  }

  if (WaitForSingleObject(p->rcvEvt, 3000) != WAIT_OBJECT_0)
    return -1;

  if (p->sndStatus < 0 || p->rcvStatus < 0)
    return -1;

  return p->rcvMsg.m_head.m_status;
}


/************************************************************************/
/* 设备重启命令                                                          */
/************************************************************************/
NETLIB_API int NetLib_Reboot(long *handle)
{
  if (!handle || !(*handle))
    return -1;

  net_sdk_st *p = (net_sdk_st *)(*handle);

  if (!p->rcvRun || !p->sndRun)
    return -1;

  if (WaitForSingleObject(p->sndEvt, 3000) != WAIT_OBJECT_0)
    return -1;

  memset(&p->sndMsg, 0, sizeof(net_msg_st));
  p->sndMsg.m_head.m_id = NET_ID;
  p->sndMsg.m_head.m_type = NET_CMD_REBOOT;
  p->sndMsg.m_head.m_status = 0;
  p->sndMsg.m_head.m_size = 0;

  p->enRsp = 1; //需要回复
  if (!PostThreadMessage(p->sndThreadID, NET_CMD_REBOOT, 0, 0)) {
    logErr("post message failed,errno:%d\n", GetLastError());
    return -1;
  }

  if (WaitForSingleObject(p->rcvEvt, 3000) != WAIT_OBJECT_0)
    return -1;

  if (p->sndStatus < 0 || p->rcvStatus < 0)
    return -1;

  return p->rcvMsg.m_head.m_status;
}

/************************************************************************/
/* 修改网络设置                                                          */
/* const char *ip: 新的ip地址                                            */
/* const char *mask: 新的子网掩码                                        */
/* const char *gateway: 新的网关                                         */
/************************************************************************/
NETLIB_API int NetLib_SetNet(long *handle, const char *ip, const char *submask, const char *gateway)
{
  if (!handle || !(*handle))
    return -1;

  net_sdk_st *p = (net_sdk_st *)(*handle);

  if (!p->rcvRun || !p->sndRun)
    return -1;

  if (WaitForSingleObject(p->sndEvt, 3000) != WAIT_OBJECT_0)
    return -1;

  memset(&p->sndMsg, 0, sizeof(net_msg_st));
  p->sndMsg.m_head.m_id = NET_ID;
  p->sndMsg.m_head.m_type = NET_CMD_SETNET;
  p->sndMsg.m_head.m_status = 0;
  p->sndMsg.m_head.m_size = sizeof(network_config_t);

  network_config_t tmp;
  memset(&tmp, 0, sizeof(network_config_t));
  strcpy(tmp.eth_ipaddr, ip);
  strcpy(tmp.eth_subnet_mask, submask);
  strcpy(tmp.eth_gateway, gateway);
  memmove(p->sndMsg.m_data, &tmp, sizeof(network_config_t));

  p->enRsp = 1; //需要回复
  if (!PostThreadMessage(p->sndThreadID, NET_CMD_SETNET, 0, 0)) {
    logErr("post message failed,errno:%d\n", GetLastError());
    return -1;
  }

  if (WaitForSingleObject(p->rcvEvt, 3000) != WAIT_OBJECT_0)
    return -1;

  if (p->sndStatus < 0 || p->rcvStatus < 0)
    return -1;

  return p->rcvMsg.m_head.m_status;
}

/************************************************************************/
/* 升级命令                                                              */
/* filename: 升级文件绝对路径                                             */
/************************************************************************/
NETLIB_API int NetLib_Update(long *handle, const char *filename)
{
  if (!handle || !(*handle))
    return -1;

  net_sdk_st *p = (net_sdk_st *)(*handle);

  if (!p->rcvRun || !p->sndRun)
    return -1;

  FILE *fp = fopen(filename, "rb");
  if (!fp)
    return -1;

  fseek(fp, 0L, SEEK_END);
  int size = ftell(fp);
  fseek(fp, 0L, SEEK_SET);

  int sndnum = size + sizeof(int);
  int cnt = sndnum / NET_DATA_SIZE;
  if (sndnum % NET_DATA_SIZE != 0)
    cnt++;

  for (int i = 0; i < cnt; i++) {

    if (WaitForSingleObject(p->sndEvt, 3000) != WAIT_OBJECT_0) {
      logErr("");
      p->rcvMsg.m_head.m_status = -1;
      goto END;
    }

    memset(&p->sndMsg, 0, sizeof(net_msg_st));
    p->sndMsg.m_head.m_id = NET_ID;
    p->sndMsg.m_head.m_type = NET_CMD_UPDATE;
    p->sndMsg.m_head.m_status = i;
    p->sndMsg.m_head.m_size = (sndnum >= NET_DATA_SIZE ? NET_DATA_SIZE : sndnum);

    if (i == 0) {
      memmove(p->sndMsg.m_data, &sndnum, sizeof(int));
      fread(p->sndMsg.m_data + sizeof(int), p->sndMsg.m_head.m_size - sizeof(int), 1, fp);
    }
    else {
      fread(p->sndMsg.m_data, p->sndMsg.m_head.m_size, 1, fp);
    }
    sndnum -= p->sndMsg.m_head.m_size;

    p->enRsp = 1; //需要回复
    if (!PostThreadMessage(p->sndThreadID, NET_CMD_UPDATE, 0, 0)) {
      logErr("post message failed,errno:%d\n", GetLastError());
      p->rcvMsg.m_head.m_status = -1;
      goto END;
    }

    if (WaitForSingleObject(p->rcvEvt, 60000) != WAIT_OBJECT_0) {
      logErr("");
      p->rcvMsg.m_head.m_status = -1;
      goto END;
    }

    if (p->sndStatus < 0 || p->rcvStatus < 0) {
      p->rcvMsg.m_head.m_status = -1;
      goto END;
    }
  }

END:
  fclose(fp);
  fp = NULL;

  return p->rcvMsg.m_head.m_status;
}

/************************************************************************/
/* 发送告警设置                                                          */
/* const char *alarm: 告警设置 1->故障，0->正常                           */
/* int num: 通道个数                                                     */
/************************************************************************/
NETLIB_API int NetLib_SndAlarm(long *handle, const char *alarm, int num)
{
  if (!handle || !(*handle))
    return -1;

  net_sdk_st *p = (net_sdk_st *)(*handle);

  if (!p->rcvRun || !p->sndRun)
    return -1;

  if (WaitForSingleObject(p->sndEvt, 3000) != WAIT_OBJECT_0)
    return -1;

  memset(&p->sndMsg, 0, sizeof(net_msg_st));
  p->sndMsg.m_head.m_id = NET_ID;
  p->sndMsg.m_head.m_type = NET_CMD_ALARM;
  p->sndMsg.m_head.m_status = 0;
  p->sndMsg.m_head.m_size = sizeof(int) + num;
  memmove(p->sndMsg.m_data, &num, sizeof(int));
  memmove(p->sndMsg.m_data + sizeof(int), alarm, num);

  p->enRsp = 1; //需要回复
  if (!PostThreadMessage(p->sndThreadID, NET_CMD_ALARM, 0, 0)) {
    logErr("post message failed,errno:%d\n", GetLastError());
    return -1;
  }

  if (WaitForSingleObject(p->rcvEvt, 3000) != WAIT_OBJECT_0)
    return -1;

  if (p->sndStatus < 0 || p->rcvStatus < 0)
    return -1;

  return p->rcvMsg.m_head.m_status;
}

/************************************************************************/
/* 设置告警通道个数，字体，内容                                            */
/************************************************************************/
NETLIB_API int NetLib_SetAlarm(long *handle, const alarm_config_t *alarm)
{
  if (!handle || !(*handle))
    return -1;

  net_sdk_st *p = (net_sdk_st *)(*handle);

  if (!p->rcvRun || !p->sndRun)
    return -1;

  if (WaitForSingleObject(p->sndEvt, 3000) != WAIT_OBJECT_0)
    return -1;

  memset(&p->sndMsg, 0, sizeof(net_msg_st));
  p->sndMsg.m_head.m_id = NET_ID;
  p->sndMsg.m_head.m_type = NET_CMD_SETALARM;
  p->sndMsg.m_head.m_status = 0;
  p->sndMsg.m_head.m_size = sizeof(alarm_config_t);
  memmove(p->sndMsg.m_data, alarm, sizeof(alarm_config_t));

  p->enRsp = 1; //需要回复
  if (!PostThreadMessage(p->sndThreadID, NET_CMD_SETALARM, 0, 0)) {
    logErr("post message failed,errno:%d\n", GetLastError());
    return -1;
  }

  if (WaitForSingleObject(p->rcvEvt, 3000) != WAIT_OBJECT_0)
    return -1;

  if (p->sndStatus < 0 || p->rcvStatus < 0)
    return -1;

  return p->rcvMsg.m_head.m_status;
}

/************************************************************************/
/* 获取告警通道个数，字体，内容                                            */
/************************************************************************/
NETLIB_API int NetLib_GetAlarm(long *handle, alarm_config_t *alarm)
{
  if (!handle || !(*handle))
    return -1;

  net_sdk_st *p = (net_sdk_st *)(*handle);

  if (!p->rcvRun || !p->sndRun)
    return -1;

  if (WaitForSingleObject(p->sndEvt, 3000) != WAIT_OBJECT_0)
    return -1;

  memset(&p->sndMsg, 0, sizeof(net_msg_st));
  p->sndMsg.m_head.m_id = NET_ID;
  p->sndMsg.m_head.m_type = NET_CMD_GETALARM;
  p->sndMsg.m_head.m_status = 0;
  p->sndMsg.m_head.m_size = 0;

  p->enRsp = 1; //需要回复
  if (!PostThreadMessage(p->sndThreadID, NET_CMD_GETALARM, 0, 0)) {
    logErr("post message failed,errno:%d\n", GetLastError());
    return -1;
  }

  if (WaitForSingleObject(p->rcvEvt, 3000) != WAIT_OBJECT_0)
    return -1;

  if (p->sndStatus < 0 || p->rcvStatus < 0)
    return -1;

  memmove(alarm, p->rcvMsg.m_data, sizeof(alarm_config_t));

  return p->rcvMsg.m_head.m_status;
}

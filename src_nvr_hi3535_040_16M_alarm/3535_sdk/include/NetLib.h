#ifndef NETLIB_H
#define NETLIB_H

#ifdef __cplusplus
extern "C"{
#endif

/******************************************************************************
*                             Redefine Data Type
*******************************************************************************/
// Redefine integer data type
typedef signed char        INT8;
typedef signed short       INT16;
typedef signed int         INT32;
typedef unsigned char      UINT8;
typedef unsigned short     UINT16;
typedef unsigned int       UINT32;

#if defined(__GNUC__) || defined(__TMS320C6X_PLATFORM__)
typedef          long long INT64;
typedef unsigned long long UINT64;
//typedef         signed int intptr_t;
#else
typedef          __int64   INT64;
typedef unsigned __int64   UINT64;
#endif

#ifdef __TMS320C6X_PLATFORM__  //__TMS320C6X__
// redefine 40bit data type, only for TI DSP platform
typedef unsigned long       UINT40;
typedef long                INT40;
#else
// redefine 40bit data type, only for VC platform
typedef INT64               UINT40;
typedef INT64               INT40;
#endif

// Redefine float data type
typedef float              FLOAT32;
typedef double             FLOAT64;

// Redefine bool data type
typedef char               BOOL8;
typedef short              BOOL16;
typedef int                BOOL32;

#ifndef TRUE
#define TRUE               1
#endif

#ifndef FALSE
#define FALSE              0
#endif

/******************************************************************************
*                             Dynamic and static lib config
*******************************************************************************/
#if defined(_MSC_VER)

//#define NETLIB_IMPORTS
#if defined(NETLIB_IMPORTS)
#define NETLIB_API extern _declspec(dllimport)
#elif defined(NETLIB_EXPORTS)
#define NETLIB_API extern _declspec(dllexport)
#else
#define NETLIB_API
#endif

#define inline __inline

#elif defined(__GNUC__)
#define NETLIB_API
#else
#define NETLIB_API
#endif

#pragma comment(lib, "Ws2_32")

#define SERVER_PORT           (6789)

typedef struct {
  char ip_addr[16];
  char mac_addr[24];
  char version[24];
} search_st;

#define MAX_ALARM_NUM           (256)
#define MAX_ALARM_STRCH         (2 * 17)
typedef struct {
  short   m_chnum;                                        /* 有效通道个数 */
  short   m_fontsize;                                     /* 0->32*32, 1->48*48, 2->64*64 */
  short   m_interval;                                     /* 刷屏时间间隔 */
  char    m_strch[MAX_ALARM_NUM][MAX_ALARM_STRCH];        /* 每个通道最多显示16个汉字，32个ASCII  */
  short   m_reserve;
} alarm_config_t;


/************************************************************************/
/* 初始化windows sock 环境                                               */
/************************************************************************/
NETLIB_API int NetLib_Init(void);

/************************************************************************/
/* 反初始化windows sock 环境                                             */
/************************************************************************/
NETLIB_API int NetLib_DeInit(void);

/************************************************************************/
/* 广播搜索设备    udp广播搜索                                            */
/* search_st *plist: 用户申请空间，获取搜索到的设备                        */
/* int *plen: 用户申请空间，获取设备个数                                  */
/* int maxnum : 但搜索个数大于maxnum时，只返回maxnum个                    */
/* int missecond : 搜索等待时间ms                                        */
/************************************************************************/
NETLIB_API int NetLib_Search(search_st *plist, int *plen, int maxnum, int missecond);

/************************************************************************/
/* 连接设备，TCP长连接                                                   */
/************************************************************************/
NETLIB_API int NetLib_Open(long *handle, const char *ip, short port);

/************************************************************************/
/* 断开设备                                                              */
/************************************************************************/
NETLIB_API int NetLib_Close(long *handle);

/************************************************************************/
/* 网络心跳命令 可以用于侦测网络连接是否断开                                */
/************************************************************************/
NETLIB_API int NetLib_HeartBeat(long *handle);

/************************************************************************/
/* 设备重启命令                                                          */
/************************************************************************/
NETLIB_API int NetLib_Reboot(long *handle);

/************************************************************************/
/* 修改网络设置                                                          */
/* const char *ip: 新的ip地址                                            */
/* const char *submask: 新的子网掩码                                        */
/* const char *gateway: 新的网关                                         */
/************************************************************************/
NETLIB_API int NetLib_SetNet(long *handle, const char *ip, const char *submask, const char *gateway);

/************************************************************************/
/* 升级命令                                                              */
/* filename: 升级文件绝对路径                                             */
/************************************************************************/
NETLIB_API int NetLib_Update(long *handle, const char *filename);

/************************************************************************/
/* 发送告警设置                                                          */
/* const char *alarm: 告警设置 1->故障，0->正常                           */
/* int num: 通道个数                                                     */
/************************************************************************/
NETLIB_API int NetLib_SndAlarm(long *handle, const char *alarm, int num);

/************************************************************************/
/* 设置告警通道个数，字体，内容                                            */
/************************************************************************/
NETLIB_API int NetLib_SetAlarm(long *handle, const alarm_config_t *alarm);

/************************************************************************/
/* 获取告警通道个数，字体，内容                                            */
/************************************************************************/
NETLIB_API int NetLib_GetAlarm(long *handle, alarm_config_t *alarm);

#ifdef __cplusplus
}
#endif

#endif

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
  short   m_chnum;                                        /* ��Чͨ������ */
  short   m_fontsize;                                     /* 0->32*32, 1->48*48, 2->64*64 */
  short   m_interval;                                     /* ˢ��ʱ���� */
  char    m_strch[MAX_ALARM_NUM][MAX_ALARM_STRCH];        /* ÿ��ͨ�������ʾ16�����֣�32��ASCII  */
  short   m_reserve;
} alarm_config_t;


/************************************************************************/
/* ��ʼ��windows sock ����                                               */
/************************************************************************/
NETLIB_API int NetLib_Init(void);

/************************************************************************/
/* ����ʼ��windows sock ����                                             */
/************************************************************************/
NETLIB_API int NetLib_DeInit(void);

/************************************************************************/
/* �㲥�����豸    udp�㲥����                                            */
/* search_st *plist: �û�����ռ䣬��ȡ���������豸                        */
/* int *plen: �û�����ռ䣬��ȡ�豸����                                  */
/* int maxnum : ��������������maxnumʱ��ֻ����maxnum��                    */
/* int missecond : �����ȴ�ʱ��ms                                        */
/************************************************************************/
NETLIB_API int NetLib_Search(search_st *plist, int *plen, int maxnum, int missecond);

/************************************************************************/
/* �����豸��TCP������                                                   */
/************************************************************************/
NETLIB_API int NetLib_Open(long *handle, const char *ip, short port);

/************************************************************************/
/* �Ͽ��豸                                                              */
/************************************************************************/
NETLIB_API int NetLib_Close(long *handle);

/************************************************************************/
/* ������������ ��������������������Ƿ�Ͽ�                                */
/************************************************************************/
NETLIB_API int NetLib_HeartBeat(long *handle);

/************************************************************************/
/* �豸��������                                                          */
/************************************************************************/
NETLIB_API int NetLib_Reboot(long *handle);

/************************************************************************/
/* �޸���������                                                          */
/* const char *ip: �µ�ip��ַ                                            */
/* const char *submask: �µ���������                                        */
/* const char *gateway: �µ�����                                         */
/************************************************************************/
NETLIB_API int NetLib_SetNet(long *handle, const char *ip, const char *submask, const char *gateway);

/************************************************************************/
/* ��������                                                              */
/* filename: �����ļ�����·��                                             */
/************************************************************************/
NETLIB_API int NetLib_Update(long *handle, const char *filename);

/************************************************************************/
/* ���͸澯����                                                          */
/* const char *alarm: �澯���� 1->���ϣ�0->����                           */
/* int num: ͨ������                                                     */
/************************************************************************/
NETLIB_API int NetLib_SndAlarm(long *handle, const char *alarm, int num);

/************************************************************************/
/* ���ø澯ͨ�����������壬����                                            */
/************************************************************************/
NETLIB_API int NetLib_SetAlarm(long *handle, const alarm_config_t *alarm);

/************************************************************************/
/* ��ȡ�澯ͨ�����������壬����                                            */
/************************************************************************/
NETLIB_API int NetLib_GetAlarm(long *handle, alarm_config_t *alarm);

#ifdef __cplusplus
}
#endif

#endif

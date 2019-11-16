#ifndef __NET_LIB_H__
#define __NET_LIB_H__

#define NET_ID                      (0x5555AAAA)
#define NET_DATA_SIZE               (200*1024)
#define NET_MAX_CLIENT              (32)

#define SERVER_PORT                 (6789)
#define LISTEN_NUM                  (3)
#define MAX_EVENT                   (1024)

#define WM_USER                     (0x0400)
#define NET_CMD_SEARCH              (WM_USER + 101)
#define NET_CMD_SETNET              (WM_USER + 102)
#define NET_CMD_UPDATE              (WM_USER + 103)
#define NET_CMD_HEART               (WM_USER + 104)
#define NET_CMD_EXIT                (WM_USER + 105)
#define NET_CMD_ALARM               (WM_USER + 106) // 发送通道故障或者正常
#define NET_CMD_REBOOT              (WM_USER + 107)
#define NET_CMD_SETALARM            (WM_USER + 108) // 设置告警结构体
#define NET_CMD_GETALARM            (WM_USER + 109) // 获取告警结构体

#define SEARCH_RCV_PORT             (23451)
#define SEARCH_SND_PORT             (23452)

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

typedef struct
{
    int port_no; //监听端口
    int net_type; //网络类型
    int rtspport; //rtst端口
    int web_port; //web 端口
    char password[32];
    char username[32];
    int channelnum;
    char mac_addr[24];
    char ip_addr[16];
    char host_name[24]; //设备名
    char version[24];    //版本日期
    char device_type[24];//产品型号	
} msg_broadcast_st;

struct arp_packet_t {
    unsigned char dest_mac[6];
    unsigned char src_mac[6];
    unsigned short arp_type;      // 0x0806
    unsigned short hardware_type; // 0x0001
    unsigned short protocol_type; // 0x0800
    unsigned char src_mac_length; // 0x0006
    unsigned char src_ip_length;  // 0x0004
    unsigned short contorl_type;  // 0x0002
    unsigned char src_mac2[6];
    unsigned char src_ip[4];
    unsigned char dest_mac2[6];
    unsigned char dest_ip[4];
    unsigned unused[18];
};

typedef struct iphdr_t {
    unsigned char ihl:4, version:4;
    unsigned char tos;
    unsigned short tot_len;
    unsigned short id;
    unsigned short frag_off;
    unsigned char ttl;
    unsigned char protocol;
    unsigned short check;
    unsigned int saddr;
    unsigned int daddr;
    /*The options start here. */
} iphdr;

typedef struct udphdr_t {
    unsigned short source;
    unsigned short dest;
    unsigned short len;
    unsigned short check;
} udphdr;

typedef struct udp_packet_t {
    iphdr ip;
    udphdr udp;
} udp_packet;

/*
【函数描述】
初试化设备的关于网络的所有配置如：网络Ip地址的配置，pppoe拨号服务等。
【语法】
int  NetLIb_IniNetLib(void);
【参数】
Null;
【返回值】
返回值	描述
0	成功
非0	失败。
*/
int NetLib_InitNetLib(void);


/*
【函数描述】
反初试化网络连接库。
【语法】
int  NetLIb_DeinitNetLib (void);
【参数】
Null;
【返回值】
返回值	描述
0	成功
非0	失败。
*/
int NetLIb_DeinitNetLib(void);

/*
【函数描述】
 开始一个私有协议网络服务。
【语法】
int  NetLIb_StartNetService (void);
【参数】
Null;
【返回值】
返回值	描述
0	成功
非0	失败。
*/
int NetLib_StartNetService(void);

/*
【函数描述】
停止私有协议网络服务。
【语法】
int  NetLIb_StartNetService (void);
【参数】
Null;
【返回值】
*/
int NetLib_StopNetServer(void);

#endif /* __ENC_NET_LIB_H__ */


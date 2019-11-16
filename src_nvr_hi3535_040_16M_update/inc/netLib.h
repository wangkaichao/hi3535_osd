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
#define NET_CMD_ALARM               (WM_USER + 106) // ����ͨ�����ϻ�������
#define NET_CMD_REBOOT              (WM_USER + 107)
#define NET_CMD_SETALARM            (WM_USER + 108) // ���ø澯�ṹ��
#define NET_CMD_GETALARM            (WM_USER + 109) // ��ȡ�澯�ṹ��

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
    int port_no; //�����˿�
    int net_type; //��������
    int rtspport; //rtst�˿�
    int web_port; //web �˿�
    char password[32];
    char username[32];
    int channelnum;
    char mac_addr[24];
    char ip_addr[16];
    char host_name[24]; //�豸��
    char version[24];    //�汾����
    char device_type[24];//��Ʒ�ͺ�	
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
������������
���Ի��豸�Ĺ�����������������磺����Ip��ַ�����ã�pppoe���ŷ���ȡ�
���﷨��
int  NetLIb_IniNetLib(void);
��������
Null;
������ֵ��
����ֵ	����
0	�ɹ�
��0	ʧ�ܡ�
*/
int NetLib_InitNetLib(void);


/*
������������
�����Ի��������ӿ⡣
���﷨��
int  NetLIb_DeinitNetLib (void);
��������
Null;
������ֵ��
����ֵ	����
0	�ɹ�
��0	ʧ�ܡ�
*/
int NetLIb_DeinitNetLib(void);

/*
������������
 ��ʼһ��˽��Э���������
���﷨��
int  NetLIb_StartNetService (void);
��������
Null;
������ֵ��
����ֵ	����
0	�ɹ�
��0	ʧ�ܡ�
*/
int NetLib_StartNetService(void);

/*
������������
ֹͣ˽��Э���������
���﷨��
int  NetLIb_StartNetService (void);
��������
Null;
������ֵ��
*/
int NetLib_StopNetServer(void);

#endif /* __ENC_NET_LIB_H__ */


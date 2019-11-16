#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <sys/syscall.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <errno.h>
#include <pthread.h>

#include "netLib.h"
#include "paramLib.h"
#include "commLib.h"

#define BUFSIZE (10 * 1024)

typedef struct {
  int m_cli;
} net_cli_st;

typedef struct {
  int svr;
  net_cli_st m_cli[NET_MAX_CLIENT];
} net_svr_st;

/*************mtd_flash**************************************************************/
struct erase_info_user {
	uint32_t start;
	uint32_t length;
};

struct mtd_info_user {
	uint8_t type;
	uint32_t flags;
	uint32_t size;	 // Total size of the MTD
	uint32_t erasesize;
	uint32_t writesize;
	uint32_t oobsize;   // Amount of OOB data per block (e.g. 16)
	/* The below two fields are obsolete and broken, do not use them
	 * (TODO: remove at some point) */
	uint32_t ecctype;
	uint32_t eccsize;
};

#define MEMGETINFO    _IOR('M', 1, struct mtd_info_user)
#define MEMERASE      _IOW('M', 2, struct erase_info_user)
/**********************************************************************************/
typedef struct
{
  char    filename[16];
  unsigned long filesize;
  unsigned long checksum;
  char    version[16];
  char    model[16];
  char    reserved[8];
} st_sign_t; /* 升级签名 */

static pthread_t search_thread_id = -1;

static net_svr_st st_svr;
static int netserver_run = 0;
static pthread_t netserver_id = 0;
static pthread_mutex_t netserver_mutex = PTHREAD_MUTEX_INITIALIZER;

static int StartSearchService(void);
static int StopSearchService(void);
static int StartNetServer(void);
static int StopNetServer(void);

static int net_sendArp(char *eth_name, char *ip, char *mac);
static unsigned short net_checksum(void *addr, int count);
static int net_sendBroadcast(net_msg_st *pnet_msg, int ifindex);
static void *SearchThread(void *param);

static int net_msg_snd(int sock, const net_msg_st *pmsg);
static ssize_t snd(int fd, const void *vptr, size_t n);
static int net_msg_rcv(int sock, net_msg_st *pmsg);
static ssize_t rcv(int fd, void *vptr, size_t n);
static void proc_msg(int sock, net_msg_st *pmsg);
static void *NetServerThread(void *param);
static void *RebootThread(void *param);
static int UpdateUserFlash(char *buf, int filesize);

static int StartSearchService(void)
{
    if (search_thread_id != -1)
        return 0;
        
	if (pthread_create(&search_thread_id, NULL, SearchThread, NULL) < 0) {
		printf("create search thread failed!\n");
		search_thread_id = -1;
		return -1;
	}
	
	return 0;
}

static int StopSearchService(void)
{
    if (search_thread_id == -1)
        return 0;
        
    pthread_cancel(search_thread_id);
    pthread_join(search_thread_id, NULL);
    search_thread_id = -1;
    
    return 0;
}

static int StartNetServer(void)
{
    if (netserver_run)
        return 0;
        
    netserver_run = 1;
    if (pthread_create(&netserver_id, NULL, NetServerThread, NULL) < 0) {
        logErr("create NetServerThread fail");
        return -1;
    }

    return 0;
}

static int StopNetServer(void)
{
    if (!netserver_run)
        return 0;
        
    netserver_run = 0;
    pthread_join(netserver_id, 0);
    netserver_id = 0;
    
    return 0;
}


/*********************************************
函数功能:发送arp包
eth_name:网口名称  如eth0、eth2
ip:网口的IP
mac:网口的MAC地址
**********************************************/
int net_sendArp(char *eth_name, char *ip, char *mac)
{
    int n = 0;
    int sockfd = -1;
    int index = 0;
    struct sockaddr_ll dest;
    struct arp_packet_t arp_packet;

    sockfd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (sockfd < 0)
    {
        perror("socket");
        return -1;
    }

    CommLib_getIfindex(&index);
    printf("[%s line:%d] %s index[%d]\n", __FUNCTION__, __LINE__, eth_name, index);

    int a0, a1, a2, a3, a4, a5;

    bzero(&dest,sizeof(dest));
    dest.sll_family = PF_PACKET;
    dest.sll_protocol = htons(ETH_P_ALL);

    memset(&arp_packet, 0, sizeof(struct arp_packet_t));

    //MAC
    memcpy(arp_packet.dest_mac, "\xff\xff\xff\xff\xff\xff", 6);
    sscanf(mac, "%x:%x:%x:%x:%x:%x", &a0, &a1, &a2, &a3, &a4, &a5);
    arp_packet.src_mac[0] = a0; arp_packet.src_mac[1] = a1;
    arp_packet.src_mac[2] = a2; arp_packet.src_mac[3] = a3;
    arp_packet.src_mac[4] = a4; arp_packet.src_mac[5] = a5;
    memcpy(arp_packet.src_mac2, arp_packet.src_mac, 6);

    //IP
    memcpy(arp_packet.dest_ip, "\x00\x00\x00\x00", 4);
    memcpy(arp_packet.src_ip, ip, 4);

    //index
    dest.sll_ifindex = index;

    //type
    arp_packet.hardware_type = htons(0x0001);
    arp_packet.protocol_type = htons(0x0800);
    arp_packet.arp_type = htons(0x0806);
    arp_packet.contorl_type = htons(0x0002);
    memset(arp_packet.unused, 0, sizeof(arp_packet.unused));

    //length
    arp_packet.src_mac_length = 0x06;
    arp_packet.src_ip_length = 0x04;

    //send
    n = sendto(sockfd, &arp_packet, 42, 0, (struct sockaddr *)&dest, sizeof(dest));
    if (n < 0) {
        perror("sendto");
        if (sockfd > 0) {
            close(sockfd);
            sockfd = -1;
        }
        return -1;
    }

    return 0;
}

static unsigned short net_checksum(void *addr, int count)
{
	/* Compute Internet Checksum for "count" bytes
	* *         beginning at location "addr".
	* */
	register int sum = 0;
	unsigned short *source = (unsigned short *) addr;
	while (count > 1)  
	{
		/* This is the inner loop */
		sum += *source++;
		count -= 2;
	}
	/* Add left-over byte, if any */
	if (count > 0) 
	{
		/* Make sure that the left-over byte is added correctly both
		* * with little and big endian hosts */
		unsigned short tmp = 0;
		*(unsigned char *) (&tmp) = * (unsigned char *) source;
		sum += tmp;
	}
	/* Fold 32-bit sum to 16 bits */
	while (sum >> 16)
		sum = (sum & 0xffff) + (sum >> 16);
	return ~sum;
}

static int net_sendBroadcast(net_msg_st *pnet_msg, int ifindex)
{
    struct sockaddr_ll dest;
    int fd;
    udp_packet packet;
    int result;
    char buffer[2*1024];
    int size = 0;

    memset(&packet, 0, sizeof(packet));
    memset(buffer, 0, sizeof(buffer));

    fd = socket(PF_PACKET, SOCK_DGRAM, htons(ETH_P_IP));
    memset(&dest, 0, sizeof(dest));
    dest.sll_family = AF_PACKET;
    dest.sll_protocol = htons(ETH_P_IP);
    dest.sll_ifindex = ifindex;
    dest.sll_halen = 6;
    memcpy(dest.sll_addr, "\xff\xff\xff\xff\xff\xff", 6);
    if (bind(fd, (struct sockaddr *)&dest, sizeof(struct sockaddr_ll)) < 0) 
    {
        printf("bind fail\n");
        close(fd);
        return -1;
    }
    
    packet.udp.source = htons(SEARCH_RCV_PORT + 2); /*htons(source_port);*/
    packet.udp.dest = htons(SEARCH_SND_PORT); /*htons(dest_port);*/
    packet.udp.len = htons(sizeof(packet.udp) + sizeof(pnet_msg->m_head) + pnet_msg->m_head.m_size);
    
    //packet.ip.tot_len = packet.udp.len;

    size = sizeof(packet.ip);
    
    memcpy(&buffer[size], &packet.udp, sizeof(packet.udp));
    size += sizeof(packet.udp);
    memcpy(&buffer[size], &pnet_msg->m_head, sizeof(pnet_msg->m_head));
    size += sizeof(pnet_msg->m_head);
    memcpy(&buffer[size], &pnet_msg->m_data, pnet_msg->m_head.m_size);
    size += pnet_msg->m_head.m_size;
    packet.udp.check = net_checksum(&buffer, size);

    packet.ip.protocol = IPPROTO_UDP;
    packet.ip.saddr = INADDR_ANY; /*source_ip;*/
    packet.ip.daddr = INADDR_BROADCAST; /*dest_ip;*/
    packet.ip.tot_len = htons(sizeof(udp_packet) + sizeof(pnet_msg->m_head) + pnet_msg->m_head.m_size);
    packet.ip.ihl = sizeof(packet.ip) >> 2;
    packet.ip.version = 4; /*IPVERSION;*/
    packet.ip.ttl = 64; /*IPDEFTTL;*/
    packet.ip.check = net_checksum(&(packet.ip), sizeof(packet.ip));

    memcpy(buffer, &packet.ip, sizeof(packet.ip));

    result = sendto(fd, buffer, size, 0, (struct sockaddr *) &dest, sizeof(dest));
    if (result == size)
    {
        printf("Search Thread send data.\n");
    }
    else 
    {
        perror("Search Thread send data error!\n");
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}

static void *SearchThread(void *param)
{
    int search_sock;
    net_msg_st net_msg;
    msg_broadcast_st msg_broadcast;
    struct sockaddr_in servaddr_s, clientaddr;
    socklen_t clientaddr_len = sizeof(clientaddr);
    network_config_t network_config;
    char buffer[32];
    int ifindex = 0;

    ParamLib_GetNetWorkInfo(&network_config);
    
    if ((search_sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        printf("err: create serchthread fail\n");
        return NULL;
    }

    int sock_flag = 1;
    if (setsockopt(search_sock, SOL_SOCKET, SO_REUSEADDR, &sock_flag, sizeof(int)) < 0)
        perror("setsockopt():SO_REUSEADDR");

    memset(&servaddr_s, 0, sizeof(servaddr_s));
    servaddr_s.sin_family = AF_INET;
    servaddr_s.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr_s.sin_port = htons(SEARCH_RCV_PORT);
    if (bind(search_sock, (struct sockaddr *)&servaddr_s, sizeof(servaddr_s)) < 0) {
        perror("bind fail");
        return NULL;
    }

    printf("===Start Search Thread mac[%s]===\n", network_config.eth_mac);
    
    while (1) {
        recvfrom(search_sock, &net_msg, sizeof(net_msg_st), 0, (struct sockaddr*)&clientaddr, &clientaddr_len);
        printf("<- %s, type 0x%x\n", inet_ntoa(*(struct in_addr *)&clientaddr.sin_addr.s_addr), net_msg.m_head.m_type);

        switch (net_msg.m_head.m_type)
        {
        case NET_CMD_SEARCH:
        {
            printf("NET_CMD_SEARCH\n");
            memset(net_msg.m_data, 0, sizeof(net_msg.m_data));
            strcpy(msg_broadcast.mac_addr, network_config.eth_mac);

            /* broadcast ip */
            memset(buffer, 0, sizeof(buffer));
            CommLib_getIpAddr(buffer, sizeof(buffer));
            strcpy(msg_broadcast.ip_addr, buffer);

            /* broadcast port */
            network_port_config_t netport;
            memset(&netport, 0, sizeof(network_port_config_t));
            ParamLib_GetPortInfo(&netport);
            msg_broadcast.port_no = netport.netserver_port;
            msg_broadcast.web_port = netport.httppport;
            msg_broadcast.rtspport = netport.rtspport;

            /* broadcast systeminfo */
            system_Info_t systeminfo;
            memset(&systeminfo, 0, sizeof(system_Info_t));
            ParamLib_GetSystemInfo(&systeminfo);
            sprintf(msg_broadcast.version, "%s", "bakup");
            strcpy(msg_broadcast.device_type, systeminfo.device_type);

            /* broadcast sipconfig */
            sip_config_t sipconfig;
            memset(&sipconfig, 0, sizeof(sip_config_t));
            ParamLib_GetSIPInfo(&sipconfig);
            strcpy(msg_broadcast.host_name, sipconfig.sip_user);

            /* broadcast userinfo */
            user_config_t buff[20];
            ParamLib_GetUserInfo((char *)buff);
            strcpy(msg_broadcast.password, buff[0].user_passwd);
            strcpy(msg_broadcast.username, buff[0].user_name);

            /* broadcast channel num */
            sys_config_t sysconfig;
            ParamLib_GetSysConfig(&sysconfig);
            msg_broadcast.channelnum = sysconfig.channel_num;

            /* send broadcast */
            memcpy(net_msg.m_data, &msg_broadcast, sizeof(msg_broadcast_st));
            net_msg.m_head.m_size = sizeof(msg_broadcast_st);

            CommLib_getIfindex(&ifindex);
            net_sendBroadcast(&net_msg, ifindex);
            break;
        }
        default:
            break;
        }
    }
    
    return NULL;
}

/* Write "n" bytes to a descriptor. */
static ssize_t snd(int fd, const void *vptr, size_t n)
{
  size_t nleft;
  ssize_t nwritten;
  const char *ptr;

  ptr = vptr;
  nleft = n;

  while (nleft > 0) 
  {
    if ( (nwritten = write(fd, ptr, nleft)) < 0) {
      if (errno == EINTR) {
        logErr("EINTR");
        nwritten = 0;
      } else if (errno == EAGAIN) {
        logErr("EAGAIN"); // buf full
        return -1;
      } else {
        logErr("err errno(%d)", errno);
        return -1;
      }
    } else if (nwritten == 0) {
      logErr("EOF");
      break;
    }

    nleft -= nwritten;
    ptr += nwritten;
  }
  
  return (n - nleft);
}

static int net_msg_snd(int sock, const net_msg_st *pmsg)
{
  int ret = snd(sock, (const char *)pmsg, sizeof(pmsg->m_head) + pmsg->m_head.m_size);

  if (ret != sizeof(pmsg->m_head) + pmsg->m_head.m_size) {
    logErr("ret(%d)", ret);
    return -1;
  }

  return 0;
}

static ssize_t rcv(int fd, void *vptr, size_t n)
{
  size_t nleft;
  ssize_t nread;
  char *ptr;
  int count = 0;
  
  ptr = vptr;
  nleft = n;
  while (nleft > 0) 
  {
    if ( (nread = read(fd, ptr, nleft)) < 0) {
      if (errno == EINTR) {
        logErr("EINTR");
        nread = 0;
      } else if (errno == EAGAIN) {
      	count++;
				usleep(10*1000);
				if (count >= 50) {
				  logDbg("===========EAGAIN==========");
				  return -1;
				}
				continue;
      } else {
        logErr("err errno(%d), nread(%d)", errno, nread);
        return -1;
      }
    } else if (nread == 0) {
      logErr("EOF");
      break;
    }

    count = 0;
    nleft -= nread;
    ptr += nread;
  }

  return (n - nleft);
}

static int net_msg_rcv(int sock, net_msg_st *pmsg)
{
  int ret = 0;

  memset(pmsg, 0, sizeof(net_msg_st));

  ret = rcv(sock, (char *)&pmsg->m_head, sizeof(pmsg->m_head));
  if (ret <= 0) {
    logErr("demo err: rcv=%d", ret);
    return -1;
  }

  if (pmsg->m_head.m_id != NET_ID)
    return -1;

  if (pmsg->m_head.m_size < 0 || pmsg->m_head.m_size > NET_DATA_SIZE) {
    logErr("err: msg_size=%d", pmsg->m_head.m_size);
    return -1;
  }

  if (pmsg->m_head.m_size == 0)
    return 0;

  ret = rcv(sock, (char *)pmsg->m_data, pmsg->m_head.m_size);
  if (ret <= 0 || ret != pmsg->m_head.m_size) {
    logErr("err: rcv=%d", ret);
    return -1;
  }

  return 0;
}

static void proc_msg(int sock, net_msg_st *pmsg)
{
#define SAVE_FILE   (0)
#if SAVE_FILE
  static FILE *fp = NULL;
#else
  static char *pbuf = NULL;
  static int idx  = 0;
#endif

  static int size = 0;
  
  int i;
  
  for (i = 0; i < NET_MAX_CLIENT; i++) {
    if (sock == st_svr.m_cli[i].m_cli)
      break;
  }

  switch (pmsg->m_head.m_type) {
    case NET_CMD_HEART:
    {
      //logDbg("[%d]NET_CMD_HEART", i);
      pmsg->m_head.m_status = 0;
      pmsg->m_head.m_size = 0;
      break;
    }
    case NET_CMD_SETNET:
    {
      logDbg("[%d]NET_CMD_SETNET", i);
      network_config_t *p = (network_config_t *)pmsg->m_data;
      network_config_t tmp;
      
      ParamLib_GetNetWorkInfo(&tmp);
      strcpy(tmp.eth_ipaddr, p->eth_ipaddr);
      strcpy(tmp.eth_subnet_mask, p->eth_subnet_mask);
      strcpy(tmp.eth_gateway, p->eth_gateway);
      ParamLib_SetNetWorkInfo(tmp);

      pmsg->m_head.m_status = 0;
      pmsg->m_head.m_size = 0;
      break;
    }
    case NET_CMD_UPDATE:
    {
      logDbg("[%d]NET_CMD_UPDATE", i);
      int ret = 0;
      
      if (pmsg->m_head.m_status == 0) {
        memmove(&size, pmsg->m_data, sizeof(int));
      #if SAVE_FILE
        fp = fopen("update", "wb+");
        if (fp == NULL) {
          logErr("fopen err");
          break;
        }
        fwrite(pmsg->m_data + sizeof(int), pmsg->m_head.m_size - sizeof(int), 1, fp);
      #else  
        pbuf = (char *)malloc(size);
        if (pbuf == NULL) {
          logErr("malloc err");
          break;
        }
        memmove(pbuf, pmsg->m_data + sizeof(int), pmsg->m_head.m_size - sizeof(int));
        idx += pmsg->m_head.m_size - sizeof(int);
      #endif
      }
      else {
      #if SAVE_FILE
        if (fp != NULL) {
          fwrite(pmsg->m_data, pmsg->m_head.m_size, 1, fp);
        }
      #else
        if (pbuf != NULL) {
          memmove(pbuf + idx, pmsg->m_data, pmsg->m_head.m_size);
          idx += pmsg->m_head.m_size;
        }
      #endif
      }

      size -= pmsg->m_head.m_size;
      
      if (size == 0) {
      #if SAVE_FILE
        if (fp != NULL) {
          fflush(fp);
          fclose(fp);
          fp = NULL;
        }
      #else
        if (pbuf != NULL) {
          st_sign_t sign;	
          memcpy(&sign, (pbuf + idx - sizeof(st_sign_t)), sizeof(st_sign_t));
          if (sign.filesize + sizeof(st_sign_t) != idx) {
            logErr("rcv update file err!");
          } 
          else if ((strstr(sign.filename, "rootfs.cramfs") != NULL) || (strstr(sign.filename, "rootfs.jffs2") != NULL)) {
            system_Info_t systeminfo;
            
            ParamLib_GetSystemInfo(&systeminfo);
            logDbg("ProductHardVersion:%s, sign.version:%s", systeminfo.ProductHardVersion, sign.version);
            if (strcmp(sign.version, systeminfo.ProductHardVersion) != 0) {
              logErr("version err!");
            }
            else {
              // 升级擦写flash
              ret = UpdateUserFlash(pbuf, sign.filesize);
            }
          }
          
          free(pbuf);
          pbuf = NULL;
          idx = 0;
        }
      #endif
      }
      
      pmsg->m_head.m_status = ret;
      pmsg->m_head.m_size = 0;
      break;
    }
    case NET_CMD_ALARM:
    {
      logDbg("[%d]NET_CMD_ALARM", i);
      int num;
      memmove(&num, pmsg->m_data, sizeof(int));
      
      char *alarm = (char *)malloc(sizeof(char) * num);
      if (alarm == NULL)
        logErr("malloc err");
      memmove(alarm, pmsg->m_data + sizeof(int), num);
      int i;
      printf("[%d]", num);
      for (i = 0; i < num; i++)
        printf("%d", alarm[i]);
      printf("\n");
      
      free(alarm);

      pmsg->m_head.m_status = 0;
      pmsg->m_head.m_size = 0;
      break;
    }
    case NET_CMD_REBOOT:
    {
      logDbg("[%d]NET_CMD_REBOOT", i);
      
      pthread_t reboot_thread_id;
	    if (pthread_create(&reboot_thread_id, NULL, RebootThread, NULL) < 0) {
        logErr("create RebootThread thread failed!\n");
      }
      
      pmsg->m_head.m_status = 0;
      pmsg->m_head.m_size = 0;
      break;
    }
    case NET_CMD_SETALARM:
    {
      logDbg("[%d]NET_CMD_SETALARM", i);
      ParamLib_SetAlarmInfo((alarm_config_t *)pmsg->m_data);
      pmsg->m_head.m_status = 0;
      pmsg->m_head.m_size = 0;
      break;
    }
    case NET_CMD_GETALARM:
    {
      logDbg("[%d]NET_CMD_GETALARM", i);
      ParamLib_GetAlarmInfo((alarm_config_t *)pmsg->m_data);
      pmsg->m_head.m_status = 0;
      pmsg->m_head.m_size = sizeof(alarm_config_t);
      break;
    }
    default:
    {
      logDbg("default");
      break;
    }
  }

  net_msg_snd(sock, pmsg);
}

static void *NetServerThread(void *param)
{
    logDbg("~~~~~~~~~\n");
    int i;

    struct sigaction sa;
    sa.sa_handler = SIG_IGN;//设定接受到指定信号后的动作为忽略
    sa.sa_flags = 0;
    //初始化信号集为空，屏蔽SIGPIPE信号
    if (sigemptyset(&sa.sa_mask) == -1 || sigaction(SIGPIPE, &sa, 0) == -1) {   
        printf("[%s:%d]failed to ignore SIGPIPE\n", __FUNCTION__, __LINE__);
        return NULL;
    }

    int svr = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    if (setsockopt(svr, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt)) == -1) {
        logErr("setsockopt(SO_REUSEADDR) err");
        close(svr);
        return NULL;
    }

    int flag = 1;
    if (setsockopt(svr, IPPROTO_TCP, 0x0001, (char *)&flag, sizeof(flag)) < 0) {
        logErr("setsockopt(IPPROTO_TCP) err");
        close(svr);
        return NULL;
    }

    struct sockaddr_in server;
    bzero(&server, sizeof(server));   
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(SERVER_PORT);

    if (bind(svr, (struct sockaddr *)&server, sizeof(server)) < 0) {
        logErr("bind err");
        close(svr);
        return NULL;
    }

    if (listen(svr, LISTEN_NUM) < 0) {
        logErr("listen");
        close(svr);
        return NULL;
    }

    struct epoll_event ev, events[MAX_EVENT];
    int epfd = epoll_create(MAX_EVENT); printf("epoll_create(1024) -> %d\n", epfd);
    ev.data.fd = svr;
    ev.events = EPOLLIN;
    epoll_ctl(epfd, EPOLL_CTL_ADD, svr, &ev);

    st_svr.svr = svr;

  while (netserver_run) {
    int nfds = epoll_wait(epfd, events, MAX_EVENT, -1);
    for (i = 0; i < nfds; ++i) {
      if (events[i].data.fd == svr) {
        struct sockaddr_in client_sockaddr;
        socklen_t len = (socklen_t)sizeof(client_sockaddr);
        int connfd = accept(svr, (struct sockaddr *)&client_sockaddr, &len);
        if (connfd < 0)
          continue;

        logDbg("client: %s\n", inet_ntoa(client_sockaddr.sin_addr));
        int size = 512*1024;
        if (setsockopt(connfd, SOL_SOCKET, SO_SNDBUF, (const char*)&size, sizeof(int)) != 0)
          logErr("setsockopt(SO_SNDBUF)~~~~~2\n");

        if (setsockopt(connfd, SOL_SOCKET, SO_RCVBUF, (const char*)&size, sizeof(int)) != 0)
          logErr("setsockopt(SO_RCVBUF)~~~~~2\n");
		  
        struct linger so_linger;
        so_linger.l_onoff = 1;
        so_linger.l_linger = 0;
        setsockopt(connfd, SOL_SOCKET,SO_LINGER, (char *)&so_linger, sizeof(so_linger));
        
        int bNodelay = 1;   
        setsockopt(connfd, IPPROTO_TCP, 1, (const  char*)&bNodelay, sizeof(int));
        
    	int opts = fcntl(connfd, F_GETFL);
    	if (opts < 0)
    		perror("fcntl(sock,GETFL)");
    	opts |= O_NONBLOCK;
    	if (fcntl(connfd, F_SETFL, opts) < 0)
    		perror("fcntl(sock,SETFL,opts)");
    		
        //epoll
        ev.data.fd = connfd;
        ev.events = EPOLLIN;
        epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &ev);
        
        int j;
        pthread_mutex_lock(&netserver_mutex);
        for (j = 0; j < NET_MAX_CLIENT; j++) {
          if (!st_svr.m_cli[j].m_cli) {
            st_svr.m_cli[j].m_cli = connfd;
            break;
          }
        }
        pthread_mutex_unlock(&netserver_mutex);

        if (j > NET_MAX_CLIENT) {
          logDbg("sock num > %d", NET_MAX_CLIENT);
          close(connfd);
        }

      } else if (events[i].events & EPOLLIN) {
        int sockfd = events[i].data.fd;
        if (sockfd < 0)
          continue;

        /* hisiv500编译器优化问题，必须加上memset，否则数据接收出错 */
        net_msg_st msg;
        memset(&msg, 0, sizeof(msg));

        int ret = net_msg_rcv(sockfd, &msg);
        if (ret < 0) {
          epoll_ctl(epfd, EPOLL_CTL_DEL, sockfd, &ev);
          events[i].data.fd = -1;
          close(sockfd);

          int j;
          
          pthread_mutex_lock(&netserver_mutex);
          for (j = 0; j < NET_MAX_CLIENT; j++) {
            if (st_svr.m_cli[j].m_cli == sockfd) {
              memset(&st_svr.m_cli[j], 0, sizeof(net_cli_st));
              break;
            }
          }
          pthread_mutex_unlock(&netserver_mutex);
          
          continue;
        }

        pthread_mutex_lock(&netserver_mutex);
        proc_msg(sockfd, &msg);
        pthread_mutex_unlock(&netserver_mutex);
      }
    }
  }

  close(epfd);

  pthread_mutex_lock(&netserver_mutex);
  close(st_svr.svr);
  for (i = 0; i < NET_MAX_CLIENT; i++)
    if (st_svr.m_cli[i].m_cli != 0)
      close(st_svr.m_cli[i].m_cli);
  memset(&st_svr, 0, sizeof(st_svr));
  pthread_mutex_unlock(&netserver_mutex);

  return NULL;
}

static void *RebootThread(void *param)
{
    usleep(2 * 1000 * 1000);
    system("reboot");
    return NULL;
}

static int UpdateUserFlash(char *buf, int filesize)
{
	char *device = "/dev/mtd4";
	size_t size, offset, result;
	struct mtd_info_user mtd;
	struct erase_info_user erase;
	int dev_fd = -1;
	
	/* get some info about the flash device */
	if ((dev_fd = open(device, O_SYNC | O_RDWR)) < 0) {
		perror("open()");
		return -1;
	}
	
	if (ioctl (dev_fd, MEMGETINFO, &mtd) < 0) {
		perror("MEMGETINFO");
		close(dev_fd);
		return -1;
	}

	if (filesize > mtd.size) {
		fprintf(stderr, "upgrade filesize > mtd.size(%d)\n", mtd.size);
		close(dev_fd);
		return -1;
	}

	erase.start = 0;
	erase.length = mtd.size;
	/* if not, erase the whole chunk in one shot */
	if (ioctl(dev_fd, MEMERASE, &erase) < 0) {
	    perror("MEMERASE");
	    close(dev_fd);
		return -1;
	}
	
	size = filesize;
	offset = 0;
	while (size) {
	    int i = (size < BUFSIZE ? size : BUFSIZE);
	    
		result = write(dev_fd, buf + offset, i);
		if (result != i) {
			if (result < 0) {
				perror("flash write err!");
				close(dev_fd);
				return -1;
			}
			printf("should wirte[%d], but write[%d]\n", i, result);
		}

		offset += result;
		size -= result;
		printf("written = %d\n", offset);
	}	
	
    if (close(dev_fd) < 0) {
		perror("close()");
		return -1;
	}
	
	return 0;
}



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
int NetLib_InitNetLib( void )
{
    char macbuf[64];
    memset(macbuf, 0, 64);

    network_config_t temp;
    ParamLib_GetNetWorkInfo(&temp);
    printf("eth0 ip[%s] submask[%s] gateway[%s] mac[%s] dhcp[%d]\n", 
        temp.eth_ipaddr, temp.eth_subnet_mask, temp.eth_gateway, temp.eth_mac, temp.eth_ipmode);

    CommLib_getMacAddr(macbuf, sizeof(macbuf));
    if (strcmp(temp.eth_mac, macbuf) != 0) {
        system("ifconfig eth0 down");
        sprintf(macbuf, "ifconfig eth0 hw ether %s", temp.eth_mac);
        system(macbuf);
        system("ifconfig eth0 up");
    }

    CommLib_setIpAddr(temp.eth_ipaddr);
    CommLib_setNetMask(temp.eth_subnet_mask);
    CommLib_setGateWay(temp.eth_gateway);
    CommLib_setDns(temp.dns_server1, temp.dns_server2);
    net_sendArp("eth0", temp.eth_ipaddr, temp.eth_mac);
    
    return 0;
}

/*
【函数描述】
反初试化网络连接库。
【语法】
int  NetLIb_deinitNetLib (void);
【参数】
Null;
【返回值】
返回值	描述
0	成功
非0	失败。
*/
int NetLIb_DeinitNetLib( void )
{
   return 0;
}

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

int NetLib_StartNetService(void)
{
    StartSearchService();
    StartNetServer();
    return 0;
}

int NetLib_StopNetServer(void)
{
    StopSearchService();
    StopNetServer();
    return 0;
}


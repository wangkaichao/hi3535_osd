#include "commLib.h"

#define NETWORK_FILE_EXIST 0

#define COMM_ADDRSIZE 20
#define COMM_FILELINE 256
#define COMM_ETHFILE "/etc/sysconfig/network-scripts/ifcfg-eth1"
#define COMM_ETHNAME "eth0"
#define COMM_PPPNAME "ppp0"
#define COMM_GATEWAY "/etc/sysconfig/network"
#define COMM_RESOLVCONF "/etc/resolv.conf"
#define PROC_ROUTE "/proc/net/route"

#define ETHTOOL_GLINK 0x0000000a /* Get link status (ethtool_value) */
#define SIOCETHTOOL 0x8946
typedef unsigned long long __u64;
typedef __uint32_t __u32;
typedef __uint16_t __u16;
typedef __uint8_t __u8;

struct ethtool_cmd {
	__u32 cmd;
	__u32 supported; /* Features this interface supports */
	__u32 advertising; /* Features this interface advertises */
	__u16 speed; /* The forced speed, 10Mb, 100Mb, gigabit */
	__u8 duplex; /* Duplex, half or full */
	__u8 port; /* Which connector port */
	__u8 phy_address;
	__u8 transceiver; /* Which transceiver to use */
	__u8 autoneg; /* Enable or disable autonegotiation */
	__u32 maxtxpkt; /* Tx pkts before generating tx int */
	__u32 maxrxpkt; /* Rx pkts before generating rx int */
	__u32 reserved[4];
};

/* for passing single values */
struct ethtool_value {
	__u32 cmd;
	__u32 data;
};

//根据指定的网卡名称，获取对应的ifindex
int CommLib_getIfindex(int *ifindex)
{
	int fd;
	/*char buffer[COMM_ADDRSIZE];*/
	struct ifreq ifr;

	if ((fd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) >= 0) {
		ifr.ifr_addr.sa_family = AF_INET;
		ifr.ifr_name[IFNAMSIZ - 1] = '\0';
		strncpy(ifr.ifr_name, COMM_ETHNAME, IFNAMSIZ);
		if (ioctl(fd, SIOCGIFINDEX, &ifr) == 0) {
			*ifindex = ifr.ifr_ifindex;
		}
		else
		{
			close(fd);
			return -2;
		}
	}
	else
	{
		close(fd);
		return -1;
	}

	close(fd);
	return 0;
}

//获取指定的网卡名称，获取网卡的mac地址
int CommLib_getMacAddr(char *macaddr, size_t len)
{
	int fd;
	char buffer[COMM_ADDRSIZE];
	struct ifreq ifr;

	if ((fd = socket (AF_INET, SOCK_DGRAM, 0)) >= 0)
	{
		strncpy(ifr.ifr_name, COMM_ETHNAME, IFNAMSIZ);
		ifr.ifr_name[IFNAMSIZ - 1] = '\0';
		if (ioctl(fd, SIOCGIFHWADDR, &ifr) == 0)
		{
			snprintf(buffer, COMM_ADDRSIZE, "%02x:%02x:%02x:%02x:%02x:%02x",
				(unsigned char)ifr.ifr_hwaddr.sa_data[0],
				(unsigned char)ifr.ifr_hwaddr.sa_data[1],
				(unsigned char)ifr.ifr_hwaddr.sa_data[2],
				(unsigned char)ifr.ifr_hwaddr.sa_data[3],
				(unsigned char)ifr.ifr_hwaddr.sa_data[4],
				(unsigned char)ifr.ifr_hwaddr.sa_data[5]);
		}
		else
		{
			close(fd);
			CommLib_setErrorNum(-100);
			return(-1);
		}
	}
	else
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}

	if (strlen(buffer) > len-1)
	{
		CommLib_setErrorNum(-101);
		return(-1);
	}
	strncpy(macaddr, buffer, len);

	close(fd);
	return(0);
}

//指定ppp的名称，获取ip
int CommLib_getPPPIpAddr(char *ipaddr, size_t len)
{
	int fd;
	char buffer[COMM_ADDRSIZE];
	struct ifreq ifr;
	struct sockaddr_in *addr;

	if ((fd = socket (AF_INET, SOCK_DGRAM, 0)) >= 0)
	{
		strncpy(ifr.ifr_name, COMM_PPPNAME, IFNAMSIZ);
		ifr.ifr_name[IFNAMSIZ - 1] = '\0';
		if (ioctl(fd, SIOCGIFADDR, &ifr) == 0)
		{
			addr = (struct sockaddr_in *)&(ifr.ifr_addr);
			inet_ntop(AF_INET, &addr->sin_addr, buffer, 20);
		}
		else
		{
			close(fd);
			CommLib_setErrorNum(-100);
			return(-1);
		}
	}
	else
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}

	if (strlen(buffer) > len-1)
	{
		CommLib_setErrorNum(-101);
		return(-1);
	}
	strncpy(ipaddr, buffer, len);

	close(fd);
	return(0);
}

//获取ra0的ip地址
int CommLib_getWifiIpAddr(char *ipaddr, size_t len)
{
	int fd;
	char buffer[COMM_ADDRSIZE];
	struct ifreq ifr;
	struct sockaddr_in *addr;

	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) >= 0)
	{
		strncpy(ifr.ifr_name, "ra0", IFNAMSIZ);
		ifr.ifr_name[IFNAMSIZ - 1] = '\0';
		//printf("----------------%s-------IFNAMSIZ:%d-------------\n",ifr.ifr_name,IFNAMSIZ);
		if (ioctl(fd, SIOCGIFADDR, &ifr) == 0)
		{
			addr = (struct sockaddr_in *)&(ifr.ifr_addr);
			inet_ntop(AF_INET, &addr->sin_addr, buffer, 20);
		}
		else
		{
			close(fd);
			CommLib_setErrorNum(-100);
			return(-1);
		}
	}
	else
	{
		CommLib_setErrorNum(-100);
		perror("===========CommLib_getIpAddr error :");
		return(-1);
	}

	if (strlen(buffer) > len-1)
	{
		CommLib_setErrorNum(-101);
		return(-1);
	}
	strncpy(ipaddr, buffer, len);
	close(fd);
	return(0);
}

//获取eth0的ip地址
int CommLib_getIpAddr(char *ipaddr, size_t len)
{
	int fd;
	char buffer[COMM_ADDRSIZE];
	struct ifreq ifr;
	struct sockaddr_in *addr;

	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) >= 0)
	{
		strncpy(ifr.ifr_name, COMM_ETHNAME, IFNAMSIZ);
		ifr.ifr_name[IFNAMSIZ - 1] = '\0';
		if (ioctl(fd, SIOCGIFADDR, &ifr) == 0)
		{
			addr = (struct sockaddr_in *)&(ifr.ifr_addr);
			inet_ntop(AF_INET, &addr->sin_addr, buffer, 20);
		}
		else
		{
			close(fd);
			CommLib_setErrorNum(-100);
			return -1;
		}
	}
	else
	{
		CommLib_setErrorNum(-100);
		perror("CommLib_getIpAddr error :");
		return -1;
	}

	if (strlen(buffer) > len - 1)
	{
		CommLib_setErrorNum(-101);
		return -1;
	}

	strncpy(ipaddr, buffer, strlen(buffer));
	close(fd);
	
	return 0;
}

//获取eth0的子网掩码
int CommLib_getNetMask(char *netmask, size_t len)
{
	int fd;
	char buffer[COMM_ADDRSIZE];
	struct ifreq ifr;
	struct sockaddr_in *addr;

	if ((fd = socket (AF_INET, SOCK_DGRAM, 0)) >= 0)
	{
		strncpy(ifr.ifr_name, COMM_ETHNAME, IFNAMSIZ);
		ifr.ifr_name[IFNAMSIZ - 1] = '\0';
		if (ioctl(fd, SIOCGIFNETMASK, &ifr) == 0)
		{
			addr = (struct sockaddr_in *)&(ifr.ifr_addr);
			inet_ntop(AF_INET, &addr->sin_addr, buffer, 20);
		}
		else
		{
			close(fd);
			CommLib_setErrorNum(-100);
			return(-1);
		}
	}
	else
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}

	if (strlen(buffer) > len-1)
	{
		CommLib_setErrorNum(-101);
		return(-1);
	}
	strncpy(netmask, buffer, len);

	close(fd);
	return(0);
}

//通过netmask&ip后的网络地址
int CommLib_getNetAddr(char *netaddr, size_t len)
{
	int m1, m2, m3, m4, i1, i2, i3, i4;
	char ipbuf[COMM_ADDRSIZE], maskbuf[COMM_ADDRSIZE];

	if (CommLib_getIpAddr(ipbuf, COMM_ADDRSIZE) < 0)
		return(-1);
	if (CommLib_getNetMask(maskbuf, COMM_ADDRSIZE) < 0)
		return(-1);

	sscanf(maskbuf, "%d.%d.%d.%d", &m1, &m2, &m3, &m4);
	sscanf(ipbuf, "%d.%d.%d.%d", &i1, &i2, &i3, &i4);
	snprintf(netaddr, len, "%d.%d.%d.%d", m1&i1, m2&i2, m3&i3, m4&i4);

	return(0);	
}

//获取eth0广播地址
int CommLib_getBroadCast(char *broadcast, size_t len)
{
	int fd;
	char buffer[COMM_ADDRSIZE];
	struct ifreq ifr;
	struct sockaddr_in *addr;

	if ((fd = socket (AF_INET, SOCK_DGRAM, 0)) >= 0)
	{
		strncpy(ifr.ifr_name, COMM_ETHNAME, IFNAMSIZ);
		ifr.ifr_name[IFNAMSIZ - 1] = '\0';
		if (ioctl(fd, SIOCGIFBRDADDR, &ifr) == 0)
		{
			addr = (struct sockaddr_in *)&(ifr.ifr_addr);
			inet_ntop(AF_INET, &addr->sin_addr, buffer, 20);
		}
		else
		{
			close(fd);
			CommLib_setErrorNum(-100);
			return(-1);
		}
	}
	else
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}

	if (strlen(buffer) > len-1)
	{
		CommLib_setErrorNum(-101);
		return(-1);
	}
	strncpy(broadcast, buffer, len);

	close(fd);
	return(0);
}

#define CH_TO_HEX(ch) ((ch)<='9'?(ch)-48:(ch)-'A'+10)


//获取eth0网关
int CommLib_getGateWay(char *gateway, size_t len)
{

	int i;
	int n[4];
	int fd;
	char name[10], dest[10], dg[10], buf[1024];

	memset(gateway, '\0', len);
	if ((fd = open(PROC_ROUTE, O_RDONLY)) < 0)
	{
		perror("open route");
		return(-1);
	}
	while (readline(fd, buf, 1024) > 0)
	{
		if (memcmp(buf, "eth0", 4) != 0)
			continue;
		sscanf(buf, "%4s\t%8s\t%8s\t", name, dest, dg);
		if (!strcmp(dest, "00000000"))
		{
			for (i = 0; i < 8; i = i+2)
			{
				n[i/2] = CH_TO_HEX(dg[i])*16+CH_TO_HEX(dg[i+1]);
			}
			snprintf(gateway, len, "%d.%d.%d.%d", n[3], n[2], n[1], n[0]);
			break;
		}
	}
	close(fd);

	if (gateway[0] == '\0')
		return(-1);

	return(0);
}

int CommLib_getGateWay_ex(char *gateway, size_t len)
{
	int i;
	int n[4];
	int fd;
	char name[10], dest[10], dg[10], buf[1024];
	int dev_type=0;

	memset(gateway, '\0', len);
	if ((fd = open(PROC_ROUTE, O_RDONLY)) < 0)
	{
		perror("open route");
		return(-1);
	}
	while (readline(fd, buf, 1024) > 0)
	{
		dev_type = -1;
		sscanf(buf, "%4s\t%8s\t%8s\t", name, dest, dg);
		if (!strcmp(dest, "00000000"))
		{
			for (i = 0; i < 8; i = i+2)
			{
				n[i/2] = CH_TO_HEX(dg[i])*16+CH_TO_HEX(dg[i+1]);
			}
			snprintf(gateway, len, "%d.%d.%d.%d", n[3], n[2], n[1], n[0]);
			if (memcmp(buf, "eth0", 4) == 0)
				dev_type = 0; 
			else if(memcmp(buf, "eth1", 4)==0)
				dev_type = 1;
			break;
		}
	}
	close(fd);
	if (gateway[0] == '\0')
		dev_type = -1;
	return dev_type;
}

//获取DNS1 DNS2
int CommLib_getDns(char *firstdns, char *seconddns, size_t len)
{
	int flag = 0; /*isn't find the DNS?*/
	int fd;
	char buf[COMM_FILELINE], *ptr;

	if(firstdns == NULL)
		return -1;
	if((fd = open(COMM_RESOLVCONF, O_RDONLY)) < 0)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}
	while (readline(fd, buf, COMM_FILELINE) > 0)
	{
		if (strncmp(buf, "nameserver", 10) == 0)
		{
			ptr = buf;
			my_strsep(&ptr, " ");
			if (flag == 0)
			{
				strncpy(firstdns, my_strsep(&ptr, " "), len);
				if (firstdns[strlen(firstdns)-1] == '\n')
					firstdns[strlen(firstdns)-1] = '\0';
				flag = 1;
				if (seconddns == NULL)
					break;
				continue;
			}
			else
			{
				if(seconddns == NULL)
					return -1;
				strncpy(seconddns, my_strsep(&ptr, " "), len);
				if (seconddns[strlen(seconddns)-1] == '\n')
					seconddns[strlen(seconddns)-1] = '\0';
				flag = 2;
				break;
			}
		}
	}

	close(fd);
	if (flag == 0)
	{
		CommLib_setErrorNum(-106);
		return(-1);
	}

	return(flag);
}

int CommLib_setMacAddr(const char *macaddr)
{
	int i = 0;
	int fd;
	int imac[6];
	struct ifreq ifr;

	if (CommLib_checkMacAddress(macaddr) < 0)
	{
		printf("===========MAC addr format is not correct. %s",macaddr);
		//		assert(0);
		CommLib_setErrorNum(-107);
		return(-1);
	}
	if ((fd =socket(AF_INET,SOCK_DGRAM,0))< 0)
	{
		printf("===========Create socket error.");
		//		assert(0);
		CommLib_setErrorNum(-100);
		return(-1);
	}
	strncpy(ifr.ifr_name,COMM_ETHNAME,IFNAMSIZ);
	ifr.ifr_name[IFNAMSIZ - 1] = '\0';
	/*shutdown interface.*/
	ifr.ifr_flags &= ~IFF_UP;
	if (ioctl(fd, SIOCSIFFLAGS, &ifr) < 0)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}
	/*change mac address.*/
	ifr.ifr_hwaddr.sa_family = 1;
	CommLib_convertMacToInt(macaddr,imac);
	for (i = 0; i < 6; i++)
		ifr.ifr_hwaddr.sa_data[i] = (unsigned char)imac[i];
	if (ioctl(fd,SIOCSIFHWADDR,&ifr) < 0)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}
	/*up interface.*/
	ifr.ifr_flags |= IFF_UP;
	if (ioctl(fd, SIOCSIFFLAGS, &ifr) < 0)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}
	close(fd);
	return(0);
}

int CommLib_setIpAddr(const char *ipaddr)
{
	int fd;
	struct ifreq ifr;
	struct sockaddr_in addr;
	if (CommLib_checkIpAddress(ipaddr) < 0)
	{
		CommLib_setErrorNum(-108);
		return(-1);
	}
	/*set ip address, and take affect instant.*/
	if ((fd = socket (AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}
	strncpy(ifr.ifr_name, COMM_ETHNAME, IFNAMSIZ);
	ifr.ifr_name[IFNAMSIZ - 1] = '\0';
	bzero(&addr, sizeof(struct sockaddr_in));
	if (inet_pton(AF_INET, ipaddr, &addr.sin_addr) < 0)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}
	addr.sin_family = AF_INET;
	addr.sin_port = 0;
	memcpy(&ifr.ifr_addr, &addr, sizeof(struct sockaddr));
	if (ioctl(fd, SIOCSIFADDR, &ifr) < 0)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}
	close(fd);
	return(0);
}

int CommLib_setNetMask(const char *netmask)
{
	int fd;
	struct ifreq ifr;
	struct sockaddr_in addr;
	if (CommLib_checkIpAddress(netmask) < 0)
	{
		CommLib_setErrorNum(-108);
		return(-1);
	}

	/*set netmask, and take affect instant.*/
	if ((fd = socket (AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}
	strncpy(ifr.ifr_name, COMM_ETHNAME, IFNAMSIZ);
	ifr.ifr_name[IFNAMSIZ - 1] = '\0';
	bzero(&addr, sizeof(struct sockaddr_in));
	if (inet_pton(AF_INET, netmask, &addr.sin_addr) < 0)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}
	addr.sin_family = AF_INET;
	addr.sin_port = 0;
	memcpy(&ifr.ifr_addr, &addr, sizeof(struct sockaddr));
	if (ioctl(fd, SIOCSIFNETMASK, &ifr) < 0)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}
	close(fd);

	return(0);
}

int CommLib_setNetAddr(const char *netaddr)
{
	return(0);
}

int CommLib_setBroadCast(const char *broadcast)
{
	int fd;
	struct ifreq ifr;
	struct sockaddr_in addr;


	if (CommLib_checkIpAddress(broadcast) < 0)
	{
		CommLib_setErrorNum(-108);
		return(-1);
	}

	/*set broadcast, and take affect instant.*/
	if ((fd = socket (AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}
	strncpy(ifr.ifr_name, COMM_ETHNAME, IFNAMSIZ);
	ifr.ifr_name[IFNAMSIZ - 1] = '\0';
	bzero(&addr, sizeof(struct sockaddr_in));
	if (inet_pton(AF_INET, broadcast, &addr.sin_addr) < 0)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}
	addr.sin_family = AF_INET;
	addr.sin_port = 0;
	memcpy(&ifr.ifr_addr, &addr, sizeof(struct sockaddr));
	if (ioctl(fd, SIOCSIFBRDADDR, &ifr) < 0)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}
	close(fd);
	return(0);
}

int CommLib_delGateWay(const char *gateway)
{
	char cmdbuf[COMM_FILELINE] = "";
	
	if (gateway == NULL)
		return -1;

	printf("[%s]-%d: gateway[%s]\n", __FUNCTION__, __LINE__, gateway);
	FILE *fp_read = NULL;
	snprintf(cmdbuf, COMM_FILELINE, "route del default gw %s", gateway);

	fp_read = popen(cmdbuf, "r");
	if (fp_read == NULL)
	{
		printf("del gateway failure\n");
	}
	else
	{
		printf("del gateway success\n");
		pclose(fp_read);
	}

	return 0;
}

int CommLib_setGateWay(const char *gateway)
{
	char cmdbuf[COMM_FILELINE] = "";
	
	if (gateway == NULL)
		return -1;
		
	FILE * fp_read = NULL;
	snprintf(cmdbuf, COMM_FILELINE, "route add default gw %s dev eth0", gateway);
	
	fp_read = popen(cmdbuf, "r");
	if(fp_read == NULL)
	{
		printf("set gateway failure\n");
	}
	else
	{
		printf("set gateway success\n");
		pclose(fp_read);
	}

	return 0;
}

int CommLib_setDns(const char *firstdns, const char *seconddns)
{
    FILE *fp;
    if (firstdns == NULL)
        return -1;
    if ((fp = fopen(COMM_RESOLVCONF, "w")) == NULL) {
        CommLib_setErrorNum(-100);
        return -1;
    }

    fprintf(fp, "nameserver %s\n", firstdns);

    if (seconddns != NULL)
        fprintf(fp, "nameserver %s\n", seconddns);
    
    fclose(fp);
    return 0;
}

int CommLib_setAutoDns(void)
{
	unlink(COMM_RESOLVCONF);
	return 0;
}

/* 1->eth0 is linked with RJ45, 0->eth0 is not linked */
int CommLib_getLinkStatus(void)
{
	struct ethtool_value edata;
	struct ifreq ifr;
	int fd;
	int err;

	memset(&ifr, 0, sizeof(ifr));
	strcpy(ifr.ifr_name, "eth0");

	/* Open control socket. */
	fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd < 0) {
		perror("===========Cannot get control socket");
		return 70;
	}
	edata.cmd = ETHTOOL_GLINK;
	ifr.ifr_data = (caddr_t)&edata;
	err = ioctl(fd, SIOCETHTOOL, &ifr);
	if (err == 0) {
		close(fd);
		return (edata.data);
	} 
	else if (errno != EOPNOTSUPP) {
		perror("===========Cannot get link status");
	}
	close(fd);
	return 0;
}

/*some tools function, not in the commlib.*/
int CommLib_checkIpAddress(const char *ipaddr)
{
	int i = 0, flag = 0;
	char buf[16];
	char *tmp, *ptr;

	if (strlen(ipaddr) > 15)
		return(-1);
	strncpy(buf, ipaddr, 16);

	while (buf[i] != '\0')
	{
		switch (buf[i])
		{
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			break;
		case '.':
			flag++;
			break;
		default:
			return(-1);
		}
		i++;
	}
	if (flag != 3)
		return(-1);

	ptr = buf;
	for (i = 0; i < 4; i++)
	{
		tmp = my_strsep(&ptr, ".");
		if (tmp == NULL)	
			return(-1);
		if (atoi(tmp) > 255 || strlen(tmp) > 3 || (tmp[0] == '0' && strlen(tmp) != 1))
			return(-1);
	}

	return(0);
}

int CommLib_checkMacAddress(const char *macaddr)
{
	int i = 0;
	int flag = 0;

	//	assert(macaddr);
	if(macaddr == NULL)
		return -1;
	if (strlen(macaddr) != 17)
		return(-1);
	while (macaddr[i] != '\0')
	{
		switch (macaddr[i])
		{
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case 'a':
		case 'b':
		case 'c':
		case 'd':
		case 'e':
		case 'f':
		case 'A':
		case 'B':
		case 'C':
		case 'D':
		case 'E':
		case 'F':
			break;
		case ':':
			flag++;
			break;
		default:
			return(-1);
		}
		i++;
	}
	if (flag != 5)
		return(-1);
	if (macaddr[2] != ':' || macaddr[5] != ':' || macaddr[8] != ':' 
		|| macaddr[11] != ':' || macaddr[14] != ':')
		return(-1);
	return(0);
}

int CommLib_convertMacToInt(const char *macaddr, int *p)
{
	int i = 0, j = 0;
	int n1 = 0, n2 = 0;

	for (i = 0, j = 0; i < strlen(macaddr); i = i+3, j++)
	{
		if (macaddr[i] >= 48 && macaddr[i] <= 57) /*'0' <--> '9'*/
			n1 = macaddr[i] - 48; /*'0' == 48*/
		else if (macaddr[i] >= 65 && macaddr[i] <= 70) /*'A' <--> 'F'*/
			n1 = macaddr[i] - 55; /*'A' == 65, and in hex 'A' = 10.*/
		else if (macaddr[i] >= 97) /*'a' <--> 'f'*/
			n1 = macaddr[i] - 87; /*'a' == 97, and in hex 'a' = 10.*/

		if (macaddr[i+1] >= 48 && macaddr[i+1] <= 57)
			n2 = macaddr[i+1] - 48;
		else if (macaddr[i+1] >= 65 && macaddr[i+1] <= 70)
			n2 = macaddr[i+1] - 55;
		else if (macaddr[i+1] >= 97)
			n2 = macaddr[i+1] - 87;

		p[j] = n1*16 + n2;
		printf("===========p[%d]=%d\n", j, p[j]);
	}

	return(0);
}

/*---------------------------- END ------------------------------*/

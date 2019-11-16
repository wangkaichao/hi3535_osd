/*
 *Copyright :Santachi,2005
 *Name :comm_network.c
 *Summary :some function of network.
 *Date :2005.3.24
 *Author :liuzuxu
 *ChangeInfo:liuzuxu 2005.3.24 The first create.
 * liuzx 2006.6.1 add get default gw from /proc/net/route.
 * liuzuxu 2006.11.16 change st*k_ to st_.
 */

#include "commLib.h"

#define NETWORK_FILE_EXIST 0

#define COMM_ADDRSIZE 20
#define COMM_FILELINE 256
#define COMM_ETHFILE "/etc/sysconfig/network-scripts/ifcfg-eth1"
#define COMM_ETHNAME "ra0"
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

static int _CommLib_checkEth1MacAddress(const char *macaddr);
static int _CommLib_convertEth1MacToInt(const char *macaddr, int *p);
static int _CommLib_checkEth1IpAddress(const char *ipaddr);

int CommLib_getEth1Ifindex(int *ifindex)
{
    int fd;
    struct ifreq ifr;

    if ((fd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) >= 0) {
        ifr.ifr_addr.sa_family = AF_INET;
        ifr.ifr_name[IFNAMSIZ - 1] = '\0';
        strncpy(ifr.ifr_name, "ra0", IFNAMSIZ);
        if (ioctl(fd, SIOCGIFINDEX, &ifr) == 0) {
            *ifindex = ifr.ifr_ifindex;
        }
    }
    else {
        close(fd);
        return -1;
    }
    close(fd);

    return 0;
}

int CommLib_getEth1MacAddr(char *macaddr, size_t len)
{
    int fd;
    char buffer[COMM_ADDRSIZE];
    struct ifreq ifr;

    if ((fd = socket (AF_INET, SOCK_DGRAM, 0)) >= 0) {
        strncpy(ifr.ifr_name, "ra0", IFNAMSIZ);
        ifr.ifr_name[IFNAMSIZ - 1] = '\0';
        if (ioctl(fd, SIOCGIFHWADDR, &ifr) == 0) {
            snprintf(buffer, COMM_ADDRSIZE, "%02x:%02x:%02x:%02x:%02x:%02x",
                (unsigned char)ifr.ifr_hwaddr.sa_data[0],
                (unsigned char)ifr.ifr_hwaddr.sa_data[1],
                (unsigned char)ifr.ifr_hwaddr.sa_data[2],
                (unsigned char)ifr.ifr_hwaddr.sa_data[3],
                (unsigned char)ifr.ifr_hwaddr.sa_data[4],
                (unsigned char)ifr.ifr_hwaddr.sa_data[5]);
        }
        else {
            close(fd);
            CommLib_setErrorNum(-100);
            return -1;
        }
    }
    else {
        CommLib_setErrorNum(-100);
        return -1;
    }

    if (strlen(buffer) > len - 1) {
        CommLib_setErrorNum(-101);
        return -1;
    }
    strncpy(macaddr, buffer, len);

    close(fd);
    return 0;
}

int CommLib_getEth1PPPIpAddr(char *ipaddr, size_t len)
{
    int fd;
    char buffer[COMM_ADDRSIZE];
    struct ifreq ifr;
    struct sockaddr_in *addr;

    if ((fd = socket (AF_INET, SOCK_DGRAM, 0)) >= 0) {
        strncpy(ifr.ifr_name, COMM_PPPNAME, IFNAMSIZ);
        ifr.ifr_name[IFNAMSIZ - 1] = '\0';
        if (ioctl(fd, SIOCGIFADDR, &ifr) == 0) {
            addr = (struct sockaddr_in *)&(ifr.ifr_addr);
            inet_ntop(AF_INET, &addr->sin_addr, buffer, 20);
        }
        else {
            close(fd);
            CommLib_setErrorNum(-100);
            return -1;
        }
    }
    else {
        CommLib_setErrorNum(-100);
        return -1;
    }

    if (strlen(buffer) > len-1) {
        CommLib_setErrorNum(-101);
        return -1;
    }
    strncpy(ipaddr, buffer, len);

    close(fd);
    return 0;
}

int CommLib_getEth1IpAddr(char *ipaddr, size_t len)
{
    int fd;
    char buffer[COMM_ADDRSIZE];
    struct ifreq ifr;
    struct sockaddr_in *addr;

    if ((fd = socket (AF_INET, SOCK_DGRAM, 0)) >= 0) {
        strncpy(ifr.ifr_name, "ra0", IFNAMSIZ);
        ifr.ifr_name[IFNAMSIZ - 1] = '\0';
        if (ioctl(fd, SIOCGIFADDR, &ifr) == 0) {
            addr = (struct sockaddr_in *)&(ifr.ifr_addr);
            inet_ntop(AF_INET, &addr->sin_addr, buffer, 20);
        }
        else {
            close(fd);
            CommLib_setErrorNum(-100);
            return -1;
        }
    }
    else {
        CommLib_setErrorNum(-100);
        return -1;
    }

    if (strlen(buffer) > len - 1) {
        CommLib_setErrorNum(-101);
        return -1;
    }
    strncpy(ipaddr, buffer, len);

    close(fd);
    return 0;
}

int CommLib_getEth1NetMask(char *netmask, size_t len)
{
    int fd;
    char buffer[COMM_ADDRSIZE];
    struct ifreq ifr;
    struct sockaddr_in *addr;

    if ((fd = socket (AF_INET, SOCK_DGRAM, 0)) >= 0) {
        strncpy(ifr.ifr_name, "ra0", IFNAMSIZ);
        ifr.ifr_name[IFNAMSIZ - 1] = '\0';
        if (ioctl(fd, SIOCGIFNETMASK, &ifr) == 0) {
            addr = (struct sockaddr_in *)&(ifr.ifr_addr);
            inet_ntop(AF_INET, &addr->sin_addr, buffer, 20);
        }
        else {
            close(fd);
            CommLib_setErrorNum(-100);
            return -1;
        }
    }
    else {
        CommLib_setErrorNum(-100);
        return -1;
    }

    if (strlen(buffer) > len-1) {
        CommLib_setErrorNum(-101);
        return -1;
    }
    strncpy(netmask, buffer, len);

    close(fd);
    return(0);
}

int CommLib_getEth1NetAddr(char *netaddr, size_t len)
{
    int m1, m2, m3, m4, i1, i2, i3, i4;
    char ipbuf[COMM_ADDRSIZE], maskbuf[COMM_ADDRSIZE];

    if (CommLib_getEth1IpAddr(ipbuf, COMM_ADDRSIZE) < 0)
        return -1;
    if (CommLib_getEth1NetMask(maskbuf, COMM_ADDRSIZE) < 0)
        return -1;

    sscanf(maskbuf, "%d.%d.%d.%d", &m1, &m2, &m3, &m4);
    sscanf(ipbuf, "%d.%d.%d.%d", &i1, &i2, &i3, &i4);
    snprintf(netaddr, len, "%d.%d.%d.%d", m1&i1, m2&i2, m3&i3, m4&i4);

    return 0;	
}

int CommLib_getEth1BroadCast(char *broadcast, size_t len)
{
    int fd;
    char buffer[COMM_ADDRSIZE];
    struct ifreq ifr;
    struct sockaddr_in *addr;

    if ((fd = socket (AF_INET, SOCK_DGRAM, 0)) >= 0) {
        strncpy(ifr.ifr_name, "ra0", IFNAMSIZ);
        ifr.ifr_name[IFNAMSIZ - 1] = '\0';
        if (ioctl(fd, SIOCGIFBRDADDR, &ifr) == 0) {
            addr = (struct sockaddr_in *)&(ifr.ifr_addr);
            inet_ntop(AF_INET, &addr->sin_addr, buffer, 20);
        }
        else {
            close(fd);
            CommLib_setErrorNum(-100);
            return -1;
        }
    }
    else {
        CommLib_setErrorNum(-100);
        return -1;
    }

    if (strlen(buffer) > len - 1) {
        CommLib_setErrorNum(-101);
        return -1;
    }
    strncpy(broadcast, buffer, len);

    close(fd);
    return 0;
}

#define CH_TO_HEX(ch) ((ch)<='9'?(ch)-48:(ch)-'A'+10)

int CommLib_getEth1GateWay(char *gateway, size_t len)
{
#if NETWORK_FILE_EXIST
	int flag = 0; /*isn't find the GATEWAY */
	FILE *fp;
	char buf[COMM_FILELINE], *ptr;

	if((fp = fopen(COMM_GATEWAY, "r")) == NULL)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}
	while (fgets(buf, COMM_FILELINE, fp) != NULL)
	{
		if (strncmp(buf, "GATEWAY", 7) == 0)
		{
			sscanf(buf, "GATEWAY=%s", gateway);
			if (gateway[strlen(gateway)-1] == '\n')	
				gateway[strlen(gateway)-1] = '\0';
			flag = 1;
			break;
		}
	}
	
	fclose(fp);
	if (flag == 0)
	{
		CommLib_setErrorNum(-105);
		return(-1);
	}
#else
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
		if (memcmp(buf, "ra0", 3) != 0)
			continue;
		sscanf(buf, "%4s\t%8s\t%8s\t", name, dest, dg);
		if (!strcmp(dest, "00000000"))
		{
			for (i = 0; i < 8; i = i+2)
			{
				n[i/2] = CH_TO_HEX(dg[i])*16+CH_TO_HEX(dg[i+1]);
			}
#ifdef COMM_PPC
			snprintf(gateway, len, "%d.%d.%d.%d", n[0], n[1], n[2], n[3]);
#else
			snprintf(gateway, len, "%d.%d.%d.%d", n[3], n[2], n[1], n[0]);
#endif
			break;
		}
	}
	close(fd);
#endif
	if (gateway[0] == '\0')
		return(-1);

	return(0);
}

int CommLib_getEth1Dns(char *firstdns, char *seconddns, size_t len)
{
    int flag = 0; /*isn't find the DNS */
    int fd;
    char buf[COMM_FILELINE], *ptr;

    if (firstdns == NULL)
        return -1;

    if ((fd = open(COMM_RESOLVCONF, O_RDONLY)) < 0) {
        CommLib_setErrorNum(-100);
        return -1;
    }
	
    while (readline(fd, buf, COMM_FILELINE) > 0)
    {
        if (strncmp(buf, "nameserver", 10) == 0) {
            ptr = buf;
            my_strsep(&ptr, " ");
            if (flag == 0) {
                strncpy(firstdns, my_strsep(&ptr, " "), len);
                if (firstdns[strlen(firstdns)-1] == '\n')
                    firstdns[strlen(firstdns)-1] = '\0';
                    
                flag = 1;
                if (seconddns == NULL)
                    break;
                    
                continue;
            }
            else {
                if (seconddns)
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
    if (flag == 0) {
        CommLib_setErrorNum(-106);
        return -1;
    }

    return flag;
}

int CommLib_setEth1MacAddr(const char *macaddr)
{
	int i = 0;
	int fd;
	int imac[6];
	struct ifreq ifr;
	
#if NETWORK_FILE_EXIST
	int flag = 0; /*COMM_ETHFILE hasn't a line of MACADDR?*/
	FILE *fp, *fp_tmp;
	char tmpfile[COMM_FILELINE], buf[COMM_FILELINE];
#endif

	if (_CommLib_checkEth1MacAddress(macaddr) < 0)
	{
		CommLib_setErrorNum(-107);
		return(-1);
	}

	if ((fd = socket (AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}
	strncpy(ifr.ifr_name, "ra0", IFNAMSIZ);
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
	_CommLib_convertEth1MacToInt(macaddr, imac);
	for (i = 0; i < 6; i++)
		ifr.ifr_hwaddr.sa_data[i] = (unsigned char)imac[i];
        if (ioctl(fd, SIOCSIFHWADDR, &ifr) < 0)
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

#if NETWORK_FILE_EXIST
	/*change interface file, and it will take affect when reboot.*/
	if((fp = fopen(COMM_ETHFILE, "r")) == NULL)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}

	snprintf(tmpfile, COMM_FILELINE, "%s_%d", COMM_ETHFILE, getpid());
	if((fp_tmp = fopen(tmpfile, "w")) == NULL)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}

	while (fgets(buf, COMM_FILELINE, fp) != NULL)
	{
		if (strncmp(buf, "MACADDR", 6) == 0)
		{
			snprintf(buf, COMM_FILELINE, "%s=%s\n", "MACADDR", macaddr);
			flag = 1;
		}
		fputs(buf, fp_tmp);
	}
	
	if (flag == 0)
	{
		snprintf(buf, COMM_FILELINE, "%s=%s\n", "MACADDR", macaddr);
		fputs(buf, fp_tmp);
	}

	fclose(fp);
	fclose(fp_tmp);
	if (rename(tmpfile, COMM_ETHFILE) < 0)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}
#endif

	return(0);
}

int CommLib_setEth1IpAddr(const char *ipaddr)
{
	int fd;
	struct ifreq ifr;
	struct sockaddr_in addr;
	
#if NETWORK_FILE_EXIST
	FILE *fp, *fp_tmp;
	char tmpfile[COMM_FILELINE], buf[COMM_FILELINE];
#endif

	if (_CommLib_checkEth1IpAddress(ipaddr) < 0)
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
	strncpy(ifr.ifr_name, "ra0", IFNAMSIZ);
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

#if NETWORK_FILE_EXIST
	/*change interface file, and it will take affect when reboot.*/
	if((fp = fopen(COMM_ETHFILE, "r")) == NULL)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}

	snprintf(tmpfile, COMM_FILELINE, "%s_%d", COMM_ETHFILE, getpid());
	if((fp_tmp = fopen(tmpfile, "w")) == NULL)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}

	while (fgets(buf, COMM_FILELINE, fp) != NULL)
	{
		if (strncmp(buf, "IPADDR", 6) == 0)
			snprintf(buf, COMM_FILELINE, "%s=%s\n", "IPADDR", ipaddr);
		fputs(buf, fp_tmp);
	}
	
	fclose(fp);
	fclose(fp_tmp);
	if (rename(tmpfile, COMM_ETHFILE) < 0)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}
#endif
	return(0);
}

int CommLib_setEth1NetMask(const char *netmask)
{
	int fd;
	struct ifreq ifr;
	struct sockaddr_in addr;
	
#if NETWORK_FILE_EXIST
	FILE *fp, *fp_tmp;
	char tmpfile[COMM_FILELINE], buf[COMM_FILELINE];
#endif

	if (_CommLib_checkEth1IpAddress(netmask) < 0)
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
	strncpy(ifr.ifr_name, "ra0", IFNAMSIZ);
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

#if NETWORK_FILE_EXIST
	/*change interface file, and it will take affect when reboot.*/
	if((fp = fopen(COMM_ETHFILE, "r")) == NULL)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}

	snprintf(tmpfile, COMM_FILELINE, "%s_%d", COMM_ETHFILE, getpid());
	if((fp_tmp = fopen(tmpfile, "w")) == NULL)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}

	while (fgets(buf, COMM_FILELINE, fp) != NULL)
	{
		if (strncmp(buf, "NETMASK", 6) == 0)
			snprintf(buf, COMM_FILELINE, "%s=%s\n", "NETMASK", netmask);
		fputs(buf, fp_tmp);
	}
	
	fclose(fp);
	fclose(fp_tmp);
	if (rename(tmpfile, COMM_ETHFILE) < 0)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}
#endif

	return(0);
}

int CommLib_setEth1NetAddr(const char *netaddr)
{
#if NETWORK_FILE_EXIST
	int flag = 0; /*COMM_ETHFILE hasn't a line of NETADDR?*/
	FILE *fp, *fp_tmp;
	char tmpfile[COMM_FILELINE], buf[COMM_FILELINE];

	if((fp = fopen(COMM_ETHFILE, "r")) == NULL)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}

	snprintf(tmpfile, COMM_FILELINE, "%s_%d", COMM_ETHFILE, getpid());
	if((fp_tmp = fopen(tmpfile, "w")) == NULL)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}

	while (fgets(buf, COMM_FILELINE, fp) != NULL)
	{
		if (strncmp(buf, "NETADDR", 6) == 0)
		{
			snprintf(buf, COMM_FILELINE, "%s=%s\n", "NETADDR", netaddr);
			flag = 1;
		}
		fputs(buf, fp_tmp);
	}
	
	if (flag == 0)
	{
		snprintf(buf, COMM_FILELINE, "%s=%s\n", "NETADDR", netaddr);
		fputs(buf, fp_tmp);
	}

	fclose(fp);
	fclose(fp_tmp);
	if (rename(tmpfile, COMM_ETHFILE) < 0)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}
#endif

	return(0);
}

int CommLib_setEth1BroadCast(const char *broadcast)
{
	int fd;
	struct ifreq ifr;
	struct sockaddr_in addr;
	
#if NETWORK_FILE_EXIST
	int flag = 0; /*COMM_ETHFILE hasn't a line of BROADCAST?*/
	FILE *fp, *fp_tmp;
	char tmpfile[COMM_FILELINE], buf[COMM_FILELINE];
#endif

	if (_CommLib_checkEth1IpAddress(broadcast) < 0)
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
	strncpy(ifr.ifr_name, "ra0", IFNAMSIZ);
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

#if NETWORK_FILE_EXIST
	/*change interface file, and it will take affect when reboot.*/
	if((fp = fopen(COMM_ETHFILE, "r")) == NULL)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}

	snprintf(tmpfile, COMM_FILELINE, "%s_%d", COMM_ETHFILE, getpid());
	if((fp_tmp = fopen(tmpfile, "w")) == NULL)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}

	while (fgets(buf, COMM_FILELINE, fp) != NULL)
	{
		if (strncmp(buf, "BROADCAST", 6) == 0)
		{
			snprintf(buf, COMM_FILELINE, "%s=%s\n", "BROADCAST", broadcast);
			flag = 1;
		}
		fputs(buf, fp_tmp);
	}
	
	if (flag == 0)
	{
		snprintf(buf, COMM_FILELINE, "%s=%s\n", "BROADCAST", broadcast);
		fputs(buf, fp_tmp);
	}

	fclose(fp);
	fclose(fp_tmp);
	if (rename(tmpfile, COMM_ETHFILE) < 0)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}
#endif

	return(0);
}

int CommLib_delEth1GateWay(const char *gateway)
{
	char cmdbuf[COMM_FILELINE];
	FILE *fp_read;
	snprintf(cmdbuf, COMM_FILELINE, "route del default gw %s", gateway);
	printf("cmdbuf: %s\n",cmdbuf);
//	system(cmdbuf);
	fp_read = popen(cmdbuf, "r");
	if(fp_read == NULL)
	{
		printf("============>%s failure\n",cmdbuf);
		pclose(fp_read);
	}
	else
	{
		printf("============>%s  success\n",cmdbuf);
		pclose(fp_read);
	}
	return(0);
}

int CommLib_setEth1GateWay(const char *gateway)
{
#if NETWORK_FILE_EXIST
	FILE *fp, *fp_tmp;
	char tmpfile[COMM_FILELINE], buf[COMM_FILELINE];
#endif
	char cmdbuf[COMM_FILELINE];

	if (gateway == NULL)
		return -1;

#if NETWORK_FILE_EXIST
	if((fp = fopen(COMM_GATEWAY, "r")) == NULL)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}

	snprintf(tmpfile, COMM_FILELINE, "%s_%d", COMM_GATEWAY, getpid());
	if((fp_tmp = fopen(tmpfile, "w")) == NULL)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}

	while (fgets(buf, COMM_FILELINE, fp) != NULL)
	{
		if (strncmp(buf, "GATEWAY", 7) == 0)
			snprintf(buf, COMM_FILELINE, "%s=%s\n", "GATEWAY", gateway);
		fputs(buf, fp_tmp);
	}
	
	fclose(fp);
	fclose(fp_tmp);
	if (rename(tmpfile, COMM_GATEWAY) < 0)
	{
		CommLib_setErrorNum(-100);
		return(-1);
	}
#endif

	snprintf(cmdbuf, COMM_FILELINE, "route add default gw %s dev ra0", gateway);
	printf("cmdbuf: %s\n", cmdbuf);
	FILE *fp_read = NULL;
	fp_read = popen(cmdbuf, "r");
	if (fp_read == NULL)
	{
		printf("============>%s failure\n",cmdbuf);
		pclose(fp_read);
	}
	else
	{
		printf("============>%s  success\n",cmdbuf);
		pclose(fp_read);
	}
	return(0);
}

int CommLib_setEth1Dns(const char *firstdns, const char *seconddns)
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

/*some tools function, not in the commlib.*/
static int _CommLib_checkEth1IpAddress(const char *ipaddr)
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

static int _CommLib_checkEth1MacAddress(const char *macaddr)
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
                return(0);

        if (macaddr[2] != ':' || macaddr[5] != ':' || macaddr[8] != ':' || macaddr[11] != ':' || macaddr[14] != ':')
                return(-1);

        return(0);
}

static int _CommLib_convertEth1MacToInt(const char *macaddr, int *p)
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
/* debug_Printf(DPFL,"p[%d]=%d\n", j, p[j]);*/
        }

        return(0);
}

void CommLib_joinAP_wep(char *essid,char safety_option,char passwd_format,char key_numb,char *password)
{
	char buf[128];
#if 0
	if(channel !=0)
	{
		bzero(buf,sizeof(buf));
		sprintf(buf, "iwconfig Eth1 channel %d",channel);
		printf("%s\n",buf);
		system(buf);
	}
#endif
	printf("CommLib_joinAP_wep \n");
	if(key_numb > 4 || key_numb < 1)
		return ;
	if((safety_option == 1) || (safety_option == 2))//自动选择  开放系统
	{
		if(passwd_format == 1)
		{
			bzero(buf,sizeof(buf));
			sprintf(buf, "iwconfig eth1 key [%d] %s",key_numb,password);
			printf("%s\n",buf);
			FILE *fp_read = NULL;
			fp_read = popen(buf, "r");
			if(fp_read == NULL)
			{
				printf("============>%s failure\n",buf);
				pclose(fp_read);
			}
			else
			{
				printf("============>%s  success\n",buf);
				pclose(fp_read);
			}
		}
		else if(passwd_format == 2)
		{
			bzero(buf,sizeof(buf));
			sprintf(buf, "iwconfig eth1 key [%d] s:%s",key_numb,password);
			printf("%s\n",buf);
			FILE *fp_read = NULL;
			fp_read = popen(buf, "r");
			if(fp_read == NULL)
			{
				printf("============>%s failure\n",buf);
				pclose(fp_read);
			}
			else
			{
				printf("============>%s  success\n",buf);
				pclose(fp_read);
			}
		}
	}
	else if(safety_option == 3)//共享加密
	{
		if(passwd_format == 1)
		{
			bzero(buf,sizeof(buf));
			sprintf(buf, "iwconfig eth1 key [%d] %s restricted",key_numb,password);
			printf("%s\n",buf);
			FILE *fp_read = NULL;
			fp_read = popen(buf, "r");
			if(fp_read == NULL)
			{
				printf("============>%s failure\n",buf);
				pclose(fp_read);
			}
			else
			{
				printf("============>%s  success\n",buf);
				pclose(fp_read);
			}
		}
		else if(passwd_format == 2)
		{
			bzero(buf,sizeof(buf));
			sprintf(buf, "iwconfig eth1 key [%d] s:%s restricted",key_numb,password);
			printf("%s\n",buf);
			FILE *fp_read = NULL;
			fp_read = popen(buf, "r");
			if(fp_read == NULL)
			{
				printf("============>%s failure\n",buf);
				pclose(fp_read);
			}
			else
			{
				printf("============>%s  success\n",buf);
				pclose(fp_read);
			}
		}
	}
	else
		return;
}

void CommLib_joinAP_psk(void)
{
}

/* 1->Connected, 0->Unconnect */
int CommLib_getEth1LinkState(void)
{
    FILE* pipe = popen("/opt/user/iwpriv ra0 connStatus", "r");
    if (pipe == NULL) {
        printf("[fun]%s, [line]%d, popen(/opt/user/iwpriv ra0 connStatus) failed!\n", __FUNCTION__, __LINE__);
        return 0;
    }

    char buffer[64];
    memset(buffer, 0, sizeof(buffer));
    char *str = "ra0       connStatus:Connected";
	int len = strlen(str);
	
	fread(buffer, 1, len, pipe);
	pclose(pipe);
	
	buffer[len] = 0;

	return (strcmp(str, buffer) == 0);
}



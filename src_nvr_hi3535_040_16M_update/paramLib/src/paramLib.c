/*
 * Filename	    : ParamLib.c
 * Description	: Parameter and log manipulation.
 * Created	    : 2012-04-12  	
 */
#include "paramLib.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <utime.h>
#include <pthread.h>
#include <semaphore.h>
#include <assert.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <net/if_arp.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <ctype.h>
#include <math.h>
#include <inttypes.h>
#include <sched.h>
#include <sys/timeb.h>
#include <netdb.h>
#include <signal.h>
#include <sys/syscall.h>

#include "commLib.h"

/************************************************************************/
/*                         全局变量                                     */
/************************************************************************/

//这里是包含了各个模块参数的总的接口体变量，在初始化paramlib的时候会
//存储参数的容器中把所有模块的参数统一读取到g_param中
static struct param g_param;
static pthread_mutex_t g_param_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t g_file_mutex=PTHREAD_MUTEX_INITIALIZER;
static int g_param_dirty;    //当g_param_dirty=1的时候表示有参数需要更新
static int g_flash_sync_flag;//控制参数更新线程的标

static unsigned char atoh(const char *str);
/************************************************************************/
/*                        函数实现                                     */
/************************************************************************/
void DefaultSyscomfig(void)
{
    printf("[%s]\n", __FUNCTION__);
    memset(&g_param.sysconfig, 0, sizeof(g_param.sysconfig));
    g_param.sysconfig.dev_type = DEV_TYPE_GM_8126;
    g_param.sysconfig.chip_num = 1;
    g_param.sysconfig.channel_num = MAX_CHANNEL_NUM;
    g_param.sysconfig.prober_num = 1;
    g_param.sysconfig.output_num = 2;
    g_param.sysconfig.serial_num = 2;
    g_param.sysconfig.adchip_type = 2;
    memset(g_param.sysconfig.version, 0, 64);
	strcpy(g_param.sysconfig.version, "IPC3519v101-V1.0.0.1--2017-07-19");
}

void DefaultPortconfig(void)
{
    printf("[%s]\n", __FUNCTION__);
    memset(&g_param.network_port_config, 0, sizeof(g_param.network_port_config));
    g_param.network_port_config.netserver_port = 9060;
    g_param.network_port_config.rtspport = 554;
    g_param.network_port_config.httppport = 80;
    g_param.network_port_config.maxlink = 0;
    g_param.network_port_config.onvifport = 8080;
}

/*
时间同步方式	time_sync	unsigned short	(0:ntp, 1:manual)	ntp	R/W
系统日期和时间  datetime	Char 	(yyyy-MM-dd hh:mm:ss)	不适用	R/W
NTP服务器地址	ntp_server	char	　	198.123.30.132	R/W
时区	time_zone	unsigned short	(0:GMT, 1:GMT1,2:GMT2,3:GMT-1,4:GMT-2 GMT8	R/W
*/
void DefaultDateTimeConfigParam(void)
{
    printf("[%s]\n", __FUNCTION__);
    memset(&g_param.datetimeconfig, 0, sizeof(g_param.datetimeconfig));
    g_param.datetimeconfig.time_sync = 1;
    strcpy(g_param.datetimeconfig.datetime,"2012-04-24 15:25:45");
    strcpy(g_param.datetimeconfig.ntp_server,"198.123.30.132");
    g_param.datetimeconfig.time_zone = 27;//GMT8
}

/*
网卡类型        eth_type               unsigned short     0:有线网络：无线网
IP版本          eth_protocol           unsigned short	  0:IPV4:IPV6	
MAC 地址	    eth_mac                char	              (XX:XX:XX:XX:XX:XX)	ntp	R/W
IP地址获取模式	eth_ipmode	           unsigned short	  0:static,1:dhcp	不适用	R/W
静态IP地址	    eth_ipaddr	           char	　	          198.123.30.132	R/W
子网掩码	    eth_subnet_mask	       char	　	          GMT8	R/W
网关地址	    eth_gateway	           char	　		
DNS服务器地址	dns_server_f	       char	　		      首选DNS服务
DNS服务器地址	dns_server_s	       char	              备用DNS服务
DNS服务器使能   dns_enable	           unsigned short	(1,0)		
*/
void DefaultNetworkConfigParam(void)
{
    printf("[%s]\n", __FUNCTION__);

    char g_mac_random[20]="00:00:00:00:00:01", g_mac1_random[20]="00:00:00:00:00:02";
    struct timeval mac_tm;
    memset(&g_param.network_config, 0, sizeof(g_param.network_config));
    gettimeofday(&mac_tm, NULL);
    sprintf(g_mac_random, "00:00:%02x:%02x:%02x:01", ((int)mac_tm.tv_usec>>16)&0xff, ((int)mac_tm.tv_usec>>8)&0xff, (int)mac_tm.tv_usec&0xff);
    sprintf(g_mac1_random, "00:00:%02x:%02x:%02x:%02x", ((int)mac_tm.tv_usec>>16)&0xff, ((int)mac_tm.tv_usec>>8)&0xff, (int)mac_tm.tv_usec&0xff, ((int)mac_tm.tv_usec>>4)&0xff);
    printf("mac random is  %s\n", g_mac_random);
    printf("mac1 random is %s\n", g_mac1_random);
    strcpy(g_param.network_config.eth_ipaddr,"192.168.1.37");
    strcpy(g_param.network_config.eth_subnet_mask,"255.255.255.0");
    strcpy(g_param.network_config.eth_gateway,"192.168.1.1");
    strcpy(g_param.network_config.dns_server1,"192.168.1.1");
    strcpy(g_param.network_config.dns_server2,"8.8.8.8");
    memcpy(g_param.network_config.eth_mac, g_mac1_random, 20);
    g_param.network_config.dns_enable = 1;
    g_param.network_config.dhcp_flag = 0;
    g_param.network_config.eth_ipmode = 0;
    g_param.network_config.eth_type = 0;
    g_param.network_config.eth_protocol = 0;
}

/*
是否启用PPPOE 	pppoe_enable	unsigned short	(1,0)	ntp	R/W
PPPOE 用户	pppoe_user	Char	(英文字母, 数字, 下划	不适用	R/W
PPPOE 密码	pppoe_password	Char	　	198.123.30.132	R/W
PPPOE 连接状态 pppoe_status	Char	　拨号成功显示公网IP	GMT8	R/W
*/
void DefaultPPPOEConfigParam(void)
{
    printf("[%s]\n", __FUNCTION__);
    memset(&g_param.pppoe_config, 0, sizeof(g_param.pppoe_config));
    strcpy(g_param.pppoe_config.pppoe_user,"");
    strcpy(g_param.pppoe_config.pppoe_password,"");
    strcpy(g_param.pppoe_config.pppoe_status,"000.000.000.000");
    g_param.pppoe_config.pppoe_enable = 0;
}

void DefaultAutoMaintainConfigParam(void)
{
    printf("[%s]\n", __FUNCTION__);
    memset(&g_param.automaintain, 0, sizeof(auto_maintain_t));
    g_param.automaintain.enable = 0;
    g_param.automaintain.data = 127;
    strcpy(g_param.automaintain.time,"00:00:00");
    g_param.pppoe_config.pppoe_enable = 0;
}

/*
是否启用邮件功能 	    email_enable	        unsigned short   	
发送邮件服务器	        email_serverIP	        Char	 
发送人地址              email_fromAddr          Char
发送人邮箱密码	        email_password	        Char	　		
发件人名字              email_userName          Char
接收人地址              email_toAddr            Char
邮件主题	            email_subject	        Char	　     
登陆名                  email_loginName         Char  
端口                    email_port              int
*/
void DefaultEmailConfigParam(void)
{
    printf("[%s]\n", __FUNCTION__);
    memset(&g_param.email_config, 0, sizeof(g_param.email_config));
    strcpy(g_param.email_config.email_serverIP,"");
    strcpy(g_param.email_config.email_fromAddr,"");
    strcpy(g_param.email_config.email_password,"");
    strcpy(g_param.email_config.email_userName,"");
    strcpy(g_param.email_config.email_toAddr,"");
    strcpy(g_param.email_config.email_subject,"");
    strcpy(g_param.email_config.email_loginName,"");
    g_param.email_config.email_enable = 0;
    g_param.email_config.email_port = 0;
}

/*
是否启用FTP 	ftp_enable	    unsigned short	
FTP 用户	    ftp_user	    Char		
FTP 密码	    ftp_password	Char	　		
服务器路径      ftp_directory	Char	　     ftp://主机IP:端口/路径/ 
预留字段        unused          char
*/
void DefaultFTPConfigParam(void)
{
    printf("[%s]\n", __FUNCTION__);
    memset(&g_param.ftp_config, 0, sizeof(g_param.ftp_config));
    strcpy(g_param.ftp_config.ftp_directory,"ftp://192.168.10.73:21/TDDOWNLOAD/test");
    strcpy(g_param.ftp_config.ftp_username,"admin");
    strcpy(g_param.ftp_config.ftp_password,"admin");
    g_param.ftp_config.ftp_enable = 0;
}

/*
是否启用 DDNS 	ddns_enable	unsigned short	(1,0)	ntp	R/W
DDNS 服务器地址	ddns_server	Char	IP 地址或域名不适用	R/W
DDNS 用户名 ddns_user	Char	(英文字母, 数字, 下划线	198.123.30.132	R/W
DDNS 密码	ddns_password	Char	(英文字母, 数字, 下划线	GMT8	R/W
*/
void DefaultDDNSConfigParam(void)
{
    printf("[%s]\n", __FUNCTION__);
    memset(&g_param.ddns_config, 0, sizeof(ddns_config_t));
    strcpy(g_param.ddns_config.ddns_server,"");
    strcpy(g_param.ddns_config.ddns_user,"");
    strcpy(g_param.ddns_config.ddns_password,"");
    g_param.ddns_config.ddns_enable = 0;
}


void DefaultSIPConfigParam(void)
{
    printf("[%s]\n", __FUNCTION__);
    memset(&g_param.sip_config, 0, sizeof(sip_config_t));
    #if 0
    strcpy(g_param.sip_config.sip_user,"v3021a89bc"); //v3022b36c4, v3021a89bc
    strcpy(g_param.sip_config.sip_password,"admin");
    strcpy(g_param.sip_config.sip_ServerPath1,"admin");
    strcpy(g_param.sip_config.sip_ServerPath2,"admin");
    g_param.sip_config.sip_enable = 1;
    #else
    strcpy(g_param.sip_config.sip_user,""); //v3022b36c4, v3021a89bc
    strcpy(g_param.sip_config.sip_password,"");
    strcpy(g_param.sip_config.sip_ServerPath1,"");
    strcpy(g_param.sip_config.sip_ServerPath2,"");
    g_param.sip_config.sip_enable = 0;
    #endif
    g_param.sip_config.sip_resetflag = 0;
}

/*
亮度	        brightness	unsigned short	(0-256)
对比度	        contrast	unsigned short	(0-256)
色度	        hue	        unsigned short	(0-256)
饱和度	        saturation	unsigned short	(0-256)
锐度            sharpness   unsigned short	(0-256)
白天黑夜模式	daynight	unsigned short	 0 : night mode 1: day mode 2: auto 3: timer
自动白平衡	    whitebalance short	(0-100)	 0 : indoor 1: outdoor 2: Auto 
背光	        backlight	unsigned short	(0-100)	0:low 1:median 2: hight 
闪烁频率控制    imagesource unsigned short   0 : 60hz 1 : 50hz
翻转            flip        unsigned short   0 : 禁止 1 : 启动
镜像            mirrorimage unsigned short   0 : 禁止 1 : 启动
制式	        video_standard	unsigned short	(NTSC,PAL,auto)		
*/
void DefaultSensorConfigParam(void)
{
    printf("[%s]\n", __FUNCTION__);
    int i, ir_cut, light_sensor;
    
    for (i = 0; i < MAX_CHANNEL_NUM; i++)
    {
        ir_cut = !!g_param.sensor_config[i].ir_cut;
        light_sensor = !!g_param.sensor_config[i].light_sensor;
        
        memset(&g_param.sensor_config[i], 0, sizeof(sensor_config_t));
        g_param.sensor_config[i].brightness = 128;
        g_param.sensor_config[i].contrast = 128;
        g_param.sensor_config[i].hue = 128;
        g_param.sensor_config[i].saturation = 128;
        g_param.sensor_config[i].backlight = 5;
        g_param.sensor_config[i].sharpness = 128;
        g_param.sensor_config[i].whitebalance = 0;
        g_param.sensor_config[i].imagesource = 1;
        g_param.sensor_config[i].daynight = 2;
        g_param.sensor_config[i].video_standard = 1;
        g_param.sensor_config[i].flip = 0;
        g_param.sensor_config[i].mirrorimage = 0;
        g_param.sensor_config[i].ir_cut = ir_cut; //恢复出厂设置不改icut方向
        g_param.sensor_config[i].light_sensor = light_sensor; //恢复出厂设置不改光敏输入电平
        strcpy(g_param.daynight_schedule[i].strattime, "00:00:00");
        strcpy(g_param.daynight_schedule[i].endtime, "18:00:00");
    }
}

void DefaultIspConfigParam(void)
{
	printf("[%s]\n", __FUNCTION__);
	memset(&g_param.isp_config, 0, sizeof(isp_config_t));
	g_param.isp_config.weightingEnable = 20;
	g_param.isp_config.weightingMode = 6;
	
	g_param.isp_config.aeType = 0;
	g_param.isp_config.aeExpStrength = 13;
	g_param.isp_config.aeExpCompensation = 128;
	
	g_param.isp_config.denoiseType = 0;
	g_param.isp_config.SfStrength = 128;
	g_param.isp_config.TfStrength = 128;
	
	g_param.isp_config.IframeStrength = 5;
	
	g_param.isp_config.defogEnable = 0;
	g_param.isp_config.defogStrength = 128;
	
	g_param.isp_config.disEnable = 0;
}

/*
分辨率  resolution	unsigned short	(QCIF = 0,CIF = 1,HD1 = 2,D1 = 3,MD1 = 4,QVGA = 5,VGA = 6,720P = 7,1080P = 8,)	ntp	R/W
帧率	fps	unsigned short	1~25 (PAL), 1~30 (NTSC)	不适用	R/W
码流	bitrate	unsigned short	16~8000kbps	198.123.30.132	R/W
码流控制方式	bitrate_control	unsigned short	CBR = 0, VBR = 1	GMT8	R/W
图像质量级别	BaseImageQuality	unsigned short	22~39		
I帧间隔     IFrameInterval	unsigned short	I帧间隔：1~300		
音频编码	audio_format	unsigned short	PCM=0, G711A=1, G711Mu=2, AAC=3		
采样率      audio_sample	unsigned short	8000, 16000,  48000		
声道        audio_channels	unsigned short	1, 2		
位率	audio_bits	unsigned short	8, 16		
*/
void DefaultVideoEncConfigParam(void)
{
    printf("[%s]\n", __FUNCTION__);
    int i;
    for (i = 0; i < MAX_CHANNEL_NUM; i++)
    {
        memset(&g_param.v_enc_config[i], 0, sizeof(v_enc_config_t));
        g_param.v_enc_config[i].encmodle = 0;
        g_param.v_enc_config[i].enclevel = 0; // h264采用BaseLine方式编码
	    g_param.v_enc_config[i].resolution = 8; // PIC_2592x1520
        g_param.v_enc_config[i].fps = 25;
        g_param.v_enc_config[i].bitrate = 4096;
        g_param.v_enc_config[i].bitrate_control = 1;
        g_param.v_enc_config[i].BaseImageQuality = 32;
        g_param.v_enc_config[i].IFrameInterval = 50;	
    }
}

void DefaultDSTConfigParam(void)
{
    printf("[%s]\n", __FUNCTION__);
    memset(&g_param.dst_config, 0, sizeof(DST_config_t));
}

/*
分辨率  resolution	unsigned short	(QCIF = 0,CIF = 1,HD1 = 2,D1 = 3,MD1 = 4,QVGA = 5,VGA = 6,720P = 7,1080P = 8,)	ntp	R/W
帧率	fps	unsigned short	1~25 (PAL), 1~30 (NTSC)	不适用	R/W
码流	bitrate	unsigned short	16~8000kbps	198.123.30.132	R/W
码流控制方式	bitrate_control	unsigned short	CBR = 0, VBR = 1	GMT8	R/W
图像质量级别	BaseImageQuality	unsigned short	22~39		
I帧间隔     IFrameInterval	unsigned short	I帧间隔：1~300		
音频编码	audio_format	unsigned short	PCM=0, G711A=1, G711Mu=2, AAC=3		
采样率      audio_sample	unsigned short	8000, 16000,  48000		
声道    audio_channels	unsigned short	1, 2		
位率	audio_bits	unsigned short	8, 16		
*/
void DefaultSVideoEncConfigParam(void)
{
    printf("[%s]\n", __FUNCTION__);
    int i;
    for (i = 0; i < MAX_CHANNEL_NUM; i++)
    {
        memset(&g_param.v_s_enc_config[i], 0, sizeof(v_s_enc_config_t));
        g_param.v_s_enc_config[i].encmodle = 0;
        g_param.v_s_enc_config[i].enclevel = 0;		
		g_param.v_s_enc_config[i].resolution = 3;
        g_param.v_s_enc_config[i].fps = 25;
        g_param.v_s_enc_config[i].bitrate = 512;
        g_param.v_s_enc_config[i].bitrate_control = 1;
        g_param.v_s_enc_config[i].ds_enable_flag = 1;
        g_param.v_s_enc_config[i].BaseImageQuality = 32;
        g_param.v_s_enc_config[i].IFrameInterval = 50;	
    }
}

void DefaultWifiParam(void)
{
    memset(&g_param.wireless_config, 0, sizeof(wireless_config_t));
    g_param.wireless_config.enable = 0;
    strcpy(g_param.wireless_config.essid, "Altumview");
    g_param.wireless_config.signalStrength = -255; // -255 -> 信号状态为无
    g_param.wireless_config.state = 0;
    g_param.wireless_config.encrypt_type = 3;
    g_param.wireless_config.encrypt_option = 0;
    g_param.wireless_config.key_numb = 0;
    strcpy(g_param.wireless_config.password, "88889999");

    g_param.wireless_config.eth_ipmode = 0;
    g_param.wireless_config.eth_type = 0; 
    strcpy(g_param.wireless_config.eth_ipaddr, "192.168.199.88");
    strcpy(g_param.wireless_config.eth_subnet_mask, "255.255.255.0");
    strcpy(g_param.wireless_config.eth_gateway, "192.168.199.1");
    strcpy(g_param.wireless_config.dns_server1, "192.168.199.1");
    strcpy(g_param.wireless_config.dns_server2, "8.8.8.8");
    g_param.wireless_config.dns_enable = 0;
}

/*	
是否启动音频编码  audio_enable      unsigned short  0;不启动，1：启动
音频编码	      audio_format	    unsigned short	PCM=0, G711A=1, G711Mu=2, AAC=3		
采样率	          audio_sample	    unsigned short	0:8000, 1:16000	
声道数	          audio_channels	unsigned short	1, 2		
位率	          audio_bits	    unsigned short	0:8, 1:16,2:64;	
输入音量          audio_inputvol;   unsigned short  0~100
输出音量          audio_outputvol;  nsigned short   0~100
是否打开双向语音  unsigned short    audio_micspeaker;0;不启动，1：启动
*/
void DefaultAudioEncConfigParam(void)
{
    printf("[%s]\n", __FUNCTION__);
    int i;
    for (i = 0; i < MAX_CHANNEL_NUM; i++)
    {
        memset(&g_param.a_enc_config[i], 0, sizeof(a_enc_config_t));
        g_param.a_enc_config[i].audio_enable = 0;
        g_param.a_enc_config[i].audio_format = 1;
        g_param.a_enc_config[i].audio_sample = 0;
        g_param.a_enc_config[i].audio_channels = 1;
        g_param.a_enc_config[i].audio_bits = 1;
        g_param.a_enc_config[i].audio_inputvol = 75;
        g_param.a_enc_config[i].audio_outputvol = 75;
        g_param.a_enc_config[i].audio_micspeaker = 0;
    }
}

/*
是否启用OSD	osd_enable	unsigned short	(1,0)	ntp	R/W
通道        channal_num	unsigned short	　		
OSD 文本	osd_text	char	　	不适用	R/W
OSD X位置	osd_x	unsigned short	　	198.123.30.132	R/W
OSD Y位置	osd_y	unsigned short	　	GMT8	R/W
OSD 颜色	osd_color	unsigned short	0:black, 1:white, 2:red, 3:green,4: blue,5: yellow
显示日期和时间  time	unsigned short	(1,0)		
显示时间 X位置	time_x	unsigned short	　		
显示时间 y位置	time_y	unsigned short	　		
time 颜色	time_color	unsigned short	0:black, 1:white, 2:red, 3:green,4: blue,5: yellow
*/
void DefaultOSDConfigParam(void)
{
    printf("[%s]\n", __FUNCTION__);
    int i;
    for (i = 0; i < MAX_CHANNEL_NUM; i++)
    {
    	memset(&g_param.osd_config[i], 0, sizeof(osd_config_t));
    	g_param.osd_config[i].osd_enable = 1;
    	strcpy(g_param.osd_config[i].osd_text, "IPC");
    	g_param.osd_config[i].osd_x = 0;
    	g_param.osd_config[i].osd_y = 50;
    	g_param.osd_config[i].osd_color = 0;
    	g_param.osd_config[i].time_enable = 1;
    	g_param.osd_config[i].time_x = 0;
    	g_param.osd_config[i].time_y = 0;
    	g_param.osd_config[i].time_color = 0;
    	memset(&g_param.osd_config_ex[i], 0, sizeof(osd_config_t_EX));
    }
}

/*
启用overlay	overlay_enable	unsigned short	(1,0)	ntp	R/W
通道channal_num	unsigned short	　	不适用	R/W
overlay X位置	overlay_x	unsigned short	　	198.123.30.132	R/W
overlay y位置	overlay_y	unsigned short	　	GMT8	R/W
overlay width	overlay_width	unsigned short	　		
overlay height	overlay_height	unsigned short	　		
*/
void DefaultOverlayConfigParam(void)
{
    printf("[%s]\n", __FUNCTION__);
    int i, j;
    for (i = 0; i < MAX_CHANNEL_NUM; i++)
    {
        for (j = 0; j < 4; j++)
        {	
            memset(&g_param.overlay_config[i][j], 0, sizeof(overlay_config_t));
            g_param.overlay_config[i][j].overlay_enable = 0;
            g_param.overlay_config[i][j].overlay_x = 50*(1+j);
            g_param.overlay_config[i][j].overlay_y = 50*(1+j);
            g_param.overlay_config[i][j].overlay_width = 50;
            g_param.overlay_config[i][j].overlay_height = 50;
        }
    }
}

/*
是否启用移动侦测	md_enable	unsigned short	(1,0)	ntp	R/W
移动侦测区域	md_region	char	BASE64 编码的二进制数据	不适用	R/W
灵敏度          md_sensitivity	unsigned short	0~10级，10最灵敏	198.123.30.132	R/W
侦测时间段一	md_time1	char	秒数范围，例如：0~86399 0:00:00 ~23:59:59	GMT8	R/W
侦测时间段二	md_time2	char	秒数范围，例如：0~86399 0:00:00 ~23:59:59		
侦测时间段三	md_time3	char	秒数范围，例如：0~86399 0:00:00 ~23:59:59		
侦测时间段四	md_time4	char	秒数范围，例如：0~86399 0:00:00 ~23:59:59		
*/
void DefaultMDConfigParam(void)
{
    printf("[%s]\n", __FUNCTION__);
    int i;
    for (i = 0; i < MAX_CHANNEL_NUM; i++)
    {			
        memset(&g_param.md_config[i], 0, sizeof(md_config_t));
        g_param.md_config[i].md_sensitivity = 9;
        strcpy(g_param.md_config[i].md_time1, "00:00:00~23:59:59");
        strcpy(g_param.md_config[i].md_time2, "00:00:00~23:59:59");
        strcpy(g_param.md_config[i].md_time3, "00:00:00~23:59:59");
        strcpy(g_param.md_config[i].md_time4, "00:00:00~23:59:59");		
        g_param.md_config[i].md_data = 127;  
        g_param.md_config[i].md_time = 15;  
        g_param.md_config[i].md_alarmout = 0;
        g_param.md_config[i].md_alarmrecycle = 10;
        memset(g_param.md_config[i].md_region, 0, 12 * 16 * sizeof(char));
        g_param.md_config[i].md_enable = 0;
        g_param.md_config[i].md_alarmrecord = 0;
        g_param.md_config[i].md_capturetoftp = 0;
        g_param.md_config[i].md_sendemail = 0;
        g_param.md_config[i].md_alarmout = 0;
    }
}

/*
是否启用IO告警功能	io_enable	          unsigned short   (1,0)	ntp	R/W
检测类型            io_type	              unsigned short   0:常开, 1：常闭
联动录像使能        io_alarmrecord        unsigned short   0:联动录像不启动, 2:联动录像启
联动抓拍            io_capturetoftp       unsigned short   0:不启动, 1:启动抓拍功能并把图片保存到本地，如果ftp功能是可有的就把图片告警图片上传到ftp
联动发送邮件        io_sendemail          unsigned short   0:不启动功能不发送抓拍图片, 1：启动功能发送抓拍图片
io口联动输出        io_alarmout           unsigned short   F:1111 表示最多支持四路输出
生效时间类型        io_datatype           unsigned short   0:每天, 1:星期的某一天，或星期的多天
生效时间            io_data               char             1,2,3,4,5,6,7：星期一到星期天，时间类型为每天的这个时候该参数无效
侦测时间段一	    io_time1	          char	           秒数范围，例如：0~86399, 0:00:00 ~23:59:59	GMT8	R/W
侦测时间段二	    io_time2	          char	           秒数范围，例如：0~86399, 0:00:00 ~23:59:59		
侦测时间段三	    io_time3	          char	           秒数范围，例如：0~86399, 0:00:00 ~23:59:59		
侦测时间段四	    io_time4	          char	           秒数范围，例如：0~86399, 0:00:00 ~23:59:59		
*/
void DefaultIOConfigParam(void)
{
    printf("[%s]\n", __FUNCTION__);
    int i;
    for (i = 0; i < MAX_CHANNEL_NUM; i++)
    {			
        memset(&g_param.io_config[i], 0, sizeof(io_config_t));
        g_param.io_config[i].io_type = 0;
        strcpy(g_param.io_config[i].io_time1,"00:00:00~23:59:59");
        strcpy(g_param.io_config[i].io_time2,"00:00:00~23:59:59");
        strcpy(g_param.io_config[i].io_time3,"00:00:00~23:59:59");
        strcpy(g_param.io_config[i].io_time4,"00:00:00~23:59:59");	
        g_param.io_config[i].io_data = 127;  
        g_param.io_config[i].io_time = 15;  
        g_param.io_config[i].io_alarmout = 0;
        g_param.io_config[i].io_alarmrecord = 0;
        g_param.io_config[i].io_enable = 0;
        g_param.io_config[i].io_sendemail = 0;
        g_param.io_config[i].io_alarmrecycle = 10;
    }
}

/*
文件分隔模式        record_packetsize	   unsigned short	分钟为单位 5~120
是否启用循环录像	   record_recycle	       unsigned short	(1,0), 0:不覆盖，停止录像, 1：覆盖录像
预录像时间          record_advancetime      unsigned short  秒为单位5~30
告警录像时长        record_alarmtime        unsigned short  秒为单位5~300
录像码流            record_streantype       unsigned short  0:主码流，1:从码流
*/
void DefaultRecordBaseConfigParam(void)
{
    printf("[%s]\n", __FUNCTION__);
    memset(&g_param.recordbase_config, 0, sizeof(recordbase_config_t));
    g_param.recordbase_config.record_packetsize = 30;
    g_param.recordbase_config.record_recycle = 1;
    g_param.recordbase_config.record_advancetime = 10 ;
    g_param.recordbase_config.record_alarmtime = 10;
    g_param.recordbase_config.record_streamtype = 0;
    g_param.recordbase_config.unuse = 63;
}

/*
启用录像	    record_enable	unsigned short	(1,0)	ntp	R/W
录像日期类型	record_datetype	unsigned short	0:每天,1:星期, 2:具体的日期，不适用	R/W
录像日期类型    record_datetypeValue	字符对应录像日期类型，198.123.30.132	R/W
录像时间段一	record_time1	字符秒数范围，例如：0~86399, 0:00:00 ~23:59:59	GMT8	R/W
录像时间段二	record_time2	字符秒数范围，例如：0~86399, 0:00:00 ~23:59:59		
录像时间段三	record_time3	字符秒数范围，例如：0~86399, 0:00:00 ~23:59:59		
录像时间段四	record_time4	字符秒数范围，例如：0~86399, 0:00:00 ~23:59:59		
*/
void DefaultRecordScheduleConfigParam(void)
{
    printf("[%s]\n", __FUNCTION__);
    int i;
    for (i = 0; i < MAX_CHANNEL_NUM; i++)
    {
    	memset(&g_param.recordschedule_config[i], 0, sizeof(recordschedule_config_t));
    	g_param.recordschedule_config[i].record_enable = 0;
    	strcpy(g_param.recordschedule_config[i].record_time1,"00:00:00~23:59:59");
    	strcpy(g_param.recordschedule_config[i].record_time2,"00:00:00~23:59:59");
    	strcpy(g_param.recordschedule_config[i].record_time3,"00:00:00~23:59:59");
    	strcpy(g_param.recordschedule_config[i].record_time4,"00:00:00~23:59:59");
    	g_param.recordschedule_config[i].record_data = 127;
    	g_param.recordschedule_config[i].record_time = 15;
    }
}

/*
ptz_enable          unsigned short	(1,0)
ptz_address        unsigned short	0~255
ptz_protocol        unsigned short	0:Pelco D;1:Pelco P
ptz_bandrate       unsigned short	0:1200,1:2400;2:4800;3:9600;4:14400;5:19200;6:38400;7:57600;8:115200
ptz_parity           unsigned short	0:none, 1:odd, 2:even		
ptz_databit         unsigned short	7, 8		
ptz_stopbit         unsigned short	1, 2		
*/
void DefaultPTZConfigParam(void)
{
	printf("[%s]\n", __FUNCTION__);
	memset(&g_param.ptz_config, 0, sizeof(ptz_config_t));
	g_param.ptz_config.ptz_enable = 0;
	g_param.ptz_config.ptz_address = 1;
	g_param.ptz_config.ptz_protocol = 1;
	g_param.ptz_config.ptz_bandrate = 1;
	g_param.ptz_config.ptz_parity = 0;
	g_param.ptz_config.ptz_databit = 1;
	g_param.ptz_config.ptz_stopbit = 0;
}

/*
user_name       char                R/W
user_passwd     char                R/W
*/
void DefaultUserConfigParam(void)
{
   printf("[%s]\n", __FUNCTION__);
   memset(g_param.user_config, 0, sizeof(user_config_t)*20);
   strcpy(g_param.user_config[0].user_name,"admin");
   strcpy(g_param.user_config[0].user_passwd,"admin");
   strcpy(g_param.user_config[0].user_description,"");
   g_param.user_config[0].user_enable = 1;
   g_param.user_config->user_jurisdiction = 0;
}

//音视频
// device_serial_no[0]-> 视频
// device_serial_no[1]-> 音频
// device_serial_no[2]-> 图像
// device_serial_no[3]-> OSD COVER
//网络
// device_serial_no[4]-> TCP/IP
// device_serial_no[5]-> WIFI
// device_serial_no[6]-> 端口
// device_serial_no[7]-> PPPoE DDNS
// device_serial_no[8]-> FTP
// device_serial_no[9]-> Email
// device_serial_no[10]-> 云服务
// device_serial_no[11]-> GB28181
//高级
// device_serial_no[12]-> ptz
// device_serial_no[13]-> 时间
// device_serial_no[14]-> 移动侦测
// device_serial_no[15]-> IO告警
// device_serial_no[16]-> 磁盘管理，告警录像checkbox的div
// device_serial_no[17]-> 录像管理
// device_serial_no[18]-> 用户管理
//系统
// device_serial_no[19]-> 升级
// device_serial_no[20]-> 重启
// device_serial_no[21]-> 出厂设置
// device_serial_no[22]-> 备份还原
// device_serial_no[23]-> 版本信息
// device_serial_no[24]-> 日志管理
//智能
// device_serial_no[25]-> 人脸检测:人脸参数设置
// device_serial_no[26]-> 人脸检测:区域屏蔽
// device_serial_no[27]-> 人脸检测:区域识别
// device_serial_no[28]-> 人脸检查:版本信息
// device_serial_no[29]-> 人脸识别:人脸告警设置
void DefaultDeviceInfo(void)
{
  printf("[%s]\n", __FUNCTION__);
  char date[16];
  ParamLib_GetCompileDateTime(date, NULL);

  strcpy(g_param.systeminfo.factory_name, "");
  strcpy(g_param.systeminfo.version_number, "HI3535_V100");
  strcpy(g_param.systeminfo.device_type, "ALARM");
  strcpy(g_param.systeminfo.version_date, date);
  strcpy(g_param.systeminfo.Max_Support_Resolution, "2.0M");
                                             /*012345678901234567890123456789 */
  strcpy(g_param.systeminfo.device_serial_no, "101111111001010100111111110011");
  strcpy(g_param.systeminfo.kernel_version, "V1.0.0.2");
  strcpy(g_param.systeminfo.web_version, "V1.0.0.3");
  strcpy(g_param.systeminfo.Model, "MT9D111"); // GB28181 使用的公安设备型号
  strcpy(g_param.systeminfo.ProductHardVersion, "HI3535_V100");
  strcpy(g_param.systeminfo.HardwareId, "20120305-00001");
  printf("g_param.systeminfo.device_serial_no:%s\n", g_param.systeminfo.device_serial_no);
}

int ParamLib_GetSerialInfo(char *serialNum)
{
    strcpy(serialNum, g_param.systeminfo.device_serial_no);
    return 0;
}

int ParamLib_SetSerialInfo(char *serialNum)
{
    strcpy(g_param.systeminfo.device_serial_no, serialNum);
    return 0;
}

int ParamLib_RefreshDeviceInfo(void)
{
    printf("[%s]\n", __FUNCTION__);
    Pthread_mutex_lock(&g_param_mutex);
    DefaultDeviceInfo();
    g_param_dirty = 1;
    Pthread_mutex_unlock(&g_param_mutex);
    return 0;
}

/*hand record */
static void DefaultHandRecordStruct(void)
{
    printf("[%s]\n", __FUNCTION__);
    int i;
    for (i = 0; i < MAX_CHANNEL_NUM; i++)
    {			
    	memset(&g_param.hand_record[i], 0, sizeof(hand_record_t));
    }
}

static void DefaultGB28181Struct(void)
{
    printf("[%s]\n", __FUNCTION__);
    g_param.gb28181_config.chEnable = 0;
    g_param.gb28181_config.chAlarmLevel = 2;
    strcpy(g_param.gb28181_config.szSipServerID,"34020000002000000001");
    strcpy(g_param.gb28181_config.szSipDomain,"3402000000");
    strcpy(g_param.gb28181_config.szSipServerIP,"");
    g_param.gb28181_config.nSipServerPort = 5060;
    strcpy(g_param.gb28181_config.szSipDeviceID,"34020000001320000001");
    strcpy(g_param.gb28181_config.szSipAlarmID,"34020000001340000001");
    strcpy(g_param.gb28181_config.szPassword,"");
    strcpy(g_param.gb28181_config.szSipChannelID,"34020000001320000001");
    strcpy(g_param.gb28181_config.szOwner,"");
    strcpy(g_param.gb28181_config.szCivilCode,"");
    strcpy(g_param.gb28181_config.szBlock,"");
    strcpy(g_param.gb28181_config.szAddress,"");
    strcpy(g_param.gb28181_config.szLongitude,"0.0");
    strcpy(g_param.gb28181_config.szLatitude,"0.0");
    g_param.gb28181_config.nExpires = 3600;
    g_param.gb28181_config.nAlive = 60;
    g_param.gb28181_config.nTimeoutNum = 3;
}

static void DefaultAlarmStruct(void)
{
  printf("[%s]\n", __FUNCTION__);
  g_param.alarm_config.m_chnum = 256;
  g_param.alarm_config.m_fontsize = 1;
  g_param.alarm_config.m_interval = 5;
  
  int i;
  char buf[MAX_ALARM_STRCH];
  memset(buf, 0, MAX_ALARM_STRCH);
  for (i = 0; i < MAX_ALARM_NUM; i++) {
    memset(g_param.alarm_config.m_strch[i], 0, MAX_ALARM_STRCH);
    sprintf(buf, "第%d通道", i);
    strcpy(g_param.alarm_config.m_strch[i], buf);
  }
}

static void SoftwareDefaultParam(void)
{
  printf("[%s]\n", __FUNCTION__);
  DefaultSyscomfig();
  DefaultPortconfig();
  DefaultDateTimeConfigParam();
  //DefaultNetworkConfigParam();
  DefaultPPPOEConfigParam();
  DefaultDDNSConfigParam();
  DefaultSensorConfigParam();
  DefaultVideoEncConfigParam();
  DefaultSVideoEncConfigParam();
  DefaultAudioEncConfigParam();
  DefaultOSDConfigParam();
  DefaultOverlayConfigParam();
  DefaultMDConfigParam();
  DefaultIOConfigParam();
  DefaultRecordBaseConfigParam();
  DefaultRecordScheduleConfigParam();
  DefaultPTZConfigParam();	
  DefaultUserConfigParam();
  DefaultFTPConfigParam();
  DefaultEmailConfigParam();
  DefaultDeviceInfo();
  //DefaultSIPConfigParam();
  DefaultHandRecordStruct();
  DefaultGB28181Struct();
  DefaultAutoMaintainConfigParam();
  //DefaultWifiParam();

  DefaultIspConfigParam();
  DefaultDSTConfigParam();
  DefaultAlarmStruct();
}

static void DefaultParam(void)
{
  printf("[%s]\n", __FUNCTION__);
  DefaultSyscomfig();
  DefaultPortconfig();
  DefaultDateTimeConfigParam();
  DefaultNetworkConfigParam();
  DefaultPPPOEConfigParam();
  DefaultDDNSConfigParam();
  DefaultSensorConfigParam();
  DefaultVideoEncConfigParam();
  DefaultSVideoEncConfigParam();
  DefaultAudioEncConfigParam();
  DefaultOSDConfigParam();
  DefaultOverlayConfigParam();
  DefaultMDConfigParam();
  DefaultIOConfigParam();
  DefaultRecordBaseConfigParam();
  DefaultRecordScheduleConfigParam();
  DefaultPTZConfigParam();	
  DefaultUserConfigParam();
  DefaultFTPConfigParam();
  DefaultEmailConfigParam();
  DefaultDeviceInfo();
  DefaultSIPConfigParam();
  DefaultHandRecordStruct();
  DefaultGB28181Struct();
  DefaultAutoMaintainConfigParam();
  DefaultWifiParam();

  DefaultIspConfigParam();
  DefaultDSTConfigParam();
  DefaultAlarmStruct();
}

int GetParamDirty(void)
{
   return g_param_dirty;
}

static void *SyncFlashThread(void *arg)
{
    char buffer[2*64*1024] = "";
    int dirty;
    
    while (g_flash_sync_flag) {
        Pthread_mutex_lock(&g_param_mutex);
        dirty = 0;
        if (g_param_dirty) {
            g_param_dirty = 0;
        	dirty = 1;
            memcpy(buffer, &g_param, sizeof(g_param));
        }
        Pthread_mutex_unlock(&g_param_mutex);

    	if (dirty == 1) {
            Pthread_mutex_lock(&g_file_mutex);
            FILE *pf = fopen(RUN_PARAM_FILE, "wb");
            if (pf != NULL) {
                fwrite(buffer, 1, sizeof(g_param), pf);
				usleep(1000*1000);
				system("cp -rf "RUN_PARAM_FILE" "BACKUP_PARAM_FILE);
				printf("write the param to the %s\n", RUN_PARAM_FILE);
                fclose(pf); pf = NULL;
            }	
            Pthread_mutex_unlock(&g_file_mutex);
    	}
    	usleep(1000*1000);		
    }
    return NULL;
}

int ParamLib_InitParamLib(void)
{    
	Pthread_mutex_lock(&g_file_mutex);
    FILE *pf = fopen(RUN_PARAM_FILE, "rb");
	FILE *pPort = fopen(PORT_PARAM_FILE, "r+");
    int bedfile = 1;

	if (pf != NULL) {
		fread(&g_param, 1, sizeof(param), pf);
		fclose(pf); pf = NULL;
		if (g_param.end == sizeof(param))
		    bedfile = 0;
	}

	if (bedfile) {
        struct stat statbuff;
        
		if (stat(BACKUP_PARAM_FILE, &statbuff) < 0) {  
			DefaultParam();
			
			Pthread_mutex_lock(&g_param_mutex);
			g_param.end = sizeof(param);
			pf = fopen(RUN_PARAM_FILE, "wb");
			if (pf != NULL) {
				fwrite(&g_param, 1, sizeof(param), pf);
				fclose(pf); pf = NULL;
			    system("cp -rf "RUN_PARAM_FILE" "BACKUP_PARAM_FILE);	
			}
			Pthread_mutex_unlock(&g_param_mutex);
			
		} else {
			system("cp -rf "BACKUP_PARAM_FILE" "RUN_PARAM_FILE);
			pf = fopen(RUN_PARAM_FILE, "rb");
			if (pf != NULL) {
				fread(&g_param, 1, sizeof(param), pf);
				fclose(pf); pf = NULL;
			}
		}		
	}
	Pthread_mutex_unlock(&g_file_mutex);	

	if (pPort != NULL) {
		fwrite(&g_param.network_port_config, sizeof(network_port_config_t), 1, pPort);
		fclose(pPort); pPort = NULL;
	}
	ParamLib_RefreshDeviceInfo();
	
	return 0;
}

int ParamLib_deinitParamLib(void)
{
	return 0;
}

int ParamLib_StartParamService(void)
{
	pthread_t thid;
	g_flash_sync_flag = 1;
	Pthread_create(&thid, NULL, SyncFlashThread, NULL);
	return 0;
}

int ParamLib_StopParamService(void)
{
	g_flash_sync_flag = 0;
	return 0;
}

int ParamLib_SetDateTimeInfo(datetime_config_t datetime)
{
	if(memcmp(&g_param.datetimeconfig, &datetime, sizeof(datetime_config_t)) !=0)
	{
		Pthread_mutex_lock(&g_param_mutex);
		memcpy(&g_param.datetimeconfig, &datetime, sizeof(datetime_config_t));
		g_param_dirty =1;
		Pthread_mutex_unlock(&g_param_mutex);
	}
	return 0;
}

int ParamLib_GetDateTimeInfo(datetime_config_t* datetime)
{
	Pthread_mutex_lock(&g_param_mutex);
	*datetime = g_param.datetimeconfig;
	Pthread_mutex_unlock(&g_param_mutex);
	return 0;
}

int ParamLib_SetDSTInfo(DST_config_t dstinfo)
{
	if(memcmp(&g_param.dst_config, &dstinfo, sizeof(DST_config_t)) !=0)
	{
		Pthread_mutex_lock(&g_param_mutex);
		memcpy(&g_param.dst_config, &dstinfo, sizeof(DST_config_t));
		g_param_dirty =1;
		Pthread_mutex_unlock(&g_param_mutex);
	}
	return 0;
}

int ParamLib_GetDSTInfo(DST_config_t* dstinfo)
{
	Pthread_mutex_lock(&g_param_mutex);
	*dstinfo = g_param.dst_config;
	Pthread_mutex_unlock(&g_param_mutex);
	return 0;
}

int ParamLib_SetNetWorkInfo(network_config_t networkinfo)
{
    if (CommLib_checkIpAddress(networkinfo.eth_ipaddr) == -1)
    {
        return -1;
    }

    if (CommLib_checkIpAddress(networkinfo.eth_subnet_mask) == -1)
    {
        return -1;
    }

    if (CommLib_checkIpAddress(networkinfo.eth_gateway) == -1)
    {
        return -1;
    }
    
    if(memcmp(&g_param.network_config, &networkinfo, sizeof(network_config_t)) !=0)
    {
        Pthread_mutex_lock(&g_param_mutex);
        memcpy(&g_param.network_config, &networkinfo, sizeof(network_config_t));
        g_param_dirty =1;
        Pthread_mutex_unlock(&g_param_mutex);
    }
    
    return 0;
}

int ParamLib_GetNetWorkInfo(network_config_t *networkinfo)
{
	Pthread_mutex_lock(&g_param_mutex);
	*networkinfo = g_param.network_config;
	Pthread_mutex_unlock(&g_param_mutex);
	return 0;
}

void ParamLib_GetWirelessInfo(wireless_config_t *wirelessinfo)
{
	Pthread_mutex_lock(&g_param_mutex);
	*wirelessinfo = g_param.wireless_config;
	Pthread_mutex_unlock(&g_param_mutex);
}

void ParamLib_SetWirelessInfo(wireless_config_t wirelessinfo)
{
	if(memcmp(&g_param.network_config, &wirelessinfo, sizeof(wireless_config_t)) !=0)
	{
		Pthread_mutex_lock(&g_param_mutex);
		memcpy(&g_param.wireless_config, &wirelessinfo, sizeof(wireless_config_t));
		g_param_dirty = 1;
		Pthread_mutex_unlock(&g_param_mutex);
	}
}

int ParamLib_SetPPPOEInfo(pppoe_config_t  pppoeinfo)
{
	if(memcmp(&g_param.pppoe_config, &pppoeinfo, sizeof(pppoe_config_t)) !=0)
	{
		Pthread_mutex_lock(&g_param_mutex);
		memcpy(&g_param.pppoe_config, &pppoeinfo, sizeof(pppoe_config_t));
		g_param_dirty =1;
		Pthread_mutex_unlock(&g_param_mutex);
	}
	return 0;
}

int ParamLib_GetPPPOEInfo(pppoe_config_t * pppoeinfo)
{
	Pthread_mutex_lock(&g_param_mutex);
	*pppoeinfo = g_param.pppoe_config;
	Pthread_mutex_unlock(&g_param_mutex);
	return 0;
}

int ParamLib_SetEmailInfo(email_config_t emailinfo)
{
	if(memcmp(&g_param.email_config, &emailinfo, sizeof(email_config_t)) !=0)
	{
		Pthread_mutex_lock(&g_param_mutex);
		memcpy(&g_param.email_config, &emailinfo, sizeof(email_config_t));
		g_param_dirty =1;
		Pthread_mutex_unlock(&g_param_mutex);
	}
	return 0;
}

int ParamLib_GetEmailInfo(email_config_t *emailinfo)
{
	Pthread_mutex_lock(&g_param_mutex);
	*emailinfo = g_param.email_config;
	Pthread_mutex_unlock(&g_param_mutex);
	return 0;
}

/************************************************************************/
/*                           FTP                                       */
/************************************************************************/
int ParamLib_SetFTPInfo(ftp_config_t ftpinfo)
{
	if(memcmp(&g_param.ftp_config, &ftpinfo, sizeof(ftp_config_t)) !=0)
	{
		Pthread_mutex_lock(&g_param_mutex);
		memcpy(&g_param.ftp_config, &ftpinfo, sizeof(ftp_config_t));
		g_param_dirty =1;
		Pthread_mutex_unlock(&g_param_mutex);
	}
	return 0;
}

int ParamLib_GetFTPInfo(ftp_config_t *ftpinfo)
{
	Pthread_mutex_lock(&g_param_mutex);
	*ftpinfo = g_param.ftp_config;
	Pthread_mutex_unlock(&g_param_mutex);
	return 0;
}

int ParamLib_SetDDNSInfo(ddns_config_t  ddnsinfo)
{
	if(memcmp(&g_param.ddns_config, &ddnsinfo, sizeof(ddns_config_t)) !=0)
	{
		Pthread_mutex_lock(&g_param_mutex);
		printf("ddns_server=%s\n",ddnsinfo.ddns_server);
		memcpy(&g_param.ddns_config, &ddnsinfo, sizeof(ddns_config_t));
		g_param_dirty =1;
		Pthread_mutex_unlock(&g_param_mutex);
	}
	return 0;
}

int ParamLib_GetDDNSInfo(ddns_config_t * ddnsinfo)
{
	Pthread_mutex_lock(&g_param_mutex);
	*ddnsinfo = g_param.ddns_config;
	Pthread_mutex_unlock(&g_param_mutex);
	return 0;
}

int ParamLib_SetAutoMaintain(auto_maintain_t  automaintain)
{
	if(memcmp(&g_param.automaintain, &automaintain, sizeof(auto_maintain_t)) !=0)
	{
		Pthread_mutex_lock(&g_param_mutex);
		memcpy(&g_param.automaintain, &automaintain, sizeof(auto_maintain_t));
		g_param_dirty =1;
		Pthread_mutex_unlock(&g_param_mutex);
	}
	return 0;
}

int ParamLib_GetAutoMaintain(auto_maintain_t *automaintain)
{
	Pthread_mutex_lock(&g_param_mutex);
	*automaintain = g_param.automaintain;
	Pthread_mutex_unlock(&g_param_mutex);
	return 0;
}

int ParamLib_SetSensorInfo(unsigned short channal_num,sensor_config_t  sensorinfo)
{
	assert((channal_num >= 0) && (channal_num < MAX_CHANNEL_NUM));
	if(memcmp(&g_param.sensor_config[channal_num], &sensorinfo, sizeof(sensor_config_t)) != 0)
	{
		Pthread_mutex_lock(&g_param_mutex);
        memcpy(&g_param.sensor_config[channal_num], &sensorinfo, sizeof(sensor_config_t));	
		g_param_dirty = 1;
		Pthread_mutex_unlock(&g_param_mutex);
	}
	return 0;
}

int ParamLib_GetSensorInfo(unsigned short channal_num,sensor_config_t * sensorinfo)
{
	assert((channal_num >= 0) && (channal_num < MAX_CHANNEL_NUM));
	Pthread_mutex_lock(&g_param_mutex);
	*sensorinfo  = g_param.sensor_config[channal_num];
	Pthread_mutex_unlock(&g_param_mutex);
	return 0;
}

int ParamLib_SetIspInfo(unsigned short channal_num,isp_config_t  ispinfo)
{
	if(memcmp(&g_param.isp_config, &ispinfo, sizeof(isp_config_t)) !=0)
	{
		Pthread_mutex_lock(&g_param_mutex);
		memcpy(&g_param.isp_config, &ispinfo, sizeof(isp_config_t));	
		g_param_dirty = 1;
		Pthread_mutex_unlock(&g_param_mutex);
	}
	return 0;
}

int ParamLib_GetIspInfo(unsigned short channal_num,isp_config_t *ispinfo)
{
	assert((channal_num >= 0) && (channal_num < MAX_CHANNEL_NUM));
	Pthread_mutex_lock(&g_param_mutex);
	*ispinfo  = g_param.isp_config;
	Pthread_mutex_unlock(&g_param_mutex);
	return 0;
}

int ParamLib_SetDayNightSchedule(unsigned short channal_num,daynight_schedule_t daynightschedule)
{
	assert((channal_num >= 0) && (channal_num < MAX_CHANNEL_NUM));
	if(memcmp(&g_param.daynight_schedule[channal_num], &daynightschedule, sizeof(daynight_schedule_t)) != 0)
	{
		Pthread_mutex_lock(&g_param_mutex);
		memcpy(&g_param.daynight_schedule[channal_num], &daynightschedule, sizeof(daynight_schedule_t));	
		g_param_dirty = 1;
		Pthread_mutex_unlock(&g_param_mutex);
	}
	return 0;
}

int ParamLib_GetDayNightSchedule(unsigned short channal_num,daynight_schedule_t * daynightschedule)
{
	assert((channal_num >= 0) && (channal_num < MAX_CHANNEL_NUM));
	Pthread_mutex_lock(&g_param_mutex);
	*daynightschedule  = g_param.daynight_schedule[channal_num];
	Pthread_mutex_unlock(&g_param_mutex);
	return 0;
}

int ParamLib_SetVEncInfo(unsigned short channal_num,v_enc_config_t  v_encinfo)
{
	assert((channal_num >= 0) && (channal_num < MAX_CHANNEL_NUM));
	if(memcmp(&g_param.v_enc_config[channal_num], &v_encinfo, sizeof(v_enc_config_t)) !=0)
	{
		Pthread_mutex_lock(&g_param_mutex);
		memcpy(&g_param.v_enc_config[channal_num], &v_encinfo, sizeof(v_enc_config_t));	
		g_param_dirty = 1;
		Pthread_mutex_unlock(&g_param_mutex);
	}
	return 0;
}

int ParamLib_GetVEncInfo(unsigned short channal_num,v_enc_config_t * v_encinfo)
{
	assert((channal_num >= 0) && (channal_num < MAX_CHANNEL_NUM));
	Pthread_mutex_lock(&g_param_mutex);
	*v_encinfo  = g_param.v_enc_config[channal_num];
	Pthread_mutex_unlock(&g_param_mutex);
	return 0;
}

int ParamLib_SetSVEncInfo(unsigned short channal_num,v_s_enc_config_t  v_s_encinfo)
{
	assert((channal_num >= 0) && (channal_num < MAX_CHANNEL_NUM));
	if(memcmp(&g_param.v_s_enc_config[channal_num], &v_s_encinfo, sizeof(v_s_enc_config_t)) !=0)
	{
		Pthread_mutex_lock(&g_param_mutex);
		memcpy(&g_param.v_s_enc_config[channal_num], &v_s_encinfo, sizeof(v_s_enc_config_t));	
		g_param_dirty = 1;
		Pthread_mutex_unlock(&g_param_mutex);
	}
	return 0;
}

int ParamLib_GetSVEncInfo(unsigned short channal_num,v_s_enc_config_t * v_s_encinfo)
{
	assert((channal_num >= 0) && (channal_num < MAX_CHANNEL_NUM));
	Pthread_mutex_lock(&g_param_mutex);
	*v_s_encinfo  = g_param.v_s_enc_config[channal_num];
	Pthread_mutex_unlock(&g_param_mutex);
	return 0;
}

int ParamLib_SetAEncInfo(unsigned short channal_num,a_enc_config_t  a_encinfo)
{
	assert((channal_num >= 0) && (channal_num < MAX_CHANNEL_NUM));
	if(memcmp(&g_param.a_enc_config[channal_num], &a_encinfo, sizeof(a_enc_config_t)) !=0)
	{
		Pthread_mutex_lock(&g_param_mutex);
		memcpy(&g_param.a_enc_config[channal_num], &a_encinfo, sizeof(a_enc_config_t));
		g_param_dirty = 1;
		Pthread_mutex_unlock(&g_param_mutex);
	}
	return 0;
}

int ParamLib_GetAEncInfo(unsigned short channal_num,a_enc_config_t * a_encinfo)
{
	assert((channal_num >= 0) && (channal_num < MAX_CHANNEL_NUM));
	Pthread_mutex_lock(&g_param_mutex);
	*a_encinfo  = g_param.a_enc_config[channal_num];
	Pthread_mutex_unlock(&g_param_mutex);
	return 0;
}

int ParamLib_SetOSDInfo(unsigned short channal_num,osd_config_t  osdinfo)
{
	assert((channal_num >= 0) && (channal_num < MAX_CHANNEL_NUM));
	if(memcmp(&g_param.osd_config[channal_num], &osdinfo, sizeof(osd_config_t)) !=0)
	{
		Pthread_mutex_lock(&g_param_mutex);
		memcpy(&g_param.osd_config[channal_num], &osdinfo, sizeof(osd_config_t));
			g_param_dirty = 1;
		Pthread_mutex_unlock(&g_param_mutex);
	}
	return 0;
}

int ParamLib_GetOSDInfo(unsigned short channal_num,osd_config_t * osdinfo)
{
	assert((channal_num >= 0) && (channal_num < MAX_CHANNEL_NUM));
	Pthread_mutex_lock(&g_param_mutex);
	*osdinfo  = g_param.osd_config[channal_num];
	Pthread_mutex_unlock(&g_param_mutex);
	return 0;
}

int ParamLib_SetOSDInfo_Ex(unsigned short channal_num, osd_config_t_EX osdinfo)
{
	assert((channal_num >= 0) && (channal_num < MAX_CHANNEL_NUM));
	if(memcmp(&g_param.osd_config_ex[channal_num], &osdinfo, sizeof(osd_config_t_EX)) !=0)
	{
		Pthread_mutex_lock(&g_param_mutex);
		memcpy(&g_param.osd_config_ex[channal_num], &osdinfo, sizeof(osd_config_t_EX));
		g_param_dirty = 1;
		Pthread_mutex_unlock(&g_param_mutex);
	}
	return 0;
}

int ParamLib_GetOSDInfo_Ex(unsigned short channal_num, osd_config_t_EX *osdinfo)
{
	assert((channal_num >= 0) && (channal_num < MAX_CHANNEL_NUM));
	Pthread_mutex_lock(&g_param_mutex);
	*osdinfo  = g_param.osd_config_ex[channal_num];
	Pthread_mutex_unlock(&g_param_mutex);
	return 0;
}

int ParamLib_SetOverlayInfo(unsigned short channal_num,unsigned short overlayno,overlay_config_t  Overlayinfo)
{
	assert((channal_num >= 0) && (channal_num < MAX_CHANNEL_NUM));
	if(memcmp(&g_param.overlay_config[channal_num][overlayno], &Overlayinfo, sizeof(overlay_config_t)) !=0)
	{
		Pthread_mutex_lock(&g_param_mutex);
		memcpy(&g_param.overlay_config[channal_num][overlayno], &Overlayinfo, sizeof(overlay_config_t));
			g_param_dirty = 1;
		Pthread_mutex_unlock(&g_param_mutex);
	}
	return 0;
}

int ParamLib_GetOverlayInfo(unsigned short channal_num,unsigned short overlayno,overlay_config_t * Overlayinfo)
{ 
	if (overlayno > 3)
	{
		return -1;
	}
	assert((channal_num >= 0) && (channal_num < MAX_CHANNEL_NUM));
	Pthread_mutex_lock(&g_param_mutex);
	*Overlayinfo  = g_param.overlay_config[channal_num][overlayno];
	Pthread_mutex_unlock(&g_param_mutex);
	return 0;
}

int ParamLib_SetMDInfo(unsigned short channal_num,md_config_t  mdinfo)
{
	if(memcmp(&g_param.md_config[channal_num], &mdinfo, sizeof(md_config_t)) !=0)
	{
		Pthread_mutex_lock(&g_param_mutex);
		memcpy(&g_param.md_config[channal_num], &mdinfo, sizeof(md_config_t));
		g_param_dirty =1;
		Pthread_mutex_unlock(&g_param_mutex);
	}
	return 0;

	
}

int ParamLib_GetMDInfo(unsigned short channal_num,md_config_t * mdinfo)
{
	assert((channal_num >= 0) && (channal_num < MAX_CHANNEL_NUM));
	Pthread_mutex_lock(&g_param_mutex);
	*mdinfo  = g_param.md_config[channal_num];
	Pthread_mutex_unlock(&g_param_mutex);
	return 0;
}

int ParamLib_SetIOAlarmInfo(unsigned short channal_num,io_config_t ioinfo)
{
	if(memcmp(&g_param.md_config[channal_num], &ioinfo, sizeof(io_config_t)) !=0)
	{
		Pthread_mutex_lock(&g_param_mutex);
		memcpy(&g_param.io_config[channal_num], &ioinfo, sizeof(io_config_t));
		g_param_dirty =1;
		Pthread_mutex_unlock(&g_param_mutex);
	}
	return 0;
}

int ParamLib_GetIOAlarmInfo(unsigned short channal_num,io_config_t * ioinfo)
{
	assert((channal_num >= 0) && (channal_num < MAX_CHANNEL_NUM));
	Pthread_mutex_lock(&g_param_mutex);
	*ioinfo  = g_param.io_config[channal_num];
	Pthread_mutex_unlock(&g_param_mutex);
	return 0;
}

int ParamLib_SetRecordBaseInfo(recordbase_config_t  recordbaseinfo)
{
	if(memcmp(&g_param.recordbase_config, &recordbaseinfo, sizeof(recordbase_config_t)) !=0)
	{
		Pthread_mutex_lock(&g_param_mutex);
		memcpy(&g_param.recordbase_config, &recordbaseinfo, sizeof(recordbase_config_t));
		g_param_dirty =1;
		Pthread_mutex_unlock(&g_param_mutex);
	}
	return 0;
}

int ParamLib_GetRecordBaseInfo(recordbase_config_t * recordbaseinfo)
{
	Pthread_mutex_lock(&g_param_mutex);
	*recordbaseinfo = g_param.recordbase_config;
	Pthread_mutex_unlock(&g_param_mutex);
	return 0;
}

void ParamLib_GetHandRecordStruct(int channal_num, hand_record_t *handrecord)
{
	assert(channal_num>=0 && channal_num<MAX_CHANNEL_NUM);
	Pthread_mutex_lock(&g_param_mutex);
	*handrecord =g_param.hand_record[channal_num];
	Pthread_mutex_unlock(&g_param_mutex);
}

void ParamLib_SetHandRecordStruct(int channal_num, hand_record_t handrecord)
{
	assert(channal_num>=0 && channal_num<MAX_CHANNEL_NUM);
	Pthread_mutex_lock(&g_param_mutex);
	if(memcmp(&g_param.hand_record[channal_num], &handrecord, sizeof(handrecord)) != 0)
	{
	    g_param.hand_record[channal_num] = handrecord;
	}
	Pthread_mutex_unlock(&g_param_mutex);
}

int ParamLib_SetRecordScheduleInfo(unsigned short channal_num,recordschedule_config_t  recordscheduleinfo)
{
	assert((channal_num >= 0) && (channal_num < MAX_CHANNEL_NUM));
	if(memcmp(&g_param.recordschedule_config[channal_num], &recordscheduleinfo, sizeof(recordschedule_config_t)) !=0)
	{
		Pthread_mutex_lock(&g_param_mutex);
		memcpy(&g_param.recordschedule_config[channal_num], &recordscheduleinfo, sizeof(recordschedule_config_t));
			g_param_dirty = 1;
		Pthread_mutex_unlock(&g_param_mutex);
	}
	return 0;
}

int ParamLib_GetSystemInfo(system_Info_t *systeminfo)
{
	Pthread_mutex_lock(&g_param_mutex);
	*systeminfo = g_param.systeminfo;
	Pthread_mutex_unlock(&g_param_mutex);
	return 0;
}

int ParamLib_SetSystemInfo(system_Info_t systeminfo)
{
	if(memcmp(&g_param.systeminfo, &systeminfo, sizeof(system_Info_t)) !=0)
	{
		Pthread_mutex_lock(&g_param_mutex);
		memcpy(&g_param.systeminfo, &systeminfo, sizeof(system_Info_t));
		g_param_dirty = 1;
		Pthread_mutex_unlock(&g_param_mutex);
	}
	return 0;
}

int ParamLib_GetRecordScheduleInfo(unsigned short channal_num,recordschedule_config_t * recordscheduleinfo)
{
	assert((channal_num >= 0) && (channal_num < MAX_CHANNEL_NUM));
	Pthread_mutex_lock(&g_param_mutex);
	*recordscheduleinfo  = g_param.recordschedule_config[channal_num];
	Pthread_mutex_unlock(&g_param_mutex);
	return 0;
}

int ParamLib_SetPTZInfo(ptz_config_t  ptzinfo)
{
	if(memcmp(&g_param.ptz_config, &ptzinfo, sizeof(ptz_config_t)) !=0)
	{
		Pthread_mutex_lock(&g_param_mutex);
		memcpy(&g_param.ptz_config, &ptzinfo, sizeof(ptz_config_t));
		g_param_dirty =1;
		Pthread_mutex_unlock(&g_param_mutex);
	}
	return 0;
}

int ParamLib_GetPTZInfo(ptz_config_t * ptzinfo)
{
	Pthread_mutex_lock(&g_param_mutex);
	*ptzinfo = g_param.ptz_config;
	Pthread_mutex_unlock(&g_param_mutex);
	return 0;
}

int ParamLib_SetSIPInfo(sip_config_t  sipinfo)
{
	if(memcmp(&g_param.sip_config, &sipinfo, sizeof(sip_config_t)) !=0)
	{
		Pthread_mutex_lock(&g_param_mutex);
		memcpy(&g_param.sip_config, &sipinfo, sizeof(sip_config_t));
		g_param_dirty =1;
		Pthread_mutex_unlock(&g_param_mutex);
	}
	return 0;
}

int ParamLib_GetSIPInfo(sip_config_t * sipinfo)
{
	Pthread_mutex_lock(&g_param_mutex);
	*sipinfo = g_param.sip_config;
	Pthread_mutex_unlock(&g_param_mutex);
	return 0;
}

int  ParamLib_SetUserInfo(char *userinfo)
{
	if(memcmp(&g_param.ddns_config, &userinfo, sizeof(user_config_t)) !=0)
	{
		Pthread_mutex_lock(&g_param_mutex);
		memcpy(g_param.user_config, userinfo, sizeof(user_config_t)*20);
		g_param_dirty =1;
		Pthread_mutex_unlock(&g_param_mutex);
	}
	return 0;
}

int ParamLib_GetUserInfo(char *userinfo)
{
	Pthread_mutex_lock(&g_param_mutex);	
    memcpy(userinfo,g_param.user_config,sizeof(user_config_t)*20);
	Pthread_mutex_unlock(&g_param_mutex);
    return 0;
}

int ParamLib_GetPortInfo(network_port_config_t * portinfo)
{
	Pthread_mutex_lock(&g_param_mutex);
	*portinfo = g_param.network_port_config;
	Pthread_mutex_unlock(&g_param_mutex);
	return 0;
}

int ParamLib_SetPortInfo(network_port_config_t  portinfo)
{
	if(memcmp(&g_param.network_port_config, &portinfo, sizeof(network_port_config_t)) !=0)
	{
		Pthread_mutex_lock(&g_param_mutex);
		memcpy(&g_param.network_port_config, &portinfo, sizeof(network_port_config_t));
		g_param_dirty =1;
		Pthread_mutex_unlock(&g_param_mutex);
	}
	return 0;
}


int ParamLib_GetSysConfig(sys_config_t *sysconfig)
{
	Pthread_mutex_lock(&g_param_mutex);
	*sysconfig = g_param.sysconfig;
	Pthread_mutex_unlock(&g_param_mutex);
	return 0;
}

int ParamLib_SetSysConfig(sys_config_t sysconfig)
{
	assert(sysconfig.channel_num <= MAX_CHANNEL_NUM);
	if(memcmp(&g_param.sysconfig, &sysconfig, sizeof(sysconfig))!=0)
	{
		Pthread_mutex_lock(&g_param_mutex);
		memcpy(&g_param.sysconfig, &sysconfig, sizeof(sys_config_t));
		g_param_dirty =1;
		Pthread_mutex_unlock(&g_param_mutex);	
	}
	return 0;
}

int ParamLib_BackUpParam(void)
{
	char buff[128];
	memset(buff,0,128);
	sprintf(buff,"cat %s >> %s", RUN_PARAM_FILE, BACKUP_PARAM_FILE);
	system(buff);
	system("reboot");
	return 0;
}


int ParamLib_ResumeParam(void)
{  
	char buff[128];
	memset(buff,0,128);
	sprintf(buff, "cat %s >> %s", BACKUP_PARAM_FILE, RUN_PARAM_FILE);
	system(buff);
	system("reboot");
	return 0;
}

int ParamLib_ResumeFactoryParam(void)
{  
	Pthread_mutex_lock(&g_param_mutex);
	SoftwareDefaultParam();
	g_param_dirty = 1;
	Pthread_mutex_unlock(&g_param_mutex);   
	usleep(2000*1000);
	system("reboot");
	return 0;
}

int ParamLib_GetParamInfo(param *paraminfo)
{
	Pthread_mutex_lock(&g_param_mutex);
	memcpy(paraminfo,&g_param,sizeof(param));
	paraminfo->end = sizeof(param);
	printf("paraminfo->end=%d\n", sizeof(param));
	Pthread_mutex_unlock(&g_param_mutex);
	return 0;
}

int ParamLib_SetParamInfo(param paraminfo)
{
    Pthread_mutex_lock(&g_param_mutex);
    memcpy(&(paraminfo.network_config), &(g_param.network_config), sizeof(network_config_t));
    memcpy(&(paraminfo.sip_config), &(g_param.sip_config), sizeof(sip_config_t));
    memcpy(&g_param, &paraminfo, sizeof(param));
    FILE *pf = fopen(RUN_PARAM_FILE, "wb");
    if (pf != NULL)
    {
        fwrite(&paraminfo, 1, sizeof(param), pf);
        fclose(pf);
        printf("write the param to the %s.\n", RUN_PARAM_FILE);
        //system("reboot");
    }	
    Pthread_mutex_unlock(&g_param_mutex);	
	
    return 0;
}

int ParamLib_GetVEncInfo_EX(unsigned short channal_num,unsigned streamtype,v_enc_config_t *v_encinfo)
{
    if (streamtype == 0)
    {
		assert((channal_num >= 0) && (channal_num < MAX_CHANNEL_NUM));
		Pthread_mutex_lock(&g_param_mutex);
		*v_encinfo  = g_param.v_enc_config[channal_num];
		Pthread_mutex_unlock(&g_param_mutex);
    }
    else
    {
    	v_s_enc_config_t v_s_encinfo;
    	assert((channal_num >= 0) && (channal_num < MAX_CHANNEL_NUM));
    	Pthread_mutex_lock(&g_param_mutex);
    	v_s_encinfo  = g_param.v_s_enc_config[channal_num];
    	Pthread_mutex_unlock(&g_param_mutex);  
        v_encinfo->BaseImageQuality = v_s_encinfo.bitrate_control;
    	v_encinfo->bitrate = v_s_encinfo.bitrate;
    	v_encinfo->bitrate_control = v_s_encinfo.bitrate_control;
    	v_encinfo->encmodle = v_s_encinfo.encmodle;
    	v_encinfo->fps = v_s_encinfo.fps;
    	v_encinfo->IFrameInterval = v_s_encinfo.IFrameInterval;
    	v_encinfo->resolution = v_s_encinfo.resolution;	
    }
	
	return 0;
}

int ParamLib_SetGB28181Info(gb28181_config_t  gb28181_info)
{
	if(memcmp(&g_param.gb28181_config, &gb28181_info, sizeof(gb28181_config_t)) !=0)
	{
		Pthread_mutex_lock(&g_param_mutex);
		memcpy(&g_param.gb28181_config, &gb28181_info, sizeof(gb28181_config_t));
		g_param_dirty = 1;
		Pthread_mutex_unlock(&g_param_mutex);
	}
	return 0;
}

int ParamLib_GetGB28181Info(gb28181_config_t * gb28181_info)
{
	Pthread_mutex_lock(&g_param_mutex);
	*gb28181_info = g_param.gb28181_config;
	Pthread_mutex_unlock(&g_param_mutex);
	return 0;
}

int ParamLib_SetAlarmInfo(const alarm_config_t * alarm_info)
{
	Pthread_mutex_lock(&g_param_mutex);
	g_param.alarm_config = *alarm_info;
	g_param_dirty = 1;
	Pthread_mutex_unlock(&g_param_mutex);
	return 0;
}

int ParamLib_GetAlarmInfo(alarm_config_t * alarm_info)
{
	Pthread_mutex_lock(&g_param_mutex);
	*alarm_info = g_param.alarm_config;
	Pthread_mutex_unlock(&g_param_mutex);
	return 0;
}

int ParamLib_GetCompileDateTime(char *szDate, char *szTime)
{        
    const char szEnglishMonth[12][4] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
    char szTmpDate[16] = {0};
    char szMonth[4] = {0};
    int iYear,iMonth,iDay;

    if (szDate != NULL) {
        sprintf(szTmpDate, "%s", __DATE__); //"Sep 18 2010"
        sscanf(szTmpDate,"%s %d %d", szMonth, &iDay, &iYear);
        int i;
        for (i = 0; i < 12; i++) {
            if (strncmp(szMonth, szEnglishMonth[i], 3) == 0) {
                iMonth = i + 1;
                break;
            }
        }
        sprintf(szDate, "%04d-%02d-%02d", iYear, iMonth, iDay);
    }

    if (szTime != NULL) {
        sprintf(szTime, "%s", __TIME__); //"10:59:19"
    }

    return 0;
}


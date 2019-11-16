#ifndef __PARAMLIB_H__
#define __PARAMLIB_H__

#define MAX_CHANNEL_NUM 1
#define FLAG_OFFSET    20  //参数库版本号的字符长度  

#define RUN_PARAM_FILE      "/opt/param/ipc_run.conf"
#define BACKUP_PARAM_FILE   "/etc/ipc_run.conf"
#define FACTORY_PARAM_FILE  "/opt/param/ipclog.conf"
#define PORT_PARAM_FILE     "/opt/user/www/cgi-bin/port.conf"
#define ALTUM_PARAM_FILE    "/opt/param/altum.conf"
#define VERSION_LEN 32

//TI-IPC
#define DEV_TYPE_TIIPC_365 0x53460000
#define DEV_TYPE_GM_8126 0x5346474D
typedef struct sys_config_t
{
	unsigned int	    dev_type;	/* 设备类型 区分方案 此方案为Hisi3511 以区别于 Hisi3510 */
	unsigned char		chip_num;   /*芯片的数量*/
	unsigned char		channel_num;/*设备的通道数*/
	unsigned char		prober_num; /*告警输入口数*/
	unsigned char		output_num; /*告警输出入口数*/
	unsigned char		adchip_type;/* 0:no ad chip 1:tvp5150  2:TW2815 */
	unsigned char		encode_format;/*编码格式  0：PAL  1：N*/
	unsigned char		language; /*语言  0：英文  1：中文*/
	unsigned char 		serial_num;
	char                version[64]; //版本信息
	char                devicename[64];
	unsigned char		unused2[4];
}sys_config_t;

/*
时间同步方式	time_sync	unsigned short	(0:ntp, 1:manual)	ntp	R/W
系统日期和时间	datetime	Char 	(yyyy-MM-dd hh:mm:ss)	不适用	R/W
NTP服务器地址	ntp_server	char	　	198.123.30.132	R/W
时区	time_zone	unsigned short	(0:GMT, 1:GMT1,2:GMT2,3:GMT-1,4:GMT-2 …)	GMT8	R/W
*/

typedef struct datetime_config_t
{
	unsigned short	  time_sync;	
	char		      datetime[32];
	char		      ntp_server[32];	
	unsigned short	  time_zone;	
}datetime_config_t;

typedef struct DST_config_t
{
	unsigned short	  enable;	
	unsigned short	  timetype;
	unsigned short    offsettime;
	char		      starttime[32];	
	char	          endtime[32];	
	char              unuse[64];
}DST_config_t;

/*
网卡类型        eth_type               unsigned short     0:有线网络，1：无线网络
IP版本          eth_protocol           unsigned short	  0:IPV4，1:IPV6	
MAC 地址	    eth_mac                char	              (XX:XX:XX:XX:XX:XX)	ntp	R/W
IP地址获取模式	eth_ipmode	           unsigned short	  0:static,1:dhcp	不适用	R/W
静态IP地址	    eth_ipaddr	           char	　	          198.123.30.132	R/W
子网掩码	    eth_subnet_mask	       char	　	          GMT8	R/W
网关地址	    eth_gateway	           char	　		
DNS服务器地址	dns_server_f	       char	　		      首选DNS服务器
DNS服务器地址	dns_server_s	       char	              备用DNS服务器     
DNS服务器使能	dns_enable	           unsigned short	(1,0)		
*/
typedef struct network_config_t
{
	unsigned short 	      eth_type;
	unsigned short 	      eth_protocol;
	char	              eth_mac[32];	
	unsigned short 	      eth_ipmode;
	char		          eth_ipaddr[32];	
	char 	              eth_subnet_mask[32];	
	char		          eth_gateway [32];
	char		          dns_server1[32];	
	char	              dns_server2[32];	
	unsigned short	      dns_enable;	
	unsigned short	      dhcp_flag;
} network_config_t;

typedef struct __wireless_wep_password_t
{
    unsigned char safety_option;    /* WEP安全选项(1->AUTO, 2->OPEN, 3->SHARE) */
    unsigned char passwd_format;    /* 密钥格式(ASCII, HEX)*/
    unsigned char key_numb;         /* 频率 通道号 */
    unsigned char unused;
    unsigned char password[4][32];  /* 密码 */
    unsigned char key_type[4];      /* 密钥类型(64秘钥hex(10)/ascii(5), 128秘钥hex(26)/ascii(13), 152秘钥hex(32)/ascii(16)) */
}wireless_wep_password_t;

typedef struct __wireless_scanning_t
{
    int current_used;   /* 1:enable, 0:disable */
    char essid[64];     /* ssid */
    int signalStrength; /* signal leval */
    int isEncrypt;      /* 0->noEncrypt 1->encrypt */
    int safety_type;    /* 1->WEP  2->WPA-PSK  3->WPA2-PSK */
    int safety_option;  /* WEP(1->auto, 2->open 3->share), WPA/WPA2-PSK(0->AES, 1->TKIP) */
    int key_numb;       /* iwlistLib.c-> freq channel no, iwpriv.c->(0->AES, 1->TKIP) */
    char password[96];  /* passwd */
}wireless_scanning_t;

typedef struct __wireless_config_t
{
    int enable;             /* 1:enable, 0:disable */
    char essid[64];         /* ssid */
    int signalStrength;     /* signal leval */
    int state;              /* wifi connect state */
    int encrypt_type;       /* 0->nokey  1->WEP  2->WPA-PSK  3->WPA2-PSK */
    int encrypt_option;     /* WEP(0->auto, 1->open 2->share), WPA/WPA2-PSK(0->AES, 1->TKIP) */
    int key_numb;           /* iwlistLib.c-> freq channel no, iwpriv.c->(0->AES, 1->TKIP) */
    char password[96];      /* passwd */
    
    unsigned short eth_ipmode;  /* dhcp */
    unsigned short eth_type;    /* ipv4, ipv6*/
    char eth_ipaddr[32];        /* ip */
    char eth_subnet_mask[32];   /* netmask */
    char eth_gateway[32];       /* gateway */
    char dns_server1[32];       /* dns1 */
    char dns_server2[32];       /* dns2 */
    unsigned short dns_enable;  /* dns enable */
}wireless_config_t;

/*
网络的通信接口	netserver_port	unsigned short	
最大连接数	    maxlink     	unsigned short 	
rtsp端口	    rtspport	    unsigned short	　
web端口        	httppport	    unsigned short	
*/
typedef struct network_port_config_t
{
	unsigned short	   netserver_port;//网络的通信接口：9060
	unsigned short		maxlink;   /*最大连接数*/
	unsigned short		rtspport;	/*rtsp端口*/
	unsigned short		httppport;	 /*web端口*/ 
	unsigned short		onvifport;
}network_port_config_t;

/*
是否启用邮件功能 	    email_enable	        unsigned short   	
发送邮件服务器	        email_serverIP	        Char	 
发送人地址              email_fromAddr          Char
发送人邮箱密码	        email_password	        Char	　		
发件人名称              email_userName          Char
接收人地址              email_toAddr            Char
邮件主题	            email_subject	        Char	　     
登陆名                  email_loginName         Char  
端口                    email_port              int
*/
typedef struct __email_config_t
{
	unsigned short email_enable;	        /* 1:允许EMAIL  0: 禁止EMAIL */
	char           email_serverIP[64];	    /* 发送邮件服务器 */
	char           email_fromAddr[64];	    /* 发送人地址 */
	char           email_password[32];	    /* 发送人邮箱密码*/
	char           email_userName[32];	    /* 发件人名称 */
	char           email_toAddr[256];	    /* 接收人地址 */	
	char           email_subject[64];       /* 邮件主题 */
	char           email_loginName[32];     /* 登陆名 */
	unsigned short email_port;
}email_config_t;

/*
是否启用FTP 	ftp_enable	    unsigned short	
FTP 用户	    ftp_user	    Char		
FTP 密码	    ftp_password	Char	　		
服务器路径	    ftp_directory	Char	　     ftp://主机名:端口/路径/ 
预留字段        unused          char
*/
typedef struct ftp_config_t
{
	char ftp_enable;	
	char ftp_username[64];
	char ftp_password[64];
	char ftp_directory[256];	
}ftp_config_t;

/*
是否启用PPPOE 	pppoe_enable	unsigned short	(1,0)	ntp	R/W
PPPOE 用户	pppoe_user	Char	(英文字母, 数字, 下划线)	不适用	R/W
PPPOE 密码	pppoe_password	Char	　	198.123.30.132	R/W
PPPOE 连接状态	pppoe_status	Char	　拨号成功显示公网IP	GMT8	R/W
*/
typedef struct pppoe_config_t
{
	unsigned short 	   pppoe_enable;
	char		       pppoe_user [32];	
	char 	           pppoe_password[32];	
	char		       pppoe_status[32];
} pppoe_config_t;

/*
是否启用 DDNS 	ddns_enable	unsigned short	(1,0)	ntp	R/W
DDNS 服务器地址	ddns_server	Char	IP 地址或域名	不适用	R/W
DDNS 用户名	ddns_user	Char	(英文字母, 数字, 下划线)	198.123.30.132	R/W
DDNS 密码	ddns_password	Char	(英文字母, 数字, 下划线)	GMT8	R/W
*/
typedef struct ddns_config_t
{
	unsigned short 	   ddns_enable;
	char		       ddns_server [32];	
	char 	           ddns_user [32];	
	char		       ddns_password [32];
}ddns_config_t;

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
typedef struct sensor_config_t
{
    unsigned short brightness;	
    unsigned short contrast;	
    unsigned short hue;
    unsigned short saturation;	
    unsigned short sharpness;
    unsigned short daynight;	
    unsigned short whitebalance;
    unsigned short backlight;
    unsigned short imagesource;
    unsigned short mirrorimage;
    unsigned short flip;
    unsigned short video_standard;
    unsigned short ir_cut;
    unsigned short light_sensor;
}sensor_config_t;

/*
背光补偿开关    weightingEnable     默认值20    参数已经废弃
背光补偿选项    weightingMode       默认值6     [1,6]->(up down left right center all)
曝光模式        aeType              默认值0     0->自动 1->手动
手动曝光时间    aeExpStrength       默认值13    [1,18] 1->1/100000, 2->1/10000, 3->1/4000, 4->1/2000, 5->1/1000, 6->1/750, 7->1/500, 8->1/250, 9->1/200, 10->1/150, 50HZ取这三个值(11->1/100, 12->1/50, 13->1/25), 60HZ取这三个值(14->1/120, 15->1/60, 16->1/30), 17->1/16, 18->1/3
手动曝光增益    aeExpCompensation   默认值128   [0,255]
降噪模式        denoiseType         默认值0     0为自动 1为手动
手动空域降噪    SfStrength          默认值128   [0,255]
手动时域降噪    TfStrength          默认值128   [0,255]
I帧平滑         IframeStrength      默认值5     [0,20]  参数已经废弃
去雾开关        defogEnable         默认值0     0为关闭 1为打开
去雾强度        defogStrength       默认值128   [0,255]
防抖开关        disEnable           默认值0     0为关闭 1为打开
*/
typedef struct isp_config_t
{
  unsigned short weightingEnable;
  unsigned short weightingMode;
  unsigned short aeType;
  unsigned short aeExpStrength;
  unsigned short aeExpCompensation;
  unsigned short denoiseType;
  unsigned short SfStrength;
  unsigned short TfStrength;
  unsigned short IframeStrength;
  unsigned short defogEnable;
  unsigned short defogStrength;
  unsigned short disEnable;
  unsigned char reserve1[122];
}isp_config_t;

/*
开始时间        starttime   char      
结束时间        endtime     char
预留1           reserve1    char 
预留2           reserve2    char 
*/
typedef struct daynight_schedule_t
{
	char		        strattime[32];	
	char		        endtime[32];	
	char		        reserve1[32];	
	char		        reserve2[32];	
}daynight_schedule_t;

/*
编码格式        encmodle          unsigned short    0:H264;1:MPEG4；2:JPEG
编码级别        enclevel          unsigned short    0:Baseline,1:Main,2:High
分辨率	         resolution        unsigned short	(QCIF = 0,qqvag(160*120) = 1,qvga(320*240) = 2,VGA(640*480) = 3,Qcif(176*144) = 4,Cif(352*288)= 5,D1(720*576) = 6,720P(1280*720) = 7,1080P(1960*1080) = 8,960p(1280*960) = 9)	ntp	R/W
帧率	        fps	              unsigned short	1~25 (PAL), 1~30 (NTSC)	不适用	R/W
码流	        bitrate	          unsigned short	16~8000kbps	198.123.30.132	R/W
码流控制方式	bitrate_control	  unsigned short	CBR = 0, VBR = 1	GMT8	R/W
图像质量级别	BaseImageQuality  unsigned short	22~39		
I帧间隔	        IFrameInterval	  unsigned short	I帧间隔：1~300	
*/
typedef struct v_enc_config_t
{
	unsigned short		encmodle;	
	unsigned short      enclevel;
	unsigned short		resolution;	
	unsigned short	    fps;	
	unsigned short		bitrate;
	unsigned short	    bitrate_control;	
	unsigned short		BaseImageQuality;
	unsigned short		IFrameInterval;	
}v_enc_config_t;

/*
从码流
是否启动从码流   ds_enable_flag    unsigned short
编码格式         encmodle          unsigned short   0:H264;1:MPEG4；2:JPEG
编码级别         enclevel           unsigned short  0:Baseline,1:Main,2:High(h264专用)
分辨率	         resolution        unsigned short	(QCIF = 0,qqvag(160*120) = 1,qvga(320*240) = 2,VGA(640*480) = 3,Qcif(176*144) = 4,Cif(352*288)= 5,D1(720*576) = 6,720P(1280*720) = 7,1080P(1960*1080) = 8,960p(1280*960) = 9)	ntp	R/W
帧率	         fps	           unsigned short	1~25 (PAL), 1~30 (NTSC)	不适用	R/W
码流	         bitrate	       unsigned short	16~8000kbps	198.123.30.132	R/W
码流控制方式	 bitrate_control   unsigned short	CBR = 0, VBR = 1	GMT8	R/W
图像质量级别	 BaseImageQuality  unsigned short	22~39		
I帧间隔	         IFrameInterval	   unsigned short	I帧间隔：1~300	
*/
typedef struct v_s_enc_config_t
{
	unsigned char       ds_enable_flag; //双码流开启标志
	unsigned short		encmodle;
	unsigned short      enclevel;
	unsigned short		resolution;	
	unsigned short	    fps;	
	unsigned short		bitrate;
	unsigned short	    bitrate_control;	
	unsigned short		BaseImageQuality;
	unsigned short		IFrameInterval;	
}v_s_enc_config_t;

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
typedef struct a_enc_config_t
{
	unsigned short	    audio_enable;
	unsigned short	    audio_format;	
	unsigned short		audio_sample;
	unsigned short	    audio_channels;	
	unsigned short		audio_bits;
	unsigned short      audio_inputvol;
	unsigned short      audio_outputvol;
	unsigned short      audio_micspeaker;
}a_enc_config_t;

/*
是否启用OSD	    osd_enable	     unsigned short	(1,0)	ntp	R/W
通道号	        channal_num	     unsigned short	　		
OSD 文本	    osd_text	     char	　	不适用	R/W
OSD X位置	    osd_x	         unsigned short	　	198.123.30.132	R/W
OSD Y位置	    osd_y	         unsigned short	　	GMT8	R/W
OSD 颜色	    osd_color	     unsigned short	0:black, 1:white, 2:red, 3:green,4: blue,5: yellow…		
显示日期和时间	time_enable	     unsigned short	(1,0)		
显示时间 X位置	time_x	         unsigned short	　		
显示时间 y位置	time_y	         unsigned short	　		
time 颜色	    time_color	     unsigned short	0:black, 1:white, 2:red, 3:green,4: blue,5: yellow…
水印功能使能    watermark_enable unsigned short  (1,0)
水印内容        watermake_text   char    
*/
typedef struct osd_config_t
{
	unsigned short 	    osd_enable;
	char                osd_text[128];	
	unsigned short		osd_x;
	unsigned short	    osd_y;	
	unsigned short		osd_color;
	unsigned short		time_enable;	
	unsigned short	    time_x;	
	unsigned short		time_y;
	unsigned short	    time_color;	
	unsigned short      watermark_enable;
	char                watermake_text[64];
}osd_config_t;

typedef struct osd_config_t_EX
{
	unsigned short 	    osd1_enable;
	char                osd1_text[128];	
	unsigned short		osd1_x;
	unsigned short	    osd1_y;	
	unsigned short		osd1_color;
	unsigned short 	    osd2_enable;
	char                osd2_text[128];	
	unsigned short		osd2_x;
	unsigned short	    osd2_y;	
	unsigned short		osd2_color;
	unsigned short 	    osd3_enable;
	char                osd3_text[128];	
	unsigned short		osd3_x;
	unsigned short	    osd3_y;	
	unsigned short		osd3_color;
	unsigned short 	    osd4_enable;
	char                osd4_text[128];	
	unsigned short		osd4_x;
	unsigned short	    osd4_y;	
	unsigned short		osd4_color;
}osd_config_t_EX;

/*
启用overlay	overlay_enable	unsigned short	(1,0)	ntp	R/W
通道号	channal_num	unsigned short	　	不适用	R/W
overlay X位置	overlay_x	unsigned short	　	198.123.30.132	R/W
overlay y位置	overlay_y	unsigned short	　	GMT8	R/W
overlay width	overlay_width	unsigned short	　		
overlay height	overlay_height	unsigned short	
overlay color ;  0:black, 1:white, 2:red, 3:green,4: blue,5: yellow…　		
*/
typedef struct overlay_config_t
{
	unsigned short 	    overlay_enable;
	unsigned short      overlay_x;	
	unsigned short		overlay_y;
	unsigned short	    overlay_width;	
	unsigned short		overlay_height;
	unsigned short      overlay_color;
}overlay_config_t;

typedef struct md_rect
{
	unsigned short left;
	unsigned short right;
	unsigned short top;
	unsigned short bottom;
}md_rect;

/*
是否启用移动侦测	md_enable	          unsigned short   (1,0)	ntp	R/W
灵敏度	            md_sensitivity	      unsigned short   0：~10级，10最灵敏	198.123.30.132	R/W
联动录像使能        md_alarmrecord        unsigned short   0：联动录像不启动，1：联动录像启用
告警持续时间        md_alarmrecycle       unsigned short   秒为单位，默认为10秒  
联动抓拍            md_capturetoftp       unsigned short   0：不启动，1：启动抓拍功能并把图片保存到本地，如果ftp功能是可有的就把图片告警图片上传到ftp
联动发送邮件        md_sendemail          unsigned short   0:不启用改功能，1：启动功能不发送抓拍图片，2：启动功能发送抓拍图片
io口联动输出        md_alarmout           unsigned short   F:1111 表示最多支持 四路输出
移动侦测区域	    md_region	          char	           BASE64 编码的二进制数据	不适用	R/W
生效时间            md_data               int              127：1111111：星期一到星期天，
时间断使能          md_time               int              F:1111 四个时间段全开
侦测时间段一	    md_time1	          char	           秒数范围，例如：0~86399：00:00:00 ~23:59:59	GMT8	R/W
侦测时间段二	    md_time2	          char	           秒数范围，例如：0~86399：00:00:00 ~23:59:59		
侦测时间段三	    md_time3	          char	           秒数范围，例如：0~86399：00:00:00 ~23:59:59		
侦测时间段四	    md_time4	          char	           秒数范围，例如：0~86399：00:00:00 ~23:59:59		
*/
typedef struct md_config_t
{
	unsigned short 	      md_enable;
	unsigned short        md_sensitivity;
	unsigned short        md_alarmrecycle;
	unsigned short        md_alarmrecord;
	unsigned short        md_recordtime;
	unsigned short        md_capturetoftp;
	unsigned short        md_sendemail; 
	unsigned short        md_alarmout; 
	unsigned char         md_region[12][16];
	int                   md_data;
	int                   md_time;
	char		          md_time1[32];
	char	              md_time2[32];	
	char	              md_time3[32];
	char		          md_time4[32];
}md_config_t;

typedef struct auto_maintain_t
{
	int 	               enable;
	int                    data;
	char                   time[32];
}auto_maintain_t;

/*
是否启用IO告警功能	io_enable	          unsigned short   (1,0)	ntp	R/W
检测类型            io_type	              unsigned short   0:输入高电平告警，1：输入低电平告警
告警持续时间        io_alarmrecycle       unsigned short   秒为单位，默认为10秒   
联动录像使能        io_alarmrecord        unsigned short   0：联动录像不启动，1：联动录像启用
联动抓拍            io_capturetoftp       unsigned short   0：不启动，1：启动抓拍功能并把图片保存到本地，如果ftp功能是可有的就把图片告警图片上传到ftp
联动发送邮件        io_sendemail          unsigned short   0:不启用改功能，1：启动功能不发送抓拍图片，2：启动功能发送抓拍图片
生效时间            io_data               int              127：1111111：星期一到星期天，
时间断使能          io_time               int              F:1111 四个时间段全开
侦测时间段一	    io_time1	          char	           秒数范围，例如：0~86399：00:00:00 ~23:59:59	GMT8	R/W
侦测时间段二	    io_time2	          char	           秒数范围，例如：0~86399：00:00:00 ~23:59:59		
侦测时间段三	    io_time3	          char	           秒数范围，例如：0~86399：00:00:00 ~23:59:59		
侦测时间段四	    io_time4	          char	           秒数范围，例如：0~86399：00:00:00 ~23:59:59		
*/
typedef struct io_config_t
{
	unsigned short 	      io_enable;
	unsigned short        io_type;
	unsigned short        io_alarmrecycle;
	unsigned short        io_alarmrecord;
	unsigned short        io_recordtime;
	unsigned short        io_capturetoftp;
	unsigned short        io_sendemail; 
	unsigned short        io_alarmout;
	int                   io_data;
	int                   io_time;
	char		          io_time1[32];
	char	              io_time2[32];	
	char	              io_time3[32];
	char		          io_time4[32];
}io_config_t;

/*
文件分隔模式	    record_packetsize	    unsigned short	分钟为单位 5~120分
是否启用循环录像	record_recycle	        unsigned short	(1,0)	0;不覆盖，停止录像。1：覆盖录像
预录像时长          record_advancetime      unsigned short  秒为单位5~30
告警录像时长        record_alarmtime        unsigned short  秒为单位5~300
录像码流            record_streantype       unsigned short  0:主码流，1:从码流
*/
typedef struct recordbase_config_t
{
	unsigned short 	       record_packetsize;
	unsigned short		   record_recycle;    
	unsigned short         record_advancetime;
	unsigned short         record_alarmtime;
	unsigned short         record_streamtype;
	unsigned short         unuse;
}recordbase_config_t;

/* 手动录像设置 */
typedef struct __hand_record_t
{
	unsigned char	record_hour_len;
	unsigned char	record_minute_len;
	unsigned char	record_status;
	unsigned char unused;
}hand_record_t;

/*
启用录像	     record_enable	unsigned short	(1,0)	ntp	R/W
生效时间      record_data               int              127：1111111：星期一到星期天，
时间断使能    record_time               int              F:1111 四个时间段全开
录像时间段一	record _ time1	字符串	秒数范围，例如：0~86399：00:00:00~23:59:59	GMT8	R/W
录像时间段二	record _time2	字符串	秒数范围，例如：0~86399：00:00:00~23:59:59		
录像时间段三	record _time3	字符串	秒数范围，例如：0~86399：00:00:00~23:59:59		
录像时间段四	record _time4	字符串	秒数范围，例如：0~86399：00:00:00~23:59:59		
*/
typedef struct recordschedule_config_t
{
	int 	                            record_enable;
	int                                 record_data;	
	int                                 record_time;	
	char                                record_time1[32];    
	char                                record_time2[32];	
	char                                record_time3[32];
	char                                record_time4[32];
}recordschedule_config_t;

/*
是否启用手机	sip_enable	        unsigned short	(1,0)
登录用户名   	sip_user	        char
登录密码    	sip_password	    char
服务器地址一     sip_ServerPath1	char
服务器地址二	sip_ServerPath2	    char
绑定复位标记     sip_resetflag       int
*/
typedef struct sip_config_t_
{
	unsigned short 	   sip_enable;
	char		       sip_user[32];	
	char 	           sip_password[32];	
	char		       sip_ServerPath1[64];
	char		       sip_ServerPath2[64];
    int                sip_resetflag;
} sip_config_t;

/*
是否启用云台	ptz_enable	    unsigned short	(1,0)
云台设备地址	ptz_address	    unsigned short	0~255
控制协议地址	ptz_protocol	unsigned short	0:Pelco D;1:Pelco P
波特率	        ptz_bandrate	unsigned short	0:1200,1:2400;2:4800;3:9600;4:14400;5:19200;6:38400;7:57600;8:115200
校验位	        ptz_parity	    unsigned short	0:none, 1:odd, 2:even		
数据位	        ptz_databit	    unsigned short	7, 8		
停止位	        ptz_stopbit	    unsigned short	1, 2		
*/
typedef struct ptz_config_t
{
	unsigned short 	         ptz_enable;
	unsigned short	         ptz_address;	
	unsigned short           ptz_protocol;	
	unsigned short		     ptz_bandrate;    
	unsigned short	         ptz_parity;	
	unsigned short           ptz_databit;	
	unsigned short		     ptz_stopbit;  
}ptz_config_t;

/*
用户使能    user_enable        unsigned short     0：未使用，1：启动
用户名	    user_name	       char		          ntp	R/W
用户密码	user_passwd	       char	　	          不适用	R/W
用户描述    user_description   char
用户权限    user_jurisdiction  int                最大为：0xffff，当为0xffff为最高权限      
*/
typedef struct user_config_t
{
	unsigned int user_enable;
	char 	     user_name[32];
	char         user_passwd[32];
	char         user_description[64];
	int          user_jurisdiction;
}user_config_t;

/*
厂商名称         factory_name                 char
版本号           version_number               char    
产品型号         device_type	          	  char		         
版本日期         version_date	          	  char	　	          
最大编码分辨率   Max_Support_Resolution       char
产品序列号       device_serial_no             char               
内核版本号       kernel_version               char
WEB的版本号      web_version                  char              
*/
typedef struct  _system_Info_t
{
    char    factory_name[VERSION_LEN];             /* 厂商名称 */
    char    version_number[VERSION_LEN];           /* 版本号 */
    char    device_type[VERSION_LEN];              /* 产品型号 */
    char    version_date[VERSION_LEN];             /* 版本日期 */
    char    Max_Support_Resolution[VERSION_LEN];   /* 最大编码分辨率 */
    char    device_serial_no[32];                  /* 产品序列号 */
    char    kernel_version[VERSION_LEN];           /* 内核版本号 */
    char    web_version[VERSION_LEN];              /* WEB的版本号 */
    char    Model[128];                            /* 产品模组信息 */
    char    HardwareId[32];                        /* 产品硬件ＩＤ */
    char    ProductHardVersion[32];                /* 硬件版本 */
}system_Info_t;

typedef struct  _gb28181_config_t
{
	char            chEnable;                                  /* 国标28181使能开关 默认 关*/
	char            chAlarmLevel;                              /* 报警级别 取值1~6 默认2 */
	char            szSipServerID[24];                         /* SIP服务器编码 默认 34020000002000000001 */
	char            szSipDomain[16];                           /* SIP域       默认  3402000000 */
	char            szSipServerIP[24];                         /* SIP服务器IP 默认 192.168.10.124 */
	unsigned short  nSipServerPort;                            /* SIP服务器端口 默认 5060*/
	char            szSipDeviceID[24];                         /* 设备编码 默认 34020000001320000001*/
	char            szSipAlarmID[24];                          /* 设备告警编码 默认 34020000001340000001*/
	char            szPassword[32];                            /* 注册密码 默认 12345678*/
	char            szSipChannelID[24];                        /* 通道编码 默认34020000001320000001*/
	char            szOwner[24];                               /* 设备归属   默认 NULL*/
	char            szCivilCode[24];                           /* 行政区域   默认 NULL*/
	char            szBlock[24];                               /* 警区      默认 NULL*/
	char            szAddress[24];                             /* 安装地址   默认 NULL*/
	char            szLongitude[24];                           /* 经度      默认 0.0 */
	char            szLatitude[24];                            /* 纬度      默认 0.0 */
	unsigned int    nExpires;                                  /* 注册有效期(秒) 默认3600 */
	unsigned int    nAlive;                                    /* 心跳周期(秒)      默认60*/
	unsigned int    nTimeoutNum;                               /* 最大心跳超时次数 默认 3 */
}gb28181_config_t;

#define MAX_ALARM_NUM           (256)
#define MAX_ALARM_STRCH         (2 * 17)
typedef struct {
  short   m_chnum;                                        /* 有效通道个数 */
  short   m_fontsize;                                     /* 0->32*32, 1->48*48, 2->64*64 */
  short   m_interval;                                     /* 刷屏时间间隔 */
  char    m_strch[MAX_ALARM_NUM][MAX_ALARM_STRCH];        /* 每个通道最多显示16个汉字，32个ASCII  */
  short   m_reserve;
} alarm_config_t;

typedef struct param {
  char 			            param_exist_flag[FLAG_OFFSET]; //这是一个版本控制的标记，当有扩展结构体的时候需要修改该标记，
  sys_config_t                sysconfig;
  datetime_config_t           datetimeconfig;
  network_config_t            network_config;
  network_port_config_t       network_port_config;
  pppoe_config_t              pppoe_config;
  ddns_config_t               ddns_config;
  ftp_config_t                ftp_config;
  email_config_t              email_config;
  sensor_config_t             sensor_config[MAX_CHANNEL_NUM];
  v_enc_config_t              v_enc_config[MAX_CHANNEL_NUM];
  v_s_enc_config_t            v_s_enc_config[MAX_CHANNEL_NUM];
  a_enc_config_t              a_enc_config[MAX_CHANNEL_NUM];
  osd_config_t                osd_config[MAX_CHANNEL_NUM];
  overlay_config_t            overlay_config[MAX_CHANNEL_NUM][4];
  md_config_t                 md_config[MAX_CHANNEL_NUM];
  io_config_t                 io_config[MAX_CHANNEL_NUM];
  recordbase_config_t         recordbase_config;
  hand_record_t		            hand_record[MAX_CHANNEL_NUM];
  recordschedule_config_t     recordschedule_config[MAX_CHANNEL_NUM];
  ptz_config_t                ptz_config;
  user_config_t               user_config[20];
  system_Info_t               systeminfo;
  sip_config_t                sip_config;
  wireless_config_t           wireless_config;
  gb28181_config_t            gb28181_config;
  daynight_schedule_t         daynight_schedule[MAX_CHANNEL_NUM];
  auto_maintain_t             automaintain;
  DST_config_t                dst_config;
  isp_config_t                isp_config;
  osd_config_t_EX             osd_config_ex[MAX_CHANNEL_NUM];
  alarm_config_t              alarm_config;
  char                        reserve[4*1024 - sizeof(int)];
  int                         end;
}param;

/*
【函数描述】
初试化ParamLib库。
【参数】
Null;
【返回值】
返回值	描述
0	成功
非0	失败。
*/
int ParamLib_InitParamLib(void);



/*
【函数描述】
反初试化ParamLib库。
【参数】
Null;
【返回值】
返回值	描述
0	成功
非0	失败。
*/
int ParamLib_deinitParamLib(void);


/*
【函数描述】
启动一个监听参数修改服务。
【参数】
Null;
【返回值】
返回值	描述
0	成功
非0	失败。
*/
int ParamLib_StartParamService(void);


/*
【函数描述】
停止一个监听参数修改服务。
【参数】
Null;
【返回值】
返回值	描述
0	成功
非0	失败。
*/

int ParamLib_StopParamService(void);

/************************************************************************/
/*                           DateTime                                   */
/************************************************************************/
/*
【函数描述】
配置系统的日期和时间。
【参数】
参数名称	描述	输入/输出
datetime	保存日期和时间的结构体	输入
【返回值】
返回值	描述
0	成功
非0	失败。
*/
int ParamLib_SetDateTimeInfo(datetime_config_t datetime);

/*
【函数描述】
获取系统的日期和时间的相关配置。

【参数】
参数名称	描述	输入/输出
datetime	保存日期和时间的结构体	输出
【返回值】
返回值	描述
0	成功
非0	失败。
*/
int ParamLib_GetDateTimeInfo(datetime_config_t* datetime);
int ParamLib_SetDSTInfo(DST_config_t dstinfo);
int ParamLib_GetDSTInfo(DST_config_t* dstinfo);

/************************************************************************/
/*                           NetWork                                    */
/************************************************************************/
/*
【函数描述】
设置设备网路的相关配置。
【参数】
参数名称	描述	输入/输出
networkinfo	保存网络配置的结构体	输入
【返回值】
返回值	描述
0	成功
非0	失败。
*/

int ParamLib_SetNetWorkInfo(network_config_t networkinfo);

/*
【函数描述】
获取设备网路的相关配置。
【参数】   
参数名称	描述	输入/输出
networkinfo	保存网络配置的结构体	输出
【返回值】
返回值	描述
0	成功
非0	失败。
*/
int ParamLib_GetNetWorkInfo(network_config_t *networkinfo);


/************************************************************************/
/*                           NetWork                                    */
/************************************************************************/
/*
【函数描述】
获取无线网路的相关配置。
【参数】   
参数名称	描述	输入/输出
wirelessinfo	保存无线网络配置的结构体	输出
【返回值】
返回值	描述
0	成功
非0	失败。
*/
void ParamLib_GetWirelessInfo(wireless_config_t *wirelessinfo);

/*
【函数描述】
设置无线网路的相关配置。
【参数】   
参数名称	描述	输入/输出
wirelessinfo	保存无线网络配置的结构体	输入
【返回值】
返回值	描述
0	成功
非0	失败。
*/
void ParamLib_SetWirelessInfo(wireless_config_t wirelessinfo);


/************************************************************************/
/*                           MAIL                                       */
/************************************************************************/

/*
【函数描述】
设置电子邮件的相关配置。
【参数】
参数名称	描述	                        输入/输出
mailinfo	设置电子邮件的相关配置的结构体	输入
【返回值】
返回值	描述
0	成功
非0	失败。
*/

int ParamLib_SetEmailInfo(email_config_t emailinfo);


/*
【函数描述】
获取电子邮件的相关配置。
【参数】   
参数名称	描述	输入/输出
mailinfo	保存电子邮件配置的结构体	输出

【返回值】
返回值	描述
0	成功
非0	失败。
*/
int ParamLib_GetEmailInfo(email_config_t *emailinfo);


/************************************************************************/
/*                           FTP                                       */
/************************************************************************/

/*
【函数描述】
设置FTP相关配置。
【参数】
参数名称	描述	                        输入/输出
mailinfo	设置FTP的相关配置的结构体	    输入
【返回值】
返回值	描述
0	成功
非0	失败。
*/

int ParamLib_SetFTPInfo(ftp_config_t ftpinfo);


/*
【函数描述】
获取FTP的相关配置。
【参数】   
参数名称	描述	输入/输出
mailinfo	保存FTP配置的结构体	输出

【返回值】
返回值	描述
0	成功
非0	失败。
*/
int ParamLib_GetFTPInfo(ftp_config_t *ftpinfo);

/************************************************************************/
/*                          PPPOE                                       */
/************************************************************************/
/*
【函数描述】
设置设备PPPOE拨号的相关配置。
【参数】
参数名称	描述	输入/输出
pppoeinfo	保存PPPOE拨号配置的结构体	输入
【返回值】
返回值	描述
0	成功
非0	失败。
*/
int ParamLib_SetPPPOEInfo(pppoe_config_t  pppoeinfo);

/*
【函数描述】
获取设备PPPOE拨号的相关配置。。
【参数】   
参数名称	描述	输入/输出
pppoeinfo	保存网络配置的结构体	输出

【返回值】
返回值	描述
0	成功
非0	失败。
*/
int ParamLib_GetPPPOEInfo(pppoe_config_t * pppoeinfo);

/*
【函数描述】
设置设备DDNS的相关配置。
【参数】
参数名称	描述	输入/输出
ddnsinfo	保存DDNS配置的结构体	输入
【返回值】
返回值	描述
0	成功
非0	失败。
*/
int ParamLib_SetDDNSInfo(ddns_config_t  ddnsinfo);


/*
【函数描述】
获取设备PPPOE拨号的相关配置。。
【参数】   
参数名称	描述	输入/输出
ddnsinfo	保存DDNS配置的结构体	输出
【返回值】
返回值	描述
0	成功
非0	失败。
*/
int ParamLib_GetDDNSInfo(ddns_config_t * ddnsinfo);



/*
【函数描述】
设置设备Sensor的相关配置。
【参数】
参数名称	描述	输入/输出
channal_num 通道号                  输入 
sensorinfo	保存Sensor配置的结构体	输入
【返回值】
返回值	描述
0	成功
非0	失败。
*/
int ParamLib_SetSensorInfo(unsigned short channal_num,sensor_config_t  sensorinfo);

/*
【函数描述】
获取设备Sensor的相关配置。。
【参数】   
参数名称	描述	输入/输出
channal_num 通道号                  输入 
sensorinfo	保存Sensor配置的结构体	输出
【返回值】
返回值	描述
0	成功
非0	失败。
*/
int ParamLib_GetSensorInfo(unsigned short channal_num,sensor_config_t * sensorinfo);

/*
【函数描述】
设置设备isp的相关配置。
【参数】
参数名称	描述	输入/输出
channal_num 通道号                  输入 
sensorinfo	保存isp配置的结构体	输入
【返回值】
返回值	描述
0	成功
非0	失败。
*/
int ParamLib_SetIspInfo(unsigned short channal_num,isp_config_t  ispinfo);


/*
【函数描述】
获取设备isp的相关配置。。
【参数】   
参数名称	描述	输入/输出
channal_num 通道号                  输入 
sensorinfo	保存isp配置的结构体	输出
【返回值】
返回值	描述
0	成功
非0	失败。
*/
int ParamLib_GetIspInfo(unsigned short channal_num,isp_config_t *ispinfo);

/*
【函数描述】
设置启用白天黑夜模式的时间表。
【参数】   
参数名称	描述	输入/输出
channal_num 通道号                  输入 
daynightschedule	保存白天黑夜模式切换的时间表 配置的结构体	输出
【返回值】
返回值	描述
0	成功
非0	失败。
*/
int ParamLib_SetDayNightSchedule(unsigned short channal_num,daynight_schedule_t daynightschedule);

/*
【函数描述】
获取启用白天黑夜模式的时间表。
【参数】   
参数名称	描述	输入/输出
channal_num 通道号                  输入 
daynightschedule	保存白天黑夜模式切换的时间表 配置的结构体	输出
【返回值】
返回值	描述
0	成功
非0	失败。
*/
int ParamLib_GetDayNightSchedule(unsigned short channal_num,daynight_schedule_t * daynightschedule);

/*【函数描述】
设置设备视频编码的相关配置。
【参数】
参数名称	描述	输入/输出
channal_num 通道号      输入 
视频编码配置的结构体	输入
【返回值】
返回值	描述
0	成功
非0	失败。
*/
int ParamLib_SetVEncInfo(unsigned short channal_num,v_enc_config_t  v_encinfo);

/*
【函数描述】
获取设备视频编码的相关配置。。
【参数】   
参数名称	描述	输入/输出
channal_num 通道号      输入 
视频编码配置的结构体	输出
【返回值】
返回值	描述
0	成功
非0	失败。
*/
int ParamLib_GetVEncInfo(unsigned short channal_num,v_enc_config_t *v_encinfo);

/*
【函数描述】
获取设备视频编码的相关配置。。
【参数】   
参数名称	描述	输入/输出
channal_num 通道号      输入 
视频编码配置的结构体	输出
【返回值】
返回值	描述
0	成功
非0	失败。
*/
int ParamLib_GetVEncInfo_EX(unsigned short channal_num,unsigned streamtype,v_enc_config_t *v_encinfo);


/*【函数描述】
设置设备从码流编码的相关配置。
【参数】
参数名称	描述	输入/输出
channal_num 通道号      输入 
视频编码配置的结构体	输入
【返回值】
返回值	描述
0	成功
非0	失败。
*/

int ParamLib_SetSVEncInfo(unsigned short channal_num,v_s_enc_config_t  v_s_encinfo);

/*
【函数描述】
获取设从码流编的相关配置。。
【参数】   
参数名称	描述	输入/输出
channal_num 通道号      输入 
视频编码配置的结构体	输出
【返回值】
返回值	描述
0	成功
非0	失败。
*/

int ParamLib_GetSVEncInfo(unsigned short channal_num,v_s_enc_config_t *v_s_encinfo);


/*【函数描述】
设置设备音频编码的相关配置。
【参数】
参数名称	描述	输入/输出
channal_num 通道号      输入 
音频编码配置的结构体	输入
【返回值】
返回值	描述
0	成功
非0	失败。
*/
int ParamLib_SetAEncInfo(unsigned short channal_num,a_enc_config_t  a_encinfo);

/*
【函数描述】
获取设备音频编码的相关配置。。
【参数】   
参数名称	描述	输入/输出
channal_num 通道号      输入 
音频编码配置的结构体	输出
【返回值】
返回值	描述
0	成功
非0	失败。
*/
int ParamLib_GetAEncInfo(unsigned short channal_num,a_enc_config_t *a_encinfo);



/*
【函数描述】
设置设备OSD的相关配置。
【参数】
参数名称	描述	输入/输出
channal_num 通道号                  输入 
osdinfo	保存OSD配置的结构体	输入
【返回值】
返回值	描述
0	成功
非0	失败。
*/
int ParamLib_SetOSDInfo(unsigned short channal_num,osd_config_t  osdinfo);


/*
【函数描述】
获取设备OSD的相关配置。。
【参数】   
参数名称	描述	输入/输出
channal_num 通道号                  输入 
osdinfo	保存OSD配置的结构体	输出
【返回值】
返回值	描述
0	成功
非0	失败。
*/
int ParamLib_GetOSDInfo(unsigned short channal_num,osd_config_t * osdinfo);

int ParamLib_SetOSDInfo_Ex(unsigned short channal_num, osd_config_t_EX osdinfo);
int ParamLib_GetOSDInfo_Ex(unsigned short channal_num, osd_config_t_EX *osdinfo);

/*
【函数描述】
设置设备Overlay的相关配置。
【参数】
参数名称	描述	输入/输出
channal_num 通道号                  输入 
Overlayinfo	保存Overlay配置的结构体	输入
【返回值】
返回值	描述
0	成功
非0	失败。
*/
int ParamLib_SetOverlayInfo(unsigned short channal_num,unsigned short overlayno,overlay_config_t  Overlayinfo);


/*
【函数描述】
获取设备Overlay的相关配置。。
【参数】   
参数名称	描述	输入/输出
channal_num 通道号                  输入 
Overlayinfo	保存Overlay配置的结构体	输出
【返回值】
返回值	描述
0	成功
非0	失败。
*/
int ParamLib_GetOverlayInfo(unsigned short channal_num,unsigned short overlayno,overlay_config_t * Overlayinfo);

/*
4.1.3.22  ParamLib_SetMDInfo
【函数描述】
设置设备移动侦测的相关配置。
【参数】
参数名称	描述	输入/输出
channal_num 通道号                  输入 
mdinfo	保存移动侦测配置的结构体	输入
【返回值】
返回值	描述
0	成功
非0	失败。
*/
int ParamLib_SetMDInfo(unsigned short channal_num,md_config_t  mdinfo);

/*
【函数描述】
获取设备移动侦测的相关配置。。
【参数】   
参数名称	描述	输入/输出
channal_num 通道号                  输入 
mdinfo	保存移动侦测配置的结构体	输出
【返回值】
返回值	描述
0	成功
非0	失败。
*/
int ParamLib_GetMDInfo(unsigned short channal_num,md_config_t * mdinfo);


/*
4.1.3.22  ParamLib_SetAutoMaintain
【函数描述】
设置系统自动维护的相关配置。
【参数】
参数名称	描述	输入/输出
channal_num 通道号                  输入 
mdinfo	保存移动侦测配置的结构体	输入
【返回值】
返回值	描述
0	成功
非0	失败。
*/
int ParamLib_SetAutoMaintain(auto_maintain_t  automaintain);

/*
【函数描述】
获取系统自动维护的相关配置。。
【参数】   
参数名称	描述	输入/输出
channal_num 通道号                  输入 
mdinfo	保存移动侦测配置的结构体	输出
【返回值】
返回值	描述
0	成功
非0	失败。
*/
int ParamLib_GetAutoMaintain(auto_maintain_t * automaintain);

/*
4.1.3.22  ParamLib_SetIOAlarmInfo
【函数描述】
设置设备IO告警检测的相关配置。
【参数】
参数名称	描述	              输入/输出
channal_num 通道号                  输入 
mdinfo	告警配置的结构体	        输入
【返回值】
返回值	描述
0	成功
非0	失败。
*/
int ParamLib_SetIOAlarmInfo(unsigned short channal_num,io_config_t ioinfo);

/*
【函数描述】
获取设备IO告警检测的相关配置。。
【参数】   
参数名称	   描述	                 输入/输出
channal_num    通道号                  输入 
ioinfo         告警配置的结构体	       输出
【返回值】
返回值	描述
0	成功
非0	失败。
*/
int ParamLib_GetIOAlarmInfo(unsigned short channal_num,io_config_t *ioinfo);


/*
【函数描述】
设置设备录像的基本配置。
【参数】
参数名称	描述	输入/输出
channal_num 通道号                  输入 
recordbaseinfo	保存设备录像的基本配置的结构体	输入
【返回值】
返回值	描述
0	成功
非0	失败。
*/
int ParamLib_SetRecordBaseInfo(recordbase_config_t  recordbaseinfo);


/*
【函数描述】
获取设备录像的基本配置。。
【参数】   
参数名称	描述	输入/输出  
recordbaseinfo	保存手动录像状态的结构体	输出
【返回值】
返回值	描述
0	成功
非0	失败。
*/
int ParamLib_GetRecordBaseInfo(recordbase_config_t * recordbaseinfo);


/*
【函数描述】
设置获取手动录像状态配置。
【参数】
参数名称	描述	输入/输出
channal_num 通道号                  输入 
handrecord	保存设备录像的基本配置的结构体	输入
【返回值】
返回值	描述
0	成功
非0	失败。
*/
void ParamLib_GetHandRecordStruct(int channal_num, hand_record_t *handrecord);


/*
【函数描述】
设置手动录像状态配置。
【参数】
参数名称	描述	输入/输出
channal_num 通道号                  输入 
handrecord	保存手动录像状态配置的结构体	输入
【返回值】
返回值	描述
0	成功
非0	失败。
*/
void ParamLib_SetHandRecordStruct(int channal_num, hand_record_t handrecord);


/*
【函数描述】
设置设备录像计划的基本配置。

【参数】
参数名称	描述	输入/输出
recordscheduleinfo	保存设备录像计划的基本配置的结构体	输入
【返回值】
返回值	描述
0	成功
非0	失败。
*/
int ParamLib_SetRecordScheduleInfo(unsigned short channal_num,recordschedule_config_t  recordscheduleinfo);


/*
【函数描述】
设置设备的相关系统信息。
【参数】
参数名称	描述	输入/输出
systeminfo	保存设备的相关系统信息的结构体	输入
【返回值】
返回值	描述
0	成功
非0	失败。
*/
int ParamLib_GetSystemInfo(system_Info_t *systeminfo);

/*
【函数描述】
获取相关系统信息。
【参数】
参数名称	描述	输入/输出
systeminfo	保存设备的相关系统信息的结构体	输入
【返回值】
返回值	描述
0	成功
非0	失败。
*/
int ParamLib_SetSystemInfo(system_Info_t systeminfo);


/*
【函数描述】
获取设备录像计划的基本配置。。
【参数】   
参数名称	描述	输入/输出
channal_num 通道号                  输入 
recordscheduleinfo	保存设备录像计划的基本配置的结构体	输出
【返回值】
返回值	描述
0	成功
非0	失败。
*/
int ParamLib_GetRecordScheduleInfo(unsigned short channal_num,recordschedule_config_t * recordscheduleinfo);


/*
【函数描述】
设置设备PTZ的基本配置。
【参数】
参数名称	描述	输入/输出
channal_num 通道号                  输入 
ptzinfo	保存设备PTZ配置的结构体	输入
【返回值】
返回值	描述
0	成功
非0	失败。
*/
int ParamLib_SetPTZInfo(ptz_config_t  ptzinfo);


/*
【函数描述】
获取设备PTZ的基本配置。。
【参数】   
参数名称	描述	输入/输出
ptzinfo	保存PTZ配置的结构体	输出
【返回值】
返回值	描述
0	成功
非0	失败。
*/
int ParamLib_GetPTZInfo(ptz_config_t * ptzinfo);
int ParamLib_SetSIPInfo(sip_config_t  sipinfo);
int ParamLib_GetSIPInfo(sip_config_t * sipinfo);

/*
【函数描述】
获取系统的运行的基本配置。。
【参数】   
参数名称	描述	                    输入/输出
sysconfig	保存系统运行配置的结构体	输出
【返回值】
返回值	描述
0	成功
非0	失败。
*/
int ParamLib_GetSysConfig(sys_config_t *sysconfig);


/*
【函数描述】
设置系统的运行的基本配置。
【参数】   
参数名称	描述	                    输入/输出
sysconfig	保存系统运行配置的结构体	输出
【返回值】
返回值	描述
0	成功
非0	失败。
*/
int ParamLib_SetSysConfig(sys_config_t sysconfig);


/*
【函数描述】
设置设备用户的基本配置。
【参数】
参数名称	描述	                输入/输出
userinfo	保存用户配置的结构体	输入
【返回值】
返回值	描述
0	成功
非0	失败。
*/
int  ParamLib_SetUserInfo(char *userinfo);

/*
【函数描述】
获取设备用户的基本配置。。
【参数】   
参数名称	描述	                    输入/输出
userinfo	保存所有用户的信息的结构体	    输出
【返回值】
返回值	描述
0	成功
非0	失败。
*/
int ParamLib_GetUserInfo(char *userinfo);


/*
【函数描述】
获取设备用户的基本配置。。
【参数】   
参数名称	描述	                    输入/输出
userinfo	保存网络连接端口的结构体	输出
【返回值】
返回值	描述
0	成功
非0	失败。
*/
int ParamLib_GetPortInfo(network_port_config_t * portinfo);

/*
【函数描述】
设置网络连接端口配置。
【参数】
参数名称	描述	                    输入/输出
portinfo	保存网络连接端口的结构体	输入
【返回值】
返回值	描述
0	成功
非0	失败。
*/
int ParamLib_SetPortInfo(network_port_config_t  portinfo);


/*
【函数描述】
设置GB28181协议相关配置。
【参数】
参数名称	描述	                    输入/输出
gb28181_info	保存GB28181协议的结构体	输入
【返回值】
返回值	描述
0	成功
非0	失败。
*/
int ParamLib_SetGB28181Info(gb28181_config_t  gb28181_info);

/*
【函数描述】
获取GB28181协议相关配置。
【参数】   
参数名称	描述	                    输入/输出
gb28181_info	保GB28181协议的结构体	输出
【返回值】
返回值	描述
0	成功
非0	失败。
*/
int ParamLib_GetGB28181Info(gb28181_config_t *gb28181_info);

/*
【函数描述】
备份IPC的参数
【参数】   
NULL
【返回值】
返回值	描述
0	成功
非0	失败。
*/
int ParamLib_BackUpParam(void);

/*
【函数描述】
把已经备份的参数还原回来
【参数】   
NULL
【返回值】
返回值	描述
0	成功
非0	失败。
*/
int ParamLib_ResumeParam(void);

/*
【函数描述】
恢复IPC的出厂配置
【参数】   
【返回值】
返回值	描述
0	成功
非0	失败。
*/
int ParamLib_ResumeFactoryParam(void);


/*
【函数描述】
获取设备的所有参数的基本配置。。
【参数】   
参数名称	描述	                    输入/输出
paraminfo  	保存设备所有参数的结构体 输出
【返回值】
返回值	描述
0	成功
非0	失败。
*/
int ParamLib_GetParamInfo(param *paraminfo);

/*
【函数描述】
获取设备的所有参数的基本配置。。
【参数】   
参数名称	描述	                    输入/输出
paraminfo  	保存设备所有参数的结构体 输出
【返回值】
返回值	描述
0	成功
非0	失败。
*/
int ParamLib_SetParamInfo(param paraminfo);

/*
【函数描述】
获取编译日期和时间
【参数】   
参数名称	描述	                    输入/输出
szDate  	编译日期                    输出
szTime      编译时间                    输出
【返回值】
返回值	描述
0	成功
非0	失败。
*/
int ParamLib_GetCompileDateTime(char *szDate, char *szTime);

int ParamLib_GetSerialInfo(char *serialNum);

int ParamLib_SetSerialInfo(char *serialNum);

int ParamLib_SetAlarmInfo(const alarm_config_t * alarm_info);

int ParamLib_GetAlarmInfo(alarm_config_t * alarm_info);

int ParamLib_RefreshDeviceInfo(void);

//这里获取一个同步参数到flash的标记，防止在写数据到配置文件的过程中吧文件给写坏了
int GetParamDirty(void);
#endif

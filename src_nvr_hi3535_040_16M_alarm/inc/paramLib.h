#ifndef __PARAMLIB_H__
#define __PARAMLIB_H__

#define MAX_CHANNEL_NUM 1
#define FLAG_OFFSET    20  //������汾�ŵ��ַ�����  

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
	unsigned int	    dev_type;	/* �豸���� ���ַ��� �˷���ΪHisi3511 �������� Hisi3510 */
	unsigned char		chip_num;   /*оƬ������*/
	unsigned char		channel_num;/*�豸��ͨ����*/
	unsigned char		prober_num; /*�澯�������*/
	unsigned char		output_num; /*�澯��������*/
	unsigned char		adchip_type;/* 0:no ad chip 1:tvp5150  2:TW2815 */
	unsigned char		encode_format;/*�����ʽ  0��PAL  1��N*/
	unsigned char		language; /*����  0��Ӣ��  1������*/
	unsigned char 		serial_num;
	char                version[64]; //�汾��Ϣ
	char                devicename[64];
	unsigned char		unused2[4];
}sys_config_t;

/*
ʱ��ͬ����ʽ	time_sync	unsigned short	(0:ntp, 1:manual)	ntp	R/W
ϵͳ���ں�ʱ��	datetime	Char 	(yyyy-MM-dd hh:mm:ss)	������	R/W
NTP��������ַ	ntp_server	char	��	198.123.30.132	R/W
ʱ��	time_zone	unsigned short	(0:GMT, 1:GMT1,2:GMT2,3:GMT-1,4:GMT-2 ��)	GMT8	R/W
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
��������        eth_type               unsigned short     0:�������磬1����������
IP�汾          eth_protocol           unsigned short	  0:IPV4��1:IPV6	
MAC ��ַ	    eth_mac                char	              (XX:XX:XX:XX:XX:XX)	ntp	R/W
IP��ַ��ȡģʽ	eth_ipmode	           unsigned short	  0:static,1:dhcp	������	R/W
��̬IP��ַ	    eth_ipaddr	           char	��	          198.123.30.132	R/W
��������	    eth_subnet_mask	       char	��	          GMT8	R/W
���ص�ַ	    eth_gateway	           char	��		
DNS��������ַ	dns_server_f	       char	��		      ��ѡDNS������
DNS��������ַ	dns_server_s	       char	              ����DNS������     
DNS������ʹ��	dns_enable	           unsigned short	(1,0)		
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
    unsigned char safety_option;    /* WEP��ȫѡ��(1->AUTO, 2->OPEN, 3->SHARE) */
    unsigned char passwd_format;    /* ��Կ��ʽ(ASCII, HEX)*/
    unsigned char key_numb;         /* Ƶ�� ͨ���� */
    unsigned char unused;
    unsigned char password[4][32];  /* ���� */
    unsigned char key_type[4];      /* ��Կ����(64��Կhex(10)/ascii(5), 128��Կhex(26)/ascii(13), 152��Կhex(32)/ascii(16)) */
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
�����ͨ�Žӿ�	netserver_port	unsigned short	
���������	    maxlink     	unsigned short 	
rtsp�˿�	    rtspport	    unsigned short	��
web�˿�        	httppport	    unsigned short	
*/
typedef struct network_port_config_t
{
	unsigned short	   netserver_port;//�����ͨ�Žӿڣ�9060
	unsigned short		maxlink;   /*���������*/
	unsigned short		rtspport;	/*rtsp�˿�*/
	unsigned short		httppport;	 /*web�˿�*/ 
	unsigned short		onvifport;
}network_port_config_t;

/*
�Ƿ������ʼ����� 	    email_enable	        unsigned short   	
�����ʼ�������	        email_serverIP	        Char	 
�����˵�ַ              email_fromAddr          Char
��������������	        email_password	        Char	��		
����������              email_userName          Char
�����˵�ַ              email_toAddr            Char
�ʼ�����	            email_subject	        Char	��     
��½��                  email_loginName         Char  
�˿�                    email_port              int
*/
typedef struct __email_config_t
{
	unsigned short email_enable;	        /* 1:����EMAIL  0: ��ֹEMAIL */
	char           email_serverIP[64];	    /* �����ʼ������� */
	char           email_fromAddr[64];	    /* �����˵�ַ */
	char           email_password[32];	    /* ��������������*/
	char           email_userName[32];	    /* ���������� */
	char           email_toAddr[256];	    /* �����˵�ַ */	
	char           email_subject[64];       /* �ʼ����� */
	char           email_loginName[32];     /* ��½�� */
	unsigned short email_port;
}email_config_t;

/*
�Ƿ�����FTP 	ftp_enable	    unsigned short	
FTP �û�	    ftp_user	    Char		
FTP ����	    ftp_password	Char	��		
������·��	    ftp_directory	Char	��     ftp://������:�˿�/·��/ 
Ԥ���ֶ�        unused          char
*/
typedef struct ftp_config_t
{
	char ftp_enable;	
	char ftp_username[64];
	char ftp_password[64];
	char ftp_directory[256];	
}ftp_config_t;

/*
�Ƿ�����PPPOE 	pppoe_enable	unsigned short	(1,0)	ntp	R/W
PPPOE �û�	pppoe_user	Char	(Ӣ����ĸ, ����, �»���)	������	R/W
PPPOE ����	pppoe_password	Char	��	198.123.30.132	R/W
PPPOE ����״̬	pppoe_status	Char	�����ųɹ���ʾ����IP	GMT8	R/W
*/
typedef struct pppoe_config_t
{
	unsigned short 	   pppoe_enable;
	char		       pppoe_user [32];	
	char 	           pppoe_password[32];	
	char		       pppoe_status[32];
} pppoe_config_t;

/*
�Ƿ����� DDNS 	ddns_enable	unsigned short	(1,0)	ntp	R/W
DDNS ��������ַ	ddns_server	Char	IP ��ַ������	������	R/W
DDNS �û���	ddns_user	Char	(Ӣ����ĸ, ����, �»���)	198.123.30.132	R/W
DDNS ����	ddns_password	Char	(Ӣ����ĸ, ����, �»���)	GMT8	R/W
*/
typedef struct ddns_config_t
{
	unsigned short 	   ddns_enable;
	char		       ddns_server [32];	
	char 	           ddns_user [32];	
	char		       ddns_password [32];
}ddns_config_t;

/*
����	        brightness	unsigned short	(0-256)
�Աȶ�	        contrast	unsigned short	(0-256)
ɫ��	        hue	        unsigned short	(0-256)
���Ͷ�	        saturation	unsigned short	(0-256)
���            sharpness   unsigned short	(0-256)
�����ҹģʽ	daynight	unsigned short	 0 : night mode 1: day mode 2: auto 3: timer
�Զ���ƽ��	    whitebalance short	(0-100)	 0 : indoor 1: outdoor 2: Auto 
����	        backlight	unsigned short	(0-100)	0:low 1:median 2: hight 
��˸Ƶ�ʿ���    imagesource unsigned short   0 : 60hz 1 : 50hz
��ת            flip        unsigned short   0 : ��ֹ 1 : ����
����            mirrorimage unsigned short   0 : ��ֹ 1 : ����
��ʽ	        video_standard	unsigned short	(NTSC,PAL,auto)		
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
���ⲹ������    weightingEnable     Ĭ��ֵ20    �����Ѿ�����
���ⲹ��ѡ��    weightingMode       Ĭ��ֵ6     [1,6]->(up down left right center all)
�ع�ģʽ        aeType              Ĭ��ֵ0     0->�Զ� 1->�ֶ�
�ֶ��ع�ʱ��    aeExpStrength       Ĭ��ֵ13    [1,18] 1->1/100000, 2->1/10000, 3->1/4000, 4->1/2000, 5->1/1000, 6->1/750, 7->1/500, 8->1/250, 9->1/200, 10->1/150, 50HZȡ������ֵ(11->1/100, 12->1/50, 13->1/25), 60HZȡ������ֵ(14->1/120, 15->1/60, 16->1/30), 17->1/16, 18->1/3
�ֶ��ع�����    aeExpCompensation   Ĭ��ֵ128   [0,255]
����ģʽ        denoiseType         Ĭ��ֵ0     0Ϊ�Զ� 1Ϊ�ֶ�
�ֶ�������    SfStrength          Ĭ��ֵ128   [0,255]
�ֶ�ʱ����    TfStrength          Ĭ��ֵ128   [0,255]
I֡ƽ��         IframeStrength      Ĭ��ֵ5     [0,20]  �����Ѿ�����
ȥ����        defogEnable         Ĭ��ֵ0     0Ϊ�ر� 1Ϊ��
ȥ��ǿ��        defogStrength       Ĭ��ֵ128   [0,255]
��������        disEnable           Ĭ��ֵ0     0Ϊ�ر� 1Ϊ��
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
��ʼʱ��        starttime   char      
����ʱ��        endtime     char
Ԥ��1           reserve1    char 
Ԥ��2           reserve2    char 
*/
typedef struct daynight_schedule_t
{
	char		        strattime[32];	
	char		        endtime[32];	
	char		        reserve1[32];	
	char		        reserve2[32];	
}daynight_schedule_t;

/*
�����ʽ        encmodle          unsigned short    0:H264;1:MPEG4��2:JPEG
���뼶��        enclevel          unsigned short    0:Baseline,1:Main,2:High
�ֱ���	         resolution        unsigned short	(QCIF = 0,qqvag(160*120) = 1,qvga(320*240) = 2,VGA(640*480) = 3,Qcif(176*144) = 4,Cif(352*288)= 5,D1(720*576) = 6,720P(1280*720) = 7,1080P(1960*1080) = 8,960p(1280*960) = 9)	ntp	R/W
֡��	        fps	              unsigned short	1~25 (PAL), 1~30 (NTSC)	������	R/W
����	        bitrate	          unsigned short	16~8000kbps	198.123.30.132	R/W
�������Ʒ�ʽ	bitrate_control	  unsigned short	CBR = 0, VBR = 1	GMT8	R/W
ͼ����������	BaseImageQuality  unsigned short	22~39		
I֡���	        IFrameInterval	  unsigned short	I֡�����1~300	
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
������
�Ƿ�����������   ds_enable_flag    unsigned short
�����ʽ         encmodle          unsigned short   0:H264;1:MPEG4��2:JPEG
���뼶��         enclevel           unsigned short  0:Baseline,1:Main,2:High(h264ר��)
�ֱ���	         resolution        unsigned short	(QCIF = 0,qqvag(160*120) = 1,qvga(320*240) = 2,VGA(640*480) = 3,Qcif(176*144) = 4,Cif(352*288)= 5,D1(720*576) = 6,720P(1280*720) = 7,1080P(1960*1080) = 8,960p(1280*960) = 9)	ntp	R/W
֡��	         fps	           unsigned short	1~25 (PAL), 1~30 (NTSC)	������	R/W
����	         bitrate	       unsigned short	16~8000kbps	198.123.30.132	R/W
�������Ʒ�ʽ	 bitrate_control   unsigned short	CBR = 0, VBR = 1	GMT8	R/W
ͼ����������	 BaseImageQuality  unsigned short	22~39		
I֡���	         IFrameInterval	   unsigned short	I֡�����1~300	
*/
typedef struct v_s_enc_config_t
{
	unsigned char       ds_enable_flag; //˫����������־
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
�Ƿ�������Ƶ����  audio_enable      unsigned short  0;��������1������
��Ƶ����	      audio_format	    unsigned short	PCM=0, G711A=1, G711Mu=2, AAC=3		
������	          audio_sample	    unsigned short	0:8000, 1:16000	
������	          audio_channels	unsigned short	1, 2		
λ��	          audio_bits	    unsigned short	0:8, 1:16,2:64;	
��������          audio_inputvol;   unsigned short  0~100
�������          audio_outputvol;  nsigned short   0~100
�Ƿ��˫������  unsigned short    audio_micspeaker;0;��������1������
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
�Ƿ�����OSD	    osd_enable	     unsigned short	(1,0)	ntp	R/W
ͨ����	        channal_num	     unsigned short	��		
OSD �ı�	    osd_text	     char	��	������	R/W
OSD Xλ��	    osd_x	         unsigned short	��	198.123.30.132	R/W
OSD Yλ��	    osd_y	         unsigned short	��	GMT8	R/W
OSD ��ɫ	    osd_color	     unsigned short	0:black, 1:white, 2:red, 3:green,4: blue,5: yellow��		
��ʾ���ں�ʱ��	time_enable	     unsigned short	(1,0)		
��ʾʱ�� Xλ��	time_x	         unsigned short	��		
��ʾʱ�� yλ��	time_y	         unsigned short	��		
time ��ɫ	    time_color	     unsigned short	0:black, 1:white, 2:red, 3:green,4: blue,5: yellow��
ˮӡ����ʹ��    watermark_enable unsigned short  (1,0)
ˮӡ����        watermake_text   char    
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
����overlay	overlay_enable	unsigned short	(1,0)	ntp	R/W
ͨ����	channal_num	unsigned short	��	������	R/W
overlay Xλ��	overlay_x	unsigned short	��	198.123.30.132	R/W
overlay yλ��	overlay_y	unsigned short	��	GMT8	R/W
overlay width	overlay_width	unsigned short	��		
overlay height	overlay_height	unsigned short	
overlay color ;  0:black, 1:white, 2:red, 3:green,4: blue,5: yellow����		
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
�Ƿ������ƶ����	md_enable	          unsigned short   (1,0)	ntp	R/W
������	            md_sensitivity	      unsigned short   0��~10����10������	198.123.30.132	R/W
����¼��ʹ��        md_alarmrecord        unsigned short   0������¼��������1������¼������
�澯����ʱ��        md_alarmrecycle       unsigned short   ��Ϊ��λ��Ĭ��Ϊ10��  
����ץ��            md_capturetoftp       unsigned short   0����������1������ץ�Ĺ��ܲ���ͼƬ���浽���أ����ftp�����ǿ��еľͰ�ͼƬ�澯ͼƬ�ϴ���ftp
���������ʼ�        md_sendemail          unsigned short   0:�����øĹ��ܣ�1���������ܲ�����ץ��ͼƬ��2���������ܷ���ץ��ͼƬ
io���������        md_alarmout           unsigned short   F:1111 ��ʾ���֧�� ��·���
�ƶ��������	    md_region	          char	           BASE64 ����Ķ���������	������	R/W
��Чʱ��            md_data               int              127��1111111������һ�������죬
ʱ���ʹ��          md_time               int              F:1111 �ĸ�ʱ���ȫ��
���ʱ���һ	    md_time1	          char	           ������Χ�����磺0~86399��00:00:00 ~23:59:59	GMT8	R/W
���ʱ��ζ�	    md_time2	          char	           ������Χ�����磺0~86399��00:00:00 ~23:59:59		
���ʱ�����	    md_time3	          char	           ������Χ�����磺0~86399��00:00:00 ~23:59:59		
���ʱ�����	    md_time4	          char	           ������Χ�����磺0~86399��00:00:00 ~23:59:59		
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
�Ƿ�����IO�澯����	io_enable	          unsigned short   (1,0)	ntp	R/W
�������            io_type	              unsigned short   0:����ߵ�ƽ�澯��1������͵�ƽ�澯
�澯����ʱ��        io_alarmrecycle       unsigned short   ��Ϊ��λ��Ĭ��Ϊ10��   
����¼��ʹ��        io_alarmrecord        unsigned short   0������¼��������1������¼������
����ץ��            io_capturetoftp       unsigned short   0����������1������ץ�Ĺ��ܲ���ͼƬ���浽���أ����ftp�����ǿ��еľͰ�ͼƬ�澯ͼƬ�ϴ���ftp
���������ʼ�        io_sendemail          unsigned short   0:�����øĹ��ܣ�1���������ܲ�����ץ��ͼƬ��2���������ܷ���ץ��ͼƬ
��Чʱ��            io_data               int              127��1111111������һ�������죬
ʱ���ʹ��          io_time               int              F:1111 �ĸ�ʱ���ȫ��
���ʱ���һ	    io_time1	          char	           ������Χ�����磺0~86399��00:00:00 ~23:59:59	GMT8	R/W
���ʱ��ζ�	    io_time2	          char	           ������Χ�����磺0~86399��00:00:00 ~23:59:59		
���ʱ�����	    io_time3	          char	           ������Χ�����磺0~86399��00:00:00 ~23:59:59		
���ʱ�����	    io_time4	          char	           ������Χ�����磺0~86399��00:00:00 ~23:59:59		
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
�ļ��ָ�ģʽ	    record_packetsize	    unsigned short	����Ϊ��λ 5~120��
�Ƿ�����ѭ��¼��	record_recycle	        unsigned short	(1,0)	0;�����ǣ�ֹͣ¼��1������¼��
Ԥ¼��ʱ��          record_advancetime      unsigned short  ��Ϊ��λ5~30
�澯¼��ʱ��        record_alarmtime        unsigned short  ��Ϊ��λ5~300
¼������            record_streantype       unsigned short  0:��������1:������
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

/* �ֶ�¼������ */
typedef struct __hand_record_t
{
	unsigned char	record_hour_len;
	unsigned char	record_minute_len;
	unsigned char	record_status;
	unsigned char unused;
}hand_record_t;

/*
����¼��	     record_enable	unsigned short	(1,0)	ntp	R/W
��Чʱ��      record_data               int              127��1111111������һ�������죬
ʱ���ʹ��    record_time               int              F:1111 �ĸ�ʱ���ȫ��
¼��ʱ���һ	record _ time1	�ַ���	������Χ�����磺0~86399��00:00:00~23:59:59	GMT8	R/W
¼��ʱ��ζ�	record _time2	�ַ���	������Χ�����磺0~86399��00:00:00~23:59:59		
¼��ʱ�����	record _time3	�ַ���	������Χ�����磺0~86399��00:00:00~23:59:59		
¼��ʱ�����	record _time4	�ַ���	������Χ�����磺0~86399��00:00:00~23:59:59		
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
�Ƿ������ֻ�	sip_enable	        unsigned short	(1,0)
��¼�û���   	sip_user	        char
��¼����    	sip_password	    char
��������ַһ     sip_ServerPath1	char
��������ַ��	sip_ServerPath2	    char
�󶨸�λ���     sip_resetflag       int
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
�Ƿ�������̨	ptz_enable	    unsigned short	(1,0)
��̨�豸��ַ	ptz_address	    unsigned short	0~255
����Э���ַ	ptz_protocol	unsigned short	0:Pelco D;1:Pelco P
������	        ptz_bandrate	unsigned short	0:1200,1:2400;2:4800;3:9600;4:14400;5:19200;6:38400;7:57600;8:115200
У��λ	        ptz_parity	    unsigned short	0:none, 1:odd, 2:even		
����λ	        ptz_databit	    unsigned short	7, 8		
ֹͣλ	        ptz_stopbit	    unsigned short	1, 2		
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
�û�ʹ��    user_enable        unsigned short     0��δʹ�ã�1������
�û���	    user_name	       char		          ntp	R/W
�û�����	user_passwd	       char	��	          ������	R/W
�û�����    user_description   char
�û�Ȩ��    user_jurisdiction  int                ���Ϊ��0xffff����Ϊ0xffffΪ���Ȩ��      
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
��������         factory_name                 char
�汾��           version_number               char    
��Ʒ�ͺ�         device_type	          	  char		         
�汾����         version_date	          	  char	��	          
������ֱ���   Max_Support_Resolution       char
��Ʒ���к�       device_serial_no             char               
�ں˰汾��       kernel_version               char
WEB�İ汾��      web_version                  char              
*/
typedef struct  _system_Info_t
{
    char    factory_name[VERSION_LEN];             /* �������� */
    char    version_number[VERSION_LEN];           /* �汾�� */
    char    device_type[VERSION_LEN];              /* ��Ʒ�ͺ� */
    char    version_date[VERSION_LEN];             /* �汾���� */
    char    Max_Support_Resolution[VERSION_LEN];   /* ������ֱ��� */
    char    device_serial_no[32];                  /* ��Ʒ���к� */
    char    kernel_version[VERSION_LEN];           /* �ں˰汾�� */
    char    web_version[VERSION_LEN];              /* WEB�İ汾�� */
    char    Model[128];                            /* ��Ʒģ����Ϣ */
    char    HardwareId[32];                        /* ��ƷӲ���ɣ� */
    char    ProductHardVersion[32];                /* Ӳ���汾 */
}system_Info_t;

typedef struct  _gb28181_config_t
{
	char            chEnable;                                  /* ����28181ʹ�ܿ��� Ĭ�� ��*/
	char            chAlarmLevel;                              /* �������� ȡֵ1~6 Ĭ��2 */
	char            szSipServerID[24];                         /* SIP���������� Ĭ�� 34020000002000000001 */
	char            szSipDomain[16];                           /* SIP��       Ĭ��  3402000000 */
	char            szSipServerIP[24];                         /* SIP������IP Ĭ�� 192.168.10.124 */
	unsigned short  nSipServerPort;                            /* SIP�������˿� Ĭ�� 5060*/
	char            szSipDeviceID[24];                         /* �豸���� Ĭ�� 34020000001320000001*/
	char            szSipAlarmID[24];                          /* �豸�澯���� Ĭ�� 34020000001340000001*/
	char            szPassword[32];                            /* ע������ Ĭ�� 12345678*/
	char            szSipChannelID[24];                        /* ͨ������ Ĭ��34020000001320000001*/
	char            szOwner[24];                               /* �豸����   Ĭ�� NULL*/
	char            szCivilCode[24];                           /* ��������   Ĭ�� NULL*/
	char            szBlock[24];                               /* ����      Ĭ�� NULL*/
	char            szAddress[24];                             /* ��װ��ַ   Ĭ�� NULL*/
	char            szLongitude[24];                           /* ����      Ĭ�� 0.0 */
	char            szLatitude[24];                            /* γ��      Ĭ�� 0.0 */
	unsigned int    nExpires;                                  /* ע����Ч��(��) Ĭ��3600 */
	unsigned int    nAlive;                                    /* ��������(��)      Ĭ��60*/
	unsigned int    nTimeoutNum;                               /* ���������ʱ���� Ĭ�� 3 */
}gb28181_config_t;

#define MAX_ALARM_NUM           (256)
#define MAX_ALARM_STRCH         (2 * 17)
typedef struct {
  short   m_chnum;                                        /* ��Чͨ������ */
  short   m_fontsize;                                     /* 0->32*32, 1->48*48, 2->64*64 */
  short   m_interval;                                     /* ˢ��ʱ���� */
  char    m_strch[MAX_ALARM_NUM][MAX_ALARM_STRCH];        /* ÿ��ͨ�������ʾ16�����֣�32��ASCII  */
  short   m_reserve;
} alarm_config_t;

typedef struct param {
  char 			            param_exist_flag[FLAG_OFFSET]; //����һ���汾���Ƶı�ǣ�������չ�ṹ���ʱ����Ҫ�޸ĸñ�ǣ�
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
������������
���Ի�ParamLib�⡣
��������
Null;
������ֵ��
����ֵ	����
0	�ɹ�
��0	ʧ�ܡ�
*/
int ParamLib_InitParamLib(void);



/*
������������
�����Ի�ParamLib�⡣
��������
Null;
������ֵ��
����ֵ	����
0	�ɹ�
��0	ʧ�ܡ�
*/
int ParamLib_deinitParamLib(void);


/*
������������
����һ�����������޸ķ���
��������
Null;
������ֵ��
����ֵ	����
0	�ɹ�
��0	ʧ�ܡ�
*/
int ParamLib_StartParamService(void);


/*
������������
ֹͣһ�����������޸ķ���
��������
Null;
������ֵ��
����ֵ	����
0	�ɹ�
��0	ʧ�ܡ�
*/

int ParamLib_StopParamService(void);

/************************************************************************/
/*                           DateTime                                   */
/************************************************************************/
/*
������������
����ϵͳ�����ں�ʱ�䡣
��������
��������	����	����/���
datetime	�������ں�ʱ��Ľṹ��	����
������ֵ��
����ֵ	����
0	�ɹ�
��0	ʧ�ܡ�
*/
int ParamLib_SetDateTimeInfo(datetime_config_t datetime);

/*
������������
��ȡϵͳ�����ں�ʱ���������á�

��������
��������	����	����/���
datetime	�������ں�ʱ��Ľṹ��	���
������ֵ��
����ֵ	����
0	�ɹ�
��0	ʧ�ܡ�
*/
int ParamLib_GetDateTimeInfo(datetime_config_t* datetime);
int ParamLib_SetDSTInfo(DST_config_t dstinfo);
int ParamLib_GetDSTInfo(DST_config_t* dstinfo);

/************************************************************************/
/*                           NetWork                                    */
/************************************************************************/
/*
������������
�����豸��·��������á�
��������
��������	����	����/���
networkinfo	�����������õĽṹ��	����
������ֵ��
����ֵ	����
0	�ɹ�
��0	ʧ�ܡ�
*/

int ParamLib_SetNetWorkInfo(network_config_t networkinfo);

/*
������������
��ȡ�豸��·��������á�
��������   
��������	����	����/���
networkinfo	�����������õĽṹ��	���
������ֵ��
����ֵ	����
0	�ɹ�
��0	ʧ�ܡ�
*/
int ParamLib_GetNetWorkInfo(network_config_t *networkinfo);


/************************************************************************/
/*                           NetWork                                    */
/************************************************************************/
/*
������������
��ȡ������·��������á�
��������   
��������	����	����/���
wirelessinfo	���������������õĽṹ��	���
������ֵ��
����ֵ	����
0	�ɹ�
��0	ʧ�ܡ�
*/
void ParamLib_GetWirelessInfo(wireless_config_t *wirelessinfo);

/*
������������
����������·��������á�
��������   
��������	����	����/���
wirelessinfo	���������������õĽṹ��	����
������ֵ��
����ֵ	����
0	�ɹ�
��0	ʧ�ܡ�
*/
void ParamLib_SetWirelessInfo(wireless_config_t wirelessinfo);


/************************************************************************/
/*                           MAIL                                       */
/************************************************************************/

/*
������������
���õ����ʼ���������á�
��������
��������	����	                        ����/���
mailinfo	���õ����ʼ���������õĽṹ��	����
������ֵ��
����ֵ	����
0	�ɹ�
��0	ʧ�ܡ�
*/

int ParamLib_SetEmailInfo(email_config_t emailinfo);


/*
������������
��ȡ�����ʼ���������á�
��������   
��������	����	����/���
mailinfo	��������ʼ����õĽṹ��	���

������ֵ��
����ֵ	����
0	�ɹ�
��0	ʧ�ܡ�
*/
int ParamLib_GetEmailInfo(email_config_t *emailinfo);


/************************************************************************/
/*                           FTP                                       */
/************************************************************************/

/*
������������
����FTP������á�
��������
��������	����	                        ����/���
mailinfo	����FTP��������õĽṹ��	    ����
������ֵ��
����ֵ	����
0	�ɹ�
��0	ʧ�ܡ�
*/

int ParamLib_SetFTPInfo(ftp_config_t ftpinfo);


/*
������������
��ȡFTP��������á�
��������   
��������	����	����/���
mailinfo	����FTP���õĽṹ��	���

������ֵ��
����ֵ	����
0	�ɹ�
��0	ʧ�ܡ�
*/
int ParamLib_GetFTPInfo(ftp_config_t *ftpinfo);

/************************************************************************/
/*                          PPPOE                                       */
/************************************************************************/
/*
������������
�����豸PPPOE���ŵ�������á�
��������
��������	����	����/���
pppoeinfo	����PPPOE�������õĽṹ��	����
������ֵ��
����ֵ	����
0	�ɹ�
��0	ʧ�ܡ�
*/
int ParamLib_SetPPPOEInfo(pppoe_config_t  pppoeinfo);

/*
������������
��ȡ�豸PPPOE���ŵ�������á���
��������   
��������	����	����/���
pppoeinfo	�����������õĽṹ��	���

������ֵ��
����ֵ	����
0	�ɹ�
��0	ʧ�ܡ�
*/
int ParamLib_GetPPPOEInfo(pppoe_config_t * pppoeinfo);

/*
������������
�����豸DDNS��������á�
��������
��������	����	����/���
ddnsinfo	����DDNS���õĽṹ��	����
������ֵ��
����ֵ	����
0	�ɹ�
��0	ʧ�ܡ�
*/
int ParamLib_SetDDNSInfo(ddns_config_t  ddnsinfo);


/*
������������
��ȡ�豸PPPOE���ŵ�������á���
��������   
��������	����	����/���
ddnsinfo	����DDNS���õĽṹ��	���
������ֵ��
����ֵ	����
0	�ɹ�
��0	ʧ�ܡ�
*/
int ParamLib_GetDDNSInfo(ddns_config_t * ddnsinfo);



/*
������������
�����豸Sensor��������á�
��������
��������	����	����/���
channal_num ͨ����                  ���� 
sensorinfo	����Sensor���õĽṹ��	����
������ֵ��
����ֵ	����
0	�ɹ�
��0	ʧ�ܡ�
*/
int ParamLib_SetSensorInfo(unsigned short channal_num,sensor_config_t  sensorinfo);

/*
������������
��ȡ�豸Sensor��������á���
��������   
��������	����	����/���
channal_num ͨ����                  ���� 
sensorinfo	����Sensor���õĽṹ��	���
������ֵ��
����ֵ	����
0	�ɹ�
��0	ʧ�ܡ�
*/
int ParamLib_GetSensorInfo(unsigned short channal_num,sensor_config_t * sensorinfo);

/*
������������
�����豸isp��������á�
��������
��������	����	����/���
channal_num ͨ����                  ���� 
sensorinfo	����isp���õĽṹ��	����
������ֵ��
����ֵ	����
0	�ɹ�
��0	ʧ�ܡ�
*/
int ParamLib_SetIspInfo(unsigned short channal_num,isp_config_t  ispinfo);


/*
������������
��ȡ�豸isp��������á���
��������   
��������	����	����/���
channal_num ͨ����                  ���� 
sensorinfo	����isp���õĽṹ��	���
������ֵ��
����ֵ	����
0	�ɹ�
��0	ʧ�ܡ�
*/
int ParamLib_GetIspInfo(unsigned short channal_num,isp_config_t *ispinfo);

/*
������������
�������ð����ҹģʽ��ʱ���
��������   
��������	����	����/���
channal_num ͨ����                  ���� 
daynightschedule	��������ҹģʽ�л���ʱ��� ���õĽṹ��	���
������ֵ��
����ֵ	����
0	�ɹ�
��0	ʧ�ܡ�
*/
int ParamLib_SetDayNightSchedule(unsigned short channal_num,daynight_schedule_t daynightschedule);

/*
������������
��ȡ���ð����ҹģʽ��ʱ���
��������   
��������	����	����/���
channal_num ͨ����                  ���� 
daynightschedule	��������ҹģʽ�л���ʱ��� ���õĽṹ��	���
������ֵ��
����ֵ	����
0	�ɹ�
��0	ʧ�ܡ�
*/
int ParamLib_GetDayNightSchedule(unsigned short channal_num,daynight_schedule_t * daynightschedule);

/*������������
�����豸��Ƶ�����������á�
��������
��������	����	����/���
channal_num ͨ����      ���� 
��Ƶ�������õĽṹ��	����
������ֵ��
����ֵ	����
0	�ɹ�
��0	ʧ�ܡ�
*/
int ParamLib_SetVEncInfo(unsigned short channal_num,v_enc_config_t  v_encinfo);

/*
������������
��ȡ�豸��Ƶ�����������á���
��������   
��������	����	����/���
channal_num ͨ����      ���� 
��Ƶ�������õĽṹ��	���
������ֵ��
����ֵ	����
0	�ɹ�
��0	ʧ�ܡ�
*/
int ParamLib_GetVEncInfo(unsigned short channal_num,v_enc_config_t *v_encinfo);

/*
������������
��ȡ�豸��Ƶ�����������á���
��������   
��������	����	����/���
channal_num ͨ����      ���� 
��Ƶ�������õĽṹ��	���
������ֵ��
����ֵ	����
0	�ɹ�
��0	ʧ�ܡ�
*/
int ParamLib_GetVEncInfo_EX(unsigned short channal_num,unsigned streamtype,v_enc_config_t *v_encinfo);


/*������������
�����豸�����������������á�
��������
��������	����	����/���
channal_num ͨ����      ���� 
��Ƶ�������õĽṹ��	����
������ֵ��
����ֵ	����
0	�ɹ�
��0	ʧ�ܡ�
*/

int ParamLib_SetSVEncInfo(unsigned short channal_num,v_s_enc_config_t  v_s_encinfo);

/*
������������
��ȡ����������������á���
��������   
��������	����	����/���
channal_num ͨ����      ���� 
��Ƶ�������õĽṹ��	���
������ֵ��
����ֵ	����
0	�ɹ�
��0	ʧ�ܡ�
*/

int ParamLib_GetSVEncInfo(unsigned short channal_num,v_s_enc_config_t *v_s_encinfo);


/*������������
�����豸��Ƶ�����������á�
��������
��������	����	����/���
channal_num ͨ����      ���� 
��Ƶ�������õĽṹ��	����
������ֵ��
����ֵ	����
0	�ɹ�
��0	ʧ�ܡ�
*/
int ParamLib_SetAEncInfo(unsigned short channal_num,a_enc_config_t  a_encinfo);

/*
������������
��ȡ�豸��Ƶ�����������á���
��������   
��������	����	����/���
channal_num ͨ����      ���� 
��Ƶ�������õĽṹ��	���
������ֵ��
����ֵ	����
0	�ɹ�
��0	ʧ�ܡ�
*/
int ParamLib_GetAEncInfo(unsigned short channal_num,a_enc_config_t *a_encinfo);



/*
������������
�����豸OSD��������á�
��������
��������	����	����/���
channal_num ͨ����                  ���� 
osdinfo	����OSD���õĽṹ��	����
������ֵ��
����ֵ	����
0	�ɹ�
��0	ʧ�ܡ�
*/
int ParamLib_SetOSDInfo(unsigned short channal_num,osd_config_t  osdinfo);


/*
������������
��ȡ�豸OSD��������á���
��������   
��������	����	����/���
channal_num ͨ����                  ���� 
osdinfo	����OSD���õĽṹ��	���
������ֵ��
����ֵ	����
0	�ɹ�
��0	ʧ�ܡ�
*/
int ParamLib_GetOSDInfo(unsigned short channal_num,osd_config_t * osdinfo);

int ParamLib_SetOSDInfo_Ex(unsigned short channal_num, osd_config_t_EX osdinfo);
int ParamLib_GetOSDInfo_Ex(unsigned short channal_num, osd_config_t_EX *osdinfo);

/*
������������
�����豸Overlay��������á�
��������
��������	����	����/���
channal_num ͨ����                  ���� 
Overlayinfo	����Overlay���õĽṹ��	����
������ֵ��
����ֵ	����
0	�ɹ�
��0	ʧ�ܡ�
*/
int ParamLib_SetOverlayInfo(unsigned short channal_num,unsigned short overlayno,overlay_config_t  Overlayinfo);


/*
������������
��ȡ�豸Overlay��������á���
��������   
��������	����	����/���
channal_num ͨ����                  ���� 
Overlayinfo	����Overlay���õĽṹ��	���
������ֵ��
����ֵ	����
0	�ɹ�
��0	ʧ�ܡ�
*/
int ParamLib_GetOverlayInfo(unsigned short channal_num,unsigned short overlayno,overlay_config_t * Overlayinfo);

/*
4.1.3.22  ParamLib_SetMDInfo
������������
�����豸�ƶ�����������á�
��������
��������	����	����/���
channal_num ͨ����                  ���� 
mdinfo	�����ƶ�������õĽṹ��	����
������ֵ��
����ֵ	����
0	�ɹ�
��0	ʧ�ܡ�
*/
int ParamLib_SetMDInfo(unsigned short channal_num,md_config_t  mdinfo);

/*
������������
��ȡ�豸�ƶ�����������á���
��������   
��������	����	����/���
channal_num ͨ����                  ���� 
mdinfo	�����ƶ�������õĽṹ��	���
������ֵ��
����ֵ	����
0	�ɹ�
��0	ʧ�ܡ�
*/
int ParamLib_GetMDInfo(unsigned short channal_num,md_config_t * mdinfo);


/*
4.1.3.22  ParamLib_SetAutoMaintain
������������
����ϵͳ�Զ�ά����������á�
��������
��������	����	����/���
channal_num ͨ����                  ���� 
mdinfo	�����ƶ�������õĽṹ��	����
������ֵ��
����ֵ	����
0	�ɹ�
��0	ʧ�ܡ�
*/
int ParamLib_SetAutoMaintain(auto_maintain_t  automaintain);

/*
������������
��ȡϵͳ�Զ�ά����������á���
��������   
��������	����	����/���
channal_num ͨ����                  ���� 
mdinfo	�����ƶ�������õĽṹ��	���
������ֵ��
����ֵ	����
0	�ɹ�
��0	ʧ�ܡ�
*/
int ParamLib_GetAutoMaintain(auto_maintain_t * automaintain);

/*
4.1.3.22  ParamLib_SetIOAlarmInfo
������������
�����豸IO�澯����������á�
��������
��������	����	              ����/���
channal_num ͨ����                  ���� 
mdinfo	�澯���õĽṹ��	        ����
������ֵ��
����ֵ	����
0	�ɹ�
��0	ʧ�ܡ�
*/
int ParamLib_SetIOAlarmInfo(unsigned short channal_num,io_config_t ioinfo);

/*
������������
��ȡ�豸IO�澯����������á���
��������   
��������	   ����	                 ����/���
channal_num    ͨ����                  ���� 
ioinfo         �澯���õĽṹ��	       ���
������ֵ��
����ֵ	����
0	�ɹ�
��0	ʧ�ܡ�
*/
int ParamLib_GetIOAlarmInfo(unsigned short channal_num,io_config_t *ioinfo);


/*
������������
�����豸¼��Ļ������á�
��������
��������	����	����/���
channal_num ͨ����                  ���� 
recordbaseinfo	�����豸¼��Ļ������õĽṹ��	����
������ֵ��
����ֵ	����
0	�ɹ�
��0	ʧ�ܡ�
*/
int ParamLib_SetRecordBaseInfo(recordbase_config_t  recordbaseinfo);


/*
������������
��ȡ�豸¼��Ļ������á���
��������   
��������	����	����/���  
recordbaseinfo	�����ֶ�¼��״̬�Ľṹ��	���
������ֵ��
����ֵ	����
0	�ɹ�
��0	ʧ�ܡ�
*/
int ParamLib_GetRecordBaseInfo(recordbase_config_t * recordbaseinfo);


/*
������������
���û�ȡ�ֶ�¼��״̬���á�
��������
��������	����	����/���
channal_num ͨ����                  ���� 
handrecord	�����豸¼��Ļ������õĽṹ��	����
������ֵ��
����ֵ	����
0	�ɹ�
��0	ʧ�ܡ�
*/
void ParamLib_GetHandRecordStruct(int channal_num, hand_record_t *handrecord);


/*
������������
�����ֶ�¼��״̬���á�
��������
��������	����	����/���
channal_num ͨ����                  ���� 
handrecord	�����ֶ�¼��״̬���õĽṹ��	����
������ֵ��
����ֵ	����
0	�ɹ�
��0	ʧ�ܡ�
*/
void ParamLib_SetHandRecordStruct(int channal_num, hand_record_t handrecord);


/*
������������
�����豸¼��ƻ��Ļ������á�

��������
��������	����	����/���
recordscheduleinfo	�����豸¼��ƻ��Ļ������õĽṹ��	����
������ֵ��
����ֵ	����
0	�ɹ�
��0	ʧ�ܡ�
*/
int ParamLib_SetRecordScheduleInfo(unsigned short channal_num,recordschedule_config_t  recordscheduleinfo);


/*
������������
�����豸�����ϵͳ��Ϣ��
��������
��������	����	����/���
systeminfo	�����豸�����ϵͳ��Ϣ�Ľṹ��	����
������ֵ��
����ֵ	����
0	�ɹ�
��0	ʧ�ܡ�
*/
int ParamLib_GetSystemInfo(system_Info_t *systeminfo);

/*
������������
��ȡ���ϵͳ��Ϣ��
��������
��������	����	����/���
systeminfo	�����豸�����ϵͳ��Ϣ�Ľṹ��	����
������ֵ��
����ֵ	����
0	�ɹ�
��0	ʧ�ܡ�
*/
int ParamLib_SetSystemInfo(system_Info_t systeminfo);


/*
������������
��ȡ�豸¼��ƻ��Ļ������á���
��������   
��������	����	����/���
channal_num ͨ����                  ���� 
recordscheduleinfo	�����豸¼��ƻ��Ļ������õĽṹ��	���
������ֵ��
����ֵ	����
0	�ɹ�
��0	ʧ�ܡ�
*/
int ParamLib_GetRecordScheduleInfo(unsigned short channal_num,recordschedule_config_t * recordscheduleinfo);


/*
������������
�����豸PTZ�Ļ������á�
��������
��������	����	����/���
channal_num ͨ����                  ���� 
ptzinfo	�����豸PTZ���õĽṹ��	����
������ֵ��
����ֵ	����
0	�ɹ�
��0	ʧ�ܡ�
*/
int ParamLib_SetPTZInfo(ptz_config_t  ptzinfo);


/*
������������
��ȡ�豸PTZ�Ļ������á���
��������   
��������	����	����/���
ptzinfo	����PTZ���õĽṹ��	���
������ֵ��
����ֵ	����
0	�ɹ�
��0	ʧ�ܡ�
*/
int ParamLib_GetPTZInfo(ptz_config_t * ptzinfo);
int ParamLib_SetSIPInfo(sip_config_t  sipinfo);
int ParamLib_GetSIPInfo(sip_config_t * sipinfo);

/*
������������
��ȡϵͳ�����еĻ������á���
��������   
��������	����	                    ����/���
sysconfig	����ϵͳ�������õĽṹ��	���
������ֵ��
����ֵ	����
0	�ɹ�
��0	ʧ�ܡ�
*/
int ParamLib_GetSysConfig(sys_config_t *sysconfig);


/*
������������
����ϵͳ�����еĻ������á�
��������   
��������	����	                    ����/���
sysconfig	����ϵͳ�������õĽṹ��	���
������ֵ��
����ֵ	����
0	�ɹ�
��0	ʧ�ܡ�
*/
int ParamLib_SetSysConfig(sys_config_t sysconfig);


/*
������������
�����豸�û��Ļ������á�
��������
��������	����	                ����/���
userinfo	�����û����õĽṹ��	����
������ֵ��
����ֵ	����
0	�ɹ�
��0	ʧ�ܡ�
*/
int  ParamLib_SetUserInfo(char *userinfo);

/*
������������
��ȡ�豸�û��Ļ������á���
��������   
��������	����	                    ����/���
userinfo	���������û�����Ϣ�Ľṹ��	    ���
������ֵ��
����ֵ	����
0	�ɹ�
��0	ʧ�ܡ�
*/
int ParamLib_GetUserInfo(char *userinfo);


/*
������������
��ȡ�豸�û��Ļ������á���
��������   
��������	����	                    ����/���
userinfo	�����������Ӷ˿ڵĽṹ��	���
������ֵ��
����ֵ	����
0	�ɹ�
��0	ʧ�ܡ�
*/
int ParamLib_GetPortInfo(network_port_config_t * portinfo);

/*
������������
�����������Ӷ˿����á�
��������
��������	����	                    ����/���
portinfo	�����������Ӷ˿ڵĽṹ��	����
������ֵ��
����ֵ	����
0	�ɹ�
��0	ʧ�ܡ�
*/
int ParamLib_SetPortInfo(network_port_config_t  portinfo);


/*
������������
����GB28181Э��������á�
��������
��������	����	                    ����/���
gb28181_info	����GB28181Э��Ľṹ��	����
������ֵ��
����ֵ	����
0	�ɹ�
��0	ʧ�ܡ�
*/
int ParamLib_SetGB28181Info(gb28181_config_t  gb28181_info);

/*
������������
��ȡGB28181Э��������á�
��������   
��������	����	                    ����/���
gb28181_info	��GB28181Э��Ľṹ��	���
������ֵ��
����ֵ	����
0	�ɹ�
��0	ʧ�ܡ�
*/
int ParamLib_GetGB28181Info(gb28181_config_t *gb28181_info);

/*
������������
����IPC�Ĳ���
��������   
NULL
������ֵ��
����ֵ	����
0	�ɹ�
��0	ʧ�ܡ�
*/
int ParamLib_BackUpParam(void);

/*
������������
���Ѿ����ݵĲ�����ԭ����
��������   
NULL
������ֵ��
����ֵ	����
0	�ɹ�
��0	ʧ�ܡ�
*/
int ParamLib_ResumeParam(void);

/*
������������
�ָ�IPC�ĳ�������
��������   
������ֵ��
����ֵ	����
0	�ɹ�
��0	ʧ�ܡ�
*/
int ParamLib_ResumeFactoryParam(void);


/*
������������
��ȡ�豸�����в����Ļ������á���
��������   
��������	����	                    ����/���
paraminfo  	�����豸���в����Ľṹ�� ���
������ֵ��
����ֵ	����
0	�ɹ�
��0	ʧ�ܡ�
*/
int ParamLib_GetParamInfo(param *paraminfo);

/*
������������
��ȡ�豸�����в����Ļ������á���
��������   
��������	����	                    ����/���
paraminfo  	�����豸���в����Ľṹ�� ���
������ֵ��
����ֵ	����
0	�ɹ�
��0	ʧ�ܡ�
*/
int ParamLib_SetParamInfo(param paraminfo);

/*
������������
��ȡ�������ں�ʱ��
��������   
��������	����	                    ����/���
szDate  	��������                    ���
szTime      ����ʱ��                    ���
������ֵ��
����ֵ	����
0	�ɹ�
��0	ʧ�ܡ�
*/
int ParamLib_GetCompileDateTime(char *szDate, char *szTime);

int ParamLib_GetSerialInfo(char *serialNum);

int ParamLib_SetSerialInfo(char *serialNum);

int ParamLib_SetAlarmInfo(const alarm_config_t * alarm_info);

int ParamLib_GetAlarmInfo(alarm_config_t * alarm_info);

int ParamLib_RefreshDeviceInfo(void);

//�����ȡһ��ͬ��������flash�ı�ǣ���ֹ��д���ݵ������ļ��Ĺ����а��ļ���д����
int GetParamDirty(void);
#endif

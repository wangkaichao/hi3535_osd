#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>
#include <sys/mman.h>

#include "sample_comm.h"

#include <linux/fb.h>
#include "hifb.h"
#include "loadbmp.h"
#include "hi_tde_api.h"
#include "hi_tde_type.h"
#include "hi_tde_errcode.h"

#include "paramLib.h"
#include "commLib.h"
#include "NetLib.h"

static struct fb_bitfield s_r16 = {10, 5, 0};
static struct fb_bitfield s_g16 = {5, 5, 0};
static struct fb_bitfield s_b16 = {0, 5, 0};
static struct fb_bitfield s_a16 = {15, 1, 0};

static struct fb_bitfield s_a32 = {24,8,0};
static struct fb_bitfield s_r32 = {16,8,0};
static struct fb_bitfield s_g32 = {8,8,0};
static struct fb_bitfield s_b32 = {0,8,0};

#define WIDTH                   1920
#define HEIGHT                  1080
#define SAMPLE_IMAGE_WIDTH      1440
#define SAMPLE_IMAGE_HEIGHT     100
#define SAMPLE_IMAGE_SIZE       (1440*100*2)
#define HIFB_RED_1555           0xF000//0xFC00
#define HIFB_GREEN_1555         0x83E0
#define HIFB_BLUE_1555          0x801F
#define HIFB_WHITE_1555         0xFFFF
#define HIFB_LINE_1555          0x32FF // 0xB2FF

//63B8FF R->63 G->B8 B->FF   256->32 *8
//555    R->C G->17 B->1F
//    1  01100 10111 11111 -> B2FF


#define GRAPHICS_LAYER_G0       0
#define GRAPHICS_LAYER_G1       1
#define HEAD_IMAGE_PATH         "/opt/user/res/head.bmp"

typedef struct hiPTHREAD_HIFB_SAMPLE
{
  int fd;
  int ctrlkey;
  int isrun;
  pthread_t pid;
}PTHREAD_HIFB_SAMPLE_INFO;

static VO_DEV VoDev = SAMPLE_VO_DEV_DHD0;
static PTHREAD_HIFB_SAMPLE_INFO gstInfo = {-1, 0, 0, -1};

static HI_VOID DrawBox(HI_S32 x, HI_S32 y, HI_S32 w, HI_S32 h, HI_U32 u32Stride, HI_CHAR* pcMem, HI_U32 u32Color);
static HI_S32 LoadBmp(const char *filename, HI_U8 *pAddr);
static int InitHifb(void);
static void *osd_thread(void *param);
static int osd_create(unsigned char *str, int fontsize, unsigned short color, HI_U32 *Phyaddr, HI_VOID **Viraddr, HI_U32 *pwidth, HI_U32 *pheight);

static HI_VOID DrawBox(HI_S32 x, HI_S32 y, HI_S32 w, HI_S32 h, HI_U32 u32Stride, HI_CHAR* pcMem, HI_U32 u32Color)
{
    HI_S32 i, j;
    HI_U32* puVideo = (HI_U32*)pcMem;
    for (i = y; i < y + h; i++)
    {
        puVideo = (HI_U32*)(pcMem + i * u32Stride);
        for (j = x; j < x + w; j++)
        {
            *(puVideo + j) = u32Color;
        }
    }
}

static HI_S32 LoadBmp(const char *filename, HI_U8 *pAddr)
{
  OSD_SURFACE_S Surface;
  OSD_BITMAPFILEHEADER bmpFileHeader;
  OSD_BITMAPINFO bmpInfo;

  if (GetBmpInfo(filename, &bmpFileHeader, &bmpInfo) < 0) {
    SAMPLE_PRT("GetBmpInfo err!\n");
    return HI_FAILURE;
  }

  Surface.enColorFmt = OSD_COLOR_FMT_RGB1555;
  CreateSurfaceByBitMap(filename, &Surface, pAddr);
  return HI_SUCCESS;
}

static int InitHifb(void)
{
  char file[12] = "/dev/fb0";
  HIFB_COLORKEY_S stColorKey;
  struct fb_var_screeninfo stVarInfo;
  HIFB_POINT_S stPoint = {0};
  HIFB_LAYER_INFO_S stLayerInfo = {0};
  HIFB_ALPHA_S stAlphaInfo = {0};
    
  /* 1. open framebuffer device overlay 0 */
  gstInfo.fd = open(file, O_RDWR, 0);
  
  if (gstInfo.fd < 0) {
    SAMPLE_PRT("open %s failed!\n", file);
    return -1;
  }
  
	/*all layer surport colorkey*/  
	stColorKey.bKeyEnable = HI_TRUE;
	stColorKey.u32Key = HIFB_WHITE_1555;
	if (ioctl(gstInfo.fd, FBIOPUT_COLORKEY_HIFB, &stColorKey) < 0)
	{
    	SAMPLE_PRT("FBIOPUT_COLORKEY_HIFB!\n");
    	close(gstInfo.fd);
    	return -1;
	}

  if (ioctl(gstInfo.fd, FBIOGET_ALPHA_HIFB, &stAlphaInfo) < 0) {
      SAMPLE_PRT("FBIOGET_ALPHA_HIFB!\n");
    	close(gstInfo.fd);
    	return -1;
  }
  
  logDbg("%d %d %d %d", stAlphaInfo.bAlphaEnable, stAlphaInfo.bAlphaChannel, stAlphaInfo.u8Alpha0, stAlphaInfo.u8Alpha1);
  stAlphaInfo.u8Alpha0 = 96;
  if (ioctl(gstInfo.fd, FBIOPUT_ALPHA_HIFB, &stAlphaInfo) < 0) {
      SAMPLE_PRT("FBIOGET_ALPHA_HIFB!\n");
    	close(gstInfo.fd);
    	return -1;
  }
  
	if (ioctl(gstInfo.fd, FBIOGET_VSCREENINFO, &stVarInfo) < 0)
	{
		SAMPLE_PRT("GET_VSCREENINFO failed!\n");
    close(gstInfo.fd);
		return -1;
	}
	
  if (ioctl(gstInfo.fd, FBIOPUT_SCREEN_ORIGIN_HIFB, &stPoint) < 0)
  {
    SAMPLE_PRT("set screen original show position failed!\n");
    close(gstInfo.fd);
    return -1;
  }
  stVarInfo.transp= s_a16;
  stVarInfo.red = s_r16;
  stVarInfo.green = s_g16;
  stVarInfo.blue = s_b16;
  stVarInfo.bits_per_pixel = 16;
  stVarInfo.activate = FB_ACTIVATE_NOW;
  stVarInfo.xres = stVarInfo.xres_virtual = WIDTH;
  stVarInfo.yres = stVarInfo.yres_virtual = HEIGHT;
  if (ioctl(gstInfo.fd, FBIOPUT_VSCREENINFO, &stVarInfo) < 0) {
    SAMPLE_PRT("PUT_VSCREENINFO failed!\n");
    close(gstInfo.fd);
    return -1;
  }

  switch (gstInfo.ctrlkey) {
    case 0 :
    {
      stLayerInfo.BufMode = HIFB_LAYER_BUF_ONE;
      stLayerInfo.u32Mask = HIFB_LAYERMASK_BUFMODE;
      break;
    }

    case 1 :
    {
      stLayerInfo.BufMode = HIFB_LAYER_BUF_DOUBLE;
      stLayerInfo.u32Mask = HIFB_LAYERMASK_BUFMODE;
      break;
    }

    default:
    {
      stLayerInfo.BufMode = HIFB_LAYER_BUF_NONE;
      stLayerInfo.u32Mask = HIFB_LAYERMASK_BUFMODE;
      break;
    }		
  }
  
  if(ioctl(gstInfo.fd, FBIOPUT_LAYER_INFO, &stLayerInfo) < 0) {
    SAMPLE_PRT("PUT_LAYER_INFO failed!\n");
    close(gstInfo.fd);
    return -1;
  }
  
	HI_BOOL bShow = HI_TRUE;
  if (ioctl(gstInfo.fd, FBIOPUT_SHOW_HIFB, &bShow) < 0)
  {
    SAMPLE_PRT("FBIOPUT_SHOW_HIFB failed!\n");
    close(gstInfo.fd);
    return -1;
  }

  return 0;
}

static void *osd_thread(void *param)
{
  HI_S32 s32Ret = HI_SUCCESS; 
  HIFB_BUFFER_S stCanvasBuf;
  HI_U16 *pBuf;
  HI_U32 maxW, maxH;
  TDE2_RECT_S stSrcRect,stDstRect;
  TDE2_SURFACE_S stSrc,stDst;
  HI_U32 Phyaddr;
  HI_VOID *Viraddr;
  TDE_HANDLE s32Handle;
  alarm_config_t old_alarm;
  alarm_config_t new_alarm;
  net_alarm_info_t old_netalarm;
  net_alarm_info_t new_netalarm;
  
  memset(&old_alarm, 0, sizeof(alarm_config_t));
  memset(&new_alarm, 0, sizeof(alarm_config_t));
  memset(&old_netalarm, 0, sizeof(net_alarm_info_t));
  memset(&new_netalarm, 0, sizeof(net_alarm_info_t));

  maxW = WIDTH;
  maxH = HEIGHT;
  
  /* full screen surface buffer */
  if (HI_FAILURE == HI_MPI_SYS_MmzAlloc(&(stCanvasBuf.stCanvas.u32PhyAddr), ((void**)&pBuf), NULL, NULL, maxW*maxH*2)) {
    SAMPLE_PRT("allocate memory (maxW*maxH*2 bytes) failed\n");
    close(gstInfo.fd);
    return HI_NULL;
  }

  /*head bmp buffer */
  if (HI_FAILURE == HI_MPI_SYS_MmzAlloc(&Phyaddr, ((void**)&Viraddr), NULL, NULL, SAMPLE_IMAGE_WIDTH*SAMPLE_IMAGE_HEIGHT*2))
  {
    SAMPLE_PRT("allocate memory failed\n");
    HI_MPI_SYS_MmzFree(stCanvasBuf.stCanvas.u32PhyAddr, pBuf);
    close(gstInfo.fd);
    return HI_NULL;
  }

  s32Ret = HI_TDE2_Open();
  if (s32Ret < 0) {
    SAMPLE_PRT("HI_TDE2_Open failed :%d!\n", s32Ret);
    goto EXIT_0;
  }

  int cnt = 0;
  int max_cnt = 0;
  int test = 0; // 区分大中小字体

  int err_chnum = 0;
  int err_ch[MAX_ALARM_NUM];
  
  int screenIdx = 0;
  int screenNum = 0;
  int itemsOnScreen = 0;
  
  while (gstInfo.isrun) {
    ParamLib_GetAlarmInfo(&new_alarm);
    if (memcmp(&old_alarm, &new_alarm, sizeof(alarm_config_t)) != 0) {
      memcpy(&old_alarm, &new_alarm, sizeof(alarm_config_t));
      test = new_alarm.m_fontsize;
      max_cnt = new_alarm.m_interval * 1000 / 100;
      cnt = max_cnt;
    }

    NetLib_GetNetAlarmInfo(&new_netalarm);
    if (memcmp(&old_netalarm, &new_netalarm, sizeof(net_alarm_info_t)) != 0) {
      memcpy(&old_netalarm, &new_netalarm, sizeof(net_alarm_info_t));
      
      // 统计故障通道数
      err_chnum = 0;
      memset(err_ch, 0, sizeof(err_ch));
      
      int i;
      for (i = 0; i < new_netalarm.m_num; ++i) {
        if (new_netalarm.m_ch[i]) {
          err_ch[err_chnum++] = i; /* 保存故障通道的下标 */
        }
      }

      cnt = max_cnt;
    }

    if (++cnt < max_cnt) {
      usleep(100 * 1000);
      continue;
    }

    cnt = 0;
    
    /***************************************************************/

    if (test == 0) {
      itemsOnScreen = 11;
    }
    else if (test == 1) {
      itemsOnScreen = 15 * 2;
    }
    else {
      itemsOnScreen = 23 * 3;
    }

    /* 确定当前显示的第几屏 总共需要显示多少屏 */
    screenNum = err_chnum / itemsOnScreen;
    if (err_chnum % itemsOnScreen)
      ++screenNum;

    if (screenIdx >= screenNum)
      screenIdx = 0;
      

    /******************** fresh screen **************************/
    stCanvasBuf.stCanvas.u32Height = maxH;
    stCanvasBuf.stCanvas.u32Width = maxW;
    stCanvasBuf.stCanvas.u32Pitch = maxW*2;
    stCanvasBuf.stCanvas.enFmt = HIFB_FMT_ARGB1555;
    memset(pBuf, HIFB_WHITE_1555, stCanvasBuf.stCanvas.u32Pitch*stCanvasBuf.stCanvas.u32Height);

    /*************************** draw head ***********************/
    LoadBmp(HEAD_IMAGE_PATH, (HI_U8 *)Viraddr);
    stSrcRect.s32Xpos = 0;
    stSrcRect.s32Ypos = 0;
    stSrcRect.u32Height = SAMPLE_IMAGE_HEIGHT;
    stSrcRect.u32Width = SAMPLE_IMAGE_WIDTH;
    
    stSrc.enColorFmt = TDE2_COLOR_FMT_ARGB1555;
    stSrc.u32Width = SAMPLE_IMAGE_WIDTH;
    stSrc.u32Height = SAMPLE_IMAGE_HEIGHT;
    stSrc.u32Stride = 2*SAMPLE_IMAGE_WIDTH;
    stSrc.u32PhyAddr = Phyaddr;
    stSrc.bAlphaExt1555 = HI_TRUE;
    stSrc.bAlphaMax255 = HI_TRUE;
    stSrc.u8Alpha0 = 0XFF;
    stSrc.u8Alpha1 = 0XFF;
    
    if (test == 0) {
      stDstRect.u32Height = (SAMPLE_IMAGE_HEIGHT / 3) * 4;
      stDstRect.u32Width = (SAMPLE_IMAGE_WIDTH / 3) * 4;
    }
    else if (test == 1) {
      stDstRect.u32Height = SAMPLE_IMAGE_HEIGHT;
      stDstRect.u32Width = SAMPLE_IMAGE_WIDTH;
    }
    else {
      stDstRect.u32Height = (SAMPLE_IMAGE_HEIGHT / 3) * 2;
      stDstRect.u32Width = (SAMPLE_IMAGE_WIDTH / 3) * 2;
    }
    stDstRect.s32Xpos = (WIDTH - stDstRect.u32Width) / 2;
    stDstRect.s32Ypos = 0;

    stDst.enColorFmt = TDE2_COLOR_FMT_ARGB1555;
    stDst.u32Width = WIDTH;
    stDst.u32Height = HEIGHT;
    stDst.u32Stride = maxW*2;			
    stDst.u32PhyAddr = stCanvasBuf.stCanvas.u32PhyAddr;
  		
    /* 1. start job */
    s32Handle = HI_TDE2_BeginJob();
    if (HI_ERR_TDE_INVALID_HANDLE == s32Handle) {
      SAMPLE_PRT("start job failed!\n");
      goto EXIT_0;
    }

    s32Ret = HI_TDE2_QuickResize(s32Handle, &stSrc, &stSrcRect, &stDst, &stDstRect);
    if (s32Ret < 0) {
      SAMPLE_PRT("HI_TDE2_QuickCopy:%d failed,ret=0x%x!\n", __LINE__, s32Ret);
      goto EXIT_1;
    }

    /* 3. submit job */
    s32Ret = HI_TDE2_EndJob(s32Handle, HI_FALSE, HI_TRUE, 10);
    if (s32Ret < 0) {
      SAMPLE_PRT("Line:%d,HI_TDE2_EndJob failed,ret=0x%x!\n", __LINE__, s32Ret); 
      goto EXIT_1;
    }

    /***************************画第一行说明***************************/
    int headline_width, headline_height, headline_font_x, i, j;
    char headlineBuf[120 + 1];
    sprintf(headlineBuf, "通道总数 %d，故障通道数 %d，第%d/%d屏，显示间隔%d秒", new_alarm.m_chnum, err_chnum, screenIdx + 1, screenNum == 0 ? 1 : screenNum, new_alarm.m_interval);
    int len = strlen(headlineBuf);
    
    headline_width = WIDTH;
    if (test == 0) {
      headline_height = 2 + 64 + 2;
      headline_font_x = ((30*2 - len) / 2) * 32;
    }
    else if (test == 1) {
      headline_height = 2 + 48 + 2;
      headline_font_x = ((40*2 - len) / 2) * 24;
    }
    else {
      headline_height = 2 + 32 + 2;
      headline_font_x = ((60*2 - len) / 2) * 16;
    }

    HI_U32 item_Phyaddr;
    HI_VOID *item_Viraddr;
    HI_U32 font_Phyaddr;
    HI_VOID *font_Viraddr;
    
    if (HI_FAILURE == HI_MPI_SYS_MmzAlloc(&item_Phyaddr, ((void**)&item_Viraddr), NULL, NULL, headline_width*headline_height*2)) {
      SAMPLE_PRT("allocate memory failed\n");
      goto EXIT_1;
    }

    for (i = 0; i < headline_width*headline_height; i++) {
      HI_U16 *p = (HI_U16 *)item_Viraddr;
      *(p + i) = HIFB_LINE_1555;
    }

    TDE2_RECT_S stSrcItemRect, stDstItemRect;
        
    stSrcItemRect.s32Xpos = 0;
    stSrcItemRect.s32Ypos = 0;
    stSrcItemRect.u32Height = headline_height;
    stSrcItemRect.u32Width = headline_width;

    stSrc.enColorFmt = TDE2_COLOR_FMT_ARGB1555;
    stSrc.u32Width = headline_width;
    stSrc.u32Height = headline_height;
    stSrc.u32Stride = 2*headline_width;
    stSrc.u32PhyAddr = item_Phyaddr;
    stSrc.bAlphaExt1555 = HI_TRUE;
    stSrc.bAlphaMax255 = HI_TRUE;
    stSrc.u8Alpha0 = 0xFF;
    stSrc.u8Alpha1 = 0xFF;

    stDstItemRect.s32Xpos = 0;
    stDstItemRect.s32Ypos = stDstRect.u32Height + 4;
    stDstItemRect.u32Height = headline_height;
    stDstItemRect.u32Width = headline_width;

    stDst.enColorFmt = TDE2_COLOR_FMT_ARGB1555;
    stDst.u32Width = WIDTH;
    stDst.u32Height = HEIGHT;
    stDst.u32Stride = maxW*2;			
    stDst.u32PhyAddr = stCanvasBuf.stCanvas.u32PhyAddr;

    /* 1. start job */
    s32Handle = HI_TDE2_BeginJob();
    if (HI_ERR_TDE_INVALID_HANDLE == s32Handle) {
      SAMPLE_PRT("start job failed!\n");
      goto EXIT_0;
    }

    s32Ret = HI_TDE2_QuickCopy(s32Handle, &stSrc, &stSrcItemRect, &stDst, &stDstItemRect);
    if (s32Ret < 0) {
      SAMPLE_PRT("HI_TDE2_QuickCopy:%d failed,ret=0x%x!\n", __LINE__, s32Ret);
      goto EXIT_1;
    }

    /* 3. submit job */
    s32Ret = HI_TDE2_EndJob(s32Handle, HI_FALSE, HI_TRUE, 10);
    if (s32Ret < 0) {
      SAMPLE_PRT("Line:%d,HI_TDE2_EndJob failed,ret=0x%x!\n", __LINE__, s32Ret); 
      goto EXIT_1;
    }
    
    HI_MPI_SYS_MmzFree(item_Phyaddr, item_Viraddr);
    
    /* 叠加第一行汉字 */
    stSrcItemRect.s32Xpos = 0;
    stSrcItemRect.s32Ypos = 0;
    s32Ret = osd_create(headlineBuf, test, HIFB_GREEN_1555, &font_Phyaddr, &font_Viraddr, &stSrcItemRect.u32Width, &stSrcItemRect.u32Height);
    if (s32Ret < 0) {
      logErr("osd_create err");
      HI_MPI_SYS_MmzFree(item_Phyaddr, item_Viraddr);
      goto EXIT_1;
    }
    
    stSrc.enColorFmt = TDE2_COLOR_FMT_ARGB1555;
    stSrc.u32Width = stSrcItemRect.u32Width;
    stSrc.u32Height = stSrcItemRect.u32Height;
    stSrc.u32Stride = 2*stSrcItemRect.u32Width;
    stSrc.u32PhyAddr = font_Phyaddr;
    stSrc.bAlphaExt1555 = HI_TRUE;
    stSrc.bAlphaMax255 = HI_TRUE;
    stSrc.u8Alpha0 = 0xFF;
    stSrc.u8Alpha1 = 0xFF;

    stDstItemRect.s32Xpos = headline_font_x;
    stDstItemRect.s32Ypos = stDstRect.u32Height + 4;
    stDstItemRect.u32Height = stSrcItemRect.u32Height;
    stDstItemRect.u32Width = stSrcItemRect.u32Width;

    stDst.enColorFmt = TDE2_COLOR_FMT_ARGB1555;
    stDst.u32Width = WIDTH;
    stDst.u32Height = HEIGHT;
    stDst.u32Stride = maxW*2;			
    stDst.u32PhyAddr = stCanvasBuf.stCanvas.u32PhyAddr;

    /* 1. start job */
    s32Handle = HI_TDE2_BeginJob();
    if (HI_ERR_TDE_INVALID_HANDLE == s32Handle) {
      SAMPLE_PRT("start job failed!\n");
      goto EXIT_0;
    }

    s32Ret = HI_TDE2_QuickCopy(s32Handle, &stSrc, &stSrcItemRect, &stDst, &stDstItemRect);
    if (s32Ret < 0) {
      SAMPLE_PRT("HI_TDE2_QuickCopy:%d failed,ret=0x%x!\n", __LINE__, s32Ret);
      goto EXIT_1;
    }

    /* 3. submit job */
    s32Ret = HI_TDE2_EndJob(s32Handle, HI_FALSE, HI_TRUE, 10);
    if (s32Ret < 0) {
      SAMPLE_PRT("Line:%d,HI_TDE2_EndJob failed,ret=0x%x!\n", __LINE__, s32Ret); 
      goto EXIT_1;
    }

    HI_MPI_SYS_MmzFree(font_Phyaddr, font_Viraddr);

    /*************************** draw row*************************/
    int row_width, row_height, row_num, col_num, jmp_height, font_x;
    
    if (test == 0) {
      row_num = 11; //12;
      col_num = 1;
      row_width = WIDTH - 4 * (col_num - 1);  //剔除列间隔 4 pixs
      row_height = 2 + 64 + 2;
      jmp_height = 64 + 12;

      font_x = ((30 - 16) / 2) * 64;
    }
    else if (test == 1) {
      row_num = 15; //16;
      col_num = 2;
      row_width = WIDTH / col_num - 4 * (col_num - 1);  //剔除列间隔 4 pixs;
      row_height = 2 + 48 + 2;
      jmp_height = 48 + 12;

      //font_x = ((20 - 16) / 2) * 48;
      font_x = 24;
    }
    else {
      row_num = 23; //24;
      col_num = 3;
      row_width = WIDTH / col_num - 4 * (col_num - 1);  //剔除列间隔 4 pixs;
      row_height = 2 + 32 + 2;
      jmp_height = 32 + 10;

      //font_x = ((20 - 16) / 2) * 32;
      font_x = 16;
    }
    
    if (HI_FAILURE == HI_MPI_SYS_MmzAlloc(&item_Phyaddr, ((void**)&item_Viraddr), NULL, NULL, row_width*row_height*2)) {
      SAMPLE_PRT("allocate memory failed\n");
      goto EXIT_1;
    }

    for (i = 0; i < row_width*row_height; i++) {
      HI_U16 *p = (HI_U16 *)item_Viraddr;
      *(p + i) = HIFB_LINE_1555;
    }
    
    for (i = 0; i < col_num; i++) {
      for (j = 0; j < row_num; j++) {
        /*******************贴行背景*******************************************************/
        stSrcItemRect.s32Xpos = 0;
        stSrcItemRect.s32Ypos = 0;
        stSrcItemRect.u32Height = row_height;
        stSrcItemRect.u32Width = row_width;
        
        stSrc.enColorFmt = TDE2_COLOR_FMT_ARGB1555;
        stSrc.u32Width = row_width;
        stSrc.u32Height = row_height;
        stSrc.u32Stride = 2*row_width;
        stSrc.u32PhyAddr = item_Phyaddr;
        stSrc.bAlphaExt1555 = HI_TRUE;
        stSrc.bAlphaMax255 = HI_TRUE;
        stSrc.u8Alpha0 = 0xFF;
        stSrc.u8Alpha1 = 0xFF;

        stDstItemRect.s32Xpos = i * (row_width + 4);
        // 标题 + 第一行故障率说明
        stDstItemRect.s32Ypos = (stDstRect.u32Height + jmp_height) + j * jmp_height;  
        stDstItemRect.u32Height = row_height;
        stDstItemRect.u32Width = row_width;
        
        stDst.enColorFmt = TDE2_COLOR_FMT_ARGB1555;
        stDst.u32Width = WIDTH;
        stDst.u32Height = HEIGHT;
        stDst.u32Stride = maxW*2;			
        stDst.u32PhyAddr = stCanvasBuf.stCanvas.u32PhyAddr;
      		
        /* 1. start job */
        s32Handle = HI_TDE2_BeginJob();
        if (HI_ERR_TDE_INVALID_HANDLE == s32Handle) {
          SAMPLE_PRT("start job failed!\n");
          goto EXIT_0;
        }

        s32Ret = HI_TDE2_QuickCopy(s32Handle, &stSrc, &stSrcItemRect, &stDst, &stDstItemRect);
        if (s32Ret < 0) {
          SAMPLE_PRT("HI_TDE2_QuickCopy:%d failed,ret=0x%x!\n", __LINE__, s32Ret);
          goto EXIT_1;
        }

        /* 3. submit job */
        s32Ret = HI_TDE2_EndJob(s32Handle, HI_FALSE, HI_TRUE, 10);
        if (s32Ret < 0) {
          SAMPLE_PRT("Line:%d,HI_TDE2_EndJob failed,ret=0x%x!\n", __LINE__, s32Ret); 
          goto EXIT_1;
        }

        /*******************贴汉字************************************************************************************/
        int idx = screenIdx * itemsOnScreen + i*row_num + j;
        if (idx >= err_chnum)
          continue;
          
        char itembuf[32 + 1];
        sprintf(itembuf, "%-3d %s", idx+1, new_alarm.m_strch[err_ch[idx]]);
        
        stSrcItemRect.s32Xpos = 0;
        stSrcItemRect.s32Ypos = 0;
        s32Ret = osd_create(itembuf, test, HIFB_RED_1555, &font_Phyaddr, &font_Viraddr, &stSrcItemRect.u32Width, &stSrcItemRect.u32Height);
        if (s32Ret < 0) {
          logErr("osd_create err");
          HI_MPI_SYS_MmzFree(item_Phyaddr, item_Viraddr);
          goto EXIT_1;
        }
        stSrc.enColorFmt = TDE2_COLOR_FMT_ARGB1555;
        stSrc.u32Width = stSrcItemRect.u32Width;
        stSrc.u32Height = stSrcItemRect.u32Height;
        stSrc.u32Stride = 2*stSrcItemRect.u32Width;
        stSrc.u32PhyAddr = font_Phyaddr;
        stSrc.bAlphaExt1555 = HI_TRUE;
        stSrc.bAlphaMax255 = HI_TRUE;
        stSrc.u8Alpha0 = 0xFF;
        stSrc.u8Alpha1 = 0xFF;

        
        stDstItemRect.s32Xpos = i * (row_width) + font_x;
        // 标题 + 第一行故障率说明
        stDstItemRect.s32Ypos = (stDstRect.u32Height + jmp_height) + j * jmp_height + 4;
        stDstItemRect.u32Height = stSrcItemRect.u32Height;
        stDstItemRect.u32Width = stSrcItemRect.u32Width;
        
        stDst.enColorFmt = TDE2_COLOR_FMT_ARGB1555;
        stDst.u32Width = WIDTH;
        stDst.u32Height = HEIGHT;
        stDst.u32Stride = maxW*2;			
        stDst.u32PhyAddr = stCanvasBuf.stCanvas.u32PhyAddr;
        
        /* 1. start job */
        s32Handle = HI_TDE2_BeginJob();
        if (HI_ERR_TDE_INVALID_HANDLE == s32Handle) {
          SAMPLE_PRT("start job failed!\n");
          goto EXIT_0;
        }

        s32Ret = HI_TDE2_QuickCopy(s32Handle, &stSrc, &stSrcItemRect, &stDst, &stDstItemRect);
        if (s32Ret < 0) {
          SAMPLE_PRT("HI_TDE2_QuickCopy:%d failed,ret=0x%x!\n", __LINE__, s32Ret);
          goto EXIT_1;
        }

        /* 3. submit job */
        s32Ret = HI_TDE2_EndJob(s32Handle, HI_FALSE, HI_TRUE, 10);
        if (s32Ret < 0) {
          SAMPLE_PRT("Line:%d,HI_TDE2_EndJob failed,ret=0x%x!\n", __LINE__, s32Ret); 
          goto EXIT_1;
        }
        
        HI_MPI_SYS_MmzFree(font_Phyaddr, font_Viraddr);
      }
    }
    
    HI_MPI_SYS_MmzFree(item_Phyaddr, item_Viraddr);
    /*************************** draw end*********************/

    /* refresh surface */
    stCanvasBuf.UpdateRect.x = 0;
    stCanvasBuf.UpdateRect.y = 0;
    stCanvasBuf.UpdateRect.w = maxW;
    stCanvasBuf.UpdateRect.h = maxH;
    s32Ret = ioctl(gstInfo.fd, FBIO_REFRESH, &stCanvasBuf);
    if (s32Ret < 0) {
      SAMPLE_PRT("REFRESH failed!\n");
      goto EXIT_1;
    }

    /*if (test == 0) {
      test = 1;
    }
    else if (test == 1) {
      test = 2;
    }
    else {
      test = 0;
    }*/
    
    ++screenIdx;
  }
  
EXIT_1:
  HI_TDE2_CancelJob(s32Handle);
EXIT_0:
  HI_MPI_SYS_MmzFree(Phyaddr, Viraddr);
  HI_MPI_SYS_MmzFree(stCanvasBuf.stCanvas.u32PhyAddr, pBuf);
  close(gstInfo.fd);
  return HI_NULL;   
}

#define FONT_16CH_FILE	    "/opt/lib/font/HZK16"
#define FONT_16ASC_FILE	    "/opt/lib/font/ASC16"
#define FONT_ASCI_SIZE      (16 * 8 / 8)
#define BIT_I(ch, i)        ((ch)&(1<<i))

// 传入OSD多字节，字体大小，颜色，返回bmp ARGB1555的buffer，矩形的宽高
static int osd_create(unsigned char *str, int fontsize, unsigned short color, HI_U32 *Phyaddr, HI_VOID **Viraddr, HI_U32 *pwidth, HI_U32 *pheight)
{   
    if (str == NULL || Phyaddr == NULL || Viraddr == NULL || pwidth == NULL || pheight  == NULL) {
        logErr("param is err");
        return -1;
    }
    
    unsigned short **buffer = (unsigned short **)Viraddr;
   
    /***************************初始化字库***********************************/
    int len = strlen(str);
    unsigned char *fontbuf = (unsigned char *)malloc(len * FONT_ASCI_SIZE);
    if (fontbuf == NULL) {
        logErr("malloc err");
        return -1;
    }

    FILE *fp_asc, *fp_cn;
    
    if ((fp_asc = fopen(FONT_16ASC_FILE, "rb")) == NULL) {
        logErr("open FONT_16ASC_FILE err");
        free(fontbuf);
        return -1;
    }

    if ((fp_cn = fopen(FONT_16CH_FILE, "rb")) == NULL) {
        logErr("open FONT_16CH_FILE err");
        free(fontbuf);
        fclose(fp_asc);
        return -1;
    }
    
    int i, j, k;
    unsigned char ch1, ch2;
    for (i = 0; i < len; ++i) {
        ch1 = *(str + i);
        ch2 = i + 1 < len ? *(str + i + 1) : 0;
            
        if (ch1 < 0xa1) {
            int pos = ch1 * FONT_ASCI_SIZE;
            if (fseek(fp_asc, pos, SEEK_SET) != 0) {
                logErr("fseek err pos=%d", pos);
                free(fontbuf);
                fclose(fp_asc);
                fclose(fp_cn);
                return -1;
            }
            
            size_t rdblk = fread(fontbuf + i * FONT_ASCI_SIZE, FONT_ASCI_SIZE, 1, fp_asc);
            if (rdblk != 1) {
                logErr("fread err rdblock=%d\n", rdblk);
                free(fontbuf);
                fclose(fp_asc);
                fclose(fp_cn);
                return -1;
            }
        }
        else if (ch2 >= 0xa1) {
            int pos = (94 * (ch1 - 0xa1) + (ch2 - 0xa1)) * FONT_ASCI_SIZE * 2;
            if (fseek(fp_cn, pos, SEEK_SET) != 0) {
                logErr("fseek err pos=%d", pos);
                free(fontbuf);
                fclose(fp_asc);
                fclose(fp_cn);
                return -1;
            }

            unsigned char tmp[FONT_ASCI_SIZE * 2];
            
            size_t rdblk = fread(tmp, FONT_ASCI_SIZE * 2, 1, fp_cn);
            if (rdblk != 1) {
                logErr("fread err rdblock=%d\n", rdblk);
                free(fontbuf);
                fclose(fp_asc);
                fclose(fp_cn);
                return -1;
            }
            
            for (j = 0; j < FONT_ASCI_SIZE; j++) {
              *(fontbuf + i * FONT_ASCI_SIZE + j) = tmp[j * 2];
              *(fontbuf + i * FONT_ASCI_SIZE + FONT_ASCI_SIZE + j) = tmp[j * 2 + 1];
            }
            
            ++i;
        }
    }
    
    fclose(fp_asc);
    fclose(fp_cn);
    
    /***************************缩放字库到bmp***********************************/
    /* 注意汉字是双字节 */
    if (fontsize == 0) {
        *pheight = 64;
        *pwidth = len * 32;
    }
    else if (fontsize == 1) {
        *pheight = 48;
        *pwidth = len * 24;
    }
    else {
        *pheight = 32;
        *pwidth = len * 16;
    }

    if (HI_FAILURE == HI_MPI_SYS_MmzAlloc(Phyaddr, Viraddr, NULL, NULL, (*pwidth) * (*pheight) * 2)) {
      SAMPLE_PRT("allocate memory failed\n");
      free(fontbuf);
      return -1;
    }

    /* 遍历单个字模 */
    for (i = 0; i < len; i++) {
      int offset_x;
      if (fontsize == 0) {
        offset_x = i * (8 * 4);
      }
      else if (fontsize == 1) {
        offset_x = i * (8 * 3);
      }
      else {
        offset_x = i * (8 * 2);
      }
      
      for (j = 0; j < FONT_ASCI_SIZE; j++) {
        unsigned char ch = *(fontbuf + i * FONT_ASCI_SIZE + j);
        int zoom, zoom_x, zoom_y;
        
        if (fontsize == 0) { //col 8->32, row 16->64
          zoom = 4;
        }
        else if (fontsize == 1) { //col 8->24, row 16->48
          zoom = 3;
        }
        else { //col 8->16, row 16->32
          zoom = 2;
        }

        for (k = 0; k < 8; k++) {
          for (zoom_y = 0; zoom_y < zoom; zoom_y++)
            for (zoom_x  = 0; zoom_x < zoom; zoom_x++)
              *(*buffer + offset_x + (j*zoom + zoom_y)*(*pwidth) + (k*zoom + zoom_x)) = BIT_I(ch, (7 - k)) ? color : HIFB_LINE_1555;
        }
      }
    }

    free(fontbuf);
        
    return 0;
}

int OSD_Init(void)
{
  HI_S32 s32Ret = HI_SUCCESS;
  HI_U32 u32PicWidth = WIDTH;
  HI_U32 u32PicHeight = HEIGHT;
  SIZE_S stSize;

  VO_LAYER VoLayer = 0;
  VO_PUB_ATTR_S stPubAttr;
  VO_VIDEO_LAYER_ATTR_S stLayerAttr;
  HI_U32 u32VoFrmRate;

  VB_CONF_S stVbConf;
  HI_U32 u32BlkSize;

  /******************************************
  step  1: init variable 
  ******************************************/
  memset(&stVbConf, 0, sizeof(VB_CONF_S));

  u32BlkSize = CEILING_2_POWER(u32PicWidth, SAMPLE_SYS_ALIGN_WIDTH) * CEILING_2_POWER(u32PicHeight, SAMPLE_SYS_ALIGN_WIDTH) * 2;

  stVbConf.u32MaxPoolCnt = 128;
  stVbConf.astCommPool[0].u32BlkSize = u32BlkSize;
  stVbConf.astCommPool[0].u32BlkCnt =  6;

  /******************************************
  step 2: mpp system init. 
  ******************************************/
  s32Ret = SAMPLE_COMM_SYS_Init(&stVbConf);
  if (HI_SUCCESS != s32Ret)
  {
    SAMPLE_PRT("system init failed with %d!\n", s32Ret);
    goto SAMPLE_HIFB_DoubleBufMode_0;
  }

  /******************************************
  step 3:  start vo hd0. 
  *****************************************/
  stPubAttr.enIntfSync = VO_OUTPUT_1080P60;
  stPubAttr.enIntfType = VO_INTF_VGA | VO_INTF_HDMI;
  stPubAttr.u32BgColor = 0x0000FF;

  stLayerAttr.bClusterMode = HI_FALSE;
  stLayerAttr.bDoubleFrame = HI_FALSE;
  stLayerAttr.enPixFormat = PIXEL_FORMAT_YUV_SEMIPLANAR_420;

  s32Ret = SAMPLE_COMM_VO_GetWH(stPubAttr.enIntfSync, &stSize.u32Width, &stSize.u32Height, &u32VoFrmRate);
  if (HI_SUCCESS != s32Ret)
  {
    SAMPLE_PRT("get vo wh failed with %d!\n", s32Ret);
    goto SAMPLE_HIFB_DoubleBufMode_0;
  }
  memcpy(&stLayerAttr.stImageSize, &stSize, sizeof(stSize)); 

  stLayerAttr.u32DispFrmRt = 30;
  stLayerAttr.stDispRect.s32X = 0;
  stLayerAttr.stDispRect.s32Y = 0;
  stLayerAttr.stDispRect.u32Width = stSize.u32Width;
  stLayerAttr.stDispRect.u32Height = stSize.u32Height;

  s32Ret = SAMPLE_COMM_VO_StartDev(VoDev, &stPubAttr);
  if (HI_SUCCESS != s32Ret)
  {
    SAMPLE_PRT("start vo dev failed with %d!\n", s32Ret);
    goto SAMPLE_HIFB_DoubleBufMode_0;
  }

  s32Ret = SAMPLE_COMM_VO_StartLayer(VoLayer, &stLayerAttr);
  if (HI_SUCCESS != s32Ret)
  {
    SAMPLE_PRT("start vo layer failed with %d!\n", s32Ret);
    goto SAMPLE_HIFB_DoubleBufMode_1;
  }

  if (stPubAttr.enIntfType & VO_INTF_HDMI)
  {
    s32Ret = SAMPLE_COMM_VO_HdmiStart(stPubAttr.enIntfSync);
    if (HI_SUCCESS != s32Ret)
    {
      SAMPLE_PRT("start HDMI failed with %d!\n", s32Ret);
      goto SAMPLE_HIFB_DoubleBufMode_2;
    }
  }

  InitHifb();
  
  return s32Ret;

SAMPLE_HIFB_DoubleBufMode_2:
  HI_MPI_VO_DisableVideoLayer(VoLayer);
SAMPLE_HIFB_DoubleBufMode_1:
  HI_MPI_VO_Disable(VoDev);
SAMPLE_HIFB_DoubleBufMode_0:
  SAMPLE_COMM_SYS_Exit();
  
  return s32Ret;
}

void OSD_DeInit(void)
{
  VO_LAYER VoLayer = 0;
  
  HI_MPI_VO_DisableVideoLayer(VoLayer);
  HI_MPI_VO_Disable(VoDev);
  SAMPLE_COMM_SYS_Exit();
}

int OSD_StartServer(void)
{
  if (gstInfo.pid != -1)
    return 0;

  gstInfo.isrun = HI_TRUE;
  if (pthread_create(&gstInfo.pid, NULL, osd_thread, NULL) < 0) {
    printf("create OsdThread failed!\n");
    gstInfo.pid = -1;
    return -1;
  }

  return 0;
}

int OSD_StopServer(void)
{
  if (gstInfo.pid == -1)
    return 0;

  gstInfo.isrun = HI_FALSE;
  pthread_join(gstInfo.pid, 0);
  gstInfo.pid = -1;

  return 0;
}


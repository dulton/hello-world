//==============================================================================
//
//  File        : JobDispatch.c
//  Description : ce job dispatch function
//  Author      : Robin Feng
//  Revision    : 1.0
//
//==============================================================================

#include "includes_fw.h"
#include "customer_config.h"
#include "bootloader.h"
#include "mmpf_storage_api.h"
#include "mmpf_sf.h"
#include "mmpf_saradc.h"
#include "PMUCtrl.h"
#include "AHC_Common.h"
#include "AHC_Config_SDK.h"
#include "lcd_common.h"
//#include "mmp_reg_jpeg.h"
//#include "mmp_reg_ibc.h"

//==============================================================================
//
//                              COMPILING OPTIONS
//
//==============================================================================
#define BOOT_IMG_FORMAT (1) //(BMP:JPG)/(0:1)

#ifndef CFG_BOOT_CUS_POWER_ON_IMG_DELAY_TV
#define CFG_BOOT_CUS_POWER_ON_IMG_DELAY_TV		(300)
#endif

//==============================================================================
//
//                              CONSTANT
//
//==============================================================================
#define MMC_BOOT_CHECK_ID				(0xaa550840)

typedef struct {
		int		img_w;
		int		img_h;
		int		img_clrs;
		int		img_Size;        
} IMG;

//==============================================================================
//
//								Local varible
//
//==============================================================================
static MMP_BOOL m_ubShowPowerOnImage = MMP_TRUE;

static MMP_UBYTE m_ubLCDStatus		= 0xFF;
static MMP_UBYTE m_ubSNRStatus		= 0xFF; 
static MMP_UBYTE m_ubLCDSetting		= 0xFF;
static MMP_UBYTE m_ubTVSetting		= 0xFF;
static MMP_UBYTE m_ubHDMISetting	= 0xFF;

//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================
MMP_BOOL IsHdmiConnect(void);
void OemSetLcdDirection(void);
MMP_BOOL IsTVConnect(void);
#if POWER_ON_BY_GSENSOR_EN
MMP_BOOL Gsensor_GetIntStatusForBoot(void);
#endif
extern MMP_ERR MMPS_DSC_PlaybackJpegEx(MMP_DSC_JPEG_INFO           *pJpegInfo,
                                       MMP_BOOL                    bMirror,
                                       MMP_DISPLAY_COLORMODE       DecodeColorFormat);
extern MMP_ERR MMPF_IBC_Init(void);
extern int SF_Module_Init(void);
//////////////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////////////
#if 0
void _____LCD_Rotation_________(){ruturn;} //dummy
#endif

//#define DEBUG_MENU_STATUS_IN_SF

static void GetMenuStatusInSF(void)
{
	#define INVALID_DATA	(0xFF)
	
	MMP_ULONG SF_DestAddr;
	MMP_ERR	  SF_Err = MMP_ERR_NONE;
	MMPF_SIF_INFO *info;
	
	/* Initial SF Device */
	SF_Module_Init();
	SF_Err = MMPF_SF_SetIdBufAddr(AIT_SF_WORK_BUFFER_ADDR);
    SF_Err = MMPF_SF_SetTmpAddr(AIT_SF_WORK_BUFFER_ADDR);
    SF_Err = MMPF_SF_InitialInterface(MMPF_SIF_PAD_MAX);
    SF_Err = MMPF_SF_Reset();

	info = MMPF_SF_GetSFInfo();

    RTNA_DBG_Str(0, "\r\n### Serial Flash Info");
    RTNA_DBG_Str(0, "\r\n    Device Id = "); RTNA_DBG_Long(0, info->ulDevId);
    RTNA_DBG_Str(0, "\r\n    Total Size = "); RTNA_DBG_Long(0, info->ulSFTotalSize);
    RTNA_DBG_Str(0, "\r\n    Sector Size = "); RTNA_DBG_Long(0, info->ulSFSectorSize);
    RTNA_DBG_Str(0, "\r\n    Block Size = "); RTNA_DBG_Long(0, info->ulSFBlockSize);
    RTNA_DBG_Str(0, "\r\n");
    
	SF_DestAddr = info->ulSFTotalSize - MENU_STATUS_REGION_SIZE;
    MMPF_MMU_FlushDCacheMVA((MMP_ULONG)AIT_SYS_MEDIA_START_ADDR, MENU_STATUS_REGION_SIZE);
	SF_Err = MMPF_SF_ReadData(SF_DestAddr, AIT_SYS_MEDIA_START_ADDR, MENU_STATUS_REGION_SIZE);

	if (SF_Err != MMP_ERR_NONE)
	{
		m_ubTVSetting  	= TV_PAL;  	//TV_SYSTEM_NTSC
		m_ubHDMISetting = 0;		//HDMI_OUTPUT_1080I
		m_ubSNRStatus  	= 0;		//AHC_SNR_NORMAL
		m_ubLCDStatus  	= LCD_NORMAL;
		m_ubLCDSetting 	= LCD_ROTATE_OFF;
	}
	else
	{
        MMP_UBYTE checkSum = 0;
        MMP_ULONG i;

        for (i = 0; i < SETTING_TOTAL_ITEMS; i++) {
            checkSum += *(volatile MMP_UBYTE *)(AIT_SYS_MEDIA_START_ADDR + MENU_STATUS_REGION_SIZE - 1 - i);    
        }

    	RTNA_DBG_PrintByte(0, checkSum);
        RTNA_DBG_Str(0, "\r\n");
    	
        if (checkSum != SETTING_CHECK_VALUE) {
            RTNA_DBG_Str(0, FG_RED("--E-- GetMenuStatusInSF: checksume error !!!\r\n"));

            #ifdef DEBUG_MENU_STATUS_IN_SF
            {
                MMP_ULONG j;

            	SF_DestAddr = info->ulSFTotalSize - info->ulSFSectorSize;

                for (j = 0; j < info->ulSFSectorSize; j += MENU_STATUS_REGION_SIZE) {
                    RTNA_DBG_Str(0, FG_BLUE("SF Address: "));
                    RTNA_DBG_Long(0, SF_DestAddr + j);
                    RTNA_DBG_Str(0, "\r\n");

                    MMPF_MMU_FlushDCacheMVA((MMP_ULONG)AIT_SYS_MEDIA_START_ADDR, MENU_STATUS_REGION_SIZE);
                	MMPF_SF_ReadData(SF_DestAddr + j, (MMP_ULONG) AIT_SYS_MEDIA_START_ADDR, MENU_STATUS_REGION_SIZE);

                    for (i = 0; i < MENU_STATUS_REGION_SIZE; i++) {
                        if (i && ((i % 16) == 0))
                            RTNA_DBG_Str(0, "\r\n");

                        RTNA_DBG_Byte(0, *(volatile MMP_UBYTE *)(AIT_SYS_MEDIA_START_ADDR + i));
                    }

                    RTNA_DBG_Str(0, "\r\n\r\n");
                }
            }
            #endif
            
            m_ubTVSetting   = TV_PAL;	//TV_SYSTEM_NTSC
            m_ubHDMISetting = 0;		//HDMI_OUTPUT_1080I
            m_ubSNRStatus   = 0;		//AHC_SNR_NORMAL
            m_ubLCDStatus   = LCD_NORMAL;
            m_ubLCDSetting  = LCD_ROTATE_OFF;
        }
        else {
    		m_ubTVSetting  	= *(volatile MMP_UBYTE *)(AIT_SYS_MEDIA_START_ADDR + TV_SYSTEM_SETTING_OFFSET);
    		m_ubHDMISetting = *(volatile MMP_UBYTE *)(AIT_SYS_MEDIA_START_ADDR + HDMI_SYSTEM_SETTING_OFFSET);
    		m_ubSNRStatus  	= *(volatile MMP_UBYTE *)(AIT_SYS_MEDIA_START_ADDR + SNR_STATUS_OFFSET);
    		m_ubLCDStatus  	= *(volatile MMP_UBYTE *)(AIT_SYS_MEDIA_START_ADDR + LCD_STATUS_OFFSET);
    		m_ubLCDSetting 	= *(volatile MMP_UBYTE *)(AIT_SYS_MEDIA_START_ADDR + LCD_MENU_SETTING_OFFSET);
        }

		m_ubTVSetting  	= (m_ubTVSetting    == INVALID_DATA) ? (TV_PAL) :   (m_ubTVSetting);
		m_ubHDMISetting = (m_ubHDMISetting  == INVALID_DATA) ? (0)      :   (m_ubHDMISetting);
		m_ubSNRStatus  	= (m_ubSNRStatus    == INVALID_DATA) ? (0)      :   (m_ubSNRStatus);
		m_ubLCDStatus  	= (m_ubLCDStatus    == INVALID_DATA) ? (0)      :   (m_ubLCDStatus);
		m_ubLCDSetting 	= (m_ubLCDSetting   == INVALID_DATA) ? (1)      :   (m_ubLCDSetting);
	}

	RTNA_DBG_PrintByte(0, m_ubTVSetting); 
	RTNA_DBG_PrintByte(0, m_ubHDMISetting); 
	RTNA_DBG_PrintByte(0, m_ubSNRStatus); 
	RTNA_DBG_PrintByte(0, m_ubLCDStatus); 
	RTNA_DBG_PrintByte(0, m_ubLCDSetting); 
}

MMP_BOOL LCD_GetFlipSelection(MMP_UBYTE ubCtrl, MMP_UBYTE* pSelection, MMP_UBYTE* pbLinkMenu)
{
	if(ubCtrl == CTRL_BY_MENU)
	{
		*pSelection = MENU_FLIP_SELECT;
		*pbLinkMenu = MMP_TRUE;
	}	
	else if(ubCtrl == CTRL_BY_KEY)
	{
		*pSelection = KEY_FLIP_SELECT;
		*pbLinkMenu = KEY_FLIP_LINK_MENU;
	}
	else if(ubCtrl == CTRL_BY_HALL_SNR)
	{
		*pSelection = HALL_SNR_FLIP_SELECT;
		*pbLinkMenu = HALL_SNR_FLIP_LINK_MENU;
	}
	else if(ubCtrl == CTRL_BY_G_SNR)
	{
		*pSelection = G_SNR_FLIP_SELECT;
		*pbLinkMenu = G_SNR_FLIP_LINK_MENU;
	}
							
	return MMP_TRUE;
}

MMP_BOOL LCD_CheckOSDFlipEn(MMP_UBYTE ubCheck)
{
	MMP_UBYTE    ubOSDFlipEn = 0;
	MMP_UBYTE	 ubSelect1   = 0, ubSelect2   = 0, ubSelect3   = 0, ubSelect4   = 0;
	MMP_UBYTE 	 ubLinkMenu1 = 0, ubLinkMenu2 = 0, ubLinkMenu3 = 0, ubLinkMenu4 = 0;
				
	if(ubCheck & CTRL_BY_MENU)
		LCD_GetFlipSelection(CTRL_BY_MENU, &ubSelect1, &ubLinkMenu1);
				
	if(ubCheck & CTRL_BY_KEY)
		LCD_GetFlipSelection(CTRL_BY_KEY, &ubSelect2, &ubLinkMenu2);
					
	if(ubCheck & CTRL_BY_HALL_SNR)
		LCD_GetFlipSelection(CTRL_BY_HALL_SNR, &ubSelect3, &ubLinkMenu3);				
	
	if(ubCheck & CTRL_BY_G_SNR)
		LCD_GetFlipSelection(CTRL_BY_G_SNR, &ubSelect4, &ubLinkMenu4);
	
	ubOSDFlipEn = ((ubSelect1 & FLIP_OSD) | (ubSelect2 & FLIP_OSD) | (ubSelect3 & FLIP_OSD) | (ubSelect4 & FLIP_OSD)) ;
	
	return (ubOSDFlipEn)?(MMP_TRUE):(MMP_FALSE);
}

MMP_BOOL LCD_CheckSNRFlipEn(MMP_UBYTE ubCheck)
{
	MMP_UBYTE    ubSNRFlipEn = 0;
	MMP_UBYTE	 ubSelect1   = 0, ubSelect2   = 0, ubSelect3   = 0, ubSelect4   = 0;
	MMP_UBYTE 	 ubLinkMenu1 = 0, ubLinkMenu2 = 0, ubLinkMenu3 = 0, ubLinkMenu4 = 0;
				
	if(ubCheck & CTRL_BY_MENU)
		LCD_GetFlipSelection(CTRL_BY_MENU, &ubSelect1, &ubLinkMenu1);
				
	if(ubCheck & CTRL_BY_KEY)
		LCD_GetFlipSelection(CTRL_BY_KEY, &ubSelect2, &ubLinkMenu2);
					
	if(ubCheck & CTRL_BY_HALL_SNR)
		LCD_GetFlipSelection(CTRL_BY_HALL_SNR, &ubSelect3, &ubLinkMenu3);				
	
	if(ubCheck & CTRL_BY_G_SNR)
		LCD_GetFlipSelection(CTRL_BY_G_SNR, &ubSelect4, &ubLinkMenu4);
	
	ubSNRFlipEn = ((ubSelect1 & FLIP_SNR) | (ubSelect2 & FLIP_SNR) | (ubSelect3 & FLIP_SNR) | (ubSelect4 & FLIP_SNR)) ;
	
	return (ubSNRFlipEn)?(MMP_TRUE):(MMP_FALSE);
}

void LCD_CheckStatus(MMP_UBYTE *status)
{
	MMP_UBYTE ubRotateMethod = FLIP_CTRL_METHOD;

	if(ubRotateMethod & CTRL_BY_HALL_SNR)
	{
		MMP_UBYTE temp;

		if(DEVICE_GPIO_LCD_INV == MMP_GPIO_MAX)
		{		
			*status = LCD_NORMAL;
			return;
		}
			
		MMPF_PIO_GetData(DEVICE_GPIO_LCD_INV,&temp);
			
		if(LCD_CheckOSDFlipEn(CTRL_BY_MENU | CTRL_BY_HALL_SNR)==MMP_TRUE)
		{	
			if(m_ubLCDSetting==LCD_ROTATE_ON)
				*status = (temp)?(LCD_REVERSE):(LCD_NORMAL);
			else
				*status = (temp)?(LCD_NORMAL):(LCD_REVERSE);
		}
		else
		{
			*status = (temp)?(LCD_NORMAL):(LCD_REVERSE);
		}	
	}
	else if(ubRotateMethod & CTRL_BY_G_SNR)
	{
		//TBD
	}
	else if(ubRotateMethod & CTRL_BY_KEY)
	{
		if(LCD_CheckOSDFlipEn(CTRL_BY_MENU | CTRL_BY_KEY)==MMP_TRUE)
		{
			if(m_ubLCDSetting==LCD_ROTATE_ON)
				*status = LCD_REVERSE;
			else
				*status	= LCD_NORMAL;			
		}
		else
		{
			*status	= LCD_NORMAL;		
		}
	}
	else//Menu Only
	{
		if(LCD_CheckOSDFlipEn(CTRL_BY_MENU)==MMP_TRUE)
		{
			if(m_ubLCDSetting==LCD_ROTATE_ON)
				*status = LCD_REVERSE;
			else
				*status	= LCD_NORMAL;	
		}
		else
		{
			*status	= LCD_NORMAL;
		}
	}
} 


void FlipImage(IMG *pimg, unsigned short *img, unsigned char **pptr)
{
#ifdef FLIP_POWERON_IMG

	int i, w, h;
	unsigned short* buf;

	w = pimg->img_w;
	h = pimg->img_h;
	
	buf = (unsigned short*) AIT_SYS_IMAGE_TEMP_ADDR;
	
	for (i=0; i< w*h; i++)
	{
		buf[w*h-1-i] = img[i];
	}

	*pptr = (unsigned char*)buf; 	
#else
	*pptr = (unsigned char*)img; 	
#endif
}

void MagnifyImage(IMG *pimg, unsigned short *img, unsigned char **pptr, int ubMag) //For 2X currnetly
{
	int i, w, h;
	int row;
	unsigned short *buf, usData;
	unsigned char *ucImg;

    if (ubMag > 2) ubMag = 2;
    
	w = pimg->img_w;
	h = pimg->img_h;
	
	buf = (unsigned short*) AIT_SYS_IMAGE_TEMP_ADDR1;
	ucImg = (unsigned char *) img;

	for (i=0; i< w*h; i++)
	{
    	if (24 == pimg->img_clrs) {
			// RGB888 to RGB565 bmp RB swap
			usData = ((unsigned short)(ucImg[i*3] & 0xF8)) >> 3;
			usData |= ((unsigned short)(ucImg[i*3 + 1] & 0xFC)) << 3;
			usData |= ((unsigned short)(ucImg[i*3 + 2] & 0xF8)) << 8;
		} else {
			usData = img[i];
		}

		row = i/w;
		buf[ubMag*w*row + ubMag*i]			= usData;
		buf[ubMag*w*row + ubMag*i+1]		= usData;
		buf[ubMag*w*(row+1) + ubMag*i]		= usData;
		buf[ubMag*w*(row+1) + ubMag*i+1] 	= usData;
	}
	
	*pptr = (unsigned char *)buf; 

	pimg->img_clrs = 16;
	pimg->img_w *= ubMag;
	pimg->img_h *= ubMag;
}

#if 0
void _____Drawing_Function_________(){ruturn;} //dummy
#endif

/*
 * note: w must be 4 alignment
 */
char* Tran_565ToRGBA(char *psrc, int w, int h, char *pdst)
{
	int				loop;
	unsigned int	off;
	unsigned short	*ptr;
	
	for (loop = (w * h - 1); loop >= 0 ; --loop)//Convert RGB565 format to ARGB8888
	{	
		ptr = (unsigned short*)psrc + loop; 
		off = loop << 2;
		*(pdst + off + 0) = (MMP_UBYTE)((*ptr & 0x001F) << 3); //B                
		*(pdst + off + 1) = (MMP_UBYTE)((*ptr & 0x07E0) >> 3); //G
		*(pdst + off + 2) = (MMP_UBYTE)((*ptr & 0xF800) >> 8); //R
		*(pdst + off + 3) = 0x80; //Alpha
	}
	return pdst;
}

typedef	unsigned short	WORD;
typedef	unsigned int	DWORD;
typedef	unsigned long	LONG;

#define	GET_BYTE(p)		(*p); p += 1;
#define	GET_WORD(p)		(*p) + (*(p + 1) << 8); p += 2;
#define	GET_DWORD(p)	(*p) + (*(p + 1) << 8) + (*(p + 2) << 16) + (*(p + 3) << 24); p += 4;
#define	GET_LONG(p)		(*p) + (*(p + 1) << 8) + (*(p + 2) << 16) + (*(p + 3) << 24); p += 4;

#define	GET_WORD_BE(p)	(*p << 8) + (*(p + 1)); p += 2;

#if (BOOT_IMG_FORMAT == 0)
#define	BM				('B' + ('M')
typedef struct tagBITMAPFILEHEADER {
  WORD  bfType;
  DWORD bfSize;
  WORD  bfReserved1;
  WORD  bfReserved2;
  DWORD bfOffBits;
} BITMAPFILEHEADER, *PBITMAPFILEHEADER;

#define	BITMAPFILEHEADER_SIZE	14

typedef struct tagBITMAPINFOHEADER {
  DWORD biSize;
  LONG  biWidth;
  LONG  biHeight;
  WORD  biPlanes;
  WORD  biBitCount;
  DWORD biCompression;
  DWORD biSizeImage;
  LONG  biXPelsPerMeter;
  LONG  biYPelsPerMeter;
  DWORD biClrUsed;
  DWORD biClrImportant;
} BITMAPINFOHEADER, *PBITMAPINFOHEADER;

#define	BITMAPINFOHEADER_SIZE	40

typedef struct tagRGBQUAD {
  unsigned char rgbBlue;
  unsigned char rgbGreen;
  unsigned char rgbRed;
  unsigned char rgbReserved;
} RGBQUAD;

typedef struct tagBITMAPINFO {
  BITMAPINFOHEADER bmiHeader;
  RGBQUAD          bmiColors[1];
} BITMAPINFO, *PBITMAPINFO;

#define	SET_BITMAPFILEHEADER(sp, b)		\
		b.bfType	  		= GET_WORD(sp)	\
		b.bfSize	  		= GET_DWORD(sp)	\
		b.bfReserved1 		= GET_WORD(sp)	\
		b.bfReserved2 		= GET_WORD(sp)	\
		b.bfOffBits	  		= GET_DWORD(sp)	\
		
#define	SET_BITMAPINFOHEADER(sp, b)		\
		b.biSize	  		= GET_DWORD(sp)	\
		b.biWidth	  		= GET_LONG(sp)	\
		b.biHeight	  		= GET_LONG(sp)	\
		b.biPlanes	  		= GET_WORD(sp)	\
		b.biBitCount  		= GET_WORD(sp)	\
		b.biCompression	  	= GET_DWORD(sp)	\
		b.biSizeImage	  	= GET_DWORD(sp)	\
		b.biXPelsPerMeter 	= GET_LONG(sp)	\
		b.biYPelsPerMeter 	= GET_LONG(sp)	\
		b.biClrUsed		  	= GET_DWORD(sp)	\
		b.biClrImportant  	= GET_DWORD(sp)	\
		
static BITMAPFILEHEADER	bf;
static BITMAPINFOHEADER	bi;
#endif //#if (BOOT_IMG_FORMAT == 0)

#if (BOOT_IMG_FORMAT == 1)
#define	JPG_SOI			(0xFFD8)
#define	JPG_JFIF		(0xFFE0)
#define	JPG_EXIF		(0xFFE1)
#define	JPG_SOF			(0xFFC0)
#define	JPG_EOI         (0xFFD9)

typedef struct tagJPG_APP0_HEADER {
  WORD  uwSOI;          //[00:01] 0xFFD8
  WORD  uwAPP0;         //[02:03] 0xFFE0
  WORD  uwLen;          //[04:05] 
  DWORD dwJFIF0;        //[06:0A] 0x4a46494600 "JFIF0"
  WORD  uwVersion;      //[0#if (BOOT_IMG_FORMAT == 1)B:0C] 
  BYTE  ubXYUnit;       //[0D]    0:None 1:pix/in 2:pix/mm
  WORD  uwXDensity;     //[0E:0F] 
  WORD  uwYDensity;     //[10:11] 
} JPG_APP0_HEADER, *PJPG_APP0_HEADER;

typedef struct tagJPG_JFIF_HEADER {
  WORD  uwJFIF;          //[00:01] 0xFFE0
  WORD  uwLen;       //[02:03] 
} JPG_JFIF_HEADER, *PJPG_JFIF_HEADER;

typedef struct tagJPG_EXIF_HEADER {
  WORD  uwEXIF;          //[00:01] 0xFFE1
  WORD  uwLen;       //[02:03] 
} JPG_EXIF_HEADER, *PJPG_EXIF_HEADER;

typedef struct tagJPG_SOF_HEADER {
  WORD  uwSOF;          //[00:01] 0xFFC0
  WORD  uwLen;          //[02:03] 
  BYTE  ubColor;        //[04]    bits per pixel per color component) (8, 12, 16)
  WORD  uwHeight;       //[05:06]
  WORD  uwWidth;        //[07:08] 
  BYTE  ubNumOfColor;   //[09]    1:Gray 3:YCbCr/YIQ 4:CMYK
} JPG_SOF_HEADER, *PJPG_SOF_HEADER;

#define	SET_APP0_HEADER(sp, b)		\
		b.uwSOI	  		    = GET_WORD_BE(sp)	\
		b.uwAPP0	  		= GET_WORD_BE(sp)	\
		b.uwLen 		    = GET_WORD_BE(sp)	\
		b.dwJFIF0 		    = GET_DWORD(sp)	\
		b.uwVersion	  		= GET_WORD(sp)	\
		b.ubXYUnit	  		= GET_BYTE(sp)	\
		b.uwXDensity 		= GET_WORD(sp)	\
		b.uwYDensity 		= GET_WORD(sp)	\

#define	SET_JFIF_HEADER(sp, b)		\
		b.uwJFIF	  		= GET_WORD_BE(sp)	\
		b.uwLen	  		    = GET_WORD_BE(sp)	\

#define	SET_EXIF_HEADER(sp, b)		\
		b.uwEXIF	  		= GET_WORD_BE(sp)	\
		b.uwLen	  		    = GET_WORD_BE(sp)	\

#define	SET_SOF_HEADER(sp, b)		\
		b.uwSOF	  		    = GET_WORD_BE(sp)	\
		b.uwLen	  		    = GET_WORD_BE(sp)	\
		b.ubColor	  		= GET_BYTE(sp)	\
		b.uwHeight	  		= GET_WORD_BE(sp)	\
		b.uwWidth  		    = GET_WORD_BE(sp)	\
		b.ubNumOfColor	  	= GET_BYTE(sp)	\

//static JPG_APP0_HEADER	bAPP0;
//static JPG_JFIF_HEADER	bJFIF;
//static JPG_EXIF_HEADER	bEXIF;
static JPG_SOF_HEADER	bSOF;

void DSC_DecodeEx(MMP_ULONG *ulAddr, MMP_ULONG ulLen) //ucJpeg : 0 for normal case, 1 for low battery case
{
	MMP_DSC_JPEG_INFO jpeginfo;
	MMP_ERR           err;

	MMPS_DSC_CustomedPlaybackAttr(MMP_TRUE,
								  MMP_FALSE,
								  MMP_DISPLAY_ROTATE_NO_ROTATE,
								  MMP_SCAL_FITMODE_IN,
								  bSOF.uwWidth,
								  bSOF.uwHeight,
								  0,
								  0,
								  bSOF.uwWidth,
								  bSOF.uwHeight);
	
	jpeginfo.ulJpegBufAddr          = *ulAddr;
	jpeginfo.usJpegFileNameLength   = NULL;
    jpeginfo.ulJpegBufSize          = ulLen;
	jpeginfo.bDecodeThumbnail       = MMP_FALSE;
	jpeginfo.bValid                 = MMP_FALSE;
	jpeginfo.usPrimaryWidth         = bSOF.uwWidth;
	jpeginfo.usPrimaryHeight        = bSOF.uwHeight;	
	#if 0
	err = MMPS_DSC_PlaybackJpeg(&jpeginfo);
	#else
	err = MMPS_DSC_PlaybackJpegEx(&jpeginfo, 
		                          MMP_FALSE,
		                          MMP_DISPLAY_COLOR_RGB888);
    *ulAddr = jpeginfo.ulJpegBufAddr;
	#endif
}

int OpenJpg(AIT_STORAGE_INDEX_TABLE2 *bt, IMG *img, unsigned char** pptr, int idx)
{
	unsigned char	*buf, *tmp;
    unsigned char   bErr = MMP_TRUE; 
	int				patno;    
	unsigned int    ulOffset;    
	unsigned int    ulInterStoreLen;    
    unsigned int    ulCompressBufSt;  
    
	patno = SPLASH_PARTITION_START + idx;
    ulCompressBufSt = AIT_SYS_MEDIA_START_ADDR;
	buf = (unsigned char*)ulCompressBufSt;

    //bt->it[patno].ulRealSize = 128; //64K
    ulInterStoreLen = (bt->it[patno].ulSecSize<<9);    
    MMPF_SF_FastReadData((bt->it[patno].ulStartSec<<9),
    					 (int)buf,
    					 ulInterStoreLen);    
    tmp = buf;
    
    for (ulOffset=0; ulOffset<ulInterStoreLen; ulOffset++) {
        if( *(buf+ulOffset) == 0xFF) {
            if( *(buf+ulOffset+1) == 0xC0) {
                tmp += ulOffset;
                SET_SOF_HEADER(tmp, bSOF);
                ulOffset+=(bSOF.uwLen);
                tmp = buf;
				if ((bSOF.uwWidth *  bSOF.uwHeight) >= (320*240)) {
	                RTNA_DBG_Str(0, FG_YELLOW("JPG_LOGO[W:H]/["));
    	            RTNA_DBG_Dec(0, bSOF.uwWidth);
        	        RTNA_DBG_Str(0, FG_YELLOW(":"));            
            	    RTNA_DBG_Dec(0, bSOF.uwHeight);            
                	RTNA_DBG_Str(0, FG_YELLOW("]\r\n"));                
					bErr = MMP_FALSE;			
					break;
				}
            }			
        }            
    }

    if (bErr == MMP_TRUE) {
        MMP_PRINT_RET_ERROR(0, 0, FG_RED("[ERR]Parser size of JPG"), 0);
        //RTNA_DBG_Str(0, FG_RED("Parser JPG Error!"));
        //RTNA_DBG_Str(0, "\r\n");
        return 0;
    }
    
    //if (dec->dpblist[0]->is_invalid) 
    {   
        MMPC_DSC_InitConfig();
         // Open IBC interrupt
        MMPF_IBC_Init();

	  	//MMPS_Display_SetOutputPanel(MMP_DISPLAY_PRM_CTL, MMP_DISPLAY_SEL_MAIN_LCD);
	  	//MMPS_Display_SetWinPriority(MMP_DISPLAY_WIN_PIP, MMP_DISPLAY_WIN_MAIN, MMP_DISPLAY_WIN_OVERLAY, MMP_DISPLAY_WIN_OSD);
	    //MMPS_Display_SetWinActive(MMP_DISPLAY_WIN_MAIN, 0);
	    //MMPS_Display_SetWinActive(MMP_DISPLAY_WIN_OVERLAY, 0);
	    //MMPS_Display_SetWinActive(MMP_DISPLAY_WIN_OSD, 0);
	    //MMPS_Display_SetWinActive(MMP_DISPLAY_WIN_PIP, 0);       
        
        DSC_DecodeEx(&ulCompressBufSt, ulInterStoreLen);    
        img->img_w	  = bSOF.uwWidth;
        img->img_h	  = bSOF.uwHeight;
        img->img_clrs = bSOF.ubColor * 3;
        img->img_Size = bSOF.uwWidth * bSOF.uwHeight;
        *pptr = (unsigned char*)ulCompressBufSt;      
    }
    return ulInterStoreLen;
}
#endif //#if (BOOT_IMG_FORMAT == 1)

#if (BOOT_IMG_FORMAT == 0)
int OpenBmp(AIT_STORAGE_INDEX_TABLE2 *bt, IMG *img, unsigned char** pptr, int idx)
{
	unsigned char	*buf, *tmp;
	int				total;
	int				patno;
	
	patno = SPLASH_PARTITION_START + idx;
	buf = (unsigned char*)AIT_SYS_MEDIA_START_ADDR;
	
    MMPF_SF_FastReadData((bt->it[patno].ulStartSec<<9),
    					 (int)buf,
    					 BITMAPFILEHEADER_SIZE + BITMAPINFOHEADER_SIZE);
    
    tmp = buf;
    
    SET_BITMAPFILEHEADER(tmp, bf);
    SET_BITMAPINFOHEADER(tmp, bi);
	
	if (bf.bfType != BM)
		return 0;

	total = (((bi.biWidth * bi.biBitCount + 31) >> 5) << 2) * bi.biHeight;
    
    MMPF_SF_FastReadData((bt->it[patno].ulStartSec<<9) + bf.bfOffBits,
    					 (int)buf,
    					 total);
    
    img->img_w	  = bi.biWidth;
    img->img_h	  = bi.biHeight;
    img->img_clrs = bi.biBitCount;
    *pptr = buf;

    return total;
}
#endif //#if (BOOT_IMG_FORMAT == 0)

#if !defined(SD_BOOT)
static int OpenImage(AIT_STORAGE_INDEX_TABLE2 *bt, IMG *img, unsigned char** pptr, int idx)
{
	unsigned char	*buf, *tmp;
	int				total = 0;
	int				patno;
    unsigned short  bwSOIType;	    
	
	patno 	= SPLASH_PARTITION_START;
	buf 	= (unsigned char*)AIT_SYS_MEDIA_START_ADDR;
	//Read 2byte(SOI) to judge format
    MMPF_SF_FastReadData((bt->it[patno].ulStartSec<<9),
    					 (int)buf,
    					 2);

    tmp = buf;
    bwSOIType = GET_WORD_BE(tmp);
#if (BOOT_IMG_FORMAT == 1)    
	if ( bwSOIType == JPG_SOI) {
		total = OpenJpg(bt, img, pptr, idx);        
	}    
#else 
	if ( bwSOIType == BM) {
		RTNA_DBG_Str(0, FG_YELLOW("BMP!\r\n"));        
		total = OpenBmp(bt, img, pptr, idx);
	}
#endif     
	else {
		// Old version PowerOn logo and LowBattery appended in SPLASH_PARTITION_START
		int		off;
		RTNA_DBG_Str(0, FG_YELLOW("OLD_VER!\r\n"));		
		off = (idx == BATTERY_LOW_IDX)? (320 * 240 * 2) : (0);
		
	    MMPF_SF_FastReadData((bt->it[patno].ulStartSec<<9) + off,
	    					 (int)buf,
	    					 (bt->it[patno].ulSecSize<<9));//SysMedia.bin

	    *pptr = buf;

	    if (idx == BATTERY_LOW_IDX) {
		    img->img_w	  = LOWB_WIDTH;
		    img->img_h	  = LOWB_HEIGHT;
		    img->img_clrs = SPLASH_COLOR_DEPTH;
	    } else {	        
		    img->img_w	  = POWER_ON_WIDTH;
		    img->img_h	  = POWER_ON_HEIGHT;
		    img->img_clrs = SPLASH_COLOR_DEPTH;
		}
		total = (((img->img_w * img->img_clrs + 31) >> 5) << 2) * img->img_h;
	}

    return total;
}
#endif

void DrawPowerOnImage(DISP_PROP *dp, unsigned char* img, int w, int h, int clrs, int x, int y)
{
	MMP_DISPLAY_WIN_ATTR    winattribute;
	MMP_DISPLAY_DISP_ATTR	dispattribute;
	int					    disptype;
#if (TVOUT_PREVIEW_EN || HDMI_PREVIEW_EN)
	MMP_BOOL 			    IsTVHdmiConnect = MMP_FALSE;
#endif

	if (dp->disp_type == DISP_LCDV) 
    {
    	winattribute.usWidth  = h;
    	winattribute.usHeight = w;
    }
    else
    {
    	winattribute.usWidth  = w;
    	winattribute.usHeight = h;
    }
	
	switch (clrs) 
	{
		case 16:
			winattribute.colordepth = MMP_DISPLAY_WIN_COLORDEPTH_16;
			winattribute.usLineOffset = winattribute.usWidth * 2;
            break;
		case 24:
			winattribute.colordepth = MMP_DISPLAY_WIN_COLORDEPTH_24;
			winattribute.usLineOffset = winattribute.usWidth * 3;
            break;
		case 32:
			winattribute.colordepth = MMP_DISPLAY_WIN_COLORDEPTH_32;
			winattribute.usLineOffset = winattribute.usWidth << 2;
			break;
		default:
			return;
	}		

	winattribute.ulBaseAddr 		= (int)img;
	            
	dispattribute.usStartX 			= 0;
	dispattribute.usStartY 			= 0;

	if (dp->disp_type == DISP_LCDV) {
		dispattribute.usDisplayWidth 	= h;
		dispattribute.usDisplayHeight 	= w;
		dispattribute.rotatetype 		= MMP_DISPLAY_ROTATE_NO_ROTATE; //MMP_DISPLAY_ROTATE_RIGHT_90;
		dispattribute.usDisplayOffsetX 	= y;
		dispattribute.usDisplayOffsetY 	= x;
	} else {
		dispattribute.usDisplayWidth 	= w;
		dispattribute.usDisplayHeight 	= h;        
		dispattribute.rotatetype 		= MMP_DISPLAY_ROTATE_NO_ROTATE;
		dispattribute.usDisplayOffsetX 	= x;
		dispattribute.usDisplayOffsetY 	= y;
	}

	dispattribute.bMirror 			= 0;
    
    switch (dp->disp_type) 
    {
	    case DISP_LCD:
	    case DISP_LCDV:
			disptype = MMP_DISPLAY_SEL_MAIN_LCD;
			break;
		case DISP_NTSC:
			disptype = MMP_DISPLAY_SEL_NTSC_TV;
			break;
		case DISP_PAL:
			disptype = MMP_DISPLAY_SEL_PAL_TV;
			break;
		case DISP_HDMI:
			disptype = MMP_DISPLAY_SEL_HDMI;
			break;
	    default:
	        dp->disp_type = DISP_LCD;
			disptype = MMP_DISPLAY_SEL_MAIN_LCD;
			break;
    }

	MMPS_Display_SetOutputPanel(MMP_DISPLAY_PRM_CTL, disptype);    
	MMPS_Display_SetWinPriority(MMP_DISPLAY_WIN_OVERLAY, MMP_DISPLAY_WIN_MAIN, MMP_DISPLAY_WIN_PIP, MMP_DISPLAY_WIN_MAX);

	MMPS_Display_SetWinTransparent(MMP_DISPLAY_WIN_OVERLAY, MMP_FALSE, 0);
	MMPS_Display_SetWinSemiTransparent(MMP_DISPLAY_WIN_OVERLAY, MMP_FALSE, MMP_DISPLAY_SEMITP_AVG, 0);
    MMPS_Display_SetAlphaBlending(MMP_DISPLAY_WIN_OVERLAY, MMP_FALSE, MMP_DISPLAY_ALPHA_ARGB);
    MMPS_Display_SetWindowAttrToDisp(MMP_DISPLAY_WIN_OVERLAY, winattribute, dispattribute);

    MMPS_Display_SetWinActive(MMP_DISPLAY_WIN_MAIN, 0);
    MMPS_Display_SetWinActive(MMP_DISPLAY_WIN_OVERLAY, 1);

	#ifdef FLM_GPIO_NUM
	MMPS_Display_SetDisplayRefresh(MMP_DISPLAY_PRM_CTL); 
	#endif

	#if (TVOUT_PREVIEW_EN)
	IsTVHdmiConnect |= IsTVConnect();
	#endif
	
	#if (HDMI_PREVIEW_EN)
	IsTVHdmiConnect |= IsHdmiConnect();
	#endif

	if(m_ubShowPowerOnImage)
	{
		#if (TVOUT_PREVIEW_EN || HDMI_PREVIEW_EN)
		if(IsTVHdmiConnect) {
			RTNA_WAIT_MS(CFG_BOOT_CUS_POWER_ON_IMG_DELAY_TV);
		}
		else
		#endif
		{
			RTNA_WAIT_MS(10); // reduce boot speed RTNA_WAIT_MS(150);//CZ patch...20160204
			RTNA_LCD_Backlight(MMP_TRUE);
		} 
	}
	else
	{
	#if (TVOUT_PREVIEW_EN || HDMI_PREVIEW_EN)
		
		#ifdef TV_ONLY
		{
			AITPS_DSPY		pDSPY = AITC_BASE_DSPY;
			pDSPY->DSPY_MAIN_CTL &= ~1;
			pDSPY->DSPY_OVLY_CTL &= ~1;
			pDSPY->DSPY_PIP_CTL  &= ~1;
		}
		#else//TV_ONLY
		if(!IsTVHdmiConnect)
		{
			RTNA_LCD_Backlight(MMP_FALSE);
		}
		else
		{
			AITPS_DSPY		pDSPY = AITC_BASE_DSPY;
			pDSPY->DSPY_MAIN_CTL &= ~1;
			pDSPY->DSPY_OVLY_CTL &= ~1;
			pDSPY->DSPY_PIP_CTL  &= ~1;
		}
		#endif//TV_ONLY
		
	#else//TVOUT_PREVIEW_EN
		RTNA_LCD_Backlight(MMP_FALSE);
	#endif//TVOUT_PREVIEW_EN
	}

#ifdef CFG_BOOT_CUS_POWER_ON_IMG_DELAY //may be defined in config_xxx.h
	RTNA_WAIT_MS(CFG_BOOT_CUS_POWER_ON_IMG_DELAY);
#endif
}

#if POWER_ON_BATTERY_DETECTION
static void ShowLowPowerWarning(AIT_STORAGE_INDEX_TABLE2 *bt, DISP_PROP *dp)
{
#if (BOOT_FROM_SF == 1)
	int				x, y;
	unsigned char	*img;
	BMP				theimg;

	OpenImage(bt, &theimg, &img, BATTERY_LOW_IDX);
	x = (dp->disp_w - theimg.img_w) >> 1;
	y = (dp->disp_h - theimg.img_h) >> 1;

	DrawPowerOnImage(dp, img, theimg.img_w, theimg.img_h, theimg.img_clrs, x, y);
#endif
}
#endif

#if 0
void _____TV_Out_________(){ruturn;} //dummy
#endif

MMP_BOOL IsTVConnect(void)
{
#ifdef	TV_ONLY

    return MMP_TRUE;

#elif defined(TV_OUT_DETECT_GPIO)

	if (TV_OUT_DETECT_GPIO != MMP_GPIO_MAX)
	{
	    MMP_UBYTE tempValue;

		MMPF_PIO_GetData(TV_OUT_DETECT_GPIO, &tempValue);
		return (tempValue == TV_CONNECTED_STATE)? (MMP_TRUE):(MMP_FALSE);
	}

#endif

	return MMP_FALSE;
}

MMP_BOOL IsHdmiConnect(void)
{
	return MMPF_HDMI_IsConnect();
}

void InitTVDetectGpio(void)
{
#if defined(TV_OUT_DETECT_GPIO)
	if (TV_OUT_DETECT_GPIO != MMP_GPIO_MAX)
	{
        //MMPF_PIO_PadConfig(TV_OUT_DETECT_GPIO, PAD_NORMAL_TRIG, MMP_TRUE);
	    MMPF_PIO_EnableOutputMode(TV_OUT_DETECT_GPIO, MMP_FALSE, MMP_TRUE);
	    MMPF_PIO_EnableInterrupt(TV_OUT_DETECT_GPIO, MMP_FALSE, 0, (GpioCallBackFunc *)NULL, MMP_TRUE);
	}
#endif
}

#if 0
void _____CARDV_PER_INIT_____(){ruturn;} //dummy
#endif

static void CarDV_PowerOn_Check(AIT_STORAGE_INDEX_TABLE2 *bt, DISP_PROP	*dp)
{
	MMP_UBYTE   ubPowerOnTriggerSrc = 0;
    MMP_UBYTE	ubCheckBatteryStatus = 1;
    MMP_UBYTE 	bUSBStatus = !DEVICE_GPIO_DC_INPUT_LEVEL;		// USB not in
	MMP_UBYTE   bPWRKeyStatus = !KEY_PRESSLEVEL_POWER;
    MMP_UBYTE   bDCStatus = !DEVICE_GPIO_DC_INPUT;				// DC not in

    #ifdef KEY_GPIO_POWER 
	if (KEY_GPIO_POWER != MMP_GPIO_MAX) {
        MMPF_PIO_PadConfig(KEY_GPIO_POWER, PAD_NORMAL_TRIG, MMP_TRUE);
		MMPF_PIO_EnableOutputMode((MMP_GPIO_PIN) KEY_GPIO_POWER, MMP_FALSE, MMP_TRUE);
		MMPF_PIO_GetData((MMP_GPIO_PIN) KEY_GPIO_POWER, &bPWRKeyStatus);

        if (KEY_PRESSLEVEL_POWER == bPWRKeyStatus) {
        	ubPowerOnTriggerSrc = PWR_ON_BY_KEY;
            RTNA_DBG_Str(0,FG_GREEN("--I-- Power-on by PWR_KEY\r\n"));
        }
	}
    #endif

	#if POWER_ON_BY_GSENSOR_EN && defined(GSENSOR_INT)
	if (GSENSOR_INT != MMP_GPIO_MAX) {
        MMP_UBYTE 	bGSnrStatus = 0;

        MMPF_PIO_PadConfig(GSENSOR_INT, 0, MMP_TRUE);
	    MMPF_PIO_EnableOutputMode(GSENSOR_INT, MMP_FALSE, MMP_TRUE);
	    MMPF_PIO_GetData((MMP_GPIO_PIN)GSENSOR_INT, &bGSnrStatus);
	    
	    if (GSENSOR_INT_ACT_LEVEL == bGSnrStatus) {
	        RTNA_DBG_Str(0, FG_GREEN("--I-- Power-on by G-Sensor"));
	        RTNA_DBG_Byte(0, bGSnrStatus);
	        RTNA_DBG_Str(0,"\r\n");

	    	ubPowerOnTriggerSrc = PWR_ON_BY_GSENSOR;
	    }
	}
	#endif

    #if POWER_ON_BY_GSENSOR_EN
    if (Gsensor_GetIntStatusForBoot()) {
        RTNA_DBG_Str(0, FG_GREEN("--I-- Power-on by G-Sensor INT status\r\n"));

        ubPowerOnTriggerSrc = PWR_ON_BY_GSENSOR;
    }
    #endif

	#if defined(DEVICE_GPIO_DC_INPUT)
    //Get DC In status. Set DEVICE_GPIO_DC_INPUT as input mode
    if (DEVICE_GPIO_DC_INPUT != MMP_GPIO_MAX) {
        MMPF_PIO_PadConfig(DEVICE_GPIO_DC_INPUT, PAD_NORMAL_TRIG, MMP_TRUE);
        MMPF_PIO_EnableOutputMode(DEVICE_GPIO_DC_INPUT, MMP_FALSE, MMP_TRUE);
        MMPF_PIO_GetData((MMP_GPIO_PIN)DEVICE_GPIO_DC_INPUT, &bDCStatus);

        if (DEVICE_GPIO_DC_INPUT_LEVEL == bDCStatus) {
			RTNA_DBG_Str(0, FG_GREEN("--I-- DC-In detected.\r\n"));
            
            if (!(ubPowerOnTriggerSrc & PWR_ON_BY_KEY) && !(ubPowerOnTriggerSrc & PWR_ON_BY_GSENSOR)) {
	            RTNA_DBG_Str(0, FG_GREEN("--I-- Power-on by DC-In\r\n"));
                ubPowerOnTriggerSrc = PWR_ON_BY_DC;
            }

    		ubCheckBatteryStatus = 0;
        }
    }
	#endif

	#if defined(USB_IN_DETECT_VBUS_PIN)
    // Get USB status. Set V_BUS_DETECT_GPIO as input mode
	if (USB_IN_DETECT_VBUS_PIN != MMP_GPIO_MAX) {
        MMPF_PIO_PadConfig(USB_IN_DETECT_VBUS_PIN, PAD_NORMAL_TRIG, MMP_TRUE);
	    MMPF_PIO_EnableOutputMode(USB_IN_DETECT_VBUS_PIN, MMP_FALSE, MMP_TRUE);
	    MMPF_PIO_GetData((MMP_GPIO_PIN)USB_IN_DETECT_VBUS_PIN, &bUSBStatus);

        if (USB_IN_DETECT_VBUS_PIN_ACTIVE == bUSBStatus) {
			RTNA_DBG_Str(0,"--I-- USB VBUS detected.\r\n");

            if (!(ubPowerOnTriggerSrc & PWR_ON_BY_KEY) && !(ubPowerOnTriggerSrc & PWR_ON_BY_GSENSOR)) {
	            RTNA_DBG_Str(0,FG_GREEN("--I-- Power-on by VBUS\r\n"));
                ubPowerOnTriggerSrc = PWR_ON_BY_VBUS;
            }
            
	    	ubCheckBatteryStatus = 0;
        }
	}
	#endif

    *(volatile MMP_UBYTE *) (TEMP_POWER_ON_SRC_INFO_ADDR) = ubPowerOnTriggerSrc;

	RTNA_DBG_Str(0, FG_GREEN("@@@ ubPowerOnTriggerSrc ="));
	RTNA_DBG_Byte(0, *(volatile MMP_UBYTE *) (TEMP_POWER_ON_SRC_INFO_ADDR));
	RTNA_DBG_Str(0, "\r\n");
    
    #if POWER_ON_BATTERY_DETECTION
	if (ubCheckBatteryStatus)
	{
		MMP_USHORT uLoop = 0;
		MMP_ULONG uLevelSum = 0;
        MMP_ULONG level;
		MMP_UBYTE source;

		/* Make Power on by G-Sensor / Power-key */
		#define BATTERY_MEASURE_TIMES		10

	    for (uLoop = 0; uLoop < BATTERY_MEASURE_TIMES; ++uLoop)
	    {
            PMUCtrl_ADC_Measure_Key(&level, &source, BATTERY_DETECT_ADC_CHANNEL);
            uLevelSum += level * REAL_BAT_VOLTAGE_RATIO_M / REAL_BAT_VOLTAGE_RATIO_N;
            RTNA_WAIT_MS(10);
        }

	    uLevelSum /= BATTERY_MEASURE_TIMES;

		RTNA_DBG_Str(0, "Booting battery voltage = ");
		RTNA_DBG_Dec(0, uLevelSum);
		RTNA_DBG_Str(0, "mV\r\n");

    	if (uLevelSum < BATTERY_LOW_BOOT_VOLT)
    	{
    		RTNA_DBG_Str(0, "Low Power!!!\r\n");
            ShowLowPowerWarning(bt, dp);
	        RTNA_WAIT_MS(BATTERY_LOW_BOOT_WARNING_TIME);
    		PMUCtrl_Power_Off();
            while(1);
        }

		RTNA_DBG_Str(0, "Battery In\r\n");
	}
	else 
    #endif      // POWER_ON_BATTERY_DETECTION
	{
		/* Make Power on by USB / DC */
		#ifdef CFG_BOOT_DCVBUS_IN_NOT_ON	// defined in config_xxx.h
		if ((PWR_ON_BY_VBUS == ubPowerOnTriggerSrc) || (PWR_ON_BY_DC == ubPowerOnTriggerSrc))
		{
			RTNA_DBG_Str(0, "Power-on by USB/Charger, turn-off power.\r\n");
			PMUCtrl_Power_Off();

			while(1);
	    }
		#endif
	}

	#ifdef CFG_KEY_LPRESS_POWER_UP // defined in config_sdk.h
	// needs long pressed power key to power on
	if (PWR_ON_BY_KEY == ubPowerOnTriggerSrc)
	{
    	#ifdef CFG_BOOT_CHECK_KEY_LONG_PRESS    // defined in config_sdk.h
		CFG_BOOT_CHECK_KEY_LONG_PRESS();        // defined in config_sdk.h
    	#else
		#ifdef CFG_CUS_CMBO_PWR_KEY
		MMP_UBYTE bCmboPwrKeyStatus = KEY_PRESSLEVEL_COMB_POWER;
		#endif

		#ifdef PWR_KEY_DELAY_TIME               // maybe defined in config_sdk.h
		RTNA_WAIT_MS(PWR_KEY_DELAY_TIME);
		#endif

        #ifdef KEY_GPIO_POWER 
		if (KEY_GPIO_POWER != MMP_GPIO_MAX) {
            MMPF_PIO_PadConfig(KEY_GPIO_POWER, PAD_NORMAL_TRIG, MMP_TRUE);
    		MMPF_PIO_EnableOutputMode((MMP_GPIO_PIN) KEY_GPIO_POWER, MMP_FALSE, MMP_TRUE);
    		MMPF_PIO_GetData((MMP_GPIO_PIN) KEY_GPIO_POWER, &bPWRKeyStatus);
		}
        #endif
        
		#ifdef CFG_CUS_CMBO_PWR_KEY
		if (CFG_CUS_CMBO_PWR_KEY != MMP_GPIO_MAX) {
            MMPF_PIO_PadConfig(CFG_CUS_CMBO_PWR_KEY, PAD_NORMAL_TRIG, MMP_TRUE);
    		MMPF_PIO_EnableOutputMode((MMP_GPIO_PIN) CFG_CUS_CMBO_PWR_KEY, MMP_FALSE, MMP_TRUE);
    		MMPF_PIO_GetData((MMP_GPIO_PIN) CFG_CUS_CMBO_PWR_KEY, &bCmboPwrKeyStatus);
        }
		#endif

		RTNA_DBG_Str(0, "KEY_GPIO_POWER =");
		RTNA_DBG_Byte(0, bPWRKeyStatus);
		RTNA_DBG_Str(0, "\r\n");

		if ((KEY_PRESSLEVEL_POWER != bPWRKeyStatus) &&
		    #ifdef CFG_CUS_CMBO_PWR_KEY
			(KEY_PRESSLEVEL_COMB_POWER != bCmboPwrKeyStatus) &&
		    #endif
			1)
		{
			RTNA_DBG_Str(0, "Error Trigger Power Key\r\n");
			PMUCtrl_Power_Off();
    		while(1);
		}
        #endif
	}
	#endif

	#ifdef CFG_CUS_OEM_BOOT_INIT_FUNC       // defined in config_sdk.h
	CFG_CUS_OEM_BOOT_INIT_FUNC();
	#endif
		
	#if defined(CFG_BOOT_POWER_ON_VIBRATION) && defined(DEVICE_GPIO_VIBRATION)
	if (DEVICE_GPIO_VIBRATION != MMP_GPIO_MAX) {
    	RTNA_DBG_Str(0, "!!! Vibration On !!!\r\n");
        MMPF_PIO_PadConfig(DEVICE_GPIO_VIBRATION, PAD_OUT_DRIVING(0), MMP_TRUE);
        MMPF_PIO_EnableOutputMode(DEVICE_GPIO_VIBRATION, MMP_TRUE, MMP_TRUE);
        MMPF_PIO_SetData(DEVICE_GPIO_VIBRATION, DEVICE_GPIO_VIBRATION_ON, MMP_TRUE);
    }
	#endif
    
#if SUPPORT_GSENSOR && POWER_ON_BY_GSENSOR_EN
	if (PWR_ON_BY_GSENSOR == ubPowerOnTriggerSrc)
	{
		m_ubShowPowerOnImage = MMP_FALSE;
		RTNA_DBG_Str(0, "Power On by Gsensor, don't show LOGO\r\n");
	}
#endif 
}

volatile MMP_BOOL gbDMADone= MMP_FALSE;//CZ patch...20160204
void DISPLAYDMADone(void)
{
	gbDMADone = MMP_TRUE;
}

void Customer_Init_CarDV(void)
{
	DISP_PROP	dp;
	MMPF_PANEL_ATTR  *panel;
    AIT_STORAGE_INDEX_TABLE2 *pIndexTable = (AIT_STORAGE_INDEX_TABLE2 *) AIT_BOOT_HEADER_ADDR;

    panel = RTNA_LCD_GetAttr();
    dp.disp_w = panel->usPanelW;
    dp.disp_h = panel->usPanelH;
    dp.disp_type = panel->ubDevType;

	if (RGB_D24BIT_RGB888 == panel->ubRgbFmt) {
		dp.disp_clrs = 24;
    }
	else {
		dp.disp_clrs = 16;
	}

#if (VERTICAL_LCD == VERTICAL_LCD_DEGREE_0) 
        dp.disp_type = DISP_LCD;
#elif (VERTICAL_LCD == VERTICAL_LCD_DEGREE_90) || (VERTICAL_LCD == VERTICAL_LCD_DEGREE_270)
        dp.disp_type = DISP_LCDV;
        dp.disp_w = panel->usPanelH;
        dp.disp_h = panel->usPanelW;
#endif    

	GetMenuStatusInSF();
	OemSetLcdDirection();
	MMPF_SF_ReadData(MBR_ADDRESS, (MMP_ULONG) AIT_BOOT_HEADER_ADDR, 512);
	CarDV_PowerOn_Check(pIndexTable, &dp);

	#if(SUPPORT_SECURITY_IC)
	PassIc_Detect();
	#endif

	#if TVOUT_ENABLE
	InitTVDetectGpio();
	#endif

	#if (TVOUT_PREVIEW_EN)
	if(IsTVConnect()) 
	{
		if(m_ubTVSetting == TV_PAL)
		{
			dp.disp_w		= 720;
			dp.disp_h		= 576;
			dp.disp_clrs	= 16;
			dp.disp_type 	= DISP_PAL;
		}
		else
		{
			dp.disp_w		= 720;
			dp.disp_h		= 480;
			dp.disp_clrs	= 16;
			dp.disp_type 	= DISP_NTSC;
		}

		RTNA_DBG_Str(0, "BOOT TV-OUT DETECTED m_ubTVSetting[");
		RTNA_DBG_Byte(0, m_ubTVSetting);
		RTNA_DBG_Str(0, "]\r\n");
	}
	#endif

	#if (HDMI_PREVIEW_EN)

	MMPC_HDMI_PC_Enable(MMP_TRUE);

	if(IsHdmiConnect())
	{
		if(m_ubHDMISetting==HDMI_1080I)
		{
			dp.disp_w		= 1920;
			dp.disp_h		= 540;
			dp.disp_clrs	= 16;
			dp.disp_type 	= DISP_HDMI;
		}
		else if(m_ubHDMISetting==HDMI_720P)
		{
			dp.disp_w		= 1280;
			dp.disp_h		= 720;
			dp.disp_clrs	= 16;
			dp.disp_type 	= DISP_HDMI;
		}
		else if(m_ubHDMISetting==HDMI_480P)
		{
			dp.disp_w		= 720;
			dp.disp_h		= 480;
			dp.disp_clrs	= 16;
			dp.disp_type 	= DISP_HDMI;
		}

		RTNA_DBG_Str(0, "BOOT TV-OUT DETECTED m_ubHDMISetting[");
		RTNA_DBG_Byte(0, m_ubHDMISetting);
		RTNA_DBG_Str(0, "]\r\n");	
	}
	#endif	

    if (pIndexTable->ulHeader == INDEX_TABLE_HEADER &&
    	pIndexTable->ulTail   == INDEX_TABLE_TAIL)
    {
#if defined(CFG_BOOT_SKIP_POWER_ON_IMG) || defined(SD_BOOT)		//may be defined in config_xxx.h
		// NOP
#else
        RTNA_DBG_Str(0,"Start to show Logo ...\r\n");
        
		// Show at Center of SCREEN
		{
        	unsigned char	*img;
        	IMG				theimg;
        	int				x, y;
    	
        #if (TVOUT_PREVIEW_EN)
			if(IsTVConnect())
			{
			    if (OpenImage(pIndexTable, &theimg, &img, POWER_ON_IDX)) {
    				unsigned char* newimg0;
    				unsigned char* newimg1;
    				
    				RTNA_DBG_Str(0, "##############################\r\n");
    				RTNA_DBG_Str(0, "### PowerOnImage - TV Out\r\n");
    				RTNA_DBG_Str(0, "##############################\r\n");

    			    OpenImage(pIndexTable, &theimg, &img, POWER_ON_IDX);
    				FlipImage(&theimg, (unsigned short*)img, &newimg0);
    			
    				MagnifyImage(&theimg, (unsigned short*)newimg0, &newimg1, 2);
    			
    				x = (dp.disp_w - theimg.img_w) >> 1;
    				y = (dp.disp_h - theimg.img_h) >> 1;
    			
    				DrawPowerOnImage(&dp, newimg1, theimg.img_w, theimg.img_h, theimg.img_clrs, x, y);
                }
			}
			else
        #endif	// TVOUT_PREVIEW_EN
		#if 0// (HDMI_PREVIEW_EN)       // TBD
			if(IsHdmiConnect())
			{
			    if (OpenImage(pIndexTable, &theimg, &img, POWER_ON_IDX)) {
    				unsigned char* newimg0;
    				unsigned char* newimg1;
    				
    				RTNA_DBG_Str(0, "##############################\r\n");
    				RTNA_DBG_Str(0, "### PowerOnImage - HDMI Out\r\n");
    				RTNA_DBG_Str(0, "##############################\r\n");

    				FlipImage(&theimg, (unsigned short*)img, &newimg0);
    			
    			    OpenImage(pIndexTable, &theimg, &img, POWER_ON_IDX);
    				MagnifyImage(&theimg, (unsigned short*)newimg0, &newimg1, 2);
    			
    				x = (dp.disp_w - theimg.img_w) >> 1;
    				y = (dp.disp_h - theimg.img_h) >> 1;
    				DrawPowerOnImage(&dp, newimg1, theimg.img_w, theimg.img_h, theimg.img_clrs, x, y);
                }
			}
			else
		#endif	// HDMI_PREVIEW_EN
			{
        	#ifdef CFG_BOOT_SKIP_POWER_ON_IMG_ON_LCD //may be defined in config_xxx.h
			// NOP
			#else
			    if (OpenImage(pIndexTable, &theimg, &img, POWER_ON_IDX)) {
                    MMP_ULONG ulTimeOutTick = 0x1000000; 
                    RTNA_DBG_Str(0, "##############################\r\n");
                    RTNA_DBG_Str(0, "### PowerOnImage - LCD Out\r\n");
                    RTNA_DBG_Str(0, "##############################\r\n");
			    
    				x = (dp.disp_w - theimg.img_w) >> 1;
    				y = (dp.disp_h - theimg.img_h) >> 1;
                    gbDMADone = MMP_FALSE;//CZ patch...20160204
                    
#if (VERTICAL_LCD == VERTICAL_LCD_DEGREE_90)
                    {
                        MMP_GRAPHICS_BUF_ATTR srcBufAttr = {0};
                        MMP_GRAPHICS_BUF_ATTR dstBufAttr = {0};
                        MMP_GRAPHICS_RECT srcRect = {0};
                        MMP_ERR  sRet = {0};

                        MMPF_DMA_Initialize();
                        
                        //source address
                        srcBufAttr.colordepth = MMP_GRAPHICS_COLORDEPTH_24;
                        srcBufAttr.usWidth = theimg.img_w;
                        srcBufAttr.usHeight = theimg.img_h;
                        srcBufAttr.usLineOffset = theimg.img_w*3;

                        srcBufAttr.ulBaseAddr  = (MMP_ULONG)img;
                        srcBufAttr.ulBaseUAddr = 0;
                        srcBufAttr.ulBaseVAddr = 0;

                        //destination address
                        dstBufAttr.colordepth = MMP_GRAPHICS_COLORDEPTH_24;               
                        dstBufAttr.usWidth      = srcBufAttr.usHeight;//dp.disp_w;
                        dstBufAttr.usHeight     = srcBufAttr.usWidth;//dp.disp_h;                   
                        dstBufAttr.usLineOffset = dstBufAttr.usWidth*3;
                        dstBufAttr.ulBaseAddr   = TEMP_POWER_ON_IMAGE_DMA_DEST_BUFF; //Temp address for rotated bmp
                        //ulDstAddr = (UINT32)m_OSD[uwDesIndex]->data;                
                        srcRect.usLeft   = 0;
                        srcRect.usTop    = 0;
                        srcRect.usWidth  = srcBufAttr.usWidth;
                        srcRect.usHeight = srcBufAttr.usHeight;
                                                 
                        sRet = MMPD_DMA_RotateImageBuftoBuf(&srcBufAttr, 
                                                            &srcRect, 
                                                            &dstBufAttr, 
                                                            0,
                                                            0, 
                                                            MMP_GRAPHICS_ROTATE_RIGHT_90, 
                                                            DISPLAYDMADone,//NULL,//CZ patch...20160204
                                                            MMP_FALSE, 
                                                            DMA_NO_MIRROR);
    					//RTNA_WAIT_MS(50); // Wait for Rotate DMA finished
    					
                        while((gbDMADone == MMP_FALSE) && (--ulTimeOutTick > 0));//CZ patch...20160204
                        if (ulTimeOutTick == 0) {
                            MMP_PRINT_RET_ERROR(0, 0, "PowerOn Image While TimeOut\r\n",0);
                            //return MMP_DSC_ERR_CAPTURE_FAIL;
                        }

                        img = (unsigned char*)dstBufAttr.ulBaseAddr;
                    }
#elif (VERTICAL_LCD == VERTICAL_LCD_DEGREE_270)
                    {
                        MMP_GRAPHICS_BUF_ATTR srcBufAttr = {0};
                        MMP_GRAPHICS_BUF_ATTR dstBufAttr = {0};
                        MMP_GRAPHICS_RECT srcRect = {0};
                        MMP_ERR  sRet = {0};

                        MMPF_DMA_Initialize();
                        
                        //source address
                        srcBufAttr.colordepth = MMP_GRAPHICS_COLORDEPTH_24;
                        srcBufAttr.usWidth = theimg.img_w;
                        srcBufAttr.usHeight = theimg.img_h;
                        srcBufAttr.usLineOffset = theimg.img_w*3;

                        srcBufAttr.ulBaseAddr  = (MMP_ULONG)img; //AHC_GUI_TEMP_BASE_ADDR;
                        srcBufAttr.ulBaseUAddr = 0;
                        srcBufAttr.ulBaseVAddr = 0;

                        //destination address
                        dstBufAttr.colordepth = MMP_GRAPHICS_COLORDEPTH_24;               
                        dstBufAttr.usWidth      = srcBufAttr.usHeight;//dp.disp_w;
                        dstBufAttr.usHeight     = srcBufAttr.usWidth;//dp.disp_h;                   
                        dstBufAttr.usLineOffset = dstBufAttr.usWidth*3;
                        dstBufAttr.ulBaseAddr   = TEMP_POWER_ON_IMAGE_DMA_DEST_BUFF; //Temp address for rotated bmp
                        //ulDstAddr = (UINT32)m_OSD[uwDesIndex]->data;                
                        srcRect.usLeft   = 0;
                        srcRect.usTop    = 0;
                        srcRect.usWidth  = srcBufAttr.usWidth;
                        srcRect.usHeight = srcBufAttr.usHeight;
                                                 
                        sRet = MMPD_DMA_RotateImageBuftoBuf(&srcBufAttr, 
                                                            &srcRect, 
                                                            &dstBufAttr, 
                                                            0,
                                                            0, 
                                                            MMP_GRAPHICS_ROTATE_RIGHT_270, 
                                                            DISPLAYDMADone,//NULL,//CZ patch...20160204
                                                            MMP_FALSE, 
                                                            DMA_NO_MIRROR);
//    					RTNA_WAIT_MS(50); // Wait for Rotate DMA finished

                        while((gbDMADone == MMP_FALSE) && (--ulTimeOutTick > 0));//CZ patch...20160204
                        if (ulTimeOutTick == 0) {
                            MMP_PRINT_RET_ERROR(0, 0, "PowerOn Image While TimeOut\r\n",0);
                            //return MMP_DSC_ERR_CAPTURE_FAIL;
                        }

                        img = (unsigned char*)dstBufAttr.ulBaseAddr;
                    }
#endif                    
    				DrawPowerOnImage(&dp, img, theimg.img_w, theimg.img_h, theimg.img_clrs, x, y);
                }
#endif	// CFG_BOOT_SKIP_POWER_ON_IMG_ON_LCD
            }
        }
#endif	// CFG_BOOT_SKIP_POWER_ON_IMG
    }	
}

void OemSetLcdDirection(void)
{
	MMP_UBYTE LCDStatus;
	
	LCD_CheckStatus(&LCDStatus);
	
	RTNA_DBG_Str(0,"[Bootloader] LCD_SetDirection = ");
	RTNA_DBG_Byte(0,LCDStatus);
	RTNA_DBG_Str(0,"\r\n");
		
	if(LCDStatus==LCD_NORMAL)
		RTNA_LCD_Direction(LCD_0_DEGREE);
	else if(LCDStatus==LCD_REVERSE)
		RTNA_LCD_Direction(LCD_180_DEGREE);
}


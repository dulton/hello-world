#if 0 //Rogers:***
// mmpf
#include "Ait_utility.h"
#include "mmpf_hw_counter.h"
#include "Mmpf_pio.h"
#include "Mmpf_typedef.h"
#include "controlIC_adx2003.h"
// mmps & mmpd
#include "includes_fw.h"
#include "mmps_dsc.h"
#include "mmps_sensor.h"

// ahc
#include "AHC_Display.h"
#include "AHC_General.h"
#include "ahc_parameter.h"
#include "AHC_Menu.h"
#include "AHC_utility.h"

#include "MotorCtrlFunc.h"
#include "ProductLineCali.h"

// isp
#include "isp_if.h"
#include "AE.h"



#define CALI_DATA_VERSION 0x0006  //modify by casio,each CALI_DATA_VERSION has their checksum(see checksumlen to be printed on tara)

#define INF_CALI_DATA_ADDR 0x3600000
#define LS_CS_CALI_DATA_ADDR 0x3500000


extern void SetRawPreviewMode(MMP_BOOL bForcedPreviewModeEn_in, MMP_UBYTE bForcedPreviewMode_in);
#if 0
extern int DSC_StartPreview(void);
extern int DSC_StopPreview(void);
#endif
extern void printc( char* szFormat, ... );
extern int MMPF_FS_FOpen(MMP_BYTE *pFileName, const MMP_BYTE *pMode, MMP_ULONG *ulFileID);
extern int MMPF_FS_FWrite(MMP_ULONG ulFileID, void *pData, MMP_ULONG NumBytes , MMP_ULONG *write_count);
extern int MMPF_FS_FClose(MMP_ULONG ulFileID);
extern int MMPF_FS_FRead(MMP_ULONG ulFileID, void *pData, MMP_ULONG NumBytes, MMP_ULONG *read_count);
extern int sprintc( char* szOutput, char* szFormat, ... );
extern void AHC_GetFwVersion( unsigned short * puwFwMajorVersion,
                unsigned short * puwFwMediumVersion, 
                unsigned short * puwFwMinorVersion, 
                unsigned short * puwFwBranchVersion, 
                unsigned short * puwFwTestVersion, 
                char**           pszReleaseName );
extern int AHC_SetISPParam(AHC_ISP_PARAM_ID wParamID, UINT8* wAddr);
extern int AHC_GetISPParam(AHC_ISP_PARAM_ID wParamID, UINT32 *wValue);
extern void AHC_PrintFwVersion(void);
extern int AHC_FormatStorageMedia(UINT8 byMediaID);
extern int sprintf(char * /*s*/, const char * /*format*/, ...);
extern void OSD_Draw_TestMessage(UINT32 Left, UINT32 Top, char result,char  sText[20] );

static MMP_USHORT               m_usPreviewMode 	= DSC_NORMAL_PREVIEW_MODE;
static MMP_DISPLAY_OUTPUT_SEL   m_displayPanel 		= MMP_DISPLAY_SEL_MAIN_LCD;
static MMP_BOOL                 m_bForcedPreviewModeEn	= MMP_FALSE;
static MMP_UBYTE                m_bForcedPreviewMode	= 0;
static MMP_BOOL                 m_bRawPreviewMode	= MMP_FALSE;
static MMP_USHORT               m_usCurrentSensor 	= MAIN_SENSOR;
extern MMP_BOOL CalibrationMode;


//ubType: 0 for 2568x1932, 1 for 1292x967, 2 for 1928x1087, 3 for 644x481(30p), 4 for 644x482(120p)
void CaptureRaw_AfterCS(MMP_UBYTE ubType, MMP_BYTE  *m_bLsCsRawFileName)
{
	MMP_ERR     err;
    MMP_UBYTE * tempaddr = 0;
    MMP_ULONG ulWidth;
    MMP_ULONG ulHeight;
    MMP_USHORT usFileNameLen = 0;
	MMP_UBYTE bcg_en_ori,ls_en_ori,cs_en_ori;
	ISP_AE_EV_BIAS evbias_ori;
	// MMP_BYTE  *m_bLsCsRawFileName 	= "SD:\\LsCsCali.raw";

	// get original module enable signal 
	bcg_en_ori = ISP_IF_IQ_GetSwitch(ISP_IQ_MODULE_BCG);
	ls_en_ori = ISP_IF_IQ_GetSwitch(ISP_IQ_MODULE_LS);
	cs_en_ori = ISP_IF_IQ_GetSwitch(ISP_IQ_MODULE_CS);
	evbias_ori = ISP_IF_AE_GetEVBias();

	AHC_SetMode(0x00); // switch to idle mode
	ISP_IF_AE_SetAELock(ISP_AE_LOCK_DSB);

	// turn on preview mode to train AE
	AHC_SetMode(0x10);  // DSC preview mode
	ISP_IF_IQ_SetSwitch(ISP_IQ_MODULE_BCG,MMP_FALSE);
	ISP_IF_IQ_SetSwitch(ISP_IQ_MODULE_LS,MMP_FALSE);
	ISP_IF_IQ_SetSwitch(ISP_IQ_MODULE_CS,MMP_FALSE);

	ISP_IF_3A_SetSwitch(ISP_3A_ALGO_AWB, 0 );
	ISP_IF_3A_SetSwitch(ISP_3A_ALGO_AF, 0 );
	ISP_IF_AE_SetMetering(ISP_AE_METERING_SPOT);
	ISP_IF_AE_SetEVBias(ISP_AE_EV_BIAS_P200);	
	MMPF_OS_Sleep(1000);

	// ISP_IF_AE_SetAELock(ISP_AE_LOCK_ENB);

	AHC_SetMode(0x00);  // DSC preview mode

    // m_bRawPreviewMode = MMP_TRUE;
	// SetRawPreviewMode(MMP_TRUE);

	// ready to capture RAW
#if 0	
	SetRawPreviewMode(MMP_TRUE, ubType); // 0 for 2568x1932, 1 for 1292x967, 2 for 1928x1087, 3 for 644x481(30p), 4 for 644x482(120p)
#else
	m_bForcedPreviewModeEn	= MMP_TRUE;
	m_bForcedPreviewMode	= ubType;
#endif

    MMPS_DSC_SetPreviewPath(MMPS_DSC_CS_PREVIEW, MMPS_DSC_COLORDEPTH_10);
    MMPS_DSC_EnablePreviewDisplay(PRM_SENSOR, MMP_FALSE, MMP_TRUE);

    MMPS_Display_SetOutputPanel(MMP_DISPLAY_PRM_CTL, MMP_DISPLAY_SEL_MAIN_LCD);
    #if (CHIP == P_V2)
    MMPS_Display_SetWinPriority(MMP_DISPLAY_WIN_ICON, MMP_DISPLAY_WIN_OVERLAY, MMP_DISPLAY_WIN_MAIN, MMP_DISPLAY_WIN_PIP);
    #endif
    #if (CHIP == MCR_V2)
    MMPS_Display_SetWinPriority(MMP_DISPLAY_WIN_OVERLAY, MMP_DISPLAY_WIN_MAIN, MMP_DISPLAY_WIN_PIP, MMP_DISPLAY_WIN_MAX);
    #endif
    MMPS_DSC_ResetPreviewMode(); 
    MMPS_DSC_SetSystemMode(DSC_NORMAL_PREVIEW_MODE);

    err = MMPS_Sensor_Initialize(MAIN_SENSOR, 1, 1/*MMP_SNR_DSC_PRW_MODE*/);        
    
  	MMPS_Sensor_SetCaptureMode(PRM_SENSOR, 0);
    if (err == MMP_ERR_NONE) {
        err = MMPS_DSC_EnablePreviewDisplay(PRM_SENSOR, MMP_TRUE, MMP_FALSE);
    } 

	ISP_IF_IQ_SetSwitch(ISP_IQ_MODULE_BCG,MMP_FALSE);
	ISP_IF_IQ_SetSwitch(ISP_IQ_MODULE_LS,MMP_FALSE);
	ISP_IF_IQ_SetSwitch(ISP_IQ_MODULE_CS,MMP_FALSE);
	// printc("ori AEG=%d \n",ISP_IF_IQ_GetSwitch(ISP_IQ_MODULE_AEG));

	ISP_IF_3A_SetSwitch(ISP_3A_ALGO_AWB, 0 );
	ISP_IF_3A_SetSwitch(ISP_3A_ALGO_AF, 0 );
	ISP_IF_AE_SetMetering(ISP_AE_METERING_SPOT);
	ISP_IF_SNR_SetDigitalGain(ISP_IF_AE_GetSensorDigitalGain() *4/3);
	MMPF_OS_Sleep(1000);

	#if 1
    #if (FS_INPUT_ENCODE == UCS2)
	usFileNameLen = uniStrlen((const short *)m_bLsCsRawFileName);
	#else
	usFileNameLen = STRLEN(m_bLsCsRawFileName);
	#endif
    MMPS_DSC_GetRawData(PRM_SENSOR, tempaddr,MMP_TRUE, m_bLsCsRawFileName, usFileNameLen, &ulWidth, &ulHeight); 
	printc("Raw size is %dx%d\n",ulWidth,ulHeight);
	#endif

	// set back to original value
	//ISP_IF_IQ_SetSwitch(ISP_IQ_MODULE_BCG,bcg_en_ori);
	//ISP_IF_IQ_SetSwitch(ISP_IQ_MODULE_LS,ls_en_ori);
	//ISP_IF_IQ_SetSwitch(ISP_IQ_MODULE_CS,cs_en_ori);
	//ISP_IF_3A_SetSwitch(ISP_3A_ALGO_AWB, 1 );
	//ISP_IF_3A_SetSwitch(ISP_3A_ALGO_AF, 1 );
	//ISP_IF_AE_SetEVBias(evbias_ori);
}

//add by casio
void writeFSCaliData(char* BinFileName, MMP_UBYTE* addr_out, MMP_ULONG len)
{
	MMP_ULONG ulWirteCount;
	MMP_ERR err;
	static MMP_ULONG exCaliDSCFileID;
	if (MMPF_FS_FOpen((MMP_BYTE *)BinFileName, "wb", &exCaliDSCFileID) != MMP_ERR_NONE) 
	{
		printc("MMPF_DSC_GetRawData FAIL: MMPF_FS_FOpen fail ... %s\n",BinFileName);	
	}
	printc("writeFSCaliData data = %d\n",*(MMP_USHORT*)addr_out);
	err = MMPF_FS_FWrite(exCaliDSCFileID, (MMP_UBYTE*)addr_out, len, &ulWirteCount);
	printc("ulWirteCount= %d, err= %d\n",ulWirteCount,err);
	MMPF_FS_FClose(exCaliDSCFileID);

}

void writeFSCaliData_a(char* BinFileName, MMP_UBYTE* addr_out, MMP_ULONG len)
{
	MMP_ULONG ulWirteCount;
	MMP_ERR err;
	static MMP_ULONG exCaliDSCFileID;
	if (MMPF_FS_FOpen((MMP_BYTE *)BinFileName, "ab", &exCaliDSCFileID) != MMP_ERR_NONE) 
	{
		printc("MMPF_DSC_GetRawData FAIL: MMPF_FS_FOpen fail ... %s\n",BinFileName);	
	}
	printc("writeFSCaliData data = %d\n",*(MMP_USHORT*)addr_out);
	err = MMPF_FS_FWrite(exCaliDSCFileID, (MMP_UBYTE*)addr_out, len, &ulWirteCount);
	printc("ulWirteCount= %d, err= %d\n",ulWirteCount,err);
	MMPF_FS_FClose(exCaliDSCFileID);

}

MMP_ERR readFSCaliData(char* BinFileName, MMP_UBYTE* addr_out, MMP_ULONG len)
{
	MMP_ULONG ulWirteCount;
	static MMP_ULONG exCaliDSCFileID;
	MMP_ERR err;

	err = MMPF_FS_FOpen((MMP_BYTE *)BinFileName, "rb", &exCaliDSCFileID);
	if ( err != MMP_ERR_NONE) 
	{
			printc("MMPF_DSC_GetRawData FAIL: MMPF_FS_FOpen fail ... %s\n",BinFileName);	
	}
	MMPF_FS_FRead(exCaliDSCFileID, (MMP_UBYTE*)addr_out, len, &ulWirteCount);
	MMPF_FS_FClose(exCaliDSCFileID);

	return err;
}

//return aitraw的(x,y)值
unsigned short GetAITRawPixel(unsigned char* data,int x,int y,int Width)	//x橫的,y直的
{
	int pos,Res,Q,Q_pos;
	unsigned short temp;
	unsigned char tempaddr[4];

	pos = y*Width + x;
	//Q   = pos/3+1;	//在AIT raw中第幾組(4個pixel為一組)
	Q   = pos/3;	//在AIT raw中第幾組(4個pixel為一組)
	Q_pos = 3*Q;		//該組是bayer raw的哪個起點位置
	//Res = pos%3;	//位置在3個bayer piexl中第幾個
	Res = pos-Q_pos;	//位置在3個bayer piexl中第幾個

	memcpy(tempaddr,data+Q*4,4);

	temp = (unsigned short)((tempaddr[3] >> (2*Res))  & 0x0003 ) + ((unsigned short)(tempaddr[Res])<<2) ;

	return temp;
}

//改為不是從中央切開
unsigned int GetAvgFromAITRaw(unsigned char* data, unsigned int Width, unsigned int Height, int StartX, int StartY,
							   int blk_w, int blk_h, double *r_avg,double *gr_avg,double *gb_avg,double *b_avg) 
{
	unsigned int i = 0;
	unsigned int j = 0;
	int sum_1,sum_2,sum_3,sum_4;
	double avg_1,avg_2,avg_3,avg_4;
	int cnt;
	
	sum_1 =0;
	sum_2 =0;
	sum_3 =0;
	sum_4 =0;

	//avg_1
	cnt=0;
	for(i=StartY;i<StartY+blk_h;i+=2){
		for(j=StartX;j<StartX+blk_w;j+=2){
			sum_1+=GetAITRawPixel(data,j,i,Width);
			cnt++;
		}
	}
	avg_1 = (double)sum_1/cnt;

	//avg_2
	cnt=0;
	for(i=StartY;i<StartY+blk_h;i+=2){
		for(j=StartX+1;j<StartX+blk_w;j+=2){
			sum_2+=GetAITRawPixel(data,j,i,Width);
			cnt++;
		}
	}
	avg_2 = (double)sum_2/cnt;

	//avg_3
	cnt=0;
	for(i=StartY+1;i<StartY+blk_h;i+=2){
		for(j=StartX;j<StartX+blk_w;j+=2){
			sum_3+=GetAITRawPixel(data,j,i,Width);
			cnt++;
		}
	}
	avg_3 = (double)sum_3/cnt;

	//avg_4
	cnt=0;
	for(i=StartY+1;i<StartY+blk_h;i+=2){
		for(j=StartX+1;j<StartX+blk_w;j+=2){
			sum_4+=GetAITRawPixel(data,j,i,Width);
			cnt++;
		}
	}
	avg_4 = (double)sum_4/cnt;

	//判斷起始pixel為R,Gr,Gb,B ?
	if(StartX%2==0 && StartY%2==0){	//R
		*r_avg  = avg_1; 
		*gr_avg = avg_2; 
		*gb_avg = avg_3; 
		*b_avg  = avg_4; 
	}else if(StartX%2==1 && StartY%2==0){		//Gr
		*r_avg  = avg_2; 
		*gr_avg = avg_1; 
		*gb_avg = avg_4; 
		*b_avg  = avg_3; 
	}else if(StartX%2==0 && StartY%2==1){		//Gb
		*r_avg  = avg_3; 
		*gr_avg = avg_4; 
		*gb_avg = avg_1; 
		*b_avg  = avg_2; 
	}else if(StartX%2==1 && StartY%2==1){		//B
		*r_avg  = avg_4; 
		*gr_avg = avg_3; 
		*gb_avg = avg_2; 
		*b_avg  = avg_1; 
	}

	return 0;
}	


void PLCali_AE(void)
{
	MMP_ERR     err;
    MMP_UBYTE * tempaddr = 0;
	MMP_UBYTE  i,mode_now;
    MMP_ULONG ulWidth;
    MMP_ULONG ulHeight;
    MMP_USHORT usFileNameLen = 0;
	MMP_BYTE  m_bRawFileName[64];

	// #define AECALI_RUN_MODE_NUM (4)
	// MMP_UBYTE mode_run[AECALI_RUN_MODE_NUM]={0,1,4,2};

	#define AECALI_RUN_MODE_NUM (6)
	MMP_UBYTE mode_run[AECALI_RUN_MODE_NUM]={0,1,4,2,5,6};
	
	//add by casio
	MMP_UBYTE * rawaddr = (MMP_UBYTE*)INF_CALI_DATA_ADDR;
	//MMP_UBYTE * rawaddr = (MMP_UBYTE*)LS_CS_CALI_DATA_ADDR;	//0x3500000
	double  r_avg, gr_avg, gb_avg, b_avg,g_pixel_avg;
	int Height,Width;
	int BytePerLine;
	long size_raw; 
	int tarPos, curPos;
	int min_gain_ratio[AECALI_RUN_MODE_NUM];
	int sat_value;
	AHC_ISP_PARAM * cali_value;
	

	//AE Calibration init setting
	//set Tele
	curPos = ISP_IF_AF_GetZoomTarPos();
	tarPos=0;
	ISP_IF_AF_SetZoomTarPos(tarPos);
	
	ISP_IF_MOTOR_GoToZoomPos();
	if(curPos!=0){
		ISP_IF_MOTOR_GoToZoomPos();
		printc( "Zoom motor move from %d to %d\n", curPos, tarPos);
		MMPF_OS_Sleep(5);
		while(ISP_IF_MOTOR_GetZoomMotorState() != ISP_MOTOR_STATE_STOP)
		{
			printc("zoom motor moving\n");
			MMPF_OS_Sleep(100);
		}			
	}
	
	//set AE manual
	ISP_IF_AE_SetMode(ISP_AE_MANUAL);
	MMPF_OS_Sleep(3000);
	//ISP_IF_AE_SetManualTv(6);
	//ISP_IF_AE_SetAGain(1);
	//ISP_IF_AE_SetDGain(1);
	ISP_IF_AE_SetManualSensorAGain(AE_AGAIN_1X);
	ISP_IF_AE_SetManualSensorDGain(AE_DGAIN_1X);
	ISP_IF_AE_SetManualExpTime(500<<16);
	MMPF_OS_Sleep(3000);
	//end AE Calibration init setting
	
	for(i=0;i<AECALI_RUN_MODE_NUM;i++)
	{
		mode_now = mode_run[i];
		AHC_SetMode(0x00); // switch to idle mode
		ISP_IF_AE_SetAELock(ISP_AE_LOCK_DSB);

		if(mode_now==0)
		{
			AHC_SetMode(0x10);  // DSC preview mode
			MMPF_OS_Sleep(1000);
			ISP_IF_AE_SetAELock(ISP_AE_LOCK_ENB);
			AHC_SetMode(0x00);  // DSC preview mode
		}
		
#if 0	
	    SetRawPreviewMode(MMP_TRUE, mode_now); // 0 for 2568x1932, 1 for 1292x967, 2 for 1928x1087, 3 for 1292x967, 4 for 644x482(120p), 5 for 1282x727(30p), 6 for 1282x727(60p)
#else
	    m_bForcedPreviewModeEn	= MMP_TRUE;
	    m_bForcedPreviewMode	= mode_now;
#endif
	    MMPS_DSC_SetPreviewPath(MMPS_DSC_CS_PREVIEW, MMPS_DSC_COLORDEPTH_10);
	    MMPS_DSC_EnablePreviewDisplay(PRM_SENSOR, MMP_FALSE, MMP_TRUE);

#if 0	
		err = DSC_StartPreview();
#else
        MMPS_Display_SetOutputPanel(MMP_DISPLAY_PRM_CTL, MMP_DISPLAY_SEL_MAIN_LCD);
        #if (CHIP == P_V2)
        MMPS_Display_SetWinPriority(MMP_DISPLAY_WIN_ICON, MMP_DISPLAY_WIN_OVERLAY, MMP_DISPLAY_WIN_MAIN, MMP_DISPLAY_WIN_PIP);
        #endif
        #if (CHIP == MCR_V2)
        MMPS_Display_SetWinPriority(MMP_DISPLAY_WIN_OVERLAY, MMP_DISPLAY_WIN_MAIN, MMP_DISPLAY_WIN_PIP, MMP_DISPLAY_WIN_MAX);
        #endif
        MMPS_DSC_ResetPreviewMode(); 
        MMPS_DSC_SetSystemMode(DSC_NORMAL_PREVIEW_MODE);

        err = MMPS_Sensor_Initialize(MAIN_SENSOR, 1, 1/*MMP_SNR_DSC_PRW_MODE*/);        
        
      	MMPS_Sensor_SetCaptureMode(PRM_SENSOR, 0);
        if (err == MMP_ERR_NONE) {
            err = MMPS_DSC_EnablePreviewDisplay(PRM_SENSOR, MMP_TRUE, MMP_FALSE);
        }
#endif	    

		MMPF_OS_Sleep(2000);
		ISP_IF_AE_SetAELock(ISP_AE_LOCK_ENB);

		sprintc(m_bRawFileName,"SD:\\AeCaliMode%02d.raw",mode_now);
	    #if (FS_INPUT_ENCODE == UCS2)
		usFileNameLen = uniStrlen((const short *)m_bRawFileName);
		#else
		usFileNameLen = STRLEN(m_bRawFileName);
		#endif
	    MMPS_DSC_GetRawData(PRM_SENSOR, tempaddr,MMP_TRUE, m_bRawFileName, usFileNameLen, &ulWidth, &ulHeight); 
		printc("Mode=%d, Raw size is %dx%d\n",mode_now,ulWidth,ulHeight);

		MMPF_OS_Sleep(100);
#if 0	
		DSC_StopPreview();
#else
        MMPS_DSC_EnablePreviewDisplay(PRM_SENSOR, MMP_FALSE, MMP_FALSE);
#endif	
    }
	
	//calc avg
	//cali_value = (AHC_DEFAULT_VALUE*)INF_CALI_DATA_ADDR;
	cali_value = (AHC_ISP_PARAM*)LS_CS_CALI_DATA_ADDR;	//350000
	
	for(i=0;i<AECALI_RUN_MODE_NUM;i++)
	{
		mode_now = mode_run[i];	//mode_run[6]={0,1,4,2,5,6};
		switch(mode_now)
		{
			case 0:
				Width  = 2568;
				Height = 1932;
				break;
			case 1:
				Width  = 1292;
				Height = 967;
				break;
			case 4:
				Width  = 644;
				Height = 481;
				break;
			case 2:
				Width  = 1928;
				Height = 1087;
				break;
			case 5:
				Width  = 1282;
				Height = 727;
				break;
			case 6:
				Width  = 1282;
				Height = 727;
				break;
			default:
				break;
		}
			
		BytePerLine = 2*Width;
		size_raw = BytePerLine*Height*4/6;
		printc("size_raw = %d\n",size_raw);
		
		sprintc(m_bRawFileName,"SD:\\AeCaliMode%02d.raw",mode_now);
		printc("filename = %s\n",m_bRawFileName);
		readFSCaliData(m_bRawFileName, (MMP_UBYTE*)rawaddr, size_raw);	//5M
		
		//sprintc(m_bRawFileName,"SD:\\save%02d.raw",mode_now);
		//writeFSCaliData(m_bRawFileName, (MMP_UBYTE*)rawaddr, size_raw);
		
		
		//GetAITRawPixel( (MMP_UBYTE*)rawaddr,1,1,2568);
		//printc("pixel = %d\n",GetAITRawPixel( (MMP_UBYTE*)rawaddr,1,1,2568));
		GetAvgFromAITRaw((MMP_UBYTE*)rawaddr, Width, Height, Width/2, Height/2, 10, 10, &r_avg, &gr_avg, &gb_avg, &b_avg);	
		//printc("r_avg = %d\tgr_avg = %d\tgb_avg = %d\tb_avg = %d\n", r_avg, gr_avg, gb_avg, b_avg);
		printc("r_avg=%d\tgr_avg=%d\tgb_avg=%d\tb_avg=%d\n",(int)(r_avg),(int)(gr_avg),(int)(gb_avg),(int)(b_avg));
		
		g_pixel_avg = (gr_avg + gb_avg)/2;
		sat_value = 1<<10;		//saturation value 1<<10
		printc("g_pixel_avg = %d\n",(int)g_pixel_avg);
		printc("2^10 = %d\n",1<<10);
		
		min_gain_ratio[i] = (int)( sat_value/g_pixel_avg );
		printc("min_gain_ratio[%d] = %d\n",i,min_gain_ratio[i]);
		
		cali_value->AeCali[i] =(int)( sat_value<<8 )/g_pixel_avg;	//ratio*256	
		printc("cali_value->AeCali[%d] = %d\n",i,cali_value->AeCali[i]);
	}

	for(i=0;i<AECALI_RUN_MODE_NUM;i++)
		printc("1st cali_value->AeCali[%d] = %d\n",i,cali_value->AeCali[i]);
		
	writeFSCaliData("SD:\\AeCali.raw", (MMP_UBYTE*)&(cali_value->AeCali), sizeof(cali_value->AeCali));
	readFSCaliData ("SD:\\AeCali.raw", (MMP_UBYTE*)&(cali_value->AeCali), sizeof(cali_value->AeCali));
	
	for(i=0;i<AECALI_RUN_MODE_NUM;i++)
		printc("cali_value->AeCali[%d] = %d\n",i,cali_value->AeCali[i]);

	// recover AE to auto mode
	ISP_IF_AE_SetMode(ISP_AE_MODE_AUTO);
	ISP_IF_AE_SetManualSensorAGain(0);
	ISP_IF_AE_SetManualSensorDGain(0);
	ISP_IF_AE_SetManualExpTime(0);


}

void PLCali_LSCS_SetFromSD(void)
{
	LSCS_CALI_RESULT * cali_value;
	int i;
	
	// set LS and CS results
	cali_value = (LSCS_CALI_RESULT*)(INF_CALI_DATA_ADDR);
	// read values from calibrated raw file to DRAM
	if(readFSCaliData("SD:\\AitCaliOut.bin", (MMP_UBYTE*)cali_value, sizeof(LSCS_CALI_RESULT)) != MMP_ERR_NONE)
		{
			OSD_Draw_TestMessage(180,100,1,"LSCS done");
			return;
		}

	for(i=0;i<2;i++)
	{
		ISP_IF_CALI_SetLS(	i,
					cali_value->usLumaBaseX,
					cali_value->usLumaBaseY,
					cali_value->usSHADE_CenterX,
					cali_value->usSHADE_CenterY,
					cali_value->usLumaOffsetX,
					cali_value->usLumaOffsetY,
					cali_value->usLumaRateX,
					cali_value->usLumaRateY,
					cali_value->usLensShading_R,
					cali_value->usLensShading_G,
					cali_value->usLensShading_B);
					
		ISP_IF_CALI_SetCS(	i,
					cali_value->usCS_BASE_X,
					cali_value->usCS_BASE_Y,
					cali_value->usCS_CENTER_X,
					cali_value->usCS_CENTER_Y,
					cali_value->usCS_OFFSET_X,
					cali_value->usCS_OFFSET_Y,
					cali_value->usCSScale,
					cali_value->usXSamplePos,
					cali_value->usYSamplePos,
					cali_value->usCS_Table_R,
					cali_value->usCS_Table_G,
					cali_value->usCS_Table_B);

	}
	
	printc("center = (%d,%d),base = (%d,%d) \n",cali_value->usSHADE_CenterX,cali_value->usSHADE_CenterY,cali_value->usLumaBaseX,cali_value->usLumaBaseY);
}

void PLCheck_AE(void)
{
	int  i,j,cnt;

	MMP_ULONG  gr_avg, gb_avg, g_pixel_avg;
	int curPos;
	

	//AE Calibration init setting
	//set Tele
	curPos = ISP_IF_AF_GetZoomTarPos();
	ISP_IF_AF_SetZoomTarPos(0);	
	ISP_IF_MOTOR_GoToZoomPos();
	if(curPos!=0){
		ISP_IF_MOTOR_GoToZoomPos();
		printc( "Zoom motor move to %d\n", curPos);
		MMPF_OS_Sleep(5);
		while(ISP_IF_MOTOR_GetZoomMotorState() != ISP_MOTOR_STATE_STOP)
		{
			printc("zoom motor moving\n");
			MMPF_OS_Sleep(100);
		}			
	}
	
	//set AE manual
	// ISP_IF_AE_SetManualSensorAGain(AE_AGAIN_1X);
	// ISP_IF_AE_SetManualSensorDGain(AE_DGAIN_1X);
	ISP_IF_AE_SetManualExpTime(300<<16);  // 300ms
	MMPF_OS_Sleep(1000);
	//end AE Calibration init setting

	// get AWB ACC G average
	cnt = 0;
	gr_avg = 0;
	gb_avg = 0;
	for(i=4;i<12;i++)
	{
		for(j=4;j<12;j++)
		{
			gr_avg += ISP_IF_AWB_GetAccByWnd(i*16+j,1);
			gb_avg += ISP_IF_AWB_GetAccByWnd(i*16+j,2);
			cnt++;
			printc("(%d,%d) gr:%d, gb:%d\n",i,j,ISP_IF_AWB_GetAccByWnd(i*16+j,1),ISP_IF_AWB_GetAccByWnd(i*16+j,2));
		}
	}
	g_pixel_avg = (gr_avg+gb_avg)/cnt/2;

	if(g_pixel_avg >= 1020)
		printc("g_pixel_avg=%d, AE test passed! ^_^ \n",g_pixel_avg);
	else
		printc("g_pixel_avg=%d, AE test failed! ><!! \n",g_pixel_avg);
	

	// recover AE to auto mode
	ISP_IF_AE_SetMode(ISP_AE_MODE_AUTO);
	ISP_IF_AE_SetManualSensorAGain(0);
	ISP_IF_AE_SetManualSensorDGain(0);
	ISP_IF_AE_SetManualExpTime(0);
}

#define AFINF_SWAP(x,y) {int t; t = x; x = y; y = t;} 
#define AFINF_MAX 10
void FuncAfCali_quickSort(int number[], int left, int right)          //快速排序  
{ 
	if(left < right) 
	{               
		int i = left;       
		int j = right + 1;     
		while(1) 
		{             // 向右找      
		
			while(i + 1 < AFINF_MAX && number[++i] < number[left]) ;

			// 向左找      
			while(j -1 > -1 && number[--j] > number[left]) ;      
			if(i >= j)    
				break;         
		
			AFINF_SWAP(number[i], number[j]);   
		}     
		AFINF_SWAP(number[left], number[j]);   
		FuncAfCali_quickSort(number, left, j-1);   // 對左邊進行遞迴     
		FuncAfCali_quickSort(number, j+1, right);  // 對右邊進行遞迴     	
	} 		
} 

int CalcAverage(int num_ori, int* currentIntPos)
{
	int k=0;
	int out=0;
	FuncAfCali_quickSort(currentIntPos,0,num_ori-1);
	for(k=2;k<num_ori-2;k++)  // skip 2 smallest and 2 largest values
	{
		out += currentIntPos[k];
	}

	return (out/(num_ori-4));
}

#define AFCALI_USE_MOVIE_AF_EN (1)
#define SI_16				signed short
#define AL_FIX_VAL_16(n)	((SI_16)((double)(n)*16.0))
#define INF_ERR_CALI_NUM (11)
#define INF_INI_POS_CALI (100) // inf position at calibration (for big search range)
#if AIT_LENS_TYPE==AIT_LENS_TYPE_ZM726
int IdealInfPos[INF_ERR_CALI_NUM] = {0,-199,-390,-441,-478,-492,-470,-394,-327,-235,-112};
#else // AIT_LENS_TYPE_ZM751
int IdealInfPos[INF_ERR_CALI_NUM] = {0,-179,-371,-431,-482,-517,-523,-487,-448,-390,-311};
#endif
int CaliZoomPos[INF_ERR_CALI_NUM] = {0,  30,  60,  70,  80,  90, 100, 110, 115, 120, 125};

void PLCali_AF_Inf_Pos(void)
{
	int tarPos, curPos,cnt, i, j, sum, sum_mvolt, cnt_mvolt;
	
	#if (AFCALI_USE_MOVIE_AF_EN == 0)
	ISP_AF_OPERATION preOp, tarOp;
	#endif
	
	ISP_AF_STYLE  curStyle;
	ISP_AF_RESULT result;
	// float tmp_avg,tmp_err;
	SI_16 readout[INF_ERR_CALI_NUM*2+2];
	int avg;	
	int currentIntPos[10];
	AHC_ISP_PARAM * cali_value;
	int NowInfPos;
	#if(ADX2003_EN)
	MMP_UBYTE source;
	#endif
	MMP_ULONG level = 0;
	MMP_ERR		status;
	//MMP_USHORT usActiveWin[4];

	StateSwitchMode(UI_VIDEO_STATE);  // DVC preview mode
	MMPF_OS_Sleep(500);
	ISP_IF_3A_SetSwitch(ISP_3A_ALGO_AWB, 0 );
	ISP_IF_3A_SetSwitch(ISP_3A_ALGO_AE, 0 );

	if ((FW_RUN_CALI_MODE_EN) ||(CalibrationMode)) 
		{
			OSD_Draw_TestMessage(100,150,4,"Doing..");//down
		}

	//zoompos拉到0
	curPos = ISP_IF_AF_GetZoomTarPos();
	tarPos=0;
	ISP_IF_AF_SetZoomTarPos(tarPos);

	if(curPos!=0){
		ISP_IF_MOTOR_GoToZoomPos();
		printc( "Zoom motor move from %d to %d\n", curPos, tarPos);
		MMPF_OS_Sleep(5);
		while(ISP_IF_MOTOR_GetZoomMotorState() != ISP_MOTOR_STATE_STOP)
		{
			printc("zoom motor moving\n");
			MMPF_OS_Sleep(100);
		}
			
	}
	
	//af inf pos cali
	ISP_IF_AF_SetINFPos(INF_INI_POS_CALI); // set INF position to 50 first (Let stillaf can do more range)
	
	curStyle = ISP_IF_AF_GetAFStyle();
	#if AFCALI_USE_MOVIE_AF_EN
	ISP_IF_AF_SetAFStyle(ISP_AF_STYLE_MOVIE);
	//ISP_IF_AF_SetZoomTracking( ISP_AF_ZTRACK_ENB ); //Rogers:***
	printc( "Use Movie AF\n");
	MMPF_OS_Sleep(2000);
	#else
	//ISP_IF_AF_SetZoomTracking( ISP_AF_ZTRACK_DSB ); //Rogers:***
	ISP_IF_AF_SetAFStyle(ISP_AF_STYLE_STILL_ONESHOT);
	#endif
	// switch to single window
	// ISP_IF_AF_SetWindowMode(ISP_AF_WNDMODE_ACTSPOT_WND);
	// usActiveWin[0] = ISP_IF_SNR_GetVIFGrab_H_Length()>>2;
	// usActiveWin[2] = ISP_IF_SNR_GetVIFGrab_V_Length()>>2;
	// usActiveWin[1] = (ISP_IF_SNR_GetVIFGrab_H_Length()*3)>>2;
	// usActiveWin[3] = (ISP_IF_SNR_GetVIFGrab_V_Length()*3)>>2;
	// ISP_IF_AF_SetActiveSpotWindow(usActiveWin);


	printc( "AF Style is changed from %d to %d\n", curStyle, ISP_AF_STYLE_STILL_ONESHOT );
	sum=0;
	sum_mvolt = 0; // for 2003 lens temperature
	cnt_mvolt = 0; // for 2003 lens temperature
	
	for(i=0;i<10;i++){
		//stillaf
		//ISP_AF_OPERATION preOp, tarOp;
		// 0x00 : NoRequest, 0x01 : Still Start, 0x02 : Still Cancel
	    //sscanfl( szParam, "%d ",&tarOp);

		#if AFCALI_USE_MOVIE_AF_EN
			MMPF_OS_Sleep(500);
		#else
			preOp = ISP_IF_AF_GetSillAFOperation();
			if( preOp !=  ISP_AF_OPERATION_NOREQ )
			{
				ISP_IF_AF_SetSillAFOperation(ISP_AF_OPERATION_CANCEL);
				MMPF_OS_Sleep(100);
			}
			tarOp = ISP_AF_OPERATION_START;
			ISP_IF_AF_SetSillAFOperation(tarOp);
			MMPF_OS_Sleep(500);
			printc( "Still AF operation change from %d to %d\n", preOp, tarOp);
		#endif
		//AHC_OS_Sleep(1);
		cnt=0;
		while(ISP_IF_AF_GetResult() != ISP_AF_RESULT_FOCUSED)
		{
			MMPF_OS_Sleep(10);
			result = ISP_IF_AF_GetResult();
			//printc( "AF result is %d\n", result );
			cnt+=1;
			if(cnt==500){
				printc( "stillaf time too long!!\n", result );
				break;
			}
		}
		
		if(result==ISP_AF_RESULT_FOCUSED){
			printc( "AF focused !!\n");
		}else
			printc( "AF not OK, result is %d\n", result );
		
		currentIntPos[i] = ISP_IF_AF_GetCurr2INFPos();

		#if(ADX2003_EN)
			status = ADX2003_ADC_Measure_Lens_Temp(&level, &source);
			printc("level(voltage) : %d mVolt, status=%d\n",level,status);
		#else
			status = MMP_SYSTEM_ERR_CMDTIMEOUT;
		#endif
		if(status == MMP_ERR_NONE)
		{
			cnt_mvolt++;
			sum_mvolt += (int)level;
		}
		
		printc( "Current AF Curr2Inf Position is %d\n", currentIntPos[i] );
		sum+=currentIntPos[i];
	}
	printc( "sum = %d\n",sum );
	avg = CalcAverage(10,currentIntPos);

	#if 0
	if (sum>=0)
		avg=(sum+5)/10;
	else
		avg=(sum-5)/10;
	#endif
	printc( "avg = %d\n",avg );

	if(cnt_mvolt!=0)
		sum_mvolt = sum_mvolt / cnt_mvolt;
		
	
	//write to raw	
	cali_value = (AHC_ISP_PARAM*)INF_CALI_DATA_ADDR;
	//cali_value->INFPos = 83 - avg;
	NowInfPos = ISP_IF_AF_GetINFPos();
	cali_value->INFPos[0] = (signed short)(NowInfPos - avg);
	cali_value->INFPos[1] = (signed short)sum_mvolt;
	
	printc( "change avg = %d,len =%d\n" ,(cali_value->INFPos[0]), sizeof(cali_value->INFPos));
	// writeFSCaliData("SD:\\InfPosErr.raw", (MMP_UBYTE*)&(cali_value->INFPos), sizeof(cali_value->INFPos));
	// readFSCaliData("SD:\\InfPosErr.raw", (MMP_UBYTE*)&(cali_value->INFPos), sizeof(cali_value->INFPos));
	// test = cali_value->INFPos;
	// printc( "test = %d,len =%d\n" ,test, sizeof(test));


	// ISP_IF_AF_SetINFPos(cali_value->INFPos[0]);	//設定InfPos
	ISP_IF_AF_SetINFPos(INF_INI_POS_CALI);
	
	
	//af inf err pos cali====
	for(j=0;j<INF_ERR_CALI_NUM;j++){
		//zoompos
	    //sscanfl( szParam, "%d ",&tarPos);
	    tarPos = CaliZoomPos[j];

		curPos = ISP_IF_AF_GetZoomTarPos();

		ISP_IF_AF_SetZoomTarPos(tarPos);

		ISP_IF_MOTOR_GoToZoomPos();

		printc( "Zoom motor move from %d to %d\n", curPos, tarPos);
		MMPF_OS_Sleep(5);
		while(ISP_IF_MOTOR_GetZoomMotorState() != ISP_MOTOR_STATE_STOP)
		{
			MMPF_OS_Sleep(100);
		}

		#if AFCALI_USE_MOVIE_AF_EN

		#else
			//stillaf
			// 0x00 : NoRequest, 0x01 : Still Start, 0x02 : Still Cancel
		    //sscanfl( szParam, "%d ",&tarOp);
		    curStyle = ISP_IF_AF_GetAFStyle();
			ISP_IF_AF_SetAFStyle(ISP_AF_STYLE_STILL_ONESHOT);
			printc( "AF Style is changed from %d to %d\n", curStyle, ISP_AF_STYLE_STILL_ONESHOT );
	    #endif
	    sum = 0;
		for(i=0;i<6;i++){
			//stillaf
			//ISP_AF_OPERATION preOp, tarOp;
			// 0x00 : NoRequest, 0x01 : Still Start, 0x02 : Still Cancel
		    //sscanfl( szParam, "%d ",&tarOp);
			#if AFCALI_USE_MOVIE_AF_EN
				MMPF_OS_Sleep(500);
			#else
			    MMPF_OS_Sleep(100);
				preOp = ISP_IF_AF_GetSillAFOperation();

				if( preOp !=  ISP_AF_OPERATION_NOREQ )
				{
					ISP_IF_AF_SetSillAFOperation(ISP_AF_OPERATION_CANCEL);
					MMPF_OS_Sleep(100);
				}
				tarOp = ISP_AF_OPERATION_START;
				ISP_IF_AF_SetSillAFOperation(tarOp);

				MMPF_OS_Sleep(500);
				printc( "Still AF operation change from %d to %d\n", preOp, tarOp);
			#endif
			//AHC_OS_Sleep(1);
			cnt=0;
			while(ISP_IF_AF_GetResult() != ISP_AF_RESULT_FOCUSED)
			{
				MMPF_OS_Sleep(10);
				result = ISP_IF_AF_GetResult();
				//printc( "AF result is %d\n", result );
				cnt+=1;
				if(cnt==500){
					printc( "stillaf time too long!!\n", result );
					break;
				}
			}
			
			if(result==ISP_AF_RESULT_FOCUSED){
				printc( "AF focused !!\n");
			}else
				printc( "AF not OK, result is %d\n", result );
						
			//getafcurr2infpos
			currentIntPos[i] = ISP_IF_AF_GetCurr2INFPos() + (cali_value->INFPos[0] - INF_INI_POS_CALI );
			currentIntPos[i] *= 16;
			// sum+= currentIntPos[i];
			printc("GetCurr2INFPos=%d\n",ISP_IF_AF_GetCurr2INFPos());

		}	//for(i=0;i<5;i++){
		//tmp_avg = (float)sum/5;

		avg = CalcAverage(6,currentIntPos);

		// avg = sum*16/5;

		// save it into cali_value
		cali_value->INFPos[2+j] = CaliZoomPos[j];
		cali_value->INFPos[2+INF_ERR_CALI_NUM + j] = avg - IdealInfPos[j]*16;	//SI_16
		printc( "sum = %d\tavg=%d\terr[%d] = %d\n",sum,avg,j,cali_value->INFPos[2+INF_ERR_CALI_NUM + j]);
		
		//tmp_err = tmp_avg - IdealInfPos[j];		
		//printc( "sum = %d\ttmp_avg=%d\ttmp_err = %d\n",sum,tmp_avg,tmp_err);
		//err[j] = AL_FIX_VAL_16(temp_err);
	}	//for(j=0;j<11;j++){
	
	//InfPos.raw
	writeFSCaliData("SD:\\Lens1.raw", (MMP_UBYTE*)(cali_value->INFPos), sizeof(cali_value->INFPos));
	readFSCaliData("SD:\\Lens1.raw", (MMP_UBYTE*)readout, sizeof(readout));

	i=0;
	printc("InfPos[%d] = (%d,%d)\n",i,readout[i],readout[i+1]);
	for(i=0;i<INF_ERR_CALI_NUM;i++){
		printc("InfErr[%d] = (%d,%d) (%d,%d)\n",i,readout[i+2],readout[i+2+INF_ERR_CALI_NUM],cali_value->INFPos[i+2],cali_value->INFPos[i+2+INF_ERR_CALI_NUM]);
	}

	ISP_IF_AF_SetINFPos(cali_value->INFPos[0]);	//設定InfPos
	//ISP_IF_AF_SetZoomTracking( ISP_AF_ZTRACK_ENB ); //Rogers:***

	if ((FW_RUN_CALI_MODE_EN) ||(CalibrationMode)) 
		{
		OSD_Draw_TestMessage(100,150,0,"Done");//down
		}
	/*
	sum=0;
	for(i=0;i<5;i++){
		//printc( "GetPos %d = %d\n", i, currentPos[i]);
		printc( "GetCurr2INFPos %d = %d\n", i, testPos[i]);
		sum+=testPos[i];
	}
	printc( "sum = %d\n",sum);
	avg=(float)sum/5;
	printc( "zoomPos %d err avg = %d\n",tarPos ,(int)(avg*1000));*/
}

void PLCali_LSCS(void)
{
	CaptureRaw_AfterCS(3,"SD:\\LsCsCali.raw");
}
void PLCali_LSCS_Raw(void)
{
	CaptureRaw_AfterCS(0,"SD:\\LsCsCali.raw");
}

extern AHC_ISP_PARAM 	*gISPsettings;

void PLCali_CheckCaliBin(char* szParam)
{
	#define TMP_WORK_CALI_DATA_ADDR 0x3000000

	ISP_UINT32 i;
		
	// read calibration data from SD card
	AHC_ISP_PARAM *readback = (AHC_ISP_PARAM *)TMP_WORK_CALI_DATA_ADDR;
	CALI_RESULT *caliResult;


	if(readFSCaliData(szParam, (MMP_UBYTE*)readback, sizeof(AHC_ISP_PARAM)) != MMP_ERR_NONE)
		return;

	// to get version number first
	// use different version to get check sum length 
	printc("Calibration data version: 0x%04x \n",readback->Version);

	caliResult = (CALI_RESULT *)readback;

			// new format after Version=2
	if (readback->Version >= 0x0002)
	{	
		ISP_IF_AF_SetINFPosAtCali(caliResult->INFPos);
		ISP_IF_AF_SetLensTempMiniVoltAtCali(caliResult->LensMiniVolt);
		ISP_IF_AF_SetINFErrAtCali(caliResult->InfErrZmPos,caliResult->InfErrValue,11);	

		printc("InfPos:%d, LensVolt:%d \n",caliResult->INFPos,caliResult->LensMiniVolt);
		for(i=0;i<11;i++)
		{
			printc("Zps:%d, Err:%d /16 =%d\n",caliResult->InfErrZmPos[i],caliResult->InfErrValue[i],caliResult->InfErrValue[i]>>4);
		}
	}
	// new format after Version=3
	if (readback->Version >= 0x0003)
	{	
		ISP_IF_AE_SetCalibrationData(caliResult->AeCali,6);		
		for(i=0;i<6;i++)
			printc("AeCali[%d]=%d/256 \n",i,caliResult->AeCali[i]);
	}
	// new format after Version=4
	if (readback->Version >= 0x0004)
	{
		ISP_IF_AWB_CaliUpdateCP1Data(caliResult->AWBCP1FwdMatrix,caliResult->AWBCP1InvMatrix);
		for(i=0;i<9;i++)
			printc("WBCP1Fwd[%d]=%d, WBCP1Inv[%d]=%d \n",i,caliResult->AWBCP1FwdMatrix[i],i,caliResult->AWBCP1InvMatrix[i]);
	}

	if (readback->Version >= 0x0006)
	{
		ISP_IF_AF_SetStillafAddiShift(caliResult->StillAfAddiShift);
		printc("Stillaf Addi Shift = %d %d \n",caliResult->StillAfAddiShift,ISP_IF_AF_GetStillafAddiShift());
	}
}

void PLCali_ReadCaliBinToFlash(int IsWriteToFlash)
{
	AHC_ISP_PARAM * cali_value;
	MMP_ULONG checksum,i;
	MMP_USHORT cali_ver = CALI_DATA_VERSION;
	MMP_ULONG checkSumLen;
	UINT16 ulMajorVersion;
    UINT16 ulMediumVersion;
    UINT16 ulMinorVersion;
    UINT16 ulBranchVersion;
    UINT16 ulTestVersion;
    char*  szReleaseName = 0;


	// begin to init
	AHC_FormatStorageMedia(5);
	AHC_PARAM_ISP_Init();

	// set calibration data version now
	AHC_SetISPParam(AHC_ISP_VERSION, (MMP_UBYTE*)&(cali_ver));
	AHC_GetISPParam(AHC_ISP_VERSION,&i);
	printc("Current cali ver = 0x%04x \n",i);
	
	// set turkey Fw version
	AHC_PrintFwVersion();
	AHC_GetFwVersion( &ulMajorVersion, &ulMediumVersion, &ulMinorVersion, &ulBranchVersion, &ulTestVersion, &szReleaseName );

	AHC_SetISPParam(AHC_ISP_TKEYFW_MAJOR_VERSION, (MMP_UBYTE*)&(ulMajorVersion));
	AHC_GetISPParam(AHC_ISP_TKEYFW_MAJOR_VERSION,&checksum);
	printc("cali_value->TurnkeyFwMajorVersion = %d, read out ulMajorVersion=%d \n\n",ulMajorVersion ,checksum);
	AHC_SetISPParam(AHC_ISP_TKEYFW_MEDIUM_VERSION, (MMP_UBYTE*)&(ulMediumVersion));
	AHC_GetISPParam(AHC_ISP_TKEYFW_MEDIUM_VERSION,&checksum);
	printc("cali_value->TurnkeyFwMediumVersion = %d, read out ulMediumVersion=%d \n\n",ulMediumVersion ,checksum);
	AHC_SetISPParam(AHC_ISP_TKEYFW_MINOR_VERSION, (MMP_UBYTE*)&(ulMinorVersion));
	AHC_GetISPParam(AHC_ISP_TKEYFW_MINOR_VERSION,&checksum);
	printc("cali_value->TurnkeyFwMinorVersion = %d, read out ulMinorVersion=%d \n\n",ulMinorVersion ,checksum);

	// set LS and CS results
	cali_value = (AHC_ISP_PARAM*)LS_CS_CALI_DATA_ADDR;
	// read values from calibrated raw file to DRAM
	if(readFSCaliData("SD:\\AitCaliOut.bin", (MMP_UBYTE*)cali_value->LsCsData, sizeof(cali_value->LsCsData)) != MMP_ERR_NONE)
		{
		OSD_Draw_TestMessage(10,120,1,"ISP gen");
		return;
		}
	else
		OSD_Draw_TestMessage(10,120,0,"ISP gen");
	// copy DRAM data to gISPsettings
	AHC_SetISPParam(AHC_ISP_LSCS, (MMP_UBYTE*)cali_value->LsCsData);
	
	// set AF motor inf position results, InfPos.raw
	//cali_value->INFPos = 90;
	if(readFSCaliData("SD:\\Lens1.raw", (MMP_UBYTE*)cali_value->INFPos, sizeof(cali_value->INFPos)) != MMP_ERR_NONE)
		return;
	AHC_SetISPParam(AHC_ISP_INFIPOS, (MMP_UBYTE*)(cali_value->INFPos));
	printc("cali_value->INFPos = %d\n",cali_value->INFPos[0]);

	// no ae calibration now, give default values
	// if(readFSCaliData("SD:\\AeCali.raw", (MMP_UBYTE*)cali_value->AeCali, sizeof(cali_value->AeCali)) != MMP_ERR_NONE)
	// 	return;
	for(i=0;i<6;i++)
	{
		cali_value->AeCali[i] = 1<<8;
		printc("cali_value->AeCali[%d] = %d\n\n",i,cali_value->AeCali[i]);	
	}
	AHC_SetISPParam(AHC_ISP_AECALI, (MMP_UBYTE*)(cali_value->AeCali));
	AHC_SetISPParam(AHC_ISP_AECALI, (MMP_UBYTE*)(cali_value->AeCali));
	for(i=0;i<6;i++)
		printc("cali_value->AeCali[%d] = %d\n",i,cali_value->AeCali[i]);	
	
	//=================
	// set AWB results
	if(readFSCaliData("SD:\\AwbCaliOut.raw", (MMP_UBYTE*)cali_value->AwbData, sizeof(cali_value->AwbData)) != MMP_ERR_NONE)
	{
		// use default value
		for(i=0;i<18;i++)
		{
			cali_value->AwbData[i] = 0;
		}
		cali_value->AwbData[0] = 1<<16;
		cali_value->AwbData[4] = 1<<16;
		cali_value->AwbData[8] = 1<<16;
		cali_value->AwbData[9] = 1<<16;
		cali_value->AwbData[13] = 1<<16;
		cali_value->AwbData[17] = 1<<16;
		//	return;
	}
	
	AHC_SetISPParam(AHC_ISP_AWB, (MMP_UBYTE*)(cali_value->AwbData));
	for(i=0;i<18;i++)
		printc("cali_value->AwbData[%d] = %d\n",i,cali_value->AwbData[i]);	
	printc("cali_value->AwbData Done\n");

	//=================
	// set Focus motor additional shift results, FocusMotorShift.raw
	if(readFSCaliData("SD:\\Lens2.raw", (MMP_UBYTE*)&(cali_value->FocusMotorAddiShift), sizeof(cali_value->FocusMotorAddiShift)) != MMP_ERR_NONE)
	{
		cali_value->FocusMotorAddiShift = 0;
	}
	AHC_SetISPParam(AHC_ISP_FOMOTOR_SHIFT, (MMP_UBYTE*)&(cali_value->FocusMotorAddiShift));
	printc("cali_value->FocusMotorAddiShift = %d\n",cali_value->FocusMotorAddiShift);	
	

	// calculate checksum
	checkSumLen = (sizeof(AHC_ISP_PARAM)-4) / 4;
	checksum=0;
	for (i = 0; i < checkSumLen; i++)
	 	checksum += ((MMP_ULONG*)gISPsettings)[i+1];
	AHC_SetISPParam(AHC_ISP_CHECKSUM, (UINT8*)&checksum);

	
	printc("checkSumLen = %d, checksum=%d \n",checkSumLen,checksum);
	AHC_GetISPParam(AHC_ISP_VERSION,&i);
	printc("Current cali ver = 0x%04x \n",i);

	//checksum = 0;
	//AHC_GetDefaultParam(AHC_DEFAULT_CHECKSUM,&checksum);
	// printc("read out checksum=%d \n",checksum);

	if(IsWriteToFlash==1)
	{
		// write data to Flash
		AHC_PARAM_ISP_Write(0); //0:Factory 1:User
	}
	else
	{
		// write to SD card 
		DramToFile("SD:\\ISP.bin", (INT8*)gISPsettings,sizeof(AHC_ISP_PARAM));
	}
	
	//write version number to bin
	//AHC_PrintFwVersion();
	//AHC_GetFwVersion( &ulMajorVersion, &ulMediumVersion, &ulMinorVersion, &ulBranchVersion, &szReleaseName );
	//cali_value->TurnkeyFwMajorVersion = ulMajorVersion;
	//cali_value->TurnkeyFwMediumVersion = ulMediumVersion;
	//cali_value->TurnkeyFwMinorVersion = ulMinorVersion;
	//writeFSCaliData_a("SD:\\AitCaliOut.bin", (MMP_UBYTE*)&(cali_value->TurnkeyFwMajorVersion), sizeof(cali_value->TurnkeyFwMajorVersion));
	//writeFSCaliData_a("SD:\\AitCaliOut.bin", (MMP_UBYTE*)&(cali_value->TurnkeyFwMediumVersion), sizeof(cali_value->TurnkeyFwMediumVersion));
	//writeFSCaliData_a("SD:\\AitCaliOut.bin", (MMP_UBYTE*)&(cali_value->TurnkeyFwMinorVersion), sizeof(cali_value->TurnkeyFwMinorVersion));
}

void PLCali_ResetCaliBinToFlash(void)
{
	MMP_ULONG checksum;
	MMP_ULONG i;

	// begin to init
	AHC_FormatStorageMedia(5);
	AHC_PARAM_ISP_Init();
	// calculate checksum
	checksum=0;
	for (i = 0; i < (sizeof(*gISPsettings)-4) / 4; i++)
	 	checksum += ((MMP_ULONG*)gISPsettings)[i+1];

	// add some value to let checksum fail
	checksum += 100;
	AHC_SetISPParam(AHC_ISP_CHECKSUM, (UINT8*) &checksum);

	// write data to Flash
	AHC_PARAM_ISP_Write(0); //0:Factory 1:User
}

void PLCali_AF_GetStillafPos(int stillcnt)
{
	ISP_AF_STYLE curStyle;
	ISP_AF_OPERATION preOp, tarOp;
	ISP_AF_RESULT result;
	int tarPos, curSpdMode, currentPos, i, cnt;
	// int stillcnt;
	
	
	curStyle = ISP_IF_AF_GetAFStyle();
	ISP_IF_AF_SetAFStyle(ISP_AF_STYLE_STILL_ONESHOT);
	// sscanfl( szParam, "%d,",&stillcnt);

	printc( "AF Style is changed from %d to %d\n", curStyle, ISP_AF_STYLE_STILL_ONESHOT );
	
	for(i=0;i<stillcnt;i++){
		//if ( kbhit() )
		//{
		//	break;
		//}

		preOp = ISP_IF_AF_GetSillAFOperation();

		if( preOp ==  ISP_AF_OPERATION_NOREQ )
		{
			tarOp = ISP_AF_OPERATION_START;
		}
		else
		{
			tarOp = ISP_AF_OPERATION_CANCEL;
		}

		ISP_IF_AF_SetSillAFOperation(tarOp);
		MMPF_OS_Sleep(100);
		
		cnt=0;
		while(ISP_IF_AF_GetResult() != ISP_AF_RESULT_FOCUSED)
		{
			MMPF_OS_Sleep(10);
			result = ISP_IF_AF_GetResult();
			//printc( "AF result is %d\n", result );
			cnt+=1;
			//printc( "AF resul in whileloop = %d,cnt= %d\n",result,cnt);
			if(cnt==500){
				printc( "stillaf time too long!!\n", result );
				break;
			}
		}
		
		if(result==ISP_AF_RESULT_FOCUSED){
			printc( "AF focused !!\n");
		}else
			printc( "AF not OK, result is %d\n", result );

		MMPF_OS_Sleep(10);		
		//MMPF_OS_Sleep(100);
		
		//print afpos
    	//sscanfl( szParam, "%d ",&tarPos);
    	tarPos = 0;

		currentPos = ISP_IF_AF_GetPos();
		curSpdMode = ISP_IF_AF_GetFocusMotorSpeedMode();

		ISP_IF_AF_SetPos(tarPos);
		ISP_IF_MOTOR_GoToAFPos();

		//for(filter=0; filter<3; ++filter)
		//{
		//	acc[filter] = ISP_IF_AF_GetAccByWnd(wnd, filter);
		//}
		//printc("AF ACC Wnd[%d] (High,Low,Lumi) = (%7d,%9d,%9d)\n", wnd, acc[0], acc[1], acc[2]);
		printc( "AF motor moves from %d to %d with speed mode %d, number= %d\n", currentPos, tarPos,curSpdMode,i);
		MMPF_OS_Sleep(100);
	}
}

void PLCali_AWB_GetRgbAve(int mode)
{
	ISP_INT32 pRgb[3];

	ISP_IF_AWB_CaliLineAdjGetRgbAverage(pRgb);
	if(mode == 0)
		DramToFile("SD:\\CaliAwbMeasRGB0.bin",(INT8*)pRgb,sizeof(ISP_INT32)*3);
	else
		DramToFile("SD:\\CaliAwbMeasRGB1.bin",(INT8*)pRgb,sizeof(ISP_INT32)*3);

	printc("AWB cali mode:%d (R,G,B)=(%d,%d,%d) \n",mode,pRgb[0],pRgb[1],pRgb[2]);


}

#define AL_FIX_VAL(n)	((ISP_INT32)((double)(n)*65536.0))
void PLCali_AWB_CalcData(void)
{
	ISP_INT32 pRefCamXY[4];
	ISP_INT32 pRefPboxXY[4];
	ISP_INT32 pLinePboxXY[4];
	ISP_INT32 asqUpperXy[4] = { AL_FIX_VAL(0.55), AL_FIX_VAL(0.55), AL_FIX_VAL(0.55), AL_FIX_VAL(0.55)};
	ISP_INT32 asqLowerXy[4] = { AL_FIX_VAL(0.25), AL_FIX_VAL(0.25), AL_FIX_VAL(0.25), AL_FIX_VAL(0.25)};
	ISP_INT32 pCp1LoRGB[3];
	ISP_INT32 pCp1HiRGB[3];
	ISP_INT32 pCP1Matrix[18];

	ISP_INT32 *pCP1FwdMatrix = &(pCP1Matrix[0]);
	ISP_INT32 *pCP1InvMatrix = &(pCP1Matrix[9]);
	ISP_UINT8 auiAWBResult;


	
	readFSCaliData("SD:\\CaliAwbRefCamXY0.bin", (MMP_UBYTE*)&(pRefCamXY[0]), sizeof(ISP_INT32)*2);
	readFSCaliData("SD:\\CaliAwbRefCamXY1.bin", (MMP_UBYTE*)&(pRefCamXY[2]), sizeof(ISP_INT32)*2);
	readFSCaliData("SD:\\CaliAwbRefPboxXY0.bin", (MMP_UBYTE*)&(pRefPboxXY[0]), sizeof(ISP_INT32)*2);
	readFSCaliData("SD:\\CaliAwbRefPboxXY1.bin", (MMP_UBYTE*)&(pRefPboxXY[2]), sizeof(ISP_INT32)*2);
	readFSCaliData("SD:\\CaliAwbLinePboxXY0.bin", (MMP_UBYTE*)&(pLinePboxXY[0]), sizeof(ISP_INT32)*2);
	readFSCaliData("SD:\\CaliAwbLinePboxXY1.bin", (MMP_UBYTE*)&(pLinePboxXY[2]), sizeof(ISP_INT32)*2);

	readFSCaliData("SD:\\CaliAwbMeasRGB0.bin", (MMP_UBYTE*)&(pCp1LoRGB[0]), sizeof(ISP_INT32)*3);
	readFSCaliData("SD:\\CaliAwbMeasRGB1.bin", (MMP_UBYTE*)&(pCp1HiRGB[0]), sizeof(ISP_INT32)*3);


	auiAWBResult = ISP_IF_AWB_CaliLineAdjCP1Calc(	pRefCamXY,
									pRefPboxXY,
									pLinePboxXY,
									asqUpperXy,
									asqLowerXy,
									pCp1LoRGB,
									pCp1HiRGB,
									pCP1FwdMatrix,
									pCP1InvMatrix );

	printc("auiAWBResult = %d \n",auiAWBResult);
	DramToFile("SD:\\AwbCaliOut.raw",(INT8*)pCP1Matrix,sizeof(ISP_INT32)*18);
	if(auiAWBResult)
		OSD_Draw_TestMessage(10,100,0,"AWB0 ");
	else
		OSD_Draw_TestMessage(10,100,1,"AWB0 ");

}

void EnableRawPath(unsigned char ubType)
{

	MMP_ERR     err;
	AHC_SetMode(0x00);  // DSC preview mode

    // m_bRawPreviewMode = MMP_TRUE;
	// SetRawPreviewMode(MMP_TRUE);

	// ready to capture RAW
	//SetRawPreviewMode(MMP_TRUE, ubType); // 0 for 2568x1932, 1 for 1292x967, 2 for 1928x1087, 3 for 644x481(30p), 4 for 644x482(120p)
	m_bForcedPreviewModeEn	= MMP_TRUE;
	m_bForcedPreviewMode	= ubType;

    MMPS_DSC_SetPreviewPath(MMPS_DSC_CS_PREVIEW, MMPS_DSC_COLORDEPTH_10);
    MMPS_DSC_EnablePreviewDisplay(PRM_SENSOR, MMP_FALSE, MMP_TRUE);
    
    //err = DSC_StartPreview();
    MMPS_Display_SetOutputPanel(MMP_DISPLAY_PRM_CTL, m_displayPanel);
    #if (CHIP == P_V2)
    MMPS_Display_SetWinPriority(MMP_DISPLAY_WIN_ICON, MMP_DISPLAY_WIN_OVERLAY, MMP_DISPLAY_WIN_MAIN, MMP_DISPLAY_WIN_PIP);
    #endif
    #if (CHIP == MCR_V2)
    MMPS_Display_SetWinPriority(MMP_DISPLAY_WIN_OVERLAY, MMP_DISPLAY_WIN_MAIN, MMP_DISPLAY_WIN_PIP, MMP_DISPLAY_WIN_MAX);
    #endif
    MMPS_DSC_ResetPreviewMode(); 
    MMPS_DSC_SetSystemMode(m_usPreviewMode);

    if(m_bForcedPreviewModeEn == MMP_FALSE){
	    if (m_bRawPreviewMode == MMP_FALSE) {
			err = MMPS_Sensor_Initialize(m_usCurrentSensor, 1, MMP_SNR_DSC_PRW_MODE);        
	    }else {
			err = MMPS_Sensor_Initialize(m_usCurrentSensor, 0, MMP_SNR_DSC_PRW_MODE);   
	    }
    }else{
    	err = MMPS_Sensor_Initialize(m_usCurrentSensor, m_bForcedPreviewMode, MMP_SNR_DSC_PRW_MODE);   
    
    }

  	MMPS_Sensor_SetCaptureMode(PRM_SENSOR, 0);
    if (err == MMP_ERR_NONE) {
        err = MMPS_DSC_EnablePreviewDisplay(PRM_SENSOR, MMP_TRUE, MMP_FALSE);
    }
}

void EnableCaptureRawPath(char  *m_bLsCsRawFileName)
{
    MMP_UBYTE * tempaddr = 0;
    MMP_USHORT usFileNameLen = 0;
    MMP_ULONG ulWidth;
    MMP_ULONG ulHeight;
    #if (FS_INPUT_ENCODE == UCS2)
	usFileNameLen = uniStrlen((const short *)m_bLsCsRawFileName);
	#else
	usFileNameLen = STRLEN(m_bLsCsRawFileName);
	#endif
    MMPS_DSC_GetRawData(PRM_SENSOR, tempaddr,MMP_TRUE, m_bLsCsRawFileName, usFileNameLen, &ulWidth, &ulHeight); 
	printc("Raw size is %dx%d\n",ulWidth,ulHeight);
}

//ubType: 0 for 2568x1932, 1 for 1292x967, 2 for 1928x1087, 3 for 644x481(30p), 4 for 644x482(120p)
void CaptureRaw(unsigned char ubType, char  *m_bLsCsRawFileName)	//capture raw after off shading
{
	MMP_UBYTE bcg_en_ori,ls_en_ori,cs_en_ori;
	ISP_AE_EV_BIAS evbias_ori;
	// MMP_BYTE  *m_bLsCsRawFileName 	= "SD:\\LsCsCali.raw";

	// get original module enable signal 
	bcg_en_ori = ISP_IF_IQ_GetSwitch(ISP_IQ_MODULE_BCG);
	ls_en_ori = ISP_IF_IQ_GetSwitch(ISP_IQ_MODULE_LS);
	cs_en_ori = ISP_IF_IQ_GetSwitch(ISP_IQ_MODULE_CS);
	evbias_ori = ISP_IF_AE_GetEVBias();

	AHC_SetMode(0x00); // switch to idle mode
	ISP_IF_AE_SetAELock(ISP_AE_LOCK_DSB);

	// turn on preview mode to train AE
	AHC_SetMode(0x10);  // DSC preview mode
	ISP_IF_IQ_SetSwitch(ISP_IQ_MODULE_BCG,MMP_FALSE);
	ISP_IF_IQ_SetSwitch(ISP_IQ_MODULE_LS,MMP_FALSE);
	ISP_IF_IQ_SetSwitch(ISP_IQ_MODULE_CS,MMP_FALSE);

	ISP_IF_3A_SetSwitch(ISP_3A_ALGO_AWB, 0 );
	ISP_IF_3A_SetSwitch(ISP_3A_ALGO_AF, 0 );
	ISP_IF_AE_SetMetering(ISP_AE_METERING_SPOT);
	ISP_IF_AE_SetEVBias(ISP_AE_EV_BIAS_P200);	
	MMPF_OS_Sleep(1000);

	// ISP_IF_AE_SetAELock(ISP_AE_LOCK_ENB);
	EnableRawPath(ubType);
	
	ISP_IF_IQ_SetSwitch(ISP_IQ_MODULE_BCG,MMP_FALSE);
	ISP_IF_IQ_SetSwitch(ISP_IQ_MODULE_LS,MMP_FALSE);
	ISP_IF_IQ_SetSwitch(ISP_IQ_MODULE_CS,MMP_FALSE);
	// printc("ori AEG=%d \n",ISP_IF_IQ_GetSwitch(ISP_IQ_MODULE_AEG));

	ISP_IF_3A_SetSwitch(ISP_3A_ALGO_AWB, 0 );
	ISP_IF_3A_SetSwitch(ISP_3A_ALGO_AF, 0 );
	ISP_IF_AE_SetMetering(ISP_AE_METERING_SPOT);
	//ISP_IF_SNR_SetDigitalGain(ISP_IF_AE_GetSensorDigitalGain() *4/3);
	MMPF_OS_Sleep(1000);

	EnableCaptureRawPath(m_bLsCsRawFileName);

	// set back to original value
	//ISP_IF_IQ_SetSwitch(ISP_IQ_MODULE_BCG,bcg_en_ori);
	//ISP_IF_IQ_SetSwitch(ISP_IQ_MODULE_LS,ls_en_ori);
	//ISP_IF_IQ_SetSwitch(ISP_IQ_MODULE_CS,cs_en_ori);
	//ISP_IF_3A_SetSwitch(ISP_3A_ALGO_AWB, 1 );
	//ISP_IF_3A_SetSwitch(ISP_3A_ALGO_AF, 1 );
	//ISP_IF_AE_SetEVBias(evbias_ori);
}

//ubType: 0 for 2568x1932, 1 for 1292x967, 2 for 1928x1087, 3 for 644x481(30p), 4 for 644x482(120p)
void CaptureRaw_LsCsOn(unsigned char ubType, char  *m_bLsCsRawFileBaseName,int IsTo16Bits)	//capture raw after off shading
{

	MMP_UBYTE * rawaddr = (MMP_UBYTE*)0x3600000;
	MMP_SHORT * raw16addr = (MMP_SHORT*)0x3000000;
	MMP_SHORT * WarmPixData =(MMP_SHORT*)0x2A00000;

	int BytePerLine,size_raw,i,j,k=0;
	MMP_BYTE  m_bRawFileName[64];
	int Height,Width;
	ISP_UINT32 shutter,again,isp_dgain,snr_dgain,exptime,warm_threshold;
	char m_bLsCsRawFileName[128];
	static MMP_ULONG exFileID;
	MMP_ERR err;

	// check filename
	for(i=0;i<10000;i++)
	{
		sprintf(m_bLsCsRawFileName,"%s_%04d.raw",m_bLsCsRawFileBaseName,i);
		err = MMPF_FS_FOpen((MMP_BYTE *)m_bLsCsRawFileName, "rb", &exFileID);
		MMPF_FS_FClose(exFileID);
		if(err != MMP_ERR_NONE)
		{
			// no this file, use this name
			break;
		}
	}
	

	// turn on preview mode to train AE
	AHC_SetMode(0x10);  // DSC preview mode
	MMPF_OS_Sleep(1000);
	
	shutter   = ISP_IF_AE_GetShutter();
	again     = ISP_IF_AE_GetAGain();
	isp_dgain = ISP_IF_AE_GetDGain();
	snr_dgain = ISP_IF_AE_GetSensorDigitalGain();
	exptime   = ISP_IF_AE_GetExpTime();
	printc( "Get Shutter = %d\tAGain= %d\tISP_DGain = %d\tExptime = %d\tSensor_Dgain = %d\n", shutter,again,isp_dgain,exptime,snr_dgain );

	EnableRawPath(ubType);

	ISP_IF_3A_SetSwitch(ISP_3A_ALGO_AWB, 0 );
	ISP_IF_3A_SetSwitch(ISP_3A_ALGO_AF, 0 );
	ISP_IF_3A_SetSwitch(ISP_3A_ALGO_AE, 0 );

	ISP_IF_AE_SetShutter(shutter);
	ISP_IF_AE_SetAGain(again);
	ISP_IF_AE_SetDGain(isp_dgain);
	ISP_IF_AE_SetSensorDigitalGain(snr_dgain);
	ISP_IF_AE_SetExpTime(exptime);

	ISP_IF_SNR_SetExpTime		( ISP_IF_AE_GetExpTime() 		); 	// Set sensor exposure time
	ISP_IF_SNR_SetAnalogGain	( ISP_IF_AE_GetAGain() 	); 	// Set sensor analog gain
	ISP_IF_SNR_SetDGain			( ISP_IF_AE_GetDGain() 			); 	// Set ISP digital gain
	ISP_IF_SNR_SetDigitalGain	( ISP_IF_AE_GetSensorDigitalGain() 	); 	// Set sensor digital gain.
	printc( "Force Shutter = %d\tAGain= %d\tISP_DGain = %d\tExptime = %d\tSensor_Dgain = %d\n", shutter,again,isp_dgain,exptime,snr_dgain );
	MMPF_OS_Sleep(100);

	// save debug data
	i = ISP_IF_DBG_SetData((ISP_UINT8*)raw16addr);
	sprintf(m_bRawFileName,"%s.dbg",m_bLsCsRawFileName);
	printc("Dbg Len = %d Name=%s \n",i,m_bRawFileName);
	DramToFile(m_bRawFileName, (INT8*)raw16addr,i);



	EnableCaptureRawPath(m_bLsCsRawFileName);

	switch(ubType)
	{
		case 0:
			Width  = 2568;
			Height = 1932;
			break;
		case 1:
			Width  = 1292;
			Height = 967;
			break;
		case 4:
			Width  = 644;
			Height = 481;
			break;
		case 2:
			Width  = 1928;
			Height = 1087;
			break;
		case 5:
			Width  = 1282;
			Height = 727;
			break;
		case 6:
			Width  = 1282;
			Height = 727;
			break;
		default:
			break;
	}
			
	BytePerLine = 2*Width;
	size_raw = BytePerLine*Height*4/6;
	printc("size_raw = %d\n",size_raw);
		
	printc("filename = %s\n",m_bLsCsRawFileName);

	warm_threshold = 0xFF;
	if(IsTo16Bits == 1)
	{
		readFSCaliData(m_bLsCsRawFileName, (MMP_UBYTE*)rawaddr, size_raw);	//5M


		for(i=0;i<Height;i++)
		{
			for(j=0;j<Width;j++)
				{
				raw16addr[i*Width + j]=GetAITRawPixel((MMP_UBYTE*)rawaddr,j,i,Width);
				if (((FW_RUN_CALI_MODE_EN) ||(CalibrationMode))
					&& (raw16addr[i*Width + j]>=warm_threshold))
					{
					
						WarmPixData[k]=raw16addr[i*Width + j];
						k++;	
						WarmPixData[k]=j;
						k++;	
						WarmPixData[k]=i;
						k++;	
					}
				}
		}
		sprintf(m_bRawFileName,"%s.16bits.raw",m_bLsCsRawFileName);
		DramToFile(m_bRawFileName, (INT8*)raw16addr,Width*Height*sizeof(MMP_SHORT));
		if((FW_RUN_CALI_MODE_EN) ||(CalibrationMode))
			{
			sprintf(m_bRawFileName,"SD:\\warmdata.raw",0);
			DramToFile(m_bRawFileName, (INT8*)WarmPixData,k*sizeof(MMP_SHORT));
			}
	}
}
#else
typedef int dummy_for_avoiding_warning;
#endif

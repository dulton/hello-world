/**
 @file AHC_Capture.c
 @brief AHC capture control Function
 @author 
 @version 1.0
*/

/*===========================================================================
 * Include files
 *===========================================================================*/ 

#include "AHC_General.h"
#include "AHC_Capture.h"
#include "AHC_Display.h"
#include "AHC_UF.h"
#include "AHC_DCF.h"
#include "AHC_GUI.h"
#include "AHC_Sensor.h"
#include "AHC_Config_SDK.h"
#include "AHC_Parameter.h"
#include "AHC_ISP.h"
#include "AHC_Version.h"
#include "mmp_dsc_inc.h"
#include "mmp_ldc_inc.h"
#include "mmp_err.h"
#include "mmps_iva.h"
#include "mmps_dsc.h"
#include "mmps_3gprecd.h"
#include "mmps_sensor.h"
#include "mmpd_sensor.h"
#include "ptz_cfg.h"
#include "ait_utility.h"
#include "MenuSetting.h"
#if (ENABLE_STROBE)
#include "strobe_ctl.h"
#endif
#if (GPS_CONNECT_ENABLE)
#include "GPS_ctl.h"
#endif
#if (GPS_MODULE == GPS_MODULE_NMEA0183) 
#include "gps_nmea0183.h"
#elif (GPS_MODULE == GPS_MODULE_GMC1030) 
#include "GpsRadar_GMC1030.h"
#endif

/** @addtogroup AHC_CAPTURE
@{
*/

/*===========================================================================
 * Global varible
 *===========================================================================*/ 
 
//JPEG Spec. suggestion
static const UINT8 AHC_JPEG_QTABLE_LUMA[64] = {
    16,  11,  12,  14,  12,  10,  16,  14,  
    13,  14,  18,  17,  16,  19,  24,  40, 
    26,  24,  22,  22,  24,  49,  35,  37,
    29,  40,  58,  51,  61,  60,  57,  51,
    56,  55,  64,  72,  92,  78,  64,  68,
    87,  69,  55,  56,  80, 109,  81,  87,
    95,  98, 103, 104, 103,  62,  77, 113, 
   121, 112, 100, 120,  92, 101, 103,  99
};

//JPEG Spec. suggestion
static const UINT8 AHC_JPEG_QTABLE_CHROMA[64] = {
    17,  18,  18,  24,  21,  24,  47,  26,  
    26,  47,  99,  66,  56,  66,  99,  99, 
    99,  99,  99,  99,  99,  99,  99,  99,
    99,  99,  99,  99,  99,  99,  99,  99,
    99,  99,  99,  99,  99,  99,  99,  99,
    99,  99,  99,  99,  99,  99,  99,  99,
    99,  99,  99,  99,  99,  99,  99,  99, 
    99,  99,  99,  99,  99,  99,  99,  99
};

static UINT16                   m_wReviewTime      		= 1000;///< ms
static UINT16                   m_wWindowsTime     		= 500;///< ms
static UINT16                   m_wStillImageResIdx  	= 0;
static UINT32                   m_wStillImageWidth      = 1600;
static UINT32                   m_wStillImageHeight     = 1200;
static UINT32                   m_StillImageTargetSize  = 0;
static UINT8                    m_bySeqNum           	= 3;
static UINT8                    m_bySeqDelay            = 0;
static AHC_ACC_THUMB_SIZE       m_ThumbSize             = THUMB_TYPE_CUSTOMIZE;
static UINT16                   m_wThumbCustomW         = 160;
static UINT16                   m_wThumbCustomH         = 120;
static UINT32                   m_VideoRecordWidth      = 1920;
static UINT32                   m_VideoRecordHeight     = 1088;
static AHC_BOOL                 m_bRawWithJpeg          = AHC_TRUE;
static UINT32                   m_TimeStampConfig       = (AHC_ACC_TIMESTAMP_ENABLE		|
														   AHC_ACC_TIMESTAMP_DATE_ONLY	|
														   AHC_CAPTURE_STICKER_POS		|
														   AHC_ACC_TIMESTAMP_FORMAT_1);
static UINT16             		m_wCurrentStillImageMode = 0;

static UINT16                   m_wContiShotMaxNum      = 10;
static UINT16                   m_wContiShotInterval    = 200;///< ms

#if (ENABLE_STROBE)
static AHC_BOOL                 m_byStrobeStatus        = AHC_FALSE;
static AHC_BOOL                 m_byStrobeReadyStatus   = AHC_FALSE;
static UINT16                   m_StrobeBlankDuration 	= 0;
static UINT16                   m_StrobeTriggerDuration = 0;
#endif

#if (ENABLE_DEBUG_AFTER_JPG)
static UINT8              		m_ubDebugMessageBuffer[MAX_DBG_ARRAY_SIZE]={0};
#endif

/*===========================================================================
 * Extern varible
 *===========================================================================*/ 

extern MMP_USHORT               gsAhcCurrentSensor;
extern AHC_PARAM 				glAhcParameter;
extern AHC_BOOL                 gbAhcDbgBrk;
/*===========================================================================
 * Extern function
 *===========================================================================*/ 

extern UINT16 ZoomCtrl_GetStillDigitalZoomLevel(void);
extern void MenuItemDateTimeFormatToAhcTimeStampSetting(void);

/*===========================================================================
 * Main body
 *===========================================================================*/ 

#if 0
void _____EXIF_Functions_____(){}
#endif

//------------------------------------------------------------------------------
//  Function    : AHC_SetImageEXIF
//  Description : 
//------------------------------------------------------------------------------ 
/**
 @brief Set image exif

 This function sets the fields in EXIF. The contents of the field are read from 
 the AHC_SET_EXIF_MEM_LOCATION memory location, default : width/height/date.
 Parameters:
 @param[in] pData       the update data buffer.
 @param[in] DataSize    update data size.
 @param[in] uwIfdIdx    The IFD index of tag field.
 @param[in] uwTagID     The ID of tag field.
 @param[in] wType       The type of tag.
 @param[in] uwCount     Set the amount of entries of uwType.
 @param[in] uwLevel     The level of support for the tag.
 
 @retval AHC_TRUE Success.
*/
AHC_BOOL AHC_SetImageEXIF(UINT8 *pData, UINT32 DataSize, UINT16 uwIfdIdx, UINT16 uwTagID, UINT16 uwType, UINT16 uwCount)
{
	return MMPS_DSC_SetImageEXIF((MMP_UBYTE *)pData, (MMP_ULONG)DataSize, (MMP_USHORT)uwIfdIdx, (MMP_USHORT)uwTagID, (MMP_USHORT)uwType, (MMP_USHORT)uwCount);
} 

//------------------------------------------------------------------------------
//  Function    : AHC_GetImageEXIF
//  Description : 
//------------------------------------------------------------------------------ 
/**
 @brief Get EXIF Setting

 This API gets EXIF Setting
 Parameters:
 @param[in]  Index EXIF Index
 @param[out] pExif EXIF setting
 @retval AHC_TRUE Success.
*/
AHC_BOOL AHC_GetImageEXIF(UINT32 Index, AHC_EXIF_CONFIG *pExif)
{
	return MMPS_DSC_GetImageEXIF(Index, (MMPS_DSC_EXIF_CONFIG *)pExif);
}

//------------------------------------------------------------------------------
//  Function    : AHC_OpenEXIFFileByIdx
//  Description : 
//------------------------------------------------------------------------------
/**
 @brief Open EXIF file by the DCF index

 This API open EXIF file by the DCF index
 
 Parameters:
 @param[in] DcfIdx DCF index
 @retval AHC_TRUE Success.
*/
AHC_BOOL AHC_OpenEXIFFileByIdx(UINT32 DcfIdx)
{
    MMP_ERR     error = MMP_ERR_NONE;
    char        FilePathName[MAX_FILE_NAME_SIZE];

	MEMSET(FilePathName, 0, sizeof(FilePathName));
	AHC_UF_GetFilePathNamebyIndex(DcfIdx, FilePathName);
        
    error = MMPS_DSC_OpenEXIFFile((MMP_UBYTE*)FilePathName, EXIF_NODE_ID_PRIMARY);
    
    return (error == MMP_ERR_NONE) ? AHC_TRUE : AHC_FALSE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_OpenEXIFFileByName
//  Description : 
//------------------------------------------------------------------------------
/**
 @brief Open EXIF file by the file name

 This API open EXIF file by the file name
 
 Parameters:
 @param[in] sFileName file name
 @retval AHC_TRUE Success.
*/
AHC_BOOL AHC_OpenEXIFFileByName(UINT8 *sFileName)
{
    MMP_ERR error = MMP_ERR_NONE;
        
    error = MMPS_DSC_OpenEXIFFile(sFileName, EXIF_NODE_ID_PRIMARY);
    
    return (error == MMP_ERR_NONE) ? AHC_TRUE : AHC_FALSE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_CloseEXIFFile
//  Description : 
//------------------------------------------------------------------------------
/**
 @brief Close EXIF file

 This API close the EXIF file
 
 Parameters:
 
 @retval AHC_TRUE Success.
*/
AHC_BOOL AHC_CloseEXIFFile(void)
{
    MMP_ERR error = MMP_ERR_NONE;
    
    error = MMPS_DSC_CloseEXIFFile(EXIF_NODE_ID_PRIMARY);
    
    return (error == MMP_ERR_NONE) ? AHC_TRUE : AHC_FALSE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_ConfigEXIF_RTC
//  Description : 
//------------------------------------------------------------------------------
/**
 @brief Config EXIF Related to time

 This API  Config EXIF Related to time
 
 Parameters:
 
 @retval AHC_TRUE Success.
*/
AHC_BOOL AHC_ConfigEXIF_RTC(void)
{
	#define EXIF_DATE_LENGTH	20

    UINT16 		uwYear,uwMonth,uwDay,uwDayInWeek,uwHour,uwMinute,uwSecond;
    UINT8 		ubAmOrPm, b_12FormatEn;
    
    INT8     	StrYear[8], StrMonth[8], StrDay[8], StrHour[8], StrMinute[8], StrSecond[8], date[EXIF_DATE_LENGTH]={0}; 
    AHC_BOOL	success;

    AHC_SetParam(PARAM_ID_USE_RTC, 1);
    
    success = AHC_GetClock(&uwYear, &uwMonth, &uwDay, &uwDayInWeek, &uwHour, &uwMinute, &uwSecond, &ubAmOrPm, &b_12FormatEn);

    AIHC_Int2Str(uwYear   , StrYear);
    AIHC_Int2Str(uwMonth  , StrMonth);
    AIHC_Int2Str(uwDay    , StrDay);
    AIHC_Int2Str(uwHour   , StrHour);
    AIHC_Int2Str(uwMinute , StrMinute);
    AIHC_Int2Str(uwSecond , StrSecond);

    MEMSET(date, 0, sizeof(date));
    
	STRCPY(date, StrYear);
    STRCAT(date, ":");
            
    if (uwMonth < 10)
	    STRCAT(date, "0");
                
    STRCAT(date, StrMonth);
    STRCAT(date, ":");
            
    if (uwDay < 10)
	    STRCAT(date, "0");
                
    STRCAT(date, StrDay);
    
    STRCAT(date, " ");
            
    if (uwHour < 10)
	    STRCAT(date, "0");
                
    STRCAT(date, StrHour);
    STRCAT(date, ":");
           
    if (uwMinute < 10)
	    STRCAT(date, "0");
             
    STRCAT(date, StrMinute);
    STRCAT(date, ":");
            
    if (uwSecond < 10)
	    STRCAT(date, "0");
                
    STRCAT(date, StrSecond);
    
   	AHC_SetImageEXIF((UINT8*)date, EXIF_DATE_LENGTH, IFD_IDX_IFD0, 		IFD0_DATE_TIME, 			EXIF_TYPE_ASCII, EXIF_DATE_LENGTH);
	AHC_SetImageEXIF((UINT8*)date, EXIF_DATE_LENGTH, IFD_IDX_EXIFIFD, 	EXIFIFD_DATETIMEORIGINAL, 	EXIF_TYPE_ASCII, EXIF_DATE_LENGTH);
	AHC_SetImageEXIF((UINT8*)date, EXIF_DATE_LENGTH, IFD_IDX_EXIFIFD, 	EXIFIFD_DATETIMECREATE, 	EXIF_TYPE_ASCII, EXIF_DATE_LENGTH);
	 
    return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_ConfigEXIF_SystemInfo
//  Description : 
//------------------------------------------------------------------------------
/**
 @brief Config EXIF Related to System Info

 This API  Config EXIF Related to System Info
 
 Parameters:
 
 @retval AHC_TRUE Success.
*/
AHC_BOOL AHC_ConfigEXIF_SystemInfo(void)
{
	MMP_BYTE   exif_ver[] = {'V', '2', '.', '3'};		// Customer request format.
	//MMP_BYTE   exif_ver[] = {0x30, 0x32, 0x33, 0x30}; // Default format 0230 means version 2.3
	MMP_ULONG  ulExifIfdOffset = 0;
	MMP_USHORT exif_orientation = 1;
	MMP_ULONG  exif_res[2] = {300, 1};
	MMP_USHORT exif_res_unit = 2;
	MMP_USHORT ycc_pos = 1 ;
	MMP_UBYTE  exif_sensingmethod = 2;
	MMP_UBYTE  exif_scenetype = 1;
	MMP_USHORT flash = 0x00;
	MMP_USHORT colorspace = 0x01;
	MMP_USHORT scene_capture_type = 0x00;
	MMP_USHORT gain_control = 0x00;
    MMP_USHORT contrast = 0x00;
    MMP_USHORT saturation = 0x00;
    MMP_USHORT sharpness = 0x00;
    MMP_USHORT exposureprogram = 0x02 ;
	MMP_UBYTE  components[4] = {1, 2, 3, 0};
	MMP_ULONG  focallength[2] = {68, 10};
	MMP_ULONG  aperture[2] = {37, 10};
	MMP_ULONG  maxaperture[2] = {38, 10};
	
	AHC_SetImageEXIF((UINT8*)&ulExifIfdOffset		   , 4									  , IFD_IDX_IFD0, IFD0_EXIF_OFFSET	, EXIF_TYPE_LONG , 1);

	#if (EXIF_ENABLE_MAKER_INFO == 1)
	AHC_SetImageEXIF((UINT8*)AHC_EXIF_DEFAULT_MAKER    , STRLEN(AHC_EXIF_DEFAULT_MAKER)       , IFD_IDX_IFD0, IFD0_MAKE      	, EXIF_TYPE_ASCII, STRLEN(AHC_EXIF_DEFAULT_MAKER));		
	AHC_SetImageEXIF((UINT8*)AHC_EXIF_DEFAULT_MODEL    , STRLEN(AHC_EXIF_DEFAULT_MODEL)       , IFD_IDX_IFD0, IFD0_MODEL     	, EXIF_TYPE_ASCII, STRLEN(AHC_EXIF_DEFAULT_MODEL));
	AHC_SetImageEXIF((UINT8*)AHC_EXIF_DEFAULT_SOFTWARE , STRLEN(AHC_EXIF_DEFAULT_SOFTWARE)    , IFD_IDX_IFD0, IFD0_SOFTWARE 	, EXIF_TYPE_ASCII, STRLEN(AHC_EXIF_DEFAULT_SOFTWARE));
	AHC_SetImageEXIF((UINT8*)AHC_EXIF_DEFAULT_ARTIST   , STRLEN(AHC_EXIF_DEFAULT_ARTIST)      , IFD_IDX_IFD0, IFD0_ARTIST    	, EXIF_TYPE_ASCII, STRLEN(AHC_EXIF_DEFAULT_ARTIST));
	AHC_SetImageEXIF((UINT8*)AHC_EXIF_DEFAULT_COPYRIGHT, STRLEN(AHC_EXIF_DEFAULT_COPYRIGHT)   , IFD_IDX_IFD0, IFD0_COPYRIGHT	, EXIF_TYPE_ASCII, STRLEN(AHC_EXIF_DEFAULT_COPYRIGHT));
	#endif
	
	AHC_SetImageEXIF((UINT8*)&exif_ver          	, 4,  IFD_IDX_EXIFIFD, 	EXIFIFD_VERSION			, EXIF_TYPE_ASCII	, 4);
	AHC_SetImageEXIF((UINT8*)&exif_orientation   	, 2,  IFD_IDX_IFD0, 	IFD0_ORIENTATION       	, EXIF_TYPE_SHORT	, 1);
	AHC_SetImageEXIF((UINT8*)&exif_res           	, 8,  IFD_IDX_IFD0, 	IFD0_X_RESOLUTION      	, EXIF_TYPE_RATIONAL, 1);
	AHC_SetImageEXIF((UINT8*)&exif_res            	, 8,  IFD_IDX_IFD0, 	IFD0_Y_RESOLUTION      	, EXIF_TYPE_RATIONAL, 1);
	AHC_SetImageEXIF((UINT8*)&exif_res_unit      	, 2,  IFD_IDX_IFD0, 	IFD0_RESOLUTION_UNIT   	, EXIF_TYPE_SHORT   , 1);
	AHC_SetImageEXIF((UINT8*)&ycc_pos            	, 2,  IFD_IDX_IFD0, 	IFD0_YCBCR_POSITION    	, EXIF_TYPE_SHORT   , 1);
	AHC_SetImageEXIF((UINT8*)&exif_sensingmethod	, 1,  IFD_IDX_IFD0, 	IFD0_SENSINGMETHOD  	, EXIF_TYPE_BYTE    , 1);
	AHC_SetImageEXIF((UINT8*)&exif_scenetype		, 1,  IFD_IDX_EXIFIFD, 	EXIFIFD_SCENETYPE  		, EXIF_TYPE_BYTE  	, 1);
	AHC_SetImageEXIF((UINT8*)&flash					, 2,  IFD_IDX_EXIFIFD, 	EXIFIFD_FLASH  			, EXIF_TYPE_SHORT 	, 1);
	AHC_SetImageEXIF((UINT8*)&colorspace			, 2,  IFD_IDX_EXIFIFD, 	EXIFIFD_COLORSPACE  	, EXIF_TYPE_SHORT 	, 1);
	AHC_SetImageEXIF((UINT8*)&scene_capture_type	, 2,  IFD_IDX_EXIFIFD, 	EXIFIFD_SCENE_CAP_TYPE	, EXIF_TYPE_SHORT 	, 1);
	AHC_SetImageEXIF((UINT8*)&gain_control			, 2,  IFD_IDX_EXIFIFD, 	EXIFIFD_GAINCONTROL 	, EXIF_TYPE_SHORT 	, 1);
	AHC_SetImageEXIF((UINT8*)&contrast				, 2,  IFD_IDX_EXIFIFD, 	EXIFIFD_CONTRAST  		, EXIF_TYPE_SHORT 	, 1);
	AHC_SetImageEXIF((UINT8*)&saturation			, 2,  IFD_IDX_EXIFIFD, 	EXIFIFD_SATURATION 		, EXIF_TYPE_SHORT 	, 1);
	AHC_SetImageEXIF((UINT8*)&sharpness				, 2,  IFD_IDX_EXIFIFD, 	EXIFIFD_SHARPNESS  		, EXIF_TYPE_SHORT 	, 1);
	AHC_SetImageEXIF((UINT8*)&components			, 4,  IFD_IDX_EXIFIFD, 	EXIFIFD_COMPONENTS  	, EXIF_TYPE_BYTE	, 4);
	AHC_SetImageEXIF((UINT8*)&focallength			, 8,  IFD_IDX_EXIFIFD, 	EXIFIFD_FOCAL_LENGTH 	, EXIF_TYPE_RATIONAL, 1);
	AHC_SetImageEXIF((UINT8*)&aperture				, 8,  IFD_IDX_EXIFIFD, 	EXIFIFD_APERTURE  		, EXIF_TYPE_RATIONAL, 1);
	AHC_SetImageEXIF((UINT8*)&maxaperture			, 8,  IFD_IDX_EXIFIFD, 	EXIFIFD_MAX_APERTURE  	, EXIF_TYPE_RATIONAL, 1);
	AHC_SetImageEXIF((UINT8*)&exposureprogram		, 2,  IFD_IDX_EXIFIFD, 	EXIFIFD_EXPO_PROGRAM  	, EXIF_TYPE_SHORT 	, 1);
	
	#if (GPS_CONNECT_ENABLE)
	if (AHC_GPS_Module_Attached())
	{
		MMP_ULONG  ulVersion = 2;
		MMP_ULONG  ulGPSoffset = 0;	
		MMP_ULONG  ulGpsLatitude[6] = {50, 1, 40, 1,30, 1};
		MMP_ULONG  ulGpsLongitude[6] = {30, 1, 20, 1,10, 1};
		MMP_ULONG  ulGpsTimeStamp[6] = {24, 2, 60, 2, 25, 3};
		MMP_UBYTE  ubNS[2];
		MMP_UBYTE  ubWE[2];
		MMP_UBYTE  ubSpeedRef[2];
		MMP_ULONG  ulSpeed[2] = {123, 2};
		MMP_UBYTE  ubAltitudeRef;
		MMP_ULONG  ulSeeAltitude[2] = {111, 2}; 
	    MMP_USHORT usDegree,usMin;	
	 	double     usSec;
	 	
	 	#if(GPS_MODULE == GPS_MODULE_NMEA0183)
		NMEAINFO   *pInfo;
	    pInfo =(NMEAINFO *)GPS_Information();
	    #elif(GPS_MODULE == GPS_MODULE_GMC1030)
	    GPSRADARINFO   *pInfo;
	    pInfo =(GPSRADARINFO *)GPSRadar_Information();
	    #endif
		
		ubNS[0] = 'N';
		ubNS[1] = 0;
		ubWE[0] = 'E';
		ubWE[1] = 0;
		ubSpeedRef[0] = 'K';
		ubSpeedRef[1] = 0;
		
		AHC_SetImageEXIF((UINT8*)&ulGPSoffset	, 4, IFD_IDX_IFD0, 		EXIFIFD_GPSINFO	, EXIF_TYPE_LONG, 1);
		AHC_SetImageEXIF((UINT8*)&ulVersion		, 4, IFD_IDX_GPSIFD, 	GPS_VERSION		, EXIF_TYPE_BYTE, 4);
		
		if (GPSCtrl_GPS_IsValidValue())
		{
			ubNS[0] 		= GPSCtrl_CheckRefForExif(0);
			ubWE[0] 		= GPSCtrl_CheckRefForExif(1);
			ubAltitudeRef 	= GPSCtrl_CheckRefForExif(2);
			ubSpeedRef[0] 	= GPSCtrl_CheckRefForExif(3);
			
			GPSCtrl_GPSFormatTrans(pInfo->dwLat,&usDegree,&usMin,&usSec);
			ulGpsLatitude[0] = (MMP_ULONG)usDegree; ulGpsLatitude[2] = (MMP_ULONG)usMin; ulGpsLatitude[4] = (MMP_ULONG)(usSec*10000); 
			ulGpsLatitude[1] = 1	   			  ; ulGpsLatitude[3] = 1    		   ; ulGpsLatitude[5] = 10000; 
			
			GPSCtrl_GPSFormatTrans(pInfo->dwLon,&usDegree,&usMin,&usSec);
			ulGpsLongitude[0] = (MMP_ULONG)usDegree; ulGpsLongitude[2] = (MMP_ULONG)usMin; ulGpsLongitude[4] = (MMP_ULONG)(usSec*10000); 
			ulGpsLongitude[1] = 1	    		   ; ulGpsLongitude[3] = 1    			 ; ulGpsLongitude[5] = 10000; 
			
			ulSeeAltitude[0] = GPS_ABS(pInfo->dwElv);
			ulSeeAltitude[1] = 1;
			
			ulSpeed[0] = pInfo->dwSpeed;
			ulSpeed[1] = 1;
			
			ulGpsTimeStamp[0] = (MMP_ULONG)pInfo->sUTC.iHour; ulGpsTimeStamp[2] = (MMP_ULONG)pInfo->sUTC.iMin;ulGpsTimeStamp[4] = (MMP_ULONG)pInfo->sUTC.iSec;
			ulGpsTimeStamp[1] = 1;				  			  ulGpsTimeStamp[3] = 1;			   			  ulGpsTimeStamp[5] = 1;
			
		    AHC_SetImageEXIF((UINT8*)&ubNS				, 2		, IFD_IDX_GPSIFD, GPS_LATITUDE_REF	, EXIF_TYPE_ASCII	, 2);
		    AHC_SetImageEXIF((UINT8*)&(ulGpsLatitude)	, 24	, IFD_IDX_GPSIFD, GPS_LATITUDE		, EXIF_TYPE_RATIONAL, 3);
			AHC_SetImageEXIF((UINT8*)&ubWE				, 2		, IFD_IDX_GPSIFD, GPS_LONGITUDE_REF	, EXIF_TYPE_ASCII	, 2);
		    AHC_SetImageEXIF((UINT8*)&(ulGpsLongitude)	, 24	, IFD_IDX_GPSIFD, GPS_LONGITUDE		, EXIF_TYPE_RATIONAL, 3);
		    AHC_SetImageEXIF((UINT8*)&ubAltitudeRef		, 1		, IFD_IDX_GPSIFD, GPS_ALTITUDE_REF	, EXIF_TYPE_BYTE	, 1);
		    AHC_SetImageEXIF((UINT8*)&ulSeeAltitude		, 8		, IFD_IDX_GPSIFD, GPS_ALTITUDE		, EXIF_TYPE_RATIONAL, 1);
		    AHC_SetImageEXIF((UINT8*)&(ulGpsTimeStamp)	, 24	, IFD_IDX_GPSIFD, GPS_TIME_STAMP	, EXIF_TYPE_RATIONAL, 3);
		    AHC_SetImageEXIF((UINT8*)&ubSpeedRef		, 2		, IFD_IDX_GPSIFD, GPS_SPEED_REF		, EXIF_TYPE_ASCII	, 2);
		    AHC_SetImageEXIF((UINT8*)&ulSpeed			, 8		, IFD_IDX_GPSIFD, GPS_SPEED			, EXIF_TYPE_RATIONAL, 1);
		}
	}
	else {
		return AHC_TRUE;
	}
	#endif
	
	return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_ConfigEXIF_MENU
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Config EXIF Related to MENU settings

 This API  Config EXIF Related to MENU settings
 
 Parameters:
 
 @retval AHC_TRUE Success.
*/
AHC_BOOL AHC_ConfigEXIF_MENU(void)
{
    MMP_USHORT 	whitebalance = 0x00;
    MMP_USHORT 	lightsource = 0x00;
    MMP_USHORT 	metering = 0x00;
    UINT8 		cmeter = 0;
    UINT8 		cwb = 0;

    if ((AHC_Menu_SettingGetCB((char*)COMMON_KEY_METER, &cmeter) == AHC_FALSE))
    {
    	cmeter = COMMON_METERING_DEFAULT; // default 
    }

    switch(cmeter)
	{
	#if (MENU_MANUAL_METERING_MULTI_EN)
		case COMMON_METERING_MULTI:			metering = 0x05;
		break;
	#endif
	#if (MENU_MANUAL_METERING_WEIGHTED_EN)	
		case COMMON_METERING_WEIGHTED:		metering = 0x01;
		break;
	#endif
	#if (MENU_MANUAL_METERING_CENTER_SPOT_EN)	
		case COMMON_METERING_CENTER_SPOT:	metering = 0x02;
		break;
	#endif
	default:
		break;
	}
	
	AHC_SetImageEXIF((UINT8*)&metering, 2 , IFD_IDX_EXIFIFD, EXIFIFD_METERINGMODE, EXIF_TYPE_SHORT, 1);
	
	if ((AHC_Menu_SettingGetCB((char*)COMMON_KEY_WB, &cwb) == AHC_FALSE))
	{
  		cwb = COMMON_WB_DEFAULT; // default;
    }

	switch(cwb)
	{
	#if (MENU_MANUAL_WB_AUTO_EN)
		case COMMON_WB_AUTO:			whitebalance = 0x00;
		break;
	#endif
	#if (MENU_MANUAL_WB_DAYLIGHT_EN)
		case COMMON_WB_DAYLIGHT:   		whitebalance = 0x01; lightsource = 1;
    		break;
    #endif
    #if (MENU_MANUAL_WB_CLOUDY_EN)
		case COMMON_WB_CLOUDY:			whitebalance = 0x01; lightsource = 10;
		break;
 	#endif
	#if (MENU_MANUAL_WB_FLUORESCENT1_EN)
		case COMMON_WB_FLUORESCENT1:	whitebalance = 0x01; lightsource = 2;
		break;
	#endif
    #if (MENU_MANUAL_WB_FLUORESCENT2_EN)
		case COMMON_WB_FLUORESCENT2:	whitebalance = 0x01; lightsource = 2;
		break;
	#endif
    #if (MENU_MANUAL_WB_FLUORESCENT3_EN)
    	case COMMON_WB_FLUORESCENT3:	whitebalance = 0x01; lightsource = 2;
    	break;
    #endif
	#if (MENU_MANUAL_WB_INCANDESCENT_EN)
		case COMMON_WB_INCANDESCENT:   	whitebalance = 0x01; lightsource = 21;
		break;
	#endif
	#if (MENU_MANUAL_WB_FLASH_EN)
		case COMMON_WB_FLASH:			whitebalance = 0x01; lightsource = 4;
		break;
	#endif
	#if (MENU_MANUAL_WB_ONEPUSH_EN)	
		case COMMON_WB_ONEPUSH:			whitebalance = 0x01; lightsource = 0xFF;
	break;
	#endif
	#if (MENU_MANUAL_WB_ONE_PUSH_SET_EN)
		case COMMON_WB_ONE_PUSH_SET:	whitebalance = 0x01; lightsource = 0xFF;
		break;
	#endif	
		default:
    	break;
	}
	
	AHC_SetImageEXIF((UINT8*)&whitebalance, 2, IFD_IDX_EXIFIFD, EXIFIFD_WHITEBLANCE, EXIF_TYPE_SHORT, 1);
	AHC_SetImageEXIF((UINT8*)&lightsource, 	2, IFD_IDX_EXIFIFD, EXIFIFD_LIGHTSOURCE, EXIF_TYPE_SHORT, 1);
	
	return AHC_TRUE;
}

#if 0
void _____Capture_Functions_____(){}
#endif

//------------------------------------------------------------------------------
//  Function    : AHC_ConfigCapture
//  Description : 
//------------------------------------------------------------------------------ 
/**
 @brief Config capture mode

 This API controls several parameters which are relevant to still or video capture.
 Parameters:
 @param[in] byConfig Flags for configuration.
 @param[in] wOp Operand
 @retval AHC_TRUE Success.
*/
AHC_BOOL AHC_ConfigCapture(AHC_CAPTURE_CONFIG byConfig, UINT16 wOp)
{
    switch (byConfig) {
        case ACC_THUMB_SIZE:
            m_ThumbSize = wOp;
            
            if (THUMB_TYPE_DISABLE == m_ThumbSize) {
                m_wThumbCustomW = 0;
                m_wThumbCustomH = 0;
            }
            else if (THUMB_TYPE_QVGA == m_ThumbSize) {
                m_wThumbCustomW = 320;
                m_wThumbCustomH = 240;
            }
            else if (THUMB_TYPE_VGA == m_ThumbSize) {
                m_wThumbCustomW = 640;
                m_wThumbCustomH = 480;
            }
            break;
        case ACC_CUS_THUMB_W:
            if (wOp % 16) {
                return AHC_FALSE;
            }
            m_wThumbCustomW = wOp;
            break;
        case ACC_CUS_THUMB_H:
            if (wOp % 8) {
                return AHC_FALSE;
            }    
            m_wThumbCustomH = wOp;
            break;
        case ACC_CAPTURE_RAW_WITH_JPEG:
            m_bRawWithJpeg = wOp;
            break; 
        case ACC_DATE_STAMP:
            m_TimeStampConfig= wOp;
            break;
        default:
            return AHC_FALSE;
    }
    
	return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_GetCaptureConfig
//  Description : 
//------------------------------------------------------------------------------
/**
 @brief Get the capture config

 This API gets the capture configuration 
 Parameters:
 @param[in] byConfig Flags for configuration.
 @param[in] wOp Operand
 @retval AHC_TRUE Success.
*/
AHC_BOOL AHC_GetCaptureConfig(AHC_CAPTURE_CONFIG byConfig, UINT32 *wOp)
{
    switch (byConfig) {
        case ACC_THUMB_SIZE:
            *wOp = m_ThumbSize;
            break;
        case ACC_CUS_THUMB_W:
            *wOp = m_wThumbCustomW;
            break;
        case ACC_CUS_THUMB_H:
            *wOp = m_wThumbCustomH;
            break;
        case ACC_CAPTURE_RAW_WITH_JPEG:
            *wOp = m_bRawWithJpeg;
            break;
        case ACC_DATE_STAMP:
            *wOp = m_TimeStampConfig;
            break;
        default:
            return AHC_FALSE;
    }
	return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_GetImageAttributes
//  Description : 
//------------------------------------------------------------------------------ 
/**
 @brief Get image attribute

 This function get the attributes of current selected image.
 Parameters:
 @param[out] *pImgAttr ImageHSize, ImageVSize, ThumbHSize, ThumbVSize..etc
 @retval AHC_TRUE Success.
*/
AHC_BOOL AHC_GetImageAttributes(IMG_ATTRIB *pImgAttr)
{
    MMP_ERR             error = MMP_ERR_NONE;
    MMP_DSC_JPEG_INFO   jpeginfo;
    UINT32              CurrentDcfIdx;
	UINT8 				FileType;
	char     			FilePathName[MAX_FILE_NAME_SIZE];
        
    // Get file name
    AHC_UF_GetCurrentIndex(&CurrentDcfIdx);
    
	MEMSET(FilePathName, 0, sizeof(FilePathName));

    AHC_UF_GetFilePathNamebyIndex(CurrentDcfIdx, FilePathName);
    AHC_UF_GetFileTypebyIndex(CurrentDcfIdx, &FileType); 
	
	if (FileType == DCF_OBG_JPG) {

    	STRCPY(jpeginfo.bJpegFileName, FilePathName);
        
        jpeginfo.usJpegFileNameLength   = STRLEN(FilePathName);
        jpeginfo.ulJpegBufAddr          = 0;
        jpeginfo.ulJpegBufSize          = 0;
    	jpeginfo.bDecodeThumbnail       = MMP_TRUE;
    	#if (DSC_SUPPORT_BASELINE_MP_FILE)
    	jpeginfo.bDecodeLargeThumb 		= MMP_TRUE;
    	#endif
    	jpeginfo.bValid                 = MMP_FALSE;
    	
        error = MMPS_DSC_GetJpegInfo(&jpeginfo);
        
        if (error == MMP_ERR_NONE) {
            pImgAttr->ImageHSize = jpeginfo.usPrimaryWidth;
            pImgAttr->ImageVSize = jpeginfo.usPrimaryHeight;
            pImgAttr->ThumbHSize = jpeginfo.usThumbWidth;
            pImgAttr->ThumbVSize = jpeginfo.usThumbHeight;
            
            return AHC_TRUE;
        }
        else {
            return AHC_FALSE;
    	}
    }
    else {
        return AHC_FALSE;
	}
}

//------------------------------------------------------------------------------
//  Function    : AHC_SetCompressionRatio
//  Description : 
//------------------------------------------------------------------------------ 
/**
 @brief Set compression ratio

 Set the compression ratio of still capture.
 Parameters:
 @param[in] wRatio 1~1000, means 1/1000~ 1000/1000
 @retval AHC_TRUE Success.
*/
AHC_BOOL AHC_SetCompressionRatio(UINT16 wRatio)
{
    m_StillImageTargetSize = (m_wStillImageWidth * m_wStillImageHeight / 1024) * wRatio / 1000;

	return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_GetStillImageTargetSize
//  Description : 
//------------------------------------------------------------------------------ 
UINT32 AHC_GetStillImageTargetSize(void)
{
	return m_StillImageTargetSize * 1024;
}

//------------------------------------------------------------------------------
//  Function    : AHC_SetCompressionQuality
//  Description : 
//------------------------------------------------------------------------------ 
/**
 @brief Set compression quality

 Set the compression quality of still capture.
 Parameters:
 @param[in] Quality 10~100, means 10/100~ 100/100
 @retval AHC_TRUE Success.
*/
AHC_BOOL AHC_SetCompressionQuality(UINT32 Quality)
{
    MMP_ERR error;
    INT32   temp, i;
    UINT8   QTable[2][64];
    
    if ((Quality < 10) || (Quality > 100)) {
        return AHC_FALSE;
    }

    if (Quality < 50)
        Quality = 5000 / Quality;
    else
        Quality = 200 - Quality*2;

    // LUMA
    for (i = 0; i < 64; i++) {
        temp = ((long) AHC_JPEG_QTABLE_LUMA[i] * Quality + 50) / 100;

        /* limit the values to the valid range */
        if (temp <= 0) 
        	temp = 1;
        
         /* max quantizer needed for 12 bits */
        if (temp > 32767) 
        	temp = 32767;
        	
		/* limit to baseline range if requested */
        if (temp > 255)
            temp = 255;

        QTable[0][i] = (UINT16) temp;
    }
    
    // CHROMA
    for (i = 0; i < 64; i++) {
        temp = ((long) AHC_JPEG_QTABLE_CHROMA[i] * Quality + 50) / 100;

        /* limit the values to the valid range */
        if (temp <= 0) 
        	temp = 1;
        
         /* max quantizer needed for 12 bits */
        if (temp > 32767) 
        	temp = 32767;
        	
		/* limit to baseline range if requested */
        if (temp > 255)
            temp = 255;

        QTable[1][i] = (UINT16) temp;
    }
    
    error = MMPS_DSC_SetCustomQTable(MMPS_DSC_CUSTOM_QTABLE_ALL, QTable[0], QTable[1], QTable[1]); // zigzag order
    
	return (error == MMP_ERR_NONE) ? AHC_TRUE : AHC_FALSE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_SetSeqCaptureParam
//  Description : 
//------------------------------------------------------------------------------ 
/**
 @brief Set sequence capture length

 Set the length of captured sequence and delay between 2 consecutive captured images.
 Parameters:
 @param[in] bySeqLength The length of sequential capture. This variable must take the DRAM size as a consideration.
 @param[in] byDelay The dealy between consecutive images.
 @retval AHC_TRUE Success.
*/
AHC_BOOL AHC_SetSeqCaptureParam(UINT8 bySeqNum, UINT8 byDelay)
{
    m_bySeqNum 		= bySeqNum;
    m_bySeqDelay  	= byDelay;
    
	return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_GetSeqCaptureParam
//  Description : 
//------------------------------------------------------------------------------
/**
 @brief Get sequence capture length

 Get the length of captured sequence and delay between 2 consecutive captured images.
 Parameters:
 @param[out] bySeqLength The length of sequential capture. This variable must take the DRAM size as a consideration.
 @param[out] byDelay The dealy between consecutive images.
 @retval AHC_TRUE Success.
*/
AHC_BOOL AHC_GetSeqCaptureParam(UINT8 *bySeqNum, UINT8 *byDelay)
{
    *bySeqNum = m_bySeqNum;
    *byDelay  = m_bySeqDelay;
    
	return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_GetCaptureStatus
//  Description : 
//------------------------------------------------------------------------------ 
/**
 @brief Get capture status

 This API decides whether current captured image should be stored or not. This API 
 is flexible for controlling the store operation in particular case. Such like in 
 card plug-in/out, sequential delay capture..etc.
 Parameters:
 @param[in] byCaptureMode Capture mode: 0: Standard, 1:Night mode.
 @param[out] *pwDramStatus Report if there has enough free space for next capture. 0: Not enough, 1: enough.
 @param[out] *pwPending Report number of pending images that still stored in DRAM.
 @retval AHC_TRUE Success.
*/
AHC_BOOL AHC_GetCaptureStatus(UINT16 *pwDramStatus, UINT16 *pwPending)
{
    MMPS_DSC_GetCaptureStatus((MMP_USHORT *)pwDramStatus, (MMP_USHORT *)pwPending);
    
	return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_SetImageTime
//  Description :
//------------------------------------------------------------------------------ 
AHC_BOOL AHC_SetImageTime(UINT16 wReviewTime, UINT16 wWindowsTime)
{
	m_wReviewTime	= wReviewTime;
	m_wWindowsTime	= wWindowsTime;
	
	return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_GetImageTime
//  Description :
//------------------------------------------------------------------------------ 
AHC_BOOL AHC_GetImageTime(UINT16 *wReviewTime, UINT16 *wWindowsTime)
{
	*wReviewTime  = m_wReviewTime;
	*wWindowsTime = m_wWindowsTime;
	
	return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_GetStillImageResIdx
//  Description :
//------------------------------------------------------------------------------ 
UINT16 AHC_GetStillImageResIdx(void)
{
	return m_wStillImageResIdx;
}

//------------------------------------------------------------------------------
//  Function    : AHC_SetContiShotParam
//  Description :
//------------------------------------------------------------------------------ 
AHC_BOOL AHC_SetContiShotParam(UINT16 wMaxNum, UINT16 wInterval)
{
	m_wContiShotMaxNum      = wMaxNum;
	m_wContiShotInterval    = wInterval;
	
	return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_SetImageSize
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Set capture resolution

 This API configures the captured resolution in selected mode.
 Parameters:
 @param[in] byMode Which mode to be configured.
 @param[in] wSizeX Width.
 @param[in] wSizeY Height
 @retval AHC_TRUE Success.
*/
AHC_BOOL AHC_SetImageSize(UINT8 byMode, UINT16 wSizeX, UINT16 wSizeY)
{
    MMP_ERR     error           = MMP_ERR_NONE;
    MMP_BOOL    bInPreview      = MMP_FALSE;
    MMP_BOOL    bRestartPreview = MMP_FALSE;
    UINT32      SysMode;
    UINT16		wModeIndex, ConfigWidth, ConfigHeight;
    
    switch(byMode) {
    case STILL_CAPTURE_MODE:
        if ((wSizeX == 0) || (wSizeY == 0)) {
            printc("--E-- Wrong capture size\r\n");
            return AHC_FALSE;
        }

        if ((wSizeX % 16) || (wSizeY % 8)) {
            printc("--W-- Capture encode size must Width 16x and Height 8x\r\n");
            wSizeX = (wSizeX + 15) & 0xFFF0;
            wSizeY = (wSizeY + 7) & 0xFFF8;
        }
        
	    for (wModeIndex = 0; wModeIndex < DSC_RESOL_MAX_NUM; wModeIndex++) {
	        ConfigWidth  = MMPS_DSC_GetConfig()->encParams.usDscEncW[wModeIndex];
	        ConfigHeight = MMPS_DSC_GetConfig()->encParams.usDscEncH[wModeIndex];
	        
	        if ((wSizeX * wSizeY) >= (ConfigWidth * ConfigHeight)) {
	            break;
	        }
	    }

        if (wModeIndex == DSC_RESOL_MAX_NUM) {
            printc("--E-- Capture size is not found !!!\r\n");
            wModeIndex = DSC_RESOL_MAX_NUM - 1;
        }

        m_wStillImageResIdx = DSC_RESOL_IDX_UNDEF;//wModeIndex;
        
        MMPS_DSC_GetPreviewStatus(&bInPreview);
        
        if (bInPreview) {

            if ((m_wStillImageWidth * wSizeY) != (wSizeX * m_wStillImageHeight)) {
                AHC_GetSystemStatus(&SysMode);
                
                SysMode = SysMode >> 16;
                
                AHC_SetMode(AHC_MODE_IDLE);
                
                bRestartPreview = MMP_TRUE;
            }
        }
            
        error = MMPS_DSC_SetJpegEncParam(m_wStillImageResIdx, wSizeX, wSizeY, MMP_DSC_JPEG_RC_ID_CAPTURE);

        if (error == MMP_ERR_NONE) {
            m_wStillImageWidth 	= wSizeX;
            m_wStillImageHeight = wSizeY;
        }
        
        if (bRestartPreview == MMP_TRUE) {
            AHC_SetMode(SysMode);
        }
        break;
    case STILL_SET_ONLY_MODE:
        m_wStillImageWidth 	= wSizeX;
        m_wStillImageHeight = wSizeY;
        break;
    case VIDEO_CAPTURE_MODE:
        if ((wSizeX % 16) || (wSizeY % 16))
            return AHC_FALSE;
        if ((wSizeX * wSizeY) < (MIN_VIDEO_ENC_WIDTH * MIN_VIDEO_ENC_HEIGHT))
            return AHC_FALSE;
        if ((wSizeX * wSizeY) > (MAX_VIDEO_ENC_WIDTH * MAX_VIDEO_ENC_HEIGHT))
            return AHC_FALSE;
        
        MMPS_3GPRECD_GetPreviewPipeStatus(gsAhcCurrentSensor, &bInPreview);
        
        if (bInPreview) {

            if (( m_VideoRecordWidth * wSizeY) != (wSizeX * m_VideoRecordHeight)) {
                AHC_GetSystemStatus(&SysMode);
                
                SysMode = SysMode >> 16;
                
                AHC_SetMode(AHC_MODE_IDLE);
                
                bRestartPreview = MMP_TRUE;
            }
        }
            
        m_VideoRecordWidth  = wSizeX;
        m_VideoRecordHeight = wSizeY;
            
        MMPS_3GPRECD_CustomedEncResol(0/*ubEncIdx*/, MMP_SCAL_FITMODE_OUT, wSizeX, wSizeY);
        
        if (bRestartPreview == MMP_TRUE) {
            AHC_SetMode(SysMode);
        }
        break;
    default:
        return AHC_FALSE;
        break;
    }
	return (error == MMP_ERR_NONE) ? AHC_TRUE : AHC_FALSE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_GetImageSize
//  Description : 
//------------------------------------------------------------------------------
/**
 @brief Get capture resolution

 This API gets the captured resolution in selected mode.
 Parameters:
 @param[in] byMode Which mode to be configured.
 @param[out] wSizeX Width.
 @param[out] wSizeY Height
 @retval AHC_TRUE Success.
*/
AHC_BOOL AHC_GetImageSize(UINT8 byMode, UINT32 *wSizeX, UINT32 *wSizeY)
{
    switch(byMode) {
    case STILL_CAPTURE_MODE:
        *wSizeX = m_wStillImageWidth;
        *wSizeY = m_wStillImageHeight;
        break;
    case VIDEO_CAPTURE_MODE:
        *wSizeX = m_VideoRecordWidth;
        *wSizeY = m_VideoRecordHeight;
        break;
    default:
        return AHC_FALSE;
        break;
    }

	return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_SetCustomQT
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Set QT mode

 This API configures the custom quantization table for JPEG compression.
 Parameters:
 @param[in] byFlag      Set or unset the specific custom QT: AHC_CUSTOMER_QT.
 @param[in] *pbyCusQT   A 8x8 quantization table for luma   Y component, which is after zig-zag scan order
 @param[in] *pbyCusQT   A 8x8 quantization table for chroma U component, which is after zig-zag scan order
 @param[in] *pbyCusQT   A 8x8 quantization table for chroma V component, which is after zig-zag scan order
 @retval AHC_TRUE Success.
*/
AHC_BOOL AHC_SetCustomQT(AHC_CUSTOMER_QT byFlag, UINT8 *pbyCusQT, UINT8 *pbyCusUQT, UINT8 *pbyCusVQT)
{
    MMP_ERR error = MMP_ERR_NONE;
    
    error = MMPS_DSC_SetCustomQTable(byFlag, pbyCusQT, pbyCusUQT, pbyCusVQT);
    
	return (error == MMP_ERR_NONE) ? AHC_TRUE : AHC_FALSE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_SetFaceDetection
//  Description :
//------------------------------------------------------------------------------
AHC_BOOL AHC_SetFaceDetection(UINT8 bMode)
{
#if (MENU_MANUAL_EN && MENU_MANUAL_FACE_DETECT_EN)
#if (MENU_MANUAL_FACE_DETECT_ON_EN)
	if (bMode == FACE_DETECT_ON) {
		if (!MMPS_Sensor_GetFDEnable())
        	MMPS_Sensor_SetFDEnable(AHC_TRUE);
    }
#endif
#if (MENU_MANUAL_FACE_DETECT_OFF_EN)    
    else if (bMode == FACE_DETECT_OFF){
    	if (MMPS_Sensor_GetFDEnable())
	        MMPS_Sensor_SetFDEnable(AHC_FALSE);
    }
#endif
#if (MENU_MANUAL_FACE_DETECT_TOUCHSPOT_EN)    
    else if (bMode == FACE_DETECT_TOUCHSPOT){
    	if (MMPS_Sensor_GetFDEnable())
    	    MMPS_Sensor_SetFDEnable(AHC_FALSE);
    }
#endif    
#endif
	return AHC_SUCCESS;
}

//------------------------------------------------------------------------------
//  Function    : AHC_GetCaptureCurZoomStep
//  Description :
//------------------------------------------------------------------------------
AHC_BOOL AHC_GetCaptureCurZoomStep(UINT16 *usZoomStepNum)
{
    if (!MMPS_DSC_GetCurZoomStep(usZoomStepNum))
	    return AHC_FALSE;
	else     
	    return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_GetCaptureMaxZoomStep
//  Description :
//------------------------------------------------------------------------------
AHC_BOOL AHC_GetCaptureMaxZoomStep(UINT16 *usMaxZoomStep)
{
     *usMaxZoomStep = gsDscPtzCfg.usMaxZoomSteps;

     return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_GetCaptureMaxZoomRatio
//  Description :
//------------------------------------------------------------------------------
AHC_BOOL AHC_GetCaptureMaxZoomRatio(UINT16 *usMaxZoomRatio)
{
     *usMaxZoomRatio = gsDscPtzCfg.usMaxZoomRatio;

     return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_GetCaptureCurZoomStatus
//  Description :
//------------------------------------------------------------------------------
MMP_UBYTE AHC_GetCaptureCurZoomStatus(void)
{
	return MMPS_DSC_GetCurZoomStatus();
}

//------------------------------------------------------------------------------
//  Function    : AHC_GetCameraDigitalZoomRatio
//  Description :
//------------------------------------------------------------------------------
AHC_BOOL AHC_GetCameraDigitalZoomRatio(UINT32 *usZoomRatio)
{
    UINT16 usMaxZoomRatio, usCurrentStep, usMaxZoomStep;	

    AHC_GetCaptureMaxZoomRatio(&usMaxZoomRatio);

    AHC_GetCaptureCurZoomStep(&usCurrentStep);
    AHC_GetCaptureMaxZoomStep(&usMaxZoomStep);

    *usZoomRatio = ((usMaxZoomRatio > 1) && (usMaxZoomStep > 0)) ? (100 + (usCurrentStep * (usMaxZoomRatio - 1) * 100) / usMaxZoomStep) : 100;
    return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_SetDBGDataAfterJPEG
//  Description :
//------------------------------------------------------------------------------
AHC_BOOL AHC_SetDBGDataAfterJPEG(UINT8 *ubAddress, UINT32 *ulLength)
{
    UINT16 ulMajorVer, ulMediumVer, ulMinorVer;

    //AHC_SetSensorDBGData(ubAddress, ulLength);

    MEMCPY((ubAddress+*ulLength), "MENU", sizeof("MENU"));
    *ulLength += sizeof("MENU");
    MEMCPY((ubAddress+*ulLength), MenuSettingConfig(), sizeof(MenuInfo));
    *ulLength += sizeof(MenuInfo);
    
    MEMCPY((ubAddress+*ulLength), "FwVer", sizeof("FwVer"));
    *ulLength += sizeof("FwVer");

    AHC_GetFwVersion(&ulMajorVer, &ulMediumVer, &ulMinorVer, NULL, NULL, NULL);

    MEMCPY((ubAddress+*ulLength), &ulMajorVer, sizeof(UINT16));
    *ulLength += sizeof(UINT16);
    MEMCPY((ubAddress+*ulLength), &ulMediumVer, sizeof(UINT16));
    *ulLength += sizeof(UINT16);
    MEMCPY((ubAddress+*ulLength), &ulMinorVer, sizeof(UINT16));
    *ulLength += sizeof(UINT16);

    return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AIHC_SetCapturePreviewMode
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Set Capture preview

 Set Capture preview

 @retval AHC_BOOL
*/
AHC_BOOL AIHC_SetCapturePreviewMode(AHC_BOOL bRawPreview, AHC_BOOL bMultiReturn)
{
    UINT16      wModeIndex;
    AHC_BOOL    bInPreview;
    UINT16      zoomIndex;
    AHC_DISPLAY_OUTPUTPANEL  OutputDevice;
    MMP_USHORT  usDscDispMode 	= 0;
    UINT32      ulFlickerMode 	= COMMON_FLICKER_50HZ;
    UINT8		ubEv			= COMMON_EVVALUE_00;
    MMP_ERR     sRet            = MMP_ERR_NONE;
    
    if (bMultiReturn == AHC_FALSE)
    {
        MMPS_DSC_GetPreviewStatus(&bInPreview);

        wModeIndex = AHC_GetStillImageResIdx();

        if (bInPreview) {
            if (wModeIndex == m_wCurrentStillImageMode) {
                return AHC_TRUE;
            }    
        }
		
		/* Disable Preview */
        MMPS_DSC_ResetPreviewMode();
        MMPS_DSC_SetPreviewMode(DSC_NORMAL_PREVIEW_MODE);
        MMPS_DSC_EnablePreviewDisplay(gsAhcCurrentSensor, MMP_FALSE, MMP_TRUE);

		#if (AHC_ENABLE_STILL_STICKER == 0)
        AHC_ConfigCapture(ACC_DATE_STAMP, AHC_ACC_TIMESTAMP_DISABLE);
		#endif
		
		/* Initial Display and Sensor */
        AHC_GetDisplayOutputDev(&OutputDevice);
        MMPS_Display_SetOutputPanel(MMP_DISPLAY_PRM_CTL, OutputDevice);

        MMPS_Sensor_Initialize(gsAhcCurrentSensor, AHC_GetPresetSensorMode(), MMP_SNR_DSC_PRW_MODE);

		/* Set Flicker Mode */
        if (AHC_Menu_SettingGetCB((char *)COMMON_KEY_FLICKER, &ulFlickerMode) == AHC_TRUE) {
            if (COMMON_FLICKER_60HZ == ulFlickerMode) {
                AHC_Set_SensorLightFreq(AHC_SENSOR_VIDEO_DEBAND_60HZ);
            }
            else {
                AHC_Set_SensorLightFreq(AHC_SENSOR_VIDEO_DEBAND_50HZ);
            }
        }
        else {
            AHC_Set_SensorLightFreq(AHC_SENSOR_VIDEO_DEBAND_60HZ);
        }

		/* Set EV value */
        if (AHC_Menu_SettingGetCB((char *)COMMON_KEY_EV, &ubEv) == AHC_TRUE) {
            Menu_SetEV(ubEv);
        }
        
        /* Set scale up capture enable */
        #if (SUPPORT_SCALEUP_CAPTURE)
        MMPS_DSC_EnableScaleUpCapture(MMP_TRUE); // Decided By UI
        #else
        MMPS_DSC_EnableScaleUpCapture(MMP_FALSE);
        #endif

		/* Initial Preview Parameter */
        if (bRawPreview)
            MMPS_DSC_SetPreviewPath(MMP_DSC_RAW_PREVIEW, MMP_RAW_COLORFMT_BAYER8);
        else
            MMPS_DSC_SetPreviewPath(MMP_DSC_STD_PREVIEW, MMP_RAW_COLORFMT_BAYER8);

        switch(OutputDevice){
        case MMP_DISPLAY_SEL_MAIN_LCD:
        case MMP_DISPLAY_SEL_SUB_LCD:
            usDscDispMode = DSC_NORMAL_PREVIEW_MODE;
            break;
        case MMP_DISPLAY_SEL_NTSC_TV:
            usDscDispMode = DSC_TV_NTSC_PREVIEW_MODE;
            break;
        case MMP_DISPLAY_SEL_PAL_TV:
            usDscDispMode = DSC_TV_PAL_PREVIEW_MODE;
            break;
        case MMP_DISPLAY_SEL_HDMI:
            usDscDispMode = DSC_HDMI_PREVIEW_MODE;
            break;
        case MMP_DISPLAY_SEL_CCIR:
            usDscDispMode = DSC_CCIR_PREVIEW_MODE;
            break;
        case MMP_DISPLAY_SEL_NONE:
        default:
            usDscDispMode = DSC_NORMAL_PREVIEW_MODE;
            printc("%s, %d not support yet!"__func__, __LINE__);
            break;
        }
				
        MMPS_DSC_SetPreviewMode(usDscDispMode);

		#if (SUPPORT_LDC_CAPTURE) //TBD
		MMPS_DSC_SetLdcResMode(MMP_LDC_RES_MODE_FHD, MMP_LDC_FPS_MODE_30P);
		MMPS_DSC_SetLdcRunMode(MMP_LDC_RUN_MODE_SINGLE_RUN);
		#endif

        m_wCurrentStillImageMode = wModeIndex;
        //To Check SNR Flip and set dir
        {
            AHC_BOOL ubSnrFlipEn; 
            ubSnrFlipEn = AHC_CheckSNRFlipEn(CTRL_BY_ALL) && AHC_GetSensorStatus();
            AHC_SetKitDirection(AHC_LCD_NOFLIP, AHC_FALSE, AHC_GetSensorStatus(), ubSnrFlipEn);
        }
        sRet = MMPS_DSC_InitPreview(gsAhcCurrentSensor);
        if (sRet != MMP_ERR_NONE) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 
        
        MMPS_DSC_GetCurZoomStep(&zoomIndex);
        MMPS_DSC_SetPreviewZoom(MMP_PTZ_ZOOMIN, zoomIndex);

        MMPS_DSC_EnablePreviewDisplay(gsAhcCurrentSensor, MMP_TRUE, MMP_FALSE);
    }
    else {
        UINT16 uwReviewTime;
        UINT16 uwWindowsTime;

        AHC_GetImageTime(&uwReviewTime, &uwWindowsTime);
        
        MMPS_DSC_RestorePreview(gsAhcCurrentSensor, uwWindowsTime);
    }

	#if (MENU_MANUAL_EN && MENU_MANUAL_FACE_DETECT_EN)
    if (MenuSettingConfig()->uiFaceTouch == FACE_DETECT_ON) {
        MMPS_Sensor_SetFDEnable(MMP_TRUE);
    }    
	#endif

    return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AIHC_StopCapturePreviewMode
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Stop capture preview

 Stop capture preview

 @retval AHC_BOOL
*/
AHC_BOOL AIHC_StopCapturePreviewMode(void)
{
    if (MMPS_Sensor_GetFDEnable()) {
        MMPS_Sensor_SetFDEnable(MMP_FALSE);
    }

    MMPS_DSC_EnablePreviewDisplay(gsAhcCurrentSensor, MMP_FALSE, MMP_FALSE);

	#if (SUPPORT_LDC_CAPTURE)
	MMPS_DSC_SetLdcResMode(MMP_LDC_RES_MODE_NOT_SUPPORT, MMP_LDC_FPS_MODE_NOT_SUPPORT);
	MMPS_DSC_SetLdcRunMode(MMP_LDC_RUN_MODE_DISABLE);
	#endif
	
	/* Release Buffer */
    MMPS_DSC_ExitMode();

    MMPS_Sensor_PowerDown(gsAhcCurrentSensor);

    return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AIHC_ConfigCaptureTimeStamp
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Config Capture time stamp

 Config Capture time stamp

 Parameters:

 @param[in]  TimeStampOp Time stamp configuration

 @retval TRUE or FALSE.
*/
AHC_BOOL AIHC_ConfigCaptureTimeStamp(UINT32 TimeStampOp)
{
    AHC_RTC_TIME            rtcTime;
    INT8                    Date[256];
    MMP_STICKER_ATTR        StickerBufAttr;
    //MMP_STICKER_ATTR      StickerBufAttr2;
    UINT32                  PrimaryBufAddr;
    UINT16                  PrimaryBufW, PrimaryBufH, PrimaryColorFmt;
    UINT32                  ThumbBufAddr;
    UINT16                  ThumbBufW, ThumbBufH, ThumbColorFmt;
    UINT16                  DateConfig;
    UINT16                  PositionConfig;
    UINT16                  FormatConfig;
    UINT32                  PrimaryJpegW, PrimaryJpegH;
    UINT32                  ThumbnailW, ThumbnailH;
	UINT32                  StickerX = 0, StickerY = 0;
    //UINT32                ThumbStickerX = 0, ThumbStickerY = 0;
    const GUI_FONT*         OldFont;

    if (AHC_FALSE == AIHC_GetOSDInitStatus()) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,0); return AHC_TRUE; }

    DateConfig     = TimeStampOp & (AHC_ACC_TIMESTAMP_DATE_MASK | AHC_ACC_TIMESTAMP_LOGOEX_MASK | AHC_ACC_TIMESTAMP_DATEEX_MASK);
    PositionConfig = TimeStampOp & AHC_ACC_TIMESTAMP_POSITION_MASK;
    FormatConfig   = TimeStampOp & AHC_ACC_TIMESTAMP_FORMAT_MASK;
	
    
	/* Get Jpeg/Thumbnail Attribute */
    AHC_GetCaptureConfig(ACC_CUS_THUMB_W, &ThumbnailW);
    AHC_GetCaptureConfig(ACC_CUS_THUMB_H, &ThumbnailH);
	
	AHC_GetImageSize(STILL_CAPTURE_MODE, &PrimaryJpegW, &PrimaryJpegH);
	
    BEGIN_LAYER(DATESTAMP_PRIMARY_OSD_ID);
    BEGIN_LAYER(DATESTAMP_THUMB_OSD_ID);
    
    AIHC_DrawReservedOSD(AHC_TRUE);
    	
	/* Get Jpeg/Thumbnail Sticker Attribute */
    AHC_OSDGetBufferAttr(   DATESTAMP_PRIMARY_OSD_ID,
                            &PrimaryBufW,
                            &PrimaryBufH,
                            &PrimaryColorFmt,
                            &PrimaryBufAddr);

    AHC_OSDGetBufferAttr(   DATESTAMP_THUMB_OSD_ID,
                            &ThumbBufW,
                            &ThumbBufH,
                            &ThumbColorFmt,
                            &ThumbBufAddr);

    // If no any video recrded before capture, the color of time stamp may be wrong in photo
    // So, always to load Color Palette.
    AHC_OSDLoadWinPalette(DATESTAMP_PRIMARY_OSD_ID);
	
	/* Transform time information to string */
    if (glAhcParameter.UseRtc != 0) {

        AHC_GetClock(   &rtcTime.uwYear,
                        &rtcTime.uwMonth,
                        &rtcTime.uwDay,
                        &rtcTime.uwDayInWeek,
                        &rtcTime.uwHour,
                        &rtcTime.uwMinute,
                        &rtcTime.uwSecond,
                        &rtcTime.ubAmOrPm,
                        &rtcTime.b_12FormatEn);
    }
    else {
        rtcTime.uwYear      = 2012;
        rtcTime.uwMonth     = 1;
        rtcTime.uwDay       = 1;
        rtcTime.uwHour      = 0;
        rtcTime.uwMinute    = 0;
        rtcTime.uwSecond    = 0;
    }

    MEMSET(Date, 0, sizeof(Date));

    AHC_TransferDateToString(&rtcTime, Date, DateConfig, FormatConfig);

    #ifdef ENABLE_GUI_SUPPORT_MULTITASK
    OldFont = AHC_OSDGetFont(DATESTAMP_PRIMARY_OSD_ID);
    AHC_OSDSetFont(DATESTAMP_PRIMARY_OSD_ID, &GUI_Font32B_1);
    #else
    OldFont = AHC_OSDGetFont();
    AHC_OSDSetFont(&GUI_Font32B_1);
    #endif

    if (PositionConfig == AHC_ACC_TIMESTAMP_BOTTOM_RIGHT) {
        StickerX    = PrimaryJpegW - PrimaryBufW;
        StickerY    = PrimaryJpegH - AHC_CAPTURE_STICKER_HEIGHT;
    }
    else if (PositionConfig == AHC_ACC_TIMESTAMP_BOTTOM_LEFT) {
        StickerX    = AHC_CAPTURE_STICKER_X_DEFAULT_POS;
        StickerY    = PrimaryJpegH - AHC_CAPTURE_STICKER_HEIGHT - AHC_CAPTURE_STICKER_Y_DELTA;
    }
    else if (PositionConfig == AHC_ACC_TIMESTAMP_TOP_RIGHT) {
        StickerX    = PrimaryJpegW  - PrimaryBufW;
        StickerY    = AHC_CAPTURE_STICKER_Y_DEFAULT_POS;
    }
    else if (PositionConfig == AHC_ACC_TIMESTAMP_TOP_LEFT) {
        StickerX    = AHC_CAPTURE_STICKER_X_DEFAULT_POS;
        StickerY    = AHC_CAPTURE_STICKER_Y_DEFAULT_POS;
    }

    #ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetBkColor(DATESTAMP_PRIMARY_OSD_ID, GUI_TRANSPARENT);
    AHC_OSDSetColor(DATESTAMP_PRIMARY_OSD_ID, GUI_BLACK);
    #else
    AHC_OSDSetBkColor(GUI_TRANSPARENT);
    AHC_OSDSetColor(GUI_BLACK);
    #endif
    
    AHC_OSDDrawFillRect(DATESTAMP_PRIMARY_OSD_ID, 0, 0, PrimaryBufW, PrimaryBufH);

    #ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetColor(DATESTAMP_PRIMARY_OSD_ID, GUI_WHITE);
    #else
    AHC_OSDSetColor(GUI_WHITE);
    #endif
    if(TimeStampOp & AHC_ACC_TIMESTAMP_DATEEX_MASK)
    {
    AHC_OSDDrawText(DATESTAMP_PRIMARY_OSD_ID, (UINT8*)Date, 0, 0, STRLEN(Date));
    }
    END_LAYER(DATESTAMP_PRIMARY_OSD_ID);
    END_LAYER(DATESTAMP_THUMB_OSD_ID);
	
	/* Transform GPS information to string */
    #if (AHC_GPS_STICKER_ENABLE && SUPPORT_GPS)
    if (AHC_GPS_Module_Attached())
    {
        UINT16      uiDegree = 0, uiMinute = 0;
        UINT16      tempX   = 0;
        UINT16      tempY   = 32;
        UINT32      uiOSDid = DATESTAMP_PRIMARY_OSD_ID;
        double      uiSec = 0;

        #if(GPS_MODULE == GPS_MODULE_NMEA0183)
        NMEAINFO   *pInfo;
        pInfo =(NMEAINFO *)GPS_Information();
        #elif(GPS_MODULE == GPS_MODULE_GMC1030)
        GPSRADARINFO   *pInfo;
        pInfo =(GPSRADARINFO *)GPSRadar_Information();
        #endif

        BEGIN_LAYER(uiOSDid);

        #ifdef ENABLE_GUI_SUPPORT_MULTITASK
        AHC_OSDSetFont(uiOSDid, &GUI_Font24B_1);
        #else
        AHC_OSDSetFont(&GUI_Font24B_1);
        #endif

        if (GPSCtrl_GPS_IsValidValue())
        {
            if (pInfo->dwLat>0)
            	AHC_OSDDrawText(uiOSDid,(UINT8 *)"N ", 0, tempY, sizeof("N "));
            else
                AHC_OSDDrawText(uiOSDid,(UINT8 *)"S ", 0, tempY, sizeof("S "));

            GPSCtrl_GPSFormatTrans(pInfo->dwLat, &uiDegree, &uiMinute, &uiSec);

          	tempX += 15;
         	AHC_OSDDrawText(uiOSDid,(UINT8 *)" ", tempX, tempY, sizeof(" "));
            AHC_OSDDrawFloat(uiOSDid, uiDegree, 2 );

            tempX += 30;
            AHC_UC_SetEncodeUTF8();
            AHC_OSDDrawText(uiOSDid,(UINT8 *)"\xB0", tempX, tempY ,sizeof("\xB0"));
            AHC_UC_SetEncodeNone();
            AHC_OSDDrawFloat(uiOSDid, uiMinute, 2 );

            tempX += 30;
          	AHC_OSDDrawText(uiOSDid,(UINT8 *)"'", tempX, tempY, sizeof("'"));
            AHC_OSDDrawFloat(uiOSDid, uiSec, 4 );

            tempX += 50;
        	AHC_OSDDrawText(uiOSDid,(UINT8 *)"''", tempX, tempY, sizeof("''"));

            if (pInfo->dwLon>0)
           		AHC_OSDDrawText(uiOSDid,(UINT8 *)"E ", 130+20, tempY, sizeof("E "));
            else
           		AHC_OSDDrawText(uiOSDid,(UINT8 *)"W ", 130+20, tempY, sizeof("W "));

            GPSCtrl_GPSFormatTrans(pInfo->dwLon, &uiDegree, &uiMinute, &uiSec);

            tempX = 150+20;
        	AHC_OSDDrawText(uiOSDid,(UINT8 *)" ", tempX, tempY, sizeof(" "));
            AHC_OSDDrawFloat(uiOSDid, uiDegree, 3 );

            tempX += 45;
            AHC_UC_SetEncodeUTF8();
            AHC_OSDDrawText(uiOSDid,(UINT8 *)"\xB0", tempX, tempY, sizeof("\xB0"));
            AHC_UC_SetEncodeNone();
            AHC_OSDDrawFloat(uiOSDid, uiMinute, 2 );

            tempX += 30;
       		AHC_OSDDrawText(uiOSDid,(UINT8 *)"'", tempX, tempY, sizeof("'"));
            AHC_OSDDrawFloat(uiOSDid, uiSec, 4 );

            tempX += 50;
            AHC_OSDDrawText(uiOSDid,(UINT8 *)"''",tempX,tempY,sizeof("''"));
        }
        else
        {
            AHC_UC_SetEncodeUTF8();
            AHC_OSDDrawText(uiOSDid, (UINT8*)"-  - -\xB0 - -' - - - -''   -  - -\xB0  - -' - - - -''", 0, 32, STRLEN("-  - -\xB0 - -' - - - -''   -  - -\xB0 - -' - - - -''"));
            AHC_UC_SetEncodeNone();
        }

        END_LAYER(uiOSDid);
    }
	#endif

	#if (STICKER_DRAW_EDGE)
    AIHC_DrawTextEdge(PrimaryBufAddr, PrimaryBufW, AHC_CAPTURE_STICKER_HEIGHT, 0xFF, 0x00, 0x01);
	#endif

    /* Set primary sticker */
    StickerBufAttr.ubStickerId	  	= MMP_STICKER_PIPE0_ID0;
    StickerBufAttr.ulBaseAddr       = PrimaryBufAddr;
    StickerBufAttr.usStartX         = StickerX;
    StickerBufAttr.usStartY         = StickerY;
    StickerBufAttr.usWidth          = PrimaryBufW;
    StickerBufAttr.usHeight         = AHC_CAPTURE_STICKER_HEIGHT;
    StickerBufAttr.colorformat      = MMP_ICON_COLOR_INDEX8;
    StickerBufAttr.ulTpColor        = GUI_BLACK;
    StickerBufAttr.bTpEnable        = MMP_TRUE;
    StickerBufAttr.bSemiTpEnable    = MMP_FALSE;
    StickerBufAttr.ubIconWeight     = 16;
    StickerBufAttr.ubDstWeight      = 0;

#if 0 //TBD
	BEGIN_LAYER(DATESTAMP_THUMB_OSD_ID);

    #ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetColor(DATESTAMP_THUMB_OSD_ID, GUI_BLACK);
    #else
    AHC_OSDSetColor(GUI_BLACK);
    #endif
    
    AHC_OSDDrawFillRect(DATESTAMP_THUMB_OSD_ID, 0, 0, ThumbBufW, ThumbBufH);
    
    #ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetColor(DATESTAMP_THUMB_OSD_ID, GUI_WHITE);
    AHC_OSDSetFont(DATESTAMP_THUMB_OSD_ID, &GUI_Font4x6);
    #else
    AHC_OSDSetColor(GUI_WHITE);
    AHC_OSDSetFont(&GUI_Font4x6);
    #endif

    if (PositionConfig == AHC_ACC_TIMESTAMP_BOTTOM_RIGHT) {
        StickerY = ThumbnailH - AHC_CAPTURE_STICKER_HEIGHT;
    }
    else if (PositionConfig == AHC_ACC_TIMESTAMP_BOTTOM_LEFT) {
        StickerY = ThumbnailH - AHC_CAPTURE_STICKER_HEIGHT;
    }
    else if (PositionConfig == AHC_ACC_TIMESTAMP_TOP_RIGHT) {
        StickerY = 0;
    }
    else if (PositionConfig == AHC_ACC_TIMESTAMP_TOP_LEFT) {
        StickerY = 0;
    }

    AHC_OSDDrawText(DATESTAMP_THUMB_OSD_ID, (UINT8*)Date, ThumbStickerX, 0, STRLEN(Date));

    ///Set sticker
    StickerBufAttr2.ubStickerId	  	 = MMP_STICKER_PIPE0_ID1;
    StickerBufAttr2.ulBaseAddr       = ThumbBufAddr;
    StickerBufAttr2.usStartX         = 0;
    StickerBufAttr2.usStartY         = ThumbStickerY;
    StickerBufAttr2.usWidth          = ThumbBufW;
    StickerBufAttr2.usHeight         = ThumbBufH;
    StickerBufAttr2.colorformat      = MMP_ICON_COLOR_INDEX8;
    StickerBufAttr2.ulTpColor        = GUI_BLACK;
    StickerBufAttr2.bTpEnable        = MMP_TRUE;
    StickerBufAttr2.bSemiTpEnable    = MMP_FALSE;
    StickerBufAttr2.ubIconWeight     = 16;
    StickerBufAttr2.ubDstWeight      = 0;

    #ifdef ENABLE_GUI_SUPPORT_MULTITASK
    AHC_OSDSetFont(DATESTAMP_THUMB_OSD_ID, OldFont);
    #else
    AHC_OSDSetFont(OldFont);
    #endif
	
	END_LAYER(DATESTAMP_THUMB_OSD_ID);
#endif
	
    AIHC_DrawReservedOSD(AHC_FALSE);

	MMPS_DSC_SetSticker(&StickerBufAttr, NULL);

    return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AIHC_SetCaptureMode
//  Description :
//------------------------------------------------------------------------------
/**
 @brief Start capture

 Start capture

 @param[in] uiMode        capture mode
 @retval AHC_BOOL
*/
AHC_BOOL AIHC_SetCaptureMode(UINT8 uiMode)
{
    MMP_ERR                 sRet = MMP_ERR_NONE;
    MMP_BYTE                bJpgFileName[MAX_FILE_NAME_SIZE];
    MMP_BYTE                bRawFileName[MAX_FILE_NAME_SIZE];
    MMP_BYTE                DirName[16];
    UINT16                  CaptureDirKey;
    UINT8                   CaptureFileName[32];
    UINT8                   CaptureRawName[32];
    MMPS_DSC_CAPTURE_INFO   captureinfo;
    MMP_ULONG              	CaptureWidth,CaptureHeight;
    UINT8                   MultiShotNum, MultiShotDelay;
    UINT32                  i;
    UINT8                   bCreateNewDir;
    UINT32                  ThumbMode,ThumbWidth,ThumbHeight;
    AHC_BOOL                bEnableThumbEncode  = AHC_FALSE;
    UINT32                  bSaveJPG            = AHC_TRUE;
    UINT32                  ulRawWidth,ulRawHeight;
    AHC_EXIF_CONFIG        	Exif;
    UINT32                  TimeStampOp;
    UINT16                  wReviewTime;
    UINT16                  wWindowsTime;
    UINT32 					ulJpgQuality = 0;
    AHC_BOOL                bRet;
    #if (ENABLE_DEBUG_AFTER_JPG)
    UINT32                  ulDBGbufferSize = 0;
    #endif
	#if (ENABLE_STROBE)
    AHC_BOOL                bFlashStatus;
	#endif

	#if (JPEG_INTO_AVI_WHILE_TURN_ON || PIR_JPEG_INTO_AVI_WHILE_TURN_ON)
    MMP_ULONG   			ulTestFileID;
    MMP_ULONG   			ulTestAviFileSize;
    MMP_ULONG   			ulTestAviFrameNum;
	#endif

    if (MMPS_Sensor_GetFDEnable()) {
        MMPS_Sensor_SetFDEnable(MMP_FALSE);
    }

	#if(ENALBE_MAX_PROTECT_TIME)
    AHC_SetClearCachePeriod(AHC_GetMediaPath(), AHC_STILL_MAX_PROTECT_TIME);
	#endif

	/* Strobe Setting */
	#if (ENABLE_STROBE)
    bFlashStatus = AHC_STROBE_GetStrobeStatus();

    if (bFlashStatus == AHC_TRUE) {
        // Calculated
        AHC_BOOL                bCalculated;
        UINT32                  ulCalculatedNumber;
        // Preflash
        AHC_BOOL                bPreFlash;
        UINT32                  ulPreFlashNumber;
        UINT32                  ulWaitPreFlashTime;		///<(us)
        UINT32                  ulPreFlashTime;			///<(us)
        UINT32                  ulTimeBetweenTwoFlash;	///<(us)
        // Main flash
        AHC_BOOL                bMainFlash;
        UINT32                  ulWaitMainFlashTime;	///<(us)
        UINT32                  ulMainFlashTime;		///<(us)

        if (AHC_STROBE_CheckChargeReady() == AHC_FALSE) {
            return AHC_FALSE;
		}
		
        // Calculated
        AHC_GetParam(PARAM_ID_STROBE_CALCULATED,             (UINT32 *)&bCalculated);
        AHC_GetParam(PARAM_ID_STROBE_CALCULATED_NUMBER,      &ulCalculatedNumber);
        // Preflash
        AHC_GetParam(PARAM_ID_STROBE_PREFLASH,               (UINT32 *)&bPreFlash);
        AHC_GetParam(PARAM_ID_STROBE_PREFLASH_NUMBER,        &ulPreFlashNumber);
        AHC_GetParam(PARAM_ID_STROBE_WAIT_PREFLASH_TIME,     &ulWaitPreFlashTime);
        AHC_GetParam(PARAM_ID_STROBE_PREFLASH_TIME,          &ulPreFlashTime);
        AHC_GetParam(PARAM_ID_STROBE_TIME_BETWEEN_TWO_FLASH, &ulTimeBetweenTwoFlash);
        // Main flash
        AHC_GetParam(PARAM_ID_STROBE_MAINFLASH,              (UINT32 *)&bMainFlash);
        AHC_GetParam(PARAM_ID_STROBE_WAIT_MAINFLASH_TIME,    &ulWaitMainFlashTime);
        AHC_GetParam(PARAM_ID_STROBE_MAINFLASH_TIME,         &ulMainFlashTime);

        MMPS_DSC_ConfigStrobe(  bCalculated,
                                ulCalculatedNumber,
                                bPreFlash,
                                ulPreFlashNumber,
                                ulWaitPreFlashTime,
                                ulPreFlashTime,
                                ulTimeBetweenTwoFlash,
                                bMainFlash,
                                ulWaitMainFlashTime,
                                ulMainFlashTime );
    }
    else {
        MMPS_DSC_ConfigStrobe(  0, 0,
                                0, 0, 0, 0, 0,
                                0, 0, 0 );
    }
	#endif
	
	/* Mechanical Shutter Setting */
	#if (ENABLE_MECHANICAL_SHUTTER)
    {
        AHC_BOOL bEnableShutter;
        UINT32   ulShutterTime;

        AHC_GetParam(PARAM_ID_MECHANICAL_SHUTTER,      (UINT32 *)&bEnableShutter);
        AHC_GetParam(PARAM_ID_MECHANICAL_SHUTTER_TIME, &ulShutterTime);
        
        MMPS_DSC_ConfigMechanicalShutter(bEnableShutter, ulShutterTime);
    }
	#endif

    AHC_GetImageTime(&wReviewTime, &wWindowsTime);
	
	/* Get Capture FileName */
	if (uiMode != AHC_MODE_CONTINUOUS_CAPTURE)
	{
        MEMSET(bRawFileName    	, 0, sizeof(bRawFileName));
        MEMSET(bJpgFileName    	, 0, sizeof(bJpgFileName));
        MEMSET(DirName         	, 0, sizeof(DirName));
        MEMSET(CaptureFileName 	, 0, sizeof(CaptureFileName));
        MEMSET(CaptureRawName	, 0, sizeof(CaptureRawName));

    	#if (DCF_FILE_NAME_TYPE == DCF_FILE_NAME_TYPE_NORMAL)
        bRet = AHC_UF_GetName(&CaptureDirKey, (INT8*)DirName, (INT8*)CaptureFileName, &bCreateNewDir);
        
        if (bRet == AHC_TRUE) {

            STRCPY(bJpgFileName, (INT8*)AHC_UF_GetRootDirName());
            STRCAT(bJpgFileName, "\\");
            STRCAT(bJpgFileName, DirName);

            if (bCreateNewDir) {
                MMPS_FS_DirCreate((INT8*)bJpgFileName, STRLEN(bJpgFileName));
                AHC_UF_AddDir(DirName);
            }

            STRCAT(bJpgFileName, "\\");
            STRCAT(bJpgFileName, (INT8*)CaptureFileName);

            if (uiMode == AHC_MODE_RAW_CAPTURE) {
            
                STRCPY((INT8*)CaptureRawName, (INT8*)CaptureFileName);

                STRCPY(bRawFileName, (INT8*)AHC_UF_GetRootDirName());
                STRCAT(bRawFileName, "\\");
                STRCAT(bRawFileName, DirName);
                STRCAT(bRawFileName, "\\");
                STRCAT(bRawFileName, (INT8*)CaptureFileName);
                STRCAT(bRawFileName, EXT_DOT);
                STRCAT(bRawFileName, "RAW");
            }
        }
    	#elif (DCF_FILE_NAME_TYPE == DCF_FILE_NAME_TYPE_DATE_TIME)
    	{
    	    AHC_RTC_TIME RtcTime;

    	    AHC_RTC_GetTime(&RtcTime);
    	    
    	    bRet = AHC_UF_GetName2(&RtcTime, (INT8*)bJpgFileName, (INT8*)CaptureFileName, &bCreateNewDir);

    	    if (uiMode == AHC_MODE_RAW_CAPTURE) {
    	        STRCPY(bRawFileName, (INT8*)bJpgFileName);
    	        STRCAT(bRawFileName, EXT_DOT);
    	        STRCAT(bRawFileName, "RAW");
    	    }
    	}
    	#endif
	}
    else {
        bRet = AHC_TRUE;
    }

    if (bRet == AHC_TRUE) 
    {
        if (uiMode != AHC_MODE_CONTINUOUS_CAPTURE)
        {
            STRCAT(bJpgFileName, EXT_DOT);

    		#if (JPEG_INTO_AVI_WHILE_TURN_ON)

            if (uiMode == AHC_MODE_LONG_TIME_CONTINUOUS_FIRST_CAPTURE)
                STRCAT(bJpgFileName, MOVIE_AVI_EXT);
            else
                STRCAT(bJpgFileName, PHOTO_JPG_EXT);

            if (uiMode == AHC_MODE_LONG_TIME_CONTINUOUS_NEXT_CAPTURE)
                STRCPY(bJpgFileName, AHC_InfoLog()->DCFCurStillFileName);
            else
                STRCPY(AHC_InfoLog()->DCFCurStillFileName, bJpgFileName);

    		#else //JPEG_INTO_AVI_WHILE_TURN_ON
    		
            STRCAT(bJpgFileName, PHOTO_JPG_EXT);
          	#if (USE_INFO_FILE)
        	STRCPY(AHC_InfoLog()->DCFCurStillFileName, bJpgFileName);
        	#endif
            
    		#endif
		}
	
		/* Set Thumbnail Parameter */
        AHC_GetCaptureConfig(ACC_THUMB_SIZE, 	&ThumbMode);
        AHC_GetCaptureConfig(ACC_CUS_THUMB_W, 	&ThumbWidth);
        AHC_GetCaptureConfig(ACC_CUS_THUMB_H,	&ThumbHeight);

        if ((uiMode == AHC_MODE_LONG_TIME_CONTINUOUS_FIRST_CAPTURE) || \
            (uiMode == AHC_MODE_LONG_TIME_CONTINUOUS_NEXT_CAPTURE )) {
            ThumbMode = THUMB_TYPE_DISABLE;
        }

        if (ThumbMode != THUMB_TYPE_DISABLE) {
            bEnableThumbEncode = AHC_TRUE;

            MMPS_DSC_ConfigThumbnail(ThumbWidth, ThumbHeight, MMP_DSC_THUMB_SRC_DECODED_JPEG);
        }
		
		/* Set TimeStamp */
        #if (SUPPORT_HDMI_OUT_FOCUS) // TBD
        if (AHC_Check2HdmiOutFocus()) {
            MenuSettingConfig()->uiDateTimeFormat = DATETIME_SETUP_NONE;
            MenuItemDateTimeFormatToAhcTimeStampSetting();
        }
        #endif

		AHC_GetCaptureConfig(ACC_DATE_STAMP, &TimeStampOp);
        
        if (TimeStampOp & AHC_ACC_TIMESTAMP_ENABLE_MASK)
        {
            MMP_ULONG 	ulScalInW, ulScalInH;
            MMP_ULONG 	ulPixelCount;
            MMP_USHORT 	maxImageSize;
            UINT32 		ulStillCapSize = 0;

            AHC_Menu_SettingGetCB((char *)COMMON_KEY_STILL_CAP_SIZE, &ulStillCapSize);

            MMPS_Sensor_GetCurCaptScalInputRes(gsAhcCurrentSensor, &ulScalInW, &ulScalInH);
            
            ulPixelCount = ulScalInW * ulScalInH;

            if (ulPixelCount > 14000000)
                maxImageSize = IMAGE_SIZE_14M;
            else if (ulPixelCount > 12000000)
                maxImageSize = IMAGE_SIZE_12M;
            else if (ulPixelCount > 8000000)
                maxImageSize = IMAGE_SIZE_8M;
            else if (ulPixelCount > 6000000)
                maxImageSize = IMAGE_SIZE_6M;
            else if (ulPixelCount > 5000000)
                maxImageSize = IMAGE_SIZE_5M;
            else if (ulPixelCount > 3000000)
                maxImageSize = IMAGE_SIZE_3M;
            else if (ulPixelCount > 2000000)
                maxImageSize = IMAGE_SIZE_2M;
            else if (ulPixelCount > 1200000)
                maxImageSize = IMAGE_SIZE_1_2M;
            else
                maxImageSize = IMAGE_SIZE_VGA;

            if (0)//(ZoomCtrl_GetStillDigitalZoomLevel() || (ulStillCapSize < maxImageSize))
                printc("Not Support Zoom or exceed sensor resolutation image size with TimeStamp\r\n");
            else
                AIHC_ConfigCaptureTimeStamp(TimeStampOp);
        }
		
		/* Set JPEG Quality */
        AHC_Menu_SettingGetCB(COMMON_KEY_STILL_QUALITY, &ulJpgQuality);

        MMPS_DSC_SetCaptureJpegQuality(MMP_DSC_JPEG_RC_ID_CAPTURE, MMP_TRUE, MMP_FALSE, 
                                       AHC_GetStillImageTargetSize() / 1024, 0, 3, ulJpgQuality);

		#if (ENABLE_STROBE)
        if (bFlashStatus) {
            AHC_STROBE_EnableStrobeStatus(AHC_FALSE);
        }
		#endif
		
		/* Take JPEG */
        if (uiMode == AHC_MODE_DRAFT_CAPTURE) {

            MMPS_Sensor_GetCurPrevScalInputRes(gsAhcCurrentSensor, &CaptureWidth, &CaptureHeight);
            
            MMPS_DSC_SetJpegEncParam(AHC_GetStillImageResIdx(), CaptureWidth, CaptureHeight, MMP_DSC_JPEG_RC_ID_CAPTURE);
            MMPS_DSC_SetShotMode(MMPS_DSC_SINGLE_SHOTMODE);
            sRet = MMPS_DSC_SetCaptureBuf(NULL, NULL, MMP_FALSE, MMP_TRUE, MMP_DSC_CAPTURE_NO_ROTATE, MMP_TRUE);
            if (sRet != MMP_ERR_NONE) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 
            
            for (i=0;;i++) {
                if (AHC_TRUE == AHC_GetImageEXIF(i, &Exif)) {
                    MMPS_DSC_UpdateExifBuffer(EXIF_NODE_ID_PRIMARY, 
                    						  Exif.IfdID, Exif.TagID, 
                    						  Exif.TagType, Exif.TagDataSize, 
                    						  (MMP_UBYTE*)Exif.Data, Exif.DataSize, MMP_FALSE);
                }
                else {
                    break;
                }
            }

            captureinfo.ubFilename          = NULL;
            captureinfo.usFilenamelen       = 0;
            captureinfo.bExif               = MMP_TRUE;
            captureinfo.bThumbnail          = bEnableThumbEncode;
            captureinfo.bFirstShot          = MMP_TRUE;
            captureinfo.usReviewTime        = wReviewTime;
            captureinfo.usWindowsTime       = wWindowsTime;

            sRet = MMPS_DSC_CaptureTakeJpeg(gsAhcCurrentSensor, &captureinfo);
            if (sRet != MMP_ERR_NONE) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 
            
            captureinfo.ubFilename      = bJpgFileName;
            captureinfo.usFilenamelen   = STRLEN(bJpgFileName);
            captureinfo.bFirstShot      = MMP_TRUE;
            captureinfo.ulExtraBufAddr  = 0;

            sRet = MMPS_DSC_JpegDram2Card(&captureinfo);
            if (sRet != MMP_ERR_NONE) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 

            STRCAT((INT8*)CaptureFileName, EXT_DOT);
            STRCAT((INT8*)CaptureFileName, PHOTO_JPG_EXT);

            AHC_UF_PreAddFile(CaptureDirKey, (INT8*)CaptureFileName);
            AHC_UF_PostAddFile(CaptureDirKey, (INT8*)CaptureFileName);

            sRet = MMPS_DSC_SetCaptureBuf(NULL, NULL, MMP_FALSE, MMP_TRUE, MMP_DSC_CAPTURE_NO_ROTATE, MMP_FALSE);
            if (sRet != MMP_ERR_NONE) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 

            if (TimeStampOp & AHC_ACC_TIMESTAMP_ENABLE_MASK) {
                MMPS_DSC_SetSticker(NULL, NULL);
            }
        }
        else if (uiMode == AHC_MODE_STILL_CAPTURE) {
        
            AHC_GetImageSize(STILL_CAPTURE_MODE, &CaptureWidth, &CaptureHeight);

            #if (DSC_SUPPORT_BASELINE_MP_FILE)
			MMPS_DSC_EnableMultiPicFormat(MMP_TRUE, MMP_TRUE);
			#endif

            MMPS_DSC_SetJpegEncParam(AHC_GetStillImageResIdx(), CaptureWidth, CaptureHeight, MMP_DSC_JPEG_RC_ID_CAPTURE);
            MMPS_DSC_SetShotMode(MMPS_DSC_SINGLE_SHOTMODE);
            sRet = MMPS_DSC_SetCaptureBuf(NULL, NULL, MMP_FALSE, MMP_TRUE, MMP_DSC_CAPTURE_NO_ROTATE, MMP_TRUE);
            if (sRet != MMP_ERR_NONE) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 

            for (i=0;;i++) {
                if (AHC_TRUE == AHC_GetImageEXIF(i, &Exif)) {
                    MMPS_DSC_UpdateExifBuffer(EXIF_NODE_ID_PRIMARY, 
                    						  Exif.IfdID, Exif.TagID, 
                    						  Exif.TagType, Exif.TagDataSize, 
                    						  (MMP_UBYTE*)Exif.Data, Exif.DataSize, MMP_FALSE);
                }
                else {
                    break;
                }
            }
            
            captureinfo.ubFilename          = NULL;
            captureinfo.usFilenamelen       = 0;
            #if (SUPPORT_LDC_CAPTURE)
            captureinfo.bExif               = MMP_FALSE;
            captureinfo.bThumbnail          = MMP_FALSE;
            #else
            captureinfo.bExif               = MMP_TRUE;
            captureinfo.bThumbnail          = MMP_TRUE;
            #endif
            captureinfo.bFirstShot          = MMP_TRUE;
            captureinfo.usReviewTime		= wReviewTime;
            captureinfo.usWindowsTime		= wWindowsTime;

            sRet = MMPS_DSC_CaptureTakeJpeg(gsAhcCurrentSensor, &captureinfo);
            if (sRet != MMP_ERR_NONE) { AHC_PRINT_RET_ERROR(0,sRet); return AHC_FALSE;}  
            
#if (ENABLE_DEBUG_AFTER_JPG)
            AHC_SetDBGDataAfterJPEG(m_ubDebugMessageBuffer, &ulDBGbufferSize);
#endif
            
            captureinfo.ubFilename      = bJpgFileName;
            captureinfo.usFilenamelen   = STRLEN(bJpgFileName);
            captureinfo.bFirstShot      = MMP_TRUE;
#if (ENABLE_DEBUG_AFTER_JPG)
            captureinfo.ulExtraBufAddr	= (UINT32)m_ubDebugMessageBuffer;
            captureinfo.ulExtraBufSize  = ulDBGbufferSize;
#else
            captureinfo.ulExtraBufAddr	= 0;
            captureinfo.ulExtraBufSize  = 0;
#endif
            printc("JPG filename:%s,%d\r\n",captureinfo.ubFilename,captureinfo.ulJpegFileSize);
                
            sRet = MMPS_DSC_JpegDram2Card(&captureinfo);
            if (sRet != MMP_ERR_NONE) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 

            STRCAT((INT8*)CaptureFileName, EXT_DOT);
            STRCAT((INT8*)CaptureFileName, PHOTO_JPG_EXT);

            AHC_UF_PreAddFile(CaptureDirKey, (INT8*)CaptureFileName);
            AHC_UF_PostAddFile(CaptureDirKey, (INT8*)CaptureFileName);

#if (ENABLE_AUTO_LOCKFILE_JPG)
            AHC_FS_FileDirSetAttribute((INT8*)bJpgFileName, STRLEN(bJpgFileName), AHC_FS_ATTR_READ_ONLY);
#endif

            sRet = MMPS_DSC_SetCaptureBuf(NULL, NULL, MMP_FALSE, MMP_TRUE, MMP_DSC_CAPTURE_NO_ROTATE, MMP_FALSE);
            if (sRet != MMP_ERR_NONE) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 
    		
    		if (TimeStampOp & AHC_ACC_TIMESTAMP_ENABLE_MASK) {
    		    MMPS_DSC_SetSticker(NULL, NULL);
    		}
    	}
    	else if (uiMode == AHC_MODE_RAW_CAPTURE) {
    	
    	    AHC_GetCaptureConfig(ACC_CAPTURE_RAW_WITH_JPEG, &bSaveJPG);
    	    
    	    sRet = MMP_ERR_NONE;
    	    
    	    if (bSaveJPG) {
    	    
        	    AHC_GetImageSize(STILL_CAPTURE_MODE, &CaptureWidth, &CaptureHeight);
        	    
        	    MMPS_DSC_SetJpegEncParam(AHC_GetStillImageResIdx(), CaptureWidth, CaptureHeight, MMP_DSC_JPEG_RC_ID_CAPTURE);
        	    MMPS_DSC_SetShotMode(MMPS_DSC_SINGLE_SHOTMODE);
        	    sRet = MMPS_DSC_SetCaptureBuf(NULL, NULL, MMP_FALSE, MMP_TRUE, MMP_DSC_CAPTURE_NO_ROTATE, MMP_TRUE);
        	    
        	    for (i=0;;i++) {
                    if (AHC_TRUE == AHC_GetImageEXIF(i, &Exif)) {
                        MMPS_DSC_UpdateExifBuffer(EXIF_NODE_ID_PRIMARY, 
                        						  Exif.IfdID, Exif.TagID, 
                        						  Exif.TagType, Exif.TagDataSize, 
                        						  (MMP_UBYTE*)Exif.Data, Exif.DataSize, MMP_FALSE);
                    }
                    else {
                        break;
                    }
                }
            
                captureinfo.ubFilename          = NULL;
                captureinfo.usFilenamelen       = 0;
                captureinfo.bExif               = MMP_TRUE;
                captureinfo.bThumbnail          = bEnableThumbEncode;
        		captureinfo.bFirstShot          = MMP_TRUE;
				captureinfo.usReviewTime		= wReviewTime;
				captureinfo.usWindowsTime		= wWindowsTime;
            
                sRet = MMPS_DSC_CaptureTakeJpeg(gsAhcCurrentSensor, &captureinfo);
                if (sRet != MMP_ERR_NONE) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 
                
#if (ENABLE_DEBUG_AFTER_JPG)
                AHC_SetDBGDataAfterJPEG(m_ubDebugMessageBuffer, &ulDBGbufferSize);
#endif

                captureinfo.ubFilename      = bJpgFileName;
                captureinfo.usFilenamelen   = STRLEN(bJpgFileName);
                captureinfo.bFirstShot      = MMP_TRUE;
#if (ENABLE_DEBUG_AFTER_JPG)
                captureinfo.ulExtraBufAddr  = (UINT32)m_ubDebugMessageBuffer;
                captureinfo.ulExtraBufSize  = ulDBGbufferSize;
#else
                captureinfo.ulExtraBufAddr  = 0;
                captureinfo.ulExtraBufSize  = 0;
#endif
				
                sRet = MMPS_DSC_JpegDram2Card(&captureinfo);
                if (sRet != MMP_ERR_NONE) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 

                STRCAT((INT8*)CaptureFileName, EXT_DOT);
                STRCAT((INT8*)CaptureFileName, PHOTO_JPG_EXT);

                AHC_UF_PreAddFile(CaptureDirKey, (INT8*)CaptureFileName);
                AHC_UF_PostAddFile(CaptureDirKey, (INT8*)CaptureFileName);
            }

            sRet = MMPS_DSC_GetRawData(gsAhcCurrentSensor, 0, ((bSaveJPG) ? AHC_FALSE : AHC_TRUE), bRawFileName, STRLEN(bRawFileName), &ulRawWidth, &ulRawHeight);
            if (sRet != MMP_ERR_NONE) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 

            STRCAT((INT8*)CaptureRawName, EXT_DOT);
            STRCAT((INT8*)CaptureRawName,"RAW");

            AHC_UF_PreAddFile(CaptureDirKey, (INT8*)CaptureRawName);
            AHC_UF_PostAddFile(CaptureDirKey, (INT8*)CaptureRawName);

            if (bSaveJPG) {
                sRet = MMPS_DSC_SetCaptureBuf(NULL, NULL, MMP_FALSE, MMP_TRUE, MMP_DSC_CAPTURE_NO_ROTATE, MMP_FALSE);
                if (sRet != MMP_ERR_NONE) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 
            }

            if (TimeStampOp & AHC_ACC_TIMESTAMP_ENABLE_MASK) {
                MMPS_DSC_SetSticker(NULL, NULL);
            }
        }
        else if(uiMode == AHC_MODE_CONTINUOUS_CAPTURE) {

            AHC_GetImageSize(STILL_CAPTURE_MODE, &CaptureWidth, &CaptureHeight);

#if (DSC_SUPPORT_BASELINE_MP_FILE)
            MMPS_DSC_EnableMultiPicFormat(MMP_FALSE, MMP_FALSE);
#endif

            MMPS_DSC_SetContiShotMaxNum(m_wContiShotMaxNum);
            MMPS_DSC_SetContiShotInterval(m_wContiShotInterval);

            /* Set Max Dram Address boundary for conti-shot */
            MMPS_DSC_SetContiShotMaxDramAddr(AHC_GUI_TEMP_BASE_ADDR);

            MMPS_DSC_SetJpegEncParam(DSC_RESOL_IDX_UNDEF, CaptureWidth, CaptureHeight, MMP_DSC_JPEG_RC_ID_CAPTURE); 
            MMPS_DSC_SetShotMode(MMPS_DSC_CONTINUOUS_SHOTMODE);
            sRet = MMPS_DSC_SetCaptureBuf(NULL, NULL, MMP_FALSE, MMP_FALSE, MMP_DSC_CAPTURE_NO_ROTATE, MMP_TRUE);
            if (sRet != MMP_ERR_NONE) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;} 

            for (i=0;;i++) {
                if (AHC_TRUE == AHC_GetImageEXIF(i, &Exif)) {
                    MMPS_DSC_UpdateExifBuffer(EXIF_NODE_ID_PRIMARY, 
                                              Exif.IfdID, Exif.TagID, 
                                              Exif.TagType, Exif.TagDataSize, 
                                              (MMP_UBYTE*)Exif.Data, Exif.DataSize, MMP_FALSE);
                }
                else {
                    break;
                }
            }
            
            captureinfo.ubFilename          = NULL;
            captureinfo.usFilenamelen       = 0;
            #if (SUPPORT_LDC_CAPTURE)
            captureinfo.bExif               = MMP_FALSE;
            captureinfo.bThumbnail          = MMP_FALSE;
            #else
            captureinfo.bExif               = MMP_FALSE;
            captureinfo.bThumbnail          = MMP_FALSE;
            #endif
    		captureinfo.bFirstShot          = MMP_TRUE;
			captureinfo.usReviewTime		= 0;
			captureinfo.usWindowsTime		= 0;
			captureinfo.bContiShotEnable 	= MMP_TRUE;

            sRet = MMPS_DSC_CaptureTakeJpeg(gsAhcCurrentSensor, &captureinfo);
            if (sRet != MMP_ERR_NONE) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;}             
        }
        else if (uiMode == AHC_MODE_SEQUENTIAL_CAPTURE) {

            AHC_GetImageSize(STILL_CAPTURE_MODE, &CaptureWidth, &CaptureHeight);
            AHC_GetSeqCaptureParam(&MultiShotNum, &MultiShotDelay);

            #if (DSC_SUPPORT_BASELINE_MP_FILE)
			MMPS_DSC_EnableMultiPicFormat(MMP_TRUE, MMP_TRUE);
			#endif

            MMPS_DSC_SetShotMode(MMPS_DSC_MULTI_SHOTMODE);
            MMPS_DSC_SetJpegEncParam(AHC_GetStillImageResIdx(), CaptureWidth, CaptureHeight, MMP_DSC_JPEG_RC_ID_CAPTURE);
            sRet = MMPS_DSC_SetCaptureBuf(NULL, NULL, MMP_FALSE, MMP_TRUE, MMP_DSC_CAPTURE_NO_ROTATE, MMP_TRUE);
            if (sRet != MMP_ERR_NONE) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;}             

            for (i=0;;i++) {
                if (AHC_TRUE == AHC_GetImageEXIF(i, &Exif)) {
                    MMPS_DSC_UpdateExifBuffer(EXIF_NODE_ID_PRIMARY, 
                    						  Exif.IfdID, Exif.TagID, 
                    						  Exif.TagType, Exif.TagDataSize, 
                    						  (MMP_UBYTE*)Exif.Data, Exif.DataSize, MMP_FALSE);
                }
                else {
                    break;
                }
            }

            captureinfo.usJpegBufAddr       = NULL;
            captureinfo.ubFilename          = NULL;
            captureinfo.usFilenamelen       = 0;
            #if (SUPPORT_LDC_CAPTURE)
            captureinfo.bExif               = MMP_FALSE;
            captureinfo.bThumbnail          = MMP_FALSE;
            #else
            captureinfo.bExif               = MMP_TRUE;
            captureinfo.bThumbnail          = MMP_TRUE;
            #endif
            captureinfo.bFirstShot          = MMP_TRUE;
            captureinfo.usReviewTime        = wReviewTime;
            captureinfo.usWindowsTime       = wWindowsTime;

            sRet = MMPS_DSC_CaptureTakeJpeg(gsAhcCurrentSensor, &captureinfo);
            if (sRet != MMP_ERR_NONE) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;}             

            captureinfo.bFirstShot          = MMP_FALSE;

            for (i = 1; i < MultiShotNum; i++) {
                if (MultiShotDelay) {
                    AHC_OS_Sleep(MultiShotDelay);
				}
                MMPS_DSC_SetCaptureJpegQuality(MMP_DSC_JPEG_RC_ID_CAPTURE, MMP_TRUE, MMP_FALSE, 
                                               AHC_GetStillImageTargetSize() / 1024, 0, 3, ulJpgQuality);
                
                MMPS_DSC_CaptureTakeJpeg(gsAhcCurrentSensor, &captureinfo);
            }

            for (i = 0; i < MultiShotNum; i++) {
                captureinfo.ubFilename      = bJpgFileName;
                captureinfo.usFilenamelen   = STRLEN(bJpgFileName);
                captureinfo.bFirstShot      = (i == 0) ? MMP_TRUE : MMP_FALSE;
                captureinfo.ulExtraBufAddr  = 0;

                sRet = MMPS_DSC_JpegDram2Card(&captureinfo);
                if (sRet != MMP_ERR_NONE) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;}             

                STRCAT((INT8*)CaptureFileName, EXT_DOT);
                STRCAT((INT8*)CaptureFileName, PHOTO_JPG_EXT);

                AHC_UF_PreAddFile(CaptureDirKey, (INT8*)CaptureFileName);
                AHC_UF_PostAddFile(CaptureDirKey, (INT8*)CaptureFileName);

                MEMSET(bJpgFileName 	, 0, sizeof(bJpgFileName));
                MEMSET(DirName         	, 0, sizeof(DirName));
                MEMSET(CaptureFileName 	, 0, sizeof(CaptureFileName));

                if (AHC_UF_GetName(&CaptureDirKey, (INT8*)DirName, (INT8*)CaptureFileName, &bCreateNewDir) == AHC_TRUE) {
                    
                    STRCPY(bJpgFileName, (INT8*)AHC_UF_GetRootDirName());
                    STRCAT(bJpgFileName, "\\");
                    STRCAT(bJpgFileName, DirName);

                    if (bCreateNewDir) {
                        MMPS_FS_DirCreate((INT8*)bJpgFileName, STRLEN(bJpgFileName));
                        AHC_UF_AddDir(DirName);
                    }

                    STRCAT(bJpgFileName, "\\");
                    STRCAT(bJpgFileName, (INT8*)CaptureFileName);
                    STRCAT(bJpgFileName, EXT_DOT);
                    STRCAT(bJpgFileName, PHOTO_JPG_EXT);
                }
            }

            sRet = MMPS_DSC_SetCaptureBuf(NULL, NULL, MMP_FALSE, MMP_TRUE, MMP_DSC_CAPTURE_NO_ROTATE, MMP_FALSE);
            if (sRet != MMP_ERR_NONE) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;}             

            if (TimeStampOp & AHC_ACC_TIMESTAMP_ENABLE_MASK) {
                MMPS_DSC_SetSticker(NULL, NULL);
            }
        }
        else if (uiMode == AHC_MODE_LONG_TIME_CONTINUOUS_FIRST_CAPTURE) {

            AHC_GetImageSize(STILL_CAPTURE_MODE,&CaptureWidth,&CaptureHeight);
            AHC_GetSeqCaptureParam(&MultiShotNum, &MultiShotDelay);

            MMPS_DSC_SetShotMode(MMPS_DSC_MULTI_SHOTMODE);
            MMPS_DSC_SetJpegEncParam(AHC_GetStillImageResIdx(), CaptureWidth, CaptureHeight, MMP_DSC_JPEG_RC_ID_CAPTURE);
            sRet = MMPS_DSC_SetCaptureBuf(NULL, NULL, MMP_FALSE, MMP_TRUE, MMP_DSC_CAPTURE_NO_ROTATE, MMP_TRUE);
            if (sRet != MMP_ERR_NONE) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;}             

			#if 0
            for (i=0;;i++) {
                if (AHC_TRUE == AHC_GetImageEXIF(i, &Exif)) {
                    MMPS_DSC_UpdateExifBuffer(EXIF_NODE_ID_PRIMARY, 
                    						  Exif.IfdID, Exif.TagID, 
                    						  Exif.TagType, Exif.TagDataSize, 
                    						  (MMP_UBYTE*)Exif.Data, Exif.DataSize, MMP_FALSE);
                }
                else {
                    break;
                }
            }
			#endif

            MMPS_DSC_SetCaptureJpegQuality(MMP_DSC_JPEG_RC_ID_CAPTURE, MMP_FALSE, MMP_FALSE, 
                                           0, 0, 3, MMP_DSC_CAPTURE_HIGH_QUALITY);

            captureinfo.usJpegBufAddr   = NULL;
            captureinfo.ubFilename      = NULL;
            captureinfo.usFilenamelen   = 0;
            captureinfo.bExif           = MMP_FALSE;
            captureinfo.bThumbnail      = MMP_FALSE;
            captureinfo.bFirstShot      = MMP_TRUE;
            captureinfo.usReviewTime    = 0;
            captureinfo.usWindowsTime   = 0;

            sRet = MMPS_DSC_CaptureTakeJpeg(gsAhcCurrentSensor, &captureinfo);
            if (sRet != MMP_ERR_NONE) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;}             
            
			#if (PIR_JPEG_INTO_AVI_WHILE_TURN_ON)
			
            MMPS_3GPRECD_InitAVIFile((MMP_BYTE*)bJpgFileName, STRLEN(bJpgFileName), CaptureWidth, CaptureHeight,
                                     0, 1*1000, (captureinfo.ulJpegfilesize * 1 * 8), MMP_TRUE, &ulTestFileID);
            
            sRet = MMPS_3GPRECD_AVIAppendFrame(ulTestFileID, (MMP_UBYTE *)captureinfo.ulJpegfileaddr, captureinfo.ulJpegfilesize, &ulTestAviFileSize, &ulTestAviFrameNum);
            if (sRet != MMP_ERR_NONE) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;}             
            
            MMPS_FS_FileClose(ulTestFileID);

            STRCAT((INT8*)CaptureFileName, EXT_DOT);
            STRCAT((INT8*)CaptureFileName, MOVIE_AVI_EXT);

            AHC_UF_PreAddFile(CaptureDirKey, (INT8*)CaptureFileName);
            AHC_UF_PostAddFile(CaptureDirKey, (INT8*)CaptureFileName);
            
			#else
            
            captureinfo.ubFilename      = bJpgFileName;
            captureinfo.usFilenamelen   = STRLEN(bJpgFileName);
            captureinfo.bFirstShot      = MMP_TRUE;
            captureinfo.ulExtraBufAddr  = 0;
            captureinfo.ulExtraBufSize  = 0;

            sRet = MMPS_DSC_JpegDram2Card(&captureinfo);
            if (sRet != MMP_ERR_NONE) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;}

            STRCAT((INT8*)CaptureFileName, EXT_DOT);
            STRCAT((INT8*)CaptureFileName, PHOTO_JPG_EXT);

            AHC_UF_PreAddFile(CaptureDirKey, (INT8*)CaptureFileName);
            AHC_UF_PostAddFile(CaptureDirKey, (INT8*)CaptureFileName);
			#endif

            sRet = MMPS_DSC_SetCaptureBuf(NULL, NULL, MMP_FALSE, MMP_TRUE, MMP_DSC_CAPTURE_NO_ROTATE, MMP_FALSE);
            if (sRet != MMP_ERR_NONE) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;}

            if (TimeStampOp & AHC_ACC_TIMESTAMP_ENABLE_MASK) {
                MMPS_DSC_SetSticker(NULL, NULL);
            }
        }
        else if (uiMode == AHC_MODE_LONG_TIME_CONTINUOUS_NEXT_CAPTURE) {

            AHC_GetImageSize(STILL_CAPTURE_MODE, &CaptureWidth, &CaptureHeight);
            AHC_GetSeqCaptureParam(&MultiShotNum, &MultiShotDelay);

            MMPS_DSC_SetShotMode(MMPS_DSC_MULTI_SHOTMODE);
            MMPS_DSC_SetJpegEncParam(AHC_GetStillImageResIdx(), CaptureWidth, CaptureHeight, MMP_DSC_JPEG_RC_ID_CAPTURE);
            sRet = MMPS_DSC_SetCaptureBuf(NULL, NULL, MMP_FALSE, MMP_TRUE, MMP_DSC_CAPTURE_NO_ROTATE, MMP_TRUE);
            if (sRet != MMP_ERR_NONE) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;}

			#if 0
            for (i=0;;i++) {
                if (AHC_TRUE == AHC_GetImageEXIF(i, &Exif)) {
                    MMPS_DSC_UpdateExifBuffer(EXIF_NODE_ID_PRIMARY, 
                    						  Exif.IfdID, Exif.TagID, 
                    						  Exif.TagType, Exif.TagDataSize, 
                    						  (MMP_UBYTE*)Exif.Data, Exif.DataSize, MMP_FALSE);
                }
                else {
                    break;
                }
            }
			#endif
            
            sRet = MMPS_DSC_SetCaptureJpegQuality(MMP_DSC_JPEG_RC_ID_CAPTURE, MMP_FALSE, MMP_FALSE, 
                                                  0, 0, 3, MMP_DSC_CAPTURE_HIGH_QUALITY);
            if (sRet != MMP_ERR_NONE) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;}

            captureinfo.usJpegBufAddr       = NULL;
            captureinfo.ubFilename          = NULL;
            captureinfo.usFilenamelen       = 0;
            captureinfo.bExif               = MMP_FALSE;
            captureinfo.bThumbnail          = MMP_FALSE;
            captureinfo.bFirstShot          = MMP_FALSE;
            captureinfo.usReviewTime        = 0;
            captureinfo.usWindowsTime       = 0;

            sRet = MMPS_DSC_CaptureTakeJpeg(gsAhcCurrentSensor, &captureinfo);
            if (sRet != MMP_ERR_NONE) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;}             

			#if (PIR_JPEG_INTO_AVI_WHILE_TURN_ON)

            MMPS_3GPRECD_InitAVIFile((MMP_BYTE *)bJpgFileName, STRLEN(bJpgFileName), CaptureWidth, CaptureHeight,
                                     0, 5*1000, (captureinfo.ulJpegfilesize * 5 * 8), MMP_FALSE, &ulTestFileID);
            
            sRet = MMPS_3GPRECD_AVIAppendFrame(ulTestFileID, (MMP_UBYTE *)captureinfo.ulJpegfileaddr, captureinfo.ulJpegfilesize, &ulTestAviFileSize, &ulTestAviFrameNum);
            if (sRet != MMP_ERR_NONE) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;}             

            MMPS_FS_FileClose(ulTestFileID);

			#else
			
            captureinfo.ubFilename      = bJpgFileName;
            captureinfo.usFilenamelen   = STRLEN(bJpgFileName);
            captureinfo.bFirstShot      = MMP_FALSE;
            captureinfo.ulExtraBufAddr  = 0;
            captureinfo.ulExtraBufSize  = 0;

            sRet = MMPS_DSC_JpegDram2Card(&captureinfo);
            if (sRet != MMP_ERR_NONE) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;}             

            STRCAT((INT8*)CaptureFileName, EXT_DOT);
            STRCAT((INT8*)CaptureFileName, PHOTO_JPG_EXT);

            AHC_UF_PreAddFile(CaptureDirKey, (INT8*)CaptureFileName);
            AHC_UF_PostAddFile(CaptureDirKey, (INT8*)CaptureFileName);
			#endif

            sRet = MMPS_DSC_SetCaptureBuf(NULL, NULL, MMP_FALSE, MMP_TRUE, MMP_DSC_CAPTURE_NO_ROTATE, MMP_FALSE);
            if (sRet != MMP_ERR_NONE) { AHC_PRINT_RET_ERROR(gbAhcDbgBrk,sRet); return AHC_FALSE;}             

            if (TimeStampOp & AHC_ACC_TIMESTAMP_ENABLE_MASK) {
                MMPS_DSC_SetSticker(NULL, NULL);
            }
        }
        else {
        
            if (TimeStampOp & AHC_ACC_TIMESTAMP_ENABLE_MASK) {
                MMPS_DSC_SetSticker(NULL, NULL);
            }

			#if (ENABLE_STROBE)
            if (bFlashStatus) {
                AHC_STROBE_EnableStrobeStatus(AHC_TRUE);
            }
			#endif

            return AHC_FALSE;
        }

		#if (ENABLE_STROBE)
        if (bFlashStatus) {
            AHC_STROBE_EnableStrobeStatus(AHC_TRUE);
        }
		#endif
    }
    else {
        return AHC_FALSE;
    }

    if (MMP_ERR_NONE != sRet) {
        return AHC_FALSE;
	}
	
    return AHC_TRUE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_GetCaptureFileName
//  Description : 
//------------------------------------------------------------------------------
AHC_BOOL AHC_GetCaptureFileName(MMP_USHORT *CaptureDirKey, MMP_BYTE *pJpgFileName, MMP_UBYTE *CaptureFileName)
{
    MMP_BYTE                bJpgFileName[MAX_FILE_NAME_SIZE];
    MMP_BYTE                DirName[16];
    UINT8                   CaptureRawName[32]; 
    UINT8                   bCreateNewDir;
    AHC_BOOL                bRet; 

    MEMSET(bJpgFileName    , 0, sizeof(bJpgFileName));
    MEMSET(DirName         , 0, sizeof(DirName));
    MEMSET(CaptureFileName , 0, 32);
    MEMSET(CaptureRawName  , 0, sizeof(CaptureRawName));

#if (DCF_FILE_NAME_TYPE == DCF_FILE_NAME_TYPE_NORMAL)
    bRet = AHC_UF_GetName(CaptureDirKey, (INT8*)DirName, (INT8*)CaptureFileName, &bCreateNewDir);
	
	if (bRet == AHC_TRUE) {
	
        STRCPY(bJpgFileName, (INT8*)AHC_UF_GetRootDirName());
        STRCAT(bJpgFileName, "\\");
        STRCAT(bJpgFileName, DirName);
        
        if (bCreateNewDir) {
            MMPS_FS_DirCreate((INT8*)bJpgFileName, STRLEN(bJpgFileName));
            AHC_UF_AddDir(DirName);
        }
            
        STRCAT(bJpgFileName,"\\");
        STRCAT(bJpgFileName,(INT8*)CaptureFileName);
	}
#elif (DCF_FILE_NAME_TYPE == DCF_FILE_NAME_TYPE_DATE_TIME) 
    {
        AHC_RTC_TIME RtcTime;
        
        AHC_RTC_GetTime(&RtcTime);
        
        bRet = AHC_UF_GetName2(&RtcTime, (INT8*)bJpgFileName, (INT8*)CaptureFileName, &bCreateNewDir);
    }
#endif

	if (bRet == AHC_TRUE) {
		
		STRCAT(bJpgFileName, EXT_DOT);

        #if (JPEG_INTO_AVI_WHILE_TURN_ON)
		if (uiMode == AHC_MODE_LONG_TIME_CONTINUOUS_FIRST_CAPTURE)
		    STRCAT(bJpgFileName, MOVIE_AVI_EXT);
		else
		    STRCAT(bJpgFileName, PHOTO_JPG_EXT);
        #else	 	
		STRCAT(bJpgFileName, PHOTO_JPG_EXT);
        #endif

        #if (PIR_JPEG_INTO_AVI_WHILE_TURN_ON)
		if (uiMode == AHC_MODE_LONG_TIME_CONTINUOUS_NEXT_CAPTURE)
		    STRCPY(bJpgFileName, m_InfoLog.DCFCurStillFileName);
		else
		    STRCPY(m_InfoLog.DCFCurStillFileName, bJpgFileName);
        #else	 
            #if (USE_INFO_FILE)
		    STRCPY(m_InfoLog.DCFCurStillFileName, bJpgFileName);
            #endif
        #endif

        memcpy((void *)pJpgFileName, (void *)bJpgFileName, MAX_FILE_NAME_SIZE);
    }

    return AHC_TRUE;   
}

//------------------------------------------------------------------------------
//  Function    : AHC_CaptureAddFile
//  Description : 
//------------------------------------------------------------------------------
AHC_BOOL AHC_CaptureAddFile(UINT16 CaptureDirKey, MMP_BYTE *CaptureFileName)
{
    STRCAT((INT8*)CaptureFileName, EXT_DOT);
    STRCAT((INT8*)CaptureFileName, PHOTO_JPG_EXT);

    AHC_UF_PreAddFile(CaptureDirKey,(INT8*)CaptureFileName);
    AHC_UF_PostAddFile(CaptureDirKey,(INT8*)CaptureFileName);
    
    return AHC_TRUE;
}

#if 0
void _____Strobe_Functions_____(){}
#endif

#if (ENABLE_STROBE)

//------------------------------------------------------------------------------
//  Function    : AHC_STROBE_Initialize
//  Description : 
//------------------------------------------------------------------------------
AHC_ERR AHC_STROBE_Initialize(void)
{
	STROBE_Initialize();
	return AHC_SUCCESS;
}

//------------------------------------------------------------------------------
//  Function    : AHC_STROBE_EnableStrobeStatus
//  Description : 
//------------------------------------------------------------------------------
/**	@brief	do not change the sequence of API. 
			this API enable strobe and the process of automatic charging on.
*/
AHC_ERR AHC_STROBE_EnableStrobeStatus(AHC_BOOL bEnable)
{
	if (bEnable) {
		AHC_STROBE_ChargeOn(bEnable);
		m_byStrobeStatus = bEnable;
	}
	else {
		m_byStrobeStatus = bEnable;
		AHC_STROBE_ChargeOn(bEnable);
	}
	
	return AHC_SUCCESS;
}

//------------------------------------------------------------------------------
//  Function    : AHC_STROBE_GetStrobeStatus
//  Description : 
//------------------------------------------------------------------------------
AHC_BOOL AHC_STROBE_GetStrobeStatus(void)
{
	return m_byStrobeStatus;
}

//------------------------------------------------------------------------------
//  Function    : AHC_STROBE_ChargeOn
//  Description : 
//------------------------------------------------------------------------------
AHC_ERR AHC_STROBE_ChargeOn(AHC_BOOL bEnable)
{
	return STROBE_EanbleCharge(bEnable);
}

//------------------------------------------------------------------------------
//  Function    : AHC_STROBE_CheckChargeReady
//  Description : 
//------------------------------------------------------------------------------
AHC_BOOL AHC_STROBE_CheckChargeReady(void)
{
	AHC_BOOL bEnable;
	
	STROBE_CheckChargeReady(&bEnable);
	return bEnable;
}

//------------------------------------------------------------------------------
//  Function    : AHC_STROBE_SetChargeReadyStatus
//  Description : 
//------------------------------------------------------------------------------
void AHC_STROBE_SetChargeReadyStatus(AHC_BOOL bEnable)
{
	m_byStrobeReadyStatus = bEnable;
}

//------------------------------------------------------------------------------
//  Function    : AHC_STROBE_CheckChargeReadyStatus
//  Description : 
//------------------------------------------------------------------------------
AHC_BOOL AHC_STROBE_CheckChargeReadyStatus(void)
{
	return m_byStrobeReadyStatus;
}

//------------------------------------------------------------------------------
//  Function    : AHC_STROBE_TriggerDuration
//  Description : 
//------------------------------------------------------------------------------
AHC_ERR AHC_STROBE_TriggerDuration(UINT32 ulBlankDuration, UINT32 ulTriggerDuration)
{
	m_StrobeBlankDuration	= ulBlankDuration;
	m_StrobeTriggerDuration = ulTriggerDuration;

	return STROBE_TriggerDuration(ulBlankDuration, ulTriggerDuration);
}

//------------------------------------------------------------------------------
//  Function    : AHC_STROBE_Trigger
//  Description : 
//------------------------------------------------------------------------------
AHC_ERR AHC_STROBE_Trigger(void)
{
	if (AHC_STROBE_GetStrobeStatus() == AHC_TRUE) {
		AHC_STROBE_EnableStrobeStatus(AHC_TRUE);///< stop the process of automatic charge.
		STROBE_Trigger();
		AHC_STROBE_EnableStrobeStatus(AHC_FALSE);
		return AHC_SUCCESS;
	}
	else {
		return AHC_FAIL;
	}
}
#endif

//==============================================================================
//
//  File        : ex_AHC.c
//  Description : Functions used to test AHC related functions.
//  Author      : Ben Lu
//  Revision    : 1.0
//
//==============================================================================
/**
 *  @file ex_AHC.c
 *  @brief Functions used to test AHC related functions.
 *  @author Ben Lu
 *  @version 1.0
 */

#if defined(SA_PL)

#include "includes_fw.h"
#include "config_fw.h"
#include "mmpf_uart.h"
#include "lib_retina.h"
#include "AHC_General.h"
#include "AHC_Parameter.h"
#include "ex_AHC.h"
#include "ait_utility.h"


#include "AHC_DCF.h"
#include "AHC_Browser.h"
#include "AHC_General.h"
#include "AHC_Video.h"
#include "AHC_Display.h"
#include "AHC_GUI.h"
#include "AHC_Audio.h"
#include "AHC_Capture.h"
#include "AHC_Media.h"
#include "AHC_Sensor.h"


#include "AIHC_DCF.h"

//==============================================================================
//
//                              CONSTANTS
//
//==============================================================================
#define AHC_JPEG_PAN_SPEED       4
#define MAX_AHC_JPEG_ZOOM_LEVEL  16
//==============================================================================
//
//                              VARIABLES
//
//==============================================================================
static AHC_BOOL bInitBasicAHC       = AHC_FALSE;
static UINT32   AhcDisplayRotate    = AHC_DISPLAY_ROTATE_NO_ROTATE;
static UINT32   AhcDisplayWidth     = 240;
static UINT32   AhcDisplayHeight    = 320;
static UINT32   AhcJpgDecodeWidth   = 1024;
static UINT32   AhcJpgDecodeHeight  = 768;
static UINT32   CurZoomLevel        = 0;
static UINT32   JpgStartX, JpgStartY;
static UINT32   AhcJpgDispWidth,AhcJpgDispHeight;

//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================

//==============================================================================
//
//                              FUNCTION DEFINITION
//
//==============================================================================

#define AHC_EXIF_DEFAULT_MAKER_NAME     "AIT Corp."
#define AHC_EXIF_DEFAULT_MODEL_NAME     "AIT Turnkey"
#define AHC_EXIF_DEFAULT_SOFTWARE_NAME  "AIT Turnkey"
#define AHC_EXIF_DEFAULT_ARTIST_NAME    "AIT Turnkey"
#define AHC_EXIF_DEFAULT_COPYRIGHT_NAME "AIT Corp."

//------------------------------------------------------------------------------
//  Function    : ex_AHC_Exif_Config
//  Description : sample code for programmer to test AHC_General.c related functions
//  Note        : 
//------------------------------------------------------------------------------
/** @brief sample code for programmer to test AHC_General.c related functions.
@return void.
*/

void ex_AHC_Exif_Config()
{
    MMP_USHORT exif_orientation = 1;
    MMP_ULONG  exif_res[2] = {72, 1} ;
    MMP_USHORT exif_res_unit = 2;
    MMP_USHORT ycc_pos = 1 ;
    
    AHC_SetImageEXIF((UINT8*)AHC_EXIF_DEFAULT_MAKER_NAME    , STRLEN(AHC_EXIF_DEFAULT_MAKER_NAME)       , 0, AHC_IFD0_MAKE              , AHC_EXIF_ASCII    , STRLEN(AHC_EXIF_DEFAULT_MAKER_NAME)    , 0);
	
	AHC_SetImageEXIF((UINT8*)AHC_EXIF_DEFAULT_MODEL_NAME    , STRLEN(AHC_EXIF_DEFAULT_MODEL_NAME)       , 0, AHC_IFD0_MODEL             , AHC_EXIF_ASCII    , STRLEN(AHC_EXIF_DEFAULT_MODEL_NAME)    , 0);
	
	AHC_SetImageEXIF((UINT8*)AHC_EXIF_DEFAULT_SOFTWARE_NAME , STRLEN(AHC_EXIF_DEFAULT_SOFTWARE_NAME)    , 0, AHC_IFD0_SOFTWARE          , AHC_EXIF_ASCII    , STRLEN(AHC_EXIF_DEFAULT_SOFTWARE_NAME) , 0);
	
	AHC_SetImageEXIF((UINT8*)AHC_EXIF_DEFAULT_ARTIST_NAME   , STRLEN(AHC_EXIF_DEFAULT_ARTIST_NAME)      , 0, AHC_IFD0_ARTIST            , AHC_EXIF_ASCII    , STRLEN(AHC_EXIF_DEFAULT_ARTIST_NAME)   , 0);
	
	AHC_SetImageEXIF((UINT8*)AHC_EXIF_DEFAULT_COPYRIGHT_NAME, STRLEN(AHC_EXIF_DEFAULT_COPYRIGHT_NAME)   , 0, AHC_IFD0_COPYRIGHT         , AHC_EXIF_ASCII    , STRLEN(AHC_EXIF_DEFAULT_COPYRIGHT_NAME), 0);
	
	AHC_SetImageEXIF((UINT8*)&exif_orientation              , 2                                         , 0, AHC_IFD0_ORIENTATION       , AHC_EXIF_SHORT    , 1                                      , 0);
	
	AHC_SetImageEXIF((UINT8*)&exif_res                      , 8                                         , 0, AHC_IFD0_X_RESOLUTION      , AHC_EXIF_RATIONAL , 1                                      , 0);
	AHC_SetImageEXIF((UINT8*)&exif_res                      , 8                                         , 0, AHC_IFD0_Y_RESOLUTION      , AHC_EXIF_RATIONAL , 1                                      , 0);
	
	AHC_SetImageEXIF((UINT8*)&exif_res_unit                 , 2                                         , 0, AHC_IFD0_RESOLUTION_UNIT   , AHC_EXIF_SHORT    , 1                                      , 0);
	
	AHC_SetImageEXIF((UINT8*)&ycc_pos                       , 2                                         , 0, AHC_IFD0_YCBCR_POSITION    , AHC_EXIF_SHORT    , 1                                      , 0);
	
}

//------------------------------------------------------------------------------
//  Function    : AHC_ConfigPlayback
//  Description : sample code for programmer to test AHC_General.c related functions
//  Note        : 
//------------------------------------------------------------------------------
/** @brief sample code for programmer to test AHC_General.c related functions.
@return void.
*/
void ex_AHC_CalcJpgPlaybackZoom(UINT32 ZoomLevel,UINT32 *OffsetX,UINT32 *OffsetY,UINT32 *DispWidth,UINT32 *DispHeight) {

    UINT32      MaxZoomLevel = MAX_AHC_JPEG_ZOOM_LEVEL;
    IMG_ATTRIB  ImgInfo;
    UINT32      MaxZoomRatio;
    UINT32      ZoomRatio;
    UINT32      JpegWidth;
    UINT32      JpegHeight;
    
    AHC_GetImageAttributes(&ImgInfo);
    
    JpegWidth  = ImgInfo.ImageHSize;
    JpegHeight = ImgInfo.ImageVSize;
		            
    if ( ((AhcJpgDecodeWidth*1000) / AhcDisplayWidth) > ((AhcJpgDecodeHeight*1000) / AhcDisplayHeight) ) {
        MaxZoomRatio = ( AhcJpgDecodeHeight * 1000 ) / AhcDisplayHeight;
    }
    else {
        MaxZoomRatio = ( AhcJpgDecodeWidth * 1000 ) / AhcDisplayWidth;
    }
		            
    RTNA_DBG_Str (0,"MaxZoomRatio : ");
    RTNA_DBG_Long(0,MaxZoomRatio);
    RTNA_DBG_Str (0,"\r\n");


    ZoomRatio = 1000 + ((MaxZoomRatio - 1000) * ZoomLevel) / MaxZoomLevel;
    
    *DispWidth          = (JpegWidth  * 1000) / ZoomRatio;
    *DispHeight         = (JpegHeight * 1000) / ZoomRatio;
    
    *OffsetX = (JpegWidth  - *DispWidth) / 2;
    *OffsetY = (JpegHeight - *DispHeight) / 2;    
}		            
//------------------------------------------------------------------------------
//  Function    : AHC_ConfigPlayback
//  Description : sample code for programmer to test AHC_General.c related functions
//  Note        : 
//------------------------------------------------------------------------------
/** @brief sample code for programmer to test AHC_General.c related functions.
@return void.
*/
void ex_AHC_ConfigPlayback(UINT8 *FileType) {
    UINT32              CurrentObjIdx;
	DCF_INFO            DcfInfo;
	
	
	//Get DcfInfo	
	AHC_GetCurrentDCFObjIndex(&CurrentObjIdx);
	AHC_GetDCFInfo(CurrentObjIdx, &DcfInfo);
	
	//Set display window
	if ( ( DCF_OBG_MOV == DcfInfo.FileType ) || 
         ( DCF_OBG_MP4 == DcfInfo.FileType ) ||
         ( DCF_OBG_AVI == DcfInfo.FileType ) ||
         ( DCF_OBG_3GP == DcfInfo.FileType ) ||
         ( DCF_OBG_WMV == DcfInfo.FileType ) )
    {
        AhcDisplayRotate    = AHC_DISPLAY_ROTATE_RIGHT_90;
        AhcDisplayWidth     = MMPS_Display_GetConfiguration()->mainlcd.usWidth;
        AhcDisplayHeight    = MMPS_Display_GetConfiguration()->mainlcd.usHeight;
        
        AHC_SetParam(PARAM_ID_DISPLAY_ROTATE_ENABLE     ,AhcDisplayRotate);
        
        #if (AUDIO_SET_DB == 0x01) 
        AHC_SetParam(PARAM_ID_AUDIO_VOLUME              ,0x40);
        #else
        AHC_SetParam(PARAM_ID_AUDIO_VOLUME              ,20);
        #endif
        
        AHC_SetDisplayWindow(DISPLAY_SYSMODE_PLAYBACK,AHC_TRUE,0,0,0,AhcDisplayWidth,AhcDisplayHeight,0);
        
        *FileType = 1;
    }
    else if ( DCF_OBG_JPG == DcfInfo.FileType ) {
        AhcDisplayRotate    = AHC_DISPLAY_ROTATE_NO_ROTATE;        
        //AhcDisplayRotate    = AHC_DISPLAY_ROTATE_RIGHT_90;
        AhcDisplayWidth     = MMPS_Display_GetConfiguration()->mainlcd.usWidth;
        AhcDisplayHeight    = MMPS_Display_GetConfiguration()->mainlcd.usHeight;
        
        AHC_SetParam(PARAM_ID_DISPLAY_ROTATE_ENABLE     ,AhcDisplayRotate);
        
        AHC_SetDisplayWindow(DISPLAY_SYSMODE_STILLCAPTURE,AHC_TRUE,0,0,0,AhcDisplayWidth,AhcDisplayHeight,0);
        
        AHC_SetParam(PARAM_ID_JPEG_DECODE_X_THRESH       ,AhcJpgDecodeWidth);
        AHC_SetParam(PARAM_ID_JPEG_DECODE_Y_THRESH       ,AhcJpgDecodeHeight);
               
        *FileType = 0;
        
        JpgStartX    = 0;
        JpgStartY    = 0;
        CurZoomLevel = 0;
    }
    else if ( ( DCF_OBG_MP3 == DcfInfo.FileType ) || 
              ( DCF_OBG_WAV == DcfInfo.FileType ) ||
              ( DCF_OBG_OGG == DcfInfo.FileType ) ||
              ( DCF_OBG_WMA == DcfInfo.FileType ) )
    {
        #if (AUDIO_SET_DB == 0x01) 
        AHC_SetParam(PARAM_ID_AUDIO_VOLUME              ,0x40);
        #else
        AHC_SetParam(PARAM_ID_AUDIO_VOLUME              ,20);
        #endif
        
        *FileType = 2;
    }
}

//------------------------------------------------------------------------------
//  Function    : AHC_PlaybackMode
//  Description : sample code for programmer to test AHC_General.c related functions
//  Note        : 
//------------------------------------------------------------------------------
/** @brief sample code for programmer to test AHC_General.c related functions.
@return void.
*/
void ex_AHC_PlaybackMode(){
    MMP_ULONG       len;
	MMP_UBYTE       c = 0;
	char            str[20];
	static UINT32   Direction;
	UINT8           FileType;
	UINT32          CurrentObjIdx;
	AUDIO_ATTR      AudioInfo;
	AHC_VIDEO_ATTR  VideoInfo;
	IMG_ATTRIB      ImgInfo;
	UINT32          CurTime;
	
	ex_AHC_ConfigPlayback(&FileType);
		    
    //Start the playback
    if (FileType == 2) {
        if ( AHC_PlayAudioCmd(AHC_AUDIO_PLAY_START, 0) == AHC_TRUE ) {
        }
        else {
            return;
        }
    }
    else {
    	if ( AHC_SetMode(AHC_MODE_PLAYBACK) == AHC_TRUE ) {
    	    if ( FileType == 0 ) {
    	        AHC_GetImageAttributes(&ImgInfo);
    	    }
    	}
    	else {
    	    return;
    	}
	}
	
	while (c!='q') {
		RTNA_DBG_Str(0, "------------- Thumb Menu ---------------\r\n");
		if ( FileType == 0 ) {
    		RTNA_DBG_Str(0, "u  jpeg zoom in\r\n");
    		RTNA_DBG_Str(0, "n  jpeg zoom out\r\n");
    		RTNA_DBG_Str(0, "w  jpeg pan up\r\n");
    		RTNA_DBG_Str(0, "a  jpeg pan left\r\n");
    		RTNA_DBG_Str(0, "s  jpeg pan down\r\n");
    		RTNA_DBG_Str(0, "d  jpeg pan right\r\n");
		}
		if ( ( FileType == 1 ) || ( FileType == 2 ) ){
    		RTNA_DBG_Str(0, "2  Pause playback\r\n");
    		RTNA_DBG_Str(0, "3  Resume playback\r\n");
    		RTNA_DBG_Str(0, "4  FastForward\r\n");
    		RTNA_DBG_Str(0, "5  BackForward\r\n");
    		RTNA_DBG_Str(0, "6  Normal Speed\r\n");
    		RTNA_DBG_Str(0, "7  Rotate\r\n");
    		RTNA_DBG_Str(0, "8  Stop playback\r\n");
    		RTNA_DBG_Str(0, "t  current time\r\n");
		}
		
		RTNA_DBG_Str(0, "i  file information\r\n");
		RTNA_DBG_Str(0, "]  Playback the next file\r\n");
		RTNA_DBG_Str(0, "[  Playback the previos file\r\n");
		RTNA_DBG_Str(0, "B  Back to thumbnail mode\r\n");
		RTNA_DBG_Str(0, "q  Exit menu ----------------------------\r\n");

        RTNA_DBG_Str(0, "Please Input Command:");
		RTNA_DBG_Gets(str, &len);
        c = str[0];
        RTNA_DBG_Str(0, "\r\n");

		switch(c) {
		    case '2':
		        if ( FileType == 0 ) {
		            //image
		        }
		        else if ( FileType == 1 )
		        {
		            //video
		            AHC_PlaybackClipCmd(AHC_PB_MOVIE_PAUSE, 0);
		        }
		        else if ( FileType == 2 )
		        {
		            //audio
		            AHC_PlayAudioCmd(AHC_AUDIO_PLAY_PAUSE, 0);
		        }
		        
		        break;
		    case '3':
		        if ( FileType == 0 ) {
		            //image
		        }
		        else if ( FileType == 1 )
		        {
		            //video
		            AHC_PlaybackClipCmd(AHC_PB_MOVIE_RESUME, 0);
		        }
		        else if ( FileType == 2 )
		        {
		            //audio
		            AHC_PlayAudioCmd(AHC_AUDIO_PLAY_RESUME, 0);
		        }
		        break;
		    case '4':
		        if ( FileType == 0 ) {
		            //image
		        }
		        else if ( FileType == 1 )
		        {
		            //video
		            AHC_PlaybackClipCmd(AHC_PB_MOVIE_FAST_FORWARD, 0);
		        }
		        else if ( FileType == 2 )
		        {
		            //audio
		            
		            //advance 1 sec
		            AHC_PlayAudioCmd(AHC_AUDIO_PLAY_FAST_FORWARD, 1000);
		        }
		        break;
		    case '5':
		        if ( FileType == 0 ) {
		            //image
		        }
		        else if ( FileType == 1 )
		        {
		            //video
		            AHC_PlaybackClipCmd(AHC_PB_MOVIE_BACKWARD, 0);
		        }
		        else if ( FileType == 2 )
		        {
		            //audio
		            
		            //backward 1 sec
		            AHC_PlayAudioCmd(AHC_AUDIO_PLAY_FAST_FORWARD, -1000);
		        }
		        break;
		    case '6':
		        if ( FileType == 0 ) {
		            //image
		        }
		        else if ( FileType == 1 )
		        {
		            //video
		            AHC_PlaybackClipCmd(AHC_PB_MOVIE_NORMAL_PLAY_SPEED, 0);
		        }
		        else if ( FileType == 2 )
		        {
		            //audio
		            
		            //TBD
		        }
		        break;
		    case '7':
		        if ( FileType == 1 )
		        {
		            //video
		            AHC_PlaybackClipCmd(AHC_PB_MOVIE_ROTATE, AHC_DISPLAY_ROTATE_NO_ROTATE + Direction);
		            
		            Direction = (Direction + 1) % 4;
		        }
		        break;
		    case '8':
		        if ( FileType != 2 ) {
		            AHC_SetMode(AHC_MODE_IDLE);
		        }
		        else {
		            AHC_PlayAudioCmd(AHC_AUDIO_PLAY_STOP, 0);
		        }
		        
		        break;
		    case ']':
		        //Get DcfInfo	
            	AHC_GetCurrentDCFObjIndex(&CurrentObjIdx);
            	
            	AHC_SetCurrentDCFObjIndex(CurrentObjIdx + 1);
            	
            	ex_AHC_ConfigPlayback(&FileType);
            	
		        if ( FileType != 2 ) {
	                //AHC_SetMode(AHC_MODE_IDLE);
	                
	                if ( AHC_SetMode(AHC_MODE_PLAYBACK) != AHC_TRUE ) {
    	                return;
    		        }
	            }
	            else {
	                AHC_PlayAudioCmd(AHC_AUDIO_PLAY_STOP, 0);
	                
	                if ( AHC_PlayAudioCmd(AHC_AUDIO_PLAY_START, 0) != AHC_TRUE ) {
	                    return;
                    }
	            }
		        
		        break;
		    case '[':
		        //Get DcfInfo	
            	AHC_GetCurrentDCFObjIndex(&CurrentObjIdx);
            	
            	if( CurrentObjIdx > 1 )
            	    AHC_SetCurrentDCFObjIndex(CurrentObjIdx - 1);
            	else
            	    AHC_SetCurrentDCFObjIndex(0);
            	
	            //AHC_SetMode(AHC_MODE_IDLE);
	            
            	ex_AHC_ConfigPlayback(&FileType);
            	
	            if ( FileType != 2 ) {
    	            if ( AHC_SetMode(AHC_MODE_PLAYBACK) != AHC_TRUE ) {
    	                return;
    		        }
		        }
		        else {
		            AHC_PlayAudioCmd(AHC_AUDIO_PLAY_STOP, 0);
	                
	                if ( AHC_PlayAudioCmd(AHC_AUDIO_PLAY_START, 0) != AHC_TRUE ) {
	                    return;
                    }
		        }
		        
		        break;
		    case 'i':
		        if ( FileType == 0 ) {
		            AHC_GetImageAttributes(&ImgInfo);
		            
		            RTNA_DBG_Str (0,"Width             : ");
		            RTNA_DBG_Long(0,ImgInfo.ImageHSize);
		            RTNA_DBG_Str (0,"\r\n");
		            
		            RTNA_DBG_Str (0,"Height            : ");
		            RTNA_DBG_Long(0,ImgInfo.ImageVSize);
		            RTNA_DBG_Str (0,"\r\n");
		            
                    RTNA_DBG_Str (0,"Thumb Width       : ");
		            RTNA_DBG_Long(0,ImgInfo.ThumbHSize);
		            RTNA_DBG_Str (0,"\r\n");
		            
		            RTNA_DBG_Str (0,"Thumb Height      : ");
		            RTNA_DBG_Long(0,ImgInfo.ThumbVSize);
		            RTNA_DBG_Str (0,"\r\n");
		        }
		        else if ( FileType == 1 ) {
		            AHC_GetVideoFileAttr(&VideoInfo);
                    
                    RTNA_DBG_Str (0,"Video Format : ");
		            RTNA_DBG_Long(0,VideoInfo.VideoFormat);
		            RTNA_DBG_Str (0,"\r\n");
		            
		            RTNA_DBG_Str (0,"Audio Format : ");
		            RTNA_DBG_Long(0,VideoInfo.AudioFormat);
		            RTNA_DBG_Str (0,"\r\n");
		            
                    RTNA_DBG_Str (0,"Width        : ");
		            RTNA_DBG_Long(0,VideoInfo.Width);
		            RTNA_DBG_Str (0,"\r\n");
		            
		            RTNA_DBG_Str (0,"Height       : ");
		            RTNA_DBG_Long(0,VideoInfo.Height);
		            RTNA_DBG_Str (0,"\r\n");
		            
                    RTNA_DBG_Str (0,"SampleRate   : ");
		            RTNA_DBG_Long(0,VideoInfo.SampleRate);
		            RTNA_DBG_Str (0,"\r\n");
		            
		            RTNA_DBG_Str (0,"Channels     : ");
		            RTNA_DBG_Long(0,VideoInfo.Channels);
		            RTNA_DBG_Str (0,"\r\n");
		            
		            RTNA_DBG_Str (0,"Total Time   : ");
		            RTNA_DBG_Long(0,VideoInfo.TotalTime);
		            RTNA_DBG_Str (0,"\r\n");
		        }
		        else if ( FileType == 2 ) {
		            AHC_GetAudioFileAttr(&AudioInfo);
		            
		            RTNA_DBG_Str (0,"SampleRate   : ");
		            RTNA_DBG_Long(0,AudioInfo.sample_rate);
		            RTNA_DBG_Str (0,"\r\n");
		            
		            RTNA_DBG_Str (0,"BitRate      : ");
		            RTNA_DBG_Long(0,AudioInfo.avg_bitrate);
		            RTNA_DBG_Str (0,"\r\n");
		            
		            RTNA_DBG_Str (0,"Channels     : ");
		            RTNA_DBG_Long(0,AudioInfo.channels);
		            RTNA_DBG_Str (0,"\r\n");
		            
		            RTNA_DBG_Str (0,"Total Time   : ");
		            RTNA_DBG_Long(0,AudioInfo.total_time);
		            RTNA_DBG_Str (0,"\r\n");
		            
		        }
		        break;
		    case 't':
		        if ( FileType == 1 ) {
		            AHC_GetVideoCurrentPlayTime(&CurTime);
		            
		            RTNA_DBG_Str (0,"Current Time : ");
		            RTNA_DBG_Long(0,CurTime);
		            RTNA_DBG_Str (0,"\r\n");
		        }
		        else if ( FileType == 2 ) {
		            AHC_GetAudioCurrentTime(&CurTime);
		            
		            RTNA_DBG_Str (0,"Current Time : ");
		            RTNA_DBG_Long(0,CurTime);
		            RTNA_DBG_Str (0,"\r\n");
		        }
		        break;
		    case 'u':
		        if ( FileType == 0 ) {
		            UINT32 OffsetX,OffsetY;
		            
        	        ex_AHC_CalcJpgPlaybackZoom(CurZoomLevel + 1, &OffsetX, &OffsetY,&AhcJpgDispWidth, &AhcJpgDispHeight);
		            
		            if ( AHC_PlaybackZoom(OffsetX,OffsetY,AhcJpgDispWidth, AhcJpgDispHeight) == AHC_TRUE ) {
		                JpgStartX = OffsetX;
		                JpgStartY = OffsetY;
		                
		                CurZoomLevel++;
		            }
		            
		            RTNA_DBG_Str (0,"CurZoomLevel : ");
		            RTNA_DBG_Long(0,CurZoomLevel);
		            RTNA_DBG_Str (0,"\r\n");
		        }
		        
		        break;
		    case 'n':
		        if ( FileType == 0 ) {
		            UINT32 OffsetX,OffsetY;
		            
		            if (CurZoomLevel > 0) {
        	            ex_AHC_CalcJpgPlaybackZoom(CurZoomLevel - 1, &OffsetX, &OffsetY,&AhcJpgDispWidth, &AhcJpgDispHeight);
        	        }
        	        else {
        	            ex_AHC_CalcJpgPlaybackZoom(0, &OffsetX, &OffsetY,&AhcJpgDispWidth, &AhcJpgDispHeight);
        	        }
        	        
        	        if ( AHC_PlaybackZoom(OffsetX,OffsetY,AhcJpgDispWidth, AhcJpgDispHeight) == AHC_TRUE ) {
		                JpgStartX = OffsetX;
		                JpgStartY = OffsetY;
		                
		                if (CurZoomLevel > 0)
		                    CurZoomLevel--;
		            }
		            
		            RTNA_DBG_Str (0,"CurZoomLevel : ");
		            RTNA_DBG_Long(0,CurZoomLevel);
		            RTNA_DBG_Str (0,"\r\n");
		        }		        
		        break;
		    case 'w': 
		        if ( FileType == 0 ) {
		            
		            if ( AHC_PlaybackZoom(JpgStartX,JpgStartY - AHC_JPEG_PAN_SPEED,AhcJpgDispWidth, AhcJpgDispHeight) == AHC_TRUE ) {
		                JpgStartY = JpgStartY - AHC_JPEG_PAN_SPEED;
		            }
                }
		        break;
		    case 'a':
		        if ( FileType == 0 ) {
		            
		            if ( AHC_PlaybackZoom(JpgStartX - AHC_JPEG_PAN_SPEED,JpgStartY,AhcJpgDispWidth, AhcJpgDispHeight) == AHC_TRUE ) {
		                JpgStartX = JpgStartX - AHC_JPEG_PAN_SPEED;
		            }
                }
		        break;
		    case 's':
		        if ( FileType == 0 ) {
		            
		            if ( AHC_PlaybackZoom(JpgStartX,JpgStartY + AHC_JPEG_PAN_SPEED,AhcJpgDispWidth, AhcJpgDispHeight) == AHC_TRUE ) {
		                JpgStartY = JpgStartY + AHC_JPEG_PAN_SPEED;
		            }
                }
		        break;
		    case 'd':
		        if ( FileType == 0 ) {
		            
		            if ( AHC_PlaybackZoom(JpgStartX + AHC_JPEG_PAN_SPEED,JpgStartY,AhcJpgDispWidth, AhcJpgDispHeight) == AHC_TRUE ) {
		                JpgStartX = JpgStartX + AHC_JPEG_PAN_SPEED;
		            }
                }
		        break;
		    case 'b':
		        if ( FileType != 2 ) {
		            AHC_SetMode(AHC_MODE_IDLE);
		        }
		        else {
		            AHC_PlayAudioCmd(AHC_AUDIO_PLAY_STOP, 0);
		        }
		        
		        return;
		        
		        break;
			case 'q':
			    if ( FileType != 2 ) {
		            AHC_SetMode(AHC_MODE_IDLE);
		        }
		        else {
		            AHC_PlayAudioCmd(AHC_AUDIO_PLAY_STOP, 0);
		        }
				break;
			default:
				RTNA_DBG_Str(0, "Unsupport Command, please enter again.\r\n");
			break;
		}
	}
}

//------------------------------------------------------------------------------
//  Function    : AHC_ThumbNailMode
//  Description : sample code for programmer to test AHC_General.c related functions
//  Note        : 
//------------------------------------------------------------------------------
/** @brief sample code for programmer to test AHC_General.c related functions.
@return void.
*/
void ex_AHC_ThumbNailMode(){
    MMP_ULONG       len;
	MMP_UBYTE       c = 0;
    UINT32          MaxDcfObj; 
	char            str[20];
	UINT32          CurrentObjIdx;
	DCF_INFO        DcfInfo;
    AHC_THUMB_CFG   ThumbConfig;
    UINT32          StrLen;
    INT8            DirName[32];
    INT8            FileName[32];
    AUDIO_ATTR      AudioInfo;
    AHC_VIDEO_ATTR  VideoInfo;
    IMG_ATTRIB      ImgInfo;
    
    ThumbConfig.uwTHMPerLine    = 3;
    ThumbConfig.uwTHMPerColume  = 3;
    ThumbConfig.uwHGap          = 12;
    ThumbConfig.uwVGap          = 32;
    ThumbConfig.byBkColor       = 0x101010;//0xFFFFFF;
    ThumbConfig.byFocusColor    = 0x0000FF;    
    ThumbConfig.StartX          = 0;
    ThumbConfig.StartY          = 0;
    ThumbConfig.DisplayWidth    = MMPS_Display_GetConfiguration()->mainlcd.usWidth;
    ThumbConfig.DisplayHeight   = MMPS_Display_GetConfiguration()->mainlcd.usHeight;

    AHC_ConfigThumbnail(&ThumbConfig, AHC_FALSE);
    
    //reset to the last DCF obj
    AHC_GetMaxDCFObjCount(&MaxDcfObj);    
    AHC_SetCurrentDCFObjIndex(MaxDcfObj - 1);

    AHC_SetMode(AHC_MODE_THUMB_BROWSER);
	
	while (c!='q') {
		RTNA_DBG_Str(0, "------------- Thumb Menu ---------------\r\n");
		RTNA_DBG_Str(0, "W  Move up -----------------\r\n");
		RTNA_DBG_Str(0, "S  Move down -------\r\n");
		RTNA_DBG_Str(0, "A  Move left -------\r\n");
		RTNA_DBG_Str(0, "D  Move right\r\n");
		RTNA_DBG_Str(0, "i  File information\r\n");
		RTNA_DBG_Str(0, "R  Remove file\r\n");
		RTNA_DBG_Str(0, "1  Playback\r\n");
		RTNA_DBG_Str(0, "q  Exit menu ----------------------------\r\n");

        RTNA_DBG_Str(0, "Please Input Command:");
		RTNA_DBG_Gets(str, &len);
        c = str[0];
        RTNA_DBG_Str(0, "\r\n");

		switch(c) {
			case 'w':
			    AHC_ThumbnailShiftUp(3);
				break;
			case 'a':
			    AHC_ThumbnailShiftUp(1);
				break;
			case 's':
			    AHC_ThumbnailShiftDown(3);
				break;
			case 'd':
			    AHC_ThumbnailShiftDown(1);
				break;
		    case 'i':
		        AHC_GetCurrentDCFObjIndex(&CurrentObjIdx);
            	AHC_GetDCFInfo(CurrentObjIdx, &DcfInfo);
            	
            	if ( DCF_OBG_JPG == DcfInfo.FileType ) {
		            AHC_GetImageAttributes(&ImgInfo);
		            
		            RTNA_DBG_Str (0,"Width             : ");
		            RTNA_DBG_Long(0,ImgInfo.ImageHSize);
		            RTNA_DBG_Str (0,"\r\n");
		            
		            RTNA_DBG_Str (0,"Height            : ");
		            RTNA_DBG_Long(0,ImgInfo.ImageVSize);
		            RTNA_DBG_Str (0,"\r\n");
		            
                    RTNA_DBG_Str (0,"Thumb Width       : ");
		            RTNA_DBG_Long(0,ImgInfo.ThumbHSize);
		            RTNA_DBG_Str (0,"\r\n");
		            
		            RTNA_DBG_Str (0,"Thumb Height      : ");
		            RTNA_DBG_Long(0,ImgInfo.ThumbVSize);
		            RTNA_DBG_Str (0,"\r\n");
		        }
		        else if ( ( DCF_OBG_MOV == DcfInfo.FileType ) || 
                          ( DCF_OBG_MP4 == DcfInfo.FileType ) ||
                          ( DCF_OBG_AVI == DcfInfo.FileType ) ||
                          ( DCF_OBG_3GP == DcfInfo.FileType ) ||
                          ( DCF_OBG_WMV == DcfInfo.FileType ) )
                {
                    AHC_GetVideoFileAttr(&VideoInfo);
                    
                    RTNA_DBG_Str (0,"Video Format : ");
		            RTNA_DBG_Long(0,VideoInfo.VideoFormat);
		            RTNA_DBG_Str (0,"\r\n");
		            
		            RTNA_DBG_Str (0,"Audio Format : ");
		            RTNA_DBG_Long(0,VideoInfo.AudioFormat);
		            RTNA_DBG_Str (0,"\r\n");
		            
                    RTNA_DBG_Str (0,"Width        : ");
		            RTNA_DBG_Long(0,VideoInfo.Width);
		            RTNA_DBG_Str (0,"\r\n");
		            
		            RTNA_DBG_Str (0,"Height       : ");
		            RTNA_DBG_Long(0,VideoInfo.Height);
		            RTNA_DBG_Str (0,"\r\n");
		            
                    RTNA_DBG_Str (0,"SampleRate   : ");
		            RTNA_DBG_Long(0,VideoInfo.SampleRate);
		            RTNA_DBG_Str (0,"\r\n");
		            
		            RTNA_DBG_Str (0,"Channels     : ");
		            RTNA_DBG_Long(0,VideoInfo.Channels);
		            RTNA_DBG_Str (0,"\r\n");
		            
		            RTNA_DBG_Str (0,"Total Time   : ");
		            RTNA_DBG_Long(0,VideoInfo.TotalTime);
		            RTNA_DBG_Str (0,"\r\n");
                }
                else if ( ( DCF_OBG_MP3 == DcfInfo.FileType ) || 
                          ( DCF_OBG_WAV == DcfInfo.FileType ) ||
                          ( DCF_OBG_OGG == DcfInfo.FileType ) ||
                          ( DCF_OBG_WMA == DcfInfo.FileType ) )
                {
                    AHC_GetAudioFileAttr(&AudioInfo);
		            
		            RTNA_DBG_Str (0,"SampleRate   : ");
		            RTNA_DBG_Long(0,AudioInfo.sample_rate);
		            RTNA_DBG_Str (0,"\r\n");
		            
		            RTNA_DBG_Str (0,"BitRate      : ");
		            RTNA_DBG_Long(0,AudioInfo.avg_bitrate);
		            RTNA_DBG_Str (0,"\r\n");
		            
		            RTNA_DBG_Str (0,"Channels     : ");
		            RTNA_DBG_Long(0,AudioInfo.channels);
		            RTNA_DBG_Str (0,"\r\n");
		            
		            RTNA_DBG_Str (0,"Total Time   : ");
		            RTNA_DBG_Long(0,AudioInfo.total_time);
		            RTNA_DBG_Str (0,"\r\n");
                }
    
		        break;
		    case 'r':
		        //Get DcfInfo	
            	AHC_GetCurrentDCFObjIndex(&CurrentObjIdx);
            	AHC_GetDCFInfo(CurrentObjIdx, &DcfInfo);
	
	            RTNA_DBG_Str(0,DcfInfo.FileName);
	            RTNA_DBG_Str(0,"\r\n");
	            
	            StrLen = STRLEN(DcfInfo.FileName);
	            
	            MEMSET(DirName, 0, 32);
	            MEMCPY(DirName, DcfInfo.FileName, 17);
	            
	            MEMSET(FileName, 0, 32);
	            MEMCPY(FileName, DcfInfo.FileName + 18, StrLen - 18);
    
                AHC_FileOperation((UINT8*)DirName,(UINT8*)FileName, DCF_FILE_DELETE, NULL,NULL);
	            
	            //AHC_SetMode(AHC_MODE_IDLE);
	            AHC_SetMode(AHC_MODE_THUMB_BROWSER);
	            
		        break;
		    case '1':
		        //playback mode
		        ex_AHC_PlaybackMode();
		        
		        //thumbnail mode
		        AHC_SetMode(AHC_MODE_THUMB_BROWSER);
		        
		        break;
			case 'q':
				break;
			default:
				RTNA_DBG_Str(0, "Unsupport Command, please enter again.\r\n");
			break;
		}
	}
}
//------------------------------------------------------------------------------
//  Function    : AHC_VideoRMode
//  Description : sample code for programmer to test AHC_General.c related functions
//  Note        : 
//------------------------------------------------------------------------------
/** @brief sample code for programmer to test AHC_General.c related functions.
@return void.
*/
void ex_AHC_VideoRMode(){
    MMP_ULONG len;
	MMP_UBYTE c = 0;	
	char      str[20];
	
	AHC_SetMode(AHC_MODE_IDLE);
	
	AHC_SetDisplayWindow(DISPLAY_SYSMODE_VIDEOCAPTURE,AHC_TRUE,0, 0, 0,320,240,0);
		
	AHC_SetDCFFreeChars(0,DCF_SET_FREECHAR,(UINT8*)"MOOV");

	AHC_SetImageSize(VIDEO_CAPTURE_MODE, 1280, 720);

	AHC_PreviewZoomConfig(20, 2);
		        
    AHC_SetMode(AHC_MODE_RECORD_PREVIEW);
		        
	while (c!='q') {
		RTNA_DBG_Str(0, "------------- VideoR Menu ---------------\r\n");
		RTNA_DBG_Str(0, "1  Start video record\r\n");
		RTNA_DBG_Str(0, "2  Pause video record\r\n");
		RTNA_DBG_Str(0, "3  Resume video record\r\n");
		RTNA_DBG_Str(0, "4  Stop  video record and restart preview\r\n");
		RTNA_DBG_Str(0, "5  Digital Zoom In\r\n");
		RTNA_DBG_Str(0, "6  Digital Zoom Out\r\n");
		RTNA_DBG_Str(0, "7  Setting\r\n");
		RTNA_DBG_Str(0, "q  Exit menu ----------------------------\r\n");

        RTNA_DBG_Str(0, "Please Input Command:");
		RTNA_DBG_Gets(str, &len);
        c = str[0];
        RTNA_DBG_Str(0, "\r\n");

		switch(c) {
		    case '1':

		        AHC_ConfigMovie(AHC_CLIP_CONTAINER_TYPE     , AHC_MOVIE_CONTAINER_AVI);		        
		        //video
		        AHC_ConfigMovie(AHC_VIDEO_CODEC_TYPE        , AHC_MOVIE_VIDEO_CODEC_H264);		        
		        AHC_ConfigMovie(AHC_VIDEO_COMPRESSION_RATIO , AHC_VIDEO_COMPRESS_QUALITY_HIGH);
		        AHC_ConfigMovie(AHC_MAX_PFRAME_NUM          , 300);
		        AHC_ConfigMovie(AHC_FRAME_RATE              , 30);		        		        

		        //audio
		        AHC_ConfigMovie(AHC_AUD_CODEC_TYPE          , AHC_MOVIE_AUDIO_CODEC_ADPCM);
		        AHC_ConfigMovie(AHC_AUD_BITRATE             , 12800);
		        AHC_ConfigMovie(AHC_AUD_SAMPLE_RATE         , 32000);
		        AHC_ConfigMovie(AHC_AUD_NUM_CHANNELS        , AHC_AUDIO_CHANNEL_STEREO);

		        AHC_SetMode(AHC_MODE_VIDEO_RECORD);
		        break;
		    case '2':
		        AHC_CaptureClipCmd(AHC_CAPTURE_CLIP_PAUSE, 0);
		        break;
		    case '3':
		        AHC_CaptureClipCmd(AHC_CAPTURE_CLIP_RESUME, 0);
		        break;
		    case '4':
		        //AHC_SetMode(AHC_MODE_IDLE);
		        AHC_SetMode(AHC_MODE_RECORD_PREVIEW);
		        break;
		    case '5':
		        AHC_SetPreviewZoom(VIDEO_CAPTURE_MODE, AHC_SENSOR_LINEAR_ZOOM, AHC_SENSOR_ZOOM_IN, 1);
		        break;
		    case '6':
		        AHC_SetPreviewZoom(VIDEO_CAPTURE_MODE, AHC_SENSOR_LINEAR_ZOOM, AHC_SENSOR_ZOOM_OUT, 1);
		        break;
		    case '7':
		        break;
			case 'q':
			    AHC_SetMode(AHC_MODE_IDLE);
				break;
			default:
				RTNA_DBG_Str(0, "Unsupport Command, please enter again.\r\n");
			break;
		}
	}
}



//------------------------------------------------------------------------------
//  Function    : AHC_DSCMode
//  Description : sample code for programmer to test AHC_General.c related functions
//  Note        : 
//------------------------------------------------------------------------------
/** @brief sample code for programmer to test AHC_General.c related functions.
@return void.
*/
void ex_AHC_DSCMode(){
    MMP_ULONG len;
	MMP_UBYTE c = 0;	
	char      str[20];
	
	AHC_SetMode(AHC_MODE_IDLE);
	
	//Config Digital Zoom
	AHC_PreviewZoomConfig(20, 2);
	
	//Config Thumbnail
	AHC_ConfigCapture(ACC_THUMB_SIZE, THUMB_TYPE_CUSTOMIZE);
	AHC_ConfigCapture(ACC_CUS_THUMB_XSIZE, 160);
	AHC_ConfigCapture(ACC_CUS_THUMB_YSIZE, 120);
	
	//Config TimeStamp
	{
	//NOTICE!! If you want to print the time stamp on JPEG, please check your PLL setting as 240/133MHz, Otherwise, the capture may fail.
	AHC_ConfigCapture(ACC_DATE_STAMP, (AHC_ACC_TIMESTAMP_DISABLE | AHC_ACC_TIMESTAMP_DATE_ONLY | AHC_ACC_TIMESTAMP_BOTTOM_RIGHT | AHC_ACC_TIMESTAMP_FORMAT_1));	
	}
	
	//Config JPEG Quality
	AHC_SetCompressionQuality(90);
	
	//Config Display Window
	AHC_SetDisplayWindow(DISPLAY_SYSMODE_STILLCAPTURE,AHC_TRUE,0, 0, 0,320,240,0);
	
	//Config DSC FreeChar
	AHC_SetDCFFreeChars(0,DCF_SET_FREECHAR,(UINT8*)"PICT");
	
	//Config Capture Resolution
	AHC_SetImageSize(STILL_CAPTURE_MODE, 1600, 1200);	
	
	AHC_SetMode(AHC_MODE_CAPTURE_PREVIEW);
				
	while (c!='q') {
		RTNA_DBG_Str(0, "------------- DSC Menu ---------------\r\n");
		RTNA_DBG_Str(0, "1  Start still capture\r\n");
		RTNA_DBG_Str(0, "2  Start C2C capture\r\n");
		RTNA_DBG_Str(0, "3  Start sequential capture\r\n");
		RTNA_DBG_Str(0, "4  Start draft capture\r\n");
		RTNA_DBG_Str(0, "5  Digital Zoom In\r\n");
		RTNA_DBG_Str(0, "6  Digital Zoom Out\r\n");
		RTNA_DBG_Str(0, "7  Setting\r\n");
		RTNA_DBG_Str(0, "q  Exit menu ----------------------------\r\n");

        RTNA_DBG_Str(0, "Please Input Command:");
		RTNA_DBG_Gets(str, &len);
        c = str[0];
        RTNA_DBG_Str(0, "\r\n");

		switch(c) {
		    case '1':
		        AHC_SetMode(AHC_MODE_STILL_CAPTURE);
		        break;
		    case '2':
		        AHC_SetMode(AHC_MODE_C2C_CAPTURE);
		        break;
		    case '3':
		        AHC_SetSeqCaptureLength(3,0);
		        
		        AHC_SetMode(AHC_MODE_SEQUENTIAL_CAPTURE);
		        break;
		    case '4':
		        AHC_SetMode(AHC_MODE_DRAFT_CAPTURE);
		        break;
		    case '5':
		        AHC_SetPreviewZoom(STILL_CAPTURE_MODE, AHC_SENSOR_LINEAR_ZOOM, AHC_SENSOR_ZOOM_IN, 1);
		        break;
		    case '6':
		        AHC_SetPreviewZoom(STILL_CAPTURE_MODE, AHC_SENSOR_LINEAR_ZOOM, AHC_SENSOR_ZOOM_OUT, 1);
		        break;
		    case '7':
		        break;
			case 'q':
			    AHC_SetMode(AHC_MODE_IDLE);
				break;
			default:
				RTNA_DBG_Str(0, "Unsupport Command, please enter again.\r\n");
			break;
		}
	}
}

void ex_AHC_RawMode(){
    MMP_ULONG len;
	MMP_UBYTE c = 0;	
	char      str[20];
	
	AHC_SetMode(AHC_MODE_IDLE);
	
	//Config Digital Zoom
	AHC_PreviewZoomConfig(20, 2);
	
	//Config Thumbnail
	AHC_ConfigCapture(ACC_THUMB_SIZE, THUMB_TYPE_CUSTOMIZE);
	AHC_ConfigCapture(ACC_CUS_THUMB_XSIZE, 160);
	AHC_ConfigCapture(ACC_CUS_THUMB_YSIZE, 120);
	
	//Config Raw + Jpeg or not
	AHC_ConfigCapture(ACC_CAPTURE_RAW_WITH_JPEG, AHC_TRUE);
	//AHC_ConfigCapture(ACC_CAPTURE_RAW_WITH_JPEG, AHC_FALSE);
	
	//Config JPEG Quality
	AHC_SetCompressionQuality(90);
	
	//Config Display Window
	AHC_SetDisplayWindow(DISPLAY_SYSMODE_STILLCAPTURE,AHC_TRUE,0,0,80,240,160,0);
	
	
	//Config DSC FreeChar
	AHC_SetDCFFreeChars(0,DCF_SET_FREECHAR,(UINT8*)"PICT");
	
	
	
	//Config Capture Resolution
	AHC_SetImageSize(STILL_CAPTURE_MODE, 1600, 1200);
	
	
	AHC_SetMode(AHC_MODE_RAW_PREVIEW);

				
	while (c!='q') {
		RTNA_DBG_Str(0, "------------- DSC Menu ---------------\r\n");
		RTNA_DBG_Str(0, "1  Start Raw capture\r\n");
		RTNA_DBG_Str(0, "q  Exit menu ----------------------------\r\n");

        RTNA_DBG_Str(0, "Please Input Command:");
		RTNA_DBG_Gets(str, &len);
        c = str[0];
        RTNA_DBG_Str(0, "\r\n");

		switch(c) {
		    case '1':
		        AHC_SetMode(AHC_MODE_RAW_CAPTURE);
		        break;
			case 'q':
			    AHC_SetMode(AHC_MODE_IDLE);
				break;
			default:
				RTNA_DBG_Str(0, "Unsupport Command, please enter again.\r\n");
			break;
		}
	}
}

//------------------------------------------------------------------------------
//  Function    : ex_AHC_AudioRecord
//  Description : sample code for programmer to test AHC_General.c related functions
//  Note        : 
//------------------------------------------------------------------------------
/** @brief sample code for programmer to test AHC_General.c related functions.
@return void.
*/
void ex_AHC_AudioRecord(){
    MMP_ULONG len;
	MMP_UBYTE c = 0;	
	char      str[20];

    AHC_SetDCFFreeChars(0,DCF_SET_FREECHAR,(UINT8*)"SOUN");
    
    AHC_ConfigAudioRecFormat(AHC_AUDIO_CODEC_MP3, 44100, AHC_AUDIO_CHANNEL_STEREO, 128000);
    				
	while (c!='q') {
		RTNA_DBG_Str(0, "------------- Audio Record Menu ---------------\r\n");
		RTNA_DBG_Str(0, "1  Start audio record\r\n");
		RTNA_DBG_Str(0, "2  Pause audio record\r\n");
		RTNA_DBG_Str(0, "3  Resume audio record\r\n");
		RTNA_DBG_Str(0, "4  Stop audio record\r\n");
		RTNA_DBG_Str(0, "q  Exit menu ----------------------------\r\n");

        RTNA_DBG_Str(0, "Please Input Command:");
		RTNA_DBG_Gets(str, &len);
        c = str[0];
        RTNA_DBG_Str(0, "\r\n");

		switch(c) {
		    case '1':
		        AHC_RecordAudioCmd(AHC_AUDIO_CODEC_MP3,AHC_AUDIO_RECORD_START);
		        break;
		    case '2':
		        AHC_RecordAudioCmd(AHC_AUDIO_CODEC_MP3,AHC_AUDIO_RECORD_PAUSE);
		        break;
		    case '3':
		        AHC_RecordAudioCmd(AHC_AUDIO_CODEC_MP3,AHC_AUDIO_RECORD_RESUME);
		        break;
		    case '4':
		        AHC_RecordAudioCmd(AHC_AUDIO_CODEC_MP3,AHC_AUDIO_RECORD_STOP);
		        break;
			case 'q':
				break;
			default:
				RTNA_DBG_Str(0, "Unsupport Command, please enter again.\r\n");
			    break;
		}
	}
}
//------------------------------------------------------------------------------
//  Function    : AHC_SetModeTest
//  Description : sample code for programmer to test AHC_General.c related functions
//  Note        : 
//------------------------------------------------------------------------------
/** @brief sample code for programmer to test AHC_General.c related functions.
@return void.
*/
void ex_AHC_SetModeTest(){

#if (MEMORY_POOL_CTL == 0)
//temp
extern void AIHC_SetGUIMemStartAddr(UINT32 addr);
//extern void AIHC_GetTempGUIMemStartAddr(UINT32 *addr);
//extern void AIHC_SetTempDCFMemStartAddr(UINT32 addr);
extern void AIHC_GetTempDCFMemStartAddr(UINT32 *addr);
extern void AIHC_SetTempBrowserMemStartAddr(UINT32 addr);
extern void AIHC_GetTempBrowserMemStartAddr(UINT32 *addr);
#endif

#define DATESTAMP_PRIMARY_WIDTH     320
#define DATESTAMP_PRIMARY_HEIGHT    64
#define DATESTAMP_THUMB_WIDTH       160
#define DATESTAMP_THUMB_HEIGHT      64

    MMP_ULONG           len;
	MMP_UBYTE           c = 0;
	
	#if (MEMORY_POOL_CTL == 0)
	UINT32              MemAddr;	
	#endif
	
	char                str[20];
	#if (EXT_CODEC_SUPPORT == 1)
    AHC_AUDIO_I2S_CFG   ConfigAudioI2S;
    #endif
    
    if ( !bInitBasicAHC ) {
        AHC_Initialize();
        
        #if 1
        AHC_MountStorageMedia(AHC_MEDIA_MMC);
        #endif
        
        #if (MEMORY_POOL_CTL == 0)
        AIHC_GetTempDCFMemStartAddr(&MemAddr);
        #endif
        
        AIHC_DumpDcfDB();
               
        #if (MEMORY_POOL_CTL == 0)
        AIHC_SetTempBrowserMemStartAddr(MemAddr);
        
        AIHC_GetTempBrowserMemStartAddr(&MemAddr);
        
        AIHC_SetGUIMemStartAddr(MemAddr);
        #endif
        
   
        AHC_OSDInit(MMPS_Display_GetConfiguration()->mainlcd.usWidth,
                     MMPS_Display_GetConfiguration()->mainlcd.usHeight,
                     MMPS_Display_GetConfiguration()->mainlcd.usWidth,
                     MMPS_Display_GetConfiguration()->mainlcd.usHeight,
                     DATESTAMP_PRIMARY_WIDTH,
                     DATESTAMP_PRIMARY_HEIGHT,
                     DATESTAMP_THUMB_WIDTH,
                     DATESTAMP_THUMB_HEIGHT,
                     OSD_COLOR_ARGB32,
                     AHC_TRUE);

        //currently, enable RTC will cause capture failed
        #if 0
        AHC_SetParam(PARAM_ID_USE_RTC, AHC_TRUE);
        //2011/02/16, 16 : 00 : 00
        AHC_SetClock(2011,2,16,16,0,0);
        #endif
        
        
        
        #if (EXT_CODEC_SUPPORT == 1)
        ConfigAudioI2S.workingMode      = AHC_AUDIO_I2S_MASTER_MODE;
        ConfigAudioI2S.mclkMode         = AHC_AUDIO_I2S_256FS_MODE;
        ConfigAudioI2S.outputBitSize    = AHC_AUDIO_I2S_OUT_16BIT;
        ConfigAudioI2S.lrckPolarity     = AHC_AUDIO_I2S_LRCK_L_LOW;
        
        AHC_ConfigAudioExtDevice(&ConfigAudioI2S, 44100);
        #endif
        
        
        ex_AHC_Exif_Config();

        bInitBasicAHC = AHC_TRUE;
    }
    
    while (c!='q') {
		RTNA_DBG_Str(0, "------------- SetMode Menu ---------------\r\n");
		
		RTNA_DBG_Str(0, "1  Thumbnail Mode\r\n");
		RTNA_DBG_Str(0, "2  ViderR mode\r\n");
		RTNA_DBG_Str(0, "3  Capture mode\r\n");
		RTNA_DBG_Str(0, "4  Audio record mode\r\n");
		RTNA_DBG_Str(0, "5  Raw Capture mode\r\n");
		RTNA_DBG_Str(0, "q  Exit menu ----------------------------\r\n");

        RTNA_DBG_Str(0, "Please Input Command:");
		RTNA_DBG_Gets(str, &len);
        c = str[0];
        RTNA_DBG_Str(0, "\r\n");

		switch(c) {
			case '1':
			    ex_AHC_ThumbNailMode();
				break;
			case '2':
			    ex_AHC_VideoRMode();
				break;
			case '3':
			    ex_AHC_DSCMode();
				break;
		    case '4':
		        ex_AHC_AudioRecord();
		        break;
		    case '5':
		    	ex_AHC_RawMode();
		    	break;
			case 'q':
				break;
			default:
				RTNA_DBG_Str(0, "Unsupport Command, please enter again.\r\n");
			break;
		}
	}		
}

//------------------------------------------------------------------------------
//  Function    : AHC_GeneralTest
//  Description : sample code for programmer to test AHC_General.c related functions
//  Note        : 
//------------------------------------------------------------------------------
/** @brief sample code for programmer to test AHC_General.c related functions.
@return void.
*/
void AHC_GeneralTest(void)
{
	MMP_ULONG len;
	MMP_UBYTE c = 0;
	char str[20];
	
	while (c!='q') {
		RTNA_DBG_Str(0, "============ AHC General Test ===========\r\n");
		RTNA_DBG_Str(0, "0  Set AHC IDLE mode --------------------\r\n");
		RTNA_DBG_Str(0, "1  Set AHC MSDC mode --------------------\r\n");
		RTNA_DBG_Str(0, "2  Set AHC Capture Preview --------------\r\n");
		RTNA_DBG_Str(0, "3  Set AHC Video Record -----------------\r\n");
		RTNA_DBG_Str(0, "4  Set AHC Play Back Mode ---------------\r\n");
		RTNA_DBG_Str(0, "5  Set AHC Still Mode -------------------\r\n");
		RTNA_DBG_Str(0, "6  Set AHC Video Preview Mode -----------\r\n");
		RTNA_DBG_Str(0, "q  Exit menu ----------------------------\r\n");
		
		RTNA_DBG_Str(0, "Please Input Command:");
		RTNA_DBG_Gets(str, &len);
        c = str[0];
        RTNA_DBG_Str(0, "\r\n");
        switch(c) {
        	case '0':
				AHC_SetMode(AHC_MODE_IDLE);
				break;
			case '1':
				AHC_SetMode(AHC_MODE_USB_MASS_STORAGE);
				break;
			case '2':
				AHC_SetMode(AHC_MODE_CAPTURE_PREVIEW);
				break;
			case '3':
				AHC_SetMode(AHC_MODE_VIDEO_RECORD);
				break;
			case '4':
				AHC_SetMode(AHC_MODE_PLAYBACK);
				break;
			case '5':
				AHC_SetMode(AHC_MODE_STILL_CAPTURE);
				break;
			case '6':
				AHC_SetMode(AHC_MODE_RECORD_PREVIEW);
				break;
			case 'q':
				break;
			default:
				RTNA_DBG_Str(0, "Unsupport Command, please enter again.\r\n");
			break;
        }

	}
}


//------------------------------------------------------------------------------
//  Function    : AHCMenuExample
//  Description : 1st sample code menu for programmer to test AHC related functions
//  Note        : 
//------------------------------------------------------------------------------
/** @brief 1st sample code menu for programmer to test AHC related functions.
@return void.
*/
void AHCMenuExample(void)
{
	MMP_ULONG len;
	MMP_UBYTE c = 0;
	char str[20];
	
	AHC_Initialize();
	while (c!='q') {
		RTNA_DBG_Str(0, "------------- AHC Menu ------------------\r\n");
		RTNA_DBG_Str(0, "1 AHC General Function Test -------------\r\n");
		RTNA_DBG_Str(0, "2 AHC SetMode Test ----------------------\r\n");
		RTNA_DBG_Str(0, "q  Exit menu ----------------------------\r\n");
		RTNA_DBG_Str(0, "Please Input Command:");
		RTNA_DBG_Gets(str, &len);
        c = str[0];
        RTNA_DBG_Str(0, "\r\n");
        switch(c) {
        	case '1':
        		AHC_GeneralTest();
        		break;
            case '2':
                ex_AHC_SetModeTest();
                break;
        	case 'q':
        		break;
        	default:
				RTNA_DBG_Str(0, "Unsupport Command, please enter again.\r\n");
			break;
        }
    }
}
#endif    	

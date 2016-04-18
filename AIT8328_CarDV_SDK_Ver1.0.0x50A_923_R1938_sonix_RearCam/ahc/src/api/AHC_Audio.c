/**
 @file AHC_Audio.c
 @brief AHC audio control Function
 @author 
 @version 1.0
*/
/*===========================================================================
 * Include files
 *===========================================================================*/ 

#include "AHC_Audio.h"
#include "AHC_UF.h"
#include "AHC_DCF.h"
//#include "AHC_Config_SDK.h"
#include "AIHC_DCF.h"
#include "AHC_General.h"
#include "mmpf_audio_ctl.h"
#include "mmps_audio.h"
#include "mmps_aui.h"
#include "ait_utility.h"
#include "wm8971.h"
#include "lib_retina.h"

/** @addtogroup AHC_AUDIO
@{
*/

/*===========================================================================
 * Global varible
 *===========================================================================*/ 

static MMP_UBYTE                    AudioCnxt[6] = "Audio";
static MMP_BOOL                     m_bAudioEof = MMP_FALSE;
static MMPS_AUDIO_I2S_CFG           m_AhcI2Scfg = {MMPS_AUDIO_I2S_MASTER_MODE, MMPS_AUDIO_I2S_256FS_MODE, MMPS_AUDIO_I2S_OUT_16BIT, MMPS_AUDIO_I2S_LRCK_L_LOW};
static MMP_ULONG                    m_ulI2S_SampleRate = 44100;
static UINT8                        m_uiRecAudioFmt = MMPS_AUDIO_MP3_ENCODE;
UINT8                        		m_uiPlayAudioFmt = AHC_AUDIO_PLAY_FMT_MP3;
MMPS_AUDIO_MP3_INFO					m_gsAHCMp3Info;
MMPS_AUDIO_OGG_INFO					m_gsAHCOggInfo;
MMPS_AUDIO_WMA_INFO					m_gsAHCWmaInfo;
MMPS_AUDIO_WAV_INFO					m_gsAHCWavInfo;
MMPS_AUDIO_AAC_INFO					m_gsAHCAacInfo;
AHC_BOOL                     		m_bAHCAudioPlaying = AHC_FALSE;
static MMP_BYTE                     m_bAudioEncodeFileName[MAX_FILE_NAME_SIZE];
static UINT16                       m_usAudioRecDirKey;
static UINT32                       m_ulAudioEncMode[MAX_AUDIO_ENC_FORMAT]    = {1,29,5};
static UINT32                       m_ulAudioEncChans[MAX_AUDIO_ENC_FORMAT]   = {   AHC_AUDIO_CHANNEL_MONO_R,
                                                                                    AHC_AUDIO_CHANNEL_MONO_R,
                                                                                    AHC_AUDIO_CHANNEL_MONO_R};
static AHC_BOOL                     m_bRecordDcfAudioFile = AHC_TRUE;

static const UINT32  m_uiAHCMp3EncParameter[8][2]={
											{32000,160000},{32000,128000},
                                           	{32000,96000}, {32000,64000},
                                           	{44100,160000},{44100,128000},
                                           	{44100,96000}, {44100,64000}};

static const UINT32 m_uiAHCAACEncParameter[56][2] = {
				{16000, 16000},	{16000, 24000},	 {16000, 32000},  {16000, 64000},
				{16000, 96000}, {16000, 128000}, {16000, 160000}, {16000, 192000},
				///////////////////////////////////////////////////////////////////////
				{22050, 16000},	{22050, 24000},  {22050, 32000},  {22050, 64000},
				{22050, 96000},	{22050, 128000}, {22050, 160000}, {22050, 192000}, {22050, 256000},
				///////////////////////////////////////////////////////////////////////
				{24000, 16000},	{24000, 24000},  {24000, 32000},  {24000, 64000},
				{24000, 96000},	{24000, 128000}, {24000, 160000}, {24000, 192000}, {24000, 256000},
				///////////////////////////////////////////////////////////////////////
				{32000, 16000},	{32000, 24000},  {32000, 32000},  {32000, 64000},
				{32000, 96000},	{32000, 128000}, {32000, 160000}, {32000, 192000}, {32000, 256000}, {32000, 320000},
				///////////////////////////////////////////////////////////////////////
				{44100, 16000},	{44100, 24000},  {44100, 32000},  {44100, 64000},
				{44100, 96000},	{44100, 128000}, {44100, 160000}, {44100, 192000}, {44100, 256000}, {44100, 320000},
				///////////////////////////////////////////////////////////////////////
				{48000, 16000},	{48000, 24000},  {48000, 32000},  {48000, 64000},
				{48000, 96000},	{48000, 128000}, {48000, 160000}, {48000, 192000}, {48000, 256000}, {48000, 320000}
			};

static const UINT32 m_uiAHCWAVRecParameter[8] = {8000, 11025, 16000, 22050, 24000, 32000, 44100, 48000};


/*===========================================================================
 * Extern function
 *===========================================================================*/ 
 
extern MMP_ERR MMPC_AudioExtCodec_SetPath(MMP_ULONG path);
extern MMP_ERR MMPC_AudioExtCodec_Init(MMPS_AUDIO_I2S_CFG *I2SConfig, MMP_ULONG samplerate, MMP_ULONG ulFixedMclkFreq);

/*===========================================================================
 * Main body
 *===========================================================================*/ 
 
#if 0
void __INTERNAL_API__(){}
#endif

/**
 @brief The callback function for audio stop 

 The callback function for audio stop 

 @param[in] Context  string
 @param[in] flag1    flag1
 @param[in] flag2    flag2
*/
static void AIHC_AudioStopCBTest(void *Context, MMP_ULONG flag1, MMP_ULONG flag2)
{
    DBG_S(0, "Audio is Stop!!!!!\r\n");
    DBG_S(0, (MMP_BYTE *)Context);
    VAR_L(0, flag1);
    
    m_bAudioEof = MMP_TRUE;
}

/**
 @brief Config the external audio DAC for input patch

 This function  config the external audio DAC for input patch
 
 @retval AHC_TRUE Success.
*/
AHC_BOOL AIHC_InitAudioExtDACIn(void)
{
    #if (EXT_CODEC_SUPPORT == 1)
    MMP_UBYTE                    I2Sch = I2S_CH0;
    MMPS_AUDIO_I2S_CFG           I2Scfg;
	
    if (MMPS_AUDIO_GetInPath() & MMPS_AUDIO_IN_I2S_MASK) {
	    I2Scfg.workingMode      = m_AhcI2Scfg.workingMode;
        I2Scfg.mclkMode         = m_AhcI2Scfg.mclkMode;
        I2Scfg.outputBitSize    = m_AhcI2Scfg.outputBitSize;
        I2Scfg.lrckPolarity     = m_AhcI2Scfg.lrckPolarity;

        I2Scfg.inputBitSize     = MMPS_AUDIO_I2S_IN_16BIT;
        I2Scfg.outDataDelayMode = MMPS_AUDIO_I2S_OUT_DELAY_STD;
        I2Scfg.bOutputBySDI     = MMP_FALSE;
        I2Scfg.bInputFromSDO    = MMP_FALSE;
        I2Scfg.ubInBitAlign     = 0;
        I2Scfg.ubOutBitAlign    = 0;
        #if (CHIP == VSN_V5)
        I2Sch = (MMPS_AUDIO_GetInPath() == MMPS_AUDIO_IN_I2S0) ?
                    I2S_CH0 : I2S_CH1;
        #endif
        MMPS_AUDIO_InitializeI2SConfig(I2Sch,&I2Scfg);
        MMPS_AUDIO_EnableI2SMclk(I2Sch,MMP_TRUE, m_ulI2S_SampleRate, 0);

        MMPC_AudioExtCodec_SetPath(AUDIO_EXT_MIC_IN);
        MMPC_AudioExtCodec_Init(&I2Scfg, m_ulI2S_SampleRate, 0);
    }
    #endif
    
    return AHC_TRUE;
}

/**
 @brief Config the external audio DAC for output patch

 This function  config the external audio DAC for output patch
 
 @retval AHC_TRUE Success.
*/
AHC_BOOL AIHC_InitAudioExtDACOut(void)
{
    #if (EXT_CODEC_SUPPORT == 1)
    MMP_UBYTE                    I2Sch = I2S_CH0;
    MMPS_AUDIO_I2S_CFG           I2Scfg;

    if (MMPS_AUDIO_GetOutPath() & MMPS_AUDIO_OUT_I2S_MASK) {
        I2Scfg.workingMode      = m_AhcI2Scfg.workingMode;
        I2Scfg.mclkMode         = m_AhcI2Scfg.mclkMode;
        I2Scfg.outputBitSize    = m_AhcI2Scfg.outputBitSize;
        I2Scfg.lrckPolarity     = m_AhcI2Scfg.lrckPolarity;

        I2Scfg.inputBitSize   	= MMPS_AUDIO_I2S_IN_16BIT;
        I2Scfg.outputBitSize  	= MMPS_AUDIO_I2S_OUT_16BIT;
        I2Scfg.outDataDelayMode = MMPS_AUDIO_I2S_OUT_DELAY_STD;
        I2Scfg.bOutputBySDI     = MMP_FALSE;
        I2Scfg.bInputFromSDO    = MMP_FALSE;
        I2Scfg.ubInBitAlign     = 0;
        I2Scfg.ubOutBitAlign    = 16;
        #if (CHIP == VSN_V5)
        I2Sch = (MMPS_AUDIO_GetOutPath() == MMPS_AUDIO_OUT_I2S0) ?
                    I2S_CH0 : I2S_CH1;
        #endif
        MMPS_AUDIO_InitializeI2SConfig(I2Sch, &I2Scfg);
        MMPS_AUDIO_EnableI2SMclk(I2Sch, MMP_TRUE, m_ulI2S_SampleRate, 0);

        MMPC_AudioExtCodec_SetPath(AUDIO_EXT_HP_OUT);
        MMPC_AudioExtCodec_Init(&I2Scfg, m_ulI2S_SampleRate, 0);
    }
    #endif
    
    return AHC_TRUE;
}


#if 0
void __AHC_AUDIO_API__(){}
#endif
/**
 @brief Config audio

 This function controls the most important part of audio operation. It defines 
 the configuration of audio port and its capability.
 Parameters:
 @param[in] byPortCFG Configures the basic parts of audio port.
 @param[in] uwSampleRate Sample rate.
 @param[in] uwSDivider Audio bit clock division factor.
 @param[in] uwFDivider Audio frame clock division factor.
 @retval AHC_TRUE Success.
*/
AHC_BOOL AHC_ConfigAudioExtDevice(AHC_AUDIO_I2S_CFG* byPortCFG, UINT16 uwSampleRate)
{
    //MMPS_AUDIO_I2S_CFG I2Scfg;
        
    m_AhcI2Scfg.workingMode      = byPortCFG->workingMode;
    m_AhcI2Scfg.mclkMode         = byPortCFG->mclkMode;
    m_AhcI2Scfg.outputBitSize    = byPortCFG->outputBitSize;
    m_AhcI2Scfg.lrckPolarity     = byPortCFG->lrckPolarity;

    /*
    m_AhcI2Scfg.inputBitSize     = MMPS_AUDIO_I2S_IN_16BIT;
    m_AhcI2Scfg.bOutputBySDI     = MMP_FALSE;
    m_AhcI2Scfg.bInputFromSDO    = MMP_FALSE;
    m_AhcI2Scfg.ubInBitAlign     = 0;
    m_AhcI2Scfg.ubOutBitAlign    = 16;
    */
    m_ulI2S_SampleRate           = uwSampleRate;
    
	/*	        
    MMPS_AUDIO_InitializeI2SConfig(DEFAULT_I2S_CH, &I2Scfg);
    MMPS_AUDIO_EnableI2SMclk(DEFAULT_I2S_CH, AHC_TRUE, uwSampleRate, 0);

    MMPC_AudioExtCodec_SetPath(MMPS_AUDIO_EXT_HP_OUT);
    MMPC_AudioExtCodec_Init(&I2Scfg, uwSampleRate, 0);
	*/        
	return AHC_SUCCESS;
}

/**
 @brief Get audio file attribute

 This API gets the information of an audio file. The file must be selected as 
 current DCF object first.
 @param[out] *pAudioAttr Address for placing the audio attributes.
 @retval AHC_TRUE Success.
*/
AHC_BOOL AHC_GetAudioFileAttr(AUDIO_ATTR *pAudioAttr)
{
    UINT32              CurrentDcfIdx;
    MMP_ERR             err;
    MMPS_AUDIO_FILEINFO fileinfo;
	UINT8 				FileType;
    
    if ( !pAudioAttr )
        return AHC_FALSE;
    
    MEMSET(pAudioAttr, 0, sizeof(AUDIO_ATTR) );
        
    if ( m_bAHCAudioPlaying ) {
        if ( AHC_AUDIO_PLAY_FMT_MP3 == m_uiPlayAudioFmt ) {
            pAudioAttr->sample_rate = m_gsAHCMp3Info.usSampleRate;
            pAudioAttr->channels    = m_gsAHCMp3Info.usChans;
            pAudioAttr->avg_bitrate = m_gsAHCMp3Info.ulBitRate;
            pAudioAttr->total_time  = m_gsAHCMp3Info.ulTotalTime;
        }
        else if ( AHC_AUDIO_PLAY_FMT_WAVE == m_uiPlayAudioFmt ) {
            pAudioAttr->sample_rate = m_gsAHCWavInfo.ulSampleRate;
            pAudioAttr->channels    = m_gsAHCWavInfo.usChans;
            pAudioAttr->avg_bitrate = m_gsAHCWavInfo.ulBitRate;
            pAudioAttr->total_time  = m_gsAHCWavInfo.ulTotalTime;
        }
        else if ( AHC_AUDIO_PLAY_FMT_OGG == m_uiPlayAudioFmt ) {
            pAudioAttr->sample_rate = m_gsAHCOggInfo.usSampleRate;
            pAudioAttr->channels    = m_gsAHCOggInfo.usChans;
            pAudioAttr->avg_bitrate = m_gsAHCOggInfo.ulBitRate;
            pAudioAttr->total_time  = m_gsAHCOggInfo.ulTotalTime;
        }
        else if ( AHC_AUDIO_PLAY_FMT_WMA == m_uiPlayAudioFmt ) {
            pAudioAttr->sample_rate = m_gsAHCWmaInfo.ulSampleRate;
            pAudioAttr->channels    = m_gsAHCWmaInfo.usChans;
            pAudioAttr->avg_bitrate = m_gsAHCWmaInfo.ulBitRate;
            pAudioAttr->total_time  = m_gsAHCWmaInfo.ulTotalTime;
        }
        else if ( AHC_AUDIO_PLAY_FMT_AAC == m_uiPlayAudioFmt ) {
            pAudioAttr->sample_rate = m_gsAHCAacInfo.usSampleRate;
            pAudioAttr->channels    = m_gsAHCAacInfo.usChans;
            pAudioAttr->avg_bitrate = m_gsAHCAacInfo.ulBitRate;
            pAudioAttr->total_time  = m_gsAHCAacInfo.ulTotalTime;
        }
    }
    else {        
        MMPS_AUDIO_SetMediaPath(MMPS_AUDIO_MEDIA_PATH_CARD);

	    AHC_UF_GetCurrentIndex(&CurrentDcfIdx);
        AHC_UF_GetFilePathNamebyIndex(CurrentDcfIdx, fileinfo.bFileName);
		fileinfo.usFileNameLength = STRLEN((char*)fileinfo.bFileName);
		AHC_UF_GetFileTypebyIndex(CurrentDcfIdx, &FileType);	
		
        if ( FileType == DCF_OBG_MP3 ) {
            m_uiPlayAudioFmt = AHC_AUDIO_PLAY_FMT_MP3;
            err = MMPS_AUDIO_OpenFile(fileinfo, &m_gsAHCMp3Info, MMPS_AUDIO_CODEC_MP3);
            
            if (err == MMP_ERR_NONE) {
                pAudioAttr->sample_rate = m_gsAHCMp3Info.usSampleRate;
                pAudioAttr->channels    = m_gsAHCMp3Info.usChans;
                pAudioAttr->avg_bitrate = m_gsAHCMp3Info.ulBitRate;
                pAudioAttr->total_time  = m_gsAHCMp3Info.ulTotalTime;
            
                MMPS_AUDIO_StopPlay();
            }
        }
        else if (FileType == DCF_OBG_WAV ) {
            m_uiPlayAudioFmt = AHC_AUDIO_PLAY_FMT_WAVE;
            err = MMPS_AUDIO_OpenFile(fileinfo, &m_gsAHCWavInfo, MMPS_AUDIO_CODEC_WAV);
            
            if (err == MMP_ERR_NONE) {
                pAudioAttr->sample_rate = m_gsAHCWavInfo.ulSampleRate;
                pAudioAttr->channels    = m_gsAHCWavInfo.usChans;
                pAudioAttr->avg_bitrate = m_gsAHCWavInfo.ulBitRate;
                pAudioAttr->total_time  = m_gsAHCWavInfo.ulTotalTime;
                
                MMPS_AUDIO_StopPlay();
            }
        }
        else if ( FileType == DCF_OBG_OGG ) {
            m_uiPlayAudioFmt = AHC_AUDIO_PLAY_FMT_OGG;
            err = MMPS_AUDIO_OpenFile(fileinfo, &m_gsAHCOggInfo, MMPS_AUDIO_CODEC_OGG);
            
            if (err == MMP_ERR_NONE) {
                pAudioAttr->sample_rate = m_gsAHCOggInfo.usSampleRate;
                pAudioAttr->channels    = m_gsAHCOggInfo.usChans;
                pAudioAttr->avg_bitrate = m_gsAHCOggInfo.ulBitRate;
                pAudioAttr->total_time  = m_gsAHCOggInfo.ulTotalTime;
                
                MMPS_AUDIO_StopPlay();
            }
        }
        else if (FileType == DCF_OBG_WMA ) {
            m_uiPlayAudioFmt = AHC_AUDIO_PLAY_FMT_WMA;
            err = MMPS_AUDIO_OpenFile(fileinfo, &m_gsAHCWmaInfo, MMPS_AUDIO_CODEC_WMA);
            
            if (err == MMP_ERR_NONE) {
                pAudioAttr->sample_rate = m_gsAHCWmaInfo.ulSampleRate;
                pAudioAttr->channels    = m_gsAHCWmaInfo.usChans;
                pAudioAttr->avg_bitrate = m_gsAHCWmaInfo.ulBitRate;
                pAudioAttr->total_time  = m_gsAHCWmaInfo.ulTotalTime;

                MMPS_AUDIO_StopPlay();
            }
        }
        else if (FileType == DCF_OBG_AAC ) {
            m_uiPlayAudioFmt = AHC_AUDIO_PLAY_FMT_AAC;
            err = MMPS_AUDIO_OpenFile(fileinfo, &m_gsAHCAacInfo, MMPS_AUDIO_CODEC_AAC);            
            if (err == MMP_ERR_NONE) {
                pAudioAttr->sample_rate = m_gsAHCAacInfo.usSampleRate;
                pAudioAttr->channels    = m_gsAHCAacInfo.usChans;
                pAudioAttr->avg_bitrate = m_gsAHCAacInfo.ulBitRate;
                pAudioAttr->total_time  = m_gsAHCAacInfo.ulTotalTime;
				
                MMPS_AUDIO_StopPlay();
            }
        }
    }  
    
	return AHC_TRUE;
}

/**
 @brief Capture audio message

 Captures a period of audio message in still image playback mode.
 This recorded audio will be treated as a audio memo for the playing 
 image/thumb file.
 @retval AHC_TRUE Success.
*/
AHC_BOOL AHC_CaptureAudioMessage(void)
{
	return AHC_TRUE;
}

/**
 @brief Stop audio message record

 Stops the audio capture process.
 Parameters:
 @param[out] *pulTime Returns the recorded audio time.
 @retval AHC_TRUE Success.
*/
AHC_BOOL AHC_StopRecording(UINT32 *pulTime)
{
    MMP_ERR retStatus;

    retStatus =  MMPS_AUDIO_StopRecord();
    
    if ( retStatus ) {
        return AHC_FALSE;
    }
    else {
        retStatus = MMPS_AUDIO_GetRecordTime((MMP_ULONG*)pulTime);
        
        if ( m_bRecordDcfAudioFile ) {
            AHC_UF_PreAddFile(m_usAudioRecDirKey,(INT8*)m_bAudioEncodeFileName);
            AHC_UF_PostAddFile(m_usAudioRecDirKey,(INT8*)m_bAudioEncodeFileName);
        }
    }
    
    if ( retStatus )
    	return AHC_FALSE;
    	
    return AHC_TRUE;   
}

/**
 @brief Play audio message

 Plays the corresponded audio message recorded by AHC_CaptrueAudioMessage() in 
 playback mode while the thumb or image is displayed.
 @retval AHC_TRUE Success.
*/
AHC_BOOL AHC_PlayAudioMessage(void)
{
	return AHC_TRUE;
}
/**
 @brief config record audio format

 This API configures the pure audio recording format
 @param[in] 
 @retval AHC_SUCCESS Success.
*/
AHC_BOOL AHC_ConfigAudioRecFormat(UINT8 byAudioFormat, UINT32 uiAudioSampleRate, UINT8 bySelChannel , UINT32 uiBitRate)
{	
	UINT16  i;
    UINT8   Idx;
    
    if ( bySelChannel >= AHC_AUDIO_CHANNEL_MAX )
        return AHC_FALSE;
	
	switch(byAudioFormat){
	
	case AHC_AUDIO_CODEC_MP3:		
	    m_ulAudioEncMode[0]    = 0xFFFF; //default value;
	    
		for ( i = 0; i<(sizeof(m_uiAHCMp3EncParameter)>>2); i++) {
			if ((m_uiAHCMp3EncParameter[i][1] == uiBitRate) && (m_uiAHCMp3EncParameter[i][0] == uiAudioSampleRate)) {
				m_ulAudioEncMode[0] = i;
				break;
			}
		}
		
		Idx = 0;
	    break;
	case AHC_AUDIO_CODEC_AAC:
	    m_ulAudioEncMode[1]    = 0xFFFF; //default value;
	    
		for ( i = 0; i<(sizeof(m_uiAHCAACEncParameter)>>2); i++) {
			if ((m_uiAHCAACEncParameter[i][1] == uiBitRate) && (m_uiAHCAACEncParameter[i][0] == uiAudioSampleRate)) {
				m_ulAudioEncMode[1] = i;
				break;
			}
		}
		
		Idx = 1;
		
	    break;
	case AHC_AUDIO_CODEC_WAV:
        m_ulAudioEncMode[2]    = 0xFFFF; //default value;
        
		for ( i = 0; i<(sizeof(m_uiAHCWAVRecParameter)>>2); i++) {
			if (m_uiAHCWAVRecParameter[i] == uiAudioSampleRate) {
				m_ulAudioEncMode[2] = i;
				break;
			}
		}
		
		Idx = 2;
		
	    break;
	default:
		return AHC_FALSE;
	    break;
	}
	
	if (m_ulAudioEncMode[Idx] == 0xFFFF) {
		//unsupport mode;
		return AHC_FALSE;
	}

	m_ulAudioEncChans[Idx]   = bySelChannel;
	
	return AHC_TRUE;	
}

/**
 @brief Record or play audio message

 This API controls the audio record/play operation.
 @param[in] byOpType Operation for audio playback and record.
 @retval AHC_TRUE Success.
*/
AHC_BOOL AHC_RecordAudioCmd(UINT8 byAudioFormat, UINT8 byOpType)
{
    MMP_ERR             err = MMP_ERR_NONE;
    MMPS_AUDIO_FILEINFO fileinfo;
	MMP_BYTE            DirName[16];
    UINT8               RecFileName[16];
    INT8                RecExtension[4];
    UINT32              MaxRecordSize = 0xFFFFFFFF;
    UINT8               Idx;
	UINT8               bCreateNewDir;
	
    switch( byOpType ){
    
    case AHC_AUDIO_RECORD_START:        
        MEMSET(DirName                  , 0, sizeof(DirName));
    	MEMSET(RecFileName              , 0, sizeof(RecFileName));
    	MEMSET(RecExtension             , 0, sizeof(RecExtension));
    	MEMSET(m_bAudioEncodeFileName   , 0, sizeof(m_bAudioEncodeFileName));
    	
    	if ( AHC_UF_GetName(&m_usAudioRecDirKey, (INT8*)DirName, (INT8*)RecFileName, &bCreateNewDir) == AHC_TRUE) {
    	    STRCPY(fileinfo.bFileName,(INT8*)AHC_UF_GetRootDirName());
    		STRCAT(fileinfo.bFileName,"\\");
            STRCAT(fileinfo.bFileName,DirName);
            
            if ( bCreateNewDir ) {
                MMPS_FS_DirCreate((INT8*)fileinfo.bFileName, STRLEN(fileinfo.bFileName));
                AHC_UF_AddDir(DirName);
            }
            
            STRCAT(fileinfo.bFileName,"\\");
            STRCAT(fileinfo.bFileName,(INT8*)RecFileName);
            STRCAT(fileinfo.bFileName, EXT_DOT);
	    
            switch(byAudioFormat){
        	case AHC_AUDIO_CODEC_MP3:
        	    STRCPY(RecExtension,(MMP_BYTE*)"MP3");
                
                Idx             = 0;                
                m_uiRecAudioFmt = MMPS_AUDIO_MP3_ENCODE;
                
        	    break;
        	case AHC_AUDIO_CODEC_AAC:
        	    STRCPY(RecExtension,(MMP_BYTE*)"AAC");
                
                Idx             = 1;                
                m_uiRecAudioFmt = MMPS_AUDIO_AAC_ENCODE;
        		
        	    break;
        	case AHC_AUDIO_CODEC_WAV:
        	    STRCPY(RecExtension,(MMP_BYTE*)"WAV");
                
                Idx             = 2;                
                m_uiRecAudioFmt = MMPS_AUDIO_WAV_ENCODE;
        	    
        	    break;
        	default:
        		return AHC_FALSE;
        	    break;
        	}
        	
        	if ( 0xFFFF == m_ulAudioEncMode[Idx])
                return AHC_FALSE;
        
            AIHC_InitAudioExtDACIn();
    
        	err = MMPS_AUDIO_SetLineInChannel(m_ulAudioEncChans[Idx]);
        	
            if (MMP_ERR_NONE != err)
                return AHC_FALSE;
            
            STRCAT(fileinfo.bFileName,(MMP_BYTE*)RecExtension);
			fileinfo.usFileNameLength = STRLEN(fileinfo.bFileName);

            STRCPY(m_bAudioEncodeFileName,(INT8*)RecFileName);
            STRCAT(m_bAudioEncodeFileName, EXT_DOT);
            STRCAT(m_bAudioEncodeFileName,(MMP_BYTE*)RecExtension);
            printc("m_bAudioEncodeFileName %s\r\n",m_bAudioEncodeFileName);
            //MMPS_AUDIO_SetRecordVolume();
            
        	MMPS_AUDIO_InitRecord(MMPS_AUDIO_MEDIA_PATH_CARD, &fileinfo, m_uiRecAudioFmt, m_ulAudioEncMode[Idx], MaxRecordSize);
        	if (MMP_ERR_NONE != err)
                return AHC_FALSE;
    	}
    	else {
    	    return AHC_FALSE;
    	    
    	}
	
        err = MMPS_AUDIO_StartRecord();
        
        m_bRecordDcfAudioFile = AHC_TRUE;
        break;
    case AHC_AUDIO_RECORD_PAUSE:
        err = MMPS_AUDIO_PauseRecord();
        break;
    case AHC_AUDIO_RECORD_RESUME:
        err = MMPS_AUDIO_ResumeRecord();
        break;
    case AHC_AUDIO_RECORD_STOP:
        err = MMPS_AUDIO_StopRecord();
		
        printc("MMPS_AUDIO_StopRecord %s\r\n",err);
   
        if(MMP_ERR_NONE == err) {
            if ( m_bRecordDcfAudioFile ) {
                AHC_UF_PreAddFile(m_usAudioRecDirKey,(INT8*)m_bAudioEncodeFileName);
                AHC_UF_PostAddFile(m_usAudioRecDirKey,(INT8*)m_bAudioEncodeFileName);
            }
        }
        
        break;
    }
    
    return AHC_TRUE;
}

/**
 @brief record audio clip by name

 Record a audio clip by the specific name.
 @param[in] byOpType Operation for audio record.
 @retval AHC_SUCCESS Success.
*/
AHC_BOOL AHC_RecordAudioByName(UINT8* pFileName) 
{
    MMP_ERR             err = MMP_ERR_NONE;
    MMPS_AUDIO_FILEINFO fileinfo;
    MMP_BYTE            subName;
    UINT32              Idx;
    UINT32              MaxRecordSize = 0xFFFFFFFF;
	
	STRCPY(fileinfo.bFileName, (char*)(pFileName));
	fileinfo.usFileNameLength = STRLEN((char*)(pFileName));
	
	subName = fileinfo.bFileName[fileinfo.usFileNameLength-1]; //get last char;
	fileinfo.mediaPath = MMPS_AUDIO_MEDIA_PATH_CARD;

	switch (subName){
		case '3':
			Idx             = 0;                
            m_uiRecAudioFmt = MMPS_AUDIO_MP3_ENCODE;
		    break;
		case 'c':
		case 'C':
			Idx             = 1;                
            m_uiRecAudioFmt = MMPS_AUDIO_AAC_ENCODE;
		    break;
		case 'v':
		case 'V':
			Idx             = 2;                
            m_uiRecAudioFmt = MMPS_AUDIO_WAV_ENCODE;
		    break;
		default:
			//unsupport format;
			return AHC_FALSE;
		break;
	}
		
	if ( 0xFFFF == m_ulAudioEncMode[Idx])
        return AHC_FALSE;

    AIHC_InitAudioExtDACIn();
            
	err = MMPS_AUDIO_SetLineInChannel(m_ulAudioEncChans[Idx]);
    if (MMP_ERR_NONE != err)
        return AHC_FALSE;
		            
	MMPS_AUDIO_InitRecord(MMPS_AUDIO_MEDIA_PATH_CARD, &fileinfo, m_uiRecAudioFmt, m_ulAudioEncMode[Idx], MaxRecordSize);
	if (MMP_ERR_NONE != err)
        return AHC_FALSE;
	
    err = MMPS_AUDIO_StartRecord();
    if (MMP_ERR_NONE != err)
        return AHC_FALSE;
        
    m_bRecordDcfAudioFile = AHC_FALSE;
            	    
    return AHC_TRUE;
}

AHC_BOOL AHC_GetAudioCurRecTime(UINT32 *pulTime)
{
	MMP_ERR retStatus;
	
	retStatus = MMPS_AUDIO_GetRecordTime((MMP_ULONG*)pulTime);
	
	if (!retStatus) 
		return AHC_TRUE;
	else 
		return AHC_FALSE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_PlayFadeOutEn
//------------------------------------------------------------------------------
/** 
    @brief  Enable/Disable AUI or NON_AUI player's audio fade out effect  
    @param[in]  channelID type UINT8
                -    0:MMPF_NON_AUI_PLAYER
                -    1:MMPF_AUI_PLAYER
                bAFadeOutEn.
    @return MMP_ERR_NONE
*/
AHC_BOOL AHC_PlayFadeOutEn(UINT8 channelID, AHC_BOOL bAFadeOutEn)
{
    MMPS_AUDIO_PlayFadeOutEn(channelID, bAFadeOutEn);    
	return AHC_TRUE;
}
/**
 @brief Audio playback cmd

 Audio playback cmd.
 
 @param[in] byOpType command
 @param[in] byParam  parameter
 
 @retval AHC_TRUE Success.
*/
AHC_BOOL AHC_PlayAudioCmd(UINT8 byOpType, UINT32 byParam)
{
	UINT32              CurrentDcfIdx;
	MMPS_AUDIO_FILEINFO fileinfo;
	UINT8 				FileType;
	INT32               TargetTime;
    MMP_USHORT          TimeRatio;
    MMP_ULONG           PlayTime;
    MMP_ERR             retStatus;
    UINT32              Param;
	
    switch(byOpType) {
    
        case AHC_AUDIO_PLAY_START:
            AIHC_InitAudioExtDACOut();
	        
            AHC_UF_GetCurrentIndex(&CurrentDcfIdx);
    
	        AHC_UF_GetFilePathNamebyIndex(CurrentDcfIdx, fileinfo.bFileName);
	    
			MMPS_AUDIO_SetMediaPath(MMPS_AUDIO_MEDIA_PATH_CARD);

		    fileinfo.usFileNameLength = STRLEN((char*)fileinfo.bFileName);

			AHC_UF_GetFileTypebyIndex(CurrentDcfIdx, &FileType); 
	        if ( FileType == DCF_OBG_MP3 ) {
	            m_uiPlayAudioFmt = AHC_AUDIO_PLAY_FMT_MP3;
	            retStatus = MMPS_AUDIO_OpenFile(fileinfo, &m_gsAHCMp3Info, MMPS_AUDIO_CODEC_MP3);
	        }
	        else if ( FileType == DCF_OBG_WAV ) {
	            m_uiPlayAudioFmt = AHC_AUDIO_PLAY_FMT_WAVE;
	            retStatus = MMPS_AUDIO_OpenFile(fileinfo, &m_gsAHCWavInfo, MMPS_AUDIO_CODEC_WAV);
	        }
	        else if ( FileType == DCF_OBG_OGG ) {
	            m_uiPlayAudioFmt = AHC_AUDIO_PLAY_FMT_OGG;
	            retStatus = MMPS_AUDIO_OpenFile(fileinfo, &m_gsAHCOggInfo, MMPS_AUDIO_CODEC_OGG);
	        }
	        else if (FileType == DCF_OBG_WMA ) {
	            m_uiPlayAudioFmt = AHC_AUDIO_PLAY_FMT_WMA;
	            retStatus = MMPS_AUDIO_OpenFile(fileinfo, &m_gsAHCWmaInfo, MMPS_AUDIO_CODEC_WMA);
	        }
	        else if (FileType == DCF_OBG_AAC ) {
	            m_uiPlayAudioFmt = AHC_AUDIO_PLAY_FMT_AAC;
	            retStatus = MMPS_AUDIO_OpenFile(fileinfo, &m_gsAHCAacInfo, MMPS_AUDIO_CODEC_AAC);
	        }
	        
	        // set audio volume
            AHC_GetParam(PARAM_ID_AUDIO_VOLUME      ,&Param);
            
            #if (AUDIO_SET_DB == 0x01) 
    	    MMPS_AUDIO_SetPlayVolumeDb(Param);
    	    #else
    	    MMPS_AUDIO_SetPlayVolume(Param, MMP_FALSE);
    	    #endif
	    
	        if ( retStatus == MMP_ERR_NONE ) {
	            retStatus = MMPS_AUDIO_StartPlay((void *)AIHC_AudioStopCBTest, (void *)AudioCnxt);
	        }
	        if ( retStatus != MMP_ERR_NONE )
	            return AHC_FALSE;
	            
	        m_bAHCAudioPlaying = AHC_TRUE;
	        
            break;
        case AHC_AUDIO_PLAY_PAUSE:
        	MMPS_AUDIO_PausePlay();
            break;
        case AHC_AUDIO_PLAY_STOP:
			MMPS_AUDIO_StopPlay();
			m_bAHCAudioPlaying = AHC_FALSE;
		    break;
		//TBD
        case AHC_AUDIO_PLAY_STOP_AUTO:
			MMPS_AUDIO_StopPlay();
			m_bAHCAudioPlaying = AHC_FALSE;
		    break;
        case AHC_AUDIO_PLAY_RESUME:
            MMPS_AUDIO_ResumePlay();
            break;
        case AHC_AUDIO_PLAY_SEEK_BY_TIME:
	       MMPS_AUDIO_SeekByTime(byParam);
            break;
        case AHC_AUDIO_PLAY_FAST_FORWARD:
        	MMPS_AUDIO_GetPlayTime(&TimeRatio, &PlayTime);
	        
	        TargetTime = PlayTime + byParam;
	        if (TargetTime < 0)
	            TargetTime = 0;
	        
	        MMPS_AUDIO_SeekByTime(TargetTime);    
            break;
        case AHC_AUDIO_PLAY_FAST_FORWARD_RATE:
            break;
        case AHC_AUDIO_PLAY_SET_VOLUME:
            #if (AUDIO_SET_DB == 0x01) 
    	    MMPS_AUDIO_SetPlayVolumeDb(byParam);
    	    #else
    	    MMPS_AUDIO_SetPlayVolume(byParam, MMP_FALSE);
    	    #endif
            break;
        case AHC_AUDIO_PLAY_2ND_START:
        	MMPS_AUI_StartPlay(&m_uiPlayAudioFmt);
        	m_bAHCAudioPlaying = AHC_TRUE;
            break;
		case AHC_AUDIO_PLAY_2ND_STOP_AUTO:
			MMPS_AUI_StopPlay(&m_uiPlayAudioFmt);
			m_bAHCAudioPlaying = AHC_FALSE;
		    break;
        default:
            //unsupport command;
            return AHC_FALSE;
            break;
    }
    return AHC_TRUE;
}

/**
 @brief Play audio clip by name

 Play a audio clip by the specific name.
 @param[in] byOpType Operation for audio playback.
 @retval AHC_SUCCESS Success.
*/
AHC_BOOL AHC_PlayAudioByName(UINT8 *pFileName, UINT8 byOpType)
{
    UINT32              Param;
	MMP_ERR             retStatus;
	MMPS_AUDIO_FILEINFO fileinfo;
	MMP_BYTE            subName;
	
    switch(byOpType) {
    
        case AHC_AUDIO_PLAY_START:
            AIHC_InitAudioExtDACOut();
	        
			MMPS_AUDIO_SetMediaPath(MMPS_AUDIO_MEDIA_PATH_CARD);

		    #if (FS_INPUT_ENCODE == UCS2)
		    uniStrcpy(fileinfo.bFileName, pFileName);
			fileinfo->usFileNameLength = uniStrlen((short *)fileinfo->bFileName);
		    #elif (FS_INPUT_ENCODE == UTF8)
		    STRCPY(fileinfo.bFileName, (char*)pFileName);
			fileinfo.usFileNameLength = STRLEN((char*)pFileName);
			#endif
			
			subName = fileinfo.bFileName[fileinfo.usFileNameLength-1]; //get last char;
			switch (subName){
				case 'a':
				case 'A':
					m_uiPlayAudioFmt = AHC_AUDIO_PLAY_FMT_WMA;
	            	retStatus = MMPS_AUDIO_OpenFile(fileinfo, &m_gsAHCWmaInfo, MMPS_AUDIO_CODEC_WMA);
				break;
				case '3':
					m_uiPlayAudioFmt = AHC_AUDIO_PLAY_FMT_MP3;
		            retStatus = MMPS_AUDIO_OpenFile(fileinfo, &m_gsAHCMp3Info, MMPS_AUDIO_CODEC_MP3);
				break;
				case 'v':
				case 'V':
					m_uiPlayAudioFmt = AHC_AUDIO_PLAY_FMT_WAVE;
		            retStatus = MMPS_AUDIO_OpenFile(fileinfo, &m_gsAHCWavInfo, MMPS_AUDIO_CODEC_WAV);
				break;
                case 'C':
                case 'c':
					m_uiPlayAudioFmt = AHC_AUDIO_PLAY_FMT_AAC;
		            retStatus = MMPS_AUDIO_OpenFile(fileinfo, &m_gsAHCAacInfo, MMPS_AUDIO_CODEC_AAC);
				break;
				
				case 'g':
				case 'G':
					m_uiPlayAudioFmt = AHC_AUDIO_PLAY_FMT_OGG;
		            retStatus = MMPS_AUDIO_OpenFile(fileinfo, &m_gsAHCOggInfo, MMPS_AUDIO_CODEC_OGG);
				break;
				default:
					//unsupport format;
					return AHC_FALSE;
				break;
			}
	        
	        // set audio volume
            AHC_GetParam(PARAM_ID_AUDIO_VOLUME      ,&Param);
            
            #if (AUDIO_SET_DB == 0x01) 
    	    MMPS_AUDIO_SetPlayVolumeDb(Param);
    	    #else
    	    MMPS_AUDIO_SetPlayVolume(Param, MMP_FALSE);
    	    #endif
    	    
	        if ( retStatus == MMP_ERR_NONE ) {
	            retStatus = MMPS_AUDIO_StartPlay((void *)AIHC_AudioStopCBTest, (void *)AudioCnxt);
	        }
	        if ( retStatus != MMP_ERR_NONE )
	            return AHC_FALSE;
            break;
        default:
            //unsupport command;
            return AHC_FALSE;
        break;
    }
    return AHC_TRUE;
}

/**
 @brief Config audio annotation

 Configures the audio annotation feature and ON/OFF upon the value of bOn. A kind
  of memo for still captured image.
 @param[in] bValue Enable/disable this feature.
 @param[in] uwPre Number of seconds to pre-record before capture.
 @param[in] uwAfter Number of seconds to record after capture.
 @retval AHC_TRUE Success.
*/
AHC_BOOL AHC_ConfigAudioAnnotation(AHC_BOOL bValue, UINT16 uwPre, UINT16 uwAfter)
{
	return AHC_TRUE;
}

/**
 @brief Stop audio annotation

 Terminates the audio annotation recording.
 @retval AHC_TRUE Success.
*/
AHC_BOOL AHC_AudioAnnotationStop(void)
{
	return AHC_TRUE;
}

/**
 @brief Select audio unit

 Selects the HW audio unit for current operation.
 @param[in] eAudId The audio units..
 @retval AHC_TRUE Success.
*/

AHC_BOOL AHC_SelectAudioUnit(AHC_AUDIO_UINT_ID eAudId)
{ 
	MMP_ERR mmpstatus;
	
	switch(eAudId) {
	
		case AHC_AUDIO_EXTERNAL_CODEC:
			mmpstatus = MMPS_AUDIO_InitialInPath(MMPS_AUDIO_IN_I2S0);
			if (mmpstatus)
				return AHC_FALSE;
			mmpstatus =MMPS_AUDIO_InitialOutPath(MMPS_AUDIO_OUT_I2S0);
			if (mmpstatus)
				return AHC_FALSE;
		break;
		case AHC_AUDIO_INTERNAL_CODEC:
			MMPS_AUDIO_InitialInPath(MMPS_AUDIO_IN_AFE_SING);
			if (mmpstatus)
				return AHC_FALSE;
			MMPS_AUDIO_InitialOutPath(MMPS_AUDIO_OUT_AFE_HP);
			if (mmpstatus)
				return AHC_FALSE;
		break;
		case AHC_AUDIO_HDMI:
		break;
		default:
			//unsupport unit
			return AHC_FALSE;
		break;
	}
	return AHC_TRUE;
}

/**
 @brief Get audio unit

 Get the ID of current operating audio codec.
 @param[out] *peAudId The audio units.
 @retval AHC_TRUE Success.
*/
AHC_BOOL AHC_GetCurrentAudioUnit(AHC_AUDIO_UINT_ID *peAudId)
{
    MMP_ERR mmpstatus;
    MMP_UBYTE tmpCodec;

	mmpstatus = MMPS_AUDIO_GetCurCodec(&tmpCodec);
    
    if ( mmpstatus ) {
        return AHC_FALSE;
	}
	else {
		switch(tmpCodec){
			case 0:
				*peAudId = AHC_AUDIO_INTERNAL_CODEC;
			break;
			case 1:
				*peAudId = AHC_AUDIO_EXTERNAL_CODEC;
			break;
			case 2:
				*peAudId = AHC_AUDIO_HDMI;
			break;
			default:
				return AHC_FALSE;
			break;
		}
    	return AHC_TRUE;
	}
	return AHC_TRUE;
}

/**
 @brief Get audio current time

 This API gets the current time of playing audio file.
 @param[in] *pulTime The current time of playing audio file.
 @retval AHC_TRUE Success.
*/
AHC_BOOL AHC_GetAudioCurrentTime(UINT32 *pulTime)
{
	MMP_ERR retStatus;
	MMP_USHORT timeRatio;
	
	retStatus = MMPS_AUDIO_GetPlayTime(&timeRatio, (MMP_ULONG*)pulTime);
	
	if (!retStatus) 
		return AHC_TRUE;
	else 
		return AHC_FALSE;
}


/**
 @brief Set the multiplier of audio recording

 This API set the multiplier of audio recording
 @param[in] fMultiplier, if set to 2.00, the audio value
 multiply to 2.
 @retval AHC_TRUE Success.
 @note   Now only work on adpcm format.
*/
AHC_BOOL AHC_AudioSetRecMultiplier(float fMultiplier )
{

    MMP_SHORT sMultiplier;

#if defined(SA_PL)

    if( fMultiplier < 0.0f || fMultiplier > 100.0f)
    {
        return AHC_FALSE;
    }

    fMultiplier *= 100;

    sMultiplier = (MMP_SHORT)fMultiplier;

    //MMPF_Audio_SetSoftwareGain(sMultiplier); //Rogers:***

    return AHC_TRUE;

#else
    return AHC_FALSE;
#endif //defined(SA_PL)
    

}

AHC_BOOL AHC_Audio_ParseOptions(MMPS_3GPRECD_AUDIO_OPTION option, MMPS_3GPRECD_AUDIO_FORMAT *format,
        MMP_ULONG *samplingRate, MMP_ULONG *bitRate)
{
    __packed struct MMPS_AUDIO_OPTIONS {
        MMPS_3GPRECD_AUDIO_OPTION option;
        MMPS_3GPRECD_AUDIO_FORMAT format;
        MMP_ULONG samplingRate;
        MMP_ULONG bitRate;
    };
    static const struct MMPS_AUDIO_OPTIONS options[] = {
        {MMPS_3GPRECD_AUDIO_AAC_16K_32K,     MMPS_3GPRECD_AUDIO_FORMAT_AAC, 16000,  32000},
        {MMPS_3GPRECD_AUDIO_AAC_16K_64K,     MMPS_3GPRECD_AUDIO_FORMAT_AAC, 16000,  64000},
        {MMPS_3GPRECD_AUDIO_AAC_22d05K_64K,  MMPS_3GPRECD_AUDIO_FORMAT_AAC, 22050,  64000},
        {MMPS_3GPRECD_AUDIO_AAC_22d05K_128K, MMPS_3GPRECD_AUDIO_FORMAT_AAC, 22050, 128000},
        {MMPS_3GPRECD_AUDIO_AAC_32K_64K,     MMPS_3GPRECD_AUDIO_FORMAT_AAC, 32000,  64000},
        {MMPS_3GPRECD_AUDIO_AAC_32K_128K,    MMPS_3GPRECD_AUDIO_FORMAT_AAC, 32000, 128000},
        {MMPS_3GPRECD_AUDIO_AAC_48K_128K,    MMPS_3GPRECD_AUDIO_FORMAT_AAC, 48000, 128000},
        {MMPS_3GPRECD_AUDIO_AAC_44d1K_64K,   MMPS_3GPRECD_AUDIO_FORMAT_AAC, 44100,  64000},
        {MMPS_3GPRECD_AUDIO_AAC_44d1K_128K,  MMPS_3GPRECD_AUDIO_FORMAT_AAC, 44100, 128000},
        {MMPS_3GPRECD_AUDIO_AMR_4d75K,       MMPS_3GPRECD_AUDIO_FORMAT_AMR,  8000,   4750},
        {MMPS_3GPRECD_AUDIO_AMR_5d15K,       MMPS_3GPRECD_AUDIO_FORMAT_AMR,  8000,   5150},
        {MMPS_3GPRECD_AUDIO_AMR_12d2K,       MMPS_3GPRECD_AUDIO_FORMAT_AMR,  8000,  12200},
        //ADPCM section, need to sync with MMPF_SetADPCMEncMode
        {MMPS_3GPRECD_AUDIO_ADPCM_16K_22K,   MMPS_3GPRECD_AUDIO_FORMAT_ADPCM, 16000,  22000},
        {MMPS_3GPRECD_AUDIO_ADPCM_32K_22K,   MMPS_3GPRECD_AUDIO_FORMAT_ADPCM, 32000,  22000},
        {MMPS_3GPRECD_AUDIO_ADPCM_44d1K_22K, MMPS_3GPRECD_AUDIO_FORMAT_ADPCM, 44100,  22000},
        {MMPS_3GPRECD_AUDIO_MP3_32K_128K,    MMPS_3GPRECD_AUDIO_FORMAT_MP3,   32000, 128000},
        {MMPS_3GPRECD_AUDIO_MP3_44d1K_128K,  MMPS_3GPRECD_AUDIO_FORMAT_MP3,   44100, 128000},
    };
    int i;

    for (i = 0; i < sizeof(options) / sizeof(options[0]); ++i) {
        if (options[i].option == option) {
            *format = options[i].format;
            *samplingRate = options[i].samplingRate;
            *bitRate = options[i].bitRate;
            return AHC_TRUE;
        }
    }
    *format = *samplingRate = *bitRate = 0;
    return AHC_FALSE;
}

/// @}


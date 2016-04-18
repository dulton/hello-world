
/*===========================================================================
 * Include file 
 *===========================================================================*/ 

#include "config_fw.h"
#include "mmpf_i2cm.h"
#include "os_wrap.h"
#include "mmpf_pio.h"
#include "mmpf_fs_api.h"
#include "mmpf_timer.h"
#include "lib_retina.h"
#include "AHC_utility.h"
#include "ait_utility.h"
#include "AHC_Fs.h"
#include "AHC_Menu.h"
#include "AHC_Video.h"
#include "GSensor_ctrl.h"
#include "GPS_Ctl.h"
#include "AHC_Gsensor.h"
#include "mmpf_gsensor.h"

// Add below define since too many header files have the define.
#ifdef GSENSOR_DATA_LEN
    #undef GSENSOR_DATA_LEN
    #define GSENSOR_DATA_LEN sizeof(GSNR_INFOCHUCK)
#endif

#if (GSENSOR_CONNECT_ENABLE)

#if 0  // For debug
#define DBG_GSENSOR_PRINT       printc
#else
#define DBG_GSENSOR_PRINT(...)
#endif

/*===========================================================================
 * Global varible
 *===========================================================================*/

UINT32 					m_ulInfoFileId			= NULL;
UINT32 					m_ulInfoFileByte		= 0;
UINT32 					m_ulInfoLastByte		= 0;
float 					GValueInNmea[3] = {0, 0, 0};
static MMP_UBYTE 		m_ubStartVRWithGsnrInfo = MMP_FALSE;
static MMP_ULONG 		m_ulGsnrCount_0			= 0;
static MMP_ULONG 		m_ulGsnrCount_1			= 0;
static MMP_ULONG 		m_ulGsnrCount_Last		= 0;
static MMP_ULONG 		m_ulGsnrInfoChuckCnt_0 	= 0;
static MMP_ULONG 		m_ulGsnrInfoChuckCnt_1 	= 0;
static MMP_UBYTE 		m_ubGsnrBufIndex 		= 0;
static MMP_UBYTE 		m_ubGsnrBufIndex_Store	= 0;
static MMP_UBYTE 		m_ubGsnrBufIndex_Last	= 0;
static MMP_UBYTE 		m_ubGsnrBufIndex_Backup = 0;
static MMP_BOOL  		m_bGsnrPingPongBufStart = MMP_FALSE;
static MMP_BOOL  		m_bGsensor_TriggerRestorePreRecordInfo = MMP_FALSE;
static MMP_BOOL  		m_bGsensor_FlushBackupBuffer = MMP_FALSE;
static MMP_BOOL  		m_bGsensor_UseLastData = MMP_FALSE;
static MMP_ULONG 		m_ulGsnrPreRecordTime	= 0;
static MMP_UBYTE 		m_ubGsnrBufCounter 		= 0;
static GSNR_CHUCKHDR 	GsnrChuckHdr[1];
static GSNR_CHUCKHDR 	GsnrInfoChuckHdr[1];
static GSNR_INFOCHUCK 	GsnrInfoChuckBuf_0[GSNR_MAX_INFONUM] = {0};
static GSNR_INFOCHUCK 	GsnrInfoChuckBuf_1[GSNR_MAX_INFONUM] = {0};

#if (VR_VIDEO_TYPE == COMMON_VR_VIDEO_TYPE_3GP)
    static UINT32 Gsensor_Convert_Endian(UINT32 d);

	#define	GSENSOR_BIGENDIAN(d)	Gsensor_Convert_Endian(d + 8) // 8 is FOURCC + sizeof(unsigned int) for 3GP container
#else
	#define	GSENSOR_BIGENDIAN(d)	(d)
#endif
/*===========================================================================
 * Extern varible
 *===========================================================================*/


/*===========================================================================
 * Main body
 *===========================================================================*/

#if 0
void _____AVI_Chunk_Function_________(){ruturn;} //dummy
#endif

static UINT32 Gsensor_Convert_Endian(UINT32 d)
{
	UINT32 t = d;	// 8 is FOURCC + sizeof(unsigned int) for 3GP container
	*((char*)&d + 3) = *((char*)&t + 0);
	*((char*)&d + 2) = *((char*)&t + 1);
	*((char*)&d + 1) = *((char*)&t + 2);
	*((char*)&d + 0) = *((char*)&t + 3);
	return d;
}

void AHC_Gsensor_ResetBufCtrlVariable(void)
{
	m_ulGsnrCount_0			= 0;
	m_ulGsnrCount_1			= 0;
	m_ulGsnrInfoChuckCnt_0 	= 0;
	m_ulGsnrInfoChuckCnt_1 	= 0;
	m_ubGsnrBufIndex 		= 0;
	m_bGsnrPingPongBufStart = MMP_FALSE;
	m_ubGsnrBufCounter 		= 0;
#if (GSNR_USE_FILE_AS_DATABUF)
	m_ulInfoFileByte 		= 0;
	m_ulInfoLastByte		= 0;
#endif	 
}

void AHC_Gsensor_OpenInfoFile(INT8 *ubFileName)
{
#if (GSNR_USE_FILE_AS_DATABUF)
	
	if(m_ulInfoFileId == NULL)
    {   
            // Change "w+b" to "a+b" Means to append the same file position.
	    if(AHC_FS_FileOpen(ubFileName, AHC_StrLen(ubFileName), "a+b", AHC_StrLen("a+b"), &m_ulInfoFileId))
        //if( MMPF_FS_FOpen(ubFileName, "w+b", &m_ulInfoFileId) != MMP_ERR_NONE )
        {
            m_ulInfoFileId = NULL;
            return;
        }
    }
	
	AHC_FS_FileSeek(m_ulInfoFileId, 0, AHC_FS_SEEK_SET);
    //MMPF_FS_FSeek(m_ulInfoFileId, 0, MMP_FS_FILE_BEGIN);	
	
	AHC_Gsensor_ResetBufCtrlVariable();
    DBG_GSENSOR_PRINT("%s: m_ulInfoFileId = 0x%X\r\n", __func__, m_ulInfoFileId);
#endif
}

void AHC_Gsensor_CloseInfoFile(void)
{
#if (GSNR_USE_FILE_AS_DATABUF)	
    if(m_ulInfoFileId != NULL){   
        AHC_FS_FileClose(m_ulInfoFileId);
    }
    m_ulInfoFileId = 0;
#endif    
}

void AHC_Gsensor_StartRecord(void)
{
    MMP_BOOL gsnr_inited=AHC_FALSE;
    MMPF_3RDParty_Gsensor_initiated(&gsnr_inited);
    m_ubStartVRWithGsnrInfo = gsnr_inited;

    DBG_GSENSOR_PRINT("### %s -\r\n", __func__);

#if (GSNR_USE_FILE_AS_DATABUF)

    //AHC_FS_FileOpen(GSNR_TEMP_INFOFILE, AHC_StrLen(GSNR_TEMP_INFOFILE), "w+b", AHC_StrLen("w+b"), &m_ulInfoFileId);

	m_bGsnrPingPongBufStart = MMP_TRUE;

#else

	if(m_ubGsnrBufCounter)
		m_bGsnrPingPongBufStart = MMP_TRUE;
	else
		m_ubGsnrBufCounter++;
	
#endif	
}

void AHC_Gsensor_EndRecord(void)
{
    DBG_GSENSOR_PRINT("### %s -\r\n", __func__);
    
#if (GSNR_USE_FILE_AS_DATABUF)

	if(m_ulGsnrCount_0 != 0 && m_ulGsnrCount_0 < GSNR_MAX_INFONUM)
	{	
		m_ulInfoLastByte = m_ulGsnrCount_0*GSENSOR_DATA_LEN;
        m_ubGsnrBufIndex_Store = 0;
        m_ubGsnrBufIndex = 1;
	}
	else if(m_ulGsnrCount_1 != 0 && m_ulGsnrCount_1 < GSNR_MAX_INFONUM)
	{	
		m_ulInfoLastByte = m_ulGsnrCount_1*GSENSOR_DATA_LEN;
        m_ubGsnrBufIndex_Store = 1;
        m_ubGsnrBufIndex = 0;
	}
	
#else

	if(m_ubGsnrBufIndex)
	{
		m_ubGsnrBufIndex 		= 0;
 	 	m_ulGsnrInfoChuckCnt_1 	= m_ulGsnrCount_1;
 	}
 	else
 	{
 	 	m_ubGsnrBufIndex 		= 1;
 	 	m_ulGsnrInfoChuckCnt_0 	= m_ulGsnrCount_0;
 	}

	if(m_bGsnrPingPongBufStart)
	{
	 	if(m_ubGsnrBufIndex)
	 		m_ulGsnrCount_1 = 0;
	 	else
	 		m_ulGsnrCount_0 = 0;
	}
#endif	
	m_ubStartVRWithGsnrInfo = MMP_FALSE;
}

//------------------------------------------------------------------------------
//  Function    : AHC_Gsensor_EndRecord_Backup
//  Description : It will record G-Sensor data index for current video, 
//                and keep recording G-Sensor data. (Reset nothing.)
//------------------------------------------------------------------------------
void AHC_Gsensor_EndRecord_Backup(void)
{
	if(m_ulGsnrCount_0 != 0 && m_ulGsnrCount_0 < GSNR_MAX_INFONUM)
	{	
		m_ulInfoLastByte = m_ulGsnrCount_0*sizeof(GSNR_INFOCHUCK);
	}
	else if(m_ulGsnrCount_1 != 0 && m_ulGsnrCount_1 < GSNR_MAX_INFONUM)
	{	
		m_ulInfoLastByte = m_ulGsnrCount_1*sizeof(GSNR_INFOCHUCK);
	}

    m_ubGsnrBufIndex_Backup = m_ubGsnrBufIndex;
}

//-------------------------------------------------------------------------------------------
//  Function    : AHC_Gsensor_RestorePreRecordInfo
//  Description : Copy Gsensor data from start of Pre-record to beginning of G-sensor buffer
//-------------------------------------------------------------------------------------------
void AHC_Gsensor_RestorePreRecordInfo( UINT32 ulPreRecordTime, MMP_BOOL bUseLastData )
{
    UINT32 ulPreRecordNumber = ( ulPreRecordTime * AHC_Gsensor_GetAttributeAddr()->ubStoreFreq ) / 1000;

    if( ulPreRecordNumber > GSNR_MAX_INFONUM )
    {
        printc(FG_RED("ERROR: Too many Pre-record data for G-sensor buffer (%d) !!!!!!!!!!!\r\n"), ulPreRecordNumber);
        return;
    }

    printc(FG_BLUE("-- AHC_Gsensor_RestorePreRecordInfo-- sec:%d, freq:%d, buffer:%d !!!!!!!!!!!\r\n"), ulPreRecordTime,AHC_Gsensor_GetAttributeAddr()->ubStoreFreq,ulPreRecordNumber);

    m_ulInfoFileByte = 0; // Reset m_ulInfoFileByte since it's not used

    if( m_ubGsnrBufIndex == 0 )
    {
        if( m_ulGsnrCount_0 < ulPreRecordNumber ) // Need to copy data from ping-pong buffer
        {
            UINT32 ulCopyNumber = ulPreRecordNumber - m_ulGsnrCount_0;
            if( bUseLastData == MMP_TRUE )
            {
                if( m_ubGsnrBufIndex_Last == 0)
                {
                    memcpy( &GsnrInfoChuckBuf_1[0], &GsnrInfoChuckBuf_0[m_ulGsnrCount_Last - ulCopyNumber], ulCopyNumber*sizeof(GSNR_INFOCHUCK) );
                }
                else
                {
                    memcpy( &GsnrInfoChuckBuf_1[0], &GsnrInfoChuckBuf_1[m_ulGsnrCount_Last - ulCopyNumber], ulCopyNumber*sizeof(GSNR_INFOCHUCK) );
                }
            }
            else
            {
                memcpy( &GsnrInfoChuckBuf_1[0], &GsnrInfoChuckBuf_1[GSNR_MAX_INFONUM - ulCopyNumber], ulCopyNumber*sizeof(GSNR_INFOCHUCK) );
            }
            memcpy( &GsnrInfoChuckBuf_1[ulCopyNumber], &GsnrInfoChuckBuf_0[0], m_ulGsnrCount_0*sizeof(GSNR_INFOCHUCK) );
        }
        else
        {
            memcpy( &GsnrInfoChuckBuf_1[0], &GsnrInfoChuckBuf_0[m_ulGsnrCount_0 - ulPreRecordNumber], ulPreRecordNumber*sizeof(GSNR_INFOCHUCK) );
        }

        m_ubGsnrBufIndex = 1;
        m_ulGsnrCount_1 = ulPreRecordNumber;
        m_ulGsnrCount_0 = 0;
    }
    else //m_ubGsnrBufIndex == 1
    {
        if( m_ulGsnrCount_1 < ulPreRecordNumber ) // Need to copy data from ping-pong buffer
        {
            UINT32 ulCopyNumber = ulPreRecordNumber - m_ulGsnrCount_1;
            if( bUseLastData == MMP_TRUE )
            {
                if( m_ubGsnrBufIndex_Last == 0)
                {
                    memcpy( &GsnrInfoChuckBuf_0[0], &GsnrInfoChuckBuf_0[m_ulGsnrCount_Last - ulCopyNumber], ulCopyNumber*sizeof(GSNR_INFOCHUCK) );
                }
                else
                {
                    memcpy( &GsnrInfoChuckBuf_0[0], &GsnrInfoChuckBuf_1[m_ulGsnrCount_Last - ulCopyNumber], ulCopyNumber*sizeof(GSNR_INFOCHUCK) );
                }
            }
            else
            {
                memcpy( &GsnrInfoChuckBuf_0[0], &GsnrInfoChuckBuf_0[GSNR_MAX_INFONUM - ulCopyNumber], ulCopyNumber*sizeof(GSNR_INFOCHUCK) );
            }
            memcpy( &GsnrInfoChuckBuf_0[ulCopyNumber], &GsnrInfoChuckBuf_1[0], m_ulGsnrCount_1*sizeof(GSNR_INFOCHUCK) );
        }
        else
        {
            memcpy( &GsnrInfoChuckBuf_0[0], &GsnrInfoChuckBuf_1[m_ulGsnrCount_1 - ulPreRecordNumber], ulPreRecordNumber*sizeof(GSNR_INFOCHUCK) );
        }

        m_ubGsnrBufIndex = 0;
        m_ulGsnrCount_0 = ulPreRecordNumber;
        m_ulGsnrCount_1 = 0;
    }
}

void AHC_Gsensor_RestoreAviChuck(void)
{

    AHC_GSENSOR_DATA 	sdata;
	GSNR_SBYTE		xyz[GSENSOR_DATA_LEN] = {0};
	MMP_UBYTE       i;
	MMP_UBYTE		dummy;
	UINT32			ulTimelimit;

	if(m_ubStartVRWithGsnrInfo)
	{
	    #if (GSENSOR_DETECT_MODE == GSNESOR_DETECT_DIFF)
		AHC_Gsensor_IOControl(GSNR_CMD_READ_ACCEL_WITH_COMPENSATION, (MMP_UBYTE*) &sdata);	
		#else
		AHC_Gsensor_IOControl(GSNR_CMD_READ_ACCEL_XYZ, (MMP_UBYTE*) &sdata);
		#endif

		for(i = 0; i<GSENSOR_DATA_LEN; i++)
		{
			xyz[i] = (signed char)(sdata.acc_val[i]);
		}

        AHC_GetCurrentRecordingTime(&ulTimelimit);

        if( AHC_Gsensor_NeedToRestorePreRecordInfo( &ulTimelimit ) == MMP_TRUE )
        {
            m_bGsensor_TriggerRestorePreRecordInfo = MMP_FALSE;
				
            AHC_Gsensor_RestorePreRecordInfo( ulTimelimit, m_bGsensor_UseLastData );
            m_bGsensor_UseLastData = MMP_FALSE;
        }

	   	if(m_ubGsnrBufIndex)
		{
		   	if((m_ulGsnrCount_1 + m_ulInfoFileByte/GSENSOR_DATA_LEN) < ulTimelimit/1000 * AHC_Gsensor_GetAttributeAddr()->ubStoreFreq)
		   	{
		   		dummy = ulTimelimit/1000 * (AHC_Gsensor_GetAttributeAddr()->ubStoreFreq) - (m_ulGsnrCount_1 + m_ulInfoFileByte/GSENSOR_DATA_LEN); 
		   		do{
                    GsnrInfoChuckBuf_1[m_ulGsnrCount_1].sbX = xyz[0];
                    GsnrInfoChuckBuf_1[m_ulGsnrCount_1].sbY = xyz[1];
                    GsnrInfoChuckBuf_1[m_ulGsnrCount_1].sbZ = xyz[2];
                    
                    
                    m_ulGsnrCount_1++;
                    dummy--;

		   		}while((dummy > 0) && (m_ulGsnrCount_1 < GSNR_MAX_INFONUM));

		   	}
		   	else
		   	{
		   		GsnrInfoChuckBuf_1[m_ulGsnrCount_1].sbX = xyz[0];
			   	GsnrInfoChuckBuf_1[m_ulGsnrCount_1].sbY = xyz[1];
			   	GsnrInfoChuckBuf_1[m_ulGsnrCount_1].sbZ = xyz[2];
			   	m_ulGsnrCount_1++;
		   	}
#if (GSNR_USE_FILE_AS_DATABUF)
			if(m_ulGsnrCount_1 == GSNR_MAX_INFONUM)
			{
				UINT32 ulWriteCount;

                DBG_GSENSOR_PRINT("%s - m_ulInfoFileId=0x%X\r\n", __func__, m_ulInfoFileId);

				AHC_FS_FileWrite(m_ulInfoFileId, (UINT8*)GsnrInfoChuckBuf_1, GSNR_MAX_INFONUM*GSENSOR_DATA_LEN, &ulWriteCount);
				//MMPF_FS_FWrite(m_ulInfoFileId, (UINT8*)GsnrInfoChuckBuf_1, GSNR_MAX_INFONUM*GSENSOR_DATA_LEN, &ulWriteCount);
				m_ulInfoFileByte += ulWriteCount;
				
				m_ulGsnrCount_1 	= 0;//Reset
				m_ubGsnrBufIndex 	= 0;//Toggle
				DBG_GSENSOR_PRINT("Buf1-m_ulInfoFileByte %d\r\n",m_ulInfoFileByte);
			}
#endif		   	
	   	}
	   	else
	   	{
	   		if((m_ulGsnrCount_0 + m_ulInfoFileByte/GSENSOR_DATA_LEN) < ulTimelimit/1000 * AHC_Gsensor_GetAttributeAddr()->ubStoreFreq)
	   		{
	   			
		   		dummy = ulTimelimit/1000 * (AHC_Gsensor_GetAttributeAddr()->ubStoreFreq) - (m_ulGsnrCount_0 + m_ulInfoFileByte/GSENSOR_DATA_LEN); 
		   		do{
                    GsnrInfoChuckBuf_0[m_ulGsnrCount_0].sbX = xyz[0];
                    GsnrInfoChuckBuf_0[m_ulGsnrCount_0].sbY = xyz[1];
                    GsnrInfoChuckBuf_0[m_ulGsnrCount_0].sbZ = xyz[2];
                    m_ulGsnrCount_0++;
                    dummy--;
		   		}while((dummy > 0) && (m_ulGsnrCount_0 < GSNR_MAX_INFONUM));

		   	
	   		}
	   		else
	   		{
		   		GsnrInfoChuckBuf_0[m_ulGsnrCount_0].sbX = xyz[0];
			   	GsnrInfoChuckBuf_0[m_ulGsnrCount_0].sbY = xyz[1];
			   	GsnrInfoChuckBuf_0[m_ulGsnrCount_0].sbZ = xyz[2];
			   	m_ulGsnrCount_0++;
		   	}
#if (GSNR_USE_FILE_AS_DATABUF)
			if(m_ulGsnrCount_0 == GSNR_MAX_INFONUM)
			{
				UINT32 ulWriteCount;

                DBG_GSENSOR_PRINT("%s - m_ulInfoFileId=0x%X\r\n", __func__, m_ulInfoFileId);

				AHC_FS_FileWrite(m_ulInfoFileId, (UINT8*)GsnrInfoChuckBuf_0, GSNR_MAX_INFONUM*GSENSOR_DATA_LEN, &ulWriteCount);
                DBG_GSENSOR_PRINT("%s - m_ulInfoFileId = %X\r\n", __func__, m_ulInfoFileId);
                //MMPF_FS_FWrite(m_ulInfoFileId, (UINT8*)GsnrInfoChuckBuf_0, GSNR_MAX_INFONUM*GSENSOR_DATA_LEN, &ulWriteCount);
                
				m_ulInfoFileByte += ulWriteCount;

				m_ulGsnrCount_0		= 0;//Reset
				m_ubGsnrBufIndex	= 1;//Toggle
				DBG_GSENSOR_PRINT("%s - Buf0-m_ulInfoFileByte %d\r\n", __func__, m_ulInfoFileByte);
			}
#endif
	   	}
	}
}

PGSNR_INFOCHUCK AHC_Gsensor_GetInfoChuckAddr(void)
{ 
	if(m_bGsnrPingPongBufStart)
	{
		if(m_ubGsnrBufIndex)
			return GsnrInfoChuckBuf_0;
		else
			return GsnrInfoChuckBuf_1;
	}
	else
	{
		if(m_ubGsnrBufIndex)
			return GsnrInfoChuckBuf_0;
		else
			return GsnrInfoChuckBuf_1;
	}
}

MMP_ULONG AHC_Gsensor_GetInfoChuckSize(void)
{
	MMP_ULONG ulReturnValue;

#if (GSNR_USE_FILE_AS_DATABUF)
	ulReturnValue = m_ulInfoFileByte + m_ulInfoLastByte;
#else
    if(m_bGsnrPingPongBufStart)
    {
	    if(m_ubGsnrBufIndex)
	    	ulReturnValue = m_ulGsnrInfoChuckCnt_0;
	    else
	    	ulReturnValue = m_ulGsnrInfoChuckCnt_1;
	}
	else
	{
		if(m_ubGsnrBufIndex)
	    	ulReturnValue = m_ulGsnrInfoChuckCnt_0;
	    else
	    	ulReturnValue = m_ulGsnrInfoChuckCnt_1;
	}
	ulReturnValue *= sizeof(GSNR_INFOCHUCK);
#endif	
	DBG_GSENSOR_PRINT("GSENSORINFO_SIZE:%d\r\n",ulReturnValue);	
	return ulReturnValue;
}

PGSNR_CHUCKHDR AHC_Gsensor_GetChuckHeaderAddr(MMP_UBYTE ubSelection)
{
	if(ubSelection == 0)
		return GsnrInfoChuckHdr;
	else
		return GsnrChuckHdr;
}

void AHC_Gsensor_SetInfoChuckHeader(void)
{
	GsnrChuckHdr[0].ulFourCC = GSENSOR_MAKE_FCC('g','s','e','n');
	GsnrChuckHdr[0].ulSize   = GSENSOR_BIGENDIAN(AHC_Gsensor_GetInfoChuckSize());
}

void AHC_Gsensor_SetAttribChuckHeader(void)
{
	GsnrInfoChuckHdr[0].ulFourCC = GSENSOR_MAKE_FCC('g','s','e','a');
	GsnrInfoChuckHdr[0].ulSize   = GSENSOR_BIGENDIAN(sizeof(GSNR_ATTRIBUTE));
}

PGSNR_ATTRIBUTE AHC_Gsensor_GetAttributeAddr(void)
{
	return MMPF_3RDParty_Gsensor_GetAttribute();
}

PGSNR_INFOCHUCK AHC_Gsensor_GetLastDataAddr(void)
{		
	if( m_ubGsnrBufIndex_Store == 0 )
	{	
    	m_ulGsnrCount_Last    = m_ulGsnrCount_0;
        m_ubGsnrBufIndex_Last = 0;
		DBG_GSENSOR_PRINT("Flush Buffer0 size %d\r\n",m_ulInfoLastByte);
		m_ulGsnrCount_0 = 0;
		return GsnrInfoChuckBuf_0;
	}
	else if( m_ubGsnrBufIndex_Store == 1 )
	{
    	m_ulGsnrCount_Last    = m_ulGsnrCount_1;
        m_ubGsnrBufIndex_Last = 1;
		DBG_GSENSOR_PRINT("Flush Buffer1 size %d\r\n",m_ulInfoLastByte);
		m_ulGsnrCount_1 = 0;
		return GsnrInfoChuckBuf_1;
	}
	return NULL;
}

PGSNR_INFOCHUCK AHC_Gsensor_GetBackupDataAddr(void)
{		
    if( m_ubGsnrBufIndex_Backup == 0 )
    {
        return GsnrInfoChuckBuf_0;
    }
    else
    {
        return GsnrInfoChuckBuf_1;
    }
}

#endif

#if 0
void _____RAW_Write_Function_________(){ruturn;} //dummy
#endif
#if ((GSENSOR_RAW_FILE_ENABLE == 1) && ((GPS_RAW_FILE_ENABLE == 1) || (GPS_RAW_FILE_EMER_EN == 1)))
extern int			sprintf(char * /*s*/, const char * /*format*/, ...);
extern MMP_SHORT	TempGValue[3];
extern MMP_ULONG	ulGPSRawEmerStartAddr ;
MMP_BOOL AHC_Gsensor_RawWrite(MMP_BOOL bWriteFile)//(MMP_BYTE *pBuff, MMP_USHORT usLength)
{
    
	MMP_ERR			err;
    GSENSOR_DATA	sdata;
	float			xyz[GSENSOR_DATA_LEN] = {0};
	MMP_UBYTE		i;
	//MMP_ULONG		ulWriteSize = 0;
	MMP_ULONG		ulFileId	   = 0;
	MMP_ULONG		ulFileId_Emer  = 0;
	char			buffer[32] = {0}, temp[10] = {0};

	#if (GSENSOR_MODULE == GSENSOR_MODULE_LIS3DH)

		#if (GSENSOR_DETECT_MODE == GSNESOR_DETECT_DIFF)
		xyz[0] = TempGValue[0];
		xyz[1] = TempGValue[1];
		xyz[2] = TempGValue[2];

		TempGValue[0] = 0;
		TempGValue[1] = 0;
		TempGValue[2] = 0;
		#else
		{
			MMP_ERR 			err;
			AHC_GSENSOR_DATA	sdata = {0, 0, 0};
			UINT32				CurSensitivity = 0;

			err = AHC_Gsensor_IOControl(GSNR_CMD_READ_ACCEL_XYZ, (MMP_UBYTE*) &sdata);
			xyz[0] = (abs((signed char)(sdata.acc_val[0])) * 100) / GSENSOR_ACCELERATION_UNIT_LIS3DH;
			xyz[1] = (abs((signed char)(sdata.acc_val[1])) * 100) / GSENSOR_ACCELERATION_UNIT_LIS3DH;
			xyz[2] = (abs((signed char)(sdata.acc_val[2])) * 100) / GSENSOR_ACCELERATION_UNIT_LIS3DH;
		}
		#endif

	#elif (GSENSOR_MODULE == GSENSOR_MODULE_DMARD)

	err = AHC_Gsensor_IOControl(GSNR_CMD_READ_ACCEL_WITH_COMPENSATION, (MMP_UBYTE*) &sdata);

	xyz[0] = (sdata.acc_val[0] * 100) / GSENSOR_ACCELERATION_UNIT_DMARD06;
	xyz[1] = (sdata.acc_val[1] * 100) / GSENSOR_ACCELERATION_UNIT_DMARD06;
	xyz[2] = (sdata.acc_val[2] * 100) / GSENSOR_ACCELERATION_UNIT_DMARD06;

	#elif ((GSENSOR_MODULE == GSENSOR_MODULE_GMA301) || (GSENSOR_MODULE == GSENSOR_MODULE_GMA302))

	err = AHC_Gsensor_IOControl(GSNR_CMD_READ_ACCEL_WITH_COMPENSATION, (MMP_UBYTE*) &sdata);

	xyz[0] = (sdata.acc_val[0] * 100) / GSENSOR_ACCELERATION_UNIT_GMA301;
	xyz[1] = (sdata.acc_val[1] * 100) / GSENSOR_ACCELERATION_UNIT_GMA301;
	xyz[2] = (sdata.acc_val[2] * 100) / GSENSOR_ACCELERATION_UNIT_GMA301;
	
	#elif (GSENSOR_MODULE == GSENSOR_MODULE_GMA305) 

	err = AHC_Gsensor_IOControl(GSNR_CMD_READ_ACCEL_WITH_COMPENSATION, (MMP_UBYTE*) &sdata);
    
    //printc("sdata.acc_val[0]=%d,sdata.acc_val[1]=%d,sdata.acc_val[2]=%d\r\n", sdata.acc_val[0],sdata.acc_val[1],sdata.acc_val[2]);

    for(i=0;i<3;i++) {
	    xyz[i] = (sdata.acc_val[i] * 100) / 8;//GSENSOR_ACCELERATION_UNIT_GMA301;
		xyz[i] = (xyz[i] > 1600)? xyz[i] - 3200: xyz[i];
		xyz[i] = xyz[i]/2;
    }
    
    //printc("xyz[0]=%5.3f,xyz[1]=%5.3f,xyz[2]=%5.3f\r\n", xyz[0]/100, xyz[1]/100, xyz[2]/100);
    
    #elif (GSENSOR_MODULE == GSENSOR_MODULE_MC3256 || GSENSOR_MODULE == GSENSOR_MODULE_DA312)
			err = AHC_Gsensor_IOControl(GSNR_CMD_READ_ACCEL_XYZ, (MMP_UBYTE*) &sdata);
			xyz[0] = (abs((signed char)(sdata.acc_val[0])) *100) / GSENSOR_ACCELERATION_UNIT_DMARD06;
			xyz[1] = (abs((signed char)(sdata.acc_val[1])) *100) / GSENSOR_ACCELERATION_UNIT_DMARD06;
			xyz[2] = (abs((signed char)(sdata.acc_val[2])) *100) / GSENSOR_ACCELERATION_UNIT_DMARD06;

    		if(dump_GValues)
        		printc("G-XYZ: %03d %03d %03d\r\n", (signed short) xyz[0], (signed short) xyz[1], (signed short) xyz[2]);
	#elif (GSENSOR_MODULE == GSENSOR_MODULE_RT3000)
					err = AHC_Gsensor_IOControl(GSNR_CMD_READ_ACCEL_XYZ, (MMP_UBYTE*) &sdata);
					xyz[0] = (abs((signed char)(sdata.acc_val[0])) *100) / GSENSOR_ACCELERATION_UNIT_GMA301;
					xyz[1] = (abs((signed char)(sdata.acc_val[1])) *100) / GSENSOR_ACCELERATION_UNIT_GMA301;
					xyz[2] = (abs((signed char)(sdata.acc_val[2])) *100) / GSENSOR_ACCELERATION_UNIT_GMA301;
			
					if(1)
						printc("G-XYZ: %03d %03d %03d\r\n", (signed short) xyz[0], (signed short) xyz[1], (signed short) xyz[2]);

	#endif

	//AHC_FS_FileWrite(ulFileId, (UINT8 *)GSENSOR_RAW_HEADER, sizeof(GSENSOR_RAW_HEADER), &ulWriteSize);
	STRCPY(buffer,GSENSOR_RAW_HEADER);
	snprintf(temp,9, "%5.3f,", xyz[0] / 100);
	STRCAT(buffer,temp);
	//STRCAT(buffer, ",");
	MEMSET0(temp);
	snprintf(temp, 9, "%5.3f,", xyz[1] / 100);
	STRCAT(buffer,temp);
	//STRCAT(buffer, ",");
	MEMSET0(temp);
	snprintf(temp, 9, "%5.3f", xyz[2] / 100);
	STRCAT(buffer, temp);
	STRCAT(buffer, "\r");
	STRCAT(buffer, "\n");
	//printc("sizeof(buffer) : %d\r\n", sizeof(buffer));

	GValueInNmea[0] = xyz[0] / 100;
	GValueInNmea[1] = xyz[1] / 100;
	GValueInNmea[2] = xyz[2] / 100;

	for (i = 0; i < 32; i++)
	{
		if (STRCMP(buffer + i, "\n") == 0)
			break;
	}
	if (bWriteFile)
	{
		#if (SUPPORT_PARKINGMODE != PARKINGMODE_NONE)
		if (!uiGetParkingModeEnable())
		#endif
		{
			ulFileId_Emer = GPSCtrl_GetGPSRawFileId_Emer();
			if (ulFileId_Emer != NULL)
			{
				{
					GPSCtrl_GPSRawWriteFlushBuf_Emer();
					if (ulGPSRawEmerStartAddr != 0)
					{
						ulGPSRawEmerStartAddr = 0;
					}
				}
				#if (EMERGENCY_RECODE_FLOW_TYPE == EMERGENCY_RECODE_SWITCH_FILE)
				GPSCtrl_GPSRawResetBuffer();
				#endif
			}
		}
		ulFileId = GPSCtrl_GetGPSRawFileId();
		if (ulFileId != NULL)
		{
			{
				GPSCtrl_GPSRawFlushBuf();
			}
		}
	}
	else
	{
		#if (SUPPORT_PARKINGMODE != PARKINGMODE_NONE)
		if(uiGetParkingModeRecordState() || !uiGetParkingModeEnable())
		#endif
		{
			GPSCtrl_GPSRawWriteBuffer(buffer, i + 1);
			GPSCtrl_GPSRawWriteBuffer_Emer(buffer, i + 1);
		}

		#if ((EMERGENCY_RECODE_FLOW_TYPE == EMERGENCY_RECODE_SWITCH_FILE) || (C005_PARKING_CLIP_Length_10PRE_10REC == 1))
			#if (C005_PARKING_CLIP_Length_10PRE_10REC == 0)
			GPSCtrl_GPSRawWriteBuffer_Normal(buffer, i + 1);
			#else
				#if (SUPPORT_PARKINGMODE != PARKINGMODE_NONE)
				if(uiGetParkingModeEnable())
					GPSCtrl_GPSRawWriteBuffer_Normal(buffer, i + 1);
				#endif
			#endif
		#endif
	}
	return TRUE;
}
#endif

#if (GSENSOR_CONNECT_ENABLE)
void AHC_Gsensor_TriggerRestorePreRecordInfo( MMP_BOOL bTrigger, UINT32 ulPreRecordTime, MMP_BOOL bUseLastData )
{
    m_bGsensor_TriggerRestorePreRecordInfo = bTrigger;
    m_ulGsnrPreRecordTime = ulPreRecordTime;
    m_bGsensor_UseLastData = bUseLastData;
}

MMP_BOOL AHC_Gsensor_NeedToRestorePreRecordInfo( UINT32* ulPreRecordTime )
{
    if( m_ulGsnrPreRecordTime != GSENSOR_DEFAULT_PRERECORD_TIME )
    {
        *ulPreRecordTime = m_ulGsnrPreRecordTime;
    }

    return m_bGsensor_TriggerRestorePreRecordInfo;
}

void AHC_Gsensor_FlushBackupBuffer( MMP_BOOL bEnable )
{
    m_bGsensor_FlushBackupBuffer = bEnable;
}

MMP_BOOL AHC_Gsensor_NeedToFlushBackupBuffer( void )
{
    return m_bGsensor_FlushBackupBuffer;
}
#endif

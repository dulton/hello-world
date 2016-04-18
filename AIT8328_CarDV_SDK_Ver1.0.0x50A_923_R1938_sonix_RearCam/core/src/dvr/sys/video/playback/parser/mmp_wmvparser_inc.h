
#ifndef _MMP_WNVPARSER_INC_H_
#define _MMP_WMVPARSER_INC_H_

#if defined(BUILD_FW)
#if (WMV_P_EN) || (VWMA_P_EN)
// ++ Will_Chen
// For WMV
#include "mmpf_wmvdecapi.h"
#include "wmvdec_interface.h"

typedef enum MMPF_WMVPSR_CACHE_INDEX{
	WMV_VIDEO_CACHE = 0,                                ///< video data depack buffer index
	WMV_AUDIO_CACHE,                                    ///< audio data depack buffer index
	WMV_INDEX_CACHE
}MMPF_WMVPSR_CACHE_INDEX;

MMP_WMV_SAMPLE_INFO gWMVVideoSample;
MMP_WMV_SAMPLE_INFO gWMVAudioSample;
extern unsigned char m_bWMVAdvance;

#if 0
void _____WMVPSR_APIS_____(){}
#endif

#include "wmc_type.h"
#define WMVFILE_BUFFER_SIZE (1024)
U8_WMC *FileBuffer;

U32_WMC WMCDecCBGetBSData(U8_WMC *pBuf, U32_WMC nOffset, U32_WMC NumRead)
{
    MMP_ERR error;
    U32_WMC cbRead =0;
    
    error = MMPF_VIDPSR_ReadFile(pBuf, nOffset, NumRead);
    if ( error != MMP_ERR_NONE ){
        return (U32_WMC)0;
    }
    cbRead = NumRead;
    return cbRead;
}
U32_WMC WMCDecCBGetData ( void * hWMCDec, U64_WMC nOffset,  U32_WMC nNumBytesRequest, U8_WMC **ppData, U32_WMC u32UserContext )
{
    U32_WMC cbRead =0;
    MMP_ERR error;

    if (nNumBytesRequest > MIN_WANTED){
        *ppData = NULL_WMC;
        return (U32_WMC)0;
    }
/*
    if(NULL != pInput) {
        iRet = fseek( pInput, (U32_WMC)nOffset, SEEK_SET );
        if (iRet !=0){
            *ppData = NULL_WMC;
            return (U32_WMC)0;
        }
        cbRead  = fread( FileBuffer, 1, nNumBytesRequest, pInput );
        *ppData = FileBuffer;
    }
*/
    if(((nOffset + nNumBytesRequest) > gVidFileInfo.file_size) && (gVidFileInfo.file_size != 0)) {
        *ppData = NULL_WMC;
        return (U32_WMC)0;
    }
        
    error = MMPF_VIDPSR_ReadFile(FileBuffer, (U32_WMC)nOffset, nNumBytesRequest);

    if ( error != MMP_ERR_NONE ){
        *ppData = NULL_WMC;
        return (U32_WMC)0;
    }
    *ppData = FileBuffer;
    cbRead = nNumBytesRequest;
    return cbRead;
}

U32_WMC WMCDecCBGetDataIndex ( void * hWMCDec, U64_WMC nOffset,  U32_WMC nNumBytesRequest, U8_WMC *pData, U32_WMC u32UserContext )
{
    U32_WMC cbRead =0;
    MMP_ERR error;
    MMP_ULONG ulCache;
    
    ulCache = m_ulCacheIdx;
    MMPF_VIDPSR_SwapDepackCacheIdx(WMV_INDEX_CACHE);
    
    error = MMPF_VIDPSR_ReadFile(pData, (U32_WMC)nOffset, nNumBytesRequest);

    if ( error != MMP_ERR_NONE ){
        return (U32_WMC)0;
    }
    cbRead = nNumBytesRequest;
    MMPF_VIDPSR_SwapDepackCacheIdx(m_ulCacheIdx);
    return cbRead;
}

/** @brief reset the parser variables

Reset the variable so that the file could be parsed again.
*/
MMP_ERR MMPF_WMVPSR_Init(void)
{
    MMP_ULONG ulTemp;
    MMPF_3GPP_SEEKSTATUS *pSeek = &m_seek[m_ubVideoCurTrack];
	MEMSET0(&gWMVAudioSample);
	MEMSET0(&gWMVVideoSample);
	MEMSET0(&m_seek);
	///- Reset the media info
    //MEMSET0(&gVideoMedia);
    //MEMSET0(&gAudioMedia);
    //WMVVideoSendTime = 0;
    //WMVAudioSendTime = 0;
    MEMSET0(&m_3GPPdata);

    // Keep the file size for memory mode
    ulTemp = gVidFileInfo.file_size;
    
    MEMSET0(&gVidFileInfo);
    
    gVidFileInfo.file_size = ulTemp;
	
	pSeek->sSpeed               = 10; // default set to 1x speed
    //m_3GPPdata.bEmptyBufferDone = MMP_TRUE;
    //gl3gpAudioSize              = 0;
	gDemuxOption                = 0;
	
    return MMP_ERR_NONE;
}

MMP_ERR MMPF_WMVPSR_Deinit(void)
{
    #if (AIT_WMDRM_SUPPORT == 1)
    wmvdec_uninit();
    #endif
    return MMP_ERR_NONE;
}

/** @brief Seek the next video frame

It reads VOP address, seeks the next frame, marks EOS and reports error
@return The error status of the function
@retval MMP_M_ERR_EOS End of video bitstream.
@retval MMP_M_ERR_INSUFFICIENT_RESOURCES When the input bitstream buffer overflow.
@retval MMP_M_ERR_OVERFLOW In ring buffer payload, it has to wait the others to free the buffer
@retval MMP_M_ERR_INCORRECT_STATE_OPERATION The seek mode is not supported.
*/

static MMP_ERR MMPF_WMVPSR_SeekNextVideoFrame(MMP_ULONG *RequestFrame)
{
    MMP_ERR error;
    MMPF_3GPP_SEEKSTATUS *pSeek = &m_seek[m_ubVideoCurTrack];
    *RequestFrame = 0;
    MMPF_VIDPSR_SwapDepackCacheIdx(WMV_VIDEO_CACHE);

    ///- Get video presentation time offset for following decoded bitstream
    /// See PTS/DTS in MPEG for further study.
    if (pSeek->mode == MMPF_M_SEEK_NORMAL) {
        error = MMPF_WMVPSR_GetNextSample(WMV_VIDEO_TYPE, &gWMVVideoSample);
    }
    else if (pSeek->mode == MMPF_M_FF_KEY) {
        #if 1
        error = MMPF_WMVDEC_WMVSeekNextKeyFrame(pSeek->sSpeed/100, RequestFrame, &gWMVVideoSample);
        if (error == MMP_VIDPSR_ERR_EOS) {
			// Penguin 20080128
//            error = MMP_ERR_NONE; // continue to play so not return EOS
            pSeek->mode = MMPF_M_SEEK_NONE;
        }
        #endif
    }
    else if (pSeek->mode == MMPF_M_SEEK_NONE) {
        return MMP_VIDPSR_ERR_EOS;
    }
    else {
        return MMP_VIDPSR_ERR_INCORRECT_STATE_OPERATION;
    }

    // calculate the offsets before and after the bitstream in all cases
    #if (CHIP == MCR_V2)
    m_3GPPdata.usBSOffset = sizeof(MMPF_M_FRAME_INFO);
    // ARM address padding
    m_3GPPdata.usEOFPadding = (4 - ((gWMVVideoSample.ulSize + m_3GPPdata.usBSOffset) % 4)) % 4 + RING_BUF_TAILING;
    #endif
    
    if ( m_bBsSupportLargeFrame ) {
        // if it's a large frame, use fixed header 0x20;
        if ((gWMVVideoSample.ulSize + m_3GPPdata.usBSOffset + m_3GPPdata.usEOFPadding) > m_ulVideoStreamLen){
            m_3GPPdata.usBSOffset = 0x20;
        }
    }

    return error;
    #if (RING_BUF_TAILING % 4) != 0
        #error RING_BUF_TAILING must be a multiple of 4
    #endif
    // here we've alreay known the size and offset of the next frame
    // update the frame pointer first to know if right address to read the frame
}

/** @brief Get the current time in ms from the media info

@param[in] pMedia The media info pointer.
@return The current time in ms.
*/
static MMP_ULONG MMPF_WMVPSR_GetTime(const MMPF_WMVPSR_MEDIA_TYPE Media)
{
    //MMP_ULONG ulTmpTime;
    if(Media == WMV_VIDEO_TYPE){
        //ulTmpTime = WMVVideoSendTime;
        //WMVVideoSendTime += gWMVVideoSample.lSampleTime;
        //return ulTmpTime;
        return gWMVVideoSample.lSampleTime;
    }

    return 0;
}

/** @brief Get single frame

Get single VOP from the bitsteam to the bitstream buffer and 
assign the starting position to the HW device.
@note Save code size from MP4V(122)+H263(126) to 156Bytes = 92bytes in O2
*///@param[in] videoType Video type. Currently only supports H.263 and MP4V
static MMP_ERR MMPF_WMVPSR_GetSingleFrame(  MMP_ULONG dest_addr,MMP_ULONG lSizeToLoad,
                                            MMP_ULONG seg1     ,MMP_ULONG seg2,
                                            MMP_ULONG *container_offset)
{
    MMP_ERR     error = MMP_ERR_NONE;
    MMP_ULONG   ulRingTailing = 0;
    MMP_UBYTE   *buf_start = m_VideoStreamBuf;
	MMP_ULONG   ulExtraByte = 0;
	
	if (m_bWMVAdvance) {
		ulExtraByte = 4;
	}
	else {
		ulExtraByte = 0;
	}
	
    // Swap cache index to video data
    //MMPF_VIDPSR_SwapDepackCacheIdx(VIDEO_CACHE);
    #if 0
    if(lSizeToLoad > seg1){
        DBG_S(3, "WMV Not Support Ring Buffer\r\n");
        return MMP_VIDPSR_ERR_CONTENT_CORRUPT;
    }
    MEMCPY((MMP_UBYTE *)dest_addr, (MMP_UBYTE *)(*container_offset), lSizeToLoad);
    
    if ( m_bBsSupportLargeFrame ) {
    
        *container_offset += lSizeToLoad;
        #if RING_BUF_TAILING > 0
        if (!m_3GPPdata.bLargeFrame) {
            MMPF_BS_AccessVideoHeader((MMP_UBYTE*)(dest_addr + lSizeToLoad), 
                                     (MMP_UBYTE*)&ulRingTailing, 4, RING_WRITE);
        }      
        #endif
    }
    #endif
    #if 1
        // Need wrap
        if ((lSizeToLoad + ulExtraByte)> seg1 ) {

            if(ulExtraByte) {
            	*((MMP_UBYTE *)(dest_addr+0)) = 0x00;
            	*((MMP_UBYTE *)(dest_addr+1)) = 0x00;
            	*((MMP_UBYTE *)(dest_addr+2)) = 0x01;
            	*((MMP_UBYTE *)(dest_addr+3)) = 0x0D;
            	
            	MEMCPY((MMP_UBYTE *)(dest_addr+4), (MMP_UBYTE *)(*container_offset), seg1 - 4);
	            
	            MEMCPY((MMP_UBYTE *)buf_start, (MMP_UBYTE *)(gWMVVideoSample.ulOffset + seg1 - 4), lSizeToLoad - seg1 + 4);
           	}
           	else {
	            MEMCPY((MMP_UBYTE *)dest_addr, (MMP_UBYTE *)(*container_offset), seg1);
	            
	            MEMCPY((MMP_UBYTE *)buf_start, (MMP_UBYTE *)(gWMVVideoSample.ulOffset + seg1), lSizeToLoad - seg1);
            }
                
            #if RING_BUF_TAILING > 0
                MMPF_BS_AccessVideoHeader(m_pBsBufInfo->buf + lSizeToLoad - seg1, (MMP_UBYTE*)&ulRingTailing, 4, RING_WRITE);
            #endif
        }
        else 
        {
            if(ulExtraByte) {
            	*((MMP_UBYTE *)(dest_addr+0)) = 0x00;
            	*((MMP_UBYTE *)(dest_addr+1)) = 0x00;
            	*((MMP_UBYTE *)(dest_addr+2)) = 0x01;
            	*((MMP_UBYTE *)(dest_addr+3)) = 0x0D;
            	
            	MMPF_WMVDEC_GetFrame((MMP_UBYTE *)(dest_addr+4), seg1, seg2, lSizeToLoad);
            }
            else {

            	MMPF_WMVDEC_GetFrame((MMP_UBYTE *)(dest_addr), seg1, seg2, lSizeToLoad);
            }
            
            if ( m_bBsSupportLargeFrame ) {
                *container_offset += lSizeToLoad;
                
                #if RING_BUF_TAILING > 0
                 if (!m_3GPPdata.bLargeFrame) {
                    MMPF_BS_AccessVideoHeader((MMP_UBYTE*)(dest_addr + lSizeToLoad), 
                                             (MMP_UBYTE*)&ulRingTailing, 4, RING_WRITE);
                 }
                #endif
            }
        }
    #endif
    return error;
}

/** @brief Load audio data according the the 3GP atoms

Basically it loads one audio frame. But some times if it reaches the limited size, 
it loads partial data and require call twice to load one audio frame.
@param[in] dataptr The address to be loaded
@param[in] Size The size to be loaded
@param[out] LoadedSize The actual loaded size
@return The status of the function call
*/ // using audio frame header uses extra 140 bytes of code
MMP_ERR MMPF_WMVPSR_LoadAudioData(MMP_UBYTE *dataptr, MMP_ULONG Size, MMP_ULONG *LoadedSize)
{
    typedef struct TEMP_AUDIO_HEADER {
        MMP_ULONG ulTime;
        MMP_ULONG ulSize;
        MMP_USHORT ulFlag;
        MMP_USHORT ulFlag2;
    }TEMP_AUDIO_HEADER;
    
    MMP_ULONG           LoadSize;
    MMP_WMV_SAMPLE_INFO tmpAtom;
    MMP_USHORT          usHeaderSizeToWrite = 0;
    MMP_ERR             error = MMP_ERR_NONE;
    
    static MMP_USHORT usHeadherReduntSize = 0;//How many bytes left last time
    static TEMP_AUDIO_HEADER curHeader;
    #ifndef SIZEOF_AUDIO_HEADER
    #define SIZEOF_AUDIO_HEADER (12)
    #endif
    
    if ( (m_ulPreAudioReduntSize == 0) && (m_bAudioEOS) ) {
        *LoadedSize = 0;
        return MMP_ERR_NONE;
    }
    
    LoadSize = 0;

    if (m_ulPreAudioReduntSize == 0) {

        MMPF_VIDPSR_SwapDepackCacheIdx(WMV_AUDIO_CACHE);

        error = MMPF_WMVPSR_GetNextSample(WMV_AUDIO_TYPE, &tmpAtom);

        curHeader.ulTime = tmpAtom.lSampleTime;
        usHeadherReduntSize = SIZEOF_AUDIO_HEADER;
    
        m_ulPreAudioReduntSize = tmpAtom.ulSize + SIZEOF_AUDIO_HEADER;

        curHeader.ulSize = m_ulPreAudioReduntSize;

        curHeader.ulFlag2 ++;
        curHeader.ulFlag2 = (MMP_SHORT)(MMP_ULONG)dataptr;
    
        m_ulPreAudioOffset = tmpAtom.ulOffset;
        //gl3gpAudioSize -= tmpAtom.ulSize;
        
        curHeader.ulFlag = BUFFERFLAG_EOF;
        if (error == MMP_VIDPSR_ERR_EOS) {
            curHeader.ulFlag |= BUFFERFLAG_EOS;
            m_bAudioEOS = MMP_TRUE;
        }
        m_ulBuffedAudioAtoms++;
    }

    if (m_ulPreAudioReduntSize) {

        // write header
        if (usHeadherReduntSize > 0) { // there is header to write
            MMP_USHORT i;
            if (usHeadherReduntSize >= Size) {
                usHeaderSizeToWrite = Size;
            }
            else {
                usHeaderSizeToWrite = usHeadherReduntSize;
            }
            m_ulPreAudioReduntSize -= usHeaderSizeToWrite;
            for (i = SIZEOF_AUDIO_HEADER - usHeadherReduntSize;
                 i < SIZEOF_AUDIO_HEADER - usHeadherReduntSize + usHeaderSizeToWrite; i++) {
                *(dataptr ++) = *(((MMP_UBYTE*)(&curHeader)) + i);
            }
            usHeadherReduntSize -= usHeaderSizeToWrite;
        }
        Size -= usHeaderSizeToWrite;

        if (m_ulPreAudioReduntSize >= Size) {
            LoadSize = Size;
        }
        else {
            LoadSize = m_ulPreAudioReduntSize;
        }
        m_ulPreAudioReduntSize -= LoadSize;

        memcpy(dataptr, (MMP_UBYTE *)m_ulPreAudioOffset, LoadSize);
            
        m_ulPreAudioOffset += LoadSize;

        *LoadedSize = LoadSize + usHeaderSizeToWrite;
    }
    else {
        *LoadedSize = 0;
    }
    return MMP_ERR_NONE;
}

/** @brief Fill the audio buffer without being wrapped

@param[in] ulSize The size to be filled in the audio buffer.
@param[in,out] usTransferSize The size to be transfered. It would be accumulated so that it should be
                              initialized well.
@note This function works with audio pointers.
@return Always returns success.
*/
static MMP_ERR MMPF_WMVPSR_FillAudioBuffer(MMP_ULONG dest_addr,MMP_ULONG ulSize, MMP_USHORT *usTransferSize)
{
    MMP_ULONG   file_return_size2 = 1;
    MMP_ERR     error = MMP_ERR_NONE;
    
    *usTransferSize = 0;
    
    while (ulSize > *usTransferSize && file_return_size2 != 0) {
        
        error = MMPF_WMVPSR_LoadAudioData((MMP_UBYTE *)(dest_addr+ *usTransferSize),
                                  ulSize - *usTransferSize, &file_return_size2);
        if (error == MMP_ERR_NONE)
            *usTransferSize += file_return_size2;
        else
            return error;
    }
    
    return error;
}

/** @brief Demux Audio/Video frame from the bitstream

Triggered by timer2.
Fullfill Audio buffer and Get one Video frame from the bitstream
@pre The caller should avoid playing video which is neither h.263 nor MP4VSP
@return The status of the function call
*/
MMP_ERR MMPF_WMVPSR_Demux(void)
{
    MMP_ERR                 errEOS = MMP_ERR_NONE;// If it's end of strem this time
    MMP_ERR                 error = MMP_ERR_NONE;
    MMP_ULONG               BackupRequestFrame,RequestFrame = 0;
    MMP_ULONG               SizeToLoad;
    MMP_ULONG               header_addr, frame_addr, avail_size,avail_size1,data_size;
    MMP_ULONG				ulOffset;
	MMPF_3GPP_SEEKSTATUS *pSeek = &m_seek[m_ubVideoCurTrack];        
    /// - Demux demux audio data
	if ((gDemuxOption & DEMUX_AUDIO)) {
	    // clear the demux audio option
		gDemuxOption &= ~DEMUX_AUDIO;
		
        // fill audio data
        {
            MMP_USHORT  transfer_size,transfer_size1;
            MMP_ULONG   addr,addr1;
            MMP_LONG    avail_size,avail_size1;
            MMP_ULONG   ulReturnSize;
                
            ulReturnSize = 0;
                
            //if (gAudioSample.ulOffset != 0) 
            {

                if (MMPF_BS_GetAudioStartAddr(&addr,&addr1,&avail_size,&avail_size1) == MMP_ERR_NONE) {
                    if (avail_size1 < 0) { //doesn't need to rewind, plain read
                        error = MMPF_WMVPSR_FillAudioBuffer(addr,avail_size, &transfer_size);
                        if((error != MMP_ERR_NONE) && (error != MMP_VIDPSR_ERR_EOS) ) {
                            return error;
                        }
                            
                        MMPF_BS_UpdateAudioBufptr(transfer_size,MMP_FALSE);
                    }
                    else {// not enough size to put all data, have to rewind
                        error = MMPF_WMVPSR_FillAudioBuffer(addr,avail_size, &transfer_size);
                        if((error != MMP_ERR_NONE) && (error != MMP_VIDPSR_ERR_EOS) ) {
                            return error;
                        }
                        if(transfer_size >= avail_size) {
                            MMPF_BS_UpdateAudioBufptr(transfer_size,MMP_TRUE);

                            error = MMPF_WMVPSR_FillAudioBuffer(addr1,avail_size1, &transfer_size1);
                            if((error != MMP_ERR_NONE) && (error != MMP_VIDPSR_ERR_EOS) ) {
                                return error;
                            }
                                
                            MMPF_BS_UpdateAudioBufptr(transfer_size1,MMP_FALSE);
                            
                            transfer_size += transfer_size1;
                        } else {
                            MMPF_BS_UpdateAudioBufptr(transfer_size,MMP_FALSE);
                        }
                    }
                }
            }
        }
	}

	// ===== fill video data =====
    if ( gDemuxOption & DEMUX_VIDEO ) {
 	    gDemuxOption &= ~DEMUX_VIDEO;
        if (!m_3GPPdata.bFillBufferDone) {

            //if ((!m_3GPPdata.bFilling) && (!m_3GPPdata.bLargeFrame) && (!m_bVideoEOS))
            if ((!m_3GPPdata.bFilling) && (!m_3GPPdata.bLargeFrame))
            {   
                BackupRequestFrame = pSeek->ulRequestFrame;

                errEOS = MMPF_WMVPSR_SeekNextVideoFrame(&RequestFrame);

                if ((errEOS == MMP_VIDPSR_ERR_EOS) && (m_bVideoEOS)) {
                    return MMP_VIDPSR_ERR_EOS;
                }
                
                pSeek->ulRequestFrame = BackupRequestFrame;
                
                // ===== Time stamp =====
                WRITE_HEADER->ulRequestTime = MMPF_WMVPSR_GetTime(WMV_VIDEO_TYPE);
                if ((MMP_ERR_NONE == errEOS) || (MMP_VIDPSR_ERR_EOS == errEOS)) {
                    m_3GPPdata.bFilling = MMP_TRUE;
                }
                else {
                    DBG_S(3, " Seek Error\r\n");
                    return errEOS;
                }
            }
            
            if (m_bWMVAdvance) {
            	ulOffset = 4;
            }
            else {
            	ulOffset = 0;
            }
            
            if ( errEOS == MMP_VIDPSR_ERR_EOS) {
                VAR_L(3, gWMVVideoSample.ulSize);
                m_bVideoEOS = MMP_TRUE;
            }
            
            data_size = gWMVVideoSample.ulSize + m_3GPPdata.usBSOffset + m_3GPPdata.usEOFPadding + ulOffset;

            error = MMPF_BS_GetVideoStartAddr(&header_addr,
                                              &frame_addr,
                                              &avail_size,
                                              &avail_size1,
                                              m_3GPPdata.usBSOffset,
                                              &data_size,
                                              m_ulBsAlignType);
                                              
            if((pSeek->mode == MMPF_M_FF_KEY || pSeek->mode == MMPF_M_SEEK_NONE) &&
               (m_pBsBufInfo->ringData.Atoms > 3)){
                return MMP_ERR_NONE;
            }
            
            if (error == MMP_ERR_NONE) {
                m_3GPPdata.bLargeFrame = MMP_FALSE;
            }
            else if (error == MMP_VIDPSR_ERR_INSUFFICIENT_RESOURCES) {
                if (m_bBsSupportLargeFrame) {
                    m_3GPPdata.bLargeFrame  = MMP_TRUE;
                }
                else {
                    m_3GPPdata.bLargeFrame = MMP_FALSE;
                    m_3GPPdata.bFilling    = MMP_FALSE; //skip this frame
                    return error;
                }
            }
            else {
                return error;
            }
            
            pSeek->ulRequestFrame       = RequestFrame;
            
            // ===== Update the final frame size =====
            if (m_3GPPdata.bLargeFrame) {
                //WRITE_HEADER->ulSize    = m_pBsBufInfo->length;
                WRITE_HEADER->ulSize    = avail_size + avail_size1;
            }
            else {
                WRITE_HEADER->ulSize    = data_size;
            }
            
            avail_size -= m_3GPPdata.usBSOffset;
            
            WRITE_HEADER->ulBSSize      = gWMVVideoSample.ulSize + ulOffset;
        	WRITE_HEADER->ulAddress     = frame_addr;
        	WRITE_HEADER->ulFlag        = 0;
            WRITE_HEADER->ulFrameNum    = MMPF_WMVDEC_WMVGetFrameNum(0);

            if (gWMVVideoSample.ulSize < (avail_size+avail_size1)) {
                SizeToLoad = gWMVVideoSample.ulSize;
                WRITE_HEADER->ulFlag |= BUFFERFLAG_EOF;
                
                if(m_bVideoEOS) {
        	        WRITE_HEADER->ulFlag |= BUFFERFLAG_EOS;
        	        WRITE_HEADER->ulFlag |= BUFFERFLAG_FRAME_EOS;
        	    }
            }
            else {
                SizeToLoad = (avail_size+avail_size1);
            }
            
            gWMVVideoSample.ulSize -= SizeToLoad;
            
            // copy the write header into the bitstream
            MMPF_BS_AccessVideoHeader((MMP_UBYTE*)header_addr, (MMP_UBYTE*)WRITE_HEADER, sizeof(MMPF_M_FRAME_INFO),RING_WRITE);

            // Put single VOP for video decoder
            error = MMPF_WMVPSR_GetSingleFrame(WRITE_HEADER->ulAddress,SizeToLoad,avail_size,avail_size1,&(gWMVVideoSample.ulOffset));

            if (error != MMP_ERR_NONE) {
                m_3GPPdata.bFilling         = MMP_FALSE;
                m_3GPPdata.bFillBufferDone  = MMP_FALSE;
            
                return error;
            }
            
            m_3GPPdata.bFillBufferDone = MMP_TRUE;
            
            MMPF_BS_UpdateVideoBufPtr(RING_WRITE, WRITE_HEADER->ulSize);
            
            m_3GPPdata.bFilling         = MMP_FALSE;
            m_3GPPdata.bFillBufferDone  = MMP_FALSE;
  
            //m_3GPPdata.pPreWriteHeader  = (MMPF_M_FRAME_INFO*)(header_addr);
            m_3GPPdata.pPreWriteHeader.ulRequestTime  = ((MMPF_M_FRAME_INFO*)header_addr)->ulRequestTime;
        } // ! fill buffer done
    }

    return  MMP_ERR_NONE;
}

extern void MMPF_WMVDEC_GetFirstFrame(MMP_ULONG *pulPos, MMP_ULONG *pulSize);
MMP_VID_FILEVALIDATION MMPF_WMVPSR_Parse(void)
{
    MMP_VID_FILEVALIDATION  filevalidation  = MMP_VID_FILE_VALID;
    MMP_ULONG               tmp_total_size ;
    MMP_LONG                ulWorkingSize = 0;
    MMP_ULONG               ulWorkingBuffer = m_ulWorkingBufStart;//0x1580000;
    MMP_ULONG               ulWorkingBuffer2 = 0;
    MMP_ULONG               ulFramePos = 0;
    MMP_ULONG               ulFrameSize = 0;
    MMP_UBYTE               *dest_buf = m_VideoStreamBuf;
 
	///- Reset the parser states
    (void)MMPF_WMVPSR_Init();
    
    MMPF_Player_GetSupportedFormat(&m_3GPPdata.sSupportedAudioFmt,
                                   &m_3GPPdata.sSupportedVideoFmt);
    
	tmp_total_size = gVidFileInfo.file_size;

    m_ulPreAudioOffset      = 0;
	m_ulPreAudioReduntSize  = 0;
	
    MMPF_VIDPSR_ResetCache();
    
    MMPF_VIDPSR_SwapDepackCacheIdx(WMV_VIDEO_CACHE);

    m_ulCurFileOffset = 0;
    m_ulAudioPtr  = 0;
    
    // Set Temp BS Buffer
    /*wmvdec_setvidbsbuf((unsigned char *)ulWorkingBuffer);

	ulWorkingBuffer += WMV_INTERNAL_BSSIZE;
	
	//m_ulWorkingBufSize = ulWorkingSize;
	m_ulTempBSSize = WMV_INTERNAL_BSSIZE;*/
	
	// Set Audio Buffer
	wmvdec_setaudbsbuf((unsigned char *)ulWorkingBuffer);

	ulWorkingBuffer += WMV_INTERNAL_BSSIZE;
	
	m_ulTempBSSize = WMV_INTERNAL_BSSIZE;	
	
	ulWorkingBuffer2 = ulWorkingBuffer;
	// End Set Temp BS Buffer
    
    FileBuffer = (U8_WMC *)ulWorkingBuffer;
	ulWorkingBuffer += WMVFILE_BUFFER_SIZE;
	ulWorkingBuffer =  ((ulWorkingBuffer + 0xfff) >> 12) << 12;
    wmvdec_setworkingbuf((int *)ulWorkingBuffer);
    VAR_L(3, ulWorkingBuffer);
	ulWorkingSize = wmvdec_init(&gVidFileInfo);
	if(ulWorkingSize <= 0){
	    DBG_S(3, "wmvdec_init failed\r\n");
	    return MMP_VID_UNKNOWN_INVALIDATION;
	}
	//wmvdec_init();
	
	MMPF_WMVDEC_GetFileInfo(&gVidFileInfo);
	
	
	//MEMCPY(dest_buf , (MMP_UBYTE *)ulFramePos, ulFrameSize);
	
	#if (CHIP == MCR_V2)
	if(gVidFileInfo.is_video_available) {
	    ulFramePos = (MMP_ULONG)dest_buf;
	    MMPF_WMVDEC_GetFirstFrame(&ulFramePos, &ulFrameSize);
    	if (gVidFileInfo.video_info[0].format == MMP_VID_VIDEO_MP4V_SP) {
    	    gVidFileInfo.video_info[0].width  = 0;
    	    gVidFileInfo.video_info[0].height = 0;
    	    
    	    
    	    #if (VIDEO_P_EN)
    	    {
    	        extern MMP_HANDLE          *hVideo;
                extern MMPF_VIDEO_FUNC     m_vcodec;
                
                hVideo = MMPF_VIDEO_Open(CODEC_MP4V, &m_vcodec);
                
                if (MMP_ERR_NONE != 
                    m_vcodec.MMPF_VIDDEC_Init(hVideo, dest_buf, ulFrameSize,
                                           gVidFileInfo.video_info[0].width,
                                           gVidFileInfo.video_info[0].height,MMP_TRUE)) {
                                          
                    DBG_S(0,"Init video failed\r\n");
                    #if 0
                    bPlayAudioOnly = MMP_TRUE;
                    #else
                    return MMP_VID_UNSUPPORTED_FILE;
                    #endif
                }
                m_vcodec.MMPF_VIDDEC_GetInfo(hVideo,&gVidFileInfo.video_info[0]);
    	    }
    	    #endif
    	    
    	}
	}
	#endif
	if(gVidFileInfo.is_audio_available){
	    if(gVidFileInfo.audio_info.format == MMP_VID_AUDIO_AAC_LC) {
	        MMP_USHORT usOpt = 0;
	        mp4ASC.samplingFrequency = gVidFileInfo.audio_info.sampling_rate;
	        //mp4ASC.objectTypeIndex = 2;
	        MMPF_WMVDEC_GetEncodeOpt(&usOpt);
	        mp4ASC.objectTypeIndex = ((usOpt & 0xF1) >> 3);
	    }
	    #if (VMP3_P_EN)
	    else if(gVidFileInfo.audio_info.format == MMP_VID_AUDIO_MP3) {
	        MMPF_SetMP3Layer(0x03);
	    }
	    #endif
	    #if (VWAV_P_EN)
        else if (gVidFileInfo.audio_info.format == MMP_VID_AUDIO_WAV){
            MMP_USHORT usTag;
            WAVFileInfo *wavinfo;
            
            usTag = MMPF_WMVDEC_GetFormatTag();
            
            if(usTag == 0x0001) { // PCM
                MMPF_SetWAVFormat(MMP_FALSE);
            } else {
                MMP_ULONG ulBitPerSample = 0;
                AIT_AdpcmDecCtx *tContext;
                
                if(usTag == 0x0045) { //G726
        	        ulBitPerSample = (gVidFileInfo.audio_info.bitrate + gVidFileInfo.audio_info.sampling_rate/2)/gVidFileInfo.audio_info.sampling_rate;
        	    } else {
        	        ulBitPerSample = gVidFileInfo.audio_info.bits_per_coded_sample;
        	    }
        	    
        	    if((usTag == 0x0045 || usTag == 0x0011) && (ulBitPerSample != 4)){
	                gVidFileInfo.audio_info.format = MMP_VID_AUDIO_UNSUPPORTED;
	                gVidFileInfo.is_audio_available = 0;
	                goto WMV_END;
	            }
	            
	            if(gVidFileInfo.audio_info.block_align == 0){
        	        gVidFileInfo.audio_info.format = MMP_VID_AUDIO_UNSUPPORTED;
        	        gVidFileInfo.is_audio_available = 0;
        	        goto WMV_END;
        	    }
        	    
                MMPF_SetWAVFormat(MMP_TRUE);
                MMPF_GetWAVContext(&tContext);
        	    tContext->AdpcmDCtxnCh = gVidFileInfo.audio_info.channel_nums;
        	    tContext->AdpcmDCtxBlkAlign = gVidFileInfo.audio_info.block_align;
        	    
        	    tContext->AdpcmDCtxCID = usTag;
        	    tContext->AdpcmDCtxBitPerEncSample = gVidFileInfo.audio_info.bits_per_coded_sample;
        	    tContext->AdpcmDCtxSampleRate = gVidFileInfo.audio_info.sampling_rate;
        	    tContext->AdpcmDCtxBitRate = gVidFileInfo.audio_info.bitrate;
            }
            
            MMPF_GetWAVInfo(&wavinfo);
            wavinfo->bitRate = gVidFileInfo.audio_info.bitrate;
            wavinfo->totaldataSize = 0;
            wavinfo->compressionCode = 0;
            wavinfo->nChannels = gVidFileInfo.audio_info.channel_nums;
            wavinfo->sampleRate = gVidFileInfo.audio_info.sampling_rate;
            wavinfo->newSampleRate = gVidFileInfo.audio_info.sampling_rate;
            wavinfo->bitsPerSample = gVidFileInfo.audio_info.bits_per_coded_sample;
        }
        #endif
	}
	{
	    #if 0
	    extern MMP_HANDLE          *hVideo;
        extern MMPF_VIDEO_FUNC     m_vcodec;
        
        hVideo = MMPF_VIDEO_Open(CODEC_WMVP, &m_vcodec);
        m_vcodec.MMPF_VIDDEC_Init(hVideo,NULL,0,gVidFileInfo.video_info.width,
                                    gVidFileInfo.video_info.height,MMP_TRUE);
        #endif
    }
    
WMV_END:    
    ulWorkingBuffer += ulWorkingSize;
    // align 4K byte
    ulWorkingBuffer = ((ulWorkingBuffer + 0xfff) >> 12) << 12;
    //ulWorkingSize = ((ulWorkingSize + 0xfff) >> 12) << 12; 
    ulWorkingSize = ulWorkingBuffer - ulWorkingBuffer2; 
    
    m_ulWorkingBufSize = ulWorkingSize;
        	
	#if (CHIP == P_V2)
	MMPF_MFD_SetCurrMemAddr(ulWorkingBuffer); 
	if(gVidFileInfo.is_video_available) {
	    ulFramePos = (MMP_ULONG)dest_buf;
	    MMPF_WMVDEC_GetFirstFrame(&ulFramePos, &ulFrameSize);
	
	    DBG_S(3,"Init WMV\r\n");
	    VAR_L(3,ulWorkingBuffer);
	    
    	if(gVidFileInfo.video_info.format == MMP_VID_VIDEO_MP4V_SP) {
    	
    	    #if (VIDEO_P_EN)
        	error = MMPF_VideoDecoder_InitCodec(MMP_VID_VIDEO_MP4V, (MMP_ULONG)dest_buf, ulFrameSize, &bNeedFirstFrame, gVidFileInfo.video_info.width, gVidFileInfo.video_info.height);
        	if(error != MMP_ERR_NONE) {
                DBG_S(3,"Init video/mp4 failed\r\n");
            
                gVidFileInfo.is_video_available = 0;
                gVidFileInfo.video_info.format = MMP_VID_VIDEO_UNSUPPORTED;
                return MMP_VID_UNSUPPORTED_FILE;
            }
            #endif
        } else if(gVidFileInfo.video_info.format == MMP_VID_VIDEO_WMV){
        
            #if (VIDEO_P_EN)
            error = MMPF_VideoDecoder_InitCodec(MMP_VID_VIDEO_WMV, (MMP_ULONG)m_InitBuf, sizeof(m_InitBuf), &bNeedFirstFrame, gVidFileInfo.video_info.width, gVidFileInfo.video_info.height);
            if(error != MMP_ERR_NONE) {
                DBG_S(3,"Init video/wmv failed\r\n");
            
                gVidFileInfo.is_video_available = 0;
                gVidFileInfo.video_info.format = MMP_VID_VIDEO_UNSUPPORTED;
                return MMP_VID_UNSUPPORTED_FILE;
            }
            #endif
        } else if(gVidFileInfo.video_info.format == MMP_VID_VIDEO_H264){
        
            #if (VIDEO_P_EN)
        	error = MMPF_VideoDecoder_InitCodec(MMP_VID_VIDEO_H264, (MMP_ULONG)dest_buf, ulFrameSize, &bNeedFirstFrame, gVidFileInfo.video_info.width, gVidFileInfo.video_info.height);
        	if(error != MMP_ERR_NONE) {
                DBG_S(3,"Init video/avc failed\r\n");
            
                gVidFileInfo.is_video_available = 0;
                gVidFileInfo.video_info.format = MMP_VID_VIDEO_UNSUPPORTED;
                return MMP_VID_UNSUPPORTED_FILE;
            }
            #endif
        }
    }
    if ( gVidFileInfo.is_video_available ) {
        error = MMP_ERR_NONE;
        if(bNeedFirstFrame){
            
            //TBD, Need to do decode first frame?
        }
        #if (VIDEO_P_EN)
        {
            //TODO
            MMPF_MFD_GetInfo(&gVidFileInfo.video_info);
        }
        #endif
    }
	#endif // end of (CHIP == P_V2)
	
	//MMPF_MMU_ConfigWorkingBuffer(m_ulWorkingBufStart + 16*1024 + 300*1024, m_ulWorkingBufStart + m_ulWorkingBufSize, MMP_TRUE);
	//MMPF_MMU_ConfigWorkingBuffer(0x16F0000,  0x1700000/*m_ulWorkingBufSize*/, MMP_TRUE);
	//error = MMPF_FS_FOpen(dumpfile, "wb", &dumpfileId);
	//if(error != MMP_ERR_NONE)
	//    DBG_S3("Open File Error\r\n");
    
    // Video Test
    /*{
        MMP_ULONG loop = 0;
        for(loop = 0; loop < 0x2000; loop++){
            
            //DBG_S(3, "DECODE START\r\n");
            //error = MMPF_FS_FOpen(dumpfile, "wb", &dumpfileId);
	        //if(error != MMP_ERR_NONE)
	        //    DBG_S3("Open File Error\r\n");
	            
            //MEMSET((MMP_UBYTE *)0x13D8000, 0, 0x1c200);
            //DBG_S(3, "STEP 1\r\n");
            MMPF_WMVPSR_GetNextSample(WMV_VIDEO_TYPE, &gWMVVideoSample);
            //MMPF_FS_FWrite(dumpfileId, (MMP_UBYTE *)gWMVVideoSample.ulOffset, gWMVVideoSample.ulSize, &WriteCount);
            //MMPF_FS_FClose(dumpfileId);
            DBG_S(3, "STEP 2\r\n");
            MMPF_WMVDEC_DecodeWMVFrame(gWMVVideoSample.ulOffset, gWMVVideoSample.ulSize, 0, WMV_VIDEO_TYPE);
            DBG_S(3, "STEP 3\r\n");
            MMPF_WMVDEC_GetFrameAddr((MMP_UBYTE *)0x13D8000, 0);
            //DBG_S(3, "STEP 4\r\n");
            //MMPF_FS_FWrite(dumpfileId, (MMP_UBYTE *)0x13D8000, 0x1c200, &WriteCount);
            VAR_L(3, loop);
        }
    }*/
    // Audio Test
    /*{
        MMP_ULONG loop = 0;
        MMP_LONG SampleNum;
        MMPF_WMVDEC_InitAudioBSBuf(0x1500000 + WMV_INTERNAL_BSSIZE, WMV_INTERNAL_BSSIZE);
        error = MMPF_FS_FOpen(dumpfile, "wb", &dumpfileId);
	    if(error != MMP_ERR_NONE)
	            DBG_S3("Open File Error\r\n");
        for(loop = 0; loop < 1600; loop++){
            
            //DBG_S(3, "DECODE START\r\n");
            
	            
            //MEMSET((MMP_UBYTE *)0x13D8000, 0, 0x1c200);
            //DBG_S(3, "STEP 1\r\n");
            //MMPF_WMVPSR_GetNextSample(WMV_VIDEO_TYPE, &gWMVVideoSample);
            //MMPF_FS_FWrite(dumpfileId, (MMP_UBYTE *)gWMVVideoSample.ulOffset, gWMVVideoSample.ulSize, &WriteCount);
            //MMPF_FS_FClose(dumpfileId);
            DBG_S(3, "STEP 1\r\n");
            MMPF_WMVDEC_DecodeWMAFrame(OutputSample, &SampleNum);
            DBG_S(3, "STEP 2\r\n");
            //MMPF_WMVDEC_GetFrameAddr((MMP_UBYTE *)0x13D8000, 0);
            //DBG_S(3, "STEP 4\r\n");
            MMPF_FS_FWrite(dumpfileId, (MMP_UBYTE *)OutputSample, SampleNum*4, &WriteCount);
            VAR_L(3, loop);
        }
    }

    MMPF_FS_FClose(dumpfileId);*/
    return filevalidation;
}
#else
MMP_ULONG WMCDecCBGetBSData(MMP_UBYTE *pBuf, MMP_ULONG nOffset, MMP_ULONG NumRead)
{
    return 0;
}
MMP_ULONG WMCDecCBGetData ( void * hWMCDec, MMP_ULONG64 nOffset,  MMP_ULONG nNumBytesRequest, MMP_UBYTE **ppData, MMP_ULONG u32UserContext )
{
    return 0;
}
#endif // WMV_P_EN
#endif
// -- Will_Chen

#ifndef BUILD_FW

#if 0 // for IDE function name list by section
void _____WMVHostParser_____(){}
#endif

typedef enum  _MMPD_VIDPSR_ASFERROR {
    VIDASF_ERROR_NONE           = 0,
    VIDASF_ERROR_INTERNAL       = -1,  /* incorrect input to API calls */
    VIDASF_ERROR_OUTOFMEM       = -2,  /* some malloc inside program failed */
    VIDASF_ERROR_EOF            = -3,  /* unexpected end of file */
    VIDASF_ERROR_IO             = -4,  /* error reading or writing to file */
    VIDASF_ERROR_INVALID_LENGTH = -5,  /* length value conflict in input data */
    VIDASF_ERROR_INVALID_VALUE  = -6,  /* other value conflict in input data */
    VIDASF_ERROR_INVALID_OBJECT = -7,  /* ASF object missing or in wrong place */
    VIDASF_ERROR_OBJECT_SIZE    = -8,  /* invalid ASF object size (too small) */
    VIDASF_ERROR_SEEKABLE       = -9,  /* file not seekable */
    VIDASF_ERROR_SEEK           = -10, /* file is seekable but seeking failed */
    VIDASF_ERROR_ENCRYPTED      = -11  /* file is encrypted */
} MMPD_VIDPSR_ASFERROR;

typedef struct  _MMPD_VIDPSR_ASFGUID {
    MMP_ULONG   ulv1;
    MMP_USHORT  usv2;
    MMP_USHORT  usv3;
    MMP_UBYTE   ubv4[8];
} MMPD_VIDPSR_ASFGUID;

typedef struct _MMPD_VIDPSR_ASFOBJECT {
    MMPD_VIDPSR_ASFGUID     guid;
    MMP_ULONG64             ullSize;
    MMP_ULONG64             ullDataLen;
} MMPD_VIDPSR_ASFOBJECT;

typedef enum _MMPD_WMVPSR_PARSESTATE {
    ASF_NEW_PACKET,
    ASF_PAYLOAD_START,
    ASF_STORE_PAYLOAD
} MMPD_WMVPSR_PARSESTATE;

//static MMP_ULONG m_ulWMAFilePos;
//static MMP_UBYTE m_ubUtf8Buf[512];
//static MMP_UBYTE m_ubUtf16Buf[256];
static MMP_ULONG m_ulVideoFileSize;
MMP_UBYTE WMVTmpBuf[256];
MMP_ULONG WMVTmpBufLen;


static const MMPD_VIDPSR_ASFGUID asf_guid_header =
{0x75B22630, 0x668E, 0x11CF, {0xA6, 0xD9, 0x00, 0xAA, 0x00, 0x62, 0xCE, 0x6C}};

static const MMPD_VIDPSR_ASFGUID asf_guid_data =
{0x75B22636, 0x668E, 0x11CF, {0xA6, 0xD9, 0x00, 0xAA, 0x00, 0x62, 0xCE, 0x6C}};

static const MMPD_VIDPSR_ASFGUID asf_guid_index =
{0x33000890, 0xE5B1, 0x11CF, {0x89, 0xF4, 0x00, 0xA0, 0xC9, 0x03, 0x49, 0xCB}};

/* header level object guids */

static const MMPD_VIDPSR_ASFGUID asf_guid_file_properties =
{0x8cabdca1, 0xa947, 0x11cf, {0x8E, 0xe4, 0x00, 0xC0, 0x0C, 0x20, 0x53, 0x65}};

static const MMPD_VIDPSR_ASFGUID asf_guid_stream_properties =
{0xB7DC0791, 0xA9B7, 0x11CF, {0x8E, 0xE6, 0x00, 0xC0, 0x0C, 0x20, 0x53, 0x65}};

static const MMPD_VIDPSR_ASFGUID asf_guid_content_description =
{0x75B22633, 0x668E, 0x11CF, {0xA6, 0xD9, 0x00, 0xAA, 0x00, 0x62, 0xCE, 0x6C}};

static const MMPD_VIDPSR_ASFGUID asf_guid_extended_content_description =
{0xD2D0A440, 0xE307, 0x11D2, {0x97, 0xF0, 0x00, 0xA0, 0xC9, 0x5E, 0xA8, 0x50}};

static const MMPD_VIDPSR_ASFGUID asf_guid_content_encryption =
{0x2211b3fb, 0xbd23, 0x11d2, {0xb4, 0xb7, 0x00, 0xa0, 0xc9, 0x55, 0xfc, 0x6e}};

static const MMPD_VIDPSR_ASFGUID asf_guid_extended_content_encryption =
{0x298ae614, 0x2622, 0x4c17, {0xb9, 0x35, 0xda, 0xe0, 0x7e, 0xe9, 0x28, 0x9c}};

/* stream type guids */

static const MMPD_VIDPSR_ASFGUID asf_guid_stream_type_audio =
{0xF8699E40, 0x5B4D, 0x11CF, {0xA8, 0xFD, 0x00, 0x80, 0x5F, 0x5C, 0x44, 0x2B}};

//sam: add video guid
static const MMPD_VIDPSR_ASFGUID asf_guid_stream_type_video =
{0xBC19EFC0, 0x5B4D, 0x11CF, {0xA8, 0xFD, 0x00, 0x80, 0x5F, 0x5C, 0x44, 0x2B}};

static const MMPD_VIDPSR_ASFGUID asf_guid_header_extension =
{0x5FBF03B5, 0xA92E, 0x11CF, {0x8E, 0xE3, 0x00, 0xC0, 0x0C, 0x20, 0x53, 0x65}};

static const MMPD_VIDPSR_ASFGUID asf_guid_stream_type_Ex1 =
{0x14E6A5CB, 0xC672, 0x4332, {0x83, 0x99, 0xA9, 0x69, 0x52, 0x06, 0x5B, 0x5A}};


// define
#define WMV_GIUD_SZ      (16)    //byte
#define WMV_OBJHDR_SZ    (24)    //byte
#define WMV_VIDEO_INFO_SIZE (40)
#define WMV_OBJECT_SIZE   (50)
//#define WMV

// tmp define
#define MMP_VID_VIDEO_WMVP 0x20
#define USE_HOST_GET_FRAME (0)


// end define
static MMP_LONG MMPD_WMVPSR_SkipFile(MMP_ULONG offset)
{
	m_ulCurFileOffset += offset;
	if(m_ulCurFileOffset < m_ulVideoFileSize)
	    return 0;
    return -1;
}

static MMP_LONG MMPD_WMVPSR_ReadWMVFile(MMP_UBYTE* ubBuf, MMP_ULONG ulSize)
{
    MMP_ERR error;
    
    //error = MMPD_3GPPSR_ReadFile(ubBuf, m_ulCurFileOffset, ulSize); //TBD
    if(error == MMP_ERR_NONE){
        m_ulCurFileOffset += ulSize;
        return ulSize;
    }
    return -1;
}

static MMP_LONG MMPD_WMVPSR_IsGuidMatch(const MMPD_VIDPSR_ASFGUID *guid1, const MMPD_VIDPSR_ASFGUID *guid2)
{
    MMP_LONG i;
    if((guid1->ulv1 != guid2->ulv1) || (guid1->usv2 != guid2->usv2) || (guid1->usv3 != guid2->usv3))
    {
        return 0;
    }
    for(i = 0; i < 8; i++)
    {
        if(guid1->ubv4[i]!= guid2->ubv4[i])
            return 0;
    }
    return 1;
}

static MMP_LONG MMPD_WMVPSR_ReadWMVGuid(MMPD_VIDPSR_ASFGUID* guid)
{
    MMP_ULONG lCnt;
    lCnt = MMPD_WMVPSR_ReadWMVFile((MMP_UBYTE *)&guid->ulv1, 4);
    lCnt += MMPD_WMVPSR_ReadWMVFile((MMP_UBYTE *)&guid->usv2, 2);
    lCnt += MMPD_WMVPSR_ReadWMVFile((MMP_UBYTE *)&guid->usv3, 2);
    lCnt += MMPD_WMVPSR_ReadWMVFile((MMP_UBYTE *)(guid->ubv4), 8);
    if(lCnt == WMV_GIUD_SZ)
        return WMV_GIUD_SZ;
    else
        return -1;
}

static MMP_LONG MMPD_WMVPSR_ReadObjHeader(MMPD_VIDPSR_ASFOBJECT *obj)
{
    MMP_LONG lCnt;
    lCnt = MMPD_WMVPSR_ReadWMVGuid(&obj->guid);
    lCnt += MMPD_WMVPSR_ReadWMVFile((MMP_UBYTE *)(&obj->ullSize), 8);
    obj->ullDataLen = 0;
    
    if(lCnt == WMV_OBJHDR_SZ)
        return WMV_OBJHDR_SZ;
    else
        return -1;
}
typedef struct _ASF_PACKET_INFO {
    MMP_BOOL  bParityPacket;
    MMP_ULONG ulParseOffset;

    MMP_BOOL  bEccPresent;
    MMP_UBYTE ubECLen;

    MMP_UBYTE ubPacketLenType;
    MMP_ULONG ulPacketLenTypeOffset;

    MMP_UBYTE ubPadLenType;
    MMP_UBYTE ubSequenceLenType;

    MMP_BOOL  bMultiPayloads;

    MMP_UBYTE ubOffsetBytes;

    MMP_UBYTE ubOffsetLenType;

    MMP_ULONG ulPacketLenOffset;
    
    MMP_ULONG ulPayLenTypeOffset;

    MMP_ULONG ulExplicitPacketLength; 
    
    MMP_ULONG ulSequenceNum; 

    MMP_ULONG ulPadding;

    MMP_ULONG  ulSCR;
    MMP_USHORT usDuration;

    MMP_UBYTE ubPayLenType;
    MMP_UBYTE ubPayBytes;
    MMP_ULONG ulbPayLenTypeOffset;

    MMP_ULONG ulPayloads;
    
} ASF_PACKET_INFO;

typedef struct _ASF_PAYLOAD_INFO
{
    MMP_USHORT usPacketOffset;
    MMP_USHORT usTotalSize;

    MMP_UBYTE  ubStreamId;
    MMP_UBYTE  ubObjectId;

    MMP_ULONG  ulObjectOffset;
    MMP_ULONG  ulObjectSize;
    MMP_ULONG  ulObjectPres;

    MMP_UBYTE  ubRepData;
    MMP_UBYTE  ubJunk;
    MMP_USHORT usPayloadSize;
    MMP_UBYTE  ubIsKeyFrame;
    MMP_UBYTE  ubFrameRate;
    MMP_USHORT usTotalDataBytes;
    MMP_ULONG  ulDeltaPresTime;
    MMP_UBYTE  ubIsCompressedPayload;

    MMP_USHORT usBytesRead;
    MMP_UBYTE  ubSubPayloadState;
    MMP_UBYTE  ubNextSubPayloadSize;
    MMP_USHORT usSubpayloadLeft;
    MMP_USHORT usSubCount; 

} ASF_PAYLOAD_INFO;

#if (USE_HOST_GET_FRAME == 1)
static MMPD_VIDPSR_ASFERROR MMPD_WMVPSR_ParsePacketHeader(MMP_ULONG ulPacketOffset, ASF_PACKET_INFO *pPacket)
{
    MMP_UBYTE tmp, buf[10];
    
    m_ulCurFileOffset = ulPacketOffset;
    MMPD_WMVPSR_ReadWMVFile(&tmp, 1);
    pPacket->bParityPacket = 0;
    pPacket->ulParseOffset = 0;
    
    pPacket->bEccPresent = ((tmp&0x80) == 0x80);
    pPacket->ubECLen = 0;
    
    if(pPacket->bEccPresent){
    
        if(tmp&0x10){
            pPacket->bParityPacket = MMP_TRUE;
            return VIDASF_ERROR_NONE;
        }
        
        if(tmp&0x60){
            return VIDASF_ERROR_INVALID_VALUE;    
        }
        
        pPacket->ubECLen = (tmp&0x0f);
        if(pPacket->ubECLen != 2){
            return VIDASF_ERROR_INVALID_VALUE;
        }
        
        pPacket->ulParseOffset = 1+pPacket->ubECLen;
        
        m_ulCurFileOffset = ulPacketOffset+pPacket->ulParseOffset;
        MMPD_WMVPSR_ReadWMVFile(&tmp, 1);
    }
    
    pPacket->ulPacketLenTypeOffset = pPacket->ulParseOffset;
    
    pPacket->ubPacketLenType = (tmp&0x60)>>5;
    pPacket->ubPadLenType = (tmp&0x18)>>3;
    pPacket->ubSequenceLenType = (tmp&0x06)>>1;
    pPacket->bMultiPayloads = (MMP_BOOL)(tmp&0x01);
    
    pPacket->ulParseOffset++;
    MMPD_WMVPSR_ReadWMVFile(&tmp, 1);
    
    pPacket->ubOffsetBytes = 4;
    pPacket->ubOffsetLenType = 3;
    
    if(tmp != 0x5d){
    
        if((tmp&0xc0) != 0x40)
        {
            return VIDASF_ERROR_INVALID_VALUE;
        }

        if((tmp&0x30) != 0x10)
        {
            return VIDASF_ERROR_INVALID_VALUE;
        }

        pPacket->ubOffsetLenType = (tmp&0x0c)>>2;
        if(pPacket->ubOffsetLenType == 0)
        {
            return VIDASF_ERROR_INVALID_VALUE;
        }
        else if(pPacket->ubOffsetLenType < 3)
        {
            pPacket->ubOffsetBytes = pPacket->ubOffsetLenType;
        }

        if((tmp&0x03) != 0x01)
        {
            return VIDASF_ERROR_INVALID_VALUE;
        }
    }
    pPacket->ulParseOffset++;
    pPacket->ulPacketLenOffset = pPacket->ulParseOffset;
    
    // Get ExplicitPacketLength
    switch(pPacket->ubPacketLenType)
    {
    case 0x01:
        MMPD_WMVPSR_ReadWMVFile(&tmp, 1);
        pPacket->ulParseOffset++;
        pPacket->ulExplicitPacketLength = tmp;
        break;
    case 0x02:
        {
            MMP_UBYTE data[2];
            MMPD_WMVPSR_ReadWMVFile(data, 2);
            pPacket->ulParseOffset += 2;
            pPacket->ulExplicitPacketLength = ((MMP_ULONG)data[1] << 8) | (MMP_ULONG)data[0];
        }
        break;
    case 0x03:
        {
            MMP_UBYTE data[4];
            MMPD_WMVPSR_ReadWMVFile(data, 4);
            pPacket->ulParseOffset += 4;
            pPacket->ulExplicitPacketLength = ((MMP_ULONG)data[3] << 24) | ((MMP_ULONG)data[2] << 16) | ((MMP_ULONG)data[1] << 8) | (MMP_ULONG)data[0];
        }
        break;
    default:
        pPacket->ulExplicitPacketLength = 0;
        break;
    }
    
    // Get SequenceNum
    switch(pPacket->ubSequenceLenType)
    {
    case 0x01:
        MMPD_WMVPSR_ReadWMVFile(&tmp, 1);
        pPacket->ulParseOffset++;
        pPacket->ulSequenceNum = tmp;
        break;
    case 0x02:
        {
            MMP_UBYTE data[2];
            MMPD_WMVPSR_ReadWMVFile(data, 2);
            pPacket->ulParseOffset += 2;
            pPacket->ulSequenceNum = ((MMP_ULONG)data[1] << 8) | (MMP_ULONG)data[0];
        }
        break;
    case 0x03:
        {
            MMP_UBYTE data[4];
            MMPD_WMVPSR_ReadWMVFile(data, 4);
            pPacket->ulParseOffset += 4;
            pPacket->ulSequenceNum = ((MMP_ULONG)data[3] << 24) | ((MMP_ULONG)data[2] << 16) | ((MMP_ULONG)data[1] << 8) | (MMP_ULONG)data[0];
        }
        break;
    default:
        pPacket->ulSequenceNum = 0;
        break;
    }
    
    // Get Padding
    switch(pPacket->ubPadLenType)
    {
    case 0x01:
        MMPD_WMVPSR_ReadWMVFile(&tmp, 1);
        pPacket->ulParseOffset++;
        pPacket->ulPadding = tmp;
        break;
    case 0x02:
        {
            MMP_UBYTE data[2];
            MMPD_WMVPSR_ReadWMVFile(data, 2);
            pPacket->ulParseOffset += 2;
            pPacket->ulPadding = ((MMP_ULONG)data[1] << 8) | (MMP_ULONG)data[0];
        }
        break;
    case 0x03:
        {
            MMP_UBYTE data[4];
            MMPD_WMVPSR_ReadWMVFile(data, 4);
            pPacket->ulParseOffset += 4;
            pPacket->ulPadding = ((MMP_ULONG)data[3] << 24) | ((MMP_ULONG)data[2] << 16) | ((MMP_ULONG)data[1] << 8) | (MMP_ULONG)data[0];
        }
        break;
    default:
        pPacket->ulPadding = 0;
        break;
    }
    
    MMPD_WMVPSR_ReadWMVFile(buf, 6);
    pPacket->ulSCR = ((MMP_ULONG)buf[3] << 24) | ((MMP_ULONG)buf[2] << 16) | ((MMP_ULONG)buf[1] << 8) | (MMP_ULONG)buf[0];
    pPacket->usDuration = ((MMP_ULONG)buf[5] << 8) | (MMP_ULONG)buf[4];
    pPacket->ulParseOffset += 6;
    
    if( pPacket->bEccPresent && pPacket->bParityPacket ){
        return VIDASF_ERROR_NONE;
    }
    pPacket->ulPayLenTypeOffset = 0;
    pPacket->ubPayLenType = 0;
    pPacket->ubPayBytes = 0;
    pPacket->ulPayloads = 1;
    
    if(pPacket->bMultiPayloads)
    {

        MMPD_WMVPSR_ReadWMVFile(&tmp, 1);

        pPacket->ulPayLenTypeOffset = pPacket->ulParseOffset;

        pPacket->ubPayLenType = (tmp&0xc0)>>6;
        if(pPacket->ubPayLenType != 2
           && pPacket->ubPayLenType != 1)
        {
            return VIDASF_ERROR_INVALID_VALUE;
        }

        pPacket->ubPayBytes = pPacket->ubPayLenType;

        pPacket->ulPayloads = (MMP_ULONG)(tmp&0x3f);
        if(pPacket->ulPayloads == 0)
        {
            return VIDASF_ERROR_INVALID_VALUE;
        }
        pPacket->ulParseOffset++;
    }
    
    return VIDASF_ERROR_NONE;
}



static MMPD_VIDPSR_ASFERROR MMPD_WMVPSR_ParsePayloadHeader(MMP_ULONG ulPacketOffset, ASF_PACKET_INFO *pPacket, ASF_PAYLOAD_INFO *pPayload, MMP_USHORT usExternSize, MMP_ULONG ulPacketSize)
{
    MMP_UBYTE tmp, pData[10];
    MMP_ULONG ulDummy;
    MMP_USHORT usExtnDataSize = usExternSize, usTotalDataSize;
    MMP_ULONG ulPayloadSize = 0;
    m_ulCurFileOffset = ulPacketOffset + pPacket->ulParseOffset;
    
    MMPD_WMVPSR_ReadWMVFile(pData, 2);
    pPayload->usPacketOffset = (MMP_USHORT)pPacket->ulParseOffset;
    pPayload->ubStreamId = (pData[0])&0x7f; // Amit to get correct Streamid 
    pPayload->ubIsKeyFrame = (((pData[0]) &0x80) >> 7);
    pPayload->ubObjectId = pData[1];
    ulDummy = 0;
    
    switch(pPacket->ubOffsetLenType)
    {
    case 0x01:
        MMPD_WMVPSR_ReadWMVFile(&tmp, 1);
        pPayload->ulObjectOffset = tmp;
        break;
    case 0x02:
        {
            MMP_UBYTE data[2];
            MMPD_WMVPSR_ReadWMVFile(data, 2);
            pPayload->ulObjectOffset = ((MMP_ULONG)data[1] << 8) | (MMP_ULONG)data[0];
        }
        break;
    case 0x03:
        {
            MMP_UBYTE data[4];
            MMPD_WMVPSR_ReadWMVFile(data, 4);
            pPayload->ulObjectOffset = ((MMP_ULONG)data[3] << 24) | ((MMP_ULONG)data[2] << 16) | ((MMP_ULONG)data[1] << 8) | (MMP_ULONG)data[0];
        }
        break;
    default:
        break;
    }
    m_ulCurFileOffset = ulPacketOffset + pPacket->ulParseOffset + 2 + pPacket->ubOffsetBytes;
    PRINTF("Parse Payload 1 : %X\r\n", m_ulCurFileOffset);
    
    MMPD_WMVPSR_ReadWMVFile(&tmp, 1);
    pPayload->ubRepData = tmp;
    pPayload->ubRepData -= usExtnDataSize;

    pPayload->ulObjectPres = 0xffffffff;

    if(pPayload->ubRepData == 1){
        pPayload->ulObjectPres = pPayload->ulObjectOffset;
        pPayload->ulObjectOffset = 0;
        pPayload->ulObjectSize = 0;
        pPayload->ubIsCompressedPayload = 1;
        
        MMPD_WMVPSR_ReadWMVFile(pData, 3);
        pPayload->ulDeltaPresTime = pData[0];
        
        if(pPacket->bMultiPayloads){
            usTotalDataSize = ((MMP_ULONG)pData[2] << 8) | pData[1];
        } else {
            usTotalDataSize = 0;
        }
        
    } else if(pPayload->ubRepData >= 8){
        MMPD_WMVPSR_ReadWMVFile(pData, 8);
        
        pPayload->ulObjectSize = ((MMP_ULONG)pData[3] << 24) | ((MMP_ULONG)pData[2] << 16) | ((MMP_ULONG)pData[1] << 8) | pData[0];
        pPayload->ulObjectPres = ((MMP_ULONG)pData[7] << 24) | ((MMP_ULONG)pData[6] << 16) | ((MMP_ULONG)pData[5] << 8) | pData[4];
        pPayload->ubIsCompressedPayload = 0;
    }
    
    pPayload->usTotalSize = 1 + 1 + pPacket->ubOffsetBytes + 1 + pPayload->ubRepData + usExtnDataSize;
    
    m_ulCurFileOffset = ulPacketOffset + pPacket->ulParseOffset + pPayload->usTotalSize;
    if(pPacket->bMultiPayloads){
        ulDummy = 0;
        
        switch(pPacket->ubPayLenType)
        {
        case 0x01:
            MMPD_WMVPSR_ReadWMVFile(&tmp, 1);
            ulPayloadSize = tmp;
            break;
        case 0x02:
            {
                MMP_UBYTE data[2];
                MMPD_WMVPSR_ReadWMVFile(data, 2);
                ulPayloadSize = ((MMP_ULONG)data[1] << 8) | (MMP_ULONG)data[0];
            }
            break;
        case 0x03:
            {
                MMP_UBYTE data[4];
                MMPD_WMVPSR_ReadWMVFile(data, 4);
                ulPayloadSize= ((MMP_ULONG)data[3] << 24) | ((MMP_ULONG)data[2] << 16) | ((MMP_ULONG)data[1] << 8) | (MMP_ULONG)data[0];
            }
            break;
        default:
            ulPayloadSize = 0;
            break;
        }
    } else if(pPacket->ulExplicitPacketLength > 0){
        ulPayloadSize = pPacket->ulExplicitPacketLength
                      - pPacket->ulParseOffset
                      - pPayload->usTotalSize
                      - pPacket->ulPadding;
    } else {
        ulPayloadSize = ulPacketSize
                      - pPacket->ulParseOffset
                      - pPayload->usTotalSize
                      - pPacket->ulPadding;
    }
    if(usTotalDataSize == 0)
        usTotalDataSize = ulPayloadSize;
    
    pPayload->usPayloadSize = ulPayloadSize;

    pPayload->usTotalSize += pPacket->ubPayBytes
                          + pPayload->usPayloadSize;

    pPayload->usTotalDataBytes = usTotalDataSize; // Amit

    pPacket->ulParseOffset += pPayload->usTotalSize;

    if(pPacket->ulParseOffset > ulPacketSize)
    {
        return VIDASF_ERROR_INVALID_VALUE;
    }
    
    return VIDASF_ERROR_NONE;
}

MMP_ULONG HostAddr = 0;
static MMP_ERR MMPD_WMVPSR_DemuxOneFrame(MMP_ULONG ulFileOffset, MMP_ULONG ulPacketSize, MMP_ULONG ulStreamID, MMP_ULONG ulExternSize)
{
    MMPD_VIDPSR_ASFERROR error = VIDASF_ERROR_NONE;
    MMPD_WMVPSR_PARSESTATE state = ASF_NEW_PACKET;
    ASF_PACKET_INFO Packet;
    ASF_PAYLOAD_INFO Payload;
    MMP_ULONG ulNextPacket, ulCurrPacket;
    MMP_ULONG ulPayloads = 0, ulPayloadStart = 0;
    MMP_ULONG ulFrameSize = 0, ulUsed = 0, ulFrameLeft = 0, FrameBuf = 0;
    MMP_BOOL  bFirst = MMP_FALSE;
    m_ulCurFileOffset = ulFileOffset;
    ulCurrPacket = ulFileOffset;
    ulNextPacket = ulFileOffset;
    HostAddr = 0x30A0000;
    FrameBuf = HostAddr;
    MEMSET0(&Packet);
    MEMSET0(&Payload);
    do {
        switch(state){
            case ASF_NEW_PACKET:
                ulCurrPacket = ulNextPacket;
                ulNextPacket += ulPacketSize;
                error = MMPD_WMVPSR_ParsePacketHeader(ulCurrPacket, &Packet);
                if(error != VIDASF_ERROR_NONE){
                    PRINTF("Parse Packet Header Error\r\n");
                    return MMP_VIDPSR_ERR_INSUFFICIENT_RESOURCES;
                }
                if( Packet.bEccPresent && Packet.bParityPacket ){
                    break;
                }
                state = ASF_PAYLOAD_START;
                ulPayloads = 0;
                break;
            case ASF_PAYLOAD_START:
                if(ulPayloads >= Packet.ulPayloads){
                    state = ASF_NEW_PACKET;
                    break;
                }
                error = MMPD_WMVPSR_ParsePayloadHeader(ulCurrPacket, &Packet, &Payload, ulExternSize, ulPacketSize);
                if(error != VIDASF_ERROR_NONE){
                    PRINTF("Parse Payload Header Error\r\n");
                    return MMP_VIDPSR_ERR_INSUFFICIENT_RESOURCES;
                }
                if(ulStreamID != Payload.ubStreamId){
                    ulPayloads++;
                    state = ASF_PAYLOAD_START;
                    break;
                }
                ulPayloadStart = ulCurrPacket + Payload.usPacketOffset + Payload.usTotalSize - Payload.usPayloadSize;
                state = ASF_STORE_PAYLOAD;
                break;
            case ASF_STORE_PAYLOAD:
                  
                if(Payload.ulObjectOffset == 0){
                
                    if(!bFirst){
                        if(Payload.ubIsKeyFrame == 0){
                            ulPayloads++;
                            state = ASF_PAYLOAD_START;
                            break;
                        }
                        bFirst = MMP_TRUE;
                    }
                    
                    ulUsed = Payload.usPayloadSize;
                    ulFrameLeft = Payload.ulObjectSize;
                    ulFrameSize = Payload.ulObjectSize;
                } else {
                    if(ulUsed == 0){
                        ulPayloads++;
                        state = ASF_PAYLOAD_START;
                        break;
                    }
                    
                    if(ulUsed == Payload.ulObjectOffset){
                        ulUsed += Payload.usPayloadSize;
                    } else {
                        PRINTF("Broken frame\r\n");
                        return MMP_VIDPSR_ERR_INSUFFICIENT_RESOURCES;
                    }
                }
                    
                if(ulFrameLeft > 0){
                    MMPD_3GPPSR_ReadFile((MMP_UBYTE *)FrameBuf, ulPayloadStart, Payload.usPayloadSize);
                    FrameBuf += Payload.usPayloadSize;
                    ulFrameLeft -= Payload.usPayloadSize;
                }
                
                if((MMP_LONG)ulFrameLeft <= 0){
                    #if 0
                    if(1){ // Test
                        FILE *dumpfile;
                        char Fname[40] = "D:\\Wakeup.bs";
                        dumpfile = fopen(Fname, "wb");
                        fwrite((MMP_UBYTE *)HostAddr, ulFrameSize,1,dumpfile);
                        fclose(dumpfile);
                    }
                    #endif
                    return MMP_ERR_NONE;
                }
                ulPayloads++;
                state = ASF_PAYLOAD_START;
                
                break;
        }
    } while(1);
    return MMP_ERR_NONE;
}
#endif
static void MMPD_WMVPSR_ToUpper(MMP_UBYTE *tags, MMP_ULONG size)
{
    MMP_ULONG i;
    for(i=0; i < size; i++){
        if(tags[i] >= 0x61)
            tags[i] -= 0x20;
    }
}

static MMP_ULONG MMPD_WMVPSR_GetCodecID(const AVCodecTag *tags, MMP_ULONG tag)
{
	MMP_LONG i;
	for(i=0; tags[i].id != 0;i++) {
        if(tag == tags[i].tag)
            return tags[i].id;
    }
    /*for(i=0; tags[i].id != 0; i++) {
        if(   toupper((tag >> 0)&0xFF) == toupper((tags[i].tag >> 0)&0xFF)
           && toupper((tag >> 8)&0xFF) == toupper((tags[i].tag >> 8)&0xFF)
           && toupper((tag >>16)&0xFF) == toupper((tags[i].tag >>16)&0xFF)
           && toupper((tag >>24)&0xFF) == toupper((tags[i].tag >>24)&0xFF))
            return tags[i].id;
    }*/
    return 0x100; //unsupported
}

MMP_ERR MMPD_VIDPSR_GetWMVInfo(void *pfileinfo, MMP_BOOL bInitVideoCodec)
{
    
    MMP_VIDEO_CONTAINER_INFO *pwmvInfo = (MMP_VIDEO_CONTAINER_INFO *)pfileinfo;
    MMP_ERR                 error = MMP_ERR_NONE;

    MMPD_VIDPSR_ASFOBJECT   current;
    MMPD_VIDPSR_ASFOBJECT   header;
    MMP_ULONG64             ullDataLen;
    MMP_ULONG               i;
    MMP_LONG 	            lFileProp = 0;
    MMP_ULONG64             ullPlayDuration = 0;
    MMP_USHORT              flags;
    MMP_ULONG               ulSubObjects = 0;

    MMP_USHORT              CodecID = 0;       //see above codec ID definitions
	MMP_ULONG               AvgBitRate = 0;
	MMP_ULONG               ulMinPacketSize = 0;
	MMP_ULONG               ulMaxPacketSize = 0;
	MMP_ULONG               ulVideoStreamID = 0xFF;
	MMP_ULONG               FirstHeaderSize = 0;
	MMP_ULONG64 			ullMilliSecPerFrame[128];
	
	///- Reset the parser states
    pwmvInfo->is_video_available = 0;
    pwmvInfo->is_audio_available = 0;
    pwmvInfo->is_video_seekable = 0;
    pwmvInfo->drm_type = 0;    
    
    memset(ullMilliSecPerFrame, 0, sizeof(ullMilliSecPerFrame));
    
    m_ulCurFileOffset = 0;
    m_ulVideoFileSize = pwmvInfo->file_size;
    
    if(MMPD_WMVPSR_ReadObjHeader((MMPD_VIDPSR_ASFOBJECT *) &header) < 0)
        return VIDASF_ERROR_EOF;
    
    if(header.ullSize < 30){
        return VIDASF_ERROR_OBJECT_SIZE;
    }
    
    FirstHeaderSize = header.ullSize;
    
    if(MMPD_WMVPSR_ReadWMVFile((MMP_UBYTE *)&ulSubObjects, 4) < 0)
        return VIDASF_ERROR_EOF;
        
    if(MMPD_WMVPSR_SkipFile(2) < 0)
        return VIDASF_ERROR_SEEK;
        
    if(ulSubObjects > 0){
        header.ullDataLen = header.ullSize - 30;
        
        ullDataLen = header.ullDataLen;
        
        for(i = 0; i<(MMP_LONG)ulSubObjects; i++){
            if(ullDataLen < 24){
                PRINTF("wmv parser : not enough data for reading object\n");
                break;
            }
            
            if(MMPD_WMVPSR_ReadObjHeader(&current) < 0)
                return ASF_ERROR_EOF;
            
            if(current.ullSize > ullDataLen || current.ullSize < 24){
                break;
            }
            
            if(MMPD_WMVPSR_IsGuidMatch(&current.guid, &asf_guid_file_properties)){
                MMP_LONG cnt;
                MMP_ULONG64 tmp64;
                MMP_ULONG64 pre_roll;
                
                if (current.ullSize < 104)
					return VIDASF_ERROR_OBJECT_SIZE;

				if (lFileProp) {
					/* multiple file properties objects not allowed */
					return VIDASF_ERROR_INVALID_OBJECT;
				}
				
				lFileProp = 1;
				
				/* All we want is the play duration - uint64_t at offset 40 */
				if(MMPD_WMVPSR_SkipFile(40) < 0) /* mmsid(16)+TotalSz(8)+CreatTime(8)+Pkts(8) */
					return VIDASF_ERROR_SEEK;
				
				cnt  = MMPD_WMVPSR_ReadWMVFile((MMP_UBYTE *)&ullPlayDuration, 8);
				cnt += MMPD_WMVPSR_ReadWMVFile((MMP_UBYTE *)&tmp64, 8); //send duration
				cnt += MMPD_WMVPSR_ReadWMVFile((MMP_UBYTE *)&pre_roll, 8);
				
				if(cnt != (8*3))
					return VIDASF_ERROR_EOF;
			    
			    ullPlayDuration /= 10000; //ms
			    ullPlayDuration -=pre_roll;
			    
			    pwmvInfo->total_file_time = (MMP_ULONG)ullPlayDuration;
			    
			    MMPD_WMVPSR_SkipFile(4);
			    MMPD_WMVPSR_ReadWMVFile((MMP_UBYTE *)&ulMinPacketSize, 4);
			    MMPD_WMVPSR_ReadWMVFile((MMP_UBYTE *)&ulMaxPacketSize, 4);
			    if (ulMinPacketSize > ulMaxPacketSize)
			        return VIDASF_ERROR_INVALID_OBJECT;
			    PRINTF("ulPacketSize : %X\r\n", ulMaxPacketSize);
			    cnt = 12;
			    cnt += MMPD_WMVPSR_ReadWMVFile((MMP_UBYTE *)&AvgBitRate, 4);
			    
			    PRINTF("AvgBitRate : %d\r\n", AvgBitRate);
			    
			    if(cnt != (4*4))
					return ASF_ERROR_EOF;
			    
			    if(MMPD_WMVPSR_SkipFile((MMP_ULONG)(current.ullSize - 24/*guid+size*/ - 80)) < 0)
					return VIDASF_ERROR_SEEK;
					
            } else if(MMPD_WMVPSR_IsGuidMatch(&current.guid, &asf_guid_stream_properties)){
            
                MMPD_VIDPSR_ASFGUID guid;
                MMP_ULONG ulPropDataLen;
                
                if (current.ullSize < 78)
                    return VIDASF_ERROR_OBJECT_SIZE;

                if((MMPD_WMVPSR_ReadWMVGuid(&guid)) < 0)
                    return VIDASF_ERROR_EOF;
                
                if(MMPD_WMVPSR_SkipFile(24) < 0)
                    return VIDASF_ERROR_SEEK;
                
                if(MMPD_WMVPSR_ReadWMVFile((MMP_UBYTE *)&ulPropDataLen,4) < 0)
                    return VIDASF_ERROR_EOF;
                    
                if(MMPD_WMVPSR_SkipFile(4) < 0)
                    return VIDASF_ERROR_SEEK;

                if(MMPD_WMVPSR_ReadWMVFile((MMP_UBYTE *)&flags, 2) < 0)
                    return VIDASF_ERROR_EOF;
                
                if(MMPD_WMVPSR_IsGuidMatch(&guid, &asf_guid_stream_type_audio)){
                    if(pwmvInfo->is_audio_available == MMP_FALSE){
                    
                        if(MMPD_WMVPSR_SkipFile(4) < 0)
					        return VIDASF_ERROR_SEEK;
					    if(ulPropDataLen < 18){
					        return VIDASF_ERROR_INVALID_LENGTH;
					    }
					    /* Parse & check */
					    {
					        MMP_LONG cnt = 0;
					        MMP_USHORT tmp16;
					        
					        cnt  = MMPD_WMVPSR_ReadWMVFile((MMP_UBYTE *)&CodecID, 2);
					        cnt += MMPD_WMVPSR_ReadWMVFile((MMP_UBYTE *)&tmp16, 2);
					        pwmvInfo->audio_info.channel_nums = tmp16;
					        cnt += MMPD_WMVPSR_ReadWMVFile((MMP_UBYTE *)&tmp16, 2);
					        pwmvInfo->audio_info.sampling_rate = tmp16;
					        MMPD_WMVPSR_SkipFile(2);
					        cnt += 2;
					        cnt += MMPD_WMVPSR_ReadWMVFile((MMP_UBYTE *)&(pwmvInfo->audio_info.bitrate), 4);
					        cnt += MMPD_WMVPSR_ReadWMVFile((MMP_UBYTE *)&tmp16, 2);
					        pwmvInfo->audio_info.block_align = tmp16;
					        cnt += MMPD_WMVPSR_ReadWMVFile((MMP_UBYTE *)&tmp16, 2);
					        pwmvInfo->audio_info.bits_per_coded_sample = tmp16;
					        cnt += MMPD_WMVPSR_ReadWMVFile((MMP_UBYTE *)&tmp16, 2);
					        
					        if(cnt != (2+2+4+4+2+2+2))
					            return VIDASF_ERROR_EOF;
					            
					        pwmvInfo->audio_info.bitrate *= 8;
					        
					    }
					    if(MMPD_WMVPSR_SkipFile(current.ullSize - 24 - 72) < 0)
					        return VIDASF_ERROR_SEEK;
					    pwmvInfo->audio_info.format = MMPD_WMVPSR_GetCodecID(codec_wav_tags, CodecID);//MMP_VID_AUDIO_WMA;//MMP_VID_AUDIO_WMA;
					    pwmvInfo->audio_info.total_audio_time = pwmvInfo->total_file_time; //sec
                        pwmvInfo->is_audio_available = MMP_TRUE;
                        
                    } else {
                        if(MMPD_WMVPSR_SkipFile((MMP_ULONG)(current.ullSize - 24/*guid+size*/ - 50)) < 0)
					        return VIDASF_ERROR_SEEK;
                    }
                } else if(MMPD_WMVPSR_IsGuidMatch(&guid, &asf_guid_stream_type_video)){
                    if(pwmvInfo->is_video_available == MMP_FALSE){
                    
                        ulVideoStreamID = flags & 0x7F;
                        if(MMPD_WMVPSR_SkipFile(4) < 0)
					        return VIDASF_ERROR_SEEK;
					    if(ulPropDataLen < 18){
					        return VIDASF_ERROR_INVALID_LENGTH;
					    }
					    /* Parse & check */
					    {
					        MMP_LONG cnt = 0;
					        MMP_ULONG  tmp32;
					        
					        cnt  = MMPD_WMVPSR_ReadWMVFile((MMP_UBYTE *)&tmp32, 4);
					        pwmvInfo->video_info.width = tmp32;
					        cnt  = MMPD_WMVPSR_ReadWMVFile((MMP_UBYTE *)&tmp32, 4);
					        pwmvInfo->video_info.height = tmp32;
					        
					        MMPD_WMVPSR_SkipFile(1);
					        
					        MMPD_WMVPSR_ReadWMVFile((MMP_UBYTE *)&WMVTmpBufLen, 2);
					        
					        MMPD_WMVPSR_SkipFile(16);
					        
					        cnt  = MMPD_WMVPSR_ReadWMVFile((MMP_UBYTE *)&tmp32, 4);
					        
					        MMPD_WMVPSR_ToUpper((MMP_UBYTE *)&tmp32, 4);
					        
					        pwmvInfo->video_info.format = MMPD_WMVPSR_GetCodecID(codec_bmp_tags, tmp32);
					        
					        MMPD_WMVPSR_SkipFile(WMV_VIDEO_INFO_SIZE - 20);
					        
					        MMPD_WMVPSR_ReadWMVFile(WMVTmpBuf, WMVTmpBufLen - WMV_VIDEO_INFO_SIZE);
					        
					        AvgBitRate -= pwmvInfo->audio_info.bitrate;
					        //AvgBitRate = AvgBitRate*9/10; // guess bitrate
					        
					        pwmvInfo->video_info.bitrate = AvgBitRate;
					        //pwmvInfo->video_info.format = MMP_VID_VIDEO_MP4V; // Set WMV format
					        
					    }
					    
					    pwmvInfo->video_info.fps = 0;
					    pwmvInfo->video_info.cropping_left = 0;
					    pwmvInfo->video_info.cropping_right = 0;
					    pwmvInfo->video_info.cropping_top = 0;
					    pwmvInfo->video_info.cropping_bottom = 0;
					    pwmvInfo->video_info.total_video_time = pwmvInfo->total_file_time; // sec
					    pwmvInfo->is_video_available = MMP_TRUE;
					    if(MMPD_WMVPSR_SkipFile(current.ullSize - 24 - 62 - 3 - WMVTmpBufLen) < 0)
					        return VIDASF_ERROR_SEEK;
                    } else {
                        if(MMPD_WMVPSR_SkipFile((MMP_ULONG)(current.ullSize - 24/*guid+size*/ - 50)) < 0)
					        return VIDASF_ERROR_SEEK;
                    }
                }
            } // end of asf_guid_stream_properties
            else if(MMPD_WMVPSR_IsGuidMatch(&current.guid, &asf_guid_header_extension)){
            	MMP_ULONG ulExSize = 0;
            	MMP_ULONG ulReadSize = 0;
            	MMPD_VIDPSR_ASFOBJECT   ExGUID;
            	
            	if(MMPD_WMVPSR_SkipFile(18) < 0)
                    return VIDASF_ERROR_SEEK;
                
                if(MMPD_WMVPSR_ReadWMVFile((MMP_UBYTE *)&ulExSize, 4) < 0)
                    return VIDASF_ERROR_EOF;
                
                if (current.ullSize < (ulExSize + 18 + 24 + 4))
	                return VIDASF_ERROR_OBJECT_SIZE;
                
            	while(ulReadSize < ulExSize) {
            		if(MMPD_WMVPSR_ReadObjHeader(&ExGUID) < 0)
		                return ASF_ERROR_EOF;
		            
		            if (ExGUID.ullSize < 24)
	                    return VIDASF_ERROR_OBJECT_SIZE;
		            
		            if(MMPD_WMVPSR_IsGuidMatch(&ExGUID.guid, &asf_guid_stream_type_Ex1)){
		                MMP_ULONG ulStramID = 0;
		                
		                if (ExGUID.ullSize < 76)
	                    	return VIDASF_ERROR_OBJECT_SIZE;
		                
		                if(MMPD_WMVPSR_SkipFile(48) < 0)
		                    return VIDASF_ERROR_SEEK;
		                    
		                if(MMPD_WMVPSR_ReadWMVFile((MMP_UBYTE *)&ulStramID, 2) < 0)
		                    return VIDASF_ERROR_EOF;
		                if(ulStramID < 128) {
		                
		                	if (ExGUID.ullSize < 84)
	                    		return VIDASF_ERROR_OBJECT_SIZE;
		                
		                    if(MMPD_WMVPSR_SkipFile(2) < 0)
		                        return VIDASF_ERROR_SEEK;
		                    
		                    if(MMPD_WMVPSR_ReadWMVFile((MMP_UBYTE *)&ullMilliSecPerFrame[ulStramID], 8) < 0)
		                        return VIDASF_ERROR_EOF;
		                    
		                    ullMilliSecPerFrame[ulStramID] = ullMilliSecPerFrame[ulStramID] / 10000; // millisecond
		                    if(MMPD_WMVPSR_SkipFile((MMP_ULONG)(ExGUID.ullSize - 24 - 48 - 2 - 2 - 8)) < 0)
		            			return VIDASF_ERROR_SEEK;
		            	} else {
		            		if(MMPD_WMVPSR_SkipFile((MMP_ULONG)(ExGUID.ullSize - 24 - 48 - 2)) < 0)
		            			return VIDASF_ERROR_SEEK;
		            	}
	                } else {
	                	if(MMPD_WMVPSR_SkipFile((MMP_ULONG)(ExGUID.ullSize - 24)) < 0)
		            		return VIDASF_ERROR_SEEK;
	                }
	                ulReadSize += ExGUID.ullSize;
                }

                if(MMPD_WMVPSR_SkipFile((MMP_ULONG)(current.ullSize - (ulExSize + 18 + 24 + 4))) < 0)
		        	return VIDASF_ERROR_SEEK;
            }
            else {
                if(MMPD_WMVPSR_SkipFile((MMP_ULONG)(current.ullSize - 24)) < 0)
		            return VIDASF_ERROR_SEEK;
            }
            ullDataLen -= current.ullSize;
        }
        
        if (i != (MMP_LONG)ulSubObjects || ullDataLen != 0) {
            return VIDASF_ERROR_INVALID_VALUE;
        }
    }
    m_ulCurFileOffset = FirstHeaderSize;
    if(ullMilliSecPerFrame[ulVideoStreamID] != 0) {
        pwmvInfo->video_info.fps = 1000 / ullMilliSecPerFrame[ulVideoStreamID];
    }
    while(m_ulCurFileOffset < pwmvInfo->file_size){
        if(MMPD_WMVPSR_ReadObjHeader((MMPD_VIDPSR_ASFOBJECT *) &header) < 0)
            break;
        if(MMPD_WMVPSR_IsGuidMatch(&(header.guid), &asf_guid_index)){
            if(header.ullSize > 62) {
                pwmvInfo->is_video_seekable = 1;
            }
            break;
        } else {
            if(header.ullSize < WMV_OBJHDR_SZ){                
                break;
            }
            //Skip
            MMPD_WMVPSR_SkipFile(header.ullSize - WMV_OBJHDR_SZ);
        }
    }
    //MMPD_WMVPSR_DemuxOneFrame((MMP_ULONG)(header.ullSize + WMV_OBJECT_SIZE), ulMaxPacketSize, ulVideoStreamID, 0);
	return error;

    
}
#endif // end of ifndef BUILD_FW

#endif


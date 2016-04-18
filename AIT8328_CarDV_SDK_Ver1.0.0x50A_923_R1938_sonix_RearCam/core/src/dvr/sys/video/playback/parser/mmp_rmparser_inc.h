#ifndef _MMP_RMPARSER_INC_H_
#define _MMP_RMPARSER_INC_H_

#if defined(BUILD_FW)
#if (RV_EN)||(VRA_P_EN)

#include "aitRv_parse.h"
#include "aitRv_stream.h"
#include "rv_depack.h"
#include "rm_memory_default.h"

//Ben_RA,20091208
//#include "rm_parse.h"
//#include "rm_stream.h"
#if (VRA_P_EN)
extern MMP_ERR MMPF_Audio_UpdateVRATime(MMP_ULONG ulTime);
extern MMP_ERR MMPF_Audio_SetVRASeekTime(MMP_ULONG ulTime);
#endif

#define RM2YUV_INITIAL_READ_SIZE 16

#define RV_INFRAME_DATABUF_SIZE		(220*1024)
#define RA_INFRAME_DATABUF_SIZE		(2*1024)

#define RV_MAX_SEGMENT		100

#ifdef 	MAX_DEPACK_CACHE
#define RM_MAX_DEPACK_CACHE  MAX_DEPACK_CACHE
#else
#define RM_MAX_DEPACK_CACHE (10)
#endif

typedef struct rv_frame_struct2
{
    UINT32             ulDataLen;
    UINT32			   ulDataOffset;
    UINT32             ulTimestamp;
    UINT16             usSequenceNum;
    UINT16             usFlags;
    HXBOOL             bLastPacket;
    UINT32             ulNumSegments;
    rv_segment		   *pSegment;
    rv_segment 		   pSegments[RV_MAX_SEGMENT];
    //swyang hack, padding rv_frame2 to 8 byte aligned
    UINT8               padding[0x4];
} rv_frame2;

typedef struct {
	rm_parser*          RMpParser;
	rm_parser*			RMpAudParser;
	rm_stream_header*   RMpHdr;
	rm_packet*          RMpPacket;
	rv_depack*          RVpDepack;
	rv_format_info*     RVpInfo;
	rm_stream_header*   RV_pStreamHdr;	
	rm_stream_header*   RA_pStreamHdr;

	MMP_ULONG			RM_CurOffset[RM_MAX_DEPACK_CACHE];  		//indicate the file offset using by R/W file
	MMP_BOOL			RV_GET_BS_DATA_DONE;
	MMP_BOOL			RV_DEMUX_END;
	//UINT32				rv_working_buf[RM_WORKINGBUF_SIZE>>2];
	UINT32 				INFRAME_SIZE;
	rv_frame2			RV_INFRAME_HEADER;
	BYTE				RV_INFRAME_DATABUF[RV_INFRAME_DATABUF_SIZE];
}RMPLAYER_INFO;

//Ben_RM
#ifdef BUILD_FW
extern MMPF_VIDEO_FUNC      m_vcodec;
extern MMP_HANDLE           *hVideo;
#endif

extern rm_parser* aitRv_parser_create(void* pError, rm_error_func_ptr fpError);

/** @name RM Parser Variables
@{
*/

//Ben_RM,20091102
// global variable

extern INT32	rm_malloc_size;
#ifdef BUILD_FW
//rm_stream_header*   RADec_pStreamHdr;
extern	rm_stream_header*   RADec_pStreamHdr;
//RMPLAYER_INFO m_RmInfo;
RMPLAYER_INFO *glRmPlayer_Info = NULL;
#else
//RMPLAYER_INFO glRmPlayer_Info;
RMPLAYER_INFO *glRmPlayer_Info = (RMPLAYER_INFO*)0x03180000;
#endif


MMP_ULONG	glRvStreamNum = 0;
MMP_ULONG	glRAStreamNum = 0;
static MMP_ULONG m_SearchFrameTime = 0xFFFFFFFF;

//champ realmedia parser/depack
extern  UINT32 rv_DataBuf_Size;
extern  BYTE *rv_inFrame_DataBuf;
//champ used to save the last audio packet
#define	RM_INPACKET_DATABUF_SIZE	(24 * 1024)
#ifdef BUILD_FW
MMP_UBYTE	m_ubTmpAudioPacket[RM_INPACKET_DATABUF_SIZE];
#else
MMP_UBYTE	*m_ubTmpAudioPacket = (MMP_UBYTE*)0x03160000;
#endif
MMP_UBYTE	m_ubRMSeek = 0;
static rm_packet	m_rvpacket;	//for multiple video frame

typedef enum MMPF_RMPSR_CACHE_INDEX{
    RM_INIT_CACHE = 0,
    RM_VIDEO_BS_HEADER_CACHE,
    RM_AUDIO_BS_HEADER_CACHE,
    RM_VIDEO_BS_DATA_CACHE,
    RM_AUDIO_BS_DATA_CACHE,
    RM_VIDEO_PSR_CACHE,
    RM_AUDIO_PSR_CACHE,   // audio index table cache index 
    RM_CACHE0,
    RM_CACHE1,
    RM_CACHE2
}MMPF_RMPSR_CACHE_INDEX;

MMP_ERR MMPF_RMPSR_SeekByTime(const MMP_LONG ulTime, MMP_ULONG ulSeekOption);

#if 0 // AVI Parser API
void _____RMReadHeaderAPI_____(){}
#endif

void aitRv_callback_error2(void* pError, HX_RESULT result,const char* pszMsg)
{

    #ifdef BUILD_FW
    RTNA_DBG_Str(0, (char*)pszMsg);
	if(result){
	    RTNA_DBG_Str(0, "[");
	    RTNA_DBG_Long(0, result);
	    RTNA_DBG_Str(0, "]");
	}
    RTNA_DBG_Str(0, "\r\n");
    #else
	PRINTF("%s %x\n", pszMsg, (unsigned int)result);
    #endif
}

UINT32 aitRv_readData(void *pUserRead, BYTE *rm_pInData, UINT32 InSize)
{
	MMP_ULONG error;
	MMP_ULONG idx;
	
	idx = m_ulCacheIdx;
	
	error = MMPF_VIDPSR_ReadFile((void*)rm_pInData, glRmPlayer_Info->RM_CurOffset[m_ulCacheIdx], InSize);
	
	if(error != MMP_ERR_NONE) {
		#ifdef BUILD_FW
			DBG_S0("RM Read File error\r\n");
		#else
		PRINTF("RM Read File error\r\n");
		#endif
		return 0;
	}
	
	glRmPlayer_Info->RM_CurOffset[m_ulCacheIdx] += InSize;
	
	return	InSize;
}

void aitRv_seekData(void *pUserRead,UINT32 ulOffset, UINT32 ulOrigin)
{
	
	switch(ulOrigin){
	case HX_SEEK_ORIGIN_SET:
		glRmPlayer_Info->RM_CurOffset[m_ulCacheIdx] = ulOffset;
		break;
	case HX_SEEK_ORIGIN_CUR:
        glRmPlayer_Info->RM_CurOffset[m_ulCacheIdx] += ulOffset;
		break;
	case HX_SEEK_ORIGIN_END:
		glRmPlayer_Info->RM_CurOffset[m_ulCacheIdx] =  gVidFileInfo.file_size;
		break;
	}
	
		
}

void aitRv_SeekFp(UINT32 dstidx, UINT32 srcidx)
{
    glRmPlayer_Info->RM_CurOffset[dstidx] = glRmPlayer_Info->RM_CurOffset[srcidx];
}

#if 0 // RM Demuxer
void _____RMDemuxer_____(){}
#endif

HX_RESULT rv_default_frame_avail(void* pAvail, UINT32 ulSubStreamNum, rv_frame* pCurFrame){
	MMP_ULONG i;
	
	memcpy(&(glRmPlayer_Info->RV_INFRAME_HEADER), pCurFrame, sizeof(rv_frame));
	
    for (i = 0; i < pCurFrame->ulNumSegments ; i++){
    	glRmPlayer_Info->RV_INFRAME_HEADER.pSegments[i].bIsValid = pCurFrame->pSegment[i].bIsValid;
        glRmPlayer_Info->RV_INFRAME_HEADER.pSegments[i].ulOffset = pCurFrame->pSegment[i].ulOffset;
    }
    glRmPlayer_Info->INFRAME_SIZE = pCurFrame->ulDataLen;
   	glRmPlayer_Info->RV_INFRAME_HEADER.ulDataOffset = sizeof(rv_frame2);    
    glRmPlayer_Info->RV_GET_BS_DATA_DONE = MMP_TRUE;
	return HXR_OK;
}
MMP_ERR MMPF_RMPSR_Demux(void);
MMP_VID_FILEVALIDATION MMPF_RMPSR_RV_Init()
{
	UINT32     	ulFramesPerSec = 0;
	//UINT32      ulOutFrameSize = 0;
	HX_RESULT   retVal      = HXR_OK;
	UINT32 		i = 0;

	#if 1
    /* Create the RealVideo depacketizer */
    glRmPlayer_Info->RVpDepack = rv_depack_create((void*) glRmPlayer_Info,
                               rv_default_frame_avail,
                               NULL,
                               aitRv_callback_error2);
    if (!glRmPlayer_Info->RVpDepack)
    {
        return MMP_VID_UNKNOWN_INVALIDATION;
    }
	#endif

    /* Initialize the RV depacketizer with the stream header */
    retVal = rv_depack_init(glRmPlayer_Info->RVpDepack, glRmPlayer_Info->RV_pStreamHdr );
    if (retVal != HXR_OK)
    {
        return MMP_VID_UNKNOWN_INVALIDATION;
    }
    
    //return 0;
    /* Get the bitstream header information */
    retVal = rv_depack_get_codec_init_info(glRmPlayer_Info->RVpDepack, &(glRmPlayer_Info->RVpInfo));
    if (retVal != HXR_OK)
    {
         return MMP_VID_UNKNOWN_INVALIDATION;
    }
    #if (CHIP == P_V2)
    // not support RV20, RVTR
    if(glRmPlayer_Info->RVpInfo->ulSubMOFTag == 0x52563230 || 
        glRmPlayer_Info->RVpInfo->ulSubMOFTag == 0x52565452)
            return MMP_VID_UNSUPPORTED_FILE;
    #endif
    /* Fill in the width and height */
	gVidFileInfo.video_info[0].width = glRmPlayer_Info->RVpInfo->usWidth;
	gVidFileInfo.video_info[0].height = glRmPlayer_Info->RVpInfo->usHeight;
    /*
     * Print out the width and height so the user
     * can input this into their YUV-viewing program.
     */
     #ifdef BUILD_FW
     DBG_S0("Video in has dimensions ");
     DBG_L0(glRmPlayer_Info->RVpInfo->usWidth);
     DBG_S0(" x ");
     DBG_L0(glRmPlayer_Info->RVpInfo->usHeight);
     DBG_S0("pixels (width x height)\r\n");
     #else
     //PRINTF("[%s] width %d height %d\n", __func__, glRmPlayer_Info->RVpInfo->usWidth, glRmPlayer_Info->RVpInfo->usHeight);
     #endif
    /*
     * Get the frames per second. This value is in 32-bit
     * fixed point, where the upper 16 bits is the integer
     * part of the fps, and the lower 16 bits is the fractional
     * part. We're going to truncate to integer, so all
     * we need is the upper 16 bits.
     */
    ulFramesPerSec = glRmPlayer_Info->RVpInfo->ufFramesPerSecond >> 16;
	
	gVidFileInfo.video_info[0].bitrate = aitRv_stream_get_avg_bit_rate(glRmPlayer_Info->RV_pStreamHdr);
	gVidFileInfo.video_info[0].fps = ulFramesPerSec;
    #if 0
	//DBG_S0("Output Frame Buffer Size = ");
	//DBG_L0(ulOutFrameSize);
	DBG_S0("\r\n");
	DBG_S0("realvideo bitrate = ");
	DBG_L0(aitRv_stream_get_avg_bit_rate(glRmPlayer_Info->RV_pStreamHdr));
	DBG_S0("\r\n");
	DBG_S0("frame rate = ");
	DBG_L0(ulFramesPerSec);
	DBG_S0("\r\n");
	DBG_S0("info.ulNumInputFrames = ");
	DBG_L0(ulFramesPerSec);
	DBG_S0("\r\n");
    #endif
   	
   	//RM_CurOffset[0] is used for parser and it will indicate the start of dafa packets when ending parse.
   	//Make all Video and Audio current-offset position as the start of data packets
   	for(i=1 ; i < RM_MAX_DEPACK_CACHE ; i = i + 2){
    	glRmPlayer_Info->RM_CurOffset[i] = glRmPlayer_Info->RM_CurOffset[RM_VIDEO_PSR_CACHE];
    }
   

    if(gVidFileInfo.video_info[0].format == MMP_VID_VIDEO_RV){
	
    #if (VIDEO_P_EN)
       
        m_rvpacket.ulTime = 0xFFFFFFFF;
        #if (CHIP == MCR_V2)
        #if USE_SW_RV_DEC
	    hVideo = MMPF_VIDEO_Open(CODEC_RV, &m_vcodec);
	    
	    //NULL for no use,Ben_RM,20091117
	    if (MMP_ERR_NONE != m_vcodec.MMPF_VIDDEC_Init(hVideo, (MMP_UBYTE*)glRmPlayer_Info->RVpInfo, NULL,
                                                   gVidFileInfo.video_info.width,
                                                   gVidFileInfo.video_info.height,MMP_TRUE)) {
                                                  
               DBG_S(3,"Init video failed\r\n");
               return MMP_VID_UNSUPPORTED_FILE;
        }
        m_vcodec.MMPF_VIDDEC_GetInfo(hVideo,&gVidFileInfo.video_info);
        #endif
        #endif
	    #if (CHIP == P_V2)
        
        error = MMPF_VideoDecoder_InitCodec(MMP_VID_VIDEO_RV,
                                            //(MMP_ULONG)(&glRmPlayer_Info->RVpInfo),
                                            (MMP_ULONG)(glRmPlayer_Info->RVpInfo),
                                            sizeof(rv_format_info),
                                            &bDecodeFirstFrame,
                                            gVidFileInfo.video_info.width,
                                            gVidFileInfo.video_info.height);
        		            
        if(error != MMP_ERR_NONE) {
            DBG_S(3,"Init video failed\r\n");
            return MMP_VID_UNSUPPORTED_FILE;
        }    
        gDemuxOption |= DEMUX_VIDEO;
        MMPF_RMPSR_Demux();
        {
            MMP_ULONG                   frame_addr, frame_size, data_size,frameID,flag=0;
            MMP_ULONG64                 time;
            MMPF_BS_GetVideoData(&frame_addr,&frame_size,&data_size,&flag,&time,&frameID);
            {
                rv_frame2    *pDecFrame      = (rv_frame2*)frame_addr;
                MMP_ULONG    ulDataOffset    = (MMP_ULONG)pDecFrame->ulDataOffset;
                MMP_ULONG    ulDataLeft = 0;
                
                flag = 0;//BUFFERFLAG_DECODE_RV_HEADER;
                MMPF_MFD_DecodeData(frame_addr+ulDataOffset,
                                     frame_size-ulDataOffset,
                                     frameID,
                                     //frameID,
                                     time,
                                     flag,
                                     &ulDataLeft,
                                     0,
                                     pDecFrame->ulNumSegments,
                                     (MMP_ULONG)pDecFrame->pSegments,
                                     NULL);
            }
        }
        
        
        
        MMPF_MFD_GetInfo(&gVidFileInfo.video_info);
        #endif 
    #else //++Old implementation
		//BYTE*                   pOutFrame = HXNULL;
	    MMP_UBYTE               *dest_buf = m_VideoStreamBuf;
	    rv_format_info			*rv_info;
		UINT32 		            datalen;
		
    	//20100119, champ, fill RVpInfo into dest_buf
    	memcpy(dest_buf, glRmPlayer_Info->RVpInfo, sizeof(rv_format_info));
    	memcpy(dest_buf + sizeof(rv_format_info), glRmPlayer_Info->RVpInfo->pOpaqueData, glRmPlayer_Info->RVpInfo->ulOpaqueDataSize);
    	
    	rv_info              = (rv_format_info*)dest_buf;
    	rv_info->pOpaqueData = ((BYTE*)m_video_dest_addr) + sizeof(rv_format_info);
    	
		gVidFileInfo.video_info.width = 0;
		gVidFileInfo.video_info.height = 0;
		
		datalen = sizeof(rv_format_info) + glRmPlayer_Info->RVpInfo->ulOpaqueDataSize;
		datalen += 4 - (datalen % 4);	//alignment

		//20100128, champ, duplicate RA stream header 
		//if(gVidFileInfo.is_audio_available){
		if(glRmPlayer_Info->RA_pStreamHdr != HXNULL){
			UINT32              offset      = 4;	//skip the first for byte to store the length
			MMP_UBYTE           *databuf    = dest_buf + datalen;
			rm_property         *src_prop   = NULL;
			rm_property         *dst_prop   = NULL;
			rm_stream_header    *src_ra_hdr = glRmPlayer_Info->RA_pStreamHdr;
			rm_stream_header    *dst_ra_hdr = (rm_stream_header *)(databuf + offset);
			
			memcpy(dst_ra_hdr, src_ra_hdr, sizeof(rm_stream_header));
			offset += sizeof(rm_stream_header);
			
			if(src_ra_hdr->ulNumProperties > 0){
				dst_ra_hdr->pProperty = (rm_property*)(offset);
				for(i = 0; i < src_ra_hdr->ulNumProperties; i++){
					src_prop = src_ra_hdr->pProperty + i;
					dst_prop = (rm_property*)(databuf + offset);
					memcpy(dst_prop, src_prop, sizeof(rm_property));
					offset += sizeof(rm_property);
				}
			}
			
			dst_ra_hdr->pOpaqueData = (BYTE*)(offset);
			memcpy(databuf + offset, src_ra_hdr->pOpaqueData, src_ra_hdr->ulOpaqueDataLen);
			offset += src_ra_hdr->ulOpaqueDataLen;
			
			dst_ra_hdr->pMimeType = (char*)(offset);
			memcpy(databuf + offset, src_ra_hdr->pMimeType, strlen(src_ra_hdr->pMimeType)+1);
			offset += strlen(src_ra_hdr->pMimeType)+1;
			
			dst_ra_hdr->pStreamName = (char*)(offset);
			memcpy(databuf + offset, src_ra_hdr->pStreamName, strlen(src_ra_hdr->pStreamName)+1);
			offset += strlen(src_ra_hdr->pStreamName)+1;
			
			for(i = 0; i < src_ra_hdr->ulNumProperties; i++){
				src_prop = src_ra_hdr->pProperty + i;
				dst_prop = (rm_property*)(databuf + (UINT32)(dst_ra_hdr->pProperty)) + i;
				
				dst_prop->pName = (char*)(offset);
				memcpy(databuf + offset, src_prop->pName, strlen(src_prop->pName)+1);
				offset += strlen(src_prop->pName)+1;
				
				if(src_prop->ulType != RM_PROPERTY_TYPE_UINT32){
					dst_prop->pValue = (BYTE*)offset;
					memcpy(databuf + offset, src_prop->pValue, src_prop->ulValueLen);
					offset += src_prop->ulValueLen;
				}
			}
			
			*((UINT32*)databuf) = offset;
			
			//PRINTF("ra stream header size: %d\n", offset);
			
			datalen += offset;
		}else{
			*((UINT32*)dest_buf + datalen) = 0;
		}
		
        if (MMP_ERR_NONE != MMPD_VIDPLAYER_InitVideoDec(
                                                m_video_dest_addr,
                                                dest_buf, 
                                                datalen,
                                               &gVidFileInfo.video_info.width,
                                               &gVidFileInfo.video_info.height,
                                               &gVidFileInfo.video_info.cropping_left, 
                                               &gVidFileInfo.video_info.cropping_top,
                                               &gVidFileInfo.video_info.cropping_right,
                                               &gVidFileInfo.video_info.cropping_bottom,
                                               MMP_VID_VIDEO_RV)) {
                        
            return MMP_VID_UNSUPPORTED_FILE;
        }

	    PRINTF("[%s] width %d height %d\n", __func__, gVidFileInfo.video_info.width, gVidFileInfo.video_info.height);
    #endif //--Old implementation
	}

	return MMP_VID_FILE_VALID;
}

MMP_ERR MMPF_RMPSR_GetSingleFrame(  MMP_ULONG dest_addr,MMP_ULONG SizeToLoad,
                                            MMP_ULONG seg1     ,MMP_ULONG seg2,
                                            void* INFRAME_PTR)
{
	MMP_UBYTE   *buf_start = m_VideoStreamBuf;
	
	if(seg1 >= SizeToLoad){
		memcpy((MMP_UBYTE *)dest_addr, (MMP_UBYTE *)INFRAME_PTR, SizeToLoad);
	}
	else{
		memcpy((MMP_UBYTE *)dest_addr, (MMP_UBYTE *)INFRAME_PTR, seg1);
		INFRAME_PTR = (MMP_UBYTE *)INFRAME_PTR + seg1;
		memcpy(buf_start, INFRAME_PTR, (SizeToLoad - seg1));
	}
	return MMP_ERR_NONE;
}



MMP_ULONG MMPF_RMPSR_GetTime( )
{
    #if 1   
    return ((MMP_ULONG) glRmPlayer_Info->RV_INFRAME_HEADER.ulTimestamp);
    #else
    return 0;
    #endif
}


MMP_ERR MMPF_RMPSR_GetNextSample(void)
{
	MMP_ERR 	error 		= MMP_ERR_NONE;
	HX_RESULT   retVal      = HXR_OK;
	
	rm_packet			*pPacket;

	pPacket = &m_rvpacket;
	if(rv_depack_get_frame(glRmPlayer_Info->RVpDepack, pPacket) != HXR_OK)
	{
			while (retVal == HXR_OK)
		    {
		        /* Get the next packet */
		        retVal = aitRv_parser_get_packet(glRmPlayer_Info->RMpParser, &(glRmPlayer_Info->RMpPacket), glRvStreamNum);
		        memcpy(pPacket, glRmPlayer_Info->RMpPacket, sizeof(rm_packet));
				
		        if (retVal == HXR_OK)
		        {
		            /* Is this a video packet? */
		            if (glRmPlayer_Info->RMpPacket->usStream == glRvStreamNum)
		            {
		                /*
		                 * Put the packet into the depacketizer. When frames
		                 * are available, we will get a callback to
		                 * rv_frame_available().
		                 */
		                retVal = rv_depack_add_packet(glRmPlayer_Info->RVpDepack, glRmPlayer_Info->RMpPacket);
		            }
		            /* Destroy the packet */
		            //aitRv_parser_destroy_packet(glRmPlayer_Info->RMpParser, &glRmPlayer_Info->RMpPacket);
		            
		        }
		        
		        
		        if (retVal != HXR_OK) {
		            if(retVal == HXR_AT_END || retVal == HXR_CORRUPT_FILE) {
		        	    glRmPlayer_Info->RV_DEMUX_END = 1;
		        	    return MMP_VIDPSR_ERR_EOS;
		        	}
		            m_ulDemuxErrorCount++;
		            
		            if(m_ulDemuxErrorCount < 200) {
		                retVal = HXR_OK;
		                return MMP_VIDPSR_ERR_CONTENT_UNKNOWN_DATA;
		            } 
		            else {
    		        	glRmPlayer_Info->RV_DEMUX_END = 1;
    		        	return MMP_VIDPSR_ERR_EOS;
		        	}
		        	
		        } 
		        else {
		            m_ulDemuxErrorCount = 0;
		        }
		        
		        if (glRmPlayer_Info->RV_GET_BS_DATA_DONE){  //Get enough BS data for one frame
		        	break;
		        }
		        //Make sure everytime we call Demux, we only decode one frame
		       
		    }
	}
	return error;

}
MMP_ERR MMPF_RMPSR_SeekNextVideoFrame(MMP_ULONG *RequestFrame)
{
    MMP_ERR error = MMP_ERR_NONE;
	MMPF_3GPP_SEEKSTATUS *pSeek = &m_seek[m_ubVideoCurTrack];    
    *RequestFrame = 0;
    
    glRmPlayer_Info->RV_GET_BS_DATA_DONE = MMP_FALSE;
    if (pSeek->mode == MMPF_M_SEEK_NORMAL) {
        m_SearchFrameTime = 0xFFFFFFFF;
        error = MMPF_RMPSR_GetNextSample();
    }
    else if (pSeek->mode == MMPF_M_FF_KEY) {
        HX_RESULT   retVal      = HXR_OK;
        UINT32      ulTime = 0;
        if(m_SearchFrameTime == 0xFFFFFFFF)
            ulTime = MMPF_RMPSR_GetTime();
        else
            ulTime = m_SearchFrameTime;
        if(pSeek->sSpeed < 0) {
            if (ulTime > 0) {
                ulTime -= 1;
                MMPF_VIDPSR_SwapDepackCacheIdx(RM_CACHE0);
                retVal = aitRv_parser_searchKey(glRmPlayer_Info->RMpParser, &ulTime, MMP_TRUE, (-(pSeek->sSpeed))/100);

                m_SearchFrameTime = ulTime;

                m_rvpacket.ulTime = 0xFFFFFFFF;

                aitRv_SeekFp(RM_VIDEO_BS_DATA_CACHE, RM_CACHE0);
                MMPF_VIDPSR_SwapDepackCacheIdx(RM_VIDEO_BS_DATA_CACHE);
                error = MMPF_RMPSR_GetNextSample();
                if(retVal == HXR_AT_END)
                    error = MMP_VIDPSR_ERR_EOS;
            } else {
                error = MMPF_RMPSR_GetNextSample();
                error = MMP_VIDPSR_ERR_EOS;
            }
        } else {
            ulTime += 1;
            MMPF_VIDPSR_SwapDepackCacheIdx(RM_CACHE0);
            retVal = aitRv_parser_searchKey(glRmPlayer_Info->RMpParser, &ulTime, MMP_FALSE, (pSeek->sSpeed)/100);

            m_SearchFrameTime = ulTime;
            
            m_rvpacket.ulTime = 0xFFFFFFFF;
            
            aitRv_SeekFp(RM_VIDEO_BS_DATA_CACHE, RM_CACHE0);
            MMPF_VIDPSR_SwapDepackCacheIdx(RM_VIDEO_BS_DATA_CACHE);
            error = MMPF_RMPSR_GetNextSample();
            if(retVal == HXR_AT_END)
                error = MMP_VIDPSR_ERR_EOS;
        }
        if(error == MMP_VIDPSR_ERR_EOS)
            pSeek->mode = MMPF_M_SEEK_NONE;
    }
    else if (pSeek->mode == MMPF_M_SEEK_NONE) {
        return MMP_VIDPSR_ERR_EOS;
    }
    else{
    	#if 0
    	DBG_S0("\r\n MMPF_RMPSR_SeekNextVideoFrame error 1!!\r\n");
    	#endif
    }

    #if (CHIP == MCR_V2)
    // calculate the offsets before and after the bitstream in all cases
	m_3GPPdata.usBSOffset = sizeof(MMPF_M_FRAME_INFO);
    // ARM address padding
    m_3GPPdata.usEOFPadding = (4 - ((glRmPlayer_Info->INFRAME_SIZE + sizeof(rv_frame2) + m_3GPPdata.usBSOffset) % 4)) % 4 + RING_BUF_TAILING;
    #endif
    
	return error;
}

MMP_ERR MMPF_RMPSR_LoadAudioData(MMP_UBYTE *dataptr, MMP_ULONG Size, MMP_ULONG *LoadedSize)
{

    typedef struct TEMP_AUDIO_HEADER {
        MMP_ULONG ulTime;
        MMP_ULONG ulSize;
        MMP_USHORT ulFlag;
        MMP_USHORT ulFlag2;
        rm_packet RMPacket;
    }TEMP_AUDIO_HEADER;

	MMP_UBYTE			ubGetPacket = 0;
	MMP_UBYTE			*packet_data = NULL;
    MMP_ULONG           LoadSize;
    MMP_USHORT          usHeaderSizeToWrite = 0;
    static MMP_USHORT	usHeadherReduntSize = 0;//How many header bytes left last time
    static TEMP_AUDIO_HEADER curHeader;
    #define SIZEOF_RM_AUDIO_HEADER (12 + sizeof(rm_packet))

	HX_RESULT   retVal      = HXR_OK;
    
    if ( (m_ulPreAudioReduntSize == 0) && (m_bAudioEOS) ) {
        *LoadedSize = 0;
        return MMP_ERR_NONE;
    }
    
    LoadSize = 0;
    ubGetPacket = 0;

	if(m_ulPreAudioReduntSize == 0) {
        /* Get the next packet */
        ubGetPacket = 1;
        
        do{
	
	        retVal = aitRv_parser_get_packet(glRmPlayer_Info->RMpAudParser, &(glRmPlayer_Info->RMpPacket), glRAStreamNum);
	        if(retVal != HXR_OK)
	        	break;
        }while (glRmPlayer_Info->RMpPacket->usStream != glRAStreamNum);
        
        memcpy(&(curHeader.RMPacket), glRmPlayer_Info->RMpPacket, sizeof(rm_packet));
        
        curHeader.ulTime = rm_packet_get_timestamp(glRmPlayer_Info->RMpPacket);
        
        //20100109, firmware only
        #ifdef BUILD_FW
        if(m_ubRMSeek){
        	m_ubRMSeek = 0;
        	MMPF_Player_SetRefreshTime(curHeader.ulTime);
        	#if (VRA_P_EN)
        	MMPF_Audio_UpdateVRATime(curHeader.ulTime);
        	#endif
        }
        #endif
        usHeadherReduntSize = SIZEOF_RM_AUDIO_HEADER;
        
        //print_msg("[%s] len %d %X %X\n", __func__, glRmPlayer_Info->RMpPacket->usDataLen, glRmPlayer_Info->RM_CurOffset[RM_AUDIO_BS_HEADER_CACHE], glRmPlayer_Info->RM_CurOffset[RM_VIDEO_BS_HEADER_CACHE]);
    
    	//champ
        m_ulPreAudioReduntSize = SIZEOF_RM_AUDIO_HEADER + glRmPlayer_Info->RMpPacket->usDataLen;
    
        curHeader.ulSize = m_ulPreAudioReduntSize;
        //print_msg("[%s] time %d packet_size %d bs size %d\n", __func__, curHeader.ulTime, curHeader.RMPacket.usDataLen, curHeader.ulSize);

        curHeader.ulFlag2 ++;
        curHeader.ulFlag2 = (MMP_SHORT)(MMP_ULONG)dataptr;

		//champ
        m_ulPreAudioOffset = 0;
        
        curHeader.ulFlag = BUFFERFLAG_EOF;
				
        if (retVal != HXR_OK) {
            curHeader.ulFlag |= BUFFERFLAG_EOS;
            m_bAudioEOS = MMP_TRUE;
        }
	}
	
	if(m_ulPreAudioReduntSize){
		if (usHeadherReduntSize > 0) { // there is header to write
			MMP_USHORT i;
			if (usHeadherReduntSize >= Size) {
				usHeaderSizeToWrite = Size;
				/*
				if(ubGetPacket){
					//buffer insufficient, backup packet data
					memcpy(m_ubTmpAudioPacket, glRmPlayer_Info->RMpPacket->pData, glRmPlayer_Info->RMpPacket->usDataLen);
				}
				*/
			} else {
				usHeaderSizeToWrite = usHeadherReduntSize;
			}
			for (i = SIZEOF_RM_AUDIO_HEADER - usHeadherReduntSize;
				i < SIZEOF_RM_AUDIO_HEADER - usHeadherReduntSize + usHeaderSizeToWrite; i++) {
				*(dataptr ++) = *(((MMP_UBYTE*)(&curHeader)) + i);
			}
			m_ulPreAudioReduntSize -= usHeaderSizeToWrite;
			usHeadherReduntSize -= usHeaderSizeToWrite;
			Size -= usHeaderSizeToWrite;
		}

		if (m_ulPreAudioReduntSize > Size) {
			LoadSize = Size;
			m_ulPreAudioReduntSize -= LoadSize;
			if(ubGetPacket){
				//buffer insufficient, backup packet data
				memcpy(m_ubTmpAudioPacket, glRmPlayer_Info->RMpPacket->pData, glRmPlayer_Info->RMpPacket->usDataLen);
			}
			
		}else {
			LoadSize = m_ulPreAudioReduntSize;
			m_ulPreAudioReduntSize = 0;
			
		}
		
		//copy audio packet to dataptr;
		if(ubGetPacket){
			packet_data = glRmPlayer_Info->RMpPacket->pData;
		}else{
			packet_data = (MMP_UBYTE*)m_ubTmpAudioPacket;
		}
		
		memcpy( dataptr, packet_data + m_ulPreAudioOffset, LoadSize);
		dataptr += LoadSize;
        m_ulPreAudioOffset += LoadSize;
		//print_msg("[%s] m_ulPreAudioOffset %d LoadSize %d\n", __func__, m_ulPreAudioOffset, LoadSize);

		*LoadedSize = LoadSize + usHeaderSizeToWrite;

		if ( m_ulPreAudioReduntSize == 0){
			m_ulBuffedAudioAtoms+=1;
		}
	}
    else {
        *LoadedSize = 0;
    }

    return MMP_ERR_NONE;
}


static MMP_ERR MMPF_RMPSR_FillAudioBuffer(MMP_ULONG dest_addr,MMP_ULONG ulSize, MMP_USHORT *usTransferSize)
{
    MMP_ULONG   file_return_size2 = 1;
    MMP_ERR     error = MMP_ERR_NONE;
    
    //if(RADec_rmvb != 0)
    {
	    *usTransferSize = 0;
	    
	    while (ulSize > *usTransferSize && file_return_size2 != 0) {
	        
	        error = MMPF_RMPSR_LoadAudioData((MMP_UBYTE *)(dest_addr+ *usTransferSize),
	                                  ulSize - *usTransferSize, &file_return_size2);
	        if (error == MMP_ERR_NONE)
	            *usTransferSize += file_return_size2;
	        else
	            return error;
	    }
	}
    
    return error;
}

#if 0 // for IDE function name list by section
void _____RMParser_APIs_____(){}
#endif

MMP_ERR MMPF_RMPSR_Init(void)
{
	MMP_ULONG ulTemp;
	MMPF_3GPP_SEEKSTATUS *pSeek = &m_seek[m_ubVideoCurTrack];
	MEMSET0(&gAudioSample);
	MEMSET0(&gVideoSample);
	MEMSET0(&m_seek);
	
	///- Reset the media info
    MEMSET0(&gVideoMedia);
    MEMSET0(&gAudioMedia);
    MEMSET0(&m_3GPPdata);

    // Keep the file size for memory mode
    ulTemp = gVidFileInfo.file_size;
    
    MEMSET0(&gVidFileInfo);
    
    gVidFileInfo.file_size = ulTemp;
	
	pSeek->sSpeed               = 10; // default set to 1x speed
	gDemuxOption                = 0;
	m_SearchFrameTime           = 0xFFFFFFFF;

    m_ulDemuxErrorCount      	= 0;
    
    glRmPlayer_Info = NULL;

	return MMP_ERR_NONE;
}

MMP_ERR MMPF_RMPSR_Deinit(void)
{
    return MMP_ERR_NONE;
}


MMP_ERR MMPF_RMPSR_Demux(void)
{
	MMP_ERR                 errEOS = MMP_ERR_NONE;// If it's end of strem this time
    MMP_ERR                 error = MMP_ERR_NONE;
    MMP_ULONG               BackupRequestFrame,RequestFrame = 0;
    MMP_ULONG               SizeToLoad;
    MMP_ULONG               header_addr, frame_addr, avail_size,avail_size1,data_size;
    MMP_ULONG				NumOfFrameDecoded;
    MMPF_3GPP_SEEKSTATUS 	*pSeek = &m_seek[m_ubVideoCurTrack];
    NumOfFrameDecoded = NumOfFrameDecoded ;
        
    if ((gDemuxOption & DEMUX_AUDIO)) {
	    // clear the demux audio option
		gDemuxOption &= ~DEMUX_AUDIO;

		MMPF_VIDPSR_SwapDepackCacheIdx(RM_AUDIO_BS_DATA_CACHE);
		
		 // fill audio data
        {
            MMP_USHORT  transfer_size = 0,transfer_size1 = 0;
            MMP_ULONG   addr,addr1;
            MMP_LONG    avail_size,avail_size1;
            MMP_ULONG   ulReturnSize;
                
            ulReturnSize = 0;
                
            //if (gAudioSample.ulOffset != 0) 
            {

                if (MMPF_BS_GetAudioStartAddr(&addr, &addr1, &avail_size, &avail_size1) == MMP_ERR_NONE) {
                    if (avail_size1 < 0) { //doesn't need to rewind, plain read
                        MMPF_RMPSR_FillAudioBuffer(addr,avail_size, &transfer_size);
                        MMPF_BS_UpdateAudioBufptr(transfer_size,MMP_FALSE);
                    }
                    else {// not enough size to put all data, have to rewind
                        MMPF_RMPSR_FillAudioBuffer(addr,avail_size, &transfer_size);
                        if(transfer_size >= avail_size) {
                            MMPF_BS_UpdateAudioBufptr(transfer_size,MMP_TRUE);

                            MMPF_RMPSR_FillAudioBuffer(addr1,avail_size1, &transfer_size1);
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
    
    
	#if 1 
	if ( gDemuxOption & DEMUX_VIDEO ) {
 	    gDemuxOption &= ~DEMUX_VIDEO;
    	MMPF_VIDPSR_SwapDepackCacheIdx(RM_VIDEO_BS_DATA_CACHE);
		
		if (!m_3GPPdata.bFillBufferDone) {
			//if((!m_3GPPdata.bFilling) &&(!glRmPlayer_Info->RV_DEMUX_END)){
			if((!m_3GPPdata.bFilling)){
	    
				
				//m_3GPPdata.bFilling = MMP_TRUE;
			
				BackupRequestFrame = pSeek->ulRequestFrame;
				
				//Parse BS Header and data
				errEOS = MMPF_RMPSR_SeekNextVideoFrame(&RequestFrame);
				
				if ((errEOS == MMP_VIDPSR_ERR_EOS) && (m_bVideoEOS)) {
                    return MMP_VIDPSR_ERR_EOS;
                }
				
				if(errEOS == MMP_VIDPSR_ERR_CONTENT_UNKNOWN_DATA) {
                    return MMP_ERR_NONE;
                }
				
				pSeek->ulRequestFrame = BackupRequestFrame;
				WRITE_HEADER->ulRequestTime = MMPF_RMPSR_GetTime();
				//print_msg("[%s] %d\n", __func__, WRITE_HEADER->ulRequestTime);
				
				if ((MMP_ERR_NONE == errEOS) || (MMP_VIDPSR_ERR_EOS == errEOS)) {
                    m_3GPPdata.bFilling = MMP_TRUE;
                }
                else {
		#ifdef BUILD_FW
                    DBG_S(0, " Seek Error\r\n");
		#else
                    PRINTF("Seek Error\n");
		#endif
                    return errEOS;
                }
			   
			}
		    //if(glRmPlayer_Info->RV_DEMUX_END && m_bVideoEOS)
		    //    return MMP_VIDPSR_ERR_EOS;
		    if(errEOS ==  MMP_VIDPSR_ERR_EOS){
		    	
			    m_bVideoEOS = MMP_TRUE;
			}
		    
		    data_size = glRmPlayer_Info->INFRAME_SIZE + sizeof(rv_frame2) + m_3GPPdata.usBSOffset + m_3GPPdata.usEOFPadding;
	           
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
	   			m_3GPPdata.bLargeFrame = MMP_FALSE;
	            m_3GPPdata.bFilling    = MMP_FALSE; //skip this frame
                return error;    
	        }
	        else {
	        	return error;
	        }
	    
	        WRITE_HEADER->ulSize    = data_size;
	        //DBG_S0("=====Ben start Demux  006========\r\n");    
	        avail_size -= m_3GPPdata.usBSOffset;   
	        WRITE_HEADER->ulBSSize      = glRmPlayer_Info->INFRAME_SIZE + sizeof(rv_frame2);
	        WRITE_HEADER->ulAddress     = frame_addr;
	        WRITE_HEADER->ulFlag        = 0;
			
			if ((glRmPlayer_Info->INFRAME_SIZE + sizeof(rv_frame2)) < (avail_size + avail_size1)){
				SizeToLoad =  glRmPlayer_Info->INFRAME_SIZE + sizeof(rv_frame2);
				WRITE_HEADER->ulFlag |= BUFFERFLAG_EOF;
				if(m_bVideoEOS){
				    //DBG_S(0, "Set Eos Flag\r\n");
					WRITE_HEADER->ulFlag |= BUFFERFLAG_EOS;
					WRITE_HEADER->ulFlag |= BUFFERFLAG_FRAME_EOS;
				}
			}
			else{
				SizeToLoad =  glRmPlayer_Info->INFRAME_SIZE + sizeof(rv_frame2);
			}
			
			glRmPlayer_Info->INFRAME_SIZE = 0;
	        MMPF_BS_AccessVideoHeader((MMP_UBYTE*)header_addr, (MMP_UBYTE*)WRITE_HEADER, sizeof(MMPF_M_FRAME_INFO),RING_WRITE);
	            
	           
	        error = MMPF_RMPSR_GetSingleFrame(WRITE_HEADER->ulAddress, SizeToLoad, avail_size, avail_size1, &(glRmPlayer_Info->RV_INFRAME_HEADER));

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
				
            m_3GPPdata.pPreWriteHeader.ulRequestTime  = WRITE_HEADER->ulRequestTime;
			  
			  
		} //if (!m_3GPPdata.bFillBufferDone)   
            
	 } //if ( gDemuxOption & DEMUX_VIDEO )
	
	#endif
	
    return  MMP_ERR_NONE;
}
MMP_ULONG MMPF_RMPSR_SetWorkingBuf(MMP_ULONG ulAddr)
{
    #ifdef BUILD_FW
    MMP_ULONG ulStart = ulAddr;
    VAR_L(0, ulAddr);
    ulAddr = ALIGN32(ulAddr);
	glRmPlayer_Info = (RMPLAYER_INFO *)ulAddr;
	ulAddr += sizeof(RMPLAYER_INFO);
	
	memset(glRmPlayer_Info, 0, sizeof(RMPLAYER_INFO));
		
	glRmPlayer_Info->RMpParser        = HXNULL;
	glRmPlayer_Info->RMpAudParser     = HXNULL;
	glRmPlayer_Info->RMpHdr           = HXNULL;
	glRmPlayer_Info->RMpPacket        = HXNULL;
	glRmPlayer_Info->RVpDepack        = HXNULL;
	glRmPlayer_Info->RVpInfo          = HXNULL;
	glRmPlayer_Info->RA_pStreamHdr	 = HXNULL;
	glRmPlayer_Info->RV_pStreamHdr	 = HXNULL;  
        
    MEMSET0(&(glRmPlayer_Info->RV_INFRAME_HEADER));
	MEMSET0(&(glRmPlayer_Info->RV_INFRAME_DATABUF));
	
	//20091222, champ, initialize RealVideo depack buffer
	rv_inFrame_DataBuf = (BYTE*)(&(glRmPlayer_Info->RV_INFRAME_DATABUF));
	rv_DataBuf_Size = RV_INFRAME_DATABUF_SIZE;
	
	ulAddr += aitRv_parser_set_buf((BYTE *)ulAddr);
	VAR_L(0, ulAddr);
	
	ulAddr += aitRm_memory_set_workingbuf((BYTE *)ulAddr);
	#if USE_SW_RV_DEC
	ulAddr += aitRv_memory_set_workingbuf((BYTE *)ulAddr);
	#endif
	VAR_L(0, ulAddr);
	return (ulAddr - ulStart);
	#endif
}

MMP_VID_FILEVALIDATION MMPF_RMPSR_Parser(void)
{
	HX_RESULT   retVal         = HXR_OK;
	MMP_ULONG 	tmp_total_size = 0,i=0;
	MMP_UBYTE  	ucBuf[RM2YUV_INITIAL_READ_SIZE];
	UINT32      ulNumStreams   = 0;
	MMP_ULONG   ulWorkingBuf = m_ulWorkingBufStart;
	MMP_ULONG   ulWorkingBufSize = 0;

	MMP_VID_FILEVALIDATION     VID_VALIDATION = MMP_VID_FILE_VALID;
	  
	///- Reset the parser states
    MMPF_Player_GetSupportedFormat(&m_3GPPdata.sSupportedAudioFmt,
                                   &m_3GPPdata.sSupportedVideoFmt);
    
    #ifndef BUILD_FW
    if (MMP_VID_FILE_VALID != MMPF_PSR_ResetFS()) {
	    return MMP_VID_INVALID_FILE_NAME;
	}
	#endif
	
    MMPF_VIDPSR_ResetCache();
    MMPF_VIDPSR_SwapDepackCacheIdx(RM_INIT_CACHE);
    m_ulCurFileOffset = 0;
    
    #ifdef BUILD_FW
    // Will Add, Init working buffer
    ulWorkingBufSize += MMPF_RMPSR_SetWorkingBuf(ulWorkingBuf);
    ulWorkingBuf += ulWorkingBufSize;
    m_ulTempBSSize = 0;
    m_ulWorkingBufSize = ulWorkingBufSize;
    // end init
    #endif
    
    m_SearchFrameTime = 0xFFFFFFFF;
    
    for(i=0; i < RM_MAX_DEPACK_CACHE; i++){
    	glRmPlayer_Info->RM_CurOffset[i] = 0;
    }
    
	//DBG_S0("Ben_RM parser 2 002\r\n");
	tmp_total_size = gVidFileInfo.file_size;

//	MMPF_3GPPSR_ResetAudioBuf();
    m_ulPreAudioOffset      = 0;
	m_ulPreAudioReduntSize  = 0;
    m_ulAudioPtr  = 0;		
	 
	//rm_malloc_pbuf = (BYTE*)glRmPlayer_Info->rv_working_buf;
	rm_malloc_size = 0;	
	
	aitRv_readData(NULL, ucBuf, RM2YUV_INITIAL_READ_SIZE);	
	
	if (!aitRv_parser_is_rm_file(ucBuf, RM2YUV_INITIAL_READ_SIZE))
    {
    	#ifdef BUILD_FW
        DBG_S0("Not a .rm file.\r\n");
	    #else
        PRINTF("Not a .rm file.\n");
    	#endif
        return MMP_VID_UNSUPPORTED_FILE;
    }

    glRmPlayer_Info->RMpParser = aitRv_parser_create(NULL, aitRv_callback_error2);
    
    if(!glRmPlayer_Info->RMpParser){
    	#ifdef BUILD_FW
    	DBG_S0("Create RM parser error !\r\n");
	#else
    	PRINTF("Create RM parser error !\n");
    	#endif
    	return MMP_VID_UNKNOWN_INVALIDATION;
    }
    													   
    retVal = aitRv_parser_init_io(glRmPlayer_Info->RMpParser, NULL, aitRv_readData ,aitRv_seekData);
	if (retVal != HXR_OK)
    {
    	#ifdef BUILD_FW
    	DBG_S0("Initial I/O fial !\r\n");
	#else
    	PRINTF("Initial I/O fail !\n");
    	#endif
    	return MMP_VID_UNKNOWN_INVALIDATION;
    }
    
    retVal = aitRv_parser_read_headers(glRmPlayer_Info->RMpParser);
    if (retVal != HXR_OK)
    {
    	#ifdef BUILD_FW
       	DBG_S0("Read head fial !\r\n");
	#else
       	PRINTF("Read headers fail !\n");
       	#endif
       	return MMP_VID_UNKNOWN_INVALIDATION;
    }
	
	gVidFileInfo.total_file_time = aitRv_parser_get_duration(glRmPlayer_Info->RMpParser);
	//return 0;
    /* Get the number of streams */
    ulNumStreams = aitRv_parser_get_num_streams(glRmPlayer_Info->RMpParser);
    if (ulNumStreams == 0)
    {
    	#ifdef BUILD_FW
        DBG_S0("Get num of stream fail !\r\n");
	    #else
        PRINTF("Get num of stream fail !\n");
        #endif
        return MMP_VID_UNKNOWN_INVALIDATION;
    }
    
    
    gVidFileInfo.is_audio_available = MMP_FALSE;
    gVidFileInfo.is_video_available[0] = MMP_FALSE;
    gVidFileInfo.is_video_seekable[0] = MMP_FALSE;
    
    for (i = 0; i < ulNumStreams && retVal == HXR_OK; i++)
    {
        retVal = aitRv_parser_get_stream_header(glRmPlayer_Info->RMpParser, i, &(glRmPlayer_Info->RMpHdr));
        if (retVal == HXR_OK)
        {
        
        	//Ben_RM,20091207,add RA support
        	if (aitRv_stream_is_realaudio(glRmPlayer_Info->RMpHdr))
            {
               	glRAStreamNum = (UINT16) i;
               	gVidFileInfo.is_audio_available = MMP_TRUE;
               	gVidFileInfo.audio_info.format = MMP_VID_AUDIO_RA;
               	glRmPlayer_Info->RA_pStreamHdr = glRmPlayer_Info->RMpHdr;
            }
            else if (aitRv_stream_is_realvideo(glRmPlayer_Info->RMpHdr))
            {
                MMP_ULONG ulIndexOffset = 0;
                MMP_UBYTE ubTemp[4];
                MMP_ERR   error = MMP_ERR_NONE;
                glRvStreamNum = (UINT16) i;
                gVidFileInfo.video_info[0].total_video_time = aitRv_stream_get_duration(glRmPlayer_Info->RMpHdr);
                gVidFileInfo.is_video_available[0] = MMP_TRUE;
                gVidFileInfo.video_info[0].format = MMP_VID_VIDEO_RV;
                //gVidFileInfo.is_video_seekable = aitRv_parser_get_indexoffset(glRmPlayer_Info->RMpParser, gVidFileInfo.file_size);
                gVidFileInfo.is_video_seekable[0] = 0;
                ulIndexOffset = aitRv_parser_get_indexoffset(glRmPlayer_Info->RMpParser, gVidFileInfo.file_size);
                if(ulIndexOffset != 0) {
                    error = MMPF_VIDPSR_ReadFile( ubTemp, ulIndexOffset, 4);
                    if( (error == MMP_ERR_NONE) && ((ubTemp[0] == 'I') && (ubTemp[1] == 'N') && (ubTemp[2] == 'D') && (ubTemp[3] == 'X')))
                        gVidFileInfo.is_video_seekable[0] = 1;
                }
                glRmPlayer_Info->RV_pStreamHdr = glRmPlayer_Info->RMpHdr;
                //break;
            }
            else
            {
                /* Destroy the stream header */
                aitRv_parser_destroy_stream_header(glRmPlayer_Info->RMpParser, &(glRmPlayer_Info->RMpHdr));
            }
        }
    }

   
   	for(i=1 ; i < RM_MAX_DEPACK_CACHE ; i++){
    	glRmPlayer_Info->RM_CurOffset[i] = glRmPlayer_Info->RM_CurOffset[0];
    }
   
    /* Do we have a Real Audio stream in this file? */
    if (!glRmPlayer_Info->RA_pStreamHdr)
    {
    	#ifdef BUILD_FW
        DBG_S0("There is no Real Audio stream in this file.\r\n");
	    #else
    	PRINTF("There is no Real Audio stream in this file.\r\n");
        #endif
    }
	
	/* Do we have a Real Video stream in this file? */
	if (!glRmPlayer_Info->RV_pStreamHdr)
    {
    	#ifdef BUILD_FW
        DBG_S0("There is no RealVideo stream in this file.\r\n");
	    #else
        PRINTF("There is no RealVideo stream in this file.\r\n");
        #endif    
    }
	
	//20091229, champ, here we duplicate the parser instance for audio usage. 
	//since aitRv_parser_get_packet will calculate parsed offset internally.
	//With shared parser instance,  aitRv_parser_get_packet will return error when audio+video accumulated size > file size;
	glRmPlayer_Info->RMpAudParser = (rm_parser*)aitRv_parser_dup(glRmPlayer_Info->RMpParser);
	
	if(gVidFileInfo.is_audio_available){
		gVidFileInfo.audio_info.bitrate = aitRv_parser_get_avg_bit_rate(glRmPlayer_Info->RMpAudParser);
		gVidFileInfo.audio_info.total_audio_time =aitRv_parser_get_duration(glRmPlayer_Info->RMpAudParser);
	
		//20100115, champ, firmware only, need to pass StreamHdr from Host to Dev
		#ifdef BUILD_FW
		RADec_pStreamHdr     = glRmPlayer_Info->RA_pStreamHdr;
		#else

		PRINTF("%s set Audio PlayHandshakeBuf %d\n", __func__, (MMP_ULONG)m_pHandShakeBuf1);
		MMPD_AUDIO_SetPlayHandshakeBuf((MMP_ULONG)m_pHandShakeBuf1, MMPD_AUDIO_PLAY_HANDSHAKE_BUF_SIZE);
		#endif
		
		for(i=2 ; i < RM_MAX_DEPACK_CACHE ; i = i + 2){
	    	glRmPlayer_Info->RM_CurOffset[i] = glRmPlayer_Info->RM_CurOffset[RM_AUDIO_PSR_CACHE];
	    }
		
	}
	if(gVidFileInfo.total_file_time == 0) {
	    gVidFileInfo.total_file_time = 
	                            gVidFileInfo.video_info[0].total_video_time;
	}
	if(gVidFileInfo.is_video_available){
		MMPF_VIDPSR_SwapDepackCacheIdx(RM_VIDEO_PSR_CACHE);
		VID_VALIDATION = MMPF_RMPSR_RV_Init();
	}
	
	return VID_VALIDATION;


}
#else
MMP_ERR MMPF_RMPSR_Demux(void)
{
    return MMP_VIDPSR_ERR_CONTENT_UNKNOWN_DATA;
}

MMP_VID_FILEVALIDATION MMPF_RMPSR_Parser(void)
{
    return MMP_VID_UNKNOWN_INVALIDATION;
}
#endif // end of (RV_EN) || (VRA_P_EN)

#else  // else of of if BUILD_FW

#if 0 // for IDE function name list by section
void _____RMHostParser_____(){}
#endif

#define RM_FILE_HEADER (0x2E524D46) //.RMF
#define RM_PROP_HEADER (0x50524F50) //PROP
#define RM_MDPR_HEADER (0x4D445052) //MDPR
#define RM_DATA_HEADER (0x44415441) //DATA
#define RM_INDX_HEADER (0x494E4458) //INDX

#define RM_AUDIO_MIME_TYPE		 "audio/x-pn-realaudio"
#define RM_AUDIO_ENCRYPTED_MIME_TYPE	 "audio/x-pn-realaudio-encrypted"
#define RM_VIDEO_MIME_TYPE		 "video/x-pn-realvideo"
#define RM_VIDEO_ENCRYPTED_MIME_TYPE	 "video/x-pn-realvideo-encrypted"

#define RA_CODEC_COOK  0x01000000
#define RA_CODEC_RAAC  0x02000000

MMP_ERR MMPD_RMPSR_ReadFile(MMP_UBYTE *ubTag, MMP_ULONG ulOffset, MMP_ULONG ulSize)
{
    MMP_ERR error = MMP_ERR_NONE;
    //error = MMPD_3GPPSR_ReadFile(ubTag, ulOffset, ulSize);//TBD
    return error;
}

MMP_BOOL MMPD_RMPSR_CompareTagEqual(MMP_UBYTE *Tag, MMP_ULONG Name)
{
    MMP_ULONG ulTag = 0;
    ulTag = (Tag[0] << 24) | (Tag[1] << 16) | (Tag[2] << 8) | Tag[3];
    if(ulTag == Name)
        return MMP_TRUE;
    return MMP_FALSE;
}

MMP_ERR MMPD_RMPARSER_GetRaInfo(MMP_AUDIO_INFO *info, MMP_ULONG ulOffset, MMP_ULONG ulSize)
{
    MMP_ERR error = MMP_ERR_NONE;
    MMP_UBYTE ubBuf[4];
    MMP_USHORT usVer = 0;
    MMP_ULONG  ulCodec4CC = 0;
    
    ulOffset += 4;
    MMPD_RMPSR_ReadFile(ubBuf, ulOffset, 2);
    ulOffset += 2;
    usVer = (ubBuf[0] << 8) | (ubBuf[1]);
    if(usVer == 3) {
        return MMP_VIDPSR_ERR_NOT_IMPLEMENTED;
    } else if(usVer == 4 || usVer == 5){
        if(usVer == 4) {
            ulOffset += 42;
        } else {
            ulOffset += 48;
        }
        MMPD_RMPSR_ReadFile(ubBuf, ulOffset, 2);
        info->sampling_rate = (ubBuf[0] << 8) | (ubBuf[1]);
        ulOffset += 4;
        MMPD_RMPSR_ReadFile(ubBuf, ulOffset, 2);
        info->bits_per_coded_sample = (ubBuf[0] << 8) | (ubBuf[1]);
        ulOffset += 2;
        MMPD_RMPSR_ReadFile(ubBuf, ulOffset, 2);
        info->channel_nums = (ubBuf[0] << 8) | (ubBuf[1]);
        ulOffset += 2;
        if(usVer == 4) {
            ulOffset += 12;
        } else {
            ulOffset += 4;
        }
        MMPD_RMPSR_ReadFile(ubBuf, ulOffset, 4);
        ulCodec4CC = (ubBuf[0] << 24) | (ubBuf[1] << 16) | (ubBuf[2] << 8) | (ubBuf[3]);
        if(ulCodec4CC == 0x636F6F6B) {
            info->format = RA_CODEC_COOK;
        } else if(ulCodec4CC == 0x72616163 || ulCodec4CC == 0x72616370){
            info->format = RA_CODEC_RAAC;
        }
    } else {
        return MMP_VIDPSR_ERR_NOT_IMPLEMENTED;
    }
    return error;
}

MMP_ERR MMPD_RMPARSER_GetRvInfo(MMP_VIDEO_INFO *info, MMP_ULONG ulOffset, MMP_ULONG ulSize)
{
    MMP_ERR error = MMP_ERR_NONE;
    MMP_UBYTE ubBuf[4];
    
    MEMSET0(ubBuf);
    ulOffset += 12;
    MMPD_RMPSR_ReadFile(ubBuf, ulOffset, 2);
    ulOffset += 2;
    info->width = (ubBuf[0] << 8) | (ubBuf[1]);

    MMPD_RMPSR_ReadFile(ubBuf, ulOffset, 2);
    ulOffset += 2;
    info->height = (ubBuf[0] << 8) | (ubBuf[1]);
    
    ulOffset += 6;
    MMPD_RMPSR_ReadFile(ubBuf, ulOffset, 4);
    info->fps = ((ubBuf[0] << 24) | (ubBuf[1] << 16) | (ubBuf[2] << 8) | (ubBuf[3])) >> 16;
    return error;
}

MMP_VID_FILEVALIDATION MMPD_VIDPSR_GetRMInfo(void *pfileinfo, MMP_BOOL bInitVideoCodec)
{
    MMP_ERR error = MMP_ERR_NONE;
    MMP_VID_FILEVALIDATION file_err = MMP_VID_FILE_VALID;
    MMP_VIDEO_CONTAINER_INFO *pRmInfo = (MMP_VIDEO_CONTAINER_INFO *)pfileinfo;
    MMP_ULONG ulRmOffset = 0;
    MMP_UBYTE ubMime[50];
    MMP_ULONG ulSize = 0;
    MMP_UBYTE ubTag[4];
    MMP_UBYTE ubTemp[4];
    MMP_UBYTE ubVideoDone = MMP_FALSE, ubAudioDone = MMP_FALSE;
    
    pRmInfo->is_video_available = 0;
    pRmInfo->is_audio_available = 0;
    pRmInfo->is_video_seekable  = 0;
    pRmInfo->drm_type = 0;    
    
    //MEMSET0(&ubMime);
    memset((void *)ubMime, 0, sizeof(ubMime));
    
    MMPD_RMPSR_ReadFile(ubTag, ulRmOffset, 4);
    ulRmOffset += 4;
    
    if(MMPD_RMPSR_CompareTagEqual(ubTag, RM_FILE_HEADER) == MMP_FALSE)
        return MMP_VID_UNKNOWN_FILE_HEADER;
    
    MMPD_RMPSR_ReadFile(ubTag, ulRmOffset, 4);
    ulRmOffset += 4;
    ulSize = (ubTag[0] << 24) | (ubTag[1] << 16) | (ubTag[2] << 8) | ubTag[3];
    ulRmOffset += ulSize - 8;
    
    while(1) {
        
        if(((ulRmOffset + 8) >= pRmInfo->file_size) || (ubVideoDone && ubAudioDone))
            break;
        
        MMPD_RMPSR_ReadFile(ubTag, ulRmOffset, 4);
        ulRmOffset += 4;
        MMPD_RMPSR_ReadFile(ubTemp, ulRmOffset, 4);
        ulRmOffset += 4;
        ulSize = (ubTemp[0] << 24) | (ubTemp[1] << 16) | (ubTemp[2] << 8) | ubTemp[3];
        
        if((ulRmOffset - 8 + ulSize) > pRmInfo->file_size)
            break;
        
        if(MMPD_RMPSR_CompareTagEqual(ubTag, RM_PROP_HEADER) == MMP_TRUE) {
            MMP_ULONG ulOffset = 0;
            ulRmOffset += (2 + 4*5);
            MMPD_RMPSR_ReadFile(ubTemp, ulRmOffset, 4);
            pRmInfo->total_file_time = (ubTemp[0] << 24) | (ubTemp[1] << 16) | (ubTemp[2] << 8) | ubTemp[3];
            ulRmOffset += 8;
            MMPD_RMPSR_ReadFile(ubTemp, ulRmOffset, 4);
            ulOffset = (ubTemp[0] << 24) | (ubTemp[1] << 16) | (ubTemp[2] << 8) | ubTemp[3];
            if (ulOffset != 0 && ulOffset < pRmInfo->file_size) {
                MMPD_RMPSR_ReadFile(ubTemp, ulOffset, 4);
                if( (ubTemp[0] == 'I') && (ubTemp[1] == 'N') && (ubTemp[2] == 'D') && (ubTemp[3] == 'X') )
                    pRmInfo->is_video_seekable = MMP_TRUE;
            }
            ulRmOffset += ulSize - 38;
        } else if(MMPD_RMPSR_CompareTagEqual(ubTag, RM_MDPR_HEADER) == MMP_TRUE) {
            MMP_ULONG AvgBitrate = 0;
            MMP_ULONG ulDuration = 0;
            MMP_ULONG ulTempOffset = ulRmOffset;
            MMP_ULONG ulSpecSize = 0;
            //MEMSET0(&ubMime);
            memset((void *)ubMime, 0, sizeof(ubMime));
            // bitrate
            ulRmOffset += 8;
            MMPD_RMPSR_ReadFile(ubTemp, ulRmOffset, 4);
            AvgBitrate = (ubTemp[0] << 24) | (ubTemp[1] << 16) | (ubTemp[2] << 8) | ubTemp[3];
            // druation
            ulRmOffset += 20;
            MMPD_RMPSR_ReadFile(ubTemp, ulRmOffset, 4);
            ulDuration = (ubTemp[0] << 24) | (ubTemp[1] << 16) | (ubTemp[2] << 8) | ubTemp[3];
            // skip description
            ulRmOffset += 4;
            MMPD_RMPSR_ReadFile(ubTemp, ulRmOffset, 1);
            ulRmOffset += ubTemp[0] + 1;
            // Read MIME
            MMPD_RMPSR_ReadFile(ubTemp, ulRmOffset, 1);
            ulRmOffset += 1;
            MMPD_RMPSR_ReadFile(ubMime, ulRmOffset, ubTemp[0]);
            ulRmOffset += ubTemp[0];
            
            if (!strcmp((char *)ubMime, RM_AUDIO_MIME_TYPE) ||
                !strcmp((char *)ubMime, RM_AUDIO_ENCRYPTED_MIME_TYPE)) {
                MMPD_RMPSR_ReadFile(ubTemp, ulRmOffset, 4);
                ulRmOffset += 4;
                ulSpecSize = (ubTemp[0] << 24) | (ubTemp[1] << 16) | (ubTemp[2] << 8) | ubTemp[3];
                error = MMPD_RMPARSER_GetRaInfo(&(pRmInfo->audio_info), ulRmOffset, ulSpecSize);
                if (error == MMP_ERR_NONE) {
                    pRmInfo->is_audio_available = MMP_TRUE;
                    ubAudioDone = MMP_TRUE;
                    pRmInfo->audio_info.bitrate = AvgBitrate;
                    pRmInfo->audio_info.total_audio_time = ulDuration;
                    pRmInfo->audio_info.format |= MMP_VID_AUDIO_RA;
                }
            } 
            else if (!strcmp((char *)ubMime, RM_VIDEO_MIME_TYPE) ||
                    !strcmp((char *)ubMime, RM_VIDEO_ENCRYPTED_MIME_TYPE)) {
                MMPD_RMPSR_ReadFile(ubTemp, ulRmOffset, 4);
                ulRmOffset += 4;
                ulSpecSize = (ubTemp[0] << 24) | (ubTemp[1] << 16) | (ubTemp[2] << 8) | ubTemp[3];
                error = MMPD_RMPARSER_GetRvInfo(&(pRmInfo->video_info), ulRmOffset, ulSpecSize);
                if (error == MMP_ERR_NONE) {
                    pRmInfo->is_video_available = MMP_TRUE;
                    ubVideoDone = MMP_TRUE;
                    pRmInfo->video_info.bitrate = AvgBitrate;
                    pRmInfo->video_info.total_video_time = ulDuration;
                    pRmInfo->video_info.format = MMP_VID_VIDEO_RV;
                }
            }
            ulRmOffset = (ulTempOffset + ulSize - 8);
        } else if(MMPD_RMPSR_CompareTagEqual(ubTag, RM_DATA_HEADER) == MMP_TRUE) {
            break;
        } else {
            ulRmOffset += ulSize - 8;
        }
    }
    if(!ubVideoDone && !ubAudioDone)
        return MMP_VID_NO_AV_TRACK;
    if(pRmInfo->total_file_time == 0) {
        if(pRmInfo->video_info.total_video_time >= pRmInfo->audio_info.total_audio_time)
            pRmInfo->total_file_time = pRmInfo->video_info.total_video_time;
        else
            pRmInfo->total_file_time = pRmInfo->audio_info.total_audio_time;
    }
    //pRmInfo->video_info.total_video_time /= 1000; // base on second
    //pRmInfo->audio_info.total_audio_time /= 1000; // base on second
    return file_err;
}
#endif // end of of if BUILD_FW
#endif // end of ifndef _MMP_RMPARSER_INC_H_

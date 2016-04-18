/**
 @file mmp_3gpparser_inc.h
 @brief A common 3GP demuxer
 
 A common 3GP demuxer file which is both used in firmware and the host side code.
 
 This is a very special .h file. It's an inline c file. We usually use .inc file extention
 but it would need .mcp file to add one more file.
 Because this is a common file used in firmware and the host side, we don't know
 which include file it need, such as mmp_lib.h and includes_fw.h. So I make it as a header file
 form.
 
 @author Truman Yang
 @version 1.0 Original Version
*/

#ifndef _MMP_3GPPARSER_INC_H_
#define _MMP_3GPPARSER_INC_H_
#if !defined(BUILD_FW)
#define VIDEO_P_EN 0
#define VAAC_P_EN 0
#define VAMR_P_EN 0
#define H264_DEC_SUPPORT 0
#endif
#if !defined(BUILD_FW)||(VIDEO_P_EN)
#include "mmp_3gpparser.h"
//#include "mmp_aviparser.h"
//==============================================================================
//
//                              CONSTANTS
//
//==============================================================================
#define ATOM_FTYP     MAKE_TAG_BE('f','t','y','p') ///< QuickTime Prefs file types [P:NULL, C:NULL]
#define ATOM_MDAT     MAKE_TAG_BE('m','d','a','t') ///< Media data atom [P:NULL, C:Chunk]
#define ATOM_IODS     MAKE_TAG_BE('i','o','d','s')
#define ATOM_VMHD     MAKE_TAG_BE('v','m','h','d') ///< video Media Info Atom [P:minf, C:NULL]
#define ATOM_SMHD     MAKE_TAG_BE('s','m','h','d')
#define ATOM_DINF     MAKE_TAG_BE('d','i','n','f') ///< Data Info atom [P:minf, C:dref]
#define ATOM_MVHD     MAKE_TAG_BE('m','v','h','d') ///< Movie header atom  [P:moov, C:NULL]
#define ATOM_MOOV     MAKE_TAG_BE('m','o','o','v') ///< Movie Atom [P:NULL, C:mvhd, trak]
#define ATOM_MDIA     MAKE_TAG_BE('m','d','i','a') ///< Media Atom [P:trak, C:mdhd, hdlr, minf]
#define ATOM_TRAK     MAKE_TAG_BE('t','r','a','k') ///< Track Atom. [P:moov, C:tkhd, mdia]
#define ATOM_TKHD     MAKE_TAG_BE('t','k','h','d') ///< Track header Atom. [P:trak, C:NULL]
#define ATOM_MINF     MAKE_TAG_BE('m','i','n','f') ///< Media Info. [P:mdia C:vmhd, dinf, stbl]
#define ATOM_SOUN     MAKE_TAG_BE('s','o','u','n')
#define ATOM_VIDE     MAKE_TAG_BE('v','i','d','e')
#define ATOM_MDHD     MAKE_TAG_BE('m','d','h','d') ///< Media Header Atom. [P:mdia C:NULL]
#define ATOM_HDLR     MAKE_TAG_BE('h','d','l','r') ///< Media Handler Reference Atom. [P:mdia C:NULL]
#define ATOM_STBL     MAKE_TAG_BE('s','t','b','l') ///< Sample Table Atom [P:minf C:stsd, stts, stsc, stsz, stco, stss]
#define ATOM_STSD     MAKE_TAG_BE('s','t','s','d') ///< Sample description atom [P:stbl, C:NULL]
#define ATOM_STTS     MAKE_TAG_BE('s','t','t','s') ///< Time to sample atom [P:stbl, C:NULL]
#define ATOM_STSC     MAKE_TAG_BE('s','t','s','c') ///< Sample to chunk atom [P:stbl, C:NULL]
#define ATOM_STSZ     MAKE_TAG_BE('s','t','s','z') ///< Sample size atom [P:stbl, C:NULL]
#define ATOM_STCO     MAKE_TAG_BE('s','t','c','o') ///< Chunk offset atom [P:stbl, C:NULL]
#define ATOM_STSS     MAKE_TAG_BE('s','t','s','s') ///< Sync Sample atom [P:stbl, C:NULL] contains video keyframe
#define ATOM_CTTS     MAKE_TAG_BE('c','t','t','s') ///< Composition time to sample atom [P:stbl, C:NULL]
#define ATOM_SKIP     MAKE_TAG_BE('s','k','i','p') ///< Skip atom
#define ATOM_PANA     MAKE_TAG_BE('P','A','N','A') ///< PANA atom

#define STSZ_BUFFER_SIZE 27                         ///< Number of STSZ entries to keep in the memory.
#define STSC_BUFFER_SIZE 15                         ///< Number of STSC entries to keep in the memory.
#define STCO_BUFFER_SIZE 32                         ///< Number of STCO entries to keep in the memory.
#define STTS_BUFFER_SIZE 17                         ///< Number of STTS entries to keep in the memory.
#define STSS_BUFFER_SIZE 16                         ///< Number of STSS entries to keep in the memory.
#define CTTS_BUFFER_SIZE 17


#define CachePosMask (DEPACK_CACHE_SIZE-1)          ///< cache mask to get the position in the page
#define CachePageMask (~CachePosMask)               ///< cache mask to get the file page number
#if (PSR3GP_SPEED_UP_EN == 1)
#define MAX_DEPACK_CACHE (14)                        ///< Use 4 depack caches
#else
#define MAX_DEPACK_CACHE (4)                        ///< Use 4 depack caches
#endif

#define SUPPORT_3GPP2 0
#define DEBUG_ATOM 0
#define CTTS_ENABLE 1
	
//==============================================================================
//
//                              STRUCTURES
//
//==============================================================================



/** @brief Atom type in 3GP

These atoms are in sample table (stbl)
*/
typedef enum MMPF_3GP_ATOM {
    MMPF_3GP_ATOM_STSZ = 0, ///< Sample size table
    MMPF_3GP_ATOM_STSC,     ///< Sample to chunk table
    MMPF_3GP_ATOM_STCO,     ///< Chunk offset table
    MMPF_3GP_ATOM_STTS,     ///< Time to sample table
    MMPF_3GP_ATOM_STSS,     ///< Sync sample table. Suppose only video has this table
    MMPF_3GP_ATOM_CTTS      ///< Composition Time to Sample table
}MMPF_3GP_ATOM;

/** @brief Track type in 3GP file
*/
typedef enum MMPF_3GP_TRACK {
    MMPF_3GP_TRACK_AUDIO = 0, ///< Audio track
    MMPF_3GP_TRACK_VIDEO,     ///< Video track
    MMPF_3GP_TRACK_HINT,      ///< Hint track
    MMPF_3GP_TRACK_UNDEFINED  ///< Undefined track
}MMPF_3GP_TRACK;


/** @brief Meta information for 3GP file

index 0 for video and 1 for audio
1. moov\\trak (vide)\\mdia\\minf\\stbl and,
2. moov\\trak (soun)\\mdia\\minf\\stbl
*/
#if CTTS_ENABLE
typedef struct VIDEO_ATOM_BUFFER
{
    MMP_UBYTE ubSize[6];                                        ///< Size of the atom buffer
    MMP_ULONG ulIndex[2][6];                                    ///< index
    MMP_ULONG *ulTableAddress[2][6];                            ///< The addresses to the table
    MMP_ULONG ulSizeTable[2][STSZ_BUFFER_SIZE];                 ///< Size table buffer
    MMP_ULONG ulSamplePerChunkTable[2][STSC_BUFFER_SIZE * 3];   ///< Sample per chunk table (3 double words per atom)
    MMP_ULONG ulChunkOffsetTable[2][STCO_BUFFER_SIZE];          ///< Chunk offset table
    MMP_ULONG ulSampleTimeTable[2][STTS_BUFFER_SIZE * 2];       ///< Sample to time table (2 double words per atom)
    MMP_ULONG ulVideoSyncTable[STSS_BUFFER_SIZE];               ///< Sync sample table
    MMP_ULONG ulCompositionTimeTable[CTTS_BUFFER_SIZE * 2];
} VIDEO_ATOM_BUFFER;
#else
typedef struct VIDEO_ATOM_BUFFER
{
    MMP_UBYTE ubSize[5];                                        ///< Size of the atom buffer
    MMP_ULONG ulIndex[2][5];                                    ///< index
    MMP_ULONG *ulTableAddress[2][5];                            ///< The addresses to the table
    MMP_ULONG ulSizeTable[2][STSZ_BUFFER_SIZE];                 ///< Size table buffer
    MMP_ULONG ulSamplePerChunkTable[2][STSC_BUFFER_SIZE * 3];   ///< Sample per chunk table (3 double words per atom)
    MMP_ULONG ulChunkOffsetTable[2][STCO_BUFFER_SIZE];          ///< Chunk offset table
    MMP_ULONG ulSampleTimeTable[2][STTS_BUFFER_SIZE * 2];       ///< Sample to time table (2 double words per atom)
    MMP_ULONG ulVideoSyncTable[STSS_BUFFER_SIZE];               ///< Sync sample table
} VIDEO_ATOM_BUFFER;
#endif
/** @brief Private data of the 3GP parser
*/
typedef struct MMPF_3GPPSR_PRIVATEDATA  {
    MMP_BOOL                bFilling;               ///< If the parser is filling a frame
    MMP_BOOL                bLargeFrame;            ///< If current frame is a large frame
    MMP_BOOL                bLastSeg;               ///< If current frame is the last segmentation of the large frame
    MMP_BOOL                bFillBufferDone;        ///< If the demuxer is finished and the parser status is not updated.
    MMPF_M_BUFFER_HEADER    pPreWriteHeader;       ///< Previous write buffer header
    MMPF_M_BUFFER_HEADER    curWriteHeader;         ///< Current write buffer header
    MMP_USHORT              usEOFPadding;           /**< The padding data after the end of a frame. It might includes ARM padding or
                                                        several 0 bytes for padding */
    MMP_USHORT              usBSOffset;             /**< The offset before the bitstream and the starting address of the header.
                                                    It might include LARGE-DMA SD card 8 byte alignment with 512 bytes offset
                                                    and the header itself.*/
    MMP_LONG                lMovThreshold;          /**< Data move interrupt threshold in bytes. 0 for triggering per frame.*/
    MMP_VID_AUDIOTYPE       supported_audio_format; ///< The audio format supported in current firmware
    MMP_VID_VIDEOTYPE       supported_video_format; ///< The video format supported in current firmware
}MMPF_3GPPSR_PRIVATEDATA;

#if 0
/** @brief The data structure of the mutex

This structure is designed for host and firmware critical session
@note The data in the structure must be byte or word to avoid regester DW accessing syncing isse
*/
typedef struct MMP_MUTEX {
    // use short to avoid alignment issue.
    volatile MMP_SHORT sHostLock;                   ///< Host side lock
    volatile MMP_SHORT sDevLock;                    ///< Firmware side lock
}MMP_MUTEX;
#endif
/** Used as index of 3gpp depack cache structure
*/
typedef enum MMPF_3GPPSR_CACHE_INDEX{
	VIDEO_CACHE = 0,                                ///< video data depack buffer index
	AUDIO_CACHE,                                    ///< audio data depack buffer index
	VIDEO_CHUNK_CACHE,                              ///< video atom depack buffer index
	AUDIO_CHUNK_CACHE,                              ///< audio atom depack buffer index, STSZ
	VIDEO_STSC_CACHE,
	AUDIO_STSC_CACHE,
	VIDEO_STCO_CACHE,
	AUDIO_STCO_CACHE,
	VIDEO_STTS_CACHE,
	AUDIO_STTS_CACHE,
	VIDEO_STSS_CACHE,
	AUDIO_STSS_CACHE,
	VIDEO_CTTS_CACHE,
	AUDIO_CTTS_CACHE
}MMPF_3GPPSR_CACHE_INDEX;


/** Structure of depack buffer information
*/
typedef struct MMPF_3GPPSR_DEPACKCACHE {
	MMP_UBYTE *address;                             ///< Memory address of the depack buffer
	MMP_ULONG64 oldSeekPos;                         ///< The position of the last loaded page in the file
	MMP_ULONG64 preSeekPos;                         ///< The position of the previous access in the file
    void *fileHandle;                               ///< File Handle
}MMPF_3GPPSR_DEPACKCACHE;// DepackCache;


/// @} //File System depack cache 

//==============================================================================
//
//                              VARIABLES
//
//==============================================================================

// name Parser Function

MMPF_PSR_FUNC   m_Parser;
MMP_ULONG       m_ParserType;
MMP_ULONG       m_ulSampleRate;


/** @name 3GP Parser Variables
@{
*/
MMPF_3GPP_SEEKSTATUS            m_seek;	
mp4AudioSpecificConfig          mp4ASC;             ///< Import mp4ASC in audio structure
MMP_UBYTE                       gDemuxOption;
//MMP_ULONG                       gulDemuxParam;
MMP_BOOL                        m_bVideoEOS;
MMP_BOOL                        m_bAudioEOS;

/// @brief Pointer to global 3GP player
#if CTTS_ENABLE
static VIDEO_ATOM_BUFFER        m_3GPAtomBuf = {STSZ_BUFFER_SIZE, 
                                                STSC_BUFFER_SIZE, 
                                                STCO_BUFFER_SIZE,
                                                STTS_BUFFER_SIZE,
                                                STSS_BUFFER_SIZE,
                                                CTTS_BUFFER_SIZE};
#else
static VIDEO_ATOM_BUFFER        m_3GPAtomBuf = {STSZ_BUFFER_SIZE, 
                                                STSC_BUFFER_SIZE, 
                                                STCO_BUFFER_SIZE,
                                                STTS_BUFFER_SIZE,
                                                STSS_BUFFER_SIZE};
#endif
                                                
static MMP_BOOL                 gbCardMode          = MMP_TRUE; ///< Indicate if the playing is playing with card mode
static MMP_ULONG                preAudioOffset      = 0;
static MMP_ULONG                preAudioReduntSize  = 0;
static MMP_USHORT               m_sOffsetInAtom     = 0;///< Offset in the atom. STSC and STTS has 3 or 2 DWs per atom
static MMP_ULONG                cacheIndex;             ///< Current cache index. Indicates that which depack buffer is using.
static MMPF_3GPPSR_DEPACKCACHE  m_depack_cache[MAX_DEPACK_CACHE];///< The 4 AV depack buffers
static MMP_ULONG                *OldSeekPos;        ///< The position of the last loaded page in the file of current depack cache
static MMP_3GP_SAMPLE_INFO      gAudioSample;
static MMP_3GP_SAMPLE_INFO      gVideoSample;
MMP_3GP_MEDIA_INFO              gAudioMedia;        ///< Audio media information. Most 3GP information stores here.
static MMP_3GP_MEDIA_INFO       gVideoMedia;        ///< Video media information. Most 3GP information stores here.
static MMP_ULONG                glAVDepackBufAddr;  ///< address for AV depack buffer
static MMPF_3GPPSR_PRIVATEDATA  m_3GPPdata;
MMP_VID_INFO                    gVidFileInfo;       ///< Information of the 3GP file
static MMP_ULONG                ulCurOffset; 
static MMP_UBYTE                m_ASCInfo[0x30];    ///< optional AAC information in 3GP
static MMP_ULONG                m_bsbuf_type;
static MMP_BOOL                 m_support_large_frame;
static MMP_UBYTE                *m_VideoStreamBuf;     ///< current video pointer(parser)
static MMP_ULONG                m_VideoStreamLen;
static MMP_ULONG                m_AudioPtr;

//static MMP_ULONG                gl3gpAudioSize;

extern MMP_ULONG                glAudioPlayReadPtr;      ///< The audio read position in the buffer
extern MMP_ULONG                glAudioPlayWritePtr;     ///< The audio write position in the buffer
extern MMP_ULONG                glAudioPlayReadPtrHigh;  ///< read loop counter
extern MMP_ULONG                glAudioPlayWritePtrHigh; ///< write loop counter
#if (OMA_DRM_EN == 1)
extern MMP_ULONG 				m_glDRMIV;
#if (PSR3GP_SPEED_UP_EN == 1)
extern MMP_ULONG 				m_ulDRMIVAddr[14];
#else
extern MMP_ULONG 				m_ulDRMIVAddr[4];
#endif
#endif
MMP_BOOL                        m_bs_support_large_frame;
MMP_BOOL                        m_bLargeframe_wrap;
MMPF_M_BUFFER_HEADER            *m_PrevBSHeader;

MMP_UBYTE                       *m_pHandShakeBuf;

/*
    ADDR + 0  : m_mutex.sHostLock
    ADDR + 2  : m_mutex.sDevLock
    ADDR + 4  : m_BSHeader.buftype
    ADDR + 8  : m_BSHeader.length
    ADDR + 12 : m_BSHeader.buf
    ADDR + 16 : m_BSHeader.bufend
    ADDR + 20 : m_BSHeader.ringData.ptr[0]
    ADDR + 24 : m_BSHeader.ringData.ptr[1]
    ADDR + 28 : m_BSHeader.ringData.lap[0]
    ADDR + 32 : m_BSHeader.ringData.lap[1]
    ADDR + 36 : m_BSHeader.ringData.Atoms
    ADDR + 40 : m_BuffedAudioAtoms
    ADDR + 44 : m_DecodedAudioAtoms
*/
#define HOST_LOCK_OFFSET        0
#define DEV_LOCK_OFFSET         2
#define BUF_TYPE_OFFSET         4
#define BUF_LENGTH_OFFSET       8
#define BUF_START_OFFSET        12
#define BUF_END_OFFSET          16
#define READ_PTR_OFFSET         20
#define WRITE_PTR_OFFSET        24
#define READ_LAP_OFFSET         28
#define WRITE_LAP_OFFSET        32
#define VIDEO_ATOMS_OFFSET      36
#define BUF_AUDIO_ATOM_OFFSET   40
#define DEC_AUDIO_ATOM_OFFSET   44
#define AUDIO_READ_PTR_OFFSET   48
#define AUDIO_WRITE_PTR_OFFSET  52
#define AUDIO_READ_HIGH_OFFSET  56
#define AUDIO_WRITE_HIGH_OFFSET 60


#define BS_BUF_TYPE             m_pHandShakeBuf[BUF_TYPE_OFFSET]


MMP_MUTEX               m_mutex;
MMPF_M_BITSTREAM_HEADER *m_pBSHeader;// = (MMPF_M_BITSTREAM_HEADER*)(&m_BSHeader);
MMP_ULONG               m_BuffedAudioAtoms;
MMP_ULONG               m_DecodedAudioAtoms;



#define WRITE_INDEX m_pBSHeader->ringIndex.ptr[RING_WRITE]
#define READ_INDEX  m_pBSHeader->ringIndex.ptr[RING_READ]
#define WRITE_DATA  m_pBSHeader->ringData.ptr[RING_WRITE]
#define READ_DATA   m_pBSHeader->ringData.ptr[RING_READ]
#define WRITE_LAP   m_pBSHeader->ringData.lap[RING_WRITE]
#define READ_LAP    m_pBSHeader->ringData.lap[RING_READ]


#define WRITE_HEADER (&(m_3GPPdata.curWriteHeader))

    
    
/// @} //3GP Parser Variables

//==============================================================================
//                     LOCAL FUNCTION PROTOTYPES
//==============================================================================
// AVI FileSystem
//MMP_ERR MMPF_AVIPSR_FOpen(MMP_ULONG *ulfp);
//MMP_ERR MMPF_AVIPSR_FSeek(MMP_ULONG fptr, MMP_ULONG offset, MMP_LONG lOrigin);
//MMP_ERR MMPF_VIDPSR_ReadFile(MMP_UBYTE *buf, const MMP_ULONG curSeek, MMP_ULONG size, MMP_ULONG padding_size);
//MMP_ERR MMPF_AVIPSR_FTell(MMP_ULONG fptr, MMP_LONG *lPos);

static  MMP_ERR     MMPF_VIDPSR_ReadFile(MMP_UBYTE *ptr, const MMP_ULONG SeekPos, const MMP_ULONG size,MMP_ULONG padding_size);
// parser
static  MMP_ERR     MMPF_3GPPSR_SeekIFrame(const MMP_SHORT nAdvance, const MMP_ULONG curFrame,MMP_ULONG *RequestFrame);
static  MMP_ERR     MMPF_3GPPSR_GetNextSample(MMP_3GP_MEDIA_INFO *MediaInfo, MMP_3GP_SAMPLE_INFO *pOffsetSizeTime);
static  MMP_ERR     MMPF_3GPPSR_SeekSample(const MMPF_3GP_TRACK track, const MMP_ULONG ulTarget, const MMP_BOOL bByTime);

//static  MMP_ERR     MMPF_BS_SyncRingBufPtr(const RING_BUFFERACCESS_TYPE accessType);
static  MMP_ULONG   MMPF_3GPPSR_GetTime(const MMP_3GP_MEDIA_INFO *const pMedia);

/// Extract the meaning field of the err
#define ERROR_MEANING(err) (err & 0xFFFF)

MMP_ERR             MMPF_Player_GetSupportedFormat(MMP_VID_AUDIOTYPE *supported_audio_format,
                                                   MMP_VID_VIDEOTYPE *supported_video_format);

MMP_ERR MMPF_BS_LockMutex(const MMP_ULONG timeout);
MMP_ERR MMPF_BS_UnlockMutex(void);
MMP_ERR MMPF_BS_UpdateVideoBufPtr(const RING_BUFFERACCESS_TYPE accessType, const MMP_ULONG ulSize);
MMP_ERR MMPF_BS_TestVideoBufPtr(const RING_BUFFERACCESS_TYPE accessType, const MMP_ULONG ulSize, MMP_USHORT *nWrap);
void    MMPF_BS_ResetBuffer(void);
MMP_ERR MMPF_BS_AccessVideoHeader(MMP_UBYTE* pRingBuf, MMP_UBYTE* pPlainBuf, MMP_ULONG ulSize, RING_BUFFERACCESS_TYPE access_type);
MMP_ERR MMPF_BS_GetVideoStartAddr(MMP_ULONG *header_addr, MMP_ULONG *frame_addr,MMP_ULONG *avail_size,MMP_ULONG *avail_size1,
                                  MMP_ULONG header_size,  MMP_ULONG ulSize);
MMP_ERR MMPF_BS_GetVideoData(MMP_ULONG *frame_addr, MMP_ULONG *frame_size, MMP_ULONG *data_size,MMP_ULONG *flag, MMP_ULONG64 *time);
MMP_ERR MMPF_BS_GetAudioStartAddr(MMP_ULONG *addr0,MMP_ULONG *addr1,MMP_LONG *avail_size,MMP_LONG *avail_size1);
MMP_ERR MMPF_BS_UpdateAudioBufptr(MMP_ULONG size,MMP_BOOL bWrap);


//MMP_ERR MMPF_AVIPSR_SeekNextVideoFrame(MMP_ULONG *RequestFrame);
//MMP_ULONG MMPF_AVIPSR_GetTime(MMP_AVI_MEDIA_INFO *pMedia);




//==============================================================================
//
//                              FUNCTIONS
//
//==============================================================================
#if 0 // for IDE function name list by section
void _____BITSTREAM_BUFFER_FUNCTION_____(){}
#endif

void MMPF_BS_ConnectVideoBuf(MMP_ULONG ulVideoInAddr, const MMP_ULONG ulLength,MMP_ULONG buffer_type){
    m_pBSHeader->buf          = (MMP_UBYTE*)ulVideoInAddr;
    m_pBSHeader->length       = ulLength;
    m_pBSHeader->bufend       = m_pBSHeader->buf + m_pBSHeader->length;
    m_pBSHeader->buftype      = buffer_type;
    
    if (m_bsbuf_type == MMP_MEDIA_NEAR_RING_BUFFER ) {
        m_bs_support_large_frame = 0;
    }
    else if (m_bsbuf_type == MMP_MEDIA_RING_BUFFER ) {
        m_bs_support_large_frame = 1;
    }
}

//#define SELF_CS (0) // self-maintained critical session
//96 bytes
MMP_ERR MMPF_BS_UpdateVideoBufPtr(const RING_BUFFERACCESS_TYPE accessType, const MMP_ULONG ulSize)
{
    MMPF_M_RINGBUFFER_PTR   *pRing  = &m_pBSHeader->ringData;
    MMP_ULONG               buf_length  = m_pBSHeader->length;
    MMP_ULONG               sizeToWrite;
    
    #if ( (BUFFER_TYPE == MMP_MEDIA_NEAR_RING_BUFFER) || (SUPPORT_TWO_BSBUF_TYPE == 1) )
    MMPF_M_BUFFER_HEADER    *read_header;
    MMP_BOOL                needWrap = MMP_FALSE;
    
    read_header = (MMPF_M_BUFFER_HEADER*)(m_pBSHeader->buf + READ_DATA);
    #endif
    
    MMPF_BS_LockMutex(0);
    
    if ( accessType == RING_READ ) {
        pRing->Atoms--;
    }
    
    //modify this!
    if(!m_bLargeframe_wrap)
        MMPF_BS_SyncRingBufPtr(RING_READ);
    
    // special made for the access that size == 1, it assumed to be index accessing
    // The index accessing should wait for next frame done
   sizeToWrite = ulSize * (accessType == RING_WRITE);
    
    #if (BUFFER_TYPE == MMP_MEDIA_NEAR_RING_BUFFER) || (SUPPORT_TWO_BSBUF_TYPE == 1)
        if ( m_pBSHeader->buftype == MMP_MEDIA_NEAR_RING_BUFFER ) {
            if ((pRing->ptr[RING_READ] == pRing->ptr[RING_WRITE]) && 
                (pRing->ptr[RING_WRITE] + sizeToWrite > buf_length)) {
                    // empty and write with wrapping
                    pRing->lap[RING_READ] ++;
                    pRing->ptr[RING_READ] = 0;
                    pRing->lap[RING_WRITE] ++;
                    pRing->ptr[RING_WRITE] = 0;
            }
        }
    #endif
    
    if (ulSize + pRing->ptr[accessType] >= buf_length) {

        #if SUPPORT_TWO_BSBUF_TYPE == 1
            if (  m_pBSHeader->buftype == MMP_MEDIA_NEAR_RING_BUFFER ) {
                //if (pRing->ptr[1-accessType] + (ulSize == 1) > (ulSize - 1))
                {
                    if (ulSize + pRing->ptr[accessType] > buf_length) {
                        pRing->ptr[accessType] = ulSize;
                    }
                    else {
                        pRing->ptr[accessType] = 0;
                    }
                }
            }
            else if (  m_pBSHeader->buftype == MMP_MEDIA_RING_BUFFER ) {
                //if (ulSize + pRing->ptr[accessType] - buf_length <= (pRing->ptr[1 - accessType] + (ulSize == 1)))
                //if ( (pRing->ptr[1 - accessType] + (ulSize == 1)) >= (ulSize + pRing->ptr[accessType] - buf_length) )
                {
                    pRing->ptr[accessType] = pRing->ptr[accessType] + ulSize - buf_length;
                    
                    //return MMP_ERR_NONE;
                }
            }
            pRing->lap[accessType] ++;
            goto exit_ur;
            
        #else
            #if BUFFER_TYPE == MMP_MEDIA_NEAR_RING_BUFFER
            //if (pRing->ptr[1-accessType] + (ulSize == 1) > (ulSize - 1))
            {
                if (ulSize + pRing->ptr[accessType] > buf_length) {
                    pRing->ptr[accessType] = ulSize;
                }
                else {
                    pRing->ptr[accessType] = 0;
                }
                pRing->lap[accessType] ++;
                goto exit_ur;
            }
            #endif
            #if BUFFER_TYPE == MMP_MEDIA_RING_BUFFER
            //if (ulSize + pRing->ptr[accessType] - buf_length <= (pRing->ptr[1 - accessType] + (ulSize == 1)))
            {
                pRing->ptr[accessType] = pRing->ptr[accessType] + ulSize - buf_length;
                pRing->lap[accessType] ++;
                goto exit_ur;
                //return MMP_ERR_NONE;
            }
            #endif
        #endif
    }
    
    pRing->ptr[accessType] += ulSize;
    
exit_ur:
    if ( accessType == RING_READ ) {
        #if ( (BUFFER_TYPE == MMP_MEDIA_NEAR_RING_BUFFER) || (SUPPORT_TWO_BSBUF_TYPE == 1) )
            if (  m_pBSHeader->buftype == MMP_MEDIA_NEAR_RING_BUFFER) {
                needWrap = (read_header->ulFlag & BUFFERFLAG_NEED_WRAP) ? MMP_TRUE : MMP_FALSE;
                
                if (needWrap)
                {
                    READ_LAP ++;
                    READ_DATA = 0;
                    read_header->ulFlag &= ~BUFFERFLAG_NEED_WRAP;                
                }
            }
        #endif
    }
    if ( accessType == RING_WRITE ) {
        pRing->Atoms++;
    }
    //MMPF_BS_LockMutex(0);
    MMPF_BS_SyncRingBufPtr(RING_WRITE);
    MMPF_BS_UnlockMutex();
    return MMP_ERR_NONE;
}

// 150 bytes
MMP_ERR MMPF_BS_TestVideoBufPtr(const RING_BUFFERACCESS_TYPE accessType, const MMP_ULONG ulSize, MMP_USHORT *nWrap)
{
    //#define RETURN_TR(x) error = x; goto exit_tr
    MMPF_M_RINGBUFFER_PTR   *pRing  = &m_pBSHeader->ringData;
    MMP_ULONG               buf_length = m_pBSHeader->length;
    MMP_ULONG               sizeToRead, sizeToWrite;
    MMP_ERR                 error = MMP_ERR_NONE;
    MMP_BOOL                bOverWrite = MMP_FALSE;
    
    MMPF_BS_LockMutex(0);
    
    MMPF_BS_SyncRingBufPtr(RING_READ);
    
    // special made for the access that size == 1, it assumed to be index accessing
    // The index accessing should wait for next frame done
    sizeToRead  = ulSize * (accessType == RING_READ);
    sizeToWrite = ulSize * (accessType == RING_WRITE);
    *nWrap = 0;

    if (ulSize > buf_length) {
        error = MMP_VIDPSR_ERR_INSUFFICIENT_RESOURCES;
        goto exit_tr;
    }

    if ((pRing->ptr[RING_WRITE] + sizeToWrite) > (pRing->ptr[RING_READ]) + sizeToRead) {
        if (pRing->lap[RING_READ] < (pRing->lap[RING_WRITE])) {
	        // full
	        error = MMP_VIDPSR_ERR_OVERFLOW;
            goto exit_tr;
        }
    }
    if ((pRing->ptr[RING_READ] + sizeToRead)  > (pRing->ptr[RING_WRITE] + sizeToWrite)) {
        // overflow or underflow
        if (pRing->lap[RING_READ] == (pRing->lap[RING_WRITE])) {
        // the same lap, underflow
            error = MMP_VIDPSR_ERR_UNDERFLOW;
            goto exit_tr;
        }
    }

    #if ( SUPPORT_TWO_BSBUF_TYPE == 1 ) || (BUFFER_TYPE == MMP_MEDIA_NEAR_RING_BUFFER)
        if ( m_pBSHeader->buftype == MMP_MEDIA_NEAR_RING_BUFFER) {
            if ((pRing->ptr[RING_READ] == pRing->ptr[RING_WRITE]) && 
                (pRing->ptr[RING_WRITE] + sizeToWrite > buf_length)) {
                *nWrap = 2;
                error = MMP_ERR_NONE;
                goto exit_tr;
            }
        }
    #endif

    #if ( SUPPORT_TWO_BSBUF_TYPE == 1 )
        if (  m_pBSHeader->buftype == MMP_MEDIA_NEAR_RING_BUFFER ) {
            if (pRing->ptr[1-accessType] < (ulSize))
                bOverWrite = MMP_TRUE;
        }
        else if (  m_pBSHeader->buftype == MMP_MEDIA_RING_BUFFER ) {
            if ((pRing->ptr[1 - accessType] < (ulSize + pRing->ptr[accessType] - buf_length)))
                bOverWrite = MMP_TRUE;
        }
    #else
        #if ( BUFFER_TYPE == MMP_MEDIA_NEAR_RING_BUFFER ) 
            if (pRing->ptr[1-accessType] < (ulSize))
                bOverWrite = MMP_TRUE;
        #endif
        #if ( BUFFER_TYPE == MMP_MEDIA_RING_BUFFER ) 
            if ((pRing->ptr[1 - accessType] < (ulSize + pRing->ptr[accessType] - buf_length)))
                bOverWrite = MMP_TRUE;
        #endif
        
    #endif
    
    if (ulSize + pRing->ptr[accessType] >= buf_length) {
        if( !bOverWrite ) {
            *nWrap = 1;
             error = MMP_ERR_NONE;
        }
        else if (accessType == RING_WRITE) {
            *nWrap = 1;
            error = MMP_VIDPSR_ERR_OVERFLOW;
        }
        else {
            error = MMP_VIDPSR_ERR_UNDERFLOW;
        }
    }
    
exit_tr:
    MMPF_BS_UnlockMutex();
    return error;
}

// 64 bytes
void MMPF_BS_ResetBuffer(void)
{
    // clear data pointer
    MEMSET0(&m_pBSHeader->ringData);
    
    
    glAudioPlayReadPtr      = 0;
    glAudioPlayWritePtr     = 0;
    glAudioPlayReadPtrHigh  = 0;
    glAudioPlayWritePtrHigh = 0;
    
    m_BuffedAudioAtoms    = 0;
    m_DecodedAudioAtoms   = 0;
}

// 64 bytes
void MMPF_BS_ResetAudioState(void)
{
    glAudioPlayReadPtr      = 0;
    glAudioPlayWritePtr     = 0;
    glAudioPlayReadPtrHigh  = 0;
    glAudioPlayWritePtrHigh = 0;
    
    m_BuffedAudioAtoms    = 0;
    m_DecodedAudioAtoms   = 0;
}
/** @brief Read data from the ring buffer or write the data to the ring buffer

@param[in] pRingBuf The pointer of the ring buffer structure. It should be within the ring buffer boundary.
                    But this function does not check it.
@param[in] pPlainBuf The plain data to be read or write
@param[in] ulSize The size of the data
@param[in] access_type RING_READ to read the data from the ring buffer, and RING_WRITE to write data to the
           ring buffer.
@return Always return success due to no checking the boundary
*/
MMP_ERR MMPF_BS_AccessVideoHeader(MMP_UBYTE* pRingBuf, MMP_UBYTE* pPlainBuf, MMP_ULONG ulSize,RING_BUFFERACCESS_TYPE access_type)
{
//    MMP_ULONG ulSize = sizeof(MMPF_M_BUFFER_HEADER);
    
#if (SUPPORT_TWO_BSBUF_TYPE == 1)
    if (  m_pBSHeader->buftype == MMP_MEDIA_NEAR_RING_BUFFER ) {
        if (access_type == RING_READ) {
            MEMCPY(pPlainBuf,pRingBuf,ulSize);
        }
        else {
            MMPF_BS_UpdatePrevWriteHeader(m_PrevBSHeader);
            m_PrevBSHeader = (MMPF_M_BUFFER_HEADER*)pRingBuf;
            MEMCPY(pRingBuf,pPlainBuf,ulSize);
        }
    }
    else {
        MMP_LONG lSecondSeg, lFirstSeg;
        MMP_UBYTE* pSrc;
        MMP_UBYTE* pDest;
        
        if (access_type == RING_READ) {
            pSrc = pRingBuf;
            pDest = pPlainBuf;
        }
        else {
            pSrc = pPlainBuf;
            pDest = pRingBuf;
        }

        lSecondSeg = pRingBuf + ulSize - m_pBSHeader->bufend;
        lFirstSeg = ulSize;
        if (lSecondSeg > 0) {
            lFirstSeg = ulSize - lSecondSeg;
            if (access_type == RING_READ) {
                MEMCPY(pDest + lFirstSeg, m_pBSHeader->buf, lSecondSeg);
            }
            else {
                MEMCPY(m_pBSHeader->buf, pSrc + lFirstSeg, lSecondSeg);
            }
        }
        MEMCPY(pDest, pSrc, lFirstSeg);
    }
#else

    #if BUFFER_TYPE == MMP_MEDIA_NEAR_RING_BUFFER
        if (access_type == RING_READ) {
            MEMCPY(pPlainBuf,pRingBuf,ulSize);
        }
        else {
            MMPF_BS_UpdatePrevWriteHeader(m_PrevBSHeader);
            m_PrevBSHeader = (MMPF_M_BUFFER_HEADER*)pRingBuf;
            MEMCPY(pRingBuf,pPlainBuf,ulSize);
        }
    #else
        MMP_LONG lSecondSeg, lFirstSeg;
        MMP_UBYTE* pSrc;
        MMP_UBYTE* pDest;
        
        if (access_type == RING_READ) {
            pSrc = pRingBuf;
            pDest = pPlainBuf;
        }
        else {
            pSrc = pPlainBuf;
            pDest = pRingBuf;
        }

        lSecondSeg = pRingBuf + ulSize - m_pBSHeader->bufend;
        lFirstSeg = ulSize;
        if (lSecondSeg > 0) {
            lFirstSeg = ulSize - lSecondSeg;
            if (access_type == RING_READ) {
                MEMCPY(pDest + lFirstSeg, m_pBSHeader->buf, lSecondSeg);
            }
            else {
                MEMCPY(m_pBSHeader->buf, pSrc + lFirstSeg, lSecondSeg);
            }
        }
        MEMCPY(pDest, pSrc, lFirstSeg);
    #endif
#endif

    return MMP_ERR_NONE;
}

MMP_ERR MMPF_BS_GetVideoStartAddr(  MMP_ULONG *header_addr, MMP_ULONG *frame_addr,
                                    MMP_ULONG *avail_size,MMP_ULONG *avail_size1,
                                    MMP_ULONG header_size,  MMP_ULONG ulSize)
{
    MMP_ERR                 error;
    MMP_USHORT              nWrap;
    
    error =  MMPF_BS_TestVideoBufPtr(RING_WRITE, 
                                    ulSize,
                                    &nWrap);
    
    
    #if (SUPPORT_TWO_BSBUF_TYPE == 1) || (BUFFER_TYPE == MMP_MEDIA_NEAR_RING_BUFFER)
        if ( m_pBSHeader->buftype == MMP_MEDIA_NEAR_RING_BUFFER) {
            if (nWrap >= 1) {
                m_PrevBSHeader->ulFlag |= BUFFERFLAG_NEED_WRAP;
                
                MMPF_BS_LockMutex(0);
                WRITE_DATA = 0;
                WRITE_LAP ++;
                MMPF_BS_UnlockMutex();
            }
        }
    #endif
    m_bLargeframe_wrap = MMP_FALSE;
    
    if (error == MMP_VIDPSR_ERR_INSUFFICIENT_RESOURCES) {
        if (m_bs_support_large_frame) {
            if (MMPF_BS_TestVideoBufPtr(RING_WRITE,  m_pBSHeader->length, &nWrap) != MMP_ERR_NONE) {
                return MMP_VIDPSR_ERR_OVERFLOW;
            }
            MMPF_BS_LockMutex(0);
            READ_DATA               = 
            WRITE_DATA              = 0;
            MMPF_BS_UnlockMutex();
            m_bLargeframe_wrap      = MMP_TRUE;
        }
    }
    
    *header_addr = (MMP_ULONG)(m_pBSHeader->buf + WRITE_DATA);
    
    *frame_addr = *header_addr + header_size;
    
    #if (SUPPORT_TWO_BSBUF_TYPE ==1) || (BUFFER_TYPE == MMP_MEDIA_RING_BUFFER)
        if (  m_pBSHeader->buftype == MMP_MEDIA_RING_BUFFER )
        {
            if (*frame_addr >= (MMP_ULONG)m_pBSHeader->bufend) {
                *frame_addr -= m_pBSHeader->length;
            }
        }
    #endif
    
    *avail_size     = (MMP_ULONG)(m_pBSHeader->bufend) - *header_addr;
    *avail_size1    = READ_DATA;
    
    if (  m_pBSHeader->buftype == MMP_MEDIA_NEAR_RING_BUFFER )    
        *avail_size1 = 0;
        
    return error;
}

MMP_ERR MMPF_BS_GetVideoData(MMP_ULONG *frame_addr, MMP_ULONG *frame_size, MMP_ULONG *data_size,MMP_ULONG *flag, MMP_ULONG64 *time)
{
    MMPF_M_BUFFER_HEADER    header;
    
    if (m_pBSHeader->ringData.Atoms == 0) {
        return MMP_VIDPSR_ERR_UNDERFLOW;
    }
        
    MMPF_BS_AccessVideoHeader(m_pBSHeader->buf + READ_DATA, (MMP_UBYTE*)(&header), sizeof(MMPF_M_BUFFER_HEADER),RING_READ);
    
    *frame_addr = header.ulAddress;
    *frame_size = header.ulBSSize;
    *data_size  = header.ulSize;
    *time       = header.ulRequestTime;
    *flag       = header.ulFlag;
    
    
    return MMP_ERR_NONE;
}

MMP_ERR MMPF_BS_GetAudioStartAddr(MMP_ULONG *addr0,MMP_ULONG *addr1,MMP_LONG *avail_size,MMP_LONG *avail_size1)
{
    MMPD_BS_GetAudioReadPtr();

    if (glAudioPlayReadPtr == glAudioPlayWritePtr) {
        if (glAudioPlayReadPtrHigh == glAudioPlayWritePtrHigh) {// buffer is empty
            *avail_size = glAudioPlayBufSize;// read full buffer
        }
        else {
            if ((glAudioPlayWritePtrHigh - glAudioPlayReadPtrHigh) != 1) {// error, shouldn't have gone here
                return  MMP_VIDPSR_ERR_UNDEFINED;
            }
            else { // buffer is full
                return MMP_VIDPSR_ERR_INSUFFICIENT_RESOURCES;
            }
        }
    }
    else {// able to read
        if (glAudioPlayReadPtr > glAudioPlayWritePtr) { // plain read
            *avail_size = glAudioPlayReadPtr - glAudioPlayWritePtr;
        }
        else { // rounded read
            *avail_size = glAudioPlayBufSize - (glAudioPlayWritePtr - glAudioPlayReadPtr);
        }
    }
    // Note that 0x200 is currently hard coded, half of audio buffer
    if (*avail_size < 0x200) { // It's too small so that it doesn't load. Wait for reading.
        return  MMP_VIDPSR_ERR_OVERFLOW;
    }
/*
    if (glAudioSizeRemain > 0x200) {// empty size > 512 bytes
        // multiple of 200. if 0x199<size<0x400, size = 0x200, 0x400 = 0x400
        *avail_size &= ~(0x200 - 1);// > multiple of 200. if 0x199<size<0x400, size = 0x200, 0x400 = 0x400
    }
    else {
        *avail_size = glAudioSizeRemain; // <= 512 bytes part
    }
*/    
    *avail_size &= ~(0x200 - 1);// > multiple of 200. if 0x199<size<0x400, size = 0x200, 0x400 = 0x400
    
	// cur_buf_to_put_size should be 0 ~ 200
    if (*avail_size == 0) {
        return  MMP_VIDPSR_ERR_OVERFLOW;
    }
    
    *addr0          = glAudioPlayBufStart + glAudioPlayWritePtr;
    *avail_size1    = -1;
    
    if ((glAudioPlayBufSize - glAudioPlayWritePtr) > *avail_size) { //doesn't need to rewind, plain read
    }
    else {
        *addr1          = glAudioPlayBufStart;
        *avail_size1    = *avail_size - (glAudioPlayBufSize - glAudioPlayWritePtr);
        *avail_size     = glAudioPlayBufSize - glAudioPlayWritePtr;
    }
    
    return  MMP_ERR_NONE;
}
MMP_ERR MMPF_BS_UpdateAudioBufptr(MMP_ULONG size,MMP_BOOL bWrap)
{
    glAudioPlayWritePtr += size;
    if ( bWrap ) {
        glAudioPlayWritePtr     = 0;
        glAudioPlayWritePtrHigh += 1;
    }
    
    return MMP_ERR_NONE;
}
MMP_ERR  MMPF_BS_SetAudioPlayReadPtr(void)
{
    MEMCPY(m_pHandShakeBuf+AUDIO_READ_PTR_OFFSET,(MMP_UBYTE*)(&glAudioPlayReadPtr),sizeof(glAudioPlayReadPtr));
    MEMCPY(m_pHandShakeBuf+AUDIO_READ_HIGH_OFFSET,(MMP_UBYTE*)(&glAudioPlayReadPtrHigh),sizeof(glAudioPlayReadPtrHigh));

    return MMP_ERR_NONE;
}

MMP_ERR  MMPF_BS_GetAudioPlayWritePtr(void)
{
    MEMCPY((MMP_UBYTE*)(&glAudioPlayWritePtr),m_pHandShakeBuf+AUDIO_WRITE_PTR_OFFSET,sizeof(glAudioPlayWritePtr));
    MEMCPY((MMP_UBYTE*)(&glAudioPlayWritePtrHigh),m_pHandShakeBuf+AUDIO_WRITE_HIGH_OFFSET,sizeof(glAudioPlayWritePtrHigh));

    return MMP_ERR_NONE;
}

MMP_ERR  MMPF_BS_GetAudioAtomNum(MMP_ULONG *atom)
{
    MMP_ULONG temp = m_BuffedAudioAtoms;
    
    if (!gbCardMode) {
        MEMCPY((MMP_UBYTE*)(&m_BuffedAudioAtoms),m_pHandShakeBuf+BUF_AUDIO_ATOM_OFFSET,sizeof(m_BuffedAudioAtoms));
        
        if(m_BuffedAudioAtoms <= temp) {
            MEMCPY((MMP_UBYTE*)(&m_BuffedAudioAtoms),m_pHandShakeBuf+BUF_AUDIO_ATOM_OFFSET,sizeof(m_BuffedAudioAtoms));
        }
    }
    
    *atom = m_BuffedAudioAtoms;
    
    return MMP_ERR_NONE;
}

#if 0 // for IDE function name list by section
void _____Source_____(){}
#endif

/** @brief Cache information for video bitstream atom: "mdat", type "vide"

@param[in] index Which cache the video player uses.
*/
static void MMPF_VIDPSR_SwapDepackCacheTo(const MMP_ULONG index)
{
	cacheIndex = index;
    OldSeekPos = &(m_depack_cache[index].oldSeekPos);
#if (OMA_DRM_EN == 1)
	m_glDRMIV = m_ulDRMIVAddr[cacheIndex];
#endif
}

/** @brief Reset the AV depack cache information
*/
void MMPF_VIDPSR_ResetCache()
{
    MMP_ULONG   i;
    MMP_USHORT  j;
    
	// cache size not set to DEPACK_CACHE_SIZE bytes
	for (i = 0; i < MAX_DEPACK_CACHE; i++) {
		m_depack_cache[i].oldSeekPos    = 0xFFFFFFFF;
		m_depack_cache[i].preSeekPos    = 0;
		m_depack_cache[i].address       = (MMP_UBYTE *) (glAVDepackBufAddr + DEPACK_CACHE_SIZE * i);
	}

#if 0
    m_3GPAtomBuf.ubSize[MMPF_3GP_ATOM_STSZ] = STSZ_BUFFER_SIZE;
    m_3GPAtomBuf.ubSize[MMPF_3GP_ATOM_STSC] = STSC_BUFFER_SIZE;
    m_3GPAtomBuf.ubSize[MMPF_3GP_ATOM_STCO] = STCO_BUFFER_SIZE;
    m_3GPAtomBuf.ubSize[MMPF_3GP_ATOM_STTS] = STTS_BUFFER_SIZE;
    m_3GPAtomBuf.ubSize[MMPF_3GP_ATOM_STSS] = STSS_BUFFER_SIZE;
#endif

    if(m_ParserType == MMP_PSR_3GPPARSER) {
        for (i = 0; i < 2; i++) { // 0 for Video, 1 for Audio
            
            m_3GPAtomBuf.ulTableAddress[i][MMPF_3GP_ATOM_STSZ] = m_3GPAtomBuf.ulSizeTable[i];
            m_3GPAtomBuf.ulTableAddress[i][MMPF_3GP_ATOM_STSC] = m_3GPAtomBuf.ulSamplePerChunkTable[i];
            m_3GPAtomBuf.ulTableAddress[i][MMPF_3GP_ATOM_STCO] = m_3GPAtomBuf.ulChunkOffsetTable[i];
            m_3GPAtomBuf.ulTableAddress[i][MMPF_3GP_ATOM_STTS] = m_3GPAtomBuf.ulSampleTimeTable[i];
            m_3GPAtomBuf.ulTableAddress[i][MMPF_3GP_ATOM_STSS] = m_3GPAtomBuf.ulVideoSyncTable;
            
            #if CTTS_ENABLE
            m_3GPAtomBuf.ulTableAddress[i][MMPF_3GP_ATOM_CTTS] = m_3GPAtomBuf.ulCompositionTimeTable;
            for (j = 0; j < 6; j++) {
                m_3GPAtomBuf.ulIndex[i][j] = 0x80000000;
            }
            #else
            for (j = 0; j < 5; j++) {
                m_3GPAtomBuf.ulIndex[i][j] = 0x80000000;
            }
            #endif
        }
        
        MMPF_VIDPSR_SwapDepackCacheTo(VIDEO_CACHE);
    }
}

/** @brief Read the data from the depack cache

Abstract Layer for 3gp player to access audio/video/meta data
@param[in] SeekPos The offset in the video file.
@param[in] size The size to be read.
@param[out] target The target address to put the data.
*/
void MMPF_VIDPSR_ReadFromCache(const MMP_ULONG SeekPos, MMP_ULONG size, MMP_UBYTE *target)
{
#if 0
    MMP_ULONG i;
    MMP_UBYTE *source;
    i = SeekPos & CachePosMask;
   	source = m_depack_cache[cacheIndex].address + i;
    while (size > 0) {
        *target++ = *source++;
        size--;
    }
#else
    MEMCPY(target, m_depack_cache[cacheIndex].address + (SeekPos & CachePosMask), size);
#endif
}

/** @brief Set an integer to the audio buffer

@note For code size, there is no range checking for these parameters.
*/
void MMPF_3GPPSR_SetAudioBEInt(const MMP_USHORT usDigits, MMP_ULONG ulData, MMP_UBYTE *ptr)
{
    MMP_USHORT i;
    for ( i = 0; i < usDigits; i++) {
        ptr[i] = ulData & 0xFF;
        ulData >>= 4;
    }
}



/** @brief Load audio data according the the 3GP atoms

Basically it loads one audio frame. But some times if it reaches the limited size, 
it loads partial data and require call twice to load one audio frame.
@param[in] dataptr The address to be loaded
@param[in] Size The size to be loaded
@param[out] LoadedSize The actual loaded size
@return The status of the function call
*/ // using audio frame header uses extra 140 bytes of code
MMP_ERR MMPF_3GPPSR_LoadAudioData(MMP_UBYTE *dataptr, MMP_ULONG Size, MMP_ULONG *LoadedSize)
{
    typedef struct TEMP_AUDIO_HEADER {
        MMP_ULONG ulTime;
        MMP_ULONG ulSize;
        MMP_USHORT ulFlag;
        MMP_USHORT ulFlag2;
    }TEMP_AUDIO_HEADER;
    
    MMP_ULONG           LoadSize;
    MMP_3GP_SAMPLE_INFO tmpAtom;
    MMP_USHORT          usHeaderSizeToWrite = 0;
    MMP_ERR             error = MMP_ERR_NONE;
    
    static MMP_USHORT usHeadherReduntSize = 0;//How many bytes left last time
    static TEMP_AUDIO_HEADER curHeader;
    #define SIZEOF_AUDIO_HEADER (12)

    
    if ( (preAudioReduntSize == 0) && (m_bAudioEOS) ) {
        *LoadedSize = 0;
        return MMP_ERR_NONE;
    }
    
    LoadSize = 0;

    if (preAudioReduntSize == 0) {
        /// Change for 3gp atom: moov\\trak (soun)
        #if (MAX_DEPACK_CACHE == 4)
        MMPF_VIDPSR_SwapDepackCacheTo(AUDIO_CHUNK_CACHE);
        #endif
        error = MMPF_3GPPSR_GetNextSample(&gAudioMedia, &tmpAtom);
    
        curHeader.ulTime = MMPF_3GPPSR_GetTime(&gAudioMedia);
        usHeadherReduntSize = SIZEOF_AUDIO_HEADER;
    
        preAudioReduntSize = tmpAtom.ulSize + SIZEOF_AUDIO_HEADER;
    
        curHeader.ulSize = preAudioReduntSize;

        curHeader.ulFlag2 ++;
        curHeader.ulFlag2 = (MMP_SHORT)(MMP_ULONG)dataptr;
    
        preAudioOffset = tmpAtom.ulOffset;
        //gl3gpAudioSize -= tmpAtom.ulSize;
        
        curHeader.ulFlag = BUFFERFLAG_EOF;
        if (error == MMP_VIDPSR_ERR_EOS) {
            curHeader.ulFlag |= BUFFERFLAG_EOS;
            m_bAudioEOS = MMP_TRUE;
        }
        //m_BuffedAudioAtoms++;
    }

    if (preAudioReduntSize) {

        // write header
        if (usHeadherReduntSize > 0) { // there is header to write
            MMP_USHORT i;
            if (usHeadherReduntSize >= Size) {
                usHeaderSizeToWrite = Size;
            }
            else {
                usHeaderSizeToWrite = usHeadherReduntSize;
            }
            preAudioReduntSize -= usHeaderSizeToWrite;
            for (i = SIZEOF_AUDIO_HEADER - usHeadherReduntSize;
                 i < SIZEOF_AUDIO_HEADER - usHeadherReduntSize + usHeaderSizeToWrite; i++) {
                *(dataptr ++) = *(((MMP_UBYTE*)(&curHeader)) + i);
            }
            usHeadherReduntSize -= usHeaderSizeToWrite;
        }
        Size -= usHeaderSizeToWrite;

        if (preAudioReduntSize >= Size) {
            LoadSize = Size;
        }
        else {
            LoadSize = preAudioReduntSize;
        }
        preAudioReduntSize -= LoadSize;

        MMPF_VIDPSR_SwapDepackCacheTo(AUDIO_CACHE);

        if ( MMPF_VIDPSR_ReadFile(dataptr, preAudioOffset, LoadSize,0) != MMP_ERR_NONE) 
            return MMP_VIDPSR_ERR_CONTENT_CORRUPT;
            
        preAudioOffset += LoadSize;

        *LoadedSize = LoadSize + usHeaderSizeToWrite;
        
        if ( preAudioReduntSize == 0)
            m_BuffedAudioAtoms+=1;
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
static MMP_ERR MMPF_3GPPSR_FillAudioBuffer(MMP_ULONG dest_addr,MMP_ULONG ulSize, MMP_USHORT *usTransferSize)
{
    MMP_ULONG   file_return_size2 = 1;
    MMP_ERR     error = MMP_ERR_NONE;
    
    *usTransferSize = 0;
    
    while (ulSize > *usTransferSize && file_return_size2 != 0) {
        
        error = MMPF_3GPPSR_LoadAudioData((MMP_UBYTE *)(dest_addr+ *usTransferSize),
                                  ulSize - *usTransferSize, &file_return_size2);
        if (error == MMP_ERR_NONE)
            *usTransferSize += file_return_size2;
        else
            return error;
    }
    
    return error;
}


#if 0 // for IDE function name list by section
void _____Demuxer_____(){}
#endif

/** @name Demuxer Functions
@{
Also known as parser functions. Includes 3GP file parsing, and file reading with cache management.
*/

/** @brief Utility to get a unsigned long from specific memory address

Reverse the pointer for big-endian issue.
@param[in] ptr The pointer to be converted
@return The reversed unsigned long value
*/
static MMP_ULONG MMPF_3GPPSR_Ptr2Int(const MMP_UBYTE *ptr)
{
    return (((MMP_ULONG) ptr[0]) << 24) + (((MMP_ULONG) ptr[1]) << 16) + (((MMP_ULONG) ptr[2]) << 8) + 
           (MMP_ULONG) ptr[3];
}

/** @brief Seek the next video frame

It reads VOP address, seeks the next frame, marks EOS and reports error
@return The error status of the function
@retval MMP_M_ERR_EOS End of video bitstream.
@retval MMP_M_ERR_INSUFFICIENT_RESOURCES When the input bitstream buffer overflow.
@retval MMP_M_ERR_OVERFLOW In ring buffer payload, it has to wait the others to free the buffer
@retval MMP_M_ERR_INCORRECT_STATE_OPERATION The seek mode is not supported.
*/
static MMP_ERR MMPF_3GPPSR_SeekNextVideoFrame(MMP_ULONG *RequestFrame)
{
    MMP_ERR error;
    
    *RequestFrame = 0;
    ///- Swap cache index to 3gp atom: moov\\trak (vide)
    #if (MAX_DEPACK_CACHE == 4)
    MMPF_VIDPSR_SwapDepackCacheTo(VIDEO_CHUNK_CACHE);
    #endif

    ///- Get video presentation time offset for following decoded bitstream
    /// See PTS/DTS in MPEG for further study.
    if (m_seek.mode == MMPF_M_SEEK_NORMAL/* || (m_seek.mode == MMPF_M_SEEK_FAST_P)*/ ) {
        error = MMPF_3GPPSR_GetNextSample(&gVideoMedia, &gVideoSample);
    }
    else if (m_seek.mode == MMPF_M_FF_KEY) {
        error = MMPF_3GPPSR_SeekIFrame(m_seek.sSpeed/100, CUR_DEMUXED_VIDEO_FRAME,RequestFrame);
        if (error == MMP_VIDPSR_ERR_EOS) {
			// Penguin 20080128
//            error = MMP_ERR_NONE; // continue to play so not return EOS
            m_seek.mode = MMPF_M_SEEK_NONE;
        }
        else {// make the audio follow the video
            if (gVidFileInfo.common.is_audio_available) {
                if( MMPF_3GPPSR_SeekSample(MMPF_3GP_TRACK_AUDIO, MMPF_3GPPSR_GetTime(&gVideoMedia), MMP_TRUE) == MMP_VIDPSR_ERR_CONTENT_CORRUPT)
                    return MMP_VIDPSR_ERR_CONTENT_CORRUPT;
            }
            else { // video only, find the last demuxed time (36 bytes)
                ///@warning This is a incorrect method. The FW resuming would be jaggy.
//                m_clock.ulMediaTime = m_3GPPdata.pPreWriteHeader->ulRequestTime;
            }
        }
    }
    else if (m_seek.mode == MMPF_M_SEEK_NONE) {
        return MMP_VIDPSR_ERR_EOS;
    }
    else {
        return MMP_VIDPSR_ERR_INCORRECT_STATE_OPERATION;
    }

    // calculate the offsets before and after the bitstream in all cases
    m_3GPPdata.usBSOffset = sizeof(MMPF_M_BUFFER_HEADER);
    // ARM address padding
    m_3GPPdata.usEOFPadding = (4 - ((gVideoSample.ulSize + m_3GPPdata.usBSOffset) % 4)) % 4 + RING_BUF_TAILING;
    
    //#if SUPPORT_LARGE_FRAME == 1
    if ( m_support_large_frame ) {
        // if it's a large frame, use fixed header 0x20;
        if ((gVideoSample.ulSize + m_3GPPdata.usBSOffset + m_3GPPdata.usEOFPadding) > m_VideoStreamLen){
            m_3GPPdata.usBSOffset = 0x20;
        }
    }
    //#endif
	

    return error;
    #if (RING_BUF_TAILING % 4) != 0
        #error RING_BUF_TAILING must be a multiple of 4
    #endif
    // here we've alreay known the size and offset of the next frame
    // update the frame pointer first to know if right address to read the frame
}

/** @brief Get integer from atom buffer 

@param[in,out] MediaInfo Media Information
@param[in] atomType The atom type.
@param[in] index The index number of requested index.
@return The requested 4 bytes unisnged integer.
*/
static MMP_ERR MMPF_3GPPSR_GetIntFromAtomBuffer(MMP_3GP_MEDIA_INFO *MediaInfo, const MMPF_3GP_ATOM atomType, 
          const MMP_ULONG index, MMP_ULONG *value)
{
    static MMP_UBYTE nFields[6] = {1, 3, 1, 2, 1, 2};
    MMP_UBYTE *ptr;
    MMP_ULONG track;
    MMP_ULONG size;
    MMP_USHORT DWperAtom; // number of double words(4 bytes) per atom
    MMP_USHORT remainder; // The offset in the atom buffer
    int cache_map;
    /*if ((atomType == MMPF_3GP_ATOM_STSZ) && (MediaInfo->samplesize)) {
        return MediaInfo->samplesize;
    }*/
    
    if (MediaInfo->ulTrackID == gVideoMedia.ulTrackID) {
        track = MMPF_3GP_TRACK_VIDEO;
        cache_map = 2 + (atomType * 2);
    }
    else {// assume that it's audio
        track = MMPF_3GP_TRACK_AUDIO;
        cache_map = 3 + (atomType * 2);
    }
    
    #if (MAX_DEPACK_CACHE == 14)
    MMPF_VIDPSR_SwapDepackCacheTo(cache_map);
    #endif
    DWperAtom = nFields[atomType];
    
    if (atomType == MMPF_3GP_ATOM_STSZ) {
        if (MediaInfo->usSampleSize) {
            *value = MediaInfo->usSampleSize;
            return MMP_ERR_NONE;
        }
    }
    size = m_3GPAtomBuf.ubSize[atomType];
    remainder = index % size;
    if ((index < m_3GPAtomBuf.ulIndex[track][atomType]) ||
        ((index - m_3GPAtomBuf.ulIndex[track][atomType]) >= size)) {
        m_3GPAtomBuf.ulIndex[track][atomType] = index - remainder;
        
        if ( MMPF_VIDPSR_ReadFile((MMP_UBYTE *)m_3GPAtomBuf.ulTableAddress[track][atomType],
            MediaInfo->ulSTOffset[atomType] + DWperAtom * (((m_3GPAtomBuf.ulIndex[track][atomType]) << 2)),
            DWperAtom * (size << 2),0) != MMP_ERR_NONE) 
            return MMP_VIDPSR_ERR_CONTENT_CORRUPT;
    }
    ptr = (MMP_UBYTE *) (&(m_3GPAtomBuf.ulTableAddress[track][atomType][DWperAtom * remainder +
          m_sOffsetInAtom]));

    if (atomType == MMPF_3GP_ATOM_STTS) {
        MediaInfo->lLastTime = (MMP_LONG) MMPF_3GPPSR_Ptr2Int(ptr + 4);
    }
    if (atomType == MMPF_3GP_ATOM_CTTS){
        MediaInfo->lLastCTime = (MMP_LONG)MMPF_3GPPSR_Ptr2Int(ptr + 4);
    }
    *value = MMPF_3GPPSR_Ptr2Int(ptr);
    return MMP_ERR_NONE;
}

#define MMPF_3GPPSR_GetFrameSize(MediaInfo, index, value)\
    MMPF_3GPPSR_GetIntFromAtomBuffer(MediaInfo, MMPF_3GP_ATOM_STSZ, index, value)

#define MMPF_3GPPSR_GetSamplePerChunk(MediaInfo, index, value)\
    MMPF_3GPPSR_GetIntFromAtomBuffer(MediaInfo, MMPF_3GP_ATOM_STSC, index, value)

#define MMPF_3GPPSR_GetFirstChunk(MediaInfo, index, value)\
    MMPF_3GPPSR_GetIntFromAtomBuffer(MediaInfo, MMPF_3GP_ATOM_STSC, index, value)

#define MMPF_3GPPSR_GetChunkOffset(MediaInfo, index, value)\
    MMPF_3GPPSR_GetIntFromAtomBuffer(MediaInfo, MMPF_3GP_ATOM_STCO, index, value)

#define MMPF_3GPPSR_GetSamplePerTime(MediaInfo, index, value)\
    MMPF_3GPPSR_GetIntFromAtomBuffer(MediaInfo, MMPF_3GP_ATOM_STTS, index, value)

#define MMPF_3GPPSR_GetVideoSyncFrame(index, value)\
    MMPF_3GPPSR_GetIntFromAtomBuffer(&gVideoMedia, MMPF_3GP_ATOM_STSS, index, value)

#define MMPF_3GPPSR_GetSamplePerCTime(MediaInfo, index, value)\
    MMPF_3GPPSR_GetIntFromAtomBuffer(MediaInfo, MMPF_3GP_ATOM_CTTS, index, value)
/** @brief Seek the next sample time in the 3GP file

@pre The m_seek.sAdvance is set to -1 or 1 to indicate that it's next or previous sample
     @warning The value other than -1 and 1 of m_seek.sAdvance results in undefined behavior.
@pre The m_seek.ulTragetFrame is set to current sample
@post The m_seek.ulTragetFrame would record which frame the result time belongs to
@param[in] pMedia Which media track to be seeked.
@return Always return success
*/
static MMP_ERR MMPF_3GPPSR_SeekNextSampleTime(MMP_3GP_MEDIA_INFO* pMedia)
{
    MMP_ERR error = MMP_ERR_NONE;
    
    // New sample per time atom
    if (m_seek.sAdvance > 0) {
        if (CTTS_ENABLE && pMedia->bCTTSEnable){
        
            if (pMedia->ulSamplePerTimeIndex == (MMP_ULONG)0) {
                error = MMPF_3GPPSR_GetSamplePerTime(pMedia, pMedia->ulTimeOffset, &(pMedia->ulSamplePerTime));
                if (error != MMP_ERR_NONE)
                    return error;
                //pMedia->ulSamplePerTime = MMPF_3GPPSR_GetSamplePerTime(pMedia, pMedia->ulTimeOffset);
            }
            if(pMedia->ulSamplePerCTimeIndex == (MMP_ULONG)0) {
                error = MMPF_3GPPSR_GetSamplePerCTime(pMedia, pMedia->ulCTimeOffset, &(pMedia->ulSamplePerCTime));
                if (error != MMP_ERR_NONE)
                    return error;
            }
            
            pMedia->ulSamplePerTimeIndex += m_seek.sAdvance;
            pMedia->ulSamplePerCTimeIndex += m_seek.sAdvance;

            // End of stts atom, proceed to stts atom
            if (pMedia->ulSamplePerTimeIndex == pMedia->ulSamplePerTime) {
                pMedia->ulSamplePerTimeIndex = (MMP_ULONG)0;
                pMedia->ulTimeOffset += m_seek.sAdvance;
            }
            if (pMedia->ulSamplePerCTimeIndex == pMedia->ulSamplePerCTime) {
                pMedia->ulSamplePerCTimeIndex = (MMP_ULONG)0;
                pMedia->ulCTimeOffset += m_seek.sAdvance;
            }
            pMedia->decodingTime += pMedia->lLastTime * m_seek.sAdvance;
            pMedia->tickPassTime = (pMedia->decodingTime + pMedia->lLastCTime);
            //pMedia->tickPassTime += pMedia->lLastTime * m_seek.sAdvance;
        } else{
            if (pMedia->ulSamplePerTimeIndex == (MMP_ULONG)0) {
            error = MMPF_3GPPSR_GetSamplePerTime(pMedia, pMedia->ulTimeOffset, &(pMedia->ulSamplePerTime));
            if (error != MMP_ERR_NONE)
                return error;
            //pMedia->ulSamplePerTime = MMPF_3GPPSR_GetSamplePerTime(pMedia, pMedia->ulTimeOffset);
        }
        
        pMedia->ulSamplePerTimeIndex += m_seek.sAdvance;

        // End of stts atom, proceed to stts atom
        if (pMedia->ulSamplePerTimeIndex == pMedia->ulSamplePerTime) {
            pMedia->ulSamplePerTimeIndex = (MMP_ULONG)0;
            pMedia->ulTimeOffset += m_seek.sAdvance;
        }
        pMedia->tickPassTime += pMedia->lLastTime * m_seek.sAdvance;
            
        }
    }
    else {
    if (CTTS_ENABLE && pMedia->bCTTSEnable){
        pMedia->ulSamplePerTimeIndex += m_seek.sAdvance;
        pMedia->ulSamplePerCTimeIndex += m_seek.sAdvance;
        
        if (pMedia->ulSamplePerTimeIndex == (MMP_ULONG)-1) {
        	pMedia->ulTimeOffset += m_seek.sAdvance;
        	error = MMPF_3GPPSR_GetSamplePerTime(pMedia, pMedia->ulTimeOffset, &(pMedia->ulSamplePerTime));
        	if (error != MMP_ERR_NONE)
        	    return MMP_ERR_NONE;
        	pMedia->ulSamplePerTimeIndex = pMedia->ulSamplePerTime - 1;
        }
        if(pMedia->ulSamplePerCTimeIndex == (MMP_ULONG)-1){
            pMedia->ulCTimeOffset += m_seek.sAdvance;
            error = MMPF_3GPPSR_GetSamplePerCTime(pMedia, pMedia->ulCTimeOffset, &(pMedia->ulSamplePerCTime));
            if(error != MMP_ERR_NONE)
                return MMP_ERR_NONE;
            pMedia->ulSamplePerCTimeIndex = pMedia->ulSamplePerCTime - 1;
        }
        pMedia->decodingTime -= pMedia->lLastTime;
        pMedia->tickPassTime = (pMedia->decodingTime + pMedia->lLastCTime);
        //pMedia->tickPassTime -= pMedia->lLastTime;
        
    } else {
        pMedia->ulSamplePerTimeIndex += m_seek.sAdvance;
        if (pMedia->ulSamplePerTimeIndex == (MMP_ULONG)-1) {
        	pMedia->ulTimeOffset += m_seek.sAdvance;
        	error = MMPF_3GPPSR_GetSamplePerTime(pMedia, pMedia->ulTimeOffset, &(pMedia->ulSamplePerTime));
        	if (error != MMP_ERR_NONE)
        	    return MMP_ERR_NONE;
        	pMedia->ulSamplePerTimeIndex = pMedia->ulSamplePerTime - 1;
        }
        pMedia->tickPassTime -= pMedia->lLastTime;
        }
    }

    m_seek.ulTragetFrame += m_seek.sAdvance;

    return MMP_ERR_NONE;
}

/** @brief Seek the next sample time in the 3GP file

@pre The m_seek.sAdvance is set to -1 or 1 to indicate that it's next or previous sample
     @warning The value other than -1 and 1 of m_seek.sAdvance results in undefined behavior.
@pre The m_seek.ulTempFrame is set to current sample
@post The m_seek.ulTempFrame would record which frame the result chunk starts from
@param[in] pMedia Which media track to be seeked.
@return Always return success
*/
static MMP_ERR MMPF_3GPPSR_SeekNextSampleChunk(MMP_3GP_MEDIA_INFO* pMedia)
{
    MMP_ERR error = MMP_ERR_NONE;
    
    if (m_seek.sAdvance > 0) {
        // New chunk, get sample per chunk to know how many samples in this chunk
        if (pMedia->ulSamplePerChunkIndex == 0) {
            //???? starting
            if (pMedia->lCurChunk >= pMedia->lLastChunk - 1) {
                // Get sample per chunk (how many samples in this chunk)
                m_sOffsetInAtom = 1;
                error = MMPF_3GPPSR_GetSamplePerChunk(pMedia, pMedia->ulChuckOffset, &(pMedia->ulSamplePerChunk));
                if(error != MMP_ERR_NONE)
                    return error;
                    
                //pMedia->ulSamplePerChunk = MMPF_3GPPSR_GetSamplePerChunk(pMedia, pMedia->ulChuckOffset);
                m_sOffsetInAtom = 0;
                error = MMPF_3GPPSR_GetFirstChunk(pMedia, pMedia->ulChuckOffset, (MMP_ULONG*)(&pMedia->lFirstChunk));
                if(error != MMP_ERR_NONE)
                    return error;
                
                //pMedia->lFirstChunk = MMPF_3GPPSR_GetFirstChunk(pMedia, pMedia->ulChuckOffset);
                // out of total stco entries
                if (pMedia->ulChuckOffset == pMedia->ulSTSCCount - 1) {
                    pMedia->lLastChunk = 0x7FFFFFFF;//MediaInfo->ChunkCount+1;
                }
                else {
                    error = MMPF_3GPPSR_GetFirstChunk(pMedia, pMedia->ulChuckOffset + 1, (MMP_ULONG*)(&pMedia->lLastChunk));
                    if(error != MMP_ERR_NONE)
                        return error;
                    //pMedia->lLastChunk = MMPF_3GPPSR_GetFirstChunk(pMedia, pMedia->ulChuckOffset + 1);
                }
                pMedia->ulChuckOffset++;
            }
            m_seek.lChunkToLoad = pMedia->lCurChunk;
            m_seek.ulTempFrame = pMedia->ulFrameOffset;
        }
        pMedia->ulSamplePerChunkIndex++;
        // End of current chunk, proceed to next chunk
        if (pMedia->ulSamplePerChunkIndex == pMedia->ulSamplePerChunk) {
            pMedia->ulSamplePerChunkIndex = 0;
            pMedia->lCurChunk++;
        }

        pMedia->ulFrameOffset++;
    }
    else {
        pMedia->ulFrameOffset --;
    	pMedia->ulSamplePerChunkIndex--;
        if (pMedia->ulSamplePerChunkIndex == (MMP_ULONG) -1) {
            //???? starting
            if (pMedia->lCurChunk < pMedia->lFirstChunk) {
                pMedia->ulChuckOffset--;
                // Get sample per chunk (how many samples in this chunk)
                m_sOffsetInAtom = 1;
                error = MMPF_3GPPSR_GetSamplePerChunk(pMedia, pMedia->ulChuckOffset - 1, &(pMedia->ulSamplePerChunk));
                if(error != MMP_ERR_NONE)
                    return error;
                
                //pMedia->ulSamplePerChunk = MMPF_3GPPSR_GetSamplePerChunk(pMedia, pMedia->ulChuckOffset - 1);
                m_sOffsetInAtom = 0;
                error = MMPF_3GPPSR_GetFirstChunk(pMedia, pMedia->ulChuckOffset - 1, (MMP_ULONG*)(&(pMedia->lFirstChunk)));
                if(error != MMP_ERR_NONE)
                    return error;
                error = MMPF_3GPPSR_GetFirstChunk(pMedia, pMedia->ulChuckOffset    , (MMP_ULONG*)(&(pMedia->lLastChunk)));
                if(error != MMP_ERR_NONE)
                    return error;
                //pMedia->lFirstChunk = MMPF_3GPPSR_GetFirstChunk(pMedia, pMedia->ulChuckOffset - 1);
                //pMedia->lLastChunk = MMPF_3GPPSR_GetFirstChunk(pMedia, pMedia->ulChuckOffset);
            }
            pMedia->lCurChunk --;
            m_seek.lChunkToLoad = pMedia->lCurChunk;
            m_seek.ulTempFrame = pMedia->ulFrameOffset + 1;
            pMedia->ulSamplePerChunkIndex = pMedia->ulSamplePerChunk - 1;
        }
    }
    return MMP_ERR_NONE;
}

/** @brief Seek the next sample time in the 3GP file

@pre The m_seek.sAdvance is set to -1 or 1 to indicate that it's next or previous sample
     @warning The value other than -1 and 1 of m_seek.sAdvance results in undefined behavior.
@pre The m_seek.ulTempFrame and ulTargetFrame are assigned or calculated
@param[in] pMedia Which media track to be seeked.
@param[out] pSample Output the offset in the ulOffset field.
@return Always return success
*/
static MMP_ERR MMPF_3GPPSR_SeekNextSampleOffset(MMP_3GP_MEDIA_INFO* pMedia, MMP_3GP_SAMPLE_INFO *pSample)
{
    MMP_LONG i;
    MMP_ULONG value;
    MMP_ERR   error = MMP_ERR_NONE;
    
    if (m_seek.sAdvance > 0) {
        if (m_seek.lChunkToLoad != (MMP_ULONG) -1) {
            error = MMPF_3GPPSR_GetChunkOffset(pMedia, m_seek.lChunkToLoad, &(pSample->ulOffset));
            if (error != MMP_ERR_NONE)
                return error;
            //pSample->ulOffset = MMPF_3GPPSR_GetChunkOffset(pMedia, m_seek.lChunkToLoad);
        }
        else {
            pSample->ulOffset = pMedia->ulPreFrameOffset + pMedia->ulPreFrameSize;
        }
        for ( i = 0; i < m_seek.ulTragetFrame - m_seek.ulTempFrame - 1; i += m_seek.sAdvance) {
            error = MMPF_3GPPSR_GetFrameSize(pMedia, m_seek.ulTempFrame + i, &value);
            if(error != MMP_ERR_NONE)
                return error;
            
            pSample->ulOffset += value * m_seek.sAdvance;
                
            //pSample->ulOffset += MMPF_3GPPSR_GetFrameSize(pMedia, m_seek.ulTempFrame + i) * m_seek.sAdvance;
        }
    }
    else {
        if (m_seek.lChunkToLoad != (MMP_ULONG) -1) {
            m_seek.ulTempFrame -= pMedia->ulSamplePerChunk;
            error = MMPF_3GPPSR_GetChunkOffset(pMedia, m_seek.lChunkToLoad, &(pSample->ulOffset));
            if(error != MMP_ERR_NONE)
                return error;
                
            //pSample->ulOffset = MMPF_3GPPSR_GetChunkOffset(pMedia, m_seek.lChunkToLoad);
            for ( i = 0; i < m_seek.ulTragetFrame - m_seek.ulTempFrame - 1; i ++) {
                error = MMPF_3GPPSR_GetFrameSize(pMedia, m_seek.ulTempFrame + i, &value);
                if ( error != MMP_ERR_NONE )
                    return error;
                 pSample->ulOffset += value;// * sAdvance; 
                   
                //pSample->ulOffset += MMPF_3GPPSR_GetFrameSize(pMedia, m_seek.ulTempFrame + i);// * sAdvance;
            }
        }
        else {
            pSample->ulOffset = pMedia->ulPreFrameOffset;
            for ( i = 0; i < m_seek.ulTempFrame - m_seek.ulTragetFrame; i ++) {
                error = MMPF_3GPPSR_GetFrameSize(pMedia, m_seek.ulTragetFrame + i - 1, &value);// * sAdvance;
                if (error != MMP_ERR_NONE)
                    return error;
                pSample->ulOffset -= value;// * sAdvance;
                
                //pSample->ulOffset -= MMPF_3GPPSR_GetFrameSize(pMedia, m_seek.ulTragetFrame + i - 1);// * sAdvance;
            }
       }
    }
    return MMP_ERR_NONE;
}

/** @brief Seek the next sample time in the 3GP file

@pre The m_seek.sAdvance is set to -1 or 1 to indicate that it's next or previous sample
     @warning The value other than -1 and 1 of m_seek.sAdvance results in undefined behavior.
@pre The m_seek.ulTempFrame and ulTargetFrame are assigned or calculated
@param[in] pMedia Which media track to be seeked.
@param[out] pSample Output the offset in the ulOffset field.
@return Always return success
*/
static MMP_ERR MMPF_3GPPSR_SeekNextSampleSize(MMP_3GP_MEDIA_INFO* pMedia, MMP_3GP_SAMPLE_INFO *pSample)
{
    MMP_ERR   error  = MMP_ERR_NONE;
    MMP_ULONG ulSize;
    //MMP_ULONG ulSize = MMPF_3GPPSR_GetFrameSize(pMedia, pMedia->ulFrameOffset - 1);

    error = MMPF_3GPPSR_GetFrameSize(pMedia, pMedia->ulFrameOffset - 1, &ulSize);
    if (error != MMP_ERR_NONE)
        return error;
        
    if (m_seek.sAdvance > 0) {
        pSample->ulSize = ulSize;
        pMedia->ulPreFrameSize = pSample->ulSize;
        pMedia->ulPreFrameOffset = pSample->ulOffset;
    }
    else {
        error = MMPF_3GPPSR_GetFrameSize(pMedia, pMedia->ulFrameOffset, &(pSample->ulSize));
        if (error != MMP_ERR_NONE)
            return error;
        //pSample->ulSize = MMPF_3GPPSR_GetFrameSize(pMedia, pMedia->ulFrameOffset);
        pMedia->ulPreFrameSize = ulSize;
        pMedia->ulPreFrameOffset = pSample->ulOffset - pMedia->ulPreFrameSize;
    }
    return MMP_ERR_NONE;
}

/** @brief Search Video frame to sync with audio. Mostly called when FastForward or FastRewind.

This function searches the nearest sync frame (key frame) from current frame.
@param[in] curFrame The current frame. It should be 0~max frame count.
@param[in] sOffset The number of I frame to be advanced. Generally it's been tested by -5~5, 
                   but it's not limit to it. 0 was excluded.
@param[out] pRequestFrame The pointer to out put the index of the request sync frame
@return The status of the function call.
@retval MMP_M_ERR_EOS End of bitstream;
@retval MMP_ERR_NONE Normal search.
@retval MMP_M_ERR_BAD_PARAMETER The request frame is out of range.
*/
static MMP_ERR MMPF_3GPPSR_SeekVideoSyncFrame(const MMP_ULONG curFrame, const MMP_SHORT sOffset, 
                                              MMP_ULONG *pRequestFrame)
{
    MMP_ULONG tmpFrame;
    MMP_LONG lTargetIndex;
    MMP_ULONG ulMaxSTSSIndex = gVideoMedia.ulSTSSCount - 1;
    MMP_ERR error = MMP_ERR_NONE;

    if (curFrame > gVideoMedia.ulSTSZCount) { // gVideoInfo.stszCount = number of video samples/frames
        *pRequestFrame = 0;
        return MMP_VIDPSR_ERR_PARAMETER;
    }
    else {
    // search the frame. when the number is valid
        // find current index
        lTargetIndex = m_seek.ulFFIndex; // change the calculation into the signed operation.
        while(1) {
            error = MMPF_3GPPSR_GetVideoSyncFrame(lTargetIndex,&tmpFrame);
            if (error != MMP_ERR_NONE)
                return error;
                
            //tmpFrame = MMPF_3GPPSR_GetVideoSyncFrame(lTargetIndex);
            if (((tmpFrame > curFrame) || (lTargetIndex >= ulMaxSTSSIndex))) {
                break;
            }
            lTargetIndex++;
        };
        while ((tmpFrame > curFrame) &&  (lTargetIndex != 0)){ // backward
            lTargetIndex--;
            error = MMPF_3GPPSR_GetVideoSyncFrame(lTargetIndex,&tmpFrame);
            if (error != MMP_ERR_NONE)
                return error;
        }

        // calculate the requested index
        lTargetIndex += sOffset;

        if ((sOffset < 0) && (lTargetIndex < ulMaxSTSSIndex - 1)) {
            lTargetIndex ++;
        }
        if (lTargetIndex <= 0) {
            lTargetIndex = 0;
            error = MMP_VIDPSR_ERR_EOS;
        }
        else if (lTargetIndex >= ulMaxSTSSIndex) {
            lTargetIndex = ulMaxSTSSIndex;
            error = MMP_VIDPSR_ERR_EOS;
        }
        m_seek.ulFFIndex = lTargetIndex;
        
        error = MMPF_3GPPSR_GetVideoSyncFrame(m_seek.ulFFIndex,pRequestFrame);
        //*pRequestFrame = MMPF_3GPPSR_GetVideoSyncFrame(m_seek.ulFFIndex);
    }
    return error;
} // MMPF_3GPPSR_SearchVideoSyncFrame

/** @brief AV sync core for 3gp file.

@param [in,out] pMedia The media info to be serached
@param [out] pSample The search structure of the 3GP file
@return It return the presentation time offset for following decoded bitstream.
*/  //code size 226
static MMP_ERR MMPF_3GPPSR_GetNextSample(MMP_3GP_MEDIA_INFO *pMedia, MMP_3GP_SAMPLE_INFO *pSample)
{
    MMP_ERR error = MMP_ERR_NONE;
    // frame index is out of total frame number
    /*
    if (pMedia->ulFrameOffset >= pMedia->ulFrameCount) {
        MEMSET0(pSample);
        return MMP_VIDPSR_ERR_EOS;
    }
    */
    if (pMedia->ulFrameOffset >= pMedia->ulFrameCount) {
        MEMSET0(pSample);
        return MMP_VIDPSR_ERR_EOS;
    }

    // init the seek
    m_seek.lChunkToLoad = -1;
    m_seek.ulTempFrame = m_seek.ulTragetFrame = pMedia->ulFrameOffset;
    m_seek.sAdvance = 1;

    error = MMPF_3GPPSR_SeekNextSampleTime(pMedia);
    if(error != MMP_ERR_NONE)
        return error;
    error = MMPF_3GPPSR_SeekNextSampleChunk(pMedia);
    if(error != MMP_ERR_NONE)
        return error;
        
    error = MMPF_3GPPSR_SeekNextSampleOffset(pMedia, pSample); // get offset into sample
    if(error != MMP_ERR_NONE)
        return error;
        
    error = MMPF_3GPPSR_SeekNextSampleSize(pMedia, pSample);// get size into sample
    if(error != MMP_ERR_NONE)
        return error;
        
    pSample->lSampleTime = pMedia->lLastTime;

    if (pMedia->ulFrameOffset >= (pMedia->ulFrameCount - 1))
        return MMP_VIDPSR_ERR_EOS;
        
    return MMP_ERR_NONE;
}

/** @brief Jump to the requested index frame sequntially

@param[in] nAdvance Number of frame to jump. could be negative to backward I frames. Positive for 
                    forward I frames.\n 0 is undefined.
@param[in] curFrame Current frame (INDEX?)
@return The error status of the function call
*/
static MMP_ERR MMPF_3GPPSR_SeekIFrame(const MMP_SHORT nAdvance, const MMP_ULONG curFrame,MMP_ULONG *requestFrame) 
{
	//MMP_ULONG requestFrame;
	MMP_ERR error = MMP_ERR_NONE;
	
	#if (MAX_DEPACK_CACHE == 4)
    MMPF_VIDPSR_SwapDepackCacheTo(VIDEO_CHUNK_CACHE);
    #endif
    
    if (nAdvance == 0) {
        return MMP_VIDPSR_ERR_PARAMETER;//undefined
    }
    
	error = MMPF_3GPPSR_SeekVideoSyncFrame(curFrame, nAdvance, requestFrame);// save 60/76 code size
    if ( (error != MMP_VIDPSR_ERR_PARAMETER) && (error != MMP_VIDPSR_ERR_CONTENT_CORRUPT) ) {
        error = MMPF_3GPPSR_SeekSample(MMPF_3GP_TRACK_VIDEO, *requestFrame, MMP_FALSE);        
	}
	m_seek.ulRequestFrame = *requestFrame;

	return	error;
}

/** @brief Make the current atom seek to the media track by time

@param[in] track The video or audio track to be seeked.
@param[in] ulTarget The time to be seeked in ms.
@return The error status of the function call. It always return MMP_ERR_NONE.
*/
static MMP_ERR MMPF_3GPPSR_SeekSample(const MMPF_3GP_TRACK track,
       const MMP_ULONG ulTarget, const MMP_BOOL bByTime)
{
	MMP_3GP_TICK tickTargetTime;
	MMP_3GP_MEDIA_INFO *pMedia;
	MMP_3GP_SAMPLE_INFO *pSample;
	MMP_ULONG ulTimeBase;
	MMP_LONG lDifference;
	MMP_ERR             error = MMP_ERR_NONE;

    if (track == MMPF_3GP_TRACK_AUDIO) {
        #if (MAX_DEPACK_CACHE == 4)
    	MMPF_VIDPSR_SwapDepackCacheTo(AUDIO_CHUNK_CACHE);
    	#endif
    	
    	pMedia              = &gAudioMedia;
    	pSample             = &gAudioSample;
        preAudioOffset      = 0;
        preAudioReduntSize  =0;
    }
    else if (track == MMPF_3GP_TRACK_VIDEO) {
        #if (MAX_DEPACK_CACHE == 4)
    	MMPF_VIDPSR_SwapDepackCacheTo(VIDEO_CHUNK_CACHE);
    	#endif
    	pMedia              = &gVideoMedia;
    	pSample             = &gVideoSample;
    }
    else {
        return MMP_VIDPSR_ERR_PARAMETER;
    }
    
    ulTimeBase          = pMedia->ulTimeScale;
	m_seek.sAdvance     = 1;
	m_seek.ulTempFrame  = m_seek.ulTragetFrame = pMedia->ulFrameOffset;

    if (bByTime) {	
        // out of range
    	if (ulTarget > gVidFileInfo.common.total_file_time) {
    		return MMP_VIDPSR_ERR_PARAMETER;
        }

    	tickTargetTime = (MMP_3GP_TICK)ulTarget * ulTimeBase / 1000;
    	// find the time to sample
    	if (pMedia->tickPassTime < tickTargetTime) { // forward search
    		while((pMedia->tickPassTime < tickTargetTime)) {
    		    error = MMPF_3GPPSR_SeekNextSampleTime(pMedia);
    		    if (error != MMP_ERR_NONE)
    		        return error;
    		}
    	}
    	//else if (MediaInfo->tickPassTime > tickTargetTime) { // backward search
    	else { // backward search
        	m_seek.sAdvance = -1;
    		while((pMedia->tickPassTime > tickTargetTime)) {
    		    error = MMPF_3GPPSR_SeekNextSampleTime(pMedia);
    		    if ( error != MMP_ERR_NONE )
    		        return error;
    		}
            m_seek.ulTragetFrame++;
    	}
        DBG_S(3, " Target Frame");
        if (track == MMPF_3GP_TRACK_VIDEO) {
            DBG_S(3, "(V)");
        }
        else {
            DBG_S(3, "(A)");
        }
        DBG_D(3, m_seek.ulTragetFrame, 5);
        DBG_S(3, "\r\n");
    }
    else {
        // out of range
    	if (ulTarget > gVideoMedia.ulFrameCount) {
    		return MMP_VIDPSR_ERR_PARAMETER;
        }
        m_seek.ulTragetFrame = ulTarget;
    }

    lDifference = m_seek.ulTragetFrame - pMedia->ulFrameOffset;

    // find the time to sample
    m_seek.lChunkToLoad = -1; // reset the chunk to load
    if (pMedia->ulFrameOffset < m_seek.ulTragetFrame) { // forward search
        while((pMedia->ulFrameOffset < m_seek.ulTragetFrame)) {
            error = MMPF_3GPPSR_SeekNextSampleChunk(pMedia);
            if ( error != MMP_ERR_NONE )
                return error;
        }
        error = MMPF_3GPPSR_SeekNextSampleOffset(pMedia, pSample);
        if ( error != MMP_ERR_NONE )
                return error;
    }
    else { // backward search
    	m_seek.sAdvance = -1;
		while((pMedia->ulFrameOffset + 1 > m_seek.ulTragetFrame)) {
            error = MMPF_3GPPSR_SeekNextSampleChunk(pMedia);
            if ( error != MMP_ERR_NONE )
                return error;
		}
	    error = MMPF_3GPPSR_SeekNextSampleOffset(pMedia, pSample);
	    if ( error != MMP_ERR_NONE )
                return error;
    }
    
    if (m_seek.sAdvance < 0) {
        lDifference --;
    }

    if (!bByTime) { // seek by frame
        for (; 0 != lDifference; lDifference -= m_seek.sAdvance) {
            error = MMPF_3GPPSR_SeekNextSampleTime(pMedia);
            if ( error != MMP_ERR_NONE )
                return error;
        }
    }

    error = MMPF_3GPPSR_SeekNextSampleSize(pMedia, pSample);
    if ( error != MMP_ERR_NONE )
        return error;
                
    pSample->lSampleTime = pMedia->lLastTime;

	return MMP_ERR_NONE;
}

/** @brief Get single frame

Get single VOP from the bitsteam to the bitstream buffer and 
assign the starting position to the HW device.
@note Save code size from MP4V(122)+H263(126) to 156Bytes = 92bytes in O2
*///@param[in] videoType Video type. Currently only supports H.263 and MP4V
static MMP_ERR MMPF_3GPPSR_GetSingleFrame(  MMP_ULONG dest_addr,MMP_ULONG lSizeToLoad,
                                            MMP_ULONG seg1     ,MMP_ULONG seg2,
                                            MMP_ULONG *container_offset)
{
    MMP_ERR     error = MMP_ERR_NONE;
    MMP_ULONG   ulRingTailing = 0;
    MMP_UBYTE   *buf_start = m_VideoStreamBuf;

    // Swap cache index to video data
    MMPF_VIDPSR_SwapDepackCacheTo(VIDEO_CACHE);
    
    
    
    #if SUPPORT_TWO_BSBUF_TYPE == 1
        // Need wrap
        if (lSizeToLoad > seg1 ) {
            if(MMPF_VIDPSR_ReadFile((MMP_UBYTE*)dest_addr, *container_offset, seg1,0) != MMP_ERR_NONE)
                return MMP_VIDPSR_ERR_CONTENT_CORRUPT;
                
            if (MMPF_VIDPSR_ReadFile(buf_start, gVideoSample.ulOffset + seg1, lSizeToLoad - seg1,0) != MMP_ERR_NONE) 
                return MMP_VIDPSR_ERR_CONTENT_CORRUPT;
                
            #if RING_BUF_TAILING > 0
                MMPF_BS_AccessVideoHeader(m_pBSHeader->buf + lSizeToLoad - seg1, (MMP_UBYTE*)&ulRingTailing, 4, RING_WRITE);
            #endif
        }
        else 
        {
            if ( MMPF_VIDPSR_ReadFile((MMP_UBYTE*)dest_addr, *container_offset, lSizeToLoad,0) != MMP_ERR_NONE)
                return MMP_VIDPSR_ERR_CONTENT_CORRUPT;
            
            if ( m_support_large_frame ) {
                *container_offset += lSizeToLoad;
                
                #if RING_BUF_TAILING > 0
                 if (!m_3GPPdata.bLargeFrame) {
                    MMPF_BS_AccessVideoHeader((MMP_UBYTE*)(dest_addr + lSizeToLoad), 
                                             (MMP_UBYTE*)&ulRingTailing, 4, RING_WRITE);
                 }
                #endif
            }
        }   
        
    #else
        #if (BUFFER_TYPE == MMP_MEDIA_RING_BUFFER)
        
        if (lSizeToLoad > seg1 ) {
            if(MMPF_VIDPSR_ReadFile((MMP_UBYTE*)dest_addr, *container_offset, seg1,0) != MMP_ERR_NONE)
                return MMP_VIDPSR_ERR_CONTENT_CORRUPT;
                
            if (MMPF_VIDPSR_ReadFile(buf_start, gVideoSample.ulOffset + seg1, lSizeToLoad - seg1,0) != MMP_ERR_NONE) 
                return MMP_VIDPSR_ERR_CONTENT_CORRUPT;
                
            #if RING_BUF_TAILING > 0
                MMPF_BS_AccessVideoHeader(m_pBSHeader->buf + lSizeToLoad - seg1, (MMP_UBYTE*)&ulRingTailing, 4, RING_WRITE);
            #endif
        }
        else 
        {
            if ( MMPF_VIDPSR_ReadFile((MMP_UBYTE*)dest_addr, *container_offset, lSizeToLoad,0) != MMP_ERR_NONE)
                return MMP_VIDPSR_ERR_CONTENT_CORRUPT;

            if ( m_support_large_frame ) {
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
        #if (BUFFER_TYPE == MMP_MEDIA_NEAR_RING_BUFFER)
        {
            if ( MMPF_VIDPSR_ReadFile((MMP_UBYTE*)dest_addr, *container_offset, lSizeToLoad,0) != MMP_ERR_NONE)
                return MMP_VIDPSR_ERR_CONTENT_CORRUPT;

            if ( m_support_large_frame ) {
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
        
    #endif
    return error;
}
/** @brief Get the current time in ms from the media info

@param[in] pMedia The media info pointer.
@return The current time in ms.
*/
static MMP_ULONG MMPF_3GPPSR_GetTime(const MMP_3GP_MEDIA_INFO *const pMedia)
{
    return pMedia->tickPassTime * 1000 / pMedia->ulTimeScale;
}

MMP_BOOL MMPF_3GPPSR_UseFastSeek(MMPF_3GP_TRACK track, MMP_ULONG ulTarget)
{
    MMP_3GP_TICK tickTargetTime;
	MMP_3GP_MEDIA_INFO *pMedia;
	MMP_ULONG ulTimeBase;

    if (track == MMPF_3GP_TRACK_AUDIO) {	
    	pMedia              = &gAudioMedia;
    }
    else if (track == MMPF_3GP_TRACK_VIDEO) {
    	pMedia              = &gVideoMedia;
    }

    
    ulTimeBase          = pMedia->ulTimeScale;
    
    tickTargetTime = (MMP_3GP_TICK)ulTarget * ulTimeBase / 1000;
    	// find the time to sample
    if ((pMedia->tickPassTime > tickTargetTime) && (gVidFileInfo.common.file_size > 150*1024*1024)) // 150MB
    {
        return MMP_TRUE;
    } else {
        return MMP_FALSE;
    }
}
/** @brief Utility to get a unsigned long in the file

Reverse the data for big-endian issue.
@param[in] curSeek The offset in the file
@param[in] advance The bytes to be added to the pointer
@return The reversed unsigned long value
*/
static MMP_ULONG MMPF_3GPPSR_GetBigLong(const MMP_ULONG curSeek, const MMP_SHORT advance, MMP_ULONG *value)
{
    MMP_ERR   error = MMP_ERR_NONE;
    MMP_UBYTE ptr[4];

    error = MMPF_VIDPSR_ReadFile(ptr, curSeek , 4,0);
    if ( error != MMP_ERR_NONE )
        return error;
        
    ulCurOffset += advance;
    *value = MMPF_3GPPSR_Ptr2Int(ptr);
    return MMP_ERR_NONE;
    
    //return MMPF_3GPPSR_Ptr2Int(ptr);
}

/** @brief Look up the samples per frame table of MP4A description

@param[in] sr_index The index of samples per frame
@return The samples per frame
@retval 0 Out of index range
*/
MMP_USHORT MMPF_3GPPSR_Get256SampleTime(const MMP_UBYTE sr_index)
{
    static const MMP_UBYTE sample256_time[] = {
    	0, 0, 0, 21, 23, 32,
    	42, 46, 64, 85, 93, 128
    };
    if (sr_index < 12) {
        return sample256_time[sr_index];
    }
    return 0;
}

/** @brief Look up the sampling rate table of AAC from MP4A description

@param[in] sr_index Index of the table 
@return The sampling rate of of MP4A(AAC)
@retval 0 Out of index range
*/
MMP_ULONG MMPF_3GPPSR_GetSampleRate(const MMP_UBYTE sr_index)
{
    static const MMP_ULONG sample_rates[] = {
        96000, 88200, 64000, 48000, 44100, 32000,
        24000, 22050, 16000, 12000, 11025, 8000
    };

    if (sr_index < 12)
        return sample_rates[sr_index];

    return 0;
}



/** @brief Parse the MP4A description and get its length

@return The length of the description
*/
MMP_ULONG	MMPF_3GPPSR_ReadMP4ADescrLength(void)
{
	MMP_UBYTE	b;   
	MMP_UBYTE	numBytes = 0;   
	MMP_ULONG	length = 0;
	MMP_UBYTE	*ptab = (MMP_UBYTE *)(m_ASCInfo);

    do {
    	b = ptab[m_AudioPtr++];
        numBytes++;
        length = (length << 7) | (b & 0x7F);
    } while ((b & 0x80) && numBytes < 4);

    return length;
}

/** @brief Parse the MP4ASC audio header

The atom is located in [moov->trak'soun'->mdia->minf->stbl->stsd->#1.optional description atoms]
@return The parsing result
@retval MMP_M_ERR_UNSUPPORTED_SETTING The format is not supported.
@retval MMP_ERR_NONE Success
*/
MMP_ERR MMPF_3GPPSR_DecodeMP4ASC(void)
{
	MMP_UBYTE	*ptab = (MMP_UBYTE *)(m_ASCInfo);
	
	MMP_UBYTE	tmp1, tmp2;
	
	m_AudioPtr = 4;
	if (ptab[m_AudioPtr++] == 0x03)	{	// tag
        if (MMPF_3GPPSR_ReadMP4ADescrLength() < 5 + 15) {
            return MMP_VIDPSR_ERR_UNSUPPORTED_SETTING;
       	}
		m_AudioPtr += 3;	// skip 3 bytes
	}	
	else {
		m_AudioPtr += 2;	// skip 2 bytes
	}	
	if (ptab[m_AudioPtr++] != 0x04)	{// DescrTab
		return	MMP_VIDPSR_ERR_UNSUPPORTED_SETTING;
	}	
			
	if (MMPF_3GPPSR_ReadMP4ADescrLength() < 13) {
		return	MMP_VIDPSR_ERR_UNSUPPORTED_SETTING;
	}	
	
	m_AudioPtr += 13;

	if (ptab[m_AudioPtr++] != 0x05) {
		return	MMP_VIDPSR_ERR_UNSUPPORTED_SETTING;
	}	

	MMPF_3GPPSR_ReadMP4ADescrLength();
		
	tmp1 = ptab[m_AudioPtr++];
	tmp2 = ptab[m_AudioPtr++];
		
	mp4ASC.objectTypeIndex = (MMP_UBYTE)(tmp1 >> 3);
 	mp4ASC.samplingFrequencyIndex = ((tmp1 & 0x07) << 1) | (tmp2 >> 7);
	mp4ASC.channelsConfiguration = (tmp2 & 0x78) >> 3;
	// print out the result for debuging
	#if	0
		VAR_L(3,mp4ASC.objectTypeIndex);
		VAR_L(3,mp4ASC.channelsConfiguration);
	#endif

    mp4ASC.samplingFrequency = MMPF_3GPPSR_GetSampleRate(mp4ASC.samplingFrequencyIndex);
	mp4ASC.sample256_time = MMPF_3GPPSR_Get256SampleTime(mp4ASC.samplingFrequencyIndex);
	if (mp4ASC.samplingFrequency == 0) {
		return	MMP_VIDPSR_ERR_UNSUPPORTED_SETTING;
   	}
	
	if (mp4ASC.channelsConfiguration > 7 ) {
		return	MMP_VIDPSR_ERR_UNSUPPORTED_SETTING;
   	}

	if (mp4ASC.objectTypeIndex == 2) {
		mp4ASC.frameLengthFlag = (tmp2 & 0x04) >> 2;

		if (mp4ASC.frameLengthFlag == 1) {
	        return	MMP_VIDPSR_ERR_UNSUPPORTED_SETTING;
	    }    
	}
	else {
		return	MMP_VIDPSR_ERR_UNSUPPORTED_SETTING;
	}
	// print out the result for debuging
	#if	0
		VAR_L(3,mp4ASC.samplingFrequency);
	#endif
    return MMP_ERR_NONE;
}

/** @} */ // end of name Demuxer



#if 0 // for IDE function name list by section
void _____3GPPSR_APIS_____(){}
#endif


/** @brief Set the video input buffer addresses

The 2 parameters are given by the host.
It should be any place which is not used before fopen and play.
@param[in] ulVideoInAddr The starting address of the video in (compressed) buffer.
@param[in] ulLength The length of the file name.
*/
/*
void MMPF_3GPPSR_SetVideoInBuffers(const MMP_ULONG ulVideoInAddr, const MMP_ULONG ulLength,MMP_ULONG buffer_type)
{
    m_bVideoEOS         = MMP_FALSE;
    m_bAudioEOS         = MMP_FALSE;
    m_bsbuf_type        = buffer_type;
    m_VideoStreamBuf    = (MMP_UBYTE *) ulVideoInAddr;
    m_VideoStreamLen    = ulLength;
    
    if (m_bsbuf_type == MMP_MEDIA_NEAR_RING_BUFFER ) {
        m_support_large_frame = 0;
    }
    else if (m_bsbuf_type == MMP_MEDIA_RING_BUFFER ) {
        m_support_large_frame = 1;
    }
}
*/
/** @brief reset the parser variables

Reset the variable so that the file could be parsed again.
*/
MMP_ERR MMPF_3GPPSR_Init(void)
{
    MMP_ULONG ulTemp;
    
	MEMSET0(&gAudioSample);
	MEMSET0(&gVideoSample);
	MEMSET0(&m_seek);
	MEMSET0(&mp4ASC);
	///- Reset the media info
    MEMSET0(&gVideoMedia);
    MEMSET0(&gAudioMedia);
    MEMSET0(&m_3GPPdata);

    // Keep the file size for memory mode
    ulTemp = gVidFileInfo.common.file_size;
    
    MEMSET0(&gVidFileInfo);
    
    gVidFileInfo.common.file_size = ulTemp;
	
	m_seek.sSpeed               = 10; // default set to 1x speed
    //m_3GPPdata.bEmptyBufferDone = MMP_TRUE;
    //gl3gpAudioSize              = 0;
	gDemuxOption                = 0;
	
    return MMP_ERR_NONE;
}

void MMPF_3GPPSR_Reset(void)
{
    MEMSET0(&m_3GPPdata);
    m_bVideoEOS              = MMP_FALSE;
    m_bAudioEOS              = MMP_FALSE;
}

/** @brief Demux Audio/Video frame from the bitstream

Triggered by timer2.
Fullfill Audio buffer and Get one Video frame from the bitstream
@pre The caller should avoid playing video which is neither h.263 nor MP4VSP
@return The status of the function call
*/
MMP_ERR MMPF_3GPPSR_Demux(void)
{
    MMP_ERR                 errEOS = MMP_ERR_NONE;// If it's end of strem this time
    MMP_ERR                 error = MMP_ERR_NONE;
    MMP_ULONG               BackupRequestFrame,RequestFrame = 0;
    MMP_ULONG               SizeToLoad;
    MMP_ULONG               header_addr, frame_addr, avail_size,avail_size1,data_size;
    
    if (gbCardMode == MMP_FALSE) {
        return MMP_ERR_NONE;
    }
    
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
                        error = MMPF_3GPPSR_FillAudioBuffer(addr,avail_size, &transfer_size);
                        if((error != MMP_ERR_NONE) && (error != MMP_VIDPSR_ERR_EOS) ) {
                            return error;
                        }
                            
                        MMPF_BS_UpdateAudioBufptr(transfer_size,MMP_FALSE);
                    }
                    else {// not enough size to put all data, have to rewind
                        error = MMPF_3GPPSR_FillAudioBuffer(addr,avail_size, &transfer_size);
                        if((error != MMP_ERR_NONE) && (error != MMP_VIDPSR_ERR_EOS) ) {
                            return error;
                        }
                            
                        MMPF_BS_UpdateAudioBufptr(transfer_size,MMP_TRUE);

                        error = MMPF_3GPPSR_FillAudioBuffer(addr1,avail_size1, &transfer_size1);
                        if((error != MMP_ERR_NONE) && (error != MMP_VIDPSR_ERR_EOS) ) {
                            return error;
                        }
                            
                        MMPF_BS_UpdateAudioBufptr(transfer_size1,MMP_FALSE);
                        
                        transfer_size += transfer_size1;
                    }
                        
                    MMPD_BS_TransferAudioToDev((MMP_UBYTE*)glAudioPlayBufStart, transfer_size, &ulReturnSize);
                }
            }
        }
	}
	// ===== fill video data =====
	/*
	if (m_seek.mode == MMPF_M_SEEK_NONE) {
	    return MMP_ERR_NONE;
	}
	*/
	
	// ===== fill video data =====
    if ( gDemuxOption & DEMUX_VIDEO ) {
 	    gDemuxOption &= ~DEMUX_VIDEO;
        if (!m_3GPPdata.bFillBufferDone) {

            //if ((!m_3GPPdata.bFilling) && (!m_3GPPdata.bLargeFrame) && (!m_bVideoEOS))
            if ((!m_3GPPdata.bFilling) && (!m_3GPPdata.bLargeFrame))
            {   
                BackupRequestFrame = m_seek.ulRequestFrame;
                
                
                errEOS = MMPF_3GPPSR_SeekNextVideoFrame(&RequestFrame);
                
                if ((errEOS == MMP_VIDPSR_ERR_EOS) && (m_bVideoEOS)) {
                    return MMP_VIDPSR_ERR_EOS;
                }
                
                m_seek.ulRequestFrame = BackupRequestFrame;
                
                // ===== Time stamp =====
                WRITE_HEADER->ulRequestTime = MMPF_3GPPSR_GetTime(&gVideoMedia);
                if ((MMP_ERR_NONE == errEOS) || (MMP_VIDPSR_ERR_EOS == errEOS)) {
                    m_3GPPdata.bFilling = MMP_TRUE;
                }
                else {
                    DBG_S(3, " Seek Error\r\n");
                    return errEOS;
                }
            }
            
            if ( errEOS == MMP_VIDPSR_ERR_EOS) {
                m_bVideoEOS = MMP_TRUE;
            }
            
            data_size = gVideoSample.ulSize + m_3GPPdata.usBSOffset + m_3GPPdata.usEOFPadding;
            
            error = MMPF_BS_GetVideoStartAddr(&header_addr,
                                              &frame_addr,
                                              &avail_size,
                                              &avail_size1,
                                              m_3GPPdata.usBSOffset,
                                              data_size);
            
            if (error == MMP_ERR_NONE) {
                m_3GPPdata.bLargeFrame = MMP_FALSE;
            }
            else if (error == MMP_VIDPSR_ERR_INSUFFICIENT_RESOURCES) {
                if (m_support_large_frame) {
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
            
            m_seek.ulRequestFrame       = RequestFrame;
            //pTempHeader                 = m_PrevBSHeader;
            
            // ===== Update the final frame size =====
            if (m_3GPPdata.bLargeFrame) {
                //WRITE_HEADER->ulSize    = m_pBSHeader->length;
                WRITE_HEADER->ulSize    = avail_size + avail_size1;
            }
            else {
                WRITE_HEADER->ulSize    = data_size;
            }
            
            avail_size -= m_3GPPdata.usBSOffset;
            
            WRITE_HEADER->ulBSSize      = gVideoSample.ulSize;
        	WRITE_HEADER->ulAddress     = frame_addr;
        	WRITE_HEADER->ulFlag        = 0;


            if (gVideoSample.ulSize < (avail_size+avail_size1)) {
                SizeToLoad = gVideoSample.ulSize;
                WRITE_HEADER->ulFlag |= BUFFERFLAG_EOF;
                
                if(m_bVideoEOS) {
        	        WRITE_HEADER->ulFlag |= BUFFERFLAG_EOS;
        	    }
            }
            else {
                SizeToLoad = (avail_size+avail_size1);
            }
            
            gVideoSample.ulSize -= SizeToLoad;
            
            // copy the write header into the bitstream
            MMPF_BS_AccessVideoHeader((MMP_UBYTE*)header_addr, (MMP_UBYTE*)WRITE_HEADER, sizeof(MMPF_M_BUFFER_HEADER),RING_WRITE);
            
            // Put single VOP for video decoder
            error = MMPF_3GPPSR_GetSingleFrame(WRITE_HEADER->ulAddress,SizeToLoad,avail_size,avail_size1,&(gVideoSample.ulOffset));

            if (error != MMP_ERR_NONE) {
                m_3GPPdata.bFilling         = MMP_FALSE;
                m_3GPPdata.bFillBufferDone  = MMP_FALSE;
            
                return error;
            }
            
            m_3GPPdata.bFillBufferDone = MMP_TRUE;
            
            // sync data to the AIT chip
            error = MMPF_BS_TransferVideoToDev(WRITE_HEADER,(MMP_UBYTE*)header_addr);
            
            MMPF_BS_UpdateVideoBufPtr(RING_WRITE, WRITE_HEADER->ulSize);
            
            m_3GPPdata.bFilling         = MMP_FALSE;
            m_3GPPdata.bFillBufferDone  = MMP_FALSE;
            
            
            
            //m_3GPPdata.pPreWriteHeader  = (MMPF_M_BUFFER_HEADER*)(header_addr);
            m_3GPPdata.pPreWriteHeader.ulRequestTime  = ((MMPF_M_BUFFER_HEADER*)header_addr)->ulRequestTime;
        } // ! fill buffer done
    }

    return  MMP_ERR_NONE;
}



/** @brief Parse 3GP information

@return The validation of the file
*///1600 bytes when DGB_LEVEL == 0 1560, 1526, save 74 bytes
MMP_VID_FILEVALIDATION MMPF_3GPPSR_Parse(void)
{
    MMP_3GP_MEDIA_INFO      *MediaInfo = NULL;
    MMPF_M_MP4V_CONFIG      mp4v_config;
    MMP_VID_FILEVALIDATION  filevalidation  = MMP_VID_FILE_VALID;
    MMPF_3GP_TRACK          parsingTrack    = MMPF_3GP_TRACK_UNDEFINED;
    MMP_ULONG               i;
    MMP_ULONG               length = 0, tmp_length = 0;
    MMP_ULONG               atomname;
    MMP_ULONG               tmp,value;
    MMP_ULONG               tmp_total_size ;
    MMP_ULONG               timescale = 0;
    MMP_ULONG               TrackDuration = 0;
    MMP_UBYTE               mdhd_found = 0;
    MMP_UBYTE               stsd_found = 0;
    MMP_ULONG               trackID = 0;
    MMP_UBYTE               *dest_buf = m_VideoStreamBuf;
    MMP_ERR                 error = MMP_ERR_NONE;
    
    MEMSET0(&mp4v_config);
    
	///- Reset the parser states
    (void)MMPF_3GPPSR_Init();
    
    #if (VIDEO_P_EN)
    MMPF_Player_GetSupportedFormat(&m_3GPPdata.supported_audio_format,
                                   &m_3GPPdata.supported_video_format);
    #else
    m_3GPPdata.supported_audio_format = m_supported_audio_format;
    m_3GPPdata.supported_video_format = m_supported_video_format;
    #endif
    
    if (MMP_VID_FILE_VALID != MMPF_PSR_ResetFS()) {
	    return MMP_VID_INVALID_FILE_NAME;
	}
	
	tmp_total_size = gVidFileInfo.common.file_size;

//	MMPF_3GPPSR_ResetAudioBuf();
    preAudioOffset      = 0;
	preAudioReduntSize  = 0;
	
    MMPF_VIDPSR_ResetCache();

    ulCurOffset = 0;
    m_AudioPtr  = 0;
    
	///- Parse the file header
	error      = MMPF_3GPPSR_GetBigLong(ulCurOffset, 4, &length);
	if (error != MMP_ERR_NONE)
	    return error;
	error      = MMPF_3GPPSR_GetBigLong(ulCurOffset, 0, &atomname);
	if (error != MMP_ERR_NONE)
	    return error;    
    //length      = MMPF_3GPPSR_GetBigLong(ulCurOffset, 4);
    //atomname    = MMPF_3GPPSR_GetBigLong(ulCurOffset, 0);

	// file header
    if ((atomname != ATOM_FTYP) && (atomname != ATOM_MOOV)) { //ftyp, QuickTime Prefs file types
        if ((length + 8) < gVidFileInfo.common.file_size) {
            error = MMPF_3GPPSR_GetBigLong(length + 4, 0, &atomname);
            if (error != MMP_ERR_NONE)
                return error;
            //VAR_L(3, atomname);
        }
        /*
        if (atomname != ATOM_FTYP) {
            //DBG_S(3, "Not 3GP\r\n");
            DBG_S(3,"Not FTYP\r\n");
            return MMP_VID_UNSUPPORTED_FILE;
        }
        */
    }

    if(atomname != ATOM_MOOV){
        ulCurOffset = length;
    } else {
        ulCurOffset += 4;
    }
    
    
    
	///- Parse the 3GP file body
    while (ulCurOffset < tmp_total_size) {
        error      = MMPF_3GPPSR_GetBigLong(ulCurOffset, 4, &length);
        if ( error != MMP_ERR_NONE )
            return error;
        error      = MMPF_3GPPSR_GetBigLong(ulCurOffset, 4, &atomname);
        if ( error != MMP_ERR_NONE )
            return error;
                
        //length      = MMPF_3GPPSR_GetBigLong(ulCurOffset, 4);
        //atomname    = MMPF_3GPPSR_GetBigLong(ulCurOffset, 4);

        if (length == 0) {
            break;
        }

        switch (atomname) {
        //"ftyp" QuickTime Prefs file types [P:NULL, C:NULL]
        case ATOM_FTYP:
        //"mdat" Media data atom [P:NULL, C:Chunk]
        case ATOM_MDAT:
        //"iods" 
        case ATOM_IODS:
        //"vmhd" video Media Info Atom. [P:minf, C:NULL]
        case ATOM_VMHD:
        //"smhd"
        case ATOM_SMHD:
        //"dinf" Data Info atom. [P:minf, C:dref]
        case ATOM_DINF:
            ulCurOffset += length - 8;
            break;
        //"mvhd" Movie header atom [P:moov, C:NULL]
        case ATOM_MVHD:
            // time scale in mvhd
            error = MMPF_3GPPSR_GetBigLong(ulCurOffset + 12, 0, &(gVidFileInfo.common.total_file_time) );
            if (error != MMP_ERR_NONE)
                return error;
            error = MMPF_3GPPSR_GetBigLong(ulCurOffset + 16, length - 8,&value);
            if (error != MMP_ERR_NONE)
                return error;
                
            gVidFileInfo.common.total_file_time = ((MMP_3GP_TICK)value) * 1000 / gVidFileInfo.common.total_file_time;
                
            //gVidFileInfo.common.total_file_time = MMPF_3GPPSR_GetBigLong(ulCurOffset + 12, 0);
			// duration * 1000 / time scale
            //gVidFileInfo.common.total_file_time = 
            //    ((MMP_3GP_TICK)MMPF_3GPPSR_GetBigLong(ulCurOffset + 16, length - 8)) * 1000 /
            //                                       gVidFileInfo.common.total_file_time;
            break;
        //"moov" Movie Atom [P:NULL, C:mvhd, trak]
        case ATOM_MOOV:
        //"mdia" Media Atom [P:trak, C:mdhd, hdlr, minf]    
        case ATOM_MDIA:
            break;
        //"trak" Track Atom. [P:moov, C:tkhd, mdia]
        case ATOM_TRAK:
            mdhd_found      = 0;
            stsd_found      = 0;
            parsingTrack    = MMPF_3GP_TRACK_UNDEFINED;
            break;
        //"tkhd" Track header Atom. [P:trak, C:NULL]
        case ATOM_TKHD:
            error = MMPF_3GPPSR_GetBigLong(ulCurOffset + 12, length - 8, &trackID);
            if(error != MMP_ERR_NONE)
                return error;
            //trackID = MMPF_3GPPSR_GetBigLong(ulCurOffset + 12, length - 8);
            break;
        //"minf" Media Info. [P:mdia C:vmhd, dinf, stbl]
        case ATOM_MINF:
            if (parsingTrack == MMPF_3GP_TRACK_UNDEFINED) {
                error = MMPF_3GPPSR_GetBigLong(ulCurOffset + length + 8, 0, &tmp);
                if(error != MMP_ERR_NONE)
                    return error;
                switch (tmp) {
                case ATOM_SOUN:
                    //"soun"
                    gVidFileInfo.common.is_audio_available  = MMP_TRUE;
                    parsingTrack                            = MMPF_3GP_TRACK_AUDIO;
                    MediaInfo                               = &gAudioMedia;
                    MediaInfo->ulTrackID                    = trackID;
                    break;
                case ATOM_VIDE:
                    //"vide"
                    gVidFileInfo.common.is_video_available  = MMP_TRUE;
                    parsingTrack                            = MMPF_3GP_TRACK_VIDEO;
                    MediaInfo                               = &gVideoMedia;
                    MediaInfo->ulTrackID                    = trackID;
                    break;
                default:
                    parsingTrack                            = MMPF_3GP_TRACK_UNDEFINED;
                    break;
                }
            }
            break;
            
        //"mdhd" Media Header Atom. [P:mdia C:NULL]
        case ATOM_MDHD:
            mdhd_found      = 1;
            error           = MMPF_3GPPSR_GetBigLong(ulCurOffset + 12, 4 + 12, &timescale);
            if(error != MMP_ERR_NONE)
                return error;
            error           = MMPF_3GPPSR_GetBigLong(ulCurOffset, length - 16 - 8, &TrackDuration);    
            if(error != MMP_ERR_NONE)
                return error;

            if (stsd_found) {
                MediaInfo->ulTimeScale = timescale;
                
                if (parsingTrack == MMPF_3GP_TRACK_AUDIO) {
                    gVidFileInfo.common.audio_info.total_audio_time = TrackDuration;
                }
                else if (parsingTrack == MMPF_3GP_TRACK_VIDEO) {
                    gVidFileInfo.common.video_info.total_video_time = TrackDuration;
                }
            }
            break;
            
        //"hdlr" Media Handler Reference Atom. [P:mdia C:NULL]
        case ATOM_HDLR:
            // Parsing Audio or Video Format
            error = MMPF_3GPPSR_GetBigLong(ulCurOffset + 8, 4 + 8, &tmp);
            if(error != MMP_ERR_NONE)
                return error;
                
            switch (tmp) {
            case ATOM_SOUN:
                //"soun" Sound header
                gVidFileInfo.common.is_audio_available  = MMP_TRUE;
                parsingTrack                            = MMPF_3GP_TRACK_AUDIO;
                MediaInfo                               = &gAudioMedia;
                MediaInfo->ulTrackID                    = trackID;
                break;
            case ATOM_VIDE:
                //"vide" Video header
                gVidFileInfo.common.is_video_available  = MMP_TRUE;
                parsingTrack                            = MMPF_3GP_TRACK_VIDEO;
                MediaInfo                               = &gVideoMedia;
                MediaInfo->ulTrackID                    = trackID;
                break;
            default:
                parsingTrack                            = MMPF_3GP_TRACK_UNDEFINED;
                break;
            }
            ulCurOffset += length - 12 - 8;
            break;
            
        //"stbl" Sample Table Atom [P:minf C:stsd, stts, stsc, stsz, stco, stss]
        case ATOM_STBL:
            #if 0
            if (parsingTrack == MMPF_3GP_TRACK_AUDIO) {
                //AudioTrakOffset = ulCurOffset;
            }
            else if (parsingTrack == MMPF_3GP_TRACK_VIDEO) {
                //VideoTrakOffset = ulCurOffset;
            }
            #endif
            break;
            
        //"stsd" Sample description atom [P:stbl, C:NULL]
        case ATOM_STSD:
            stsd_found = 1;
            
            if (parsingTrack == MMPF_3GP_TRACK_AUDIO) {
                if (mdhd_found) {
                    MediaInfo->ulTimeScale                          = timescale;
                    gVidFileInfo.common.audio_info.total_audio_time = TrackDuration;
                }
                error = MMPF_3GPPSR_GetBigLong(ulCurOffset + 12, 4 + 12, &tmp);
                if(error != MMP_ERR_NONE)
                    return error;

                //tmp = MMPF_3GPPSR_GetBigLong(ulCurOffset + 12, 4 + 12);
                
                if (tmp == CODEC_MP4A) {// "mp4a"
                    gVidFileInfo.common.audio_info.format = MMP_VID_AUDIO_AAC_LC;
                }
                else if (tmp == CODEC_AMR) {//"samr"
                    gVidFileInfo.common.audio_info.format = MMP_VID_AUDIO_AMR_NB;
                }
                else {
                    gVidFileInfo.common.audio_info.format = MMP_VID_AUDIO_UNSUPPORTED;
                }

                ///- Return MMP_VID_NEED_TO_CHANGE_FIRMWARE with supported audio format
                
#if 0
                if (m_3GPPdata.supported_audio_format != gVidFileInfo.common.audio_info.format) {
                    filevalidation = MMP_VID_NEED_TO_CHANGE_FIRMWARE;
                }
#endif

                if (gVidFileInfo.common.audio_info.format == MMP_VID_AUDIO_AAC_LC) {
                    error = MMPF_3GPPSR_GetBigLong(ulCurOffset + 28, 8 + 28, &tmp_length);
                    if(error != MMP_ERR_NONE)
                        return error;
                        
                    //tmp_length = MMPF_3GPPSR_GetBigLong(ulCurOffset + 28, 8 + 28);

                    error = MMPF_VIDPSR_ReadFile(m_ASCInfo, ulCurOffset, 0x30,0);
                    if(error != MMP_ERR_NONE)
                        return error;
                        
					MMPF_3GPPSR_DecodeMP4ASC();
                    ulCurOffset += length - 36 - 24;
                }
                #if 0
                else if (gVidFileInfo.common.audio_info.format == MMP_VID_AUDIO_AMR_NB) {
                    ulCurOffset += length - 24;
                }
                #endif
                else {
                    ulCurOffset += length - 24;
                }
            }
            else if (parsingTrack == MMPF_3GP_TRACK_VIDEO) {
                if (mdhd_found) {
                    MediaInfo->ulTimeScale                          = timescale;
                    gVidFileInfo.common.video_info.total_video_time = TrackDuration;
                }
                error = MMPF_3GPPSR_GetBigLong(ulCurOffset + 12, 4 + 12, &tmp);
                if(error != MMP_ERR_NONE)
                    return error;

                //tmp = MMPF_3GPPSR_GetBigLong(ulCurOffset + 12, 4 + 12);
                if (tmp == CODEC_H263) { //s263
                    //DBG_S(1, "H263 ");
                    gVidFileInfo.common.video_info.format   = MMP_VID_VIDEO_H263;
                    mp4v_config.usHWConfig                  |= VLD_H263_SHORT_HDR;
                    
                    if( (m_3GPPdata.supported_video_format & gVidFileInfo.common.video_info.format) == 0 )
                    {
                        return MMP_VID_UNSUPPORTED_FILE;
                    }
                    
                    /*
                    if( (m_3GPPdata.supported_video_format != gVidFileInfo.common.video_info.format) &&
                       (m_3GPPdata.supported_video_format != MMP_VID_VIDEO_MP4V) )
                    {
                        return MMP_VID_FILE_VALID;
                    }
                    */
                    /*
                    #if (VIDEO_DECODER == SW_H264) || (VIDEO_DECODER == HW_H264)
                    return MMP_ERR_NONE;
                    #endif
                    */
                }
                else if (tmp == CODEC_MP4V) { //mp4v
                    //DBG_S(1, "MP4V ");
                    gVidFileInfo.common.video_info.format   = MMP_VID_VIDEO_MP4V_SP;
                    
                    if ( (m_3GPPdata.supported_video_format & gVidFileInfo.common.video_info.format) == 0 )
                    {
                        return MMP_VID_UNSUPPORTED_FILE;
                    }
                    /*
                    #if (VIDEO_DECODER == SW_H264) || (VIDEO_DECODER == HW_H264)
                    return MMP_ERR_NONE;
                    #endif
                    */
                    
                }
                else if (tmp == CODEC_H264) { //avc1
                    //DBG_S(1, "AVC ");
                    gVidFileInfo.common.video_info.format   = MMP_VID_VIDEO_H264;
                    
                    #if (H264_DEC_SUPPORT == 1)
                    if ( (m_3GPPdata.supported_video_format & gVidFileInfo.common.video_info.format) == 0 )
                    {
                        return MMP_VID_UNSUPPORTED_FILE;
                    }
                    #else
                        DBG_S(3,"NOT SUPPORT!\r\n");
                        return MMP_VID_UNSUPPORTED_FILE;
                    #endif
                    /*
                    #if (VIDEO_DECODER == SW_MP4V) || (VIDEO_DECODER == HW_MP4V)
                    return MMP_ERR_NONE;
                    #endif
                    */
                }
                else {
                    DBG_S(1, "Other format\r\n");
                    gVidFileInfo.common.video_info.format   = MMP_VID_VIDEO_UNSUPPORTED;
                    
                    return MMP_VID_UNSUPPORTED_VIDEO;
                }
                error = MMPF_3GPPSR_GetBigLong(ulCurOffset + 24, 4 + 24, &tmp);
                if(error != MMP_ERR_NONE)
                    return error;

                //tmp = MMPF_3GPPSR_GetBigLong(ulCurOffset + 24, 4 + 24);
                
                gVidFileInfo.common.video_info.width    = (unsigned short)(tmp >> 16);
                gVidFileInfo.common.video_info.height   = (unsigned short) tmp;

				// parse MP4V optional description atom 'esds'
                if (gVidFileInfo.common.video_info.format == MMP_VID_VIDEO_MP4V_SP) {
                    error = MMPF_3GPPSR_GetBigLong(ulCurOffset + 50, 4 + 4 + 50, &tmp_length);// size + "avcC"
                    if(error != MMP_ERR_NONE)
                        return error;
                    
                    error = MMPF_VIDPSR_ReadFile(dest_buf, ulCurOffset, tmp_length,0);
                    if(error != MMP_ERR_NONE)
                        return error;
                    
                    gVidFileInfo.common.video_info.width  = 0;
                    gVidFileInfo.common.video_info.height = 0;
                
                    #if (VIDEO_P_EN)
                    {
                        extern MMP_HANDLE          *hVideo;
                        extern MMPF_VIDEO_FUNC     m_vcodec;
                        
                        hVideo = MMPF_VIDEO_Open(CODEC_MP4V, &m_vcodec);

                        if (MMP_ERR_NONE != 
                            m_vcodec.MMPF_VIDDEC_Init(hVideo,dest_buf, tmp_length,
                                                   gVidFileInfo.common.video_info.width,
                                                   gVidFileInfo.common.video_info.height,MMP_TRUE)) {
                                                  
                            DBG_S(3,"Init video failed\r\n");
                            return MMP_VID_UNSUPPORTED_FILE;
                        }
                        m_vcodec.MMPF_VIDDEC_GetInfo(hVideo,&gVidFileInfo.common.video_info);
                    }
                    #else //++Old implementation
                    if (MMP_ERR_NONE != MMPD_VIDPLAYER_InitVideoDec(
                                                m_video_dest_addr,
                                                dest_buf, 
                                                tmp_length,
                                               &gVidFileInfo.common.video_info.width,
                                               &gVidFileInfo.common.video_info.height,
                                               &gVidFileInfo.common.video_info.cropping_left, 
                                               &gVidFileInfo.common.video_info.cropping_top,
                                               &gVidFileInfo.common.video_info.cropping_right,
                                               &gVidFileInfo.common.video_info.cropping_bottom,
                                               MMP_VID_VIDEO_MP4V_SP)) {
                        
                        return MMP_VID_UNSUPPORTED_FILE;
                    }
                    #endif //--Old implementation
                    
                    ulCurOffset += length - 102 - 8;
                }
//#if (VIDEO_DECODER == SW_H264) || (VIDEO_DECODER == HW_H264)
#if (CHIP == D_V1)||(CHIP == PYTHON)
//#if 1
                else if (gVidFileInfo.common.video_info.format == MMP_VID_VIDEO_H264) {
                    error = MMPF_3GPPSR_GetBigLong(ulCurOffset + 50, 4 + 4 + 50, &tmp_length);// size + "avcC"
                    if(error != MMP_ERR_NONE)
                        return error;
                        
                    //tmp_length = MMPF_3GPPSR_GetBigLong(ulCurOffset + 50, 4 + 4 + 50);// size + "avcC"
                    
                    error = MMPF_VIDPSR_ReadFile(dest_buf, ulCurOffset, tmp_length,0);
                    if(error != MMP_ERR_NONE)
                        return error;
                    
                    #if (H264_DEC_SUPPORT == 1)
                    #if (VIDEO_P_EN)
                    {
                        extern MMP_HANDLE          *hVideo;
                        extern MMPF_VIDEO_FUNC     m_vcodec;
                        
                        hVideo = MMPF_VIDEO_Open(CODEC_H264, &m_vcodec);
                        
                        if (MMP_ERR_NONE !=
                            m_vcodec.MMPF_VIDDEC_Init(hVideo,dest_buf, tmp_length, (MMP_ULONG)m_VideoStreamBuf, m_VideoStreamLen,MMP_TRUE)) {
                            return MMP_VID_UNSUPPORTED_VIDEO;
                        }
                        
                        m_vcodec.MMPF_VIDDEC_GetInfo(hVideo,&gVidFileInfo.common.video_info);
                    }
                    #else
                    gVidFileInfo.common.video_info.width    = 0;
                    gVidFileInfo.common.video_info.height   = 0;
                    
                    if (MMP_ERR_NONE != MMPD_VIDPLAYER_InitVideoDec(
                                                m_video_dest_addr,
                                               dest_buf, 
                                               tmp_length,
                                               &gVidFileInfo.common.video_info.width,
                                               &gVidFileInfo.common.video_info.height,
                                               &gVidFileInfo.common.video_info.cropping_left, 
                                               &gVidFileInfo.common.video_info.cropping_top,
                                               &gVidFileInfo.common.video_info.cropping_right,
                                               &gVidFileInfo.common.video_info.cropping_bottom,
                                               MMP_VID_VIDEO_H264)) {
                        
                        return MMP_VID_UNSUPPORTED_FILE;
                    }
                    #endif
                    #endif
                    
                    gVidFileInfo.common.video_info.width =  gVidFileInfo.common.video_info.width - 
                                                            gVidFileInfo.common.video_info.cropping_left -
                                                            gVidFileInfo.common.video_info.cropping_right;
                                                            
                    gVidFileInfo.common.video_info.height = gVidFileInfo.common.video_info.height - 
                                                            gVidFileInfo.common.video_info.cropping_top -
                                                            gVidFileInfo.common.video_info.cropping_bottom;
                    ulCurOffset += length - 102 - 8;//ulCurOffset = tmp_offset + length - 52;
                    
                }
#endif
                else {
                    ulCurOffset += length - 44 - 8;
                    
                    // Set the macro blocks here so that the config won't change when playing
                    mp4v_config.usHMB = (gVidFileInfo.common.video_info.width + 0xF) >> 4;
                    mp4v_config.usVMB = (gVidFileInfo.common.video_info.height + 0xF) >> 4;
                    
                    #if (VIDEO_P_EN)
                        {
                        extern MMP_HANDLE          *hVideo;
                        extern MMPF_VIDEO_FUNC     m_vcodec;
                        MMP_USHORT twidth   = mp4v_config.usHMB * 16;
                        MMP_USHORT theight  = mp4v_config.usVMB * 16;
                        
                        hVideo = MMPF_VIDEO_Open(CODEC_H263, &m_vcodec);
                        if (MMP_ERR_NONE !=
                            m_vcodec.MMPF_VIDDEC_Init(hVideo,dest_buf, tmp_length, twidth, theight,MMP_TRUE)) {
                            DBG_S(3,"Init 263 failed\r\n");
                            return MMP_VID_UNSUPPORTED_VIDEO;
                        }
                        m_vcodec.MMPF_VIDDEC_GetInfo(hVideo,&gVidFileInfo.common.video_info);
                        }
                    #else
                        //MMPF_MP4V_SetConfig(&mp4v_config);
                        gVidFileInfo.common.video_info.width  = mp4v_config.usHMB * 16;
                        gVidFileInfo.common.video_info.height = mp4v_config.usVMB * 16;
                        
                        MMPD_VIDPLAYER_InitVideoDec(m_video_dest_addr,dest_buf, tmp_length,
                                               &gVidFileInfo.common.video_info.width,
                                               &gVidFileInfo.common.video_info.height,
                                               &gVidFileInfo.common.video_info.cropping_left, 
                                               &gVidFileInfo.common.video_info.cropping_top,
                                               &gVidFileInfo.common.video_info.cropping_right,
                                               &gVidFileInfo.common.video_info.cropping_bottom,
                                               MMP_VID_VIDEO_H263);
                        
                                               
                    #endif
                }
            }
            else {
                ulCurOffset += length - 8;
            }
            break;
            
        //"stts" Time to sample atom [P:stbl, C:NULL]
        case ATOM_STTS:
            if (parsingTrack == MMPF_3GP_TRACK_AUDIO || parsingTrack == MMPF_3GP_TRACK_VIDEO) {
                error = MMPF_3GPPSR_GetBigLong(ulCurOffset + 4, 4 + 4, &length);
                if (error != MMP_ERR_NONE)
                    return error;
                    
                //length = MMPF_3GPPSR_GetBigLong(ulCurOffset + 4, 4 + 4);
                MediaInfo->ulSTOffset[MMPF_3GP_ATOM_STTS] = ulCurOffset;
                ulCurOffset += 8 * length;
            }
            else {
                ulCurOffset += length - 8;
            }
            break;
        case ATOM_CTTS:
            if (parsingTrack == MMPF_3GP_TRACK_AUDIO || parsingTrack == MMPF_3GP_TRACK_VIDEO) {

                error = MMPF_3GPPSR_GetBigLong(ulCurOffset + 4, 4 + 4, &length);
                if (error != MMP_ERR_NONE)
                    return error;
                MediaInfo->ulSTOffset[MMPF_3GP_ATOM_CTTS] = ulCurOffset;
                MediaInfo->bCTTSEnable = MMP_TRUE;
                ulCurOffset += 8 * length;
            }
            else {
                ulCurOffset += length - 8;
            }
            break;
        
        //"stsc" Sample to chunk atom [P:stbl, C:NULL]
        case ATOM_STSC:
            if (parsingTrack == MMPF_3GP_TRACK_AUDIO || parsingTrack == MMPF_3GP_TRACK_VIDEO) {
                error = MMPF_3GPPSR_GetBigLong(ulCurOffset + 4, 4 + 4, &MediaInfo->ulSTSCCount);
                if(error != MMP_ERR_NONE)
                    return error;
                    
                //MediaInfo->ulSTSCCount = MMPF_3GPPSR_GetBigLong(ulCurOffset + 4, 4 + 4);
                MediaInfo->ulSTOffset[MMPF_3GP_ATOM_STSC] = ulCurOffset;
                ulCurOffset += length - 16;
            }
            else {
                ulCurOffset += length - 8;
            }
            break;
            
        //"stsz" Sample size atom [P:stbl, C:NULL]
        case ATOM_STSZ:
            if (parsingTrack == MMPF_3GP_TRACK_AUDIO || parsingTrack == MMPF_3GP_TRACK_VIDEO) {
                error                     = MMPF_3GPPSR_GetBigLong(ulCurOffset + 4, 4 + 4, (MMP_ULONG*)&(MediaInfo->usSampleSize));
                if(error != MMP_ERR_NONE)
                    return error;
                error                     = MMPF_3GPPSR_GetBigLong(ulCurOffset, 4,&(MediaInfo->ulFrameCount));
                if(error != MMP_ERR_NONE)
                    return error;
                //MediaInfo->usSampleSize                     = MMPF_3GPPSR_GetBigLong(ulCurOffset + 4, 4 + 4);
                //MediaInfo->ulFrameCount                     = MMPF_3GPPSR_GetBigLong(ulCurOffset, 4);
                MediaInfo->ulSTSZCount                      = MediaInfo->ulFrameCount;
                MediaInfo->ulSTOffset[MMPF_3GP_ATOM_STSZ]   = ulCurOffset;
                
                // calculate bitrate and size
        		if (MediaInfo->usSampleSize) {
        			MediaInfo->ulLength = MediaInfo->usSampleSize * MediaInfo->ulFrameCount;
        		}
        		else {
        		#if 1  // for large file
        		   	MediaInfo->ulLength = 0;
        			for (i = 0; i < MediaInfo->ulFrameCount; i++) {
                        MMP_UBYTE ptr[4];
                        error = MMPF_VIDPSR_ReadFile(ptr, MediaInfo->ulSTOffset[MMPF_3GP_ATOM_STSZ]+4*i, 4,0);
                        if(error != MMP_ERR_NONE)
                            return error;
                        
        				MediaInfo->ulLength += MMPF_3GPPSR_Ptr2Int(ptr);
        			}
        	    #else
        	        MediaInfo->ulLength = gVidFileInfo.common.file_size;
        	    
        	    #endif
        		}
                ulCurOffset += length - 20;
            }
            else {
                ulCurOffset += length - 8;
            }
//            gVidFileInfo.frameCount = gVideoInfo.frameCount;
            break;
            
        //"stco"  Chunk offset atom [P:stbl, C:NULL]
        case ATOM_STCO:
            if (parsingTrack == MMPF_3GP_TRACK_AUDIO || parsingTrack == MMPF_3GP_TRACK_VIDEO) {
                error                       = MMPF_3GPPSR_GetBigLong(ulCurOffset + 4, 4 + 4, (MMP_ULONG*)&(MediaInfo->lSTCOCount));
                if(error != MMP_ERR_NONE)
                    return error;
                //MediaInfo->lSTCOCount                       = MMPF_3GPPSR_GetBigLong(ulCurOffset + 4, 4 + 4);
                MediaInfo->ulSTOffset[MMPF_3GP_ATOM_STCO]   = ulCurOffset;
                ulCurOffset += length - 16;
            }
            else {
                ulCurOffset += length - 8;
            }
            break;
        case ATOM_STSS:
            //"stss" Sync Sample atom [P:stbl, C:NULL] contains video keyframe
            if (parsingTrack == MMPF_3GP_TRACK_AUDIO || parsingTrack == MMPF_3GP_TRACK_VIDEO) {
                error  = MMPF_3GPPSR_GetBigLong(ulCurOffset + 4, 4 + 4, &MediaInfo->ulSTSSCount);
                if(error != MMP_ERR_NONE)
                    return error;
                    
                MediaInfo->ulSTOffset[MMPF_3GP_ATOM_STSS]   = ulCurOffset;
                ulCurOffset += length - 16;
                
                if(parsingTrack == MMPF_3GP_TRACK_VIDEO)
                    gVidFileInfo.common.video_info.total_keyframe_num = MediaInfo->ulSTSSCount;
            }
            else {
                ulCurOffset += length - 8;
            }
            break;
        default:
            ulCurOffset += length - 8;
            break;
        }
    }


    // calculate video FPS, bitrate and total time
    gVidFileInfo.common.video_info.fps = 0;
    
    if (gVidFileInfo.common.is_video_available) {
        if ((gVideoMedia.ulSTOffset[MMPF_3GP_ATOM_STSC] || gVideoMedia.ulSTOffset[MMPF_3GP_ATOM_STCO] || 
             gVideoMedia.ulSTOffset[MMPF_3GP_ATOM_STSZ] || gVideoMedia.ulSTOffset[MMPF_3GP_ATOM_STTS]) == 0) {
            
            gVidFileInfo.common.is_video_available = 0;
        }
    }
    
    if (gVidFileInfo.common.is_video_available) {
        
        if ( gVideoMedia.ulTimeScale == 0 ) {
            return MMP_VID_UNSUPPORTED_FILE_PARAMETERS;
        }
        
        
        
        gVidFileInfo.common.video_info.total_video_time = 
                gVidFileInfo.common.video_info.total_video_time / gVideoMedia.ulTimeScale;
         
        if(gVidFileInfo.common.video_info.total_video_time != 0)
        {       
            gVidFileInfo.common.video_info.bitrate          = 
                    (gVideoMedia.ulLength / gVidFileInfo.common.video_info.total_video_time) * 8;
                    
            gVidFileInfo.common.video_info.fps              = 
                    gVideoMedia.ulFrameCount / gVidFileInfo.common.video_info.total_video_time;
        }
        else {
            gVidFileInfo.common.video_info.bitrate = 0;
            gVidFileInfo.common.video_info.fps     = 0;
        }
    }

    if (gVidFileInfo.common.is_video_available == 0 && gVidFileInfo.common.is_audio_available == 0) {
        DBG_S(3,"NO AV track!\r\n");
        return  MMP_VID_NO_AV_TRACK;
    }

    // calculate audio sampling rate, bitrate and total time
    gVidFileInfo.common.audio_info.sampling_rate = 0;
    
    if (gVidFileInfo.common.is_audio_available) {
        if (gVidFileInfo.common.audio_info.format == MMP_VID_AUDIO_AAC_LC) {
            if (mp4ASC.samplingFrequency == 0) {
                mp4ASC.samplingFrequency = gAudioMedia.ulTimeScale;
            }
            gVidFileInfo.common.audio_info.sampling_rate = mp4ASC.samplingFrequency;
        }
        else if (gVidFileInfo.common.audio_info.format == MMP_VID_AUDIO_AMR_NB) {
            gVidFileInfo.common.audio_info.sampling_rate = 8000;
        }
        
        if ((gAudioMedia.ulSTOffset[MMPF_3GP_ATOM_STSC] && gAudioMedia.ulSTOffset[MMPF_3GP_ATOM_STCO] && 
             gAudioMedia.ulSTOffset[MMPF_3GP_ATOM_STSZ]) == 0) {
            return MMP_VID_UNSUPPORTED_AUDIO;
        }
        
        //gl3gpAudioSize = 0xFFFFFFFF;
        //MMPF_SetAudioStreamLength(0xFFFFFFFF);
        
        if ((gAudioMedia.ulTimeScale == 0) || (gVidFileInfo.common.audio_info.total_audio_time == 0)) {
            return MMP_VID_UNSUPPORTED_FILE_PARAMETERS;
        }
        
            
        gVidFileInfo.common.audio_info.total_audio_time /= gAudioMedia.ulTimeScale;
        
        
        if(gVidFileInfo.common.audio_info.total_audio_time != 0)
        {
            gVidFileInfo.common.audio_info.bitrate = (gAudioMedia.ulLength / 
                                                      gVidFileInfo.common.audio_info.total_audio_time) * 8;
        }
        else {
            gVidFileInfo.common.audio_info.bitrate = 0;
            //gVidFileInfo.common.is_audio_available = 0;
        }
        
        m_ulSampleRate = gVidFileInfo.common.audio_info.sampling_rate;
    }
    
    
    return filevalidation;
}



#if 0 // for IDE function name list by section
void _____ParserAPIs_____(){}
#endif



/** @brief Set the video input buffer addresses

The 2 parameters are given by the host.
It should be any place which is not used before fopen and play.
@param[in] ulVideoInAddr The starting address of the video in (compressed) buffer.
@param[in] ulLength The length of the file name.
*/
void MMPF_PSR_SetVideoInBuffers(const MMP_ULONG ulVideoInAddr, const MMP_ULONG ulLength,MMP_ULONG buffer_type)
{
    m_bVideoEOS         = MMP_FALSE;
    m_bAudioEOS         = MMP_FALSE;
    m_bsbuf_type        = buffer_type;
    m_VideoStreamBuf    = (MMP_UBYTE *) ulVideoInAddr;
    m_VideoStreamLen    = ulLength;
    
    if (m_bsbuf_type == MMP_MEDIA_NEAR_RING_BUFFER ) {
        m_support_large_frame = 0;
    }
    else if (m_bsbuf_type == MMP_MEDIA_RING_BUFFER ) {
        m_support_large_frame = 1;
    }
}

void MMPF_PSR_Reset(void)
{
    MEMSET0(&m_3GPPdata);
    m_bVideoEOS              = MMP_FALSE;
    m_bAudioEOS              = MMP_FALSE;
}


#if 0
/** @} */ // end of group MMPF_3GPP
#endif
#endif
#endif

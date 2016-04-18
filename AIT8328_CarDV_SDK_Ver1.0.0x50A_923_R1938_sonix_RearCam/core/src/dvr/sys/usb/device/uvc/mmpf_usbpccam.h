#ifndef MMPF_USBPCCAM_H
#define MMPF_USBPCCAM_H



typedef enum _H264_FORMAT_TYPE {
    INVALID_H264 = 0,
    FRAMEBASE_H264 ,
    UVC_H264,
    SKYPE_H264,
    FRAMEBASE_H264_YUY2,
    FRAMEBASE_H264_MJPEG,
    FRAMEBASE_H264_Y,
    FRAMEBASE_Y,
    FRAMEBASE_MJPEG_Y,
    FRAMEBASE_MJPEG_YUY2,
    FRAMEBASE_MJPEG_H264_Y
} H264_FORMAT_TYPE ;

#define PIPE0_EN    (1<<0)
#define PIPE1_EN    (1<<1)
#define PIPE_EN_MASK (0x3)
#define PIPE_EN_SYNC (1<<2)

#define PIPE_CFG_YUY2   (1<<0)
#define PIPE_CFG_MJPEG  (1<<1)
#define PIPE_CFG_H264   (1<<2)
#define PIPE_CFG_NV12   (1<<3)
#define PIPE_CFG_MASK   (0x7)

#define PIPE_CFG(pipe,cfg)  (  ( (cfg) & PIPE_CFG_MASK ) << ( pipe << 2 )  )

#define PIPE_PH_TYPE_NA  (0)
#define PIPE_PH_TYPE_1  (1)
#define PIPE_PH_TYPE_2  (2)

typedef struct _STREAM_CTL
{
    MMP_ULONG  rd_index,wr_index ;
    MMP_ULONG  total_rd,total_wr ;
    MMP_ULONG  buf_addr,slot_size ;
    MMP_USHORT slot_num ;
} STREAM_CTL ;

typedef struct _STREAM_CFG
{
	MMP_UBYTE pipe_en  ;
	MMP_UBYTE pipe_cfg ;   
	MMP_UBYTE pipe0_ph_type ;
	MMP_UBYTE pipe1_ph_type ;
	MMP_USHORT pipe0_w,pipe0_h;
	MMP_USHORT pipe1_w,pipe1_h;
	MMP_USHORT pipe_sync ;
	STREAM_CTL pipe0_b ;
	STREAM_CTL pipe1_b ;
	MMP_USHORT pipeg_w,pipeg_h;
	STREAM_CTL pipeg_b ;
} STREAM_CFG ;

typedef enum {
    PCCAM_160_120 = 0,
    PCCAM_176_144,
    PCCAM_320_240,
    PCCAM_352_288,
    PCCAM_640_480,
    PCCAM_720_480,
    PCCAM_800_600,
    PCCAM_1024_768,
    PCCAM_1280_720,
    PCCAM_1280_960,
    PCCAM_1600_1200,
    PCCAM_1920_1080,
    PCCAM_2048_1536,
    PCCAM_2560_1920,
    PCCAM_848_480 ,
    PCCAM_1280_1024,
    PCCAM_2560_1440,
    PCCAM_400_296,
    PCCAM_384_216,
    PCCAM_640_360,
    PCCAM_960_720,
    PCCAM_272_144,
    PCCAM_368_208,
    PCCAM_480_272,
    PCCAM_624_352,
    PCCAM_912_512,
    PCCAM_800_448,
    PCCAM_960_544,
    PCCAM_160_96,
    PCCAM_320_180,
    PCCAM_2176_1224,
    PCCAM_RES_NUM
} RES_TYPE_LIST ;

typedef struct _RES_TYPE_CFG
{
    RES_TYPE_LIST res_type ;
    MMP_USHORT res_w ;
    MMP_USHORT res_h ;
        
} RES_TYPE_CFG ;


#define DMA_SINGLE_BUFFER   0
#define DMA_DUAL_BUFFER     (1<<0)
typedef struct _STREAM_DMA_BLK
{
    MMP_USHORT  max_dsize;
    MMP_USHORT  header_len;
    MMP_ULONG   blk_addr;
    MMP_ULONG   blk_size;
    MMP_ULONG   next_blk;
    MMP_ULONG   cur_addr;
    MMP_ULONG   tx_len;
    MMP_ULONG   tx_packets;
    MMP_ULONG   dma_buf[2];
    MMP_ULONG   dummy_flag;
} STREAM_DMA_BLK ;

RES_TYPE_CFG *GetResCfg(MMP_UBYTE resolution);
MMP_ULONG GetYUY2FrameSize(MMP_UBYTE resolution);
MMP_ULONG GetMJPEGFrameSize(MMP_UBYTE resolution);
MMP_ULONG GetYUV420FrameSize(MMP_UBYTE resolution);
MMP_USHORT GetMaxFrameRate(MMP_UBYTE resolution) ;

void MMPF_InitPCCam(STREAM_CFG *stream_cfg,MMP_ULONG res);
void InitScale(STREAM_CFG *stream_cfg,MMP_ULONG res);
void UninitScale(void);
STREAM_CFG *usb_get_cur_image_pipe(void);
void usb_set_cur_image_pipe(STREAM_CFG *cur_pipe);
MMP_USHORT MMPF_GetJfifTag(MMP_USHORT *pTagID, MMP_USHORT *pTagLength, MMP_UBYTE **ppImgAddr, MMP_ULONG length);

MMP_BOOL MMPF_Video_IsEmpty(MMP_UBYTE pipe);
MMP_BOOL MMPF_Video_IsFull(MMP_UBYTE pipe);
MMP_UBYTE MMPF_Video_FreeSlot(MMP_UBYTE pipe);
void MMPF_Video_UpdateWrPtr(MMP_UBYTE pipe);
void MMPF_Video_UpdateRdPtr(MMP_UBYTE pipe);
MMP_UBYTE *MMPF_Video_CurRdPtr(MMP_UBYTE pipe);
MMP_UBYTE *MMPF_Video_CurWrPtr(MMP_UBYTE pipe);
void MMPF_Video_Init_Buffer(void);
STREAM_CTL *MMPF_Video_GetStreamCtlInfo(MMP_UBYTE pipe);
void MMPF_Video_UpdateRdPtrByPayloadLength(void);

void MMPF_Video_InitDMABlk(MMP_USHORT uvc_payload_size,MMP_ULONG dmabuf1,MMP_ULONG dmabuf2) ;
void MMPF_Video_AddDMABlk(MMP_ULONG header_len,MMP_ULONG blk_addr,MMP_ULONG blk_size,MMP_USHORT dummy_flag);
STREAM_DMA_BLK *MMPF_Video_CurBlk(void);
MMP_BOOL MMPF_Video_NextBlk(void);
MMP_UBYTE *MMPF_Video_GetBlkApp3Header(STREAM_DMA_BLK *dma_blk);
void MMPF_USB_AudioDm(void);
void MMPF_USB_ReleaseDm(void);

#endif

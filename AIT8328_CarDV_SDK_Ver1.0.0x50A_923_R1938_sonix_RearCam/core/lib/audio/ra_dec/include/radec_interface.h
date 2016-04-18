#define	ENABLE_RA8LBR_DECODE
#define	ENABLE_RA10AAC_DECODE

#ifndef _RADEC_INTERFACE_INCLUDE_
#define _RADEC_INTERFACE_INCLUDE_

#include "ra_depack.h"
#include "ra_decode.h"
#include "rm_parse.h"

#ifdef ENABLE_RA8LBR_DECODE
#include "ra8lbr_decode.h"
#endif
#ifdef ENABLE_RA10AAC_DECODE
#include "aac_decode.h"
#endif

#define	RA_BACKEND_COOK		0x01
#define	RA_BACKEND_RAAC		0x02

typedef struct
{
//    FILE*      fpIn;
//    FILE*      fpOut;
    ra_depack* pDepack;
    ra_decode* pDecode;
    BYTE*      pPcmOutBuf;
    UINT32     ulOutBufSize;
    UINT32     ulDataBytesWritten;

    UINT32     raBackendCodec;
    UINT32     avg_bit_rate;
    UINT32     max_bit_rate;
    UINT32     max_packet_size;
    UINT32     avg_packet_size;
    UINT32     duration;
    UINT32     Channels;
    UINT32     SampleRate;

    UINT16     usStreamNum;// 0 for realaudio, 1 for realvideo for example.
    UINT32     MaxSamples;// = superblock size

	UINT32     PcmOutMaxSamples; //used to indicate i2s_isr to trigger ra decoder.
} radec_info;


#define RM2WAV_INITIAL_READ_SIZE 16



#define	RA8LBR_GET_DATA_STATE		0x01
#define	RA8LBR_START_DECODE_STATE	0x02
#define	RA8LBR_BLOCK_DECODE_STATE	0x03
#define	RA8LBR_FRAME_DECODE_STATE	0x04

extern	UINT8	ra8lbr_decode_state;
extern	UINT32  ra_decode_block_ulTotalConsumed;
extern	UINT32  ra_decode_block_ulBytesLeft;
extern	UINT32  rm_parser_malloc_size;

extern	rm_parser*          RADec_pParser       ;
extern	ra_depack*          RADec_pDepack       ;
extern	rm_stream_header*   RADec_pStreamHdr    ;
extern	ra_decode*          RADec_pDecode       ;
extern	ra_format_info*     RADec_pCodecInfo    ;


extern	void ait_callback_ra8lbr_set_get_data_state(void);
extern	void ait_callback_ra8lbr_set_start_decode_state(void);
extern	void ait_callback_ra8lbr_set_block_decode_state(void);
extern	void ait_callback_ra8lbr_set_frame_decode_state(void);
extern	int ait_callback_ra8lbr_be_get_data_state(void);
extern	int ait_callback_ra8lbr_be_start_decode_state(void);
extern	int ait_callback_ra8lbr_be_block_decode_state(void);
extern	int ait_callback_ra8lbr_be_frame_decode_state(void);

extern  void      RADec_init(void);
extern	HX_RESULT RADec_parse_info(radec_info *pInfo);
extern	HX_RESULT RADec_decode_frame(radec_info *pInfo);
extern	HX_RESULT RADec_decode_flush(radec_info *pInfo);
extern	HX_RESULT RADec_set_play_time(radec_info *pInfo, UINT32 ulTime);
extern	UINT32    RADec_get_play_time(radec_info *pInfo);
//extern	void	  RADec_SetWorkingBuf(int *ptr);

#endif //end of #ifndef _RADEC_INTERFACE_INCLUDE_
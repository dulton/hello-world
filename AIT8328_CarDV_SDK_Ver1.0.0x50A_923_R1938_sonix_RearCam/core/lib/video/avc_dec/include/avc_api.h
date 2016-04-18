/*
 *  Copyright (c) 2006 Alpha Imaging Technology Corp. All Rights Reserved
 *
 */

#ifndef _AVC_API_H_
#define _AVC_API_H_

#include "data_type.h"

typedef enum{
	AVC_INIT_FAILED			= -999,
	AVC_DECODE_FAILED		= -998,
	AVC_RELEASE_FAILED		= -997,
	AVC_ALLOC_MEM_FAILED	= -996,
    AVC_NOMORE_DATA         = -995,
	AVC_INIT_SUCCEED		= 1,
	AVC_DECODE_SUCCEED		= 2,
	AVC_RELEASE_SUCCEED		= 3
} ERR_MSG;

typedef void* AVC_DEC_HANDLE;
/**
 *  @brief this structure is used to indicate the decoder where the bitstream is,
 *         and then put the decoded picture in this struct
 */
typedef struct
{
    AVC_DEC_HANDLE  *h_dec;
	MMP_UBYTE       *bs_buf;        ///< bitstream buffer
	MMP_UBYTE	    *bitstream;		///< bitstream buffer pointer
	MMP_UBYTE	    *bmp;			///< decoded picture
	MMP_ULONG64     display_time;
    MMP_UBYTE       is_init_ready;
	MMP_LONG		bitstream_len;	///< current compressed frame's length
	MMP_LONG		bs_start_pos;
	MMP_LONG		buf_length;		///< the bistream buffer length
	MMP_LONG		width;			///< decoded picture width
	MMP_LONG		height;			///< decoded picture height
	MMP_USHORT	    cropping_left;	// ue(v)
	MMP_USHORT	    cropping_right;	// ue(v)
	MMP_USHORT	    cropping_top;	// ue(v)
	MMP_USHORT	    cropping_bottom;// ue(v)
	MMP_ULONG       is_nalu_to_rbsp_done;
	MMP_ULONG       frame_num;
	MMP_UBYTE       max_buffed_frame;
	
	
    MMP_UBYTE       is_output_ready;
	//aspect ratio
	MMP_UBYTE    	is_aspect_ratio_present;
	MMP_LONG		sar_width;
	MMP_LONG		sar_height;
	//time info
	MMP_UBYTE		is_time_info_present;
	MMP_LONG		num_units_in_tick;
	MMP_LONG		time_scale;
	MMP_ULONG64     input_time;
	
	
	MMP_ULONG       current_mem_addr;
	MMP_ULONG       end_mem_addr;
	MMP_ULONG       state;
	MMP_ULONG       buf_start_addr;
	MMP_ULONG       buf_end_addr;
	MMP_UBYTE       IsSemAvail;
	MMP_ULONG       semID;
	MMP_BOOL        IsKeyFrame;
	MMP_UBYTE       ParseEP3byHW;
	MMP_UBYTE       IsPython;
	MMP_UBYTE       LengthEncodedByte;
	MMP_UBYTE       FlushFrame;
	MMP_UBYTE       track_no;    
	MMP_UBYTE       is_sps_hd_type;
	//MMP_ULONG       framebuf_num;
	//MMP_ULONG       framebuf_addr[18];
	//MMP_ULONG       framebuf_size[18];
} AVC_DATA_BUF;




/*
 * avc decoder APIs
 */

//MMP_LONG decode_avc_init_header(AVC_DATA_BUF *buf);

/**
 * @brief initial the avc decoder
 * @param handle : the decoder entity
 */
MMP_LONG avc_init(AVC_DEC_HANDLE *handle,AVC_DATA_BUF *buf);

MMP_LONG avc_malloc_resource(AVC_DEC_HANDLE *handle,AVC_DATA_BUF *buf);
MMP_LONG avc_one_nal_unit(AVC_DEC_HANDLE *handle,AVC_DATA_BUF *buf);
MMP_LONG decode_avc_one_frame(AVC_DEC_HANDLE *handle,AVC_DATA_BUF *buf);

/**
 * @brief release the resources used by the avc decoder
 * @param handle : the decoder entity
 */
MMP_LONG avc_release(AVC_DEC_HANDLE *handle,AVC_DATA_BUF *buf);
void avc_refresh_framebuf(AVC_DEC_HANDLE *handle,uint32_t addr);
void get_avc_next_frame_addr(AVC_DEC_HANDLE *handle,MMP_ULONG *y_addr,MMP_ULONG *u_addr,MMP_ULONG *v_addr);
int32_t get_avc_output_frame(AVC_DEC_HANDLE *handle, AVC_DATA_BUF *buf, MMP_ULONG *ulYAddr, MMP_ULONG *ulUVAddr, MMP_ULONG *ulTime, MMP_BYTE *bIsKey);
//int32_t get_mjpg_output_frame(AVC_DEC_HANDLE *handle, MMP_MJPG_BUF *buf, MMP_ULONG *ulYAddr, MMP_ULONG *ulUVAddr, MMP_ULONG *ulTime);
void get_mjpg_next_frame_addr(AVC_DEC_HANDLE *handle, MMP_ULONG *y_addr, MMP_ULONG *u_addr, MMP_ULONG *v_addr);

int32_t avc_flush_frame(AVC_DEC_HANDLE *handle);
int32_t mjpg_flush_frame(AVC_DEC_HANDLE *handle);

#endif
/**
 *  Copyright (c) 2006 Alpha Imaging Technology Corp. All Rights Reserved
 *
 *  @file   avc_decode.h
 *  @brief  H.264/AVC APIs Header
 *  @author Hsin-Wei Yang
 */

#ifndef _AVC_DECODE_H_
#define _AVC_DECODE_H_

#include "avc_config.h"

#include "avc_utils.h"
#include "data_type.h"
#include "avc_api.h"

#if _AVC_HW_DEC_ == 1
#include "avc_reg.h"
#endif

//only for debug
#include "includes_fw.h"
#include "lib_retina.h"
 
#define MAX_COLOR_TRANS 1

/*
 * FIXME! These stuff should be moved somewhere else.
 */
#define CLIP(x)	((x & (~255)) ? ((-x) >> 31) : x)
#define MAX_PVALUE 255
#define MAX(a,b)    (((a) > (b)) ? (a) : (b))
#define MIN(a,b)    (((a) < (b)) ? (a) : (b))
#define CLIP3( _Min, _Max, _Val) (((_Val)<(_Min))? (_Min):(((_Val)>(_Max))? (_Max):(_Val)))
#define ABS(a) (a > 0 ? a : -(a))
#define MAX3(_x,_y,_z)			MAX(MAX(_x,_y),_z)
#define MIN3(_x,_y,_z)			MIN(MIN(_x,_y),_z)
#define MEDIAN(_x,_y,_z)		((_x)+(_y)+(_z)-MAX3(_x,_y,_z)-MIN3(_x,_y,_z))

#define MAXnum_ref_frames_in_pic_order_cnt_cycle	256
#define MAXnum_slice_groups_minus1					8
#define MAX_MB_WIDTH    (1920/16)
#define MAX_MB_HEIGHT   (1088/16)

#define BASELINE_PROFILE        66
#define MAIN_PROFILE            77

//FREXT Profile IDC definitions
#define FREXT_HP        100      ///< YUV 4:2:0/8 "High"
#define FREXT_Hi10P     110      ///< YUV 4:2:0/10 "High 10"
#define FREXT_Hi422     122      ///< YUV 4:2:2/10 "High 4:2:2"
#define FREXT_Hi444     244//144 ///< YUV 4:4:4/12 "High 4:4:4"


#define NALU_TYPE_SLICE    1
#define NALU_TYPE_DPA      2
#define NALU_TYPE_DPB      3
#define NALU_TYPE_DPC      4
#define NALU_TYPE_IDR      5
#define NALU_TYPE_SEI      6
#define NALU_TYPE_SPS      7
#define NALU_TYPE_PPS      8
#define NALU_TYPE_AUD      9
#define NALU_TYPE_EOSEQ    10
#define NALU_TYPE_EOSTREAM 11
#define NALU_TYPE_FILL     12

#define NALU_PRIORITY_HIGHEST     3
#define NALU_PRIORITY_HIGH        2
#define NALU_PRIRITY_LOW          1
#define NALU_PRIORITY_DISPOSABLE  0

#if _AVC_HW_DEC_ == 0

// 16x16 intra prediction modes
#define IPRED_16_V			0
#define IPRED_16_H			1
#define IPRED_16_DC			2
#define IPRED_16_PLAIN		3
#define IPRED_16_DC_LEFT	4
#define IPRED_16_DC_TOP		5
#define IPRED_16_DC_128		6

// 4x4 intra prediction modes
#define IPRED_4_V			0
#define IPRED_4_H			1
#define IPRED_4_DC			2
#define IPRED_4_DDL			3
#define IPRED_4_DDR			4
#define IPRED_4_VR			5
#define IPRED_4_HD			6
#define IPRED_4_VL			7
#define IPRED_4_HU			8
#define IPRED_4_DC_LEFT		9
#define IPRED_4_DC_TOP		10
#define IPRED_4_DC_128		11
#define IPRED_4_DDL2		12
#define IPRED_4_VL2         13

// 8x8 chroma intra prediction modes
#define CHROMA_PRED_8_DC		0
#define CHROMA_PRED_8_H			1
#define CHROMA_PRED_8_V			2
#define CHROMA_PRED_8_PLAIN		3
#define CHROMA_PRED_8_DC_LEFT	4
#define CHROMA_PRED_8_DC_TOP	5
#define CHROMA_PRED_8_DC_128	6

#define INTRA_4x4		0
#define INTRA_8x8		1
#define INTRA_16x16		2
#define INTRA_PCM		3

#define P_16x16			4
#define P_16x8			5
#define P_8x16			6
#define P_8x8			7
#define P_8x4			8
#define P_4x8			9
#define P_4x4			10
#define P_SKIP			11

#else

/*
#define INTRA_4x4		0
#define INTRA_8x8		1
#define INTRA_16x16		1
#define INTRA_PCM		3

#define P_16x16			4
#define P_16x8			5
#define P_8x16			6
#define P_8x8			7
#define P_8x4			8
#define P_4x8			9
#define P_4x4			10
#define P_SKIP			11
*/
#define INTRA_4x4		0
#define INTRA_16x16		1
#define INTRA_PCM		2

#define P_16x16			0
#define P_16x8			1
#define P_8x16			2
#define P_8x8			3
#define P_8x8_R0		4
#define P_SKIP			5

#define SW_INTRA_4x4		0
#define SW_INTRA_8x8		1
#define SW_INTRA_16x16		2
#define SW_INTRA_PCM		3

#define SW_INTER_MODE_BASE  3
#define SW_P_16x16		    3+0
#define SW_P_16x8		    3+1
#define SW_P_8x16		    3+2
#define SW_P_8x8		    3+3
#define SW_P_8x8_R0		    3+4
#define SW_P_SKIP		    3+5


#endif

#define EOS             1                       //!< End Of Sequence
#define SOP             2                       //!< Start Of Picture
#define SOS             3                       //!< Start Of Slice

#define BLOCK_LEFT_AVAILABLE        0x01
#define BLOCK_UP_AVAILABLE          0x02
#define BLOCK_LEFT_UP_AVAILABLE     0x03    //(BLOCK_LEFT_AVAILABLE | BLOCK_UP_AVAILABLE)
#define BLOCK_UPRIGHT_AVAILABLE     0x04

#define BLOCK_INDEX_CHROMA_DC   (24)
#define BLOCK_INDEX_LUMA_DC     (25)

#define MAX_LIST_SIZE	16
#define MAX_MMCO_COUNT	66
#define MAX_SPS_NUM		1//32
#define MAX_PPS_NUM		1//256
#define MAX_TRACK_NUM   2
#define REF_START_POS	1
#define AVC_RESERVE_BUF_NUM (1)
//#define EDGE			(16 + 4)

#if _AVC_HW_DEC_ == 0
#define EDGE			(16 + 8)
#define EDGE_C			(8 + 4)
#else
#define EDGE			(0)
#define EDGE_C			(0)
#endif



#define MB_DECODE_OK    3
#define MB_CONCEALED    2
#define MB_CORRUPTED    1
#define MB_EMPTY        0


typedef enum {
	DEC_P_SLICE = 0,
	DEC_B_SLICE,
	DEC_I_SLICE,
	DEC_SP_SLICE,
	DEC_SI_SLICE
} SliceType;

/**
 * @brief The structure for NAL unit
 */
typedef struct 
{
	int32_t	forbidden_bit;			///< forbidden bit
	int32_t nal_unit_type;			///< NALU TYPE
	int32_t nal_reference_idc;		///< NALU PRIORITY
	int32_t payload;				///< the length of payload buffer
	uint8_t *payload_buf;	        ///< the payload buffer
} NALU_t;


/**
 * @brief The structure for Sequence Parameter Set
 */
typedef struct 
{   
    int32_t	offset_for_non_ref_pic;				// se(v)
	int32_t	offset_for_top_to_bottom_field;		// se(v)
	int16_t	offset_for_ref_frame[MAXnum_ref_frames_in_pic_order_cnt_cycle];   // se(v)
	
    uint8_t	mb_width;							// ue(v)
	uint8_t	mb_height;							// ue(v)
    int16_t	cropping_left;						// ue(v)
	int16_t	cropping_right;						// ue(v)
	int16_t	cropping_top;						// ue(v)
	int16_t	cropping_bottom;					// ue(v)

    int8_t	sps_id;                             // ue(v)
    int8_t	log2_max_frame_num;					// ue(v)
    int8_t	log2_max_poc_lsb;					// ue(v)
    int8_t	num_ref_frames_in_poc_cycle;		// ue(v)
    int8_t	num_ref_frames;						// ue(v)

    int8_t	profile_idc;						// u(8)
	int8_t	level_idc;							// u(8)
	int8_t	poc_type;
	int8_t	is_delta_pic_order_always_zero;		// u(1)
	int8_t	gaps_in_frame_num_value_allowed;	// u(1)
	int8_t	is_frame_mbs_only;					// u(1)
	int8_t	direct8x8_inference;				// u(1)
	int8_t	frame_cropping_flag;				// u(1)
    
    int8_t  chroma_format_idc;                  // ue(v)
    int8_t  residual_colour_transform_flag;     // u(1)
    int8_t  bit_depth_luma_minus8;              // ue(v)
    int8_t  bit_depth_chroma_minus8;            // ue(v)
    int8_t  qpprime_y_zero_transform_bypass_flag; // u(1)
    int8_t  seq_scaling_matrix_present_flag;    // u(1)
    int8_t  seq_scaling_list_present_flag[8];   // u(1)
    int8_t  scaling_list[8][64];
    int8_t  mb_adaptive_frame_field;            // u(1)
} Sequence_Parameter_Set_t;

/**
 * @brief The structure for Picture Parameter Set
 */
typedef struct 
{
	//FMO stuff
	int16_t		slice_group_change_rate;                    // ue(v)
	int16_t		num_slice_group_int_map_units;              // ue(v)
	//end of FMO

    int16_t		pps_id;                                     // ue(v)
	int16_t		num_slice_groups;                           // ue(v)
    int16_t		*slice_group_id;                            // complete MBAmap u(v)

    int8_t		sps_id;                                     // ue(v)
    int8_t		slice_group_map_type;                       // ue(v)
	int8_t		num_ref_idx_l0_active;                      // ue(v)
	int8_t		num_ref_idx_l1_active;                      // ue(v)
	
	int8_t		pic_init_qp;                                // se(v)
	int8_t		pic_init_qs;                                // se(v)
	int8_t		chroma_qp_index_offset;                     // se(v)

    int8_t		is_cabac_mode;                              // u(1)
	int8_t		is_pic_order_present;                       // u(1)
    int8_t		is_slice_group_change_direction;            // u(1)
	int8_t		is_deblocking_filter_control;               // u(1)
	int8_t		is_constrained_intra_pred;                  // u(1)
	int8_t		is_redundant_pic_cnt;                       // u(1)
	int8_t      is_weighted_pred;                           // u(1)
	int8_t      weighted_bipred_idc;                        // u(1)
	// P_V3
	int8_t      transform_8x8_mode_flag;                    // u(1)
	int8_t      pic_scaling_matrix_present_flag;            // u(1)
	int8_t      pic_scaling_list_present_flag[8];           // u(1)
	int8_t      scaling_list[8][64];
	int8_t      second_chroma_qp_index_offset;              // se(v)

    int16_t		run_length[MAXnum_slice_groups_minus1];     // ue(v)
	int16_t		top_left[MAXnum_slice_groups_minus1];       // ue(v)
	int16_t		bottom_right[MAXnum_slice_groups_minus1];   // ue(v)
} Picture_Parameter_Set_t;

/**
 * @brief The structure for SLICE header
 */
typedef struct
{
	Sequence_Parameter_Set_t	*sps;
	Picture_Parameter_Set_t		*pps;
	
	uint16_t	start_mb_nr;                //!< MUST be set by NAL even in case of ei_flag == 1
	SliceType	type;                       //!< picture type
	uint8_t		pps_id;				        ///<the ID of the picture parameter set the slice is reffering to
	int8_t		is_field_pic;
	int32_t		pic_order_cnt_lsb;
	int32_t		delta_pic_order_cnt_bottom;
	int32_t		delta_pic_order_cnt[3];
	int32_t		redundant_pic_cnt;
	int8_t		num_ref_idx_l0_active;
	int8_t		num_ref_idx_l1_active;
	int8_t		is_num_ref_idx_override;
	int8_t		slice_qp_delta;
	int8_t      is_bottom_field;
	int8_t      is_direct_spatial_mv_pred;

    int32_t     ref_pic_list_reordering_flag_l0;
    int32_t		reordering_of_pic_nums_idc[17];
    int32_t     abs_diff_pic_num[17];
    int32_t     long_term_pic_num[17];
    
    int32_t     ref_pic_list_reordering_flag_l1;
    int32_t		reordering_of_pic_nums_idc_L1[17];
    int32_t     abs_diff_pic_num_L1[17];
    int32_t     long_term_pic_num_L1[17];
	
	// P_V3
    uint32_t    luma_log2_weight_denom;
    uint32_t    chroma_log2_weight_denom;
    int8_t      luma_weight_l0_flag;
    uint32_t    cabac_init_idc;
    
    /*
	 * deblocking filter
	 */
	int8_t		LFDisableIdc;		        ///< Disable loop filter on slice
	uint8_t		LFAlphaC0Offset;	        ///< Alpha and C0 offset for filtering slice
	uint8_t		LFBetaOffset;		        ///< Beta offset for filtering slice

	/*
	 * TODO, We don't support data partion yet!
	 */
	

	/*
	 * TODO, We don't support CABAC yet!
	 */

	
} AVC_Slice_t;


/**
 * @brief The structure for motion vector
 */
typedef struct
{
	int16_t x;	///< horizontal movement of the motion vector
	int16_t y;	///< vertical movement of the motion vector
} VECTOR;


/**
 * @brief The structure for macroblock
 */
typedef struct
{
#if _AVC_HW_DEC_ == 0
    int16_t	    luma_cbp;			    ///< the coded-block-pattern for the luma component
    int16_t	    chroma_cbp;			    ///< the coded-block-pattern for the chroma component
    int16_t    slice_id;               ///< indicate this MB is belong to which slice
    int8_t	    mb_type;			    ///< the type of macroblock
	int8_t	    sub_parts[4];		    ///< mb's sub-partition type : P_8x8,P_8x4,P_4x8,P_4x4
	int8_t	    qp;					    ///< the quantization parameter
	int8_t	    is_intra;			    ///< indicate if this mb is intra or not
	int8_t	    intra16x16_mode;	    ///< the intra_16x16 prediction mode
	int8_t	    intra4x4_mode[16];	    ///< the intra_4x4 prediction mode
	int8_t	    nonzero_count[16];      ///< nonzero count for luma
	int8_t	    nonzero_count_c[2][4];  ///< nonzero count for chroma
	int8_t	    chroma_pred_mode;	    ///< the intra chroma prediction mode
    int8_t     left_sample_avail;      ///< indicate if left samples are available
    int8_t     up_sample_avail;        ///< indicate if up samples are available
    int8_t     upleft_sample_avail;    ///< indicate if up-left samples are available
    int8_t     upright_sample_avail;   ///< indicate if up-right samples are available
    int8_t      left_mb_avail;
    int8_t      top_mb_avail;
    int8_t      err_mb_type;


    int32_t     ref_id[4];              ///< the reference frame id

	/*
	 *  0  1  4  5
	 *  2  3  6  7
	 *  8  9 12 13
	 * 10 11 14 15
	 */
	VECTOR	    MV[1][16];				///< the motion vectors

#ifdef _DBG_MVD_
    VECTOR	    mvd[1][16];				///< the motion vectors
#endif

	/*
	 *  0  1
	 *  2  3
	 */
	int8_t	    ref[1][4];				///< the reference index
    int8_t		LFDisableIdc;		    ///< Disable loop filter on slice
	int8_t		LFAlphaC0Offset;	    ///< Alpha and C0 offset for filtering slice
	int8_t		LFBetaOffset;		    ///< Beta offset for filtering slice
	
#else

    int16_t     slice_id;               ///< indicate this MB is belong to which slice

    //int8_t      left_sample_avail;      ///< indicate if left samples are available
    //int8_t      up_sample_avail;        ///< indicate if up samples are available
    //int8_t      upleft_sample_avail;    ///< indicate if up-left samples are available
    //int8_t      upright_sample_avail;   ///< indicate if up-right samples are available

    int8_t      is_intra;
	int8_t      nonzero_count[16];      ///< nonzero count for luma
	int8_t      nonzero_count_c[2][4];  ///< nonzero count for chroma

#endif
	
} AVC_Macroblock_t;

typedef struct
{
    uint32_t *tail;
	uint32_t *start;
    uint32_t bufA;
    uint32_t bufB;
    uint32_t initpos;
    uint32_t buf_length;
	uint32_t bitpos;				///< to record the bit position in the register
    uint32_t bits_count;
    uint32_t eof;
    uint32_t golomb_zeros;
} BS;

#define CACHE_START	7
#define CACHE_WIDTH	6
#define MB_CACHE_SIZE   30
#define CACHE1_SIZE 48

/*
  width : 8
  //Y
  1*8 + 1, 1*8 + 2, 2*8 + 1, 2*8 + 2
  1*8 + 3, 1*8 + 4, 2*8 + 3, 2*8 + 4
  3*8 + 1, 3*8 + 2, 4*8 + 1, 4*8 + 2
  3*8 + 3, 3*8 + 4, 4*8 + 3, 4*8 + 4
  //U
  1*8 + 6, 1*8 + 7, 2*8 + 6, 2*8 + 7
  //V
  4*8 + 6, 4*8 + 7, 5*8 + 6, 5*8 + 7
*/

/**
 * @brief The CACHE structure is uesd to speed up the prediction process
 */
typedef struct
{
	/*
	 *     0  1  2  3  4  5  6  7
	 * 0  
	 * 1      Y  Y  Y  Y    Cb Cb
	 * 2      Y  Y  Y  Y    Cb Cb
	 * 3      Y  Y  Y  Y 
	 * 4      Y  Y  Y  Y    Cr Cr
	 * 5                    Cr Cr  
	 */
	int32_t	nonzero_count[CACHE1_SIZE];     ///< to store nonzero counts
	
	/*
	 *     0 1  2  3  4  5
	 * 0  TL T  T  T  T TR
	 * 1  L  7  8  9 10 11
	 * 2  L 13 14 15 16 17
	 * 3  L 19 20 21 22 23
	 * 4  L 25 26 27 28 29
	 */
	 
#if _AVC_HW_DEC_ == 0
	VECTOR	MV[2][30];                      ///< to store motion vectors
#endif
	
	int32_t	ref[2][30];                     ///< to store reference index
    int32_t	intra4x4_predmode[30];          ///< to store intra4x4 prediction modes
} AVC_CACHE_t;

typedef struct{
	int16_t	len;
	int16_t	value;
} VLC_SYMBOL;

typedef struct{
	int32_t				lookup_bits;
	const VLC_SYMBOL	*table;
} AVC_VLC_TABLE;

/**
 * Memory management control operation opcode.
 */
typedef enum MMCOOpcode{
    MMCO_END=0,
    MMCO_SHORT2UNUSED,
    MMCO_LONG2UNUSED,
    MMCO_SHORT2LONG,
    MMCO_SET_MAX_LONG,
    MMCO_RESET, 
    MMCO_LONG
} MMCOOpcode;

/**
 * Memory management control operation.
 */
typedef struct MMCO{
    MMCOOpcode opcode;
    int32_t short_frame_num;
    int32_t long_index;
} MMCO;

/**
 * @brief The structure for the avc frame
 */
typedef struct
{
	int32_t		poc;		        ///< poc
	int32_t		pic_num;		    //do we really need this?
	int32_t		frame_num;		    //do we really need this?
	int32_t		is_long_term;	    //do we really need this?
	int32_t		long_term_pic_num;
	int32_t		long_term_frame_idx;
	int32_t		used_for_reference;
    int32_t     idr_flag;
    int32_t     adaptive_ref_pic_buffering_flag;
    
    int32_t     idx;
    int32_t     pshudo_addr;
    int8_t      is_out;
    uint8_t     *y_start;
    uint8_t     *uv_start[2];
	uint8_t		*yuv[3];	        ///< yuv[0] : Y, yuv[1] : Cb, yuv[2] : Cr
} AVC_Frame_t;

typedef struct{
    AVC_Frame_t frame;
    
    uint64_t time;
    int8_t  is_KeyFrame;
    int8_t  is_used;
    int8_t      is_out;
    int8_t  is_long_term;
    int8_t  is_reference;
    int32_t poc;
    int32_t frame_num;
    uint32_t	pic_num;
    int32_t	long_term_frame_idx;

    int32_t short_pic_num;
    int32_t long_pic_num;
} AVC_FRAME_STORE_t;

typedef struct
{
	int32_t		frame_num;
    int8_t      is_out;
    uint8_t     *y_start;
    uint8_t     *uv_start[2];
} JPG_Frame_t;

typedef struct{
    JPG_Frame_t frame;    
    uint64_t    time;
    int8_t      is_invalid;
} JPG_FRAME_STORE_t;

#define MAX_JPG_LIST_SIZE	3
typedef struct{
    AVC_DEC_HANDLE  *h_dec;    
    uint32_t    width;				///< image width for luma
	uint32_t    height;				///< image height for luma
    uint32_t	frame_num;          ///< current frame number
   	uint32_t    start_mem_addr;      ///< maximun frame number    
    uint32_t    current_mem_addr;
    uint32_t    end_mem_addr;
    uint32_t    jpg_linebuf_addr;
    uint32_t    jpg_linebuf_length;     
    uint32_t    dpb_size;    
    JPG_FRAME_STORE_t   dpb_mem[1+MAX_JPG_LIST_SIZE];//[1+32];
	JPG_FRAME_STORE_t   *dpblist[1+MAX_JPG_LIST_SIZE];//[1+32];    	
    uint32_t    state;
    uint64_t    jpg_timestamp;
	uint8_t     *bmp;			///< decoded picture
    uint8_t     jpg_output_ready;
} JPG_DATA_BUF;

/**
 * @brief The main structure in our AVC decoder
 */
typedef struct {
    NALU_t	            nal;
    AVC_Slice_t         slice;		///< slice header
    AVC_Macroblock_t	*mb;        ///< to store macroblock's info, such as mv,reference,qp...etc

    /* common */
    uint32_t    mb_x;
    uint32_t    mb_y;

    uint32_t    width;				///< image width for luma
	uint32_t    height;				///< image height for luma
	uint32_t    width_c;            ///< image width for chroma
	uint32_t    height_c;           ///< image height for luma
    uint8_t    mb_width;			///< image width in MBs
	uint8_t    mb_height;			///< image height in MBs
	int32_t     ewidth;				///< image width   + EDGE*2, for unrestricted mv
	int32_t     eheight;			///< image height  + EDGE*2, for unrestricted mv
	int32_t	    ewidth_c;           ///< image width_c + EDGE,   for unrestricted mv
	int32_t	    eheight_c;          ///< image height_c+ EDGE,   for unrestricted mv
    uint32_t    framesize_in_mbs;	///< the number of Mbs in one frame
    int8_t      qp;
	int32_t	    mem_stride_y;		///< used to calculate current mb address(luma)
	int32_t	    mem_stride_c;		///< used to calculate current mb address(chroma)
    int32_t     mb_skip_num;        ///< how many MBs are coded as SKIP MB
    int32_t     current_mb_nr;

    int32_t	    num_ref_frames;		///< number of reference frames
	int32_t     num_ref_idx_l0_active;///< forward references
	int32_t     num_ref_idx_l1_active;///< backward references
    uint32_t    dec_mb_num;			///< the number of macroblock of this frame be decoded
	int32_t     max_frame_num;      ///< maximun frame number
    int32_t	    frame_num;          ///< current frame number
    int32_t     prev_frame_num;     ///< previous frame number
    uint8_t     is_init_frame;      ///< indicate if we ar going to init frames or not
    int32_t	    idr_pic_id;
	SliceType   type;               ///< current slice type
    int32_t     idr_flag;
    int32_t     nal_reference_idc;
    int32_t     slice_num;          ///< total slice numbers
#if _AVC_HW_DEC_ == 0    
    int32_t     block_offset[16 + 4 + 4];
#endif    
    int32_t     slice_group_change_cycle;
    int32_t     dpb_size;

    int8_t      is_first_I_frame;
    int8_t      is_err_first_I;
    int8_t      err_conceal;
    int8_t      dec_error;

    //we reset the ref_pic_num per slice,if the pic_num > INT_MAX, we reset the pic_num to 0
	int32_t ref_pic_num[32];

    Sequence_Parameter_Set_t	*sps;		///< point to sps_array[sps_id]
	Picture_Parameter_Set_t		*pps;		///< point to pps_array[pps_id]

    /*
     * POC
     */
	int32_t	toppoc;
	int32_t	bottompoc;
    int32_t poc;
    int32_t FrameNumOffset;
    int32_t PrevPicOrderCntMsb;
    int32_t PrevPicOrderCntLsb;
    int32_t PicOrderCntMsb;
    int32_t PicOrderCntLsb;
    int32_t PreviousFrameNum;
    int32_t PreviousFrameNumOffset;

    int32_t prev_ref_poc;
    int32_t earlier_missing_poc;

    /*
     * memory management control operation
     */
    int32_t				mmco_index[2];
	int32_t				is_mmco_reset;
    int32_t             mmco_offset;

    int32_t             is_disposable;                          //!< flag for disposable frame, 1:disposable

    int32_t             idr_flag_old;
    int32_t             pps_id_old;
    int32_t             idr_pic_id_old;
    int32_t             frame_num_old;
    int32_t             nal_ref_idc;
    int32_t             pic_order_cnt_lsb_old;
    int32_t		        delta_pic_order_cnt_bottom_old;
    int32_t		        delta_pic_order_cnt_old[3];

    /*
	 * brief bitstream
	 */
    uint32_t *tail;
	uint32_t *start;
    uint32_t bufA;
    uint32_t bufB;
    uint32_t initpos;
    uint32_t buf_length;
	uint32_t bitpos;				///< to record the bit position in the register
    uint32_t bits_count;
    uint32_t eof;
    uint32_t golomb_zeros;

    //error concealment
    int32_t err_start_mbnr;
    int32_t err_mb_nums;

    /*
     * decoding picture buffer
     */
    int32_t             short_ref_nums;
	int32_t				long_ref_nums;
    int32_t             total_ref_nums;
    int32_t             list_size[2];
	int32_t				no_output_of_prior_pics_flag;
	int32_t				long_term_reference_flag;
    int32_t             adaptive_ref_pic_buffering_flag;

    /* cache */
    AVC_CACHE_t			cache;	                            ///< to store mv, reference for one MB

    /* reference frame */
    AVC_FRAME_STORE_t	*curr_fs;
	AVC_Frame_t			*curr_frame;		                ///< point to the current decoded frame
	//AVC_Macroblock_t	*mb;	                            ///< to store macroblock's info, such as mv,reference,qp...etc
	AVC_FRAME_STORE_t   ref_mem[1+MAX_LIST_SIZE];//[1+32];
	AVC_FRAME_STORE_t   *reference[1+MAX_LIST_SIZE];//[1+32];
	//AVC_FRAME_STORE_t   **reference;//[1+32];

    /* reference list */
    AVC_Frame_t			*list[2][MAX_LIST_SIZE];			///< point to the reference frame
    AVC_FRAME_STORE_t   *st_ref[MAX_LIST_SIZE];
    AVC_FRAME_STORE_t   *lt_ref[MAX_LIST_SIZE];

    #ifdef ENABLE_EC
    int8_t              *err_status_y8;
    #endif

    //check this
	//VLC
	#if ((_AVC_HW_DEC_ == 0) || (DEFAULT_VLD_MODE == SW_VLD_MODE))
	AVC_VLC_TABLE		CoeffToken_table[5];
	AVC_VLC_TABLE		LevelPrefix_table;
	AVC_VLC_TABLE		TotalZeros_table[15];
	AVC_VLC_TABLE		TotalZeros_DC_table[3];
	AVC_VLC_TABLE		RunBefore_table[7];
	#endif

	/*
	 * color transform
	 */
	color_func_ptr		output_image[MAX_COLOR_TRANS];
	
#if _AVC_HW_DEC_ == 0	
	/*
	 * motion compensation
	 */
	mc_func_ptr mc16[16];	///< mc's function pointer, it's used for 16x16 MC
	mc_func_ptr mc8[16];	///< mc's function pointer, it's used for 16x8, 8x16, 8x8 MC
	mc_func_ptr mc4[16];	///< mc's function pointer, it's used for 8x4, 4*8, 4x4 MC

	/*
	 * intra-prediction
	 */
	intrapred_func_ptr	ipred_4x4[14];
	intrapred_func_ptr	ipred_16x16[7];
	intrapred_func_ptr	ipred_8x8c[7];

    /*
	 * temp buffer
	 */
	int32_t	dct_16_dc[16];			///<used to store residual data
	int32_t	dct_4x4[16][16];		///<used to store residual data
	int32_t	dct_chroma_dc[2][4];	///<used to store residual data
	int32_t	dct_chroma_ac[8][16];	///<used to store residual data
#endif

    /*
	 * Header
	 */
	Sequence_Parameter_Set_t	sps_array[MAX_SPS_NUM];
	///< Sequence Parameter Set array, max size is 256
	Picture_Parameter_Set_t		pps_array[MAX_PPS_NUM];
	///< Picture Parameter Set array, max size is 256

    //check this
	MMCO				mmco[MAX_MMCO_COUNT];

    uint8_t                 vld_index;
    AVC_Frame_t             *rec_buffer;
    
    #if _AVC_HW_DEC_ == 1
    int16_t                 MbMapMem[MAX_MB_WIDTH * MAX_MB_HEIGHT];
    #endif
    
    int16_t                 *MbToSliceMap;
    
    
#if _AVC_HW_DEC_ == 1
    volatile AVC_DEC_GBL_CTRL        *regGBL;
    volatile AVC_REFERENCE_REG       *regReference;
    volatile AVC_REC_FRAME_REG       *regRecFrame;
    volatile AVC_DBLOCKING_PARAMS    *regDeblock;
    volatile AVC_VLD_PARAMS          *regVldParams;
    volatile AVC_VLD_MB_DATA         *regVldData[2];
    volatile AVC_VLD_BUF             *regVldBuf[2];
    volatile AVC_MB_MAP              *regMap;
    //volatile AVC_MB_INFO             **regMbInfo;
    
    //AVC_MB_INFO                     regMbInfoMem[MAX_MB_WIDTH * MAX_MB_HEIGHT];
    AVC_MB_INFO                     *regMbInfo;
    //AVC_MB_INFO                     *regMbInfo2;
    MMPF_OS_SEMID 		            mutex;
#endif

    uint32_t    current_mem_addr;
    uint32_t    end_mem_addr;
    int8_t      is_first_sps;
    int8_t      is_first_pps;
    int8_t		hw_dblk_triger_thres;
    uint32_t     pic_num;
    /* only used for test*/
    int32_t     vld_mode;
    int32_t     fmo_mode;
    
    uint32_t    bs_start_addr;
    
    uint32_t    mv;
    
    //Will Add
    AVC_FRAME_STORE_t   OutputFrame[1+MAX_LIST_SIZE];
    int32_t     OutIdxW;
    int32_t     OutIdxR;
    int32_t     OutputCount;
    int8_t      track_no;
	Sequence_Parameter_Set_t	sps_array_tmp[MAX_TRACK_NUM];
	Picture_Parameter_Set_t		pps_array_tmp[MAX_TRACK_NUM];

} AVC_DEC;

#endif

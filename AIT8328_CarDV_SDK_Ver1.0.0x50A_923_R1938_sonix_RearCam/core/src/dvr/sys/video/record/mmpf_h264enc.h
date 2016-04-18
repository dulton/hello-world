//==============================================================================
//
//  File        : mmpf_h264enc.h
//  Description : Header function of video codec
//  Author      : Will Tseng
//  Revision    : 1.0
//
//==============================================================================

#ifndef _MMPF_H264ENC_H_
#define _MMPF_H264ENC_H_

#include "includes_fw.h"
#include "mmpf_3gpmgr.h"
#include "mmpf_vidcmn.h"

/** @addtogroup MMPF_VIDEO
@{
*/

//==============================================================================
//
//                              COMPILER OPTION
//
//==============================================================================

#define BD_LOW                  (0)
#define BD_HIGH                 (1)

#define NEW_BITRATE_CTL		    (1)
#define FPS_RC_CTL              (1) // Comment: Suggest to close it if no need to dynamic change FPS.

//==============================================================================
//
//                              CONSTANTS
//
//==============================================================================

#if (SUPPORT_VUI_INFO == 1)
#define VUI_HRD_MAX_CPB_CNT     (1)
#define SUPPORT_POC_TYPE_1      (0)
#define MAX_PARSET_BUF_SIZE     (256)
#define H264E_STARTCODE_LEN     (4)
#endif

//==============================================================================
//
//                              ENUMERATION
//
//==============================================================================

typedef enum _MMPF_H264ENC_INTRA_REFRESH_MODE {
    MMPF_H264ENC_INTRA_REFRESH_DIS = 0,
    MMPF_H264ENC_INTRA_REFRESH_MB,
    MMPF_H264ENC_INTRA_REFRESH_ROW
} MMPF_H264ENC_INTRA_REFRESH_MODE;

typedef enum _MMPF_H264ENC_ENTROPY_MODE {
    MMPF_H264ENC_ENTROPY_CAVLC = 0,
    MMPF_H264ENC_ENTROPY_CABAC,
    MMPF_H264ENC_ENTROPY_NONE
} MMPF_H264ENC_ENTROPY_MODE;
typedef enum _MMPF_H264ENC_PADDING_TYPE {
    MMPF_H264ENC_PADDING_NONE,
    MMPF_H264ENC_PADDING_ZERO,
    MMPF_H264ENC_PADDING_REPEAT
} MMPF_H264ENC_PADDING_TYPE;

typedef enum _MMPF_H264ENC_HBR_MODE {
    MMPF_H264ENC_HBR_MODE_60P,
    MMPF_H264ENC_HBR_MODE_30P,
    MMPF_H264ENC_HBR_MODE_MAX
} MMPF_H264ENC_HBR_MODE;

#if (SUPPORT_VUI_INFO == 1)
typedef enum _MMPF_H264ENC_SEI_TYPE {
    MMPF_H264ENC_SEI_TYPE_BUF_PERIOD      = 0x0001,
    MMPF_H264ENC_SEI_TYPE_PIC_TIMING      = 0x0002,
    MMPF_H264ENC_SEI_TYPE_USER_DATA_UNREG = 0x0020,
    MMPF_H264ENC_SEI_TYPE_MAX             = 0x0040
} MMPF_H264ENC_SEI_TYPE;

typedef enum _MMPF_H264ENC_BYTESTREAM_TYPE {
    MMPF_H264ENC_BYTESTREAM_ANNEXB= 0,
    MMPF_H264ENC_BYTESTREAM_NALU_EBSP,
    MMPF_H264ENC_BYTESTREAM_NALU_RBSP
} MMPF_H264ENC_BYTESTREAM_TYPE;

typedef enum _MMPF_H264ENC_NALU_TYPE {
    H264_NALU_TYPE_SLICE    = 1,
    H264_NALU_TYPE_DPA      = 2,
    H264_NALU_TYPE_DPB      = 3,
    H264_NALU_TYPE_DPC      = 4,
    H264_NALU_TYPE_IDR      = 5,
    H264_NALU_TYPE_SEI      = 6,
    H264_NALU_TYPE_SPS      = 7,
    H264_NALU_TYPE_PPS      = 8,
    H264_NALU_TYPE_AUD      = 9,
    H264_NALU_TYPE_EOSEQ    = 10,
    H264_NALU_TYPE_EOSTREAM = 11,
    H264_NALU_TYPE_FILL     = 12,
    H264_NALU_TYPE_SPSEXT   = 13,
    H264_NALU_TYPE_PREFIX   = 14,
    H264_NALU_TYPE_SUBSPS   = 15
} MMPF_H264ENC_NALU_TYPE;

typedef enum _MMPF_H264ENC_NAL_REF_IDC{
    H264_NALU_PRIORITY_HIGHEST     = 3,
    H264_NALU_PRIORITY_HIGH        = 2,
    H264_NALU_PRIORITY_LOW         = 1,
    H264_NALU_PRIORITY_DISPOSABLE  = 0
} MMPF_H264ENC_NAL_REF_IDC;
#endif


//==============================================================================
//
//                              STRUCTURES
//
//==============================================================================

typedef struct _MMPF_H264ENC_PADDING_INFO {
    MMPF_H264ENC_PADDING_TYPE type;
    MMP_UBYTE ubPaddingCnt;
} MMPF_H264ENC_PADDING_INFO;

typedef struct _MMPF_H264ENC_MEMD_PARAM {
    MMP_USHORT  usMeStopThr[2];             ///< 0: low, 1: high
    MMP_USHORT  usMeSkipThr[2];             ///< 0: low, 1: high
} MMPF_H264ENC_MEMD_PARAM;

#if (SUPPORT_VUI_INFO == 1)
typedef struct _MMPF_H264ENC_SEI_PARAM {
    struct {
        MMP_ULONG init_cpb_removal_delay[2][VUI_HRD_MAX_CPB_CNT];
        MMP_ULONG init_cpb_removal_delay_offset[2][VUI_HRD_MAX_CPB_CNT];
    } BUF_PERIOD;
    struct {
        MMP_ULONG cpb_removal_delay;
        MMP_ULONG dpb_output_delay; 
    } PIC_TIMING;
} MMPF_H264ENC_SEI_PARAM;

typedef struct _MMPF_H264ENC_SYNTAX_ELEMENT {
    MMP_LONG    type;           //!< type of syntax element for data part.
    MMP_LONG    value1;         //!< numerical value of syntax element
    MMP_LONG    value2;         //!< for blocked symbols, e.g. run/level
    MMP_LONG    len;            //!< length of code
    MMP_LONG    inf;            //!< info part of UVLC code
    MMP_ULONG   bitpattern;     //!< UVLC bitpattern
    MMP_LONG    context;        //!< CABAC context

  //!< for mapping of syntaxElement to UVLC
  //void    (*mapping)(int value1, int value2, int* len_ptr, int* info_ptr);
} MMPF_H264ENC_SYNTAX_ELEMENT;

typedef struct _MMPF_H264ENC_NALU_INFO {
    //MMP_ULONG   startcodeprefix_len;
    MMPF_H264ENC_NALU_TYPE nal_unit_type;
    MMPF_H264ENC_NAL_REF_IDC nal_ref_idc;
    //MMP_ULONG   forbidden_bit;  ///< should always 0
    MMP_UBYTE   temporal_id;    ///< SVC extension
} MMPF_H264ENC_NALU_INFO;

typedef struct _MMPF_H264ENC_BS_INFO {
    //MMP_LONG buffer_size;       ///< BS buffer size      
    MMP_LONG byte_pos;          ///< current position in bitstream;
    MMP_LONG bits_to_go;        ///< current bitcounter

    //MMP_LONG stored_byte_pos;   ///< storage for position in bitstream;
    //MMP_LONG stored_bits_to_go; ///< storage for bitcounter
    //MMP_LONG byte_pos_skip;     ///< storage for position in bitstream;
    //MMP_LONG bits_to_go_skip;   ///< storage for bitcounter
    //MMP_LONG write_flag;        ///< Bitstream contains data and needs to be written

    MMP_UBYTE byte_buf;         ///< current buffer for last written byte
    //MMP_UBYTE stored_byte_buf;  ///< storage for buffer of last written byte
    //MMP_UBYTE byte_buf_skip;    ///< current buffer for last written byte
    MMP_UBYTE *streamBuffer;    ///< actual buffer for written bytes
} MMPF_H264ENC_BS_INFO;

typedef struct _MMPF_H264ENC_HRD_INFO {
    MMP_ULONG cpb_cnt_minus1;                                   // ue(v)
    MMP_ULONG bit_rate_scale;                                   // u(4)
    MMP_ULONG cpb_size_scale;                                   // u(4)
    MMP_ULONG bit_rate_value_minus1 [VUI_HRD_MAX_CPB_CNT];  	// ue(v)
    MMP_ULONG cpb_size_value_minus1 [VUI_HRD_MAX_CPB_CNT];  	// ue(v)
    MMP_ULONG cbr_flag              [VUI_HRD_MAX_CPB_CNT];  	// u(1)
    MMP_ULONG initial_cpb_removal_delay_length_minus1;          // u(5)
    MMP_ULONG cpb_removal_delay_length_minus1;                  // u(5)
    MMP_ULONG dpb_output_delay_length_minus1;                   // u(5)
    MMP_ULONG time_offset_length;                               // u(5)
} MMPF_H264ENC_HRD_INFO;

typedef struct _MMPF_H264ENC_VUI_INFO {
    MMP_BOOL  aspect_ratio_info_present_flag;               	// u(1)
    MMP_ULONG aspect_ratio_idc;                             	// u(8)
    MMP_ULONG sar_width;                                		// u(16)
    MMP_ULONG sar_height;                               		// u(16)
    MMP_BOOL  overscan_info_present_flag;                   	// u(1)
    MMP_BOOL  overscan_appropriate_flag;                		// u(1)
    MMP_BOOL  video_signal_type_present_flag;               	// u(1)
   	MMP_ULONG video_format;                                   	// u(3) 0:component, 1:PAL, 2:NTSC, 3:SECAM, 4:MAC 5:Unspecified video format
    MMP_BOOL  video_full_range_flag;                        	// u(1)
    MMP_BOOL  colour_description_present_flag;              	// u(1)
    MMP_ULONG colour_primaries;                             	// u(8)
    MMP_ULONG transfer_characteristics;                     	// u(8)
    MMP_ULONG matrix_coefficients;                          	// u(8)
    MMP_BOOL  chroma_location_info_present_flag;                // u(1)
    MMP_ULONG chroma_sample_loc_type_top_field;               	// ue(v)
    MMP_ULONG chroma_sample_loc_type_bottom_field;            	// ue(v)
    MMP_BOOL  timing_info_present_flag;                         // u(1)
    MMP_ULONG num_units_in_tick;                              	// u(32)
    MMP_ULONG time_scale;                                     	// u(32)
    MMP_BOOL  fixed_frame_rate_flag;                          	// u(1)
    MMP_BOOL  nal_hrd_parameters_present_flag;                  // u(1)
    MMPF_H264ENC_HRD_INFO nal_hrd_parameters;                   // hrd_paramters_t
    MMP_BOOL  vcl_hrd_parameters_present_flag;                  // u(1)
    MMPF_H264ENC_HRD_INFO vcl_hrd_parameters;                   // hrd_paramters_t
    // if ((nal_hrd_parameters_present_flag || (vcl_hrd_parameters_present_flag))
    MMP_BOOL  low_delay_hrd_flag;                             	// u(1)
    MMP_BOOL  pic_struct_present_flag;                        	// u(1)
    MMP_BOOL  bitstream_restriction_flag;                       // u(1)
    MMP_BOOL  motion_vectors_over_pic_boundaries_flag;        	// u(1)
    MMP_ULONG max_bytes_per_pic_denom;                        	// ue(v)
    MMP_ULONG max_bits_per_mb_denom;                          	// ue(v)
    MMP_ULONG log2_max_mv_length_vertical;                    	// ue(v)
    MMP_ULONG log2_max_mv_length_horizontal;                  	// ue(v)
    MMP_ULONG num_reorder_frames;                             	// ue(v)
    MMP_ULONG max_dec_frame_buffering;                        	// ue(v)
} MMPF_H264ENC_VUI_INFO;

typedef struct _MMPF_H264ENC_SPS_INFO {
    MMP_BOOL    Valid; // indicates the parameter set is valid

    MMP_ULONG   profile_idc;                                    // u(8)
    MMP_BOOL    constrained_set0_flag;                          // u(1)
    MMP_BOOL    constrained_set1_flag;                          // u(1)
    MMP_BOOL    constrained_set2_flag;                          // u(1)
    MMP_BOOL    constrained_set3_flag;                          // u(1)
    MMP_BOOL    constrained_set4_flag;                          // u(1)
    MMP_BOOL    constrained_set5_flag;                          // u(1)
    MMP_BOOL    constrained_set6_flag;                          // u(1)
    MMP_ULONG   level_idc;                                      // u(8)
    MMP_ULONG   seq_parameter_set_id;                           // ue(v)
    MMP_ULONG   chroma_format_idc;                              // ue(v)

    MMP_BOOL    seq_scaling_matrix_present_flag;                // u(1) => always 0
        //MMP_LONG   seq_scaling_list_present_flag[12];         // u(1)

    MMP_ULONG   bit_depth_luma_minus8;                          // ue(v)
    MMP_ULONG   bit_depth_chroma_minus8;                        // ue(v)
    MMP_ULONG   log2_max_frame_num_minus4;                      // ue(v)
    MMP_ULONG   pic_order_cnt_type;
    // if( pic_order_cnt_type == 0 )
    MMP_ULONG   log2_max_pic_order_cnt_lsb_minus4;              // ue(v)
    // else if( pic_order_cnt_type == 1 )
    #if (SUPPORT_POC_TYPE_1 == 1)
    MMP_BOOL delta_pic_order_always_zero_flag;                  // u(1)
    MMP_LONG    offset_for_non_ref_pic;                     	// se(v)
    MMP_LONG    offset_for_top_to_bottom_field;             	// se(v)
    MMP_ULONG   num_ref_frames_in_pic_order_cnt_cycle;      	// ue(v)
    // for( i = 0; i < num_ref_frames_in_pic_order_cnt_cycle; i++ )
    MMP_LONG    offset_for_ref_frame[MAX_REF_FRAME_IN_POC_CYCLE];   // se(v)
    #endif //(SUPPORT_POC_TYPE_1 == 1)
    MMP_ULONG   num_ref_frames;                                 // ue(v)
    MMP_BOOL    gaps_in_frame_num_value_allowed_flag;           // u(1)
    MMP_ULONG   pic_width_in_mbs_minus1;                        // ue(v)
    MMP_ULONG   pic_height_in_map_units_minus1;                 // ue(v)
    MMP_BOOL    frame_mbs_only_flag;                            // u(1)
    // if( !frame_mbs_only_flag )
    MMP_BOOL    mb_adaptive_frame_field_flag;                   // u(1)
    MMP_BOOL    direct_8x8_inference_flag;                      // u(1)
    MMP_BOOL    frame_cropping_flag;                            // u(1)
    MMP_ULONG   frame_cropping_rect_left_offset;                // ue(v)
    MMP_ULONG   frame_cropping_rect_right_offset;               // ue(v)
    MMP_ULONG   frame_cropping_rect_top_offset;                 // ue(v)
    MMP_ULONG   frame_cropping_rect_bottom_offset;              // ue(v)
    MMP_BOOL    vui_parameters_present_flag;                    // u(1)
    #if (SUPPORT_VUI_INFO == 1)
        MMPF_H264ENC_VUI_INFO vui_seq_parameters;
    #endif
} MMPF_H264ENC_SPS_INFO;
#endif

typedef struct _MMPF_H264ENC_FUNC_STATES {
    MMP_USHORT                 ProfileIdc;
    MMPF_H264ENC_ENTROPY_MODE  EntropyMode;
} MMPF_H264ENC_FUNC_STATES;

typedef struct _MMPF_H264ENC_MODULE {
    MMP_BOOL                      bWorking;
    struct _MMPF_H264ENC_ENC_INFO *pH264Inst;
    MMPF_H264ENC_FUNC_STATES      HwState;
} MMPF_H264ENC_MODULE;

#if (CHIP == MCR_V2)
//Image Compression
#define ICOMP_LSY_STATIC_REDUCTION_IDX      8
#define ICOMP_LSY_BLK_SIZE_IDX              0
#define ICOMP_LSY_YUVRNG_IDX                1
#define ICOMP_LSY_MAX_LVL_IDX               2
#define ICOMP_LSY_LUMA_BLK_SIZE_OFFSET      3       ///< The offset to change the block size setting value to fit hardware opr value(0:4x4, 1: 8x8) 
#define ICOMP_LSY_CHR_BLK_SIZE_OFFSET       2
#define ICOMP_LSY_YUVRNG_OFFSET             1
#define ICOMP_LSY_MAX_LVL_OFFSET            5
#define ICOMP_LSY_MEAN_OFFSET               0
#define ICOMP_LSY_CONTRAST_OFFSET           1
#define ICOMP_LSY_DIFF_THR_OFFSET           2

typedef struct _MMPF_H264ENC_ICOMP {
    MMP_BOOL bICompEnable;
    MMP_BOOL bICompCurMbLsyEn;
    MMP_BOOL bICompLsyLvlCtlEn;
    MMP_UBYTE ubICompRatio;
    MMP_UBYTE ubICompRatioIndex;
    MMP_UBYTE ubICompMinLsyLvlLum;
    MMP_UBYTE ubICompMinLsyLvlChr;
    MMP_UBYTE ubICompIniLsyLvlLum;
    MMP_UBYTE ubICompIniLsyLvlChr;
    MMP_UBYTE ubICompMaxLsyLvlLum;
    MMP_UBYTE ubICompMaxLsyLvlChr;
    MMP_ULONG ulICompFrmSize;
} MMPF_H264ENC_ICOMP;
#endif

#if NEW_BITRATE_CTL

#define I_FRAME                             (MMPF_3GPMGR_FRAME_TYPE_I)
#define P_FRAME                             (MMPF_3GPMGR_FRAME_TYPE_P)
#define B_FRAME                             (MMPF_3GPMGR_FRAME_TYPE_B)

#define WINDOW_SIZE                         (20)
#define MAX_SUPPORT_LAYER                   (MAX_NUM_TMP_LAYERS)


typedef struct {
    MMP_ULONG   LayerBitRate[MAX_SUPPORT_LAYER];
    MMP_ULONG   BitRate[MAX_SUPPORT_LAYER];
    MMP_ULONG   VBV_LayerSize[MAX_SUPPORT_LAYER];
    MMP_ULONG   VBV_size[MAX_SUPPORT_LAYER];
    MMP_LONG    VBV_fullness[MAX_SUPPORT_LAYER];
    MMP_ULONG   TargetVBVLevel[MAX_SUPPORT_LAYER];
    MMP_ULONG   TargetVBV[MAX_SUPPORT_LAYER];
    MMP_ULONG   VBVRatio[MAX_SUPPORT_LAYER];
} VBV_PARAM;

typedef struct {
    MMP_ULONG   bitrate;
    MMP_LONG    intra_period;
    MMP_LONG    VBV_fullness;
    MMP_LONG    VBV_size;
    MMP_LONG    target_framesize;

    MMP_LONG    bits_budget;
    MMP_LONG    total_frames;
    MMP_LONG    left_frames[3];
    MMP_LONG    Iframe_num;
    MMP_LONG    Pframe_num;
    MMP_LONG    total_framesize[3];
    //MMP_LONG    total_I_size;
    //MMP_LONG    total_P_size;
    //MMP_LONG    total_B_size;
    //MMP_LONG    last_qp;
    //MMP_LONG    lastXP;

    MMP_LONG    last_X[3];
    MMP_LONG    last_X2[3];
    unsigned long long    X[3];
    MMP_LONG    X_Count[3];
    unsigned long long    X2[3][WINDOW_SIZE];
    MMP_LONG    X_Idx[3];
    MMP_LONG    count[3];
    MMP_LONG    frame_count;
    MMP_LONG    target_P;
    MMP_LONG    last_bits;
    //MMP_LONG    last_IQP;
    //MMP_LONG    last_Bqp;
    MMP_ULONG   lastQP[3];
    //MMP_LONG    clip_qp;

    MMP_LONG    prev_window_size[3];
    MMP_LONG    window_size[3];

    //MMP_LONG    AlphaI;
    //MMP_LONG    AlphaB;
    MMP_LONG    Alpha[3];
    //MMP_LONG    avg_xp;
    MMP_LONG    avg_qp[3];

    //MMP_LONG    is_vbr_mode;
    MMP_LONG    rc_mode;
    MMP_LONG    enable_vbr_mode;
    MMP_LONG    GOP_frame_count;
    MMP_LONG    GOP_count;
    MMP_LONG    GOP_totalbits;
    MMP_LONG    QP_sum;
    MMP_LONG    GOP_QP[3];
    MMP_LONG    GOP_left_frames[3];

    MMP_LONG    GOP_num_per_I_period;
    MMP_LONG    GOP_count_inside_I_period;
    
    MMP_LONG    last_headerbits[3];
    MMP_ULONG   nP;
    MMP_ULONG   nB;
    
    MMP_ULONG   header_bits[3][WINDOW_SIZE];
    MMP_ULONG   header_index[3];
    MMP_ULONG   header_count[3];
    MMP_ULONG   avg_header[3];
    
    MMP_ULONG   avgXP[3];
    //test
    MMP_LONG    budget;
    MMP_ULONG   targetPSize;
    
    MMP_LONG    vbr_budget;
    MMP_LONG    vbr_total_frames;
	MMP_ULONG	framerate;
	MMP_BOOL	SkipFrame;
	
	MMP_LONG	GOPLeftFrames;
	MMP_BOOL	bResetRC;
	MMP_ULONG   light_condition;
	
	//MMP_LONG	TargetLowerBound;
	//MMP_LONG	TargetUpperBound;
	
	MMP_ULONG   MaxQPDelta[3];
	MMP_ULONG	MaxWeight[3];		//1.5 * 1000
	MMP_ULONG	MinWeight[3];		//1.0 * 1000
	MMP_ULONG	VBV_Delay;			//500 ms
	MMP_ULONG	TargetVBVLevel;		//250 ms
	MMP_ULONG   FrameCount;
	MMP_BOOL    SkipPrevFrame;
	MMP_ULONG   SkipFrameThreshold;
	
	MMP_ULONG   m_LowerQP[3];
    MMP_ULONG   m_UpperQP[3];
    MMP_ULONG   m_VideoFormat;
    MMP_ULONG   m_ResetCount;
    MMP_ULONG   m_GOP;
    MMP_ULONG   m_Budget;
    MMP_ULONG64 m_AvgHeaderSize;
    MMP_ULONG   m_lastQP;
    MMP_UBYTE   m_ubFormatIdx;
    MMP_ULONG   MBWidth;
	MMP_ULONG   MBHeight;
	MMP_ULONG   TargetVBV;
	MMP_BOOL    bPreSkipFrame;
	
	MMP_ULONG	VBVRatio;
	MMP_ULONG	bUseInitQP;
	
	MMP_ULONG   m_LastType;
	
	//++Will RC
	void*       pGlobalVBV;
	MMP_ULONG   LayerRelated;
	MMP_ULONG   Layer;
	//--Will RC
} RC;

typedef struct {
	MMP_ULONG	MaxIWeight;			//1.5 * 1000
	MMP_ULONG	MinIWeight;			//1.0 * 1000
	MMP_ULONG	MaxBWeight;			//1.0 * 1000
	MMP_ULONG	MinBWeight;			//0.5 * 1000
	MMP_ULONG	VBV_Delay;			//500 ms
	MMP_ULONG	TargetVBVLevel;		//250 ms
	MMP_ULONG	InitWeight[3];
	MMP_ULONG	MaxQPDelta[3];
	MMP_ULONG   SkipFrameThreshold;
	MMP_ULONG   MBWidth;
	MMP_ULONG   MBHeight;
	MMP_ULONG   InitQP[3];
	MMP_ULONG   rc_mode;
	MMP_ULONG   bPreSkipFrame;
	
	//++Will RC
	MMP_ULONG   LayerRelated;
	MMP_ULONG   Layer;
	MMP_ULONG   EncID;
	//--Will RC
} RC_CONFIG_PARAM;
#endif

typedef struct _MMPF_H264ENC_ENC_INFO {
    // Sequence Level
    MMP_ULONG                 enc_id;
    #if (SUPPORT_VUI_INFO == 1)
    MMPF_H264ENC_SPS_INFO     sps;
    MMP_UBYTE                 *glVidRecdTmpSPSAddr;
    MMP_ULONG                 glVidRecdTmpSPSSize;
    #endif
    MMP_UBYTE                 *VidRecdSPSAddr;
    MMP_ULONG                 VidRecdSPSSize;
    MMP_UBYTE                 *VidRecdPPSAddr;
    MMP_ULONG                 VidRecdPPSSize;

    MMP_USHORT                profile;
    MMP_ULONG                 mb_num;
    MMP_USHORT                mb_w;
    MMP_USHORT                mb_h;
    MMPF_H264ENC_PADDING_INFO paddinginfo;
	#if (H264ENC_ICOMP_EN)
    MMPF_H264ENC_ICOMP		  ICompConfig;
	#endif
    MMP_USHORT                level;
    MMP_USHORT                p_frame_num;
    MMP_USHORT                b_frame_num;
    MMP_USHORT                gop_size;
    MMP_ULONG                 mv_addr;
    MMP_BOOL				  co_mv_valid;
    MMP_BOOL                  insert_skip_frame;	 ///< insert skip frame to prevent overflow.(gbInsertSkipFrame)
    MMP_BOOL                  insert_sps_pps;
    MMP_ULONG                 stream_bitrate;		 ///< total bitrate

    RC_CONFIG_PARAM           rc_config;			 ///< config for rc temporal layers
    void                      *rc_hdl;				 ///< rc handler

    MMPF_H264ENC_ENTROPY_MODE entropy_mode;
    MMPF_VIDENC_CROPPING      crop;

    MMP_ULONG                 total_frames;          ///< total encoded frames//TODO//ulTotalEncCount
    MMP_ULONG                 prev_ref_num;          ///< increase when cur frame can be ref
    MMPF_VIDENC_CURBUF_MODE   CurBufMode;
    MMPF_H264ENC_MEMD_PARAM   Memd;
    MMP_UBYTE                 qp_tune_mode;          ///< mb, row, slice, frame
    MMP_UBYTE                 qp_tune_size;          ///< unit size
    MMP_USHORT                inter_cost_th;         ///< reset 1024
    MMP_USHORT                intra_cost_adj;        ///< reset 18
    MMP_USHORT                inter_cost_bias;       ///< reset 0
    MMP_USHORT                me_itr_steps;
    MMP_ULONG                 temporal_id;           ///< I:0, P:1(ref),2(non-ref)
    MMP_ULONG64               timestamp;
    #if (SUPPORT_VUI_INFO == 1)
    MMPF_H264ENC_SEI_CTL      SeiCtl;                ///< sei control
    MMPF_H264ENC_SEI_PARAM    sei_param;             ///< parameter for supported sei message
    #endif
    MMP_BOOL                  video_full_range;      ///< indicate video input is 16-255
    #if (CHIP == MCR_V2)
    MMP_BOOL                  bRdoEnable;
    MMP_BOOL                  bRdoMadValid;
    MMP_ULONG                 ulRdoMadFrmPre;
    MMP_ULONG                 ulRdoInitFac;
    MMP_ULONG                 ulRdoMadLastRowPre;
    MMP_UBYTE                 ubRdoMissThrLoAdj;
    MMP_UBYTE                 ubRdoMissThrHiAdj;
    MMP_UBYTE                 ubRdoMissTuneQp;
    #endif
    MMP_LONG                  MbQpBound[MMPF_3GPMGR_FRAME_TYPE_MAX][2];
    MMP_LONG                  CurRcQP[MMPF_3GPMGR_FRAME_TYPE_MAX];

    MMP_ULONG                 ulParamQueueRdWrapIdx; ///< MS3Byte wrap, LSByte idx
    MMP_ULONG                 ulParamQueueWrWrapIdx; ///< MS3Byte wrap, LSByte idx
    MMPF_VIDENC_PARAM_CTL     ParamQueue[MAX_NUM_PARAM_CTL];
    MMPF_3GPMGR_FRAME_TYPE    OpFrameType;
    MMP_BOOL                  OpIdrPic;
    MMP_BOOL                  OpInsParset;
    MMP_UBYTE                 CurRTSrcPipeId;

    // runtime, slice level
    #if (SUPPORT_VUI_INFO == 1)
    MMPF_H264ENC_NALU_INFO    nalu;
    #endif
    MMP_USHORT                usMaxNumRefFrame;
    MMP_UBYTE                 usMaxFrameNumAndPoc;

    MMP_ULONG                 slice_addr;

    MMP_ULONG                RefGenBufLowBound;
    MMP_ULONG                RefGenBufHighBound;
    MMPF_VIDENC_FRAMEBUF_BD  RefBufBound;
    MMPF_VIDENC_FRAMEBUF_BD  GenBufBound;

    MMPF_3GPMGR_FRAME_TYPE   cur_frm_type;
    MMPF_VIDENC_FRAME        cur_frm[4];        ///< frame buffer addr and timestamp info
    MMPF_VIDENC_FRAME        ref_frm;           //m_RefGenFrame
    MMPF_VIDENC_FRAME        rec_frm;

    MMP_UBYTE                enc_frm_buf;       ///< buffer idx for encode

	MMP_ULONG                ulFrameThres;      ///< frame threshold
	MMP_ULONG                ulTargSize;        ///< bitrate = ulTargSize*usframe_rate
    MMP_ULONG                ulMaxFpsRes;       ///< host controled max fps
    MMP_ULONG                ulMaxFpsInc;       ///< host controled max fps
    MMP_ULONG                ulMaxFps1000xInc;  ///< host controled max fps
    MMP_ULONG                ulEncFpsRes;       ///< host controled max fps
    MMP_ULONG                ulEncFpsInc;       ///< host controled max fps
    MMP_ULONG                ulEncFps1000xInc;  ///< host controled max fps
    MMP_ULONG                ulSnrFpsRes;       ///< host controled max fps
    MMP_ULONG                ulSnrFpsInc;       ///< host controled max fps
    MMP_ULONG                ulSnrFps1000xInc;  ///< host controled max fps
    MMP_ULONG                ulFpsInputRes;     ///< input framerate resolution
    MMP_ULONG                ulFpsInputInc;     ///< input framerate increament
    MMP_ULONG                ulFpsInputAcc;     ///< input framerate accumulate
    MMP_ULONG                ulFpsOutputRes;    ///< enc framerate resolution
    MMP_ULONG                ulFpsOutputInc;    ///< enc framerate increament
    MMP_ULONG                ulFpsOutputAcc;    ///< enc framerate accumulate

    MMP_ULONG                ulOutputAvailSize;
    MMP_ULONG                cur_frm_bs_addr;   ///< bs addr for current enc frame
    MMP_ULONG                cur_frm_bs_high_bd;///< bs addr for current enc frame
    MMP_ULONG                cur_frm_bs_low_bd; ///< bs addr for current enc frame
    MMP_ULONG                cur_frm_wr_size;   ///< accumulating size write to bs buf
    MMP_ULONG                cur_frm_rd_size;   ///< accumulating size read from bs buf
    MMP_ULONG                cur_frm_parset_num;///< hw sps/pps slice num
    MMP_ULONG                cur_frm_slice_num; ///< hw total slice num(incluing parset)
    MMP_ULONG                cur_frm_slice_idx; ///<

    // buffer addr and buffer for modify header
    MMP_ULONG                cur_slice_wr_idx;  ///<
    MMP_ULONG                cur_slice_rd_idx;  ///<
    MMP_ULONG                cur_slice_xhdr_sum;///< generated slice header size sum
    MMP_ULONG                cur_slice_parsed_sum;///< parsed slice header size sum

    MMP_ULONG                sps_len;
    MMP_ULONG                pps_len;
    #if (SUPPORT_VUI_INFO == 1)
    MMP_UBYTE                sps_buf[MAX_PARSET_BUF_SIZE];
    MMP_UBYTE                pps_buf[MAX_PARSET_BUF_SIZE];
    MMP_UBYTE                sei_buf[MAX_PARSET_BUF_SIZE];
    #endif

    VidEncEndCallBackFunc    *EncReStartCallback;
    VidEncEndCallBackFunc    *EncEndCallback;

    MMP_USHORT               Operation;        ///< operation from host command.
    MMP_USHORT               Status;           ///< status of video engine.
    MMP_UBYTE                VideoCaptureStart;///< flag of start record.
    MMPF_H264ENC_MODULE      *module;
    void                     *priv_data;
} MMPF_H264ENC_ENC_INFO;

//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================

#if (SUPPORT_VUI_INFO == 1)
MMP_UBYTE MMPF_H264ENC_GenerateNaluHeader (MMPF_H264ENC_NALU_INFO *nalu_inf);
void MMPF_H264ENC_GetUserDataUnreg(MMP_ULONG* ulAGC, MMP_ULONG* ulWBMode, MMP_ULONG* ulShutter, MMP_ULONG ShuMode, MMPF_CONTAINER_INFO *pContainer);
MMP_ULONG MMPF_H264ENC_GenerateSEI(MMP_UBYTE *nalu_buf, MMPF_H264ENC_SEI_PARAM *pSeiParam,
                                   MMPF_H264ENC_SEI_TYPE PayloadType, MMPF_H264ENC_BYTESTREAM_TYPE ByteStrType,
								   MMPF_H264ENC_ENC_INFO *pEnc, MMPF_CONTAINER_INFO *pContainer);
MMP_ULONG MMPF_H264ENC_GenerateAUD(MMP_UBYTE *nalu_buf, MMPF_3GPMGR_FRAME_TYPE fr_type,
				                   MMPF_H264ENC_BYTESTREAM_TYPE ByteStrType);
MMP_ULONG MMPF_H264ENC_GenerateSPS (MMP_UBYTE *nalu_buf, MMPF_H264ENC_SPS_INFO *sps);
#endif

MMPF_H264ENC_ENC_INFO *MMPF_H264ENC_GetHandle(MMP_UBYTE ubEncId);
MMP_ERR MMPF_H264ENC_SetCropping(MMPF_H264ENC_ENC_INFO *pEnc, MMP_BOOL bEnable);

MMP_ERR MMPF_H264ENC_SetPadding(MMP_ULONG ulEncId, MMPF_H264ENC_PADDING_TYPE ubPaddingType, MMP_UBYTE ubPaddingCnt);
MMP_ERR MMPF_H264ENC_InitImageComp(MMPF_H264ENC_ENC_INFO *pEnc, MMPF_H264ENC_ICOMP *pConfig);
MMP_ERR MMPF_H264ENC_SetH264ByteCnt(MMP_USHORT usByteCnt);
MMP_ERR MMPF_H264ENC_EnableTnr(MMPF_H264ENC_ENC_INFO *pEnc, MMP_BOOL bEnable);
MMP_ERR MMPF_H264ENC_SetIntraRefresh(MMPF_H264ENC_INTRA_REFRESH_MODE ubMode, MMP_USHORT usPeriod, MMP_USHORT usOffset);

MMP_ERR MMPF_H264ENC_SetBSBuf(MMP_UBYTE ubEncId, MMP_ULONG ulLowBound, MMP_ULONG ulHighBound);
MMP_ERR MMPF_H264ENC_SetMiscBuf(MMP_UBYTE ubEncId, MMP_ULONG ulMVBuf, MMP_ULONG ulSliceLenBuf);
MMP_ERR MMPF_H264ENC_SetRefListBound(MMP_UBYTE ubEncId, MMP_ULONG ulLowBound, MMP_ULONG ulHighBound);
MMP_ERR MMPF_H264ENC_SetSetHeaderBuf(MMP_UBYTE ubEncId, MMP_ULONG ulSPSStart, MMP_ULONG ulPPSStart, MMP_ULONG ulTmpSPSStart);
MMP_ERR MMPF_H264ENC_SetParameter (MMPF_H264ENC_ENC_INFO *pEnc, MMPF_VIDENC_ATTRIBUTE attrib, void *arg);
MMP_ERR MMPF_H264ENC_GetParameter(MMPF_H264ENC_ENC_INFO *pEnc, MMPF_VIDENC_ATTRIBUTE attrib, void *ulValue);
MMP_ERR MMPF_H264ENC_InitRCConfig (MMPF_H264ENC_ENC_INFO *pEnc);
MMP_ERR MMPF_H264ENC_SetQPBound(MMPF_H264ENC_ENC_INFO *pEnc, MMPF_3GPMGR_FRAME_TYPE type,
                                MMP_LONG lMinQp, MMP_LONG lMaxQp);
MMP_ERR MMPF_H264ENC_InitModule(MMPF_H264ENC_MODULE *pModule);
MMP_ERR MMPF_H264ENC_DeinitModule (MMPF_H264ENC_MODULE *pModule);
MMP_ERR MMPF_H264ENC_InitInstance(MMPF_H264ENC_ENC_INFO *enc, MMPF_H264ENC_MODULE *attached_mod, void *priv);
MMP_ERR MMPF_H264ENC_DeInitRCConfig(MMPF_H264ENC_ENC_INFO *enc, MMP_ULONG InstId);
MMP_ERR MMPF_H264ENC_InitRefListConfig (MMPF_H264ENC_ENC_INFO *pEnc);
#if (SUPPORT_VUI_INFO == 1)
MMP_ERR MMPF_H264ENC_InitSpsConfig(MMPF_H264ENC_ENC_INFO *enc);
#endif
MMP_ERR MMPF_H264ENC_Open(MMPF_H264ENC_ENC_INFO *pEnc);
MMP_ERR MMPF_H264ENC_Resume(MMPF_H264ENC_ENC_INFO *pEnc);
MMP_ERR MMPF_H264ENC_Close (MMPF_H264ENC_ENC_INFO *enc);
MMP_ERR MMPF_H264ENC_TriggerEncode(MMPF_H264ENC_ENC_INFO *pEnc, MMPF_3GPMGR_FRAME_TYPE FrameType, MMP_ULONG ulFrameTime, MMPF_VIDENC_FRAME *pFrameInfo);

#endif	// _MMPF_H264ENC_H_

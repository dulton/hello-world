//==============================================================================
//
//  File        : mmp_reg_h264dec.h
//  Description : INCLUDE File for the Retina register map.
//  Author      : Will Chen
//  Revision    : 1.0
//
//==============================================================================

#ifndef _MMP_REG_H264DEC_H_
#define _MMP_REG_H264DEC_H_

#include "mmp_register.h"

#define H264_BASE_ADR               0x8000C800
#define H264_REF_BASE_ADDR          0x8000C810
#define H264_REC_FRAME_ADDR         0x8000C860
#define H264_DBLK_BASE_ADDR         0x8000C870
#define H264_VLD_BASE_ADDR          0x8000C880
#define H264_VLD_DATA0_BASE_ADDR    0x8000C900
#define H264_VLD_DATA1_BASE_ADDR    0x8000C980
#define H264_MB_TO_SLICE_MAP_ADDR   0x80010000
#define H264_VLD_BUF0_ADDR          0x8000E800
#define H264_VLD_BUF1_ADDR          0x8000EC00

#define HW_VLD_MODE                 0x00
#define SW_VLD_MODE                 0x40
#define FMO_MODE                    0x02
#define NON_FMO_MODE                0x00

/** @addtogroup MMPH_reg
@{
*/
// *******************************
//   Display structure (0x8000 0000)
// *******************************
/* global register */
typedef struct _AITS_H264DEC_CTL{
    AIT_REG_B   FRAME_CTL_REG;                      //0x0000
                    #define AVC_DEC_DISABLE                     0x00
                    #define AVC_FMO_DISABLE                     0x00
                    #define AVC_DEC_ENABLE                      0x01
                    #define AVC_FMO_ENABLE                      0x02
                    #define AVC_NEW_FRAME_START                 0x04
                    #define AVC_PARSE_EP3_ENABLE                0x08
                    #define AVC_ENC_ENABLE                      0x10
                    #define AVC_ENC_DISABLE                     0x00

#define ENABLE_AVC_DEC(reg, is_avc_enable, is_fmo) { reg = (is_fmo | is_avc_enable); }
#define SET_NEW_FRAME_START(reg, new_frame_start) { (reg = reg | new_frame_start); }
    AIT_REG_B   _x0001;                             //0x0001
    AIT_REG_B   SW_RESET_CTL;                       //0x0002
                    #define VLD_RESET                           0x01
                    #define IDCT_RESET                          0x02
                    #define INTRA_PRED_RESET                    0x04
                    #define INTER_PRED_RESET                    0x08
                    #define FRAME_RECONSTRUCT_RESET             0x10
                    #define DBLK_RESET                          0x20
                    #define FLOW_CONTROL_RESET                  0x40

    AIT_REG_B   _x0003;
    AIT_REG_W   CPU_INT_ENABLE;					    //0x0004
                    #if 1
                    #define VLD_SEGMENT_DONE_CPU_STATUS        0x0001
                    #define DECODE_SLICE_DONE_CPU_STATUS       0x0002
                    #define DECODE_SLICE_ERROR_CPU_STATUS      0x0004
                    #define DECODE_TIMEOUT_ERROR_CPU_STATUS    0x0020
                    #define DBLK_FRAME_DONE_CPU_STATUS         0x0040
                    #define PARSE_EP3_DONE_STATUS              0x0080
                    #define SLICE_RC_DONE_STATUS               0x1000
                    #else                    
                    #define DECODE_SLICE_DONE_CPU_INT_ENB       0x0001
                    #define DECODE_SLICE_ERROR_CPU_INT_ENB      0x0002
                    #define MB_NR_OVERFLOW_CPU_INT_ENB          0x0004
                    #define MB_IDCT_DONE_CPU_INT_ENB            0x0008
                    #define INTRA_PRED_DONE_CPU_INT_ENB         0x0010
                    #define REBUILD_MB_DONE_CPU_INT_ENB         0x0020
                    #define FRAME_BUFFER_EMPTY_CPU_INT_ENB      0x0040
                    #define DBLK_FRAME_DONE_CPU_INT_ENB         0x0080
                    #define MV_PREDICTION_DONE_CPU_INT_ENB      0x0100
                    #define REFERENCE_FETCH_DONE_CPU_INT_ENB    0x0200
                    #define INTERPOLATION_DONE_CPU_INT_ENB      0x0400
                    #define RINGBUF_DBLK_DONE_CPU_INT_ENB       0x0800
                    #define VLD_SEGMENT_DONE_CPU_INT_ENB        0x1000
                    #define VLD_CODEWORD_MISS_CPU_INT_ENB       0x2000
                    #define SLICE_RC_DONE_ENB                   0x4000
                    #define PARSE_EP3_DONE_ENB                  0x8000
                    #endif
    AIT_REG_W   CPU_INT_STATUS;					    //0x0006
                    #if 1
                    #define VLD_SEGMENT_DONE_CPU_STATUS        0x0001
                    #define DECODE_SLICE_DONE_CPU_STATUS       0x0002
                    #define DECODE_SLICE_ERROR_CPU_STATUS      0x0004
                    #define DECODE_TIMEOUT_ERROR_CPU_STATUS    0x0020
                    #define DBLK_FRAME_DONE_CPU_STATUS         0x0040
                    #define PARSE_EP3_DONE_STATUS              0x0080
                    #define SLICE_RC_DONE_STATUS               0x1000
                    #else    
                    #define DECODE_SLICE_DONE_CPU_STATUS       0x0001
                    #define DECODE_SLICE_ERROR_CPU_STATUS      0x0002
                    #define MB_NR_OVERFLOW_CPU_STATUS          0x0004
                    #define MB_IDCT_DONE_CPU_STATUS            0x0008
                    #define INTRA_PRED_DONE_CPU_STATUS         0x0010
                    #define REBUILD_MB_DONE_CPU_STATUS         0x0020
                    #define FRAME_BUFFER_EMPTY_CPU_STATUS      0x0040
                    #define DBLK_FRAME_DONE_CPU_STATUS         0x0080
                    #define MV_PREDICTION_DONE_CPU_STATUS      0x0100
                    #define REFERENCE_FETCH_DONE_CPU_STATUS    0x0200
                    #define INTERPOLATION_DONE_CPU_STATUS      0x0400
                    #define RINGBUF_DBLK_DONE_CPU_STATUS       0x0800
                    #define VLD_SEGMENT_DONE_CPU_STATUS        0x1000
                    #define VLD_CODEWORD_MISS_CPU_STATUS       0x2000
                    #define SLICE_RC_DONE_STATUS               0x4000
                    #define PARSE_EP3_DONE_STATUS              0x8000
                    #endif
    AIT_REG_W   HOST_INT_ENABLE;				    //0x0008
                    #define DECODE_SLICE_DONE_HOST_INT_ENB      0x0001
                    #define DECODE_SLICE_ERROR_HOST_INT_ENB     0x0002
                    #define MB_NR_OVERFLOW_HOST_INT_ENB         0x0004
                    #define MB_IDCT_DONE_HOST_INT_ENB           0x0008
                    #define INTRA_PRED_DONE_HOST_INT_ENB        0x0010
                    #define REBUILD_MB_DONE_HOST_INT_ENB        0x0020
                    #define FRAME_BUFFER_EMPTY_HOST_INT_ENB     0x0040
                    #define DBLK_FRAME_DONE_HOST_INT_ENB        0x0080
                    #define MV_PREDICTION_DONE_HOST_INT_ENB     0x0100
                    #define REFERENCE_FETCH_DONE_HOST_INT_ENB   0x0200
                    #define INTERPOLATION_DONE_HOST_INT_ENB     0x0400
                    #define RINGBUF_DBLK_DONE_HOST_INT_ENB      0x0800
                    #define VLD_SEGMENT_DONE_HOST_INT_ENB       0x1000
                    #define VLD_CODEWORD_MISS_HOST_INT_ENB      0x2000
                    #define PARSE_EP3_DONE_HOST_INT_ENB         0x8000

    AIT_REG_W   HOST_INT_STATUS;				    //0x000A
                    #define DECODE_SLICE_DONE_HOST_STATUS      0x0001
                    #define DECODE_SLICE_ERROR_HOST_STATUS     0x0002
                    #define MB_NR_OVERFLOW_HOST_STATUS         0x0004
                    #define MB_IDCT_DONE_HOST_STATUS           0x0008
                    #define INTRA_PRED_DONE_HOST_STATUS        0x0010
                    #define REBUILD_MB_DONE_HOST_STATUS        0x0020
                    #define FRAME_BUFFER_EMPTY_HOST_STATUS     0x0040
                    #define DBLK_FRAME_DONE_HOST_STATUS        0x0080
                    #define MV_PREDICTION_DONE_HOST_STATUS     0x0100
                    #define REFERENCE_FETCH_DONE_HOST_STATUS   0x0200
                    #define INTERPOLATION_DONE_HOST_STATUS     0x0400
                    #define RINGBUF_DBLK_DONE_HOST_STATUS      0x0800
                    #define VLD_SEGMENT_DONE_HOST_STATUS       0x1000
                    #define VLD_CODEWORD_MISS_HOST_STATUS      0x2000
                    #define PARSE_EP3_DONE_HOST_INT_STATUS     0x8000

    AIT_REG_B       _x000B;
    AIT_REG_B       _x000C;
} AITS_H264DEC_CTL, *AITPS_H264DEC_CTL;

/* reference frame start address : 0x80000010 */
typedef struct _AITS_H264DEC_REF{
    AIT_REG_B   LIST0_MAP[8];           /* frame N+1 | frame N */
    AIT_REG_D   REFERENCE_ADDR[16];     /* 4th byte | 3rd byte | 2nd byte | 1st byte */
    AIT_REG_D   Y_SIZE;                 /* PYTHON = 20bits */
} AITS_H264DEC_REF, *AITPS_H264DEC_REF;

/* frame reconstruct register address : 0x80000060 */
typedef struct _AITS_H264DEC_REC{
    AIT_REG_D   REC_FRAME_ADDR;                     //0x0000
    AIT_REG_B   RING_BUF_SIZE;			            //0x0004
    AIT_REG_B   _x0005;
    AIT_REG_B   _x0006;
    AIT_REG_B   _x0007;
    AIT_REG_D   RING_BUF_LOWERBOUND_ADDR;	        //0x0008
    AIT_REG_D   RING_BUF_HIGHERBOUND_ADDR;	        //0x000C
} AITS_H264DEC_REC, *AITPS_H264DEC_REC;

/* deblocking register address : 0x80000070 */
typedef struct _AITS_H264DEC_DBLK{
    AIT_REG_D   SOURCE_ADDR;			            //0x0010
    AIT_REG_D   DEST_ADDR;			                //0x0014
    AIT_REG_B   SOURCE_SIZE_IN_MBROWS;              //0x0018
    AIT_REG_B   RESERVED3;
    AIT_REG_B   DEBLOCKING_CTRL;		            //0x001A
                    #define DBLK_TRIGGER_BY_HW      0x00
                    #define DBLK_START              0x01
                    #define DBLK_TRIGGER_BY_SW      0x02
                    #define DBLD_IDLE				0x08
                    #if (CHIP == MCR_V2)
                    #define DBLK_OUT_ROT_MODE       0x04
                    #define DBLK_UP_ROW_IN_FB       0x08
                    #define DBLK_ONE_MB_MODE        0x10
                    #define DBLK_OUT_EN_I           0x20
                    #define DBLK_OUT_EN_P           0x40
                    #define DBLK_OUT_EN_B           0x80
                    #endif

    AIT_REG_B   RESERVED4;

    /**/
    /* 64 bits * 8 * framesize_in_mbs */
    /* 8 bits | 8 bits | 8 bits reserved | 16 bits | 8 bits reserved | 16 bits */
    AIT_REG_D   DBLK_PARAMS_BUF_ADDR;		//0x1C
} AITS_H264DEC_DBLK, *AITPS_H264DEC_DBLK;

#if (CHIP == MCR_V2)
/* deblocking rotate mode register address : 0x800002D8 */
typedef struct _AITS_H264DEC_DBLK_ROT{
    AIT_REG_D   Y_LOWBD_ADDR;                       //0x02D8
    AIT_REG_D   Y_HIGHBD_ADDR;                      //0x02DC
    AIT_REG_D   UV_LOWBD_ADDR;                      //0x02E0
    AIT_REG_D   UV_HIGHBD_ADDR;                     //0x02E4
    AIT_REG_D   V_LOWBD_ADDR;                       //0x02E8
    AIT_REG_D   V_HIGHBD_ADDR;                      //0x02EC
    AIT_REG_D   UV_ST_ADDR;                         //0x02F0
    AIT_REG_D   V_ST_ADDR;                          //0x02F4
    AIT_REG_D   UP_ROW_ST_ADDR;                     //0x02F8
    AIT_REG_W   H264_TIMEOUT_CNT;                   //0x02FC
} AITS_H264DEC_DBLK_ROT, *AITPS_H264DEC_DBLK_ROT;
#endif

/* VLD register address : 0x80000080 */
typedef struct _AITS_H264DEC_VLD{
    AIT_REG_W   PICTURE_SIZE_IN_MBS;		        //0x0000
    AIT_REG_B   MB_WIDTH;			                //0x0002
    AIT_REG_B   RESERVED5;
    AIT_REG_B   MB_HEIGHT;			                //0x0004
    AIT_REG_B   RESERVED6;
    AIT_REG_B   ALPHA_OFFSET_DIV2;		            //0x0006
    AIT_REG_B   BETA_OFFSET_DIV2;		            //0x0007
    AIT_REG_B   QP;				                    //0x0008
    AIT_REG_B   CHROMA_QP_INDEX_OFFSET;		        //0x0009
        /*-DEFINE-----------------------------------------------------*/
        #define H264_CB_QP_IDX_OFST_MASK            0x1F
        #define H264_CB_QP_IDX_OFST(_a)             (_a & H264_CB_QP_IDX_OFST_MASK)
        /*------------------------------------------------------------*/
    AIT_REG_B   DATA_INFO_REG;			            //0x000A
    #define SET_SLICE_DATA_INFO_REG(reg, num_ref_idx_l0_active_minus1, LFDisableIdc , slice_type) {\
    reg = (((num_ref_idx_l0_active_minus1 & 0xf) << 4) | ((LFDisableIdc & 0x3) << 1) | (slice_type & 0x1));\
    }
        /*-DEFINE-----------------------------------------------------*/
        #define H264_I_SLICE_FLAG                   0x01
        #define H264_DIS_DBLK_IDC_MASK              0x06
        #define H264_DIS_DBLK_IDC(_a)               ((_a << 1) & H264_DIS_DBLK_IDC_MASK)
        #define H264_NUM_REF_IDX_L0_MINUS1_MASK     0xF0
        #define H264_NUM_REF_IDX_L0_MINUS1(_a)      (_a << 4)
        #define H264_REC_NV12                       0x08
        /*------------------------------------------------------------*/

    //! CHECK REG SPEC P.5 & P.30
    AIT_REG_B   FRAME_DATA_INFO_REG;		        //0x000B
    #define SET_FRAME_DATA_INFO_REG(reg, is_constrained_intra_pred) {\
    reg = (is_constrained_intra_pred & 1);\
    }
        /*-DEFINE-----------------------------------------------------*/
        #define H264_CURR_NV12                      0x02
        #define H264_P_SLICE_FLAG                   0x04
        #define H264_B_SLICE_FLAG                   0x08
        /*------------------------------------------------------------*/

    //! CHECK REG SPEC P.5 & P.30

    AIT_REG_W   START_MB_NR;			            //0x000C
    AIT_REG_B   START_MB_XPOS;			            //0x000E
    AIT_REG_B   RESERVED8;
    AIT_REG_B   START_MB_YPOS;			            //0x0010
    AIT_REG_B   RESERVED9;

    //! CHECK REG SPEC P.5 & P.31, Can I combine VLD_CTRL_REG1 & VLD_CTRL_REG2 into one word ?
    AIT_REG_B   VLD_CTL_REG;			            //0x0012
                    #define DISABLE_VLD_RINGBUF         0x00
                    #define DISABLE_SW_VLD              0x00

                    #define START_TO_DECODE_SLICE_DATA  0x01
                    #define SET_BS_START_BIT(reg, offset) { reg = (reg | ((offset & 0x7) << 1)); }
                    #define ENABLE_SW_VLD_BUF0          0x10
                    #define ENABLE_SW_VLD_BUF1          0x20
                    #define ENABLE_SW_VLD               0x40
                    #define ENABLE_VLD_RINGBUF          0x80
    AIT_REG_B   VLD_CTL2_REG;			            //0x0013
                    #define ENABLE_CHECK_REF_NUM        0x01
                    #define ENABLE_LATCH_SEGMENT_ADDR   0x02

                    #define SET_LIST_SIZE(reg, list_size) { reg = (AIT_REG_B)((reg & 0xF) | (list_size << 4)); }

    AIT_REG_D   BS_START_ADDR;			            //0x0014
    AIT_REG_D   BS_LENGTH_MINUS1;		            //0x0018
    AIT_REG_D   VLD_RINGBUF_LOWERBOUND_ADDR;	    //0x001C
    AIT_REG_D   VLD_RINGBUF_HIGHERBOUND_ADDR;	    //0x0020
    AIT_REG_W   CURRENT_MB_NR;			            //0x0024
    AIT_REG_B   SLICE_STATUS;       		        //0x0026	/* read-only */
    AIT_REG_B   RESERVED10;
    AIT_REG_D   BS_SHIFT_BITCOUNT;  		        //0x0028	/* RESERVED | 3rd byte | 2nd byte | 1st byte */
    AIT_REG_D   SEGMENT_LENGTH;			            //0x002C
    AIT_REG_B   MB_STATUS[50/*396 / 8*/];           //0x0030
                    #define GET_MB_VALID(VLD, i) (((VLD->MB_STATUS[i/8]) >> (i%8)) & 0x1)

    AIT_REG_B   _x62[6];                            //0x0062
    AIT_REG_B   HIGH_PROF_REG_1;                    //0x0068
                    #define H264_NUM_REF_IDX_L1_MINUS1_MASK     0xF0
                    #define H264_NUM_REF_IDX_L1_MINUS1(_a)      (_a << 4)
                    #define H264_NUM_REF_IDX_OVERRIDE_FLAG      0x08
                    #define H264_TRANS_8X8_FLAG                 0x04
                    #define H264_DIRECT_8X8_FLAG                0x02
                    #define H264_CABAC_MODE_FLAG                0x01
    AIT_REG_B   HIGH_PROF_REG_2;                    //0x0069
                    #define H264_NUM_REF_FRAME_MASK             0x0F
                    #define H264_FRM_MBS_ONLY_FLAG              0x10
                    #define H264_MAIN_PROFILE_FLAG              0x20
                    #define H264_CABAC_INIT_IDC_MASK            0xC0
                    #define H264_CABAC_INIT_IDC(_a)             (_a << 6)
    AIT_REG_B   HIGH_PROF_REG_3;                    //0x006A
    #if (CHIP == MCR_V2)
    AIT_REG_B   HIGH_PROF_REG_4;                    //0x006B
                    #define H264_POC_TYPE_MASK                  0xC0
                    #define H264_POC_TYPE(_a)                   (_a << 6)
                    #define H264_SEQ_SCAL_LIST_PRESENT_FLAG     0x20
                    #define H264_SEQ_SCAL_MATRIX_PRESENT_FLAG   0x10
                    #define H264_SPS_ID_MASK                    0x0F
    AIT_REG_B   HIGH_PROF_REG_5;                    //0x006C
                    #define H264_CR_QP_IDX_OFST_MASK            (0x1F<<3)
                    #define H264_CR_QP_IDX_OFST(_a)             ((_a<<3)&H264_CR_QP_IDX_OFST_MASK)
                    #define H264_PIC_SCAL_LIST_PRESENT_FLAG     0x04
                    #define H264_PIC_SCAL_MATRIX_PRESENT_FLAG   0x02
                    #define H264_PIC_ORDER_PRESENT_FLAG         0x01
    AIT_REG_B   HIGH_PROF_REG_6;                    //0x006D
                    #define H264_PPS_ID_MASK                    0x0F
                    #define H264_HIGH_PROFILE_FLAG              0x10
    AIT_REG_B   HIGH_PROF_REG_7;                    //0x006E
                    #define H264_LONG_TERM_REF_FLAG             0x08
                    #define H264_REF_LIST_REORDER_FLAG          0x04
                    #define H264_NAL_REF_IDC_MASK               0x03
    AIT_REG_B   HIGH_PROF_REG_8;                    //0x006F
        /*-DEFINE-----------------------------------------------------*/
        #define H264_REF_LIST_IDX(_l0, _l1)         ((_l1 << 4) | (_l0 & 0x0F))
        /*------------------------------------------------------------*/
    #endif

} AITS_H264DEC_VLD, *AITPS_H264DEC_VLD;

// Parse EP3 address : 0x800000F0
typedef struct _AITS_H264DEC_PARSE_EP3{
    AIT_REG_D   BS_SRC_ADDR;			            //0x00F0
    AIT_REG_D	BS_SRC_BITCOUNT;		            //0x00F4
    AIT_REG_D   BS_DST_ADDR;			            //0x00F8
    AIT_REG_D	BS_DST_BITCOUNT;		            //0x00FC  /*Read Only*/
    AIT_REG_B	_xC900[0xF0];
    AIT_REG_D   BS_SRC_LOWBD_ADDR;		            //0x01F0
    AIT_REG_D   BS_SRC_HIGHBD_ADDR;		            //0x01F4
    AIT_REG_D   BS_DST_LOWBD_ADDR;		            //0x01F8
    AIT_REG_D   BS_DST_HIGHBD_ADDR;		            //0x01FC
} AITS_H264DEC_PARSE_EP3, *AITPS_H264DEC_PARSE_EP3;

typedef struct {
    AIT_REG_W   MVD_X;
    AIT_REG_W   MVD_Y;
} AVC_MVD;

/* SW VLD MB register buffer 0 address : 0x80000100*/
/* SW VLD MB register buffer 1 address : 0x80000180*/
typedef struct _AITS_H264DEC_MB_DATA{
    AIT_REG_B   MB_TYPE;			                //0x0000
        #define SET_MB_TYPE(reg, mb_type, is_intra) {reg = ((mb_type << 1) | is_intra); }

    AIT_REG_B   CBP;				                //0x0001
        #define SET_CBP(reg, luma_cbp, chroma_cbp) {reg = ((chroma_cbp << 4) | (luma_cbp)); }
    AIT_REG_B   QP_Y;				                //0x02
    AIT_REG_B   ADJ_MB_INFO;			            //0x03
                    #define SET_LEFT_MB_AVAIL(reg, is_avail)        {reg = ((reg & 0xFE) | ((uint8_t)is_avail));}
                    #define SET_TOP_MB_AVAIL(reg, is_avail)         {reg = ((reg & 0xFD) | ((uint8_t)(is_avail << 1)));}
                    #define SET_TOPRIGHT_MB_AVAIL(reg, is_avail)    {reg = ((reg & 0xFB) | ((uint8_t)(is_avail << 2)));}
                    #define SET_TOPLEFT_MB_AVAIL(reg, is_avail)     {reg = ((reg & 0xF7) | ((uint8_t)(is_avail << 3)));}
                    #define SET_LEFT_MB_INTRA(reg, is_intra)        {reg = ((reg & 0xEF) | ((uint8_t)(is_intra << 4)));}
                    #define SET_TOP_MB_INTRA(reg, is_intra)         {reg = ((reg & 0xDF) | ((uint8_t)(is_intra << 5)));}
                    #define SET_TOPRIGHT_MB_INTRA(reg, is_intra)    {reg = ((reg & 0xBF) | ((uint8_t)(is_intra << 6)));}
                    #define SET_TOPLEFT_MB_INTRA(reg, is_intra)     {reg = ((reg & 0x7F) | ((uint8_t)(is_intra << 7)));}

    AIT_REG_W   MB_SLICE_NR;			            //0x0004
    AIT_REG_W   MB_NR;				                //0x0006
    AIT_REG_B   MB_XPOS;			                //0x0008
    AIT_REG_B   RESERVED10;
    AIT_REG_B   MB_YPOS;			                //0x000A
    AIT_REG_B   RESERVED11;
    AIT_REG_B   INTRA_MB_PRED_MODE;		            //0x000C
                    #define SET_INTRA_16X16_PRED_MODE(reg, mode)    { reg = ((reg & 0xFC) | mode); }
                    #define SET_INTRA_CHROMA_PRED_MODE(reg, mode)   { reg = ((reg & 0xF3) | ((mode & 3) << 2)); }

    AIT_REG_B   RESERVED12;
    AIT_REG_B   INTRA_4X4_PRED_MODE[16 / 2];	    //0x000E
                    #define SET_INTRA_4X4_PREV_PRED(reg, blk_idx, prev_mode_flag) {\
                                reg = ( reg | \
                                       (prev_mode_flag << ((blk_idx & 1) * 4))    \
                                      );\
                                }
                    #define SET_INTRA_4X4_REM_PRED(reg, blk_idx, rem_pred_mode) {\
                                reg = ( reg | \
                                       (rem_pred_mode << (1+ (blk_idx & 1)*4))    \
                                      );\
                                }

    AIT_REG_W   REF_IDX;			                //0x0016
                    #define SET_REF_IDX(reg,blk_idx,ref_idx) {\
                                reg = (reg) | (ref_idx << (4*blk_idx));\
                                }

    AVC_MVD     MVD[16];			                //0x0018
    AIT_REG_B   SUB_MB_TYPE;			            //0x0058
    AIT_REG_B   RESERVED13;
    AIT_REG_W   CBP_Y;				                //0x005A
                    #define SET_NOZERO_BLK(reg, idx, nonzero) { reg = (reg | (nonzero << idx));}

} AITS_H264DEC_MB_DATA, *AITPS_H264DEC_MB_DATA;

/* VLD buffer0 / buffer1 */
typedef struct {
    AIT_REG_W   DCT_16_DC[16];
    AIT_REG_W   DCT_4X4[16][16];
    AIT_REG_W	RESERVED[8];
    AIT_REG_W	DCT_CHROMA_DC[2*4];    
    AIT_REG_W	DCT_CHROMA_AC[8][16];
} AVC_VLD_BUF;

#define MAX_MB_MAP_ELEMENT  (396 / 4)

/* MbToSliceMap */
typedef struct {
    AIT_REG_W   MAP[MAX_MB_MAP_ELEMENT]; /* mb n+3 | mb n+2 | mb n+1 | mb n   */
                              /* 3 bits | 3 bits | 3 bits | 3 bits */
    /* NOTICE! Before use this macro, RESET REG TO ZERO */
    #define SET_MB_MAP(reg, mb_num, slice_group) { reg = ( reg | (slice_group << (3*(mb_num&3))) ); }
    /* CHECK THIS!!*/
    #define GET_MB_MAP(reg, mb_num, value) { value = ( (reg & (0x7 << (3*(mb_num&3)))) >> (3*(mb_num&3)) ); }
} AVC_MB_MAP;

#if !defined(BUILD_FW)
#define H264DEC_FRAME_CTL                    (H264DEC_BASE + (MMP_ULONG)(&(((AITPS_H264DEC_CTL)0)->FRAME_CTL_REG                 )))
#define H264DEC_VLD_RINGBUF_LOWERBOUND_ADDR  (H264VLD_BASE + (MMP_ULONG)(&(((AITPS_H264DEC_VLD)0)->VLD_RINGBUF_LOWERBOUND_ADDR   )))
#define H264DEC_VLD_RINGBUF_HIGHERBOUND_ADDR (H264VLD_BASE + (MMP_ULONG)(&(((AITPS_H264DEC_VLD)0)->VLD_RINGBUF_HIGHERBOUND_ADDR  )))

#define H264DEC_REF_Y_SIZE                   (H264REF_BASE + (MMP_ULONG)(&(((AITPS_H264DEC_REF)0)->Y_SIZE                        )))
#define H264DEC_REC_LOW_ADDR                 (H264REC_BASE + (MMP_ULONG)(&(((AITPS_H264DEC_REC)0)->RING_BUF_LOWERBOUND_ADDR      )))
#define H264DEC_REC_HIGH_ADDR                (H264REC_BASE + (MMP_ULONG)(&(((AITPS_H264DEC_REC)0)->RING_BUF_HIGHERBOUND_ADDR     )))
#define H264DEC_DBLK_DEST_ADDR               (H264DBLK_BASE + (MMP_ULONG)(&(((AITPS_H264DEC_DBLK)0)->DEST_ADDR                   )))
#define H264DEC_DBLK_CTRL                    (H264DBLK_BASE + (MMP_ULONG)(&(((AITPS_H264DEC_DBLK)0)->DEBLOCKING_CTRL             )))
#define H264DEC_PIC_MB_NUM                   (H264VLD_BASE + (MMP_ULONG)(&(((AITPS_H264DEC_VLD)0)->PICTURE_SIZE_IN_MBS           )))
#define H264DEC_PIC_MB_WIDTH                 (H264VLD_BASE + (MMP_ULONG)(&(((AITPS_H264DEC_VLD)0)->MB_WIDTH                      )))
#define H264DEC_PIC_MB_HEIGHT                (H264VLD_BASE + (MMP_ULONG)(&(((AITPS_H264DEC_VLD)0)->MB_HEIGHT                     )))
#define H264DEC_VLD_QP                       (H264VLD_BASE + (MMP_ULONG)(&(((AITPS_H264DEC_VLD)0)->QP                            )))
#define H264DEC_VLD_CB_QP_IDX_OFST           (H264VLD_BASE + (MMP_ULONG)(&(((AITPS_H264DEC_VLD)0)->CHROMA_QP_INDEX_OFFSET        )))
#define H264DEC_VLD_DATA_INFO                (H264VLD_BASE + (MMP_ULONG)(&(((AITPS_H264DEC_VLD)0)->DATA_INFO_REG                 )))
#define H264DEC_VLD_FRM_DATA_INFO_REG        (H264VLD_BASE + (MMP_ULONG)(&(((AITPS_H264DEC_VLD)0)->FRAME_DATA_INFO_REG           )))
#define H264DEC_VLD_ALPHA_DIV2               (H264VLD_BASE + (MMP_ULONG)(&(((AITPS_H264DEC_VLD)0)->ALPHA_OFFSET_DIV2             )))
#define H264DEC_VLD_BETA_DIV2                (H264VLD_BASE + (MMP_ULONG)(&(((AITPS_H264DEC_VLD)0)->BETA_OFFSET_DIV2              )))

#define H264_HP_REG_1                        (H264VLD_BASE + (MMP_ULONG)(&(((AITPS_H264DEC_VLD)0)->HIGH_PROF_REG_1               )))
#define H264_HP_REG_2                        (H264VLD_BASE + (MMP_ULONG)(&(((AITPS_H264DEC_VLD)0)->HIGH_PROF_REG_2               )))
#define H264_HP_REG_3                        (H264VLD_BASE + (MMP_ULONG)(&(((AITPS_H264DEC_VLD)0)->HIGH_PROF_REG_3               )))
#if (CHIP == MCR_V2)
#define H264DEC_DBLK_Y_LOWBD                 (H264DBLKROT_BASE + (MMP_ULONG)(&(((AITPS_H264DEC_DBLK_ROT)0)->Y_LOWBD_ADDR         )))
#define H264DEC_DBLK_Y_UPBD                  (H264DBLKROT_BASE + (MMP_ULONG)(&(((AITPS_H264DEC_DBLK_ROT)0)->Y_HIGHBD_ADDR        )))
#define H264DEC_DBLK_UV_LOWBD                (H264DBLKROT_BASE + (MMP_ULONG)(&(((AITPS_H264DEC_DBLK_ROT)0)->UV_LOWBD_ADDR        )))
#define H264DEC_DBLK_UV_UPBD                 (H264DBLKROT_BASE + (MMP_ULONG)(&(((AITPS_H264DEC_DBLK_ROT)0)->UV_HIGHBD_ADDR       )))
#define H264DEC_DBLK_UV_ST                   (H264DBLKROT_BASE + (MMP_ULONG)(&(((AITPS_H264DEC_DBLK_ROT)0)->UV_ST_ADDR           )))
#define H264DEC_DBLK_UPROW_ST                (H264DBLKROT_BASE + (MMP_ULONG)(&(((AITPS_H264DEC_DBLK_ROT)0)->UP_ROW_ST_ADDR       )))
#define H264_HP_REG_4                        (H264VLD_BASE + (MMP_ULONG)(&(((AITPS_H264DEC_VLD)0)->HIGH_PROF_REG_4               )))
#define H264_HP_REG_5                        (H264VLD_BASE + (MMP_ULONG)(&(((AITPS_H264DEC_VLD)0)->HIGH_PROF_REG_5               )))
#define H264_HP_REG_6                        (H264VLD_BASE + (MMP_ULONG)(&(((AITPS_H264DEC_VLD)0)->HIGH_PROF_REG_6               )))
#define H264_HP_REG_7                        (H264VLD_BASE + (MMP_ULONG)(&(((AITPS_H264DEC_VLD)0)->HIGH_PROF_REG_7               )))
#define H264_HP_REG_8                        (H264VLD_BASE + (MMP_ULONG)(&(((AITPS_H264DEC_VLD)0)->HIGH_PROF_REG_8               )))
#endif
#endif
#endif  /* end of #ifndef _AVC_REG_H_ */
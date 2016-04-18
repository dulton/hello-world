#ifndef _AVC_REG_H_
#define _AVC_REG_H_

#include "avc_config.h"

#if _AVC_HW_DEC_ == 1

#include "data_type.h"

#define VISION_V5 (1)
#define PYTHON_V2 (0)

#if VISION_V5 // Vision
#define H264_BASE_ADR               0x80000000
#define H264_REF_BASE_ADDR          0x80000010
#define H264_REC_FRAME_ADDR         0x80000060
#define H264_DBLK_BASE_ADDR         0x80000070
#define H264_VLD_BASE_ADDR          0x80000080
#define H264_VLD_DATA0_BASE_ADDR    0x80000100
#define H264_VLD_DATA1_BASE_ADDR    0x80000180
#define H264_MB_TO_SLICE_MAP_ADDR_1 0x80000200
#define H264_MB_TO_SLICE_MAP_ADDR_2 0x80000200
#define H264_VLD_BUF0_ADDR          0x8000E800
#define H264_VLD_BUF1_ADDR          0x8000EC00
// H264 Enc
#define H264_ENC_QP_TUNE            0x80000550

#define H264_ENC_FRAME_CTL          0x80000400
#define H264_ENC_INTER_CTL          0x8000056E
#define H264_ENC_FIRST_P_INDEX      0x800004F0

#define H264_ENC_REF_FRAME_LB_Y     0x800004A0
#define H264_ENC_REF_FRAME_HB_Y     0x800004A4
#define H264_ENC_REF_FRAME_LB_UV    0x800004A8
#define H264_ENC_REF_FRAME_HB_UV    0x800004AC
#define H264_ENC_REF_FRAME_Y        0x800004B8
#define H264_ENC_REF_FRAME_UV       0x800004BC

#define H264_ENC_BAK_FRAME_LB_Y     0x800005C0
#define H264_ENC_BAK_FRAME_HB_Y     0x800005C4
#define H264_ENC_BAK_FRAME_LB_UV    0x800004B0
#define H264_ENC_BAK_FRAME_HB_UV    0x800004B4
#define H264_ENC_BAK_FRAME_Y        0x800005C8
#define H264_ENC_BAK_FRAME_UV       0x800004C0

#define H264_ENC_SCALE_FACTOR       0x800004F8
#define H264_ENC_MV_BUFFER          0x800004F4
#endif

#if PYTHON_V2
#define H264_BASE_ADR               0x8000C800
#define H264_REF_BASE_ADDR          0x8000C810
#define H264_REC_FRAME_ADDR         0x8000C860
#define H264_DBLK_BASE_ADDR         0x8000C870
#define H264_VLD_BASE_ADDR          0x8000C880
#define H264_VLD_DATA0_BASE_ADDR    0x8000C900
#define H264_VLD_DATA1_BASE_ADDR    0x8000C980
#define H264_MB_TO_SLICE_MAP_ADDR_1 0x8000CA00
#define H264_MB_TO_SLICE_MAP_ADDR_2 0x80010000
#define H264_VLD_BUF0_ADDR          0x8000E800
#define H264_VLD_BUF1_ADDR          0x8000EC00

// H264 Enc
#define H264_ENC_QP_TUNE            0x8000F950

#define H264_ENC_FRAME_CTL          0x8000F800
#define H264_ENC_INTER_CTL          0x8000F96E
#define H264_ENC_FIRST_P_INDEX      0x8000F8F0

#define H264_ENC_REF_FRAME_LB_Y     0x8000F8A0
#define H264_ENC_REF_FRAME_HB_Y     0x8000F8A4
#define H264_ENC_REF_FRAME_LB_UV    0x8000F8A8
#define H264_ENC_REF_FRAME_HB_UV    0x8000F8AC
#define H264_ENC_REF_FRAME_Y        0x8000F8B8
#define H264_ENC_REF_FRAME_UV       0x8000F8BC

#define H264_ENC_BAK_FRAME_LB_Y     0x8000F9C0
#define H264_ENC_BAK_FRAME_HB_Y     0x8000F9C4
#define H264_ENC_BAK_FRAME_LB_UV    0x8000F8B0
#define H264_ENC_BAK_FRAME_HB_UV    0x8000F8B4
#define H264_ENC_BAK_FRAME_Y        0x8000F9C8
#define H264_ENC_BAK_FRAME_UV       0x8000F8C0

#define H264_ENC_SCALE_FACTOR       0x8000F8F8
#define H264_ENC_MV_BUFFER          0x8000F8F4
#endif


/* should be decided dynamically */
#define DBLK_PARAMS_BUF_BASE_ADDR   0x00000000


//#define HW_VLD_MODE                 1
//#define SW_VLD_MODE                 0
#define HW_VLD_MODE                 0x00
#define SW_VLD_MODE                 0x40
#define FMO_MODE                    0x02
#define NON_FMO_MODE                0x00

/* global register */
typedef struct {
    AIT_REG_B   FRAME_CTRL_REG;
                    #define AVC_DEC_DISABLE                     0x00
                    #define AVC_FMO_DISABLE                     0x00
                    #define AVC_DEC_ENABLE                      0x01
                    #define AVC_FMO_ENABLE                      0x02
                    #define AVC_NEW_FRAME_START                 0x04

#define ENABLE_AVC_DEC(reg, is_avc_enable, is_fmo) { reg = (is_fmo | is_avc_enable); }
#define SET_NEW_FRAME_START(reg, new_frame_start) { (reg = reg | new_frame_start); }

    AIT_REG_B   RESERVED1;
    AIT_REG_B   SW_RESET_CTRL;
                    #define VLD_RESET                           0x01
                    #define IDCT_RESET                          0x02
                    #define INTRA_PRED_RESET                    0x04
                    #define INTER_PRED_RESET                    0x08
                    #define FRAME_RECONSTRUCT_RESET             0x10
                    #define DBLK_RESET                          0x20
                    #define FLOW_CONTROL_RESET                  0x40

    AIT_REG_B   RESERVED2;
    AIT_REG_W   CPU_INT_ENABLE;
                    #if 1
                    #define VLD_SEGMENT_DONE_CPU_INT_ENB        0x0001
                    #define DECODE_SLICE_DONE_CPU_INT_ENB       0x0002
                    #define DECODE_SLICE_ERROR_CPU_INT_ENB      0x0004
                    #define DECODE_TIMEOUT_ERROR_CPU_INT_ENB    0x0020
                    #define DBLK_FRAME_DONE_CPU_INT_ENB         0x0040
                    #define PARSE_EP3_DONE_ENB                  0x0080
                    #define SLICE_RC_DONE_ENB                   0x1000
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

    AIT_REG_W   CPU_INT_STATUS;
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

    AIT_REG_W   HOST_INT_ENABLE;
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

    AIT_REG_W   HOST_INT_STATUS;
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

    AIT_REG_B       RESERVED3;
    AIT_REG_B       RESERVED4;
} AVC_DEC_GBL_CTRL;

/* reference frame start address */
typedef struct {
    AIT_REG_B   LIST0_MAP[8];           /* frame N+1 | frame N */
    AIT_REG_D   REFERENCE_ADDR[16];     /* 4th byte | 3rd byte | 2nd byte | 1st byte */
    AIT_REG_D   Y_SIZE;                 /* reserved | 3rd byte | 2nd byte | 1st byte */
} AVC_REFERENCE_REG;

/* frame reconstruct register */
typedef struct {
    AIT_REG_D   REC_FRAME_ADDR;         //check this
    AIT_REG_B   RING_BUF_SIZE;
    AIT_REG_B   RESERVED1;
    AIT_REG_B   RESERVED2;
    AIT_REG_B   RESERVED3;
    AIT_REG_D   RING_BUF_LOWERBOUND_ADDR;
    AIT_REG_D   RING_BUF_HIGHERBOUND_ADDR;
} AVC_REC_FRAME_REG;

/* BUG!*/
//#define GET_MVX(mb, idx)        (((mb->param[idx/2] >> 11) & 0x1FFF))
//#define GET_MVY(mb, idx)        ((mb->param[idx/2] & 0x7FF))
#define GET_MVX(mb, idx)        (((mb->param[idx/2] >> 11) & 0x1FFF))
#define GET_MVY(mb, idx)        ((mb->param[idx/2] & 0x7FF))

#define GET_MB_INTRA(mb)        (((mb->param[2] >> 59) & 0x1))
#define GET_MB_REF(mb,idx)     (((mb->param[3] >> (48 + (4*idx))) & 0xF))

#define SET_MVX(mb, idx, mvx)        (((mb->param[idx/2] >> 11) & 0x1FFF))
#define SET_MVY(mb, idx, mvy)        ((mb->param[idx/2] & 0x7FF))
#define SET_MB_INTRA(mb,is_intra)    ((((mb.param[2] >> 59) & 0x1E) | is_intra) << 59)
#define SET_MB_REF(mb,idx,ref)     {mb->param[3] = ((mb->param[3] & ~(0xF << (48+(4*idx)))) | (ref << (48+(4*idx))));}

typedef struct {

    volatile uint64_t    param[8];
    
} AVC_MB_INFO;


/* deblocking register */
typedef struct {
    AIT_REG_D   SOURCE_ADDR;
    AIT_REG_D   DEST_ADDR;
    AIT_REG_B   SOURCE_SIZE_IN_MBROWS;
    AIT_REG_B   RESERVED3;
    AIT_REG_B   DEBLOCKING_CTRL;
                    #define DBLK_TRIGGER_BY_HW                  0x00
                    #define DBLK_START                          0x01
                    #define DBLK_TRIGGER_BY_SW                  0x02
                    #define DBLK_REFDATA_ROTATE                 0x04
                    #define DBLK_UPROW_IN_MEMORY                0x08
                    #define DBLK_ONE_MB_MODE                    0x10
                    #define DBLK_I_FRAME_ENABLE                 0x20
                    #define DBLK_P_FRAME_ENABLE                 0x40
                    #define DBLK_B_FRAME_ENABLE                 0x80

    AIT_REG_B   RESERVED4;

    /**/
    /* 64 bits * 8 * framesize_in_mbs */
    /* 8 bits | 8 bits | 8 bits reserved | 16 bits | 8 bits reserved | 16 bits */
    AIT_REG_D   DBLK_PARAMS_BUF_ADDR;
} AVC_DBLOCKING_PARAMS;

/* VLD register */
typedef struct {
    AIT_REG_W   PICTURE_SIZE_IN_MBS;
    AIT_REG_B   MB_WIDTH;
    AIT_REG_B   RESERVED5;
    AIT_REG_B   MB_HEIGHT;
    AIT_REG_B   CR_QP_INDEX_OFFSET;
    AIT_REG_B   ALPHA_OFFSET_DIV2;
    AIT_REG_B   BETA_OFFSET_DIV2;
    AIT_REG_B   QP;
    AIT_REG_B   CB_QP_INDEX_OFFSET;
    AIT_REG_B   DATA_INFO_REG;
    #define SET_SLICE_DATA_INFO_REG(reg, num_ref_idx_l0_active_minus1, LFDisableIdc , slice_type) {\
    reg = (((num_ref_idx_l0_active_minus1 & 0xf) << 4) | ((LFDisableIdc & 0x3) << 1) | (slice_type & 0x1));\
    }

    //! CHECK REG SPEC P.5 & P.30
    AIT_REG_B   FRAME_DATA_INFO_REG;
    #define SET_FRAME_DATA_INFO_REG(reg, is_constrained_intra_pred) {\
    reg = (is_constrained_intra_pred & 1);\
    }
    #define VLDINFO_SLICE_TYPE_P (0x04)
    #define VLDINFO_SLICE_TYPE_B (0x08)

    //! CHECK REG SPEC P.5 & P.30
    //AIT_REG_B   RESERVED7;

    AIT_REG_W   START_MB_NR;            //???????

    AIT_REG_B   START_MB_XPOS;
    AIT_REG_B   START_MB_YPOS;
    AIT_REG_B   NEXT_MB_XPOS;
    AIT_REG_B   NEXT_MB_YPOS;

    //! CHECK REG SPEC P.5 & P.31, Can I combine VLD_CTRL_REG1 & VLD_CTRL_REG2 into one word ?
    AIT_REG_B   VLD_CTRL_REG;
                    #define DISABLE_VLD_RINGBUF         0x00
                    #define DISABLE_SW_VLD              0x00

                    #define START_TO_DECODE_SLICE_DATA  0x01
                    #define SET_BS_START_BIT(reg, offset) { reg = (reg | ((offset & 0x7) << 1)); }
                    #define ENABLE_SW_VLD_BUF0          0x10
                    #define ENABLE_SW_VLD_BUF1          0x20
                    #define ENABLE_SW_VLD               0x40
                    #define ENABLE_VLD_RINGBUF          0x80
    AIT_REG_B   VLD_CTRL2_REG;
                    #define ENABLE_CHECK_REF_NUM        0x01
                    #define ENABLE_LATCH_SEGMENT_ADDR   0x02

                    #define SET_LIST_SIZE(reg, list_size) { reg = (AIT_REG_B)((reg & 0xF) | (list_size << 4)); }

    AIT_REG_D   BS_START_ADDR;
    AIT_REG_D   BS_LENGTH_MINUS1;
    AIT_REG_D   VLD_RINGBUF_LOWERBOUND_ADDR;
    AIT_REG_D   VLD_RINGBUF_HIGHERBOUND_ADDR;
    AIT_REG_W   CURRENT_MB_NR;
    AIT_REG_B   SLICE_STATUS;       /* read-only */
    
    #define SLICE_UNDER_PROCESSED       0x01
    #define SLICE_MB_NUMBER_ERROR       0x02
    #define SLICE_BS_UNDECODED_ERROR    0x04
    #define SLICE_END_ERROR             0x08
    #define SLICE_CONCEAL_EXECUTED      0x10
    #define SLICE_CODEWORD_ERROR        0x20
    
    AIT_REG_B   RESERVED10;
    AIT_REG_D   BS_SHIFT_BITCOUNT;  /* RESERVED | 3rd byte | 2nd byte | 1st byte */
    AIT_REG_D   SEGMENT_LENGTH;
    AIT_REG_B   MB_STATUS[50/*396 / 8*/];
                #define GET_MB_VALID(VLD, i) (((VLD->MB_STATUS[i/8]) >> (i%8)) & 0x1)
    AIT_REG_B   xC8E2[6];
    AIT_REG_B   HIGH_PROFILE_SETTING1;   // 0xc8e8
    #define     NUM_REF_IDX_OVERRIDE_FLAG       (0x08)
    AIT_REG_B   HIGH_PROFILE_SETTING2;
    #define     CABAC_INIT_IDC_MASK             (0xC0)
    AIT_REG_B   HIGH_PROFILE_SETTING3;
    AIT_REG_B   HIGH_PROFILE_SETTING4;
    AIT_REG_B   HIGH_PROFILE_SETTING5;
    AIT_REG_B   HIGH_PROFILE_SETTING6;
    AIT_REG_B   HIGH_PROFILE_SETTING7;
    AIT_REG_B   HIGH_PROFILE_SETTING8;
    
} AVC_VLD_PARAMS;

typedef struct {
    AIT_REG_W   MVD_X;
    AIT_REG_W   MVD_Y;
} AVC_MVD;

/* SW VLD MB register */
typedef struct {
    AIT_REG_B   MB_TYPE;
        #define SET_MB_TYPE(reg, mb_type, is_intra) {reg = ((mb_type << 1) | is_intra); }

    AIT_REG_B   CBP;
        #define SET_CBP(reg, luma_cbp, chroma_cbp) {reg = ((chroma_cbp << 4) | (luma_cbp)); }
    AIT_REG_B   QP_Y;
    AIT_REG_B   ADJ_MB_INFO;
                    #define SET_LEFT_MB_AVAIL(reg, is_avail)        {reg = ((reg & 0xFE) | ((uint8_t)is_avail));}
                    #define SET_TOP_MB_AVAIL(reg, is_avail)         {reg = ((reg & 0xFD) | ((uint8_t)(is_avail << 1)));}
                    #define SET_TOPRIGHT_MB_AVAIL(reg, is_avail)    {reg = ((reg & 0xFB) | ((uint8_t)(is_avail << 2)));}
                    #define SET_TOPLEFT_MB_AVAIL(reg, is_avail)     {reg = ((reg & 0xF7) | ((uint8_t)(is_avail << 3)));}
                    #define SET_LEFT_MB_INTRA(reg, is_intra)        {reg = ((reg & 0xEF) | ((uint8_t)(is_intra << 4)));}
                    #define SET_TOP_MB_INTRA(reg, is_intra)         {reg = ((reg & 0xDF) | ((uint8_t)(is_intra << 5)));}
                    #define SET_TOPRIGHT_MB_INTRA(reg, is_intra)    {reg = ((reg & 0xBF) | ((uint8_t)(is_intra << 6)));}
                    #define SET_TOPLEFT_MB_INTRA(reg, is_intra)     {reg = ((reg & 0x7F) | ((uint8_t)(is_intra << 7)));}

    AIT_REG_W   MB_SLICE_NR;
    AIT_REG_W   MB_NR;
    AIT_REG_B   MB_XPOS;
    AIT_REG_B   RESERVED10;
    AIT_REG_B   MB_YPOS;
    AIT_REG_B   RESERVED11;
    AIT_REG_B   INTRA_MB_PRED_MODE;
                    #define SET_INTRA_16X16_PRED_MODE(reg, mode)    { reg = ((reg & 0xFC) | mode); }
                    #define SET_INTRA_CHROMA_PRED_MODE(reg, mode)   { reg = ((reg & 0xF3) | ((mode & 3) << 2)); }

    AIT_REG_B   RESERVED12;
    AIT_REG_B   INTRA_4X4_PRED_MODE[16 / 2];
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

    AIT_REG_W   REF_IDX;
                    #define SET_REF_IDX(reg,blk_idx,ref_idx) {\
                                reg = (reg) | (ref_idx << (4*blk_idx));\
                                }

    /* modify this */
    AVC_MVD     MVD[16];
    AIT_REG_B   SUB_MB_TYPE;
    /* RESERVED ?? */
    AIT_REG_B   RESERVED13;
    AIT_REG_W   CBP_Y;
                    #define SET_NOZERO_BLK(reg, idx, nonzero) { reg = (reg | (nonzero << idx));}

} AVC_VLD_MB_DATA;

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
    /* SET_MB_MAP(MB_MAP->MAP[mb_num/4], mb_num, slice_group) */
//#define SET_MB_MAP(reg, mb_num, slice_group) { reg = ( ( reg &~(0x7 << (3*(mb_num&3))) ) | (slice_group << (3*(mb_num&3))) ); };
//#define SET_MB_MAP(reg, mb_num, slice_group) { reg = ( ( reg & (0xFF & (3*(mb_num&3))) ) | (slice_group << (3*(mb_num&3))) ); };

    /* NOTICE! Before use this macro, RESET REG TO ZERO */
    #define SET_MB_MAP(reg, mb_num, slice_group) { reg = ( reg | (slice_group << (3*(mb_num&3))) ); }
    /* CHECK THIS!!*/
    #define GET_MB_MAP(reg, mb_num, value) { value = ( (reg & (0x7 << (3*(mb_num&3)))) >> (3*(mb_num&3)) ); }
} AVC_MB_MAP;

#endif  /* end of #if _AVC_HW_DEC_ == 1 */

#endif  /* end of #ifndef _AVC_REG_H_ */
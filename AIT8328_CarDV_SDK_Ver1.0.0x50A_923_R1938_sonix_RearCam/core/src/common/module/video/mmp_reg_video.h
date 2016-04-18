//==============================================================================
//
//  File        : mmp_reg_video.h
//  Description : INCLUDE File for the Retina register map.
//  Author      : Will Tseng/Truman Yang
//  Revision    : 1.0
//
//==============================================================================

#ifndef _MMP_REG_VID_H_
#define _MMP_REG_VID_H_

#include "mmp_register.h"

/** @addtogroup MMPH_reg
@{
*/

typedef struct _AITS_MEMC {
    AIT_REG_B MEMC_INT_HOST_EN;           // 0x0000
        /*-DEFINE-----------------------------------*/
        #define MEMC_FRM_ED               0x01
        /*------------------------------------------*/
    AIT_REG_B MEMC_INT_HOST_SR;           // 0x0001
    AIT_REG_B MEMC_INT_CPU_EN;            // 0x0002
    AIT_REG_B MEMC_INT_CPU_SR;            // 0x0003
    AIT_REG_D                             _x0004[3];
    AIT_REG_B MEMC_ES_CTL;                // 0x0010
        /*-DEFINE-----------------------------------*/
        #define MEMC_ES_EN                0x01
        #define MEMC_ES_HALF_PIXL         0x04
        #define MEMC_ES_UNRES_EN          0x08
        #define MEMC_ES_LINEBUF_EN        0x02
        #define MEMC_ES_ENC_LCL_BUF_EN    0x10
        /*------------------------------------------*/
    AIT_REG_B MEMC_MC_CTL;                // 0x0011
        /*-DEFINE-----------------------------------*/
        #define MEMC_MC_EN                0x01
        #define MEMC_CCIR601_EN           0x04
        /*------------------------------------------*/
    AIT_REG_B MEMC_MB_H;                  // 0x0012
    AIT_REG_B MEMC_MB_V;                  // 0x0013
    AIT_REG_B MEMC_SEARCH_CTL;            // 0x0014
    AIT_REG_B MEMC_SEARCH_WIN_SIZE;       // 0x0015
    AIT_REG_W MEMC_MB_SEL_OFST;           // 0x0016
    AIT_REG_D MEMC_MV_ADDR;               // 0x0018
	AIT_REG_D							  _x001C;
    AIT_REG_D MEMC_REF_Y_LINEBUF[4];        // 0x0020
    AIT_REG_D MEMC_REF_U_LINEBUF[4];        // 0x0030
    AIT_REG_D MEMC_REF_V_LINEBUF[4];        // 0x0040
    AIT_REG_D MEMC_Y_BUF_LOWBD;           // 0x0050
    AIT_REG_D MEMC_Y_BUF_UPBD;            // 0x0054
    AIT_REG_D                             _x0058[2];
    AIT_REG_D MEMC_U_BUF_LOWBD;           // 0x0060
    AIT_REG_D MEMC_U_BUF_UPBD;            // 0x0064
    AIT_REG_D                             _x0068[2];
    AIT_REG_D MEMC_V_BUF_LOWBD;           // 0x0070
    AIT_REG_D MEMC_V_BUF_UPBD;            // 0x0074
    AIT_REG_D                             _x0078[2];
    AIT_REG_D MEMC_REF_Y_FRM_ADDR;        // 0x0080
    AIT_REG_D MEMC_REF_U_FRM_ADDR;        // 0x0084
    AIT_REG_D MEMC_REF_V_FRM_ADDR;        // 0x0088
    AIT_REG_D                             _x008C;
    AIT_REG_D MEMC_GEN_Y_FRM_ADDR;        // 0x0090
    AIT_REG_D MEMC_GEN_U_FRM_ADDR;        // 0x0094
    AIT_REG_D MEMC_GEN_V_FRM_ADDR;        // 0x0098
    AIT_REG_D                             _x009C;
    AIT_REG_B MEMC_EMM_Y_LINEBUF;         // 0x00A0
    AIT_REG_B MEMC_EMM_U_LINEBUF;         // 0x00A1
    AIT_REG_B MEMC_EMM_V_LINEBUF;         // 0x00A2
    AIT_REG_B                             _x00A3;
    // the similar function of MEMC_EMM_CUR_ADDR_Y
    AIT_REG_B MEMC_EMM_Y_CURBUF;          // 0x00A4
    AIT_REG_B MEMC_EMM_U_CURBUF;          // 0x00A5
    AIT_REG_B MEMC_EMM_V_CURBUF;          // 0x00A6
    AIT_REG_B                             _x00A7;
    AIT_REG_B MEMC_EMM_Y_REFGEN;          // 0x00A8
    AIT_REG_B MEMC_EMM_U_REFGEN;          // 0x00A9
    AIT_REG_B MEMC_EMM_V_REFGEN;          // 0x00AA
    AIT_REG_B                             _x00AB;

    AIT_REG_B MEMC_TEST_MODE;             // 0x00F0
    AIT_REG_B                             _x00F1[15];
} AITS_MEMC, *AITPS_MEMC;

typedef struct _AITS_VID {
    AIT_REG_W VID_ENC_H;							// 0x0000
    AIT_REG_W VID_ENC_V;	              			// 0x0002
    AIT_REG_B VID_DCAC_PRED_TH;           			// 0x0004
    AIT_REG_B VID_DCAC_PRED_EN;          		 	// 0x0005
        /*-DEFINE-----------------------------------*/
        #define ACDC_DYN_PRED_EN        0x0003
        #define	ACDC_PRED_EN		    0x0002
        #define	DYNA_ACDC_PRED	        0x0001
        #define	ACDC_PRED_DIS	        0x0000
        /*------------------------------------------*/
    AIT_REG_B VID_ENC_CFG;							// 0x0006
        /*-DEFINE-----------------------------------*/
        #define	VID_HW_HDR_EN           0x0000
        /*------------------------------------------*/
    AIT_REG_B 							  _x0007;
    AIT_REG_W VID_VOP_TIME_RES;           			// 0x0008
    AIT_REG_W VID_FIXED_VOP_TIME_INCR;    			// 0x000A
    AIT_REG_B VID_ENC_FRM_CTL;            			// 0x000C
    	/*-DEFINE-----------------------------------*/
		#define VID_ENC_MODULO_TIME		0x0001
		#define VID_ENC_ROUNDING		0x0004
		#define VID_ENC_NOROUND			0x0000
		#define VID_ENC_TYPE_I			0x0002
		#define VID_ENC_TYPE_P			0x0000
		/*------------------------------------------*/
    AIT_REG_B                             _x000D[3];
    AIT_REG_W VID_DEC_DATA_PORT;          			// 0x0010
    AIT_REG_W VID_DEC_ADDR_PORT;          			// 0x0012
    AIT_REG_W VID_DEC_CTL0;               			// 0x0014
        /*-DEFINE-----------------------------------*/
        #define H_SCAN_PATTERN          0x0008
        #define V_SCAN_PATTERN          0x0004
        #define ZIG_ZAG_SCAN_PATTERN    0x0002
        #define ST_DEC_MB               0x0001
        #define ROUNDING_TYPE_SEL       0x0400
        #define MB_TYPE_INTRA           0x0200
        #define MB_TYPE_INTER           0x0000
        #define FRM_DEC_TYPE_I          0x0100
        #define FRM_DEC_TYPE_P          0x0000
        /*------------------------------------------*/
    AIT_REG_B VID_DEC_CTL2;               			// 0x0016
        /*-DEFINE-----------------------------------*/
        #define MEMC_VECTOR_SET         0x01
        /*------------------------------------------*/
    AIT_REG_B                             _x0017;
    AIT_REG_B 							  _x0018;
    AIT_REG_B VID_ENGINE_SR;              			// 0x0019
        /*-DEFINE-----------------------------------*/
        #define VID_CODEC_BUSY          0x01
        /*------------------------------------------*/
    AIT_REG_B                             _x001A[22];
    AIT_REG_D VID_INT_CPU_EN;             			// 0x0030
        /*-DEFINE-----------------------------------*/
        #define VID_FRM_ED				0x02
        #define VID_FRM_ST				0x01
        /*------------------------------------------*/
    AIT_REG_D VID_INT_CPU_SR;             			// 0x0034
    AIT_REG_D VID_INT_HOST_EN;            			// 0x0038
    AIT_REG_D VID_INT_HOST_SR;            			// 0x003C
    AIT_REG_W VID_MV_X_1ST;               			// 0x0040
    AIT_REG_W VID_MV_Y_1ST;               			// 0x0042
    AIT_REG_W VID_MV_X_2ND;               			// 0x0044
    AIT_REG_W VID_MV_Y_2ND;               			// 0x0046
    AIT_REG_W VID_MV_X_3RD;               			// 0x0048
    AIT_REG_W VID_MV_Y_3RD;               			// 0x004A
    AIT_REG_W VID_MV_X_4TH;               			// 0x004C
    AIT_REG_W VID_MV_Y_4TH;               			// 0x004E
    AIT_REG_W VID_MP4_ENC_CTL;            			// 0x0050
        /*-DEFINE-----------------------------------*/
        #define VID_MP4_ENC_EN          0x01
        /*------------------------------------------*/
    AIT_REG_W VID_MP4_DEC_CTL;            			// 0x0052
        /*-DEFINE-----------------------------------*/
        #define VID_MP4_DEC_EN          0x01
        /*------------------------------------------*/
    AIT_REG_W VID_H263_ENC_CTL;           			// 0x0054
        /*-DEFINE-----------------------------------*/
        #define VID_H263_ENC_EN         0x01
        /*------------------------------------------*/
    AIT_REG_W VID_H263_DEC_CTL;           			// 0x0056
        /*-DEFINE-----------------------------------*/
        #define VID_H263_DEC_EN         0x01
        #if (CHIP == P_V2)
        #define VID_CUT_SEG_EN          0x02
        #endif
        /*------------------------------------------*/
    AIT_REG_B VID_I_FRM_QP;               // 0x0058
    AIT_REG_B VID_P_FRM_QP;               // 0x0059
    AIT_REG_B VID_H263_CFG;               // 0x005A
        /*-DEFINE-----------------------------------*/
        #define VID_H263_RES_128x96     0x01
        #define VID_H263_RES_176x144    0x02
        #define VID_H263_RES_352x288    0x03
        #define VID_H263_PROFILE_0_EN   0x08
        /*------------------------------------------*/
    AIT_REG_B                             _x005B;
    AIT_REG_B VID_DEC_CFG;                // 0x005C
        /*-DEFINE-----------------------------------*/
        #define VID_VLD_PRED_EN         0x01
		#define VID_VLD_H263_SHORT_HDR	0x02
		#if (CHIP == P_V2)
		#define VID_MP4_QTYPE_EN        0x04
		#define VID_MP4_QMATRIX_EN      0x08
		#endif
        /*------------------------------------------*/
    AIT_REG_B                             _x005D;
    #if (CHIP == P_V2)
    AIT_REG_W VID_LOAD_QMATRIX_V1;                  // 0x005E
    #endif
    AIT_REG_D VID_ENC_CUR_BUF_ADDR_Y0;    			// 0x0060
    AIT_REG_D VID_ENC_CUR_BUF_ADDR_U0;    			// 0x0064
    AIT_REG_D VID_ENC_CUR_BUF_ADDR_V0;    			// 0x0068
    AIT_REG_D                             _x006C;
    AIT_REG_D							  _x0070[3];
    AIT_REG_D VID_ENC_BSOUT_ST;     	 			// 0x007C
    AIT_REG_D VID_ENC_BSOUT_CUR;     	 			// 0x0080
    AIT_REG_D VID_ENC_BSOUT_ED;		      			// 0x0084
    AIT_REG_D VID_ENC_BSOUT_STS_ADDR;     			// 0x0088
    AIT_REG_D VID_ENC_TOTAL_STS_ADDR;     			// 0x008C
    AIT_REG_D VID_ENC_FRM_SIZE;           			// 0x0090
	AIT_REG_D							  _x0094[11];
    AIT_REG_D VID_ENC_Y_DC_ADDR;       				// 0x00C0
    AIT_REG_D VID_ENC_Y_AC_ADDR;       				// 0x00C4
    AIT_REG_D VID_ENC_C_DC_ADDR;    	   			// 0x00C8
    AIT_REG_D VID_ENC_C_AC_ADDR;	       			// 0x00CC
	AIT_REG_B							  _x00D0[4];
	AIT_REG_D							  _x00D4[7];
    AIT_REG_B VID_HDR_DATA_PORT;          			// 0x00F0
    AIT_REG_B                             _0x00F1[15];
} AITS_VID, *AITPS_VID;

//------------------------------
// VLD structure (0x8000 B400)
//------------------------------
typedef struct _AITS_VLD {
    AIT_REG_B VLD_CTL_0;                    // 0x0000
        /*-DEFINE-----------------------------------*/
        #if 1 //Retina
        #define VLD_ST_EN                0x01
        #else //Retina-V1
        #define VLD_STS                  0x02 // read-only
        #define VLD_ST_EN                0x01 // start next seg
        #endif
        /*------------------------------------------*/
    AIT_REG_B                               _x0001;
    AIT_REG_B VLD_CTL_1;                    // 0x0002
    AIT_REG_B                               _x0003;
    AIT_REG_B VLD_CFG_0;                    // 0x0004
        /*-DEFINE-----------------------------------*/
        #if 0
        #define VLD_VID_FMT_MPEG4        0x00
        #define VLD_VID_FMT_H263         0x01
        #else
        #define VLD_H263_SHORT_HDR       0x01
        #endif
        #define MB_DATA_PARTITIONED      0x08
        #define VLD_RVLC_EN              0x10
        #define VLD_VOP_LEVEL            0x00
        #define VLD_VP_LEVEL             0x40
        #define VLD_MB_LEVEL             0x80
        /*------------------------------------------*/
    AIT_REG_B VLD_CFG_1;                    // 0x0005
    AIT_REG_B VLD_VOP_TIME_INC_BIT_NUM;     // 0x0006
    AIT_REG_B VLD_MB_NUM_BIT;               // 0x0007
    AIT_REG_B VLD_H263_SRC_FMT;             // 0x0008
    AIT_REG_B                               _x0009[3];
    AIT_REG_W VLD_MDLO_TIME_BASE;           // 0x000C
    AIT_REG_W VLD_VOP_TIME_INCR;            // 0x000E
    AIT_REG_B VLD_VOP_HDR;                  // 0x0010
    AIT_REG_B VLD_H263_TEMPORAL_REF;        // 0x0011
    AIT_REG_B VLD_H263_PSUPP;               // 0x0012
    AIT_REG_B VLD_H263_GOB_LAYER_HDR;       // 0x0013
    AIT_REG_W VLD_MB_NUM;                   // 0x0014
    AIT_REG_W                               _x0016;
    AIT_REG_B VLD_VOP_QP;                   // 0x0018
    AIT_REG_B VLD_VOP_DC_TH;                // 0x0019
    AIT_REG_B VLD_VOP_FCODE;                // 0x001A
    AIT_REG_B                               _x001B;
    AIT_REG_D VLD_DATA_PARTITION_ADDR;      // 0x001C
    AIT_REG_D VLD_BLK_COEF_OUT_ADDR;        // 0x0020
    AIT_REG_D VLD_BS_ADDR;                  // 0x0024
    AIT_REG_B VLD_BS_ST_BIT;                // 0x0028
    AIT_REG_B                               _x0029[3];
    AIT_REG_B VLD_INT_CPU_EN;               // 0x002C
        /*-DEFINE-----------------------------------*/
        #define VLD_FRM_ED               0x01
        #define VLD_SEG_ED               0x02
        /*------------------------------------------*/
    AIT_REG_B VLD_INT_CPU_SR;               // 0x002D
    AIT_REG_B VLD_INT_HOST_EN;              // 0x002E
    AIT_REG_B VLD_INT_HOST_SR;              // 0x002F
    AIT_REG_B VLD_INT_STS;                  // 0x0030
        /*-DEFINE-----------------------------------*/
        #define VLD_STS_DONE             0x01
        #define VLD_STS_NOT_CODED        0x02
        #define VLD_STS_DEC_ERROR        0x04
        #define VLD_STS_LEN_UNDERFLOW    0x08
        #define VLD_STS_LEN_OVERFLOW     0x10
        /*------------------------------------------*/
    AIT_REG_B                               _x0031;
    AIT_REG_B VLD_VOP_HDR_DONE;             // 0x0032
    AIT_REG_B                               _x0033;
    AIT_REG_B VLD_SRAM_BUF_DELSEL;          // 0x0034
    AIT_REG_B                               _x0035[11];
    AIT_REG_B VLD_PRED_CFG;                 // 0x0040
        #define VLD_QP_OUT_ONLY          0x02
        #define VLD_MB_INFO_OUT          0x01
    AIT_REG_B                               _x0041[3];
    AIT_REG_B VLD_VOP_W;                    // 0x0044
    AIT_REG_B                               _x0045;
    AIT_REG_W VLD_VOP_MB_NUM;               // 0x0046
    AIT_REG_D VLD_MV_PRED_BUF_ADDR;         // 0x0048
    AIT_REG_D VLD_MV_OUT_ADDR;              // 0x004C
    AIT_REG_D VLD_BS_LOWBD;                 // 0x0050;
    AIT_REG_D VLD_BS_UPBD;                  // 0x0054
    AIT_REG_D VLD_BS_LEN;                   // 0x0058, 3 bytes data
    AIT_REG_D VLD_SEG_LEN;                  // 0x005C, 3 bytes data
    AIT_REG_B VLD_LAST_SEG;                 // 0x0060;
        /*-DEFINE-----------------------------------*/
        #define VLD_IS_LAST_SEG             0x01
        /*------------------------------------------*/
    AIT_REG_B                               _x0061[3];
    AIT_REG_B VLD_BS_BUF_OPT;               // 0x0064
        /*-DEFINE-----------------------------------*/
        #define VLD_REFETCH_SEG_ST_ADDR_EN  0x02 // Set 1 to refetch the start address. 0 to continue to get next 32 bytes
        #define VLD_RING_BUF_EN             0x01 // Use ring buffer. 1 for ring buffer. 0 for plain buffer
        /*------------------------------------------*/
    AIT_REG_B                               _x0065[3];
    AIT_REG_D VLD_CUR_BIT;                  // 0X0068, 20 bits data
} AITS_VLD, *AITPS_VLD;

//-------------------------------
// PRED structure (0x8000 B800)
//-------------------------------
typedef struct _AITS_PRED {
    AIT_REG_B PRED_SW_ST;                // 0x0000
    AIT_REG_B                            _x0001[3];
    AIT_REG_B PRED_INT_CPU_EN;           // 0x0004
    AIT_REG_B PRED_INT_CPU_SR;           // 0x0005
    AIT_REG_B PRED_INT_HOST_EN;          // 0x0006
    AIT_REG_B PRED_INT_HOST_SR;          // 0x0007
    AIT_REG_B PRED_SW_RST;               // 0x0008
    AIT_REG_B                            _x0009[3];
    AIT_REG_B PRED_CFG;                  // 0x000C
    AIT_REG_B                            _x000D;
    AIT_REG_B PRED_VOP_W;                // 0x000E
    AIT_REG_B                            _x000F;
    AIT_REG_D PRED_H_BUF_ADDR;           // 0x0010
    AIT_REG_D PRED_V_BUF_ADDR;           // 0x0014
    AIT_REG_D                            _x0018[5];
    AIT_REG_B PRED_MB_CFG;               // 0x002C;
    AIT_REG_B                            _x002D[3];
    AIT_REG_W PRED_VP_NUM;               // 0x0030
    AIT_REG_W PRED_MB_NUM;               // 0x0032
    AIT_REG_D                            _x0034;
    AIT_REG_B PRED_BLK_NUM;              // 0x0038
    AIT_REG_B PRED_COEF_IDX;             // 0x0039
    AIT_REG_B                            _x003A[6];
} AITS_PRED, *AITPS_PRED;

//--------------------------------
// DBLK structure (0x8000 BC00)
//--------------------------------
typedef struct _AITS_DBLK {
    AIT_REG_B DBLK_CTL;                    // 0x0000
        /*-DEFINE-----------------------------------*/
        #define DBLK_SW_ST                 0x01
        /*------------------------------------------*/
    AIT_REG_B                              _x0001[3];
    AIT_REG_B DBLK_INT_HOST_EN;            // 0x0004
    AIT_REG_B DBLK_INT_HOST_SR;            // 0x0005
    AIT_REG_B DBLK_INT_CPU_EN;             // 0x0006
        /*-DEFINE-----------------------------------*/
        #define DBLK_FRM_ED_EN             0x01
        /*------------------------------------------*/
    AIT_REG_B DBLK_INT_CPU_SR;             // 0x0007
        /*-DEFINE-----------------------------------*/
        #define DBLK_FRM_ED_SR             0x01
        /*------------------------------------------*/
    AIT_REG_D                              _x0008[2];
    AIT_REG_D DBLK_Y_SRC_ADDR;             // 0x0010
    AIT_REG_D DBLK_U_SRC_ADDR;             // 0x0014
    AIT_REG_D DBLK_V_SRC_ADDR;             // 0x0018
    AIT_REG_D DBLK_MB_QP_ST_ADDR;          // 0x001C
    AIT_REG_D DBLK_Y_OUT_ADDR;             // 0x0020
    AIT_REG_D DBLK_U_OUT_ADDR;             // 0x0024
    AIT_REG_D DBLK_V_OUT_ADDR;             // 0x0028
    AIT_REG_D                              _x002C;
    AIT_REG_B DBLK_FRM_NUM_MB_W;           // 0x0030
    AIT_REG_B DBLK_FRM_NUM_MB_H;           // 0x0031
    AIT_REG_W                              _x0032;
    AIT_REG_B DBLK_PARAM;                  // 0x0034
        /*-DEFINE-----------------------------------*/
        #define DBLK_K3_EN                 0x04
        #define DBLK_QP_BY_MB              0x02
        #define DBLK_Y_ONLY                0x01
        /*------------------------------------------*/
    AIT_REG_B                               _x0035;
    AIT_REG_B DBLK_FRM_QP;                 // 0x0036
    AIT_REG_B                               _x0037;
    AIT_REG_B DBLK_TH;                     // 0x0038
    AIT_REG_B DBLK_TH_SCAL;                // 0x0039
    AIT_REG_B DBLK_TH_OFST;                // 0x003A
    AIT_REG_B                              _x003B;
    AIT_REG_B DBLK_TH_ADJNT;               // 0x003C
    AIT_REG_B DBLK_K1_K2;                  // 0x003D
    AIT_REG_B DBLK_K3;                     // 0x003E
    AIT_REG_B                              _x003F;
} AITS_DBLK, *AITPS_DBLK;

/// @}
#endif // _MMPH_REG_VID_H_

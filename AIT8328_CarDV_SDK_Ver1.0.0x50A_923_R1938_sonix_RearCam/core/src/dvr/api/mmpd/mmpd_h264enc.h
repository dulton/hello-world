/// @ait_only
/**
 @file mmpd_h264enc.h
 @brief INCLUDE File of Host VIDEO ENCODE Driver.
 @author Hsin-Wei Yang
 @version 1.0
*/


#ifndef _MMPD_H264ENC_H_
#define _MMPD_H264ENC_H_

#include "mmp_lib.h"
#include "ait_config.h"

/** @addtogroup MMPD_3GP
 *  @{
 */

//==============================================================================
//
//                              COMPILER OPTION
//
//==============================================================================


//==============================================================================
//
//                              CONSTANTS
//
//==============================================================================
//#define DYNA_QP_GAP             0
#define TUNE_MCI_PRIO           1

#define H264ENC_MAX_PARSET_SIZE (64)
#define H264ENC_MAX_B_NUM		32

#define I_SLICE                 1
#define P_SLICE                 0
#define BASELINE_PROFILE        66
#define MAIN_PROFILE            77

//FREXT Profile IDC definitions
#define FREXT_HP                100      ///< YUV 4:2:0/8 "High"
#define FREXT_Hi10P             110      ///< YUV 4:2:0/10 "High 10"
#define FREXT_Hi422             122      ///< YUV 4:2:2/10 "High 4:2:2"
#define FREXT_Hi444             244      ///< YUV 4:4:4/12 "High 4:4:4"

//QP Fine Tune Mode
#define FINETUNE_DIS            0x00
#define FINETUNE_ROW            0x01
#define FINETUNE_SLICE          0x03
#define FINETUNE_MB             0x05

typedef enum {
    MMPD_H264ENC_ME_SPEED0 = 0, //FASTEST
    MMPD_H264ENC_ME_SPEED1 = 1,
    MMPD_H264ENC_ME_SPEED2 = 2,
    MMPD_H264ENC_ME_SPEED3 = 3,
    MMPD_H264ENC_ME_SPEED4 = 4,
    MMPD_H264ENC_ME_SPEED5 = 5  //SLOWEST
} MMPD_H264ENC_ME_SPEED;

typedef enum _MMPD_H264ENC_FRAME_TYPE {
	MMPD_H264ENC_FRAME_TYPE_PRM = 0,	// I, P frame
	MMPD_H264ENC_FRAME_TYPE_SEC			// B frame
} MMPD_H264ENC_FRAME_TYPE;

typedef struct _MMPD_H264ENC_FRAME {
    MMP_ULONG yuv[3];
} MMPD_H264ENC_FRAME;

//==============================================================================
//
//                              STRUCTURES
//
//==============================================================================
typedef struct _MMPD_H264ENC_Config {
    MMP_ULONG   ulDRAM_MemStartAddr;
    MMP_ULONG   ulSRAM_MemStartAddr;

	// HW configuration
    MMP_BOOL    bSupportFREXT;
    MMP_BOOL    bHighSpeedEn;
    MMP_BOOL    bInterPipelineMode;
    MMP_BOOL    bYLineBufEn;
	MMP_USHORT  usBFrameNum;
    MMP_USHORT 	usPFrameNum;
    MMP_BOOL    bParsetBySw;
    MMP_BOOL    bSupportVuiInfo;

    // COMMON
    MMP_ULONG   ulResolutionIndex;
    MMP_ULONG   ulFrameWidth;
    MMP_ULONG   ulFrameHeight;
    MMP_ULONG   ulSPS_PPS_BeforeISlice;
    MMP_ULONG   ulRowNumPerSlice;

    // BUFFER
    MMP_ULONG   ulBSLen;

    //MMP_ULONG   ulSliceLen_StartAddr;
    //MMP_ULONG   ulBitstreamStartAddr;
    //MMP_ULONG   ulBitstreamBuf_LowerBoundAddr;
    //MMP_ULONG   ulBitstreamBuf_UpperBoundAddr;
    //MMP_ULONG   ulMVBuf_Addr;
    //MMP_ULONG   ulDeblockFrame_Addr;
    //MMP_ULONG   ulDeblockFrame_LowerBoundAddr;
    //MMP_ULONG   ulDeblockFrame_UpperBoundAddr;
    
    //MMP_ULONG   ulCurrentFrame_Addr;
    //MMP_ULONG   ulReconstructFrame_LowerBoundAddr;
    //MMP_ULONG   ulReconstructFrame_UpperBoundAddr;
    
    // SPS/PPS Header
    MMP_ULONG   ulProfile;
    MMP_ULONG   ulLevel;
    MMP_ULONG   ulLog2_MaxFrameNum_Minus_4;
    MMP_ULONG   ulLog2_MaxPOC_LSB_Minus_4;
    MMP_ULONG   ulGapsInFrameNum_Allowed;
    MMP_ULONG   ulFrameCropping_Flag;
    MMP_ULONG   ulFrameCropping_Left;
    MMP_ULONG   ulFrameCropping_Right;
    MMP_ULONG   ulFrameCropping_Top;
    MMP_ULONG   ulFrameCropping_Bottom;
    MMP_ULONG   ulInitQP;
    //MMP_ULONG   ulDeblocking_flag;

    //Deblock
    MMP_ULONG   ulDeblockEnable;
    MMP_ULONG   ulAlphaDivide2;
    MMP_ULONG   ulBetaDivide2;

    /* Begin of quality related setting */
    MMP_ULONG   ulIFramePeriod;

    // Mode Decision
    MMP_ULONG   ulInterMB_Threshold;
    MMP_ULONG   ulIntraMB_Threshold;
    MMP_ULONG   ulInterMB_Bias;

    // Transform Threshold
    MMP_ULONG   ulCBP_Control_Enable;
    MMP_ULONG   ulLumaMB_Cost;
    MMP_ULONG   ulLumaCoeff_Cost;
    MMP_ULONG   ulChromaCoeff_Cost;
    MMP_ULONG   ulCostLargerThanOne;

    // ME Threshold
    MMP_ULONG   ulME_MaxSearchStep;
    MMP_ULONG   ulME_SkipCandIncrStep;
    MMP_ULONG   ulME_EarlyStop_Thres;
    MMP_ULONG   ulME_StopThres_UpperBound;
    MMP_ULONG   ulME_StopThres_LowerBound;
    MMP_ULONG   ulME_SkipThres_UpperBound;
    MMP_ULONG   ulME_SkipThres_LowerBound;
    MMP_ULONG   ulME_MBPartition_Mode;
    MMP_ULONG   ulME_MotionSearch_Complexity;
    MMP_ULONG   ulME_MB_RefineCount;
    MMP_ULONG   ulME_SubMB_RefineCount;
    MMP_ULONG   ulME_PartMBCost_Thres;
    MMP_ULONG   ulIME_CostWeight_0;
    MMP_ULONG   ulIME_CostWeight_1;

    // Rate Control
    MMP_ULONG   ulRC_MBLayerControl_Enable;
    MMP_ULONG   ulRC_TargetFrameSize;
    MMP_ULONG   ulRC_SliceQP;
    MMP_ULONG   ulRC_BasicUnitSize;
    MMP_ULONG   ulRC_BasicQP;
    MMP_ULONG   ulRC_FineTuneQP_Gap;
    MMP_ULONG   ulRC_QPFineTuneMode;
    MMP_ULONG   ulRC_FrameRate;
    MMP_ULONG   ulRC_FrameSkip_Enable;
    
    MMP_ULONG	ulMaxIWeight;			//1.5 * 1000
	MMP_ULONG	ulMinIWeight;			//1.0 * 1000
	MMP_ULONG	ulMaxBWeight;			//1.0 * 1000
	MMP_ULONG	ulMinBWeight;			//0.5 * 1000
	MMP_ULONG	ulVBV_Delay;			//500 ms
	MMP_ULONG	ulTargetVBVLevel;		//500 ms
	MMP_ULONG	ulInitWeight[3];
    MMP_ULONG	ulMaxQPDelta[3];
    MMP_ULONG	ulVBR;
    MMP_ULONG   ulSkipFrameThreshold;
    /* End of quality related setting */

    // For toggle ME buffers in SRAM or DRAM
    MMP_UBYTE   ubMEBufferInDRAM_Up;
    MMP_UBYTE   ubMEBufferInDRAM_Line;
    
} MMPD_H264ENC_Config;


//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================
MMP_ERR MMPD_H264ENC_EncodeRawData(MMPD_H264ENC_FRAME_TYPE FrameType,
                                    MMP_ULONG ulFrameDspyNum, MMP_ULONG ulQP,
                                    MMP_ULONG *ulBSAddr0, MMP_ULONG *ulFrameLen0,
                                    MMP_ULONG *ulBSAddr1, MMP_ULONG *ulFrameLen1,
                                    MMP_BOOL  *bSkipFrame);
                                    
MMP_ERR MMPD_H264ENC_GetFreeFrame(MMP_ULONG *ulYAddr,MMP_ULONG *ulUAddr,MMP_ULONG *ulVAddr);
MMP_ULONG* MMPD_H264ENC_GetSliceInfo(void);
MMP_ULONG MMPD_H264ENC_GetEncFrameNum(void);

MMP_ERR MMPD_H264ENC_SetTestPlanMode(MMP_BOOL bEnable);
void MMPD_H264ENC_InitTPReg(MMP_ULONG ulRegIdx, MMP_ULONG ulAddr, MMP_ULONG ulValue);

#endif



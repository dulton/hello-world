/// @ait_only
/**
 @file mmpd_h264enc.c
 @brief H.264 Encoder Control Driver Function
 @author Hsin-Wei Yang
 @version 1.0
*/

#ifdef BUILD_CE
#undef BUILD_FW
#endif
/// @ait_only
/**
 @file mmpd_h264enc.c
 @brief H.264 Encoder Control Driver Function
 @author Hsin-Wei Yang
 @version 1.0
*/

#include "mmp_lib.h"
#include "mmp_reg_h264enc.h"
#include "mmp_reg_video.h"
#include "mmp_reg_gbl.h"
#include "mmp_reg_gpio.h"
#include "mmpd_dma.h"
#include "mmpd_mp4venc.h"
#include "mmpd_h264enc.h"
#include "mmpd_system.h"
#include "mmph_hif.h"
#include "ait_utility.h"

#include "mmpf_mp4venc.h"

#ifdef BUILD_CE
#include "lib_retina.h"
#else
#define     RTNA_DBG_Str(foo, bar)  do {} while(0)
#define     RTNA_DBG_Byte(foo, bar)  do {} while(0)
#define     RTNA_DBG_Short(foo, bar)  do {} while(0)
#define     RTNA_DBG_Long(foo, bar)  do {} while(0)
#endif

//==============================================================================
//
//                              VARIABLES
//
//==============================================================================
static MMPD_H264ENC_FRAME   m_CurFrame;
static MMPD_H264ENC_FRAME   m_RefFrame;
static MMPD_H264ENC_FRAME   m_GenFrame;
static MMP_ULONG            m_ulFrameNum;           // Encoding order frame number
static MMP_ULONG            m_ulIntraPeriod;        // GOP
static MMP_ULONG            m_ulBSAddr;
static MMP_ULONG            m_ulBSStartAddr;
static MMP_ULONG            m_ulBSEndAddr;
static MMP_ULONG            m_ulFrameSize;
static MMP_ULONG			m_ulMBWidth;
static MMP_ULONG			m_ulMBHeight;
static MMP_BOOL             m_bTestPlanMode = MMP_FALSE;
static MMP_BOOL             m_bFrameAfterI;         // True if this is 1st frame after I
static MMP_USHORT           m_usBFrameNum;          // # consecutive B frames
static MMP_ULONG			m_ulPrevRefFrameNum;    // In slice header
static MMP_ULONG            m_ulProfile = FREXT_HP;
static MMP_BOOL             m_ubSupportFREXT = MMP_TRUE;
static MMP_BOOL             m_bRCEnable = MMP_FALSE;
#if (DYNA_QP_GAP == 1)
static MMP_ULONG            m_ulRCTuneGap;
#endif
static MMP_ULONG            m_ulBMVWeight[2] = {85, 171};
static MMP_BOOL             m_bSkipFrameEn;
static MMP_BOOL             m_bSkipNextFrame;
MMP_ULONG   SliceLenBuf[256];

MMP_ULONG   m_ulTPRegAddr[38];
MMP_ULONG   m_ulTPRegVal[38];

static void* m_RCHandle;

MMP_ERR MMPD_H264ENC_SetTestPlanMode(MMP_BOOL bEnable)
{
    m_bTestPlanMode = bEnable;
    return MMP_ERR_NONE;
}

void MMPD_H264ENC_InitTPReg(MMP_ULONG ulRegIdx, MMP_ULONG ulAddr, MMP_ULONG ulValue)
{
    m_ulTPRegAddr[ulRegIdx] = ulAddr;
    m_ulTPRegVal[ulRegIdx]  = ulValue;
}

void MMPD_H264ENC_SetTPReg()
{
    MMP_ULONG i;

    PRINTF("  OLD [   ADDR   ] NEW  \r\n");
    RTNA_DBG_Str(0, "  OLD [   ADDR   ] NEW  \r\n");
    for (i = 0; i < 38; i++) {
        if (MMPH_HIF_RegGetW(m_ulTPRegAddr[i]) != m_ulTPRegVal[i]) {
            PRINTF("x%04x [0x%08X] x%04x\r\n", MMPH_HIF_RegGetW(m_ulTPRegAddr[i]), m_ulTPRegAddr[i], m_ulTPRegVal[i]);
        }
        MMPH_HIF_RegSetW(m_ulTPRegAddr[i], m_ulTPRegVal[i]);
    }
}

void MMPD_H264ENC_DumpReg(MMP_BYTE *path)
{
#if 0
    MMP_ULONG   i;
    MMP_ULONG   value;
    FILE        *fp;

    if ((fp = fopen(path, "w")) == NULL) {
        PRINTF("File open %s error error\r\n", path);
        return;
    }
    PRINTF("Dump H264 OPRs to %s...\r\n", path);
    for(i=0x8000F800;i<0x8000F9E0;i++) {
        value = MMPH_HIF_RegGetB(i);
        fprintf(fp, "REG[0x%X] : 0x%x\r\n",i,value);
    }
    for(i=0x8000FA00;i<0x8000FA28;i++) {
        value = MMPH_HIF_RegGetB(i);
        fprintf(fp, "REG[0x%X] : 0x%x\r\n",i,value);
    }
    i = 0x8000C800;
    value = MMPH_HIF_RegGetB(i);
    fprintf(fp, "REG[0x%X] : 0x%x\r\n",i,value);
    for(i=0x8000C858;i<0x8000C878;i++) {
        value = MMPH_HIF_RegGetB(i);
        fprintf(fp, "REG[0x%X] : 0x%x\r\n",i,value);
    }
    for(i=0x8000C880;i<0x8000C88C;i++) {
        value = MMPH_HIF_RegGetB(i);
        fprintf(fp, "REG[0x%X] : 0x%x\r\n",i,value);
    }
    for(i=0x8000C8E8;i<0x8000C8EB;i++) {
        value = MMPH_HIF_RegGetB(i);
        fprintf(fp, "REG[0x%X] : 0x%x\r\n",i,value);
    }
    fclose(fp);
#endif
}

RC_CONFIG_PARAM RcConfig;

MMP_ERR MMPD_H264ENC_GetFreeFrame(MMP_ULONG *ulYAddr,MMP_ULONG *ulUAddr,MMP_ULONG *ulVAddr)
{
    *ulYAddr = m_CurFrame.yuv[0];
    *ulUAddr = m_CurFrame.yuv[1];
    *ulVAddr = m_CurFrame.yuv[2];

    return MMP_ERR_NONE;
}

MMP_ULONG* MMPD_H264ENC_GetSliceInfo(void)
{
    return &(SliceLenBuf[0]);
}


MMP_ULONG MMPD_H264ENC_GetEncFrameNum(void)
{
    return m_ulFrameNum;
}

MMP_ERR MMPD_H264ENC_EncodeRawData(MMPD_H264ENC_FRAME_TYPE FrameType,
                                    MMP_ULONG ulFrameDspyNum, MMP_ULONG ulQP,
                                    MMP_ULONG *ulBSAddr0, MMP_ULONG *ulFrameLen0,
                                    MMP_ULONG *ulBSAddr1, MMP_ULONG *ulFrameLen1,
                                    MMP_BOOL  *bSkipFrame)
{
    MMP_ULONG           i, time_out = 0x1000000;
    MMP_ULONG			value, slice_len_addr, frame_size, SliceNum;
    MMPD_H264ENC_FRAME  tempFrame;
    MMP_ULONG           pending_bytes;
    
    *bSkipFrame = MMP_FALSE;
/*
    if (m_bRCEnable && m_bSkipNextFrame) {
        MMP_ULONG ulNextQP, ulTargetSize;
        MMPF_VidRateCtl_UpdateModel((m_ulFrameNum%m_ulIntraPeriod)?1:0, 0, 0, 0, m_bSkipNextFrame);
        ulNextQP = MMPF_VidRateCtl_Get_VOP_QP(((m_ulFrameNum+1)%m_ulIntraPeriod)?1:0, &ulTargetSize, &m_bSkipNextFrame);
        MMPH_HIF_RegSetB(0x8000C888, ulNextQP);
        MMPH_HIF_RegSetB(H264ENC_BASIC_QP, ulNextQP);
        MMPH_HIF_RegSetL(H264ENC_TARGET_MB_SIZE_TIMES_256, (ulTargetSize*256*8)/(m_ulMBWidth*m_ulMBHeight));

        *ulBSAddr0 = 0;
        *ulBSAddr1 = 0;
        *ulFrameLen0 = 0;
        *ulFrameLen1 = 0;
        RTNA_DBG_Short(0, ulTargetSize);
        RTNA_DBG_Str(0, " target size\r\n");
        RTNA_DBG_Short(0, ulNextQP);
        RTNA_DBG_Str(0, " QP\r\n");
        RTNA_DBG_Short(0, ulFrameDspyNum);
        RTNA_DBG_Str(0, " skip ...\r\n");
        return MMP_ERR_NONE;
    }
*/
    m_ulBSAddr = ALIGN32(m_ulBSAddr);
    MMPH_HIF_RegSetL(H264ENC_BS_START_ADDR, m_ulBSAddr);


    PRINTF("== Encode Order  : [%d]\r\n", m_ulFrameNum);
    PRINTF("== Display Order : [%d]\r\n", ulFrameDspyNum);
	PRINTF("Frame Type       : %d (0:I/P, 1:B)\r\n", FrameType);
    RTNA_DBG_Short(0, m_ulFrameNum); RTNA_DBG_Str(0, " == Encode Frame Order\r\n");
    //RTNA_DBG_Short(0, ulFrameDspyNum); RTNA_DBG_Str(0, " == Display Frame Order\r\n");
    RTNA_DBG_Long(0, m_ulBSAddr); RTNA_DBG_Str(0, " BS Addr\r\n");

    if ( (FrameType == MMPD_H264ENC_FRAME_TYPE_PRM) && (!m_bSkipNextFrame) ){ // Ref/Gen frame swap
        tempFrame.yuv[0] = m_RefFrame.yuv[0];
        tempFrame.yuv[1] = m_RefFrame.yuv[1];
        tempFrame.yuv[2] = m_RefFrame.yuv[2];

        m_RefFrame.yuv[0] = m_GenFrame.yuv[0];
        m_RefFrame.yuv[1] = m_GenFrame.yuv[1];
        m_RefFrame.yuv[2] = m_GenFrame.yuv[2];

        m_GenFrame.yuv[0] = tempFrame.yuv[0];
        m_GenFrame.yuv[1] = tempFrame.yuv[1];
        m_GenFrame.yuv[2] = tempFrame.yuv[2];
		PRINTF("Swap Ref(FW), Gen(Ref BW)\r\n");
    }
	else
		PRINTF("Keep Ref(FW), Gen(Ref BW)\r\n");

    // Set current frame addr
    MMPH_HIF_RegSetL(H264ENC_CURR_Y_ADDR    , m_CurFrame.yuv[0]);
    MMPH_HIF_RegSetB(H264ENC_CURR_Y_ADDR_MSB, m_CurFrame.yuv[0] >> 24);
    MMPH_HIF_RegSetL(H264ENC_CURR_U_ADDR    , m_CurFrame.yuv[1]);
    MMPH_HIF_RegSetB(H264ENC_CURR_U_ADDR_MSB, m_CurFrame.yuv[1] >> 24);
    MMPH_HIF_RegSetL(H264ENC_CURR_V_ADDR    , m_CurFrame.yuv[2]);
    MMPH_HIF_RegSetB(H264ENC_CURR_V_ADDR_MSB, m_CurFrame.yuv[2] >> 24);

    // Set reference frame addr
    if (m_ubSupportFREXT) {
        MMPH_HIF_RegSetL(H264ENC_REF_Y_ADDR    , m_RefFrame.yuv[0]);
        MMPH_HIF_RegSetB(H264ENC_REF_Y_ADDR_MSB, m_RefFrame.yuv[0] >> 24);
        MMPH_HIF_RegSetL(H264ENC_REF_UV_ADDR   , m_RefFrame.yuv[1]);
        MMPH_HIF_RegSetB(H264ENC_REF_UV_ADDR_MSB, m_RefFrame.yuv[1] >> 24);
        // Backward
        if (m_usBFrameNum) {
            MMPH_HIF_RegSetL(H264ENC_REF_BW_Y_ADDR    , m_GenFrame.yuv[0]);
            MMPH_HIF_RegSetB(H264ENC_REF_BW_Y_ADDR_MSB, m_GenFrame.yuv[0] >> 24);
            MMPH_HIF_RegSetL(H264ENC_REF_BW_UV_ADDR   , m_GenFrame.yuv[1]);
            MMPH_HIF_RegSetB(H264ENC_REF_BW_UV_ADDR_MSB, m_GenFrame.yuv[1] >> 24);
        }
    }
    else  {
        MMPH_HIF_RegSetL(H264ENC_REF_Y_ADDR    , m_RefFrame.yuv[0]);
        MMPH_HIF_RegSetB(H264ENC_REF_Y_ADDR_MSB, m_RefFrame.yuv[0] >> 24);
        MMPH_HIF_RegSetL(H264ENC_REF_U_ADDR    , m_RefFrame.yuv[1]);
        MMPH_HIF_RegSetB(H264ENC_REF_U_ADDR_MSB, m_RefFrame.yuv[1] >> 24);
        MMPH_HIF_RegSetL(H264ENC_REF_V_ADDR    , m_RefFrame.yuv[2]);
        MMPH_HIF_RegSetB(H264ENC_REF_V_ADDR_MSB, m_RefFrame.yuv[2] >> 24);
    }

    // REF frame buffer H/L bound
    if (m_ubSupportFREXT) {
        MMPH_HIF_RegSetL(H264ENC_REFBD_Y_LOW, m_RefFrame.yuv[0]);
        MMPH_HIF_RegSetL(H264ENC_REFBD_Y_HIGH, m_RefFrame.yuv[0] + (m_ulFrameSize*2)/3);
        MMPH_HIF_RegSetL(H264ENC_REFBD_UV_LOW, m_RefFrame.yuv[1]);
        MMPH_HIF_RegSetL(H264ENC_REFBD_UV_HIGH, m_RefFrame.yuv[1] + m_ulFrameSize/3);
        // Backward
        if (m_usBFrameNum) {
            MMPH_HIF_RegSetL(H264ENC_REFBD_BW_Y_LOW, m_GenFrame.yuv[0]);
            MMPH_HIF_RegSetL(H264ENC_REFBD_BW_Y_HIGH, m_GenFrame.yuv[0] + (m_ulFrameSize*2)/3);
            MMPH_HIF_RegSetL(H264ENC_REFBD_BW_UV_LOW, m_GenFrame.yuv[1]);
            MMPH_HIF_RegSetL(H264ENC_REFBD_BW_UV_HIGH, m_GenFrame.yuv[1] + m_ulFrameSize/3);
        }
    }
    else {
        MMPH_HIF_RegSetL(H264ENC_REFBD_Y_LOW, m_RefFrame.yuv[0]);
        MMPH_HIF_RegSetL(H264ENC_REFBD_Y_HIGH, m_RefFrame.yuv[0] + (m_ulFrameSize*2)/3);
        MMPH_HIF_RegSetL(H264ENC_REFBD_U_LOW, m_RefFrame.yuv[1]);
        MMPH_HIF_RegSetL(H264ENC_REFBD_U_HIGH, m_RefFrame.yuv[1] + m_ulFrameSize/6);
        MMPH_HIF_RegSetL(H264ENC_REFBD_V_LOW, m_RefFrame.yuv[2]);
        MMPH_HIF_RegSetL(H264ENC_REFBD_V_HIGH, m_RefFrame.yuv[2] + m_ulFrameSize/6);
    }

    MMPH_HIF_RegSetL(0x8000C874, m_GenFrame.yuv[0]); // deblock addr
    MMPH_HIF_RegSetL(0x8000C868, m_GenFrame.yuv[0]); // deblock low bound
    MMPH_HIF_RegSetL(0x8000C86C, m_GenFrame.yuv[0] + m_ulFrameSize); // deblock high bound

    if (FrameType == MMPD_H264ENC_FRAME_TYPE_PRM) {
        if ((m_ulFrameNum % m_ulIntraPeriod) == 0) {
            MMPH_HIF_RegSetB(0x8000C88A, MMPH_HIF_RegGetB(0x8000C88A)|I_SLICE);
			if (m_usBFrameNum) {
                MMPH_HIF_RegSetB(0x8000C88B, MMPH_HIF_RegGetB(0x8000C88B)&(~0x0C));
                MMPH_HIF_RegSetB(0x8000C88A, (MMPH_HIF_RegGetB(0x8000C88A)&0x0F)|0x10); //num_ref_idx_l0_active_minus1
                MMPH_HIF_RegSetB(0x8000C8E8, (MMPH_HIF_RegGetB(0x8000C8E8)&0x0F)|0x10); //num_ref_idx_l1_active_minus1
                MMPH_HIF_RegSetB(0x8000C8E8, MMPH_HIF_RegGetB(0x8000C8E8)&(~0x08)); //num_ref_idx_override_flag
            }
            m_bFrameAfterI = MMP_TRUE;
            m_ulPrevRefFrameNum = -1; // frame_num = 0 for IDR frame
        }
        else {
        	PRINTF("Change MVx search range to 160..............\r\n");
        	MMPH_HIF_RegSetB(0x8000C8EA, (MMPH_HIF_RegGetB(0x8000C8EA)&0xFC) | 0x01); //set |MVx|=160 for P_V2
            MMPH_HIF_RegSetB(0x8000C88A, MMPH_HIF_RegGetB(0x8000C88A)&(~I_SLICE));
            if (m_usBFrameNum) {
                MMPH_HIF_RegSetB(0x8000C88B, MMPH_HIF_RegGetB(0x8000C88B)&(~0x08));
                MMPH_HIF_RegSetB(0x8000C88B, MMPH_HIF_RegGetB(0x8000C88B)|0x04);
                if (m_bFrameAfterI) {
                    MMPH_HIF_RegSetB(0x8000C88A, MMPH_HIF_RegGetB(0x8000C88A)&0x0F); //num_ref_idx_l0_active_minus1
                    MMPH_HIF_RegSetB(0x8000C8E8, (MMPH_HIF_RegGetB(0x8000C8E8)&0x0F)|0x10); //num_ref_idx_l1_active_minus1
                    MMPH_HIF_RegSetB(0x8000C8E8, MMPH_HIF_RegGetB(0x8000C8E8)|0x08); //num_ref_idx_override_flag
                }
                else {
                    MMPH_HIF_RegSetB(0x8000C88A, (MMPH_HIF_RegGetB(0x8000C88A)&0x0F)|0x10); //num_ref_idx_l0_active_minus1
                    MMPH_HIF_RegSetB(0x8000C8E8, (MMPH_HIF_RegGetB(0x8000C8E8)&0x0F)|0x10); //num_ref_idx_l1_active_minus1
                    MMPH_HIF_RegSetB(0x8000C8E8, MMPH_HIF_RegGetB(0x8000C8E8)&(~0x08)); //num_ref_idx_override_flag
                }
            }
        	m_bFrameAfterI = MMP_FALSE;
        }
    }
    else { // B frame
    	MMPH_HIF_RegSetB(0x8000C8EA, (MMPH_HIF_RegGetB(0x8000C8EA)&0xFC) & ~0x01); //restore MVx search range
        if (m_usBFrameNum) {
            MMPH_HIF_RegSetB(0x8000C88B, MMPH_HIF_RegGetB(0x8000C88B)|0x08);
            MMPH_HIF_RegSetB(0x8000C88B, MMPH_HIF_RegGetB(0x8000C88B)&(~0x04));
            MMPH_HIF_RegSetB(0x8000C88A, MMPH_HIF_RegGetB(0x8000C88A)&0x0F); //num_ref_idx_l0_active_minus1
            MMPH_HIF_RegSetB(0x8000C8E8, MMPH_HIF_RegGetB(0x8000C8E8)&0x0F); //num_ref_idx_l1_active_minus1
            MMPH_HIF_RegSetB(0x8000C8E8, MMPH_HIF_RegGetB(0x8000C8E8)|0x08); //num_ref_idx_override_flag
            MMPH_HIF_RegSetW(0x8000F8F8, m_ulBMVWeight[((ulFrameDspyNum%m_ulIntraPeriod)%(1+m_usBFrameNum))-1]); //julian tbd
            PRINTF("B Forward Prediction Weight : %d\r\n", MMPH_HIF_RegGetW(0x8000F8F8)); //julian tbd
            RTNA_DBG_Short(0, MMPH_HIF_RegGetW(0x8000F8F8)); RTNA_DBG_Str(0, " B Forward Prediction Weight\r\n");
        }
        m_bFrameAfterI = MMP_FALSE;
    }

    PRINTF("m_ulFrameNum    : %d\r\n", m_ulFrameNum);
    PRINTF("frame_num       : %d\r\n", m_ulPrevRefFrameNum+1);
    PRINTF("ulFrameDspyNum  : %d\r\n", ulFrameDspyNum);

    MMPH_HIF_RegSetW(H264ENC_IDR_FRAME, (m_ulFrameNum / m_ulIntraPeriod) & 0x01); // IDR, julian tbd
    MMPH_HIF_RegSetW(H264ENC_FRAME_NUM, (m_ulPrevRefFrameNum+1)); // frame num
    //MMPH_HIF_RegSetW(H264ENC_POC, (ulFrameDspyNum % m_ulIntraPeriod) * 2);  // POC
    MMPH_HIF_RegSetW(H264ENC_POC, (m_ulPrevRefFrameNum+1)*2);  // POC
    /*
    if (FrameType == MMPD_H264ENC_FRAME_TYPE_PRM) {
        m_ulPrevRefFrameNum++; // Only primary frame can be Ref frame
    }
    */

    if (m_bTestPlanMode && (m_ulFrameNum == 0)) { // by titany, for tp, 20091209
    	MMPD_H264ENC_SetTPReg();
    }
    if (m_bRCEnable) {
        MMP_ULONG ulNextQP, ulTargetSize, ulQPDelta;
        
        //swyang        
        if((m_ulFrameNum+1)%m_ulIntraPeriod)         
        	MMPH_HIF_RegSetW(H264ENC_QP_FINE_TUNE_GAP, RcConfig.MaxQPDelta[1]);
        else
        	MMPH_HIF_RegSetW(H264ENC_QP_FINE_TUNE_GAP, RcConfig.MaxQPDelta[0]);
        
        ulNextQP = MMPF_VidRateCtl_Get_VOP_QP(m_RCHandle,((m_ulFrameNum+1)%m_ulIntraPeriod)?1:0, &ulQPDelta);
        
        MMPH_HIF_RegSetB(0x8000C888, ulNextQP);
        MMPH_HIF_RegSetB(H264ENC_BASIC_QP, ulNextQP);
        MMPH_HIF_RegSetL(H264ENC_TARGET_MB_SIZE_TIMES_256, (ulTargetSize*256*8)/(m_ulMBWidth*m_ulMBHeight));
    }
    

    MMPH_HIF_RegSetB(0x8000C800, MMPH_HIF_RegGetB(0x8000C800) | 0x04); // frame start
    MMPH_HIF_RegSetB(H264ENC_FRAME_CTL, MMPH_HIF_RegGetB(H264ENC_FRAME_CTL) | H264_ENCODE_EN);
    i = 0;
    do {
        value = MMPH_HIF_RegGetB(H264ENC_FRAME_CTL);
        i++;
        if (i >= time_out)
            break;
    } while ((value & H264_ENCODE_EN)); // Polling frame end

    if (i >= time_out) {
        PRINTF("TIMEOUT!\r\n");
        RTNA_DBG_Str(0, "TIMEOUT!\r\n");
        MMPD_H264ENC_DumpReg("D:\\h264e_lockup_opr.txt");
        return MMP_SYSTEM_ERR_HW;
    }

	SliceNum = MMPH_HIF_RegGetW(H264ENC_SLICE_SIZE_IN_ROW);
	SliceNum = (m_ulMBHeight + SliceNum - 1) / SliceNum;
    slice_len_addr = MMPH_HIF_RegGetL(H264ENC_SLICE_LEN_BUF_ADDR);
    frame_size = 0;
    i = 0;
    if ((MMPH_HIF_RegGetB(0x8000C88A)&I_SLICE)&&(MMPH_HIF_RegGetB(H264ENC_VLC_CTRL2)&INSERT_SPS_PPS_BEFORE_I_SLICE)) {
        MMPH_HIF_RegSetB(H264ENC_VLC_CTRL2,(MMPH_HIF_RegGetB(H264ENC_VLC_CTRL2) & ~INSERT_SPS_PPS_BEFORE_I_SLICE));
        MMPH_HIF_MemCopyDevToHost((MMP_UBYTE*)(&SliceLenBuf[0]),slice_len_addr,4);
        MMPH_HIF_MemCopyDevToHost((MMP_UBYTE*)(&SliceLenBuf[1]),slice_len_addr+4,4);
        frame_size = SliceLenBuf[0] + SliceLenBuf[1];
        slice_len_addr += 8;
        i = 2;
        SliceNum += 2;
    }
	for ( ; i < SliceNum; i++) {
		MMPH_HIF_MemCopyDevToHost((MMP_UBYTE*)(&SliceLenBuf[i]), slice_len_addr, 4);
		slice_len_addr += 4;
		frame_size += SliceLenBuf[i];
		PRINTF("  > Slice[%02d]= %d\r\n", i, SliceLenBuf[i]);
	}
    if (m_bRCEnable) {
        MMP_ULONG ulNextQP, ulTargetSize;
        MMPF_VidRateCtl_UpdateModel(m_RCHandle, (m_ulFrameNum%m_ulIntraPeriod)?1:0, frame_size, //arg1:0=I,1=P
                (MMPH_HIF_RegGetL(H264ENC_INTRAMB_BITS_COUNT)+MMPH_HIF_RegGetL(H264ENC_MV_BITS_COUNT)+
                MMPH_HIF_RegGetL(H264ENC_HEADER_BITS_COUNT))/8,
                MMPH_HIF_RegGetL(H264ENC_QP_SUM)/(m_ulMBWidth*m_ulMBHeight), &m_bSkipNextFrame,&pending_bytes);
    }

    *bSkipFrame = m_bSkipNextFrame;
    
    if(m_bSkipNextFrame == MMP_FALSE) 
    {
    
    if (FrameType == MMPD_H264ENC_FRAME_TYPE_PRM) {
        m_ulPrevRefFrameNum++; // Only primary frame can be Ref frame
    }
    
	PRINTF("0x%08X = %d\r\n", H264ENC_FRAME_LENGTH, MMPH_HIF_RegGetL(H264ENC_FRAME_LENGTH)&0xFFFFFF);
	PRINTF("frame size  : %d\r\n", frame_size);
    RTNA_DBG_Long(0, frame_size); RTNA_DBG_Str(0, " frame_size\r\n");


    if ((m_ulBSAddr + frame_size) < m_ulBSEndAddr) {
        *ulBSAddr0   = m_ulBSAddr;
        *ulFrameLen0 = frame_size;
        *ulBSAddr1   = 0;
        *ulFrameLen1 = 0;
        m_ulBSAddr   += frame_size;
    }
    else {
    	PRINTF("RING!\r\n");
        *ulBSAddr0   = m_ulBSAddr;
        *ulFrameLen0 = (m_ulBSEndAddr -m_ulBSAddr);
        *ulBSAddr1   = m_ulBSStartAddr;
        *ulFrameLen1 = frame_size - (m_ulBSEndAddr - m_ulBSAddr);
        m_ulBSAddr   = m_ulBSStartAddr + frame_size - (m_ulBSEndAddr - m_ulBSAddr);

        PRINTF("*ulBSAddr0   : 0x%x\r\n",*ulBSAddr0);
        PRINTF("*ulFrameLen0 : %d\r\n",*ulFrameLen0);
        PRINTF("*ulBSAddr1   : 0x%x\r\n",*ulBSAddr1);
        PRINTF("*ulFrameLen1 : %d\r\n",*ulFrameLen1);
    }

    if (m_ulFrameNum % m_ulIntraPeriod) { // not I frame
        MMPH_HIF_RegSetB(H264ENC_FIRST_P_AFTER_I_IDX, 0);
    }
    /*
    if (m_bRCEnable) {
        MMP_LONG ulNextQP;
        MMP_ULONG ulTargetSize;
        #if (DYNA_QP_GAP == 1)
        MMP_LONG low, high, local_low, local_delta;
        #endif
        MMPF_VidRateCtl_UpdateModel((m_ulFrameNum%m_ulIntraPeriod)?1:0, frame_size, //arg1:0=I,1=P
                (MMPH_HIF_RegGetL(H264ENC_INTRAMB_BITS_COUNT)+MMPH_HIF_RegGetL(H264ENC_MV_BITS_COUNT)+
                MMPH_HIF_RegGetL(H264ENC_HEADER_BITS_COUNT))/8,
                MMPH_HIF_RegGetL(H264ENC_QP_SUM)/(m_ulMBWidth*m_ulMBHeight), m_bSkipNextFrame);
        ulNextQP = MMPF_VidRateCtl_Get_VOP_QP(((m_ulFrameNum+1)%m_ulIntraPeriod)?1:0, &ulTargetSize, &m_bSkipNextFrame);
        MMPH_HIF_RegSetB(0x8000C888, ulNextQP);
        MMPH_HIF_RegSetB(H264ENC_BASIC_QP, ulNextQP);
        MMPH_HIF_RegSetL(H264ENC_TARGET_MB_SIZE_TIMES_256, (ulTargetSize*256*8)/(m_ulMBWidth*m_ulMBHeight));
        #if (DYNA_QP_GAP == 1)
        MMPF_VidRateCtl_GetQPBoundary(&low, &high);
        if (ulNextQP < m_ulRCTuneGap)
            local_low = 0;
        else
            local_low = (ulNextQP - m_ulRCTuneGap);
        local_delta = (high - local_low)/2;
        MMPH_HIF_RegSetB(0x8000C888, local_low+local_delta);
        MMPH_HIF_RegSetB(H264ENC_BASIC_QP, local_low+local_delta);
        MMPH_HIF_RegSetW(H264ENC_QP_FINE_TUNE_GAP, local_delta);
        #endif
        if (m_bSkipNextFrame) {
            RTNA_DBG_Str(0, "RC: skip next frame ...\r\n");
            PRINTF("RC: skip next frame ...\r\n");
        }
        else {
            PRINTF("RC: next QP = %d\r\n", ulNextQP);
        }
		*/
		m_ulFrameNum++;
    }
	
    

    return MMP_ERR_NONE;
}

/// @}
/// @end_ait_only

#ifdef BUILD_CE
#define BUILD_FW
#endif
/// @}
/// @end_ait_only

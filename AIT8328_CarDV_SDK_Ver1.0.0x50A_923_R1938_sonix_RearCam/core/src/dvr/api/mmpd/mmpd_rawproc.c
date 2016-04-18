//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================

#include "hdr_cfg.h"
#include "mmph_hif.h"
#include "mmpd_rawproc.h"
#include "mmpd_system.h"
#include "mmpd_sensor.h"
#include "mmpf_rawproc.h"
#include "mmpf_sensor.h"

//==============================================================================
//
//                              FUNCTIONS
//
//==============================================================================

//------------------------------------------------------------------------------
//  Function    : MMPD_RAWPROC_EnablePath
//  Description :
//------------------------------------------------------------------------------
/** @brief Enable/Disable raw path to store/fetch data

@param[in] bEnable  	Enable/disable bayer store engine
@param[in] pathSelect 	Path selection
@param[in] colorFormat 	10bit mode or 8bit mode
@retval MMP_ERR_NONE on success
@retval MMP_RAWPROC_ERR_UNSUPPORTED_PARAMETERS parameter wrong
*/
MMP_ERR MMPD_RAWPROC_EnablePath(MMP_UBYTE           ubSnrSel,
                                MMP_UBYTE           ubRawId,
                                MMP_BOOL 			bEnable, 
                                MMP_RAW_IOPATH 	 	pathSelect,
                                MMP_RAW_COLORFMT 	colorFormat)
{
    MMP_ERR     ret = MMP_ERR_NONE;
    MMP_UBYTE   ubVifId = MMPD_Sensor_GetVIFPad(ubSnrSel);

	/* Path selection check */
    if ((pathSelect & MMP_RAW_IOPATH_VIF2ISP) &&
        (pathSelect & MMP_RAW_IOPATH_RAW2ISP)) {
        return MMP_RAWPROC_ERR_UNSUPPORTED_PARAMETERS;
    }
    if ((pathSelect & MMP_RAW_IOPATH_VIF2RAW) &&
        (pathSelect & MMP_RAW_IOPATH_ISP2RAW)) {
        return MMP_RAWPROC_ERR_UNSUPPORTED_PARAMETERS;
    }
    
    /* Color depth check */
    if (colorFormat >= MMP_RAW_COLORFMT_NUM) {
        return MMP_RAWPROC_ERR_PARAMETER;
    }

    MMPD_System_EnableClock(MMPD_SYS_CLK_VIF, MMP_TRUE);
    MMPD_System_EnableClock(MMPD_SYS_CLK_RAW_F, MMP_TRUE);
    MMPD_System_EnableClock(MMPD_SYS_CLK_RAW_S0, MMP_TRUE);
    MMPD_System_EnableClock(MMPD_SYS_CLK_RAW_S1, MMP_TRUE);
	MMPD_System_EnableClock(MMPD_SYS_CLK_SCALE, MMP_TRUE);

    if (bEnable) 
    {
    	/* Set Bit Mode */
    	if (colorFormat == MMP_RAW_COLORFMT_YUV420) {
            MMPF_RAWPROC_SetStoreBitMode(ubRawId, MMP_RAW_STORE_PLANE0, MMP_RAW_COLORFMT_BAYER8);  	
    	    MMPF_RAWPROC_SetStoreBitMode(ubRawId, MMP_RAW_STORE_PLANE1, MMP_RAW_COLORFMT_BAYER8);
    	    MMPF_RAWPROC_SetStoreBitMode(ubRawId, MMP_RAW_STORE_PLANE2, MMP_RAW_COLORFMT_BAYER8);
    	
    	    MMPF_RAWPROC_SetFetchBitMode(MMP_RAW_COLORFMT_BAYER8);
    	}
    	else if (colorFormat == MMP_RAW_COLORFMT_YUV422) {
            MMPF_RAWPROC_SetStoreBitMode(ubRawId, MMP_RAW_STORE_PLANE0, MMP_RAW_COLORFMT_BAYER8);
    	    MMPF_RAWPROC_SetFetchBitMode(MMP_RAW_COLORFMT_BAYER8);
    	}
    	else {
            MMPF_RAWPROC_SetStoreBitMode(ubRawId, MMP_RAW_STORE_PLANE0, colorFormat);
            MMPF_RAWPROC_SetFetchBitMode(colorFormat);
        }
         
        /* Set Pixel/Line Delay */
        if (gsHdrCfg.bEnable) {
        	MMPF_RAWPROC_SetFetchPixelDelay(0);
     	}
     	else {
        	MMPF_RAWPROC_SetFetchPixelDelay(MMPF_RAWPROC_CalcFetchPixelDelay());
        }
        MMPF_RAWPROC_SetFetchLineDelay(0x20); // Change to 0x20 for raw fetch zoom.

        /* Set VIF Output */
        if (pathSelect & MMP_RAW_IOPATH_VIF2ISP) {
        	MMPF_VIF_SetRawOutPath(ubVifId, MMP_TRUE, MMP_FALSE);
        }
        else {
            MMPF_VIF_SetRawOutPath(ubVifId, MMP_FALSE, MMP_TRUE);
        }
        
        MMPH_HIF_WaitSem(GRP_IDX_FLOWCTL, 0);
    
        /* Set Raw Fetch Enable */
        if (pathSelect & MMP_RAW_IOPATH_RAW2ISP) {
            MMPH_HIF_SetParameterB(GRP_IDX_FLOWCTL, 0, MMP_TRUE);
        }
        else {
            MMPH_HIF_SetParameterB(GRP_IDX_FLOWCTL, 0, MMP_FALSE);
        }
        
        /* Set Raw Store Enable and Source */
        if (pathSelect & (MMP_RAW_IOPATH_VIF2RAW | MMP_RAW_IOPATH_ISP2RAW)) {
            
            if (pathSelect & MMP_RAW_IOPATH_VIF2RAW) {
                if (ubVifId == MMPF_VIF_MDL_ID0)
                    MMPH_HIF_SetParameterB(GRP_IDX_FLOWCTL, 1, MMP_RAW_S_SRC_VIF0);
                else if (ubVifId == MMPF_VIF_MDL_ID1)
                    MMPH_HIF_SetParameterB(GRP_IDX_FLOWCTL, 1, MMP_RAW_S_SRC_VIF1);                
            }
            else if (pathSelect & MMP_RAW_IOPATH_ISP2RAW) {
                MMPH_HIF_SetParameterB(GRP_IDX_FLOWCTL, 1, MMP_RAW_S_SRC_ISP_BEFORE_BS);//TBD               
            }
            MMPH_HIF_SetParameterB(GRP_IDX_FLOWCTL, 2, MMP_TRUE);
        }
        else {
            if (ubVifId == MMPF_VIF_MDL_ID0)
                MMPH_HIF_SetParameterB(GRP_IDX_FLOWCTL, 1, MMP_RAW_S_SRC_VIF0);
            else if (ubVifId == MMPF_VIF_MDL_ID1)
                MMPH_HIF_SetParameterB(GRP_IDX_FLOWCTL, 1, MMP_RAW_S_SRC_VIF1); 
            MMPH_HIF_SetParameterB(GRP_IDX_FLOWCTL, 2, MMP_FALSE);
        }
		
		/* Set Raw Module ID */
		MMPH_HIF_SetParameterB(GRP_IDX_FLOWCTL, 3, ubRawId);
		
		/* Set Raw Store Color Format (For Other Plane Store Enable) */
		MMPH_HIF_SetParameterB(GRP_IDX_FLOWCTL, 4, colorFormat);
		
        ret = MMPH_HIF_SendCmd(GRP_IDX_FLOWCTL, HIF_FCTL_CMD_SET_RAW_PREVIEW | ENABLE_RAW_PATH);
        MMPH_HIF_ReleaseSem(GRP_IDX_FLOWCTL);

        if (ret != MMP_ERR_NONE) {
            return ret;
    	}
    }
    else 
    {    
    	/* Set VIF Output */
        MMPF_VIF_SetRawOutPath(ubRawId, MMP_TRUE, MMP_FALSE);
        
        /* Set Raw Fetch Disable */
        MMPH_HIF_WaitSem(GRP_IDX_FLOWCTL, 0);

        MMPH_HIF_SetParameterB(GRP_IDX_FLOWCTL, 0, MMP_FALSE);

        /* Set Raw Store Disable and Source */
        if (pathSelect & MMP_RAW_IOPATH_VIF2RAW) {
            if (ubVifId == MMPF_VIF_MDL_ID0)
                MMPH_HIF_SetParameterB(GRP_IDX_FLOWCTL, 1, MMP_RAW_S_SRC_VIF0);
            else if (ubVifId == MMPF_VIF_MDL_ID1)
                MMPH_HIF_SetParameterB(GRP_IDX_FLOWCTL, 1, MMP_RAW_S_SRC_VIF1);
        }
        else {
            MMPH_HIF_SetParameterB(GRP_IDX_FLOWCTL, 1, MMP_RAW_S_SRC_ISP_BEFORE_BS); 
        }
        MMPH_HIF_SetParameterB(GRP_IDX_FLOWCTL, 2, MMP_FALSE);

        /* Set Raw Module ID */
		MMPH_HIF_SetParameterB(GRP_IDX_FLOWCTL, 3, ubRawId);

		/* Set Raw Store Color Format (For Other Plane Store Enable) */
		MMPH_HIF_SetParameterB(GRP_IDX_FLOWCTL, 4, colorFormat);

        ret = MMPH_HIF_SendCmd(GRP_IDX_FLOWCTL, HIF_FCTL_CMD_SET_RAW_PREVIEW | ENABLE_RAW_PATH);
        MMPH_HIF_ReleaseSem(GRP_IDX_FLOWCTL);

        if (ret != MMP_ERR_NONE) {
            return ret;
    	}
    }

    MMPD_System_EnableClock(MMPD_SYS_CLK_VIF, MMP_FALSE);
    MMPD_System_EnableClock(MMPD_SYS_CLK_RAW_F, MMP_FALSE);
    MMPD_System_EnableClock(MMPD_SYS_CLK_RAW_S0, MMP_FALSE);
    MMPD_System_EnableClock(MMPD_SYS_CLK_RAW_S1, MMP_FALSE);
	MMPD_System_EnableClock(MMPD_SYS_CLK_SCALE, MMP_FALSE);

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_RAWPROC_SetStoreBuf
//  Description :
//------------------------------------------------------------------------------
/** @brief Set raw store buffer address
@param[in] rawbuf  bayer & luma buffer address
  - ulRawBufAddr/ulLumaBufAddr must start from 0 to ulRawBufCnt-1/ulLumaBufCnt-1
@return It reports the status of the configuration.
*/
MMP_ERR MMPD_RAWPROC_SetStoreBuf(MMP_UBYTE ubRawId, MMP_RAW_STORE_BUF *pRawBuf)
{
    MMP_ULONG 	i;
    MMP_ERR 	ret = MMP_ERR_NONE;
	
    MMPD_System_EnableClock(MMPD_SYS_CLK_RAW_S0, MMP_TRUE);
    MMPD_System_EnableClock(MMPD_SYS_CLK_RAW_S1, MMP_TRUE);
	MMPD_System_EnableClock(MMPD_SYS_CLK_RAW_F, MMP_TRUE);
	
    if (pRawBuf->ulRawBufCnt > 0) {
    	MMPF_RAWPROC_InitStoreBuffer(ubRawId, MMP_RAW_STORE_PLANE0, pRawBuf->ulRawBufAddr[0]);
    	
    	/* For Store YUV420 Format */
    	if (pRawBuf->ulRawBufUAddr[0] != 0 && pRawBuf->ulRawBufVAddr[0] != 0) {
    	    MMPF_RAWPROC_InitStoreBuffer(ubRawId, MMP_RAW_STORE_PLANE1, pRawBuf->ulRawBufUAddr[0]);
    	    MMPF_RAWPROC_InitStoreBuffer(ubRawId, MMP_RAW_STORE_PLANE2, pRawBuf->ulRawBufVAddr[0]);
        }
    }
      
    MMPH_HIF_WaitSem(GRP_IDX_FLOWCTL, 0);
    MMPH_HIF_SetParameterL(GRP_IDX_FLOWCTL, 0, ubRawId);
    MMPH_HIF_SetParameterL(GRP_IDX_FLOWCTL, 4, pRawBuf->ulRawBufCnt);
    
    for (i = 0; i < pRawBuf->ulRawBufCnt; i++) {
        MMPH_HIF_SetParameterL(GRP_IDX_FLOWCTL, (i+2)*4, pRawBuf->ulRawBufAddr[i]);
    }
    
    ret = MMPH_HIF_SendCmd(GRP_IDX_FLOWCTL, HIF_FCTL_CMD_SET_RAW_PREVIEW | SET_RAWSTORE_ADDR);
    
    /* For Store YUV420 Format */
    if (pRawBuf->ulRawBufUAddr[0] != 0 && pRawBuf->ulRawBufVAddr[0] != 0) 
    {
        for (i = 0; i < pRawBuf->ulRawBufCnt; i++) {
            MMPH_HIF_SetParameterL(GRP_IDX_FLOWCTL, (i+2)*4, pRawBuf->ulRawBufUAddr[i]);
        }
        
        ret = MMPH_HIF_SendCmd(GRP_IDX_FLOWCTL, HIF_FCTL_CMD_SET_RAW_PREVIEW | SET_RAWSTORE_UADDR);

        for (i = 0; i < pRawBuf->ulRawBufCnt; i++) {
            MMPH_HIF_SetParameterL(GRP_IDX_FLOWCTL, (i+2)*4, pRawBuf->ulRawBufVAddr[i]);
        }
        
        ret = MMPH_HIF_SendCmd(GRP_IDX_FLOWCTL, HIF_FCTL_CMD_SET_RAW_PREVIEW | SET_RAWSTORE_VADDR);
    }
    
    MMPH_HIF_ReleaseSem(GRP_IDX_FLOWCTL);
    
    MMPD_System_EnableClock(MMPD_SYS_CLK_RAW_S0, MMP_FALSE);
	MMPD_System_EnableClock(MMPD_SYS_CLK_RAW_S1, MMP_FALSE);
	MMPD_System_EnableClock(MMPD_SYS_CLK_RAW_F, MMP_FALSE);	
	
    return ret;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_RAWPROC_EnablePreview
//  Description :
//------------------------------------------------------------------------------
/** @brief Enable raw preview
@param[in] bEnable  Enable/Disable
@return It reports the status of the configuration.
*/
MMP_ERR MMPD_RAWPROC_EnablePreview(MMP_UBYTE ubRawId, MMP_BOOL bEnable)
{
    MMP_ERR ret = MMP_ERR_NONE;
	
	MMPH_HIF_WaitSem(GRP_IDX_FLOWCTL, 0);
	MMPH_HIF_SetParameterL(GRP_IDX_FLOWCTL, 0, ubRawId);
    MMPH_HIF_SetParameterL(GRP_IDX_FLOWCTL, 4, bEnable);
        
    ret = MMPH_HIF_SendCmd(GRP_IDX_FLOWCTL, HIF_FCTL_CMD_SET_RAW_PREVIEW | ENABLE_RAWPREVIEW);
    MMPH_HIF_ReleaseSem(GRP_IDX_FLOWCTL);

    return ret;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_RAWPROC_SetStoreOnly
//  Description :
//------------------------------------------------------------------------------
/** @brief Enable raw store only attribute.
@param[in] bEnable  Enable/Disable
@return It reports the status of the configuration.
*/
MMP_ERR MMPD_RAWPROC_SetStoreOnly(MMP_UBYTE ubRawId, MMP_BOOL bEnable)
{
    MMP_ERR ret = MMP_ERR_NONE;
	
	MMPH_HIF_WaitSem(GRP_IDX_FLOWCTL, 0);
	MMPH_HIF_SetParameterL(GRP_IDX_FLOWCTL, 0, ubRawId);
    MMPH_HIF_SetParameterL(GRP_IDX_FLOWCTL, 4, bEnable);

    ret = MMPH_HIF_SendCmd(GRP_IDX_FLOWCTL, HIF_FCTL_CMD_SET_RAW_PREVIEW | SET_RAWSTORE_ONLY);
    MMPH_HIF_ReleaseSem(GRP_IDX_FLOWCTL);

    return ret;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_RAWPROC_GetStoreRange
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPD_RAWPROC_GetStoreRange(MMP_UBYTE ubVifId, MMP_USHORT *usWidth, MMP_USHORT *usHeight)
{
#if 1
    MMPD_System_EnableClock(MMPD_SYS_CLK_VIF, MMP_TRUE);
    MMPF_VIF_GetGrabResolution(ubVifId, (MMP_ULONG*)usWidth, (MMP_ULONG*)usHeight);
    MMPD_System_EnableClock(MMPD_SYS_CLK_VIF, MMP_FALSE);
#else
    MMPF_RAWPROC_GetStoreRange(ubVifId, usWidth, usHeight);
#endif
	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_RAWPROC_SetFetchRange
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPD_RAWPROC_SetFetchRange (MMP_USHORT usLeft,
                                    MMP_USHORT usTop,
                                    MMP_USHORT usWidth,
                                    MMP_USHORT usHeight,
                                    MMP_USHORT usLineOffset)
{
    MMP_ERR ret = MMP_ERR_NONE;
	
	MMPH_HIF_WaitSem(GRP_IDX_FLOWCTL, 0);  
    MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 0, usLeft);
    MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 2, usTop);
    MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 4, usWidth);
    MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 6, usHeight);
    MMPH_HIF_SetParameterW(GRP_IDX_FLOWCTL, 8, usLineOffset);
    
    ret = MMPH_HIF_SendCmd(GRP_IDX_FLOWCTL, HIF_FCTL_CMD_SET_RAW_PREVIEW | SET_FETCH_RANGE);
    MMPH_HIF_ReleaseSem(GRP_IDX_FLOWCTL);
	   
    return ret;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_RAWPROC_CalcBufSize
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPD_RAWPROC_CalcBufSize(MMP_RAW_COLORFMT colorFormat, MMP_USHORT usWidth, MMP_USHORT usHeight, MMP_ULONG* ulSize)
{
	return MMPF_RAWPROC_CalcBufSize(colorFormat, usWidth, usHeight, ulSize);
}

//------------------------------------------------------------------------------
//  Function    : MMPD_RAWPROC_SetDeInterlaceBuf
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPD_RAWPROC_SetDeInterlaceBuf(MMP_ULONG ulYAddr, MMP_ULONG ulUAddr, MMP_ULONG ulVAddr, MMP_ULONG ulBufCnt)
{
    return MMPF_RAWPROC_SetDeInterlaceBuf(ulYAddr, ulUAddr, ulVAddr, ulBufCnt);
}

//------------------------------------------------------------------------------
//  Function    : MMPD_RAWPROC_SetRawStoreBufferEnd
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPD_RAWPROC_SetRawStoreBufferEnd(MMP_UBYTE ubRawId, MMP_UBYTE ubPlane,
										  MMP_UBYTE ubBufIdx, MMP_ULONG ulAddr)
{
    return MMPF_RAWPROC_SetRawStoreBufferEnd(ubRawId, ubPlane,
										   ubBufIdx,  ulAddr);
}
//------------------------------------------------------------------------------
//  Function    : MMPD_RAWPROC_EnableRingStore
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPD_RAWPROC_EnableRingStore(MMP_UBYTE ubRawId, MMP_UBYTE ubPlane, MMP_BOOL bRingBufEn)
{
    MMP_ERR ret = MMP_ERR_NONE;
    MMPD_System_EnableClock(MMPD_SYS_CLK_RAW_S0, MMP_TRUE);
    MMPD_System_EnableClock(MMPD_SYS_CLK_RAW_S1, MMP_TRUE);
	MMPD_System_EnableClock(MMPD_SYS_CLK_RAW_F, MMP_TRUE);    
    ret = MMPF_RAWPROC_EnableRingStore(ubRawId, ubPlane, bRingBufEn);
    MMPD_System_EnableClock(MMPD_SYS_CLK_RAW_S0, MMP_FALSE);
    MMPD_System_EnableClock(MMPD_SYS_CLK_RAW_S1, MMP_FALSE);
	MMPD_System_EnableClock(MMPD_SYS_CLK_RAW_F, MMP_FALSE);
    return ret;
}
#if 0
void ____RAW_HDR_Function____(){ruturn;} //dummy
#endif

//------------------------------------------------------------------------------
//  Function    : MMPD_HDR_InitModule
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPD_HDR_InitModule(MMP_USHORT usSubW, MMP_USHORT usSubH)
{
	return MMPF_HDR_InitModule(usSubW, usSubH);
}

//------------------------------------------------------------------------------
//  Function    : MMPD_HDR_UnInitModule
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPD_HDR_UnInitModule(MMP_UBYTE ubSnrSel)
{
	return MMPF_HDR_UnInitModule(ubSnrSel);
}

//------------------------------------------------------------------------------
//  Function    : MMPD_HDR_SetBufEnd
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPD_HDR_SetBufEnd(MMP_ULONG ulEndAddr)
{
	return MMPF_HDR_SetBufEnd(ulEndAddr);
}
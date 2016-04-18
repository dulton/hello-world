/// @ait_only
//==============================================================================
//
//  File        : mmpd_icon.c
//  Description : Retina Icon Module Control driver function
//  Author      : Penguin Torng
//  Revision    : 1.0
//
//==============================================================================

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================

#include "mmp_lib.h"
#include "mmpd_icon.h"
#include "mmpd_system.h"
#include "mmpf_icon.h"

/** @addtogroup MMPD_Icon
 *  @{
 */
//==============================================================================
//
//                              GLOBAL VARIABLES
//
//==============================================================================

static MMP_ICO_DL_SETTING m_iconsetting[MMP_ICO_PIPE_NUM];

//==============================================================================
//
//                              FUNCTIONS
//
//==============================================================================

//------------------------------------------------------------------------------
//  Function    : MMPD_Icon_ResetModule
//  Description : This function reset Icon module.
//------------------------------------------------------------------------------
MMP_ERR MMPD_Icon_ResetModule(MMP_ICO_PIPEID pipeID)
{
	return MMPF_Icon_ResetModule(pipeID);
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Icon_GetAttributes
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPD_Icon_GetAttributes(MMP_UBYTE ubIconID, MMP_STICKER_ATTR *pBufAttr)
{
    MMPD_System_EnableClock(MMPD_SYS_CLK_ICON, MMP_TRUE);
    
    MMPF_Icon_GetAttributes(ubIconID, pBufAttr);

    MMPD_System_EnableClock(MMPD_SYS_CLK_ICON, MMP_FALSE);

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Icon_SetAttributes
//  Description :
//------------------------------------------------------------------------------
/** @brief The function sets the attributes to the specified icon with its icon ID

The function sets the attributes to the specified icon with its icon ID. These attributes include icon buffer
starting address, the width, the height and its display location. It is implemented by programming Icon
Controller registers to set those attributes.

  @param[in] usIconID the ICON ID
  @param[in] pBufAttr the ICON attribute buffer
  @return It reports the status of the operation.
*/
MMP_ERR MMPD_Icon_SetAttributes(MMP_UBYTE ubIconID, MMP_STICKER_ATTR *pBufAttr)
{
	MMP_ERR err = MMP_ERR_NONE;
    MMPD_System_EnableClock(MMPD_SYS_CLK_ICON, MMP_TRUE);
    
	MMPF_Icon_SetAttributes(ubIconID, pBufAttr);

    MMPD_System_EnableClock(MMPD_SYS_CLK_ICON, MMP_FALSE);

    return err;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Icon_SetSemiTP
//  Description :
//------------------------------------------------------------------------------
/** @brief The function activates transparent operation of ICON

The function activates transparent operation with its transparent color from the input parameter or
deactivates ¡§transparency¡¨ operation which sets transparent color to be 0. The previous transparent
color value is returned

  @param[in] usIconID the ICON ID
  @param[in] bSemiTranspActive activate or deactivate semi-transparent operation
  @param[in] ulWeight semi-transparent weighting
  @return It reports the status of the operation.
*/
MMP_ERR MMPD_Icon_SetSemiTP(MMP_UBYTE ubIconID, MMP_BOOL bSemiTPEn, MMP_ULONG ulWeight)
{
    MMPD_System_EnableClock(MMPD_SYS_CLK_ICON, MMP_TRUE);
    
	MMPF_Icon_SetSemiTP(ubIconID, bSemiTPEn, ulWeight);
    					
    MMPD_System_EnableClock(MMPD_SYS_CLK_ICON, MMP_FALSE);

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Icon_GetSemiTP_Weight
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPD_Icon_GetSemiTP_Weight(MMP_UBYTE ubIconID, MMP_USHORT *pusWeight)
{
    MMPD_System_EnableClock(MMPD_SYS_CLK_ICON, MMP_TRUE);
    
    MMPF_Icon_GetSemiTP_Weight(ubIconID, pusWeight);

    MMPD_System_EnableClock(MMPD_SYS_CLK_ICON, MMP_FALSE);

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Icon_SetTransparent
//  Description :
//------------------------------------------------------------------------------
/** @brief The function activates transparent operation of ICON

The function activates transparent operation with its transparent color from the input parameter or
deactivates ¡§transparency¡¨ operation which sets transparent color to be 0. The previous transparent
color value is returned

  @param[in] usIconID the ICON ID
  @param[in] bTranspActive activate or deactivate transparent operation
  @param[in] ulTranspColor transparent color to set
  @return It reports the status of the operation.
*/
MMP_ERR MMPD_Icon_SetTransparent(MMP_UBYTE ubIconID, MMP_BOOL bTranspActive, MMP_ULONG ulTranspColor)
{
    MMPD_System_EnableClock(MMPD_SYS_CLK_ICON, MMP_TRUE);

	MMPF_Icon_SetTransparent(ubIconID, bTranspActive, ulTranspColor);

    MMPD_System_EnableClock(MMPD_SYS_CLK_ICON, MMP_FALSE);

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Icon_SetEnable
//  Description :
//------------------------------------------------------------------------------
/** @brief The function enables or disables an icon on the display device

The function enables or disables icon with the specified icon ID on the display device. It is implemented
by programming Icon Controller registers.

  @param[in] usIconID the ICON ID
  @param[in] bEnable enable or disable ICON
  @return It reports the status of the operation.
*/
MMP_ERR MMPD_Icon_SetEnable(MMP_UBYTE ubIconID, MMP_BOOL bEnable)
{
    MMPD_System_EnableClock(MMPD_SYS_CLK_ICON, MMP_TRUE);
 
 	MMPF_Icon_SetEnable(ubIconID, bEnable);
    
    MMPD_System_EnableClock(MMPD_SYS_CLK_ICON, MMP_FALSE);

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Icon_LoadIndexColorTable
//  Description :
//------------------------------------------------------------------------------
MMP_ERR MMPD_Icon_LoadIndexColorTable(MMP_UBYTE ubIconID, MMP_ICON_COLOR ubColor, 
									  MMP_USHORT* pLUT, MMP_USHORT usColorNum)
{
    MMPD_System_EnableClock(MMPD_SYS_CLK_ICON, MMP_TRUE);
 
 	MMPF_Icon_LoadIndexColorTable(ubIconID, ubColor, 
 								  pLUT, usColorNum);
    
    MMPD_System_EnableClock(MMPD_SYS_CLK_ICON, MMP_FALSE);

    return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Icon_SetDLAttributes
//  Description : 
//------------------------------------------------------------------------------
MMP_ERR MMPD_Icon_SetDLAttributes(MMP_ICO_PIPEID pipeID, MMP_ICO_PIPE_ATTR *pPipeAttr)
{
    MMPD_System_EnableClock(MMPD_SYS_CLK_ICON, MMP_TRUE);

	MMPF_Icon_SetDLAttributes(pipeID, pPipeAttr);
    
    MMPD_System_EnableClock(MMPD_SYS_CLK_ICON, MMP_FALSE);

	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Icon_SetDLEnable
//  Description : 
//------------------------------------------------------------------------------
MMP_ERR MMPD_Icon_SetDLEnable(MMP_ICO_PIPEID pipeID, MMP_BOOL bEnable)
{
    MMPD_System_EnableClock(MMPD_SYS_CLK_ICON, MMP_TRUE);

    MMPF_Icon_SetDLEnable(pipeID, bEnable);
    
    MMPD_System_EnableClock(MMPD_SYS_CLK_ICON, MMP_FALSE);

	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : MMPD_Icon_BackupDLSetting
//  Description : 
//------------------------------------------------------------------------------
MMP_ERR MMPD_Icon_BackupDLSetting(MMP_ICO_PIPEID pipeID)
{
    MMPD_System_EnableClock(MMPD_SYS_CLK_ICON, MMP_TRUE);

	MMPF_Icon_BackupDLSetting(pipeID, &m_iconsetting[pipeID]);
    
    MMPD_System_EnableClock(MMPD_SYS_CLK_ICON, MMP_FALSE);

	return MMP_ERR_NONE;
}
//------------------------------------------------------------------------------
//  Function    : MMPD_Icon_RestoreDLSetting
//  Description : 
//------------------------------------------------------------------------------
MMP_ERR MMPD_Icon_RestoreDLSetting(MMP_ICO_PIPEID pipeID)
{
    MMPD_System_EnableClock(MMPD_SYS_CLK_ICON, MMP_TRUE);

	MMPF_Icon_RestoreDLSetting(pipeID, &m_iconsetting[pipeID]);
    
    MMPD_System_EnableClock(MMPD_SYS_CLK_ICON, MMP_FALSE);

	return MMP_ERR_NONE;
}
//------------------------------------------------------------------------------
//  Function    : MMPD_Icon_InitLinkSrc
//  Description : 
//------------------------------------------------------------------------------
MMP_ERR MMPD_Icon_InitLinkSrc(void)
{
    MMPD_System_EnableClock(MMPD_SYS_CLK_ICON, MMP_TRUE);

	MMPF_Icon_InitLinkSrc();
    
    MMPD_System_EnableClock(MMPD_SYS_CLK_ICON, MMP_FALSE);

	return MMP_ERR_NONE;
}

/// @}
/// @end_ait_only


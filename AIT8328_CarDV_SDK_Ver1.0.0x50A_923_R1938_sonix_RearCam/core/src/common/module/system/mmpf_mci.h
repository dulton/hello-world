/// @ait_only
//==============================================================================
//
//  File        : mmpf_mci.h
//  Description : INCLUDE File for the MCI Control Driver.
//  Author      : Eroy Yang
//  Revision    : 1.0
//
//==============================================================================

/**
 *  @file mmpf_mci.h
 *  @brief The header File for the MCI Control Driver
 *  @author Eroy Yang
 *  @version 1.0
 */

#ifndef _MMPF_MCI_H_
#define _MMPF_MCI_H_

//==============================================================================
//
//                              MACRO DEFINE
//
//==============================================================================

#define MCI_MODE_MASK       	(0x000F)
#define MCI_PIPE_MASK       	(0x00F0)
#define MCI_RAWS_MASK       	(0x0F00)
#define MCI_2NDPIPE_MASK    	(0xF000)

#define MCI_SET_MODE(m)    	 	(m)
#define MCI_SET_PIPE(p)     	((p) << 4)
#define MCI_SET_RAWS(s)     	((s) << 8)
#define MCI_SET_2NDPIPE(p)		((p) << 12)
#define MCI_GET_MODE(_mode) 	(_mode & MCI_MODE_MASK)
#define MCI_GET_PIPE(_mode) 	((_mode & MCI_PIPE_MASK) >> 4)
#define MCI_GET_RAWS(_mode) 	((_mode & MCI_RAWS_MASK) >> 8)
#define MCI_GET_2NDPIPE(_mode) 	((_mode & MCI_2NDPIPE_MASK) >> 12)

//==============================================================================
//
//                              ENUMERATION
//
//==============================================================================

/* Now possible MCI modes for different applications */
typedef enum _MMPF_MCI_TUNE_MODE {
    MCI_MODE_DEFAULT                = MCI_SET_MODE(1),
    MCI_MODE_DMAR_H264              = MCI_SET_MODE(2),
    MCI_MODE_DMAR_H264_P0           = MCI_SET_MODE(2)|MCI_SET_PIPE(0),
    MCI_MODE_DMAR_H264_P1           = MCI_SET_MODE(2)|MCI_SET_PIPE(1),
    MCI_MODE_DMAR_H264_P2           = MCI_SET_MODE(2)|MCI_SET_PIPE(2),
    MCI_MODE_DMAR_H264_P3           = MCI_SET_MODE(2)|MCI_SET_PIPE(3),    
    MCI_MODE_RAW_DVI                = MCI_SET_MODE(3),
    MCI_MODE_RAWS0_DVI_P0           = MCI_SET_MODE(3)|MCI_SET_PIPE(0),
    MCI_MODE_RAWS0_DVI_P1           = MCI_SET_MODE(3)|MCI_SET_PIPE(1),
    MCI_MODE_RAWS0_DVI_P2           = MCI_SET_MODE(3)|MCI_SET_PIPE(2),
    MCI_MODE_RAWS0_DVI_P3           = MCI_SET_MODE(3)|MCI_SET_PIPE(3),    
    MCI_MODE_RAWS1_DVI_P0           = MCI_SET_MODE(3)|MCI_SET_RAWS(1)|MCI_SET_PIPE(0),
    MCI_MODE_RAWS1_DVI_P1           = MCI_SET_MODE(3)|MCI_SET_RAWS(1)|MCI_SET_PIPE(1),
    MCI_MODE_GRA_LDC_H264           = MCI_SET_MODE(4),
    MCI_MODE_GRA_LDC_SRC_P0         = MCI_SET_MODE(4)|MCI_SET_PIPE(0),
    MCI_MODE_GRA_LDC_SRC_P1         = MCI_SET_MODE(4)|MCI_SET_PIPE(1),
    MCI_MODE_GRA_LDC_SRC_P2         = MCI_SET_MODE(4)|MCI_SET_PIPE(2),
    MCI_MODE_GRA_LDC_SRC_P3         = MCI_SET_MODE(4)|MCI_SET_PIPE(3),
    MCI_MODE_GRA_LDC_P0P2_H264_P1   = MCI_SET_MODE(4)|MCI_SET_PIPE(0)|MCI_SET_2NDPIPE(2),
    MCI_MODE_GRA_LDC_P3P1_H264_P0   = MCI_SET_MODE(4)|MCI_SET_PIPE(3)|MCI_SET_2NDPIPE(1),
    MCI_MODE_GRA_DECODE_H264        = MCI_SET_MODE(5),
    MCI_MODE_INVALID        		= MCI_SET_MODE(MCI_MODE_MASK)
} MMPF_MCI_TUNE_MODE;

typedef enum _MMPF_MCI_SRC {
    MMPF_MCI_SRC_CPUA_I = 0,
    MMPF_MCI_SRC_CPUA_D,
    MMPF_MCI_SRC_CPUB_I,
    MMPF_MCI_SRC_CPUB_D,
    MMPF_MCI_SRC_LCD1_IBC3,
    MMPF_MCI_SRC_LCD2_CCIR1_IBC4,
    MMPF_MCI_SRC_LCD3_CCIR2,
    MMPF_MCI_SRC_IBC0,
    MMPF_MCI_SRC_IBC1,
    MMPF_MCI_SRC_IBC2,
    MMPF_MCI_SRC_RAWS1,
    MMPF_MCI_SRC_RAWS2_JPGLB,
    MMPF_MCI_SRC_USB1_UART_SM_COLO,
    MMPF_MCI_SRC_USB2_ROM_PSPI,
    MMPF_MCI_SRC_H264_1,
    MMPF_MCI_SRC_H264_2,
    MMPF_MCI_SRC_RAW_F,
    MMPF_MCI_SRC_GRA,
    MMPF_MCI_SRC_DMAM0_SD0_JPGDEC,
    MMPF_MCI_SRC_DMAM1_SD1_JPGCB,
    MMPF_MCI_SRC_DMAR0_SD2,
    MMPF_MCI_SRC_DMAR1_ICON_SIF,
    MMPF_MCI_SRC_LDC_I2CM_HOST_DRAMBIST,
    MMPF_MCI_SRC_NUM
} MMPF_MCI_SRC;

typedef enum _MMPF_MCI_BYTECNT_SEL {
    MMPF_MCI_BYTECNT_SEL_128BYTE = 0,
    MMPF_MCI_BYTECNT_SEL_256BYTE,
    MMPF_MCI_BYTECNT_SEL_NUM
} MMPF_MCI_BYTECNT_SEL;

//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================

MMP_ERR MMPF_MCI_SetQueueDepth(MMP_USHORT usQdepth);
MMP_ERR MMPF_MCI_SetUrgentEnable(MMP_UBYTE ubSrc, MMP_BOOL ubEn);
void    MMPF_MCI_SetIBCMaxPriority(MMP_UBYTE ubPipe);
void 	MMPF_MCI_SetIBCSecondPriority(MMP_UBYTE ubPipe);
void    MMPF_MCI_SetModuleMaxPriority(MMP_UBYTE ubSrc);
MMP_ERR MMPF_MCI_TunePriority(MMPF_MCI_TUNE_MODE mode);

#endif //_MMPF_MCI_H_
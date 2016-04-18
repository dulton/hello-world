#ifndef _MMPD_RAWPROC_H_
#define _MMPD_RAWPROC_H_

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================

#include "mmp_rawproc_inc.h" 

//==============================================================================
//
//                              FUNCTION PROTOTYPE
//
//==============================================================================

MMP_ERR MMPD_RAWPROC_EnablePath(MMP_UBYTE ubSnrSel, MMP_UBYTE ubRawId, MMP_BOOL bEnable, MMP_RAW_IOPATH pathSelect, MMP_RAW_COLORFMT colorFormat);
MMP_ERR MMPD_RAWPROC_SetStoreBuf(MMP_UBYTE ubRawId, MMP_RAW_STORE_BUF *rawbuf);
MMP_ERR MMPD_RAWPROC_EnablePreview(MMP_UBYTE ubRawId, MMP_BOOL bEnable);
MMP_ERR MMPD_RAWPROC_SetStoreOnly(MMP_UBYTE ubRawId, MMP_BOOL bEnable);
MMP_ERR MMPD_RAWPROC_GetStoreRange(MMP_UBYTE ubVifId, MMP_USHORT *usWidth, MMP_USHORT *usHeight);
MMP_ERR MMPD_RAWPROC_SetFetchRange (MMP_USHORT usLeft,
						            MMP_USHORT usTop,
						            MMP_USHORT usWidth,
						            MMP_USHORT usHeight,
						            MMP_USHORT usLineOffset);
MMP_ERR MMPD_RAWPROC_CalcBufSize(MMP_RAW_COLORFMT colorFormat, MMP_USHORT usWidth, MMP_USHORT usHeight, MMP_ULONG* ulSize);
MMP_ERR MMPD_RAWPROC_SetDeInterlaceBuf(MMP_ULONG ulYAddr, MMP_ULONG ulUAddr, MMP_ULONG ulVAddr, MMP_ULONG ulBufCnt);
MMP_ERR MMPD_RAWPROC_SetRawStoreBufferEnd(MMP_UBYTE ubRawId, MMP_UBYTE ubPlane,
										  MMP_UBYTE ubBufIdx, MMP_ULONG ulAddr);
MMP_ERR MMPD_RAWPROC_EnableRingStore(MMP_UBYTE ubRawId, MMP_UBYTE ubPlane, MMP_BOOL bRingBufEn);

/* HDR Function */
MMP_ERR MMPD_HDR_InitModule(MMP_USHORT usSubW, MMP_USHORT usSubH);
MMP_ERR MMPD_HDR_UnInitModule(MMP_UBYTE ubSnrSel);
MMP_ERR MMPD_HDR_SetBufEnd(MMP_ULONG ulEndAddr);

#endif //_MMPD_RAWPROC_H_
/// @ait_only
//==============================================================================
//
//  File        : mmpd_exif.h
//  Description : INCLUDE File for the Host EXIF Driver.
//  Author      : Rogers Chen
//  Revision    : 1.0
//
//==============================================================================

/**
 *  @file mmpd_exif.h
 *  @brief The header File for the Host EXIF functions
 *  @author Rogers Chen
 *  @version 1.0
 */

#ifndef _MMPD_EXIF_H_
#define _MMPD_EXIF_H_

#include "mmp_lib.h"

#define EXIF_IFD_MAX 5
#define EXIF_TAG_MAX 15

#define EXIF_SOI			0xFFD8
#define EXIF_EOI			0xFFD9
#define EXIF_APP0_MARKER    0xFFE0 
#define EXIF_APP1_MARKER	0xFFE1
#define EXIF_ID				0x45786966

#define TIF_II_FORMAT 0x4949 
#define TIF_MM_FORMAT 0x4d4d

typedef struct _MMPD_EXIF_TIFHEADER
{
    MMP_USHORT 	usByteOrder;
    MMP_USHORT 	usFixed002a;
    MMP_ULONG 	ulIfd0Offset;
} MMPD_EXIF_TIFHEADER;


typedef struct _MMPD_EXIF_TIFTAG
{
    MMP_USHORT 	usTag;
    MMP_USHORT 	usType;
    MMP_ULONG 	ulCount; // 1 count : 2bytes
    MMP_ULONG 	ulValOffset;
} MMPD_EXIF_TIFTAG;

typedef struct _MMPD_EXIF_TIFIFD
{
    MMP_USHORT 	usFields;
    MMP_ULONG 	ulNextIfd;
    MMPD_EXIF_TIFTAG 	*Tag[EXIF_TAG_MAX];
} MMPD_EXIF_TIFIFD;

typedef struct _MMPD_EXIF_HEADER {
	MMP_USHORT usSOI;
	MMP_USHORT usApp1Marker;
	MMP_USHORT usApp1Len;
	MMP_ULONG  ulExifID;
	MMP_USHORT usPad;
} MMPD_EXIF_HEADER;

typedef struct _MMPD_EXIF_DATA {
	MMPD_EXIF_HEADER 	ExifHeader;
	MMPD_EXIF_TIFHEADER TifHeader;
	MMPD_EXIF_TIFIFD 	*Ifd[EXIF_IFD_MAX];
	MMP_LONG  			lIfdNum;
	MMP_LONG  			lExifIfdFound;
	MMP_LONG  			lExifIfdOffset;
	MMP_ULONG 			ulTifHeaderOffset;
	MMP_UBYTE 			*ubData;
	MMP_ULONG 			ulThumbnailLen;
	MMP_UBYTE 			*ubThumbnailData;
} MMPD_EXIF_DATA;

typedef struct _MMPD_EXIF_CONTEXT {
	MMP_UBYTE  	*ubBuf;
	MMP_ULONG  	ulExifHeaderSize;
	MMP_ULONG  	ulExifHeaderOffset;
	MMP_ULONG  	ulPrimarySize;
	MMP_ULONG  	ulPrimaryOffset;
	MMP_ULONG  	ulThumbnailSize;
	MMP_ULONG  	ulThumbnailOffset;
	MMP_ULONG  	ulRgb16Size;
	MMP_ULONG  	ulRgb16Offset;
	MMP_ULONG  	ulValidSize;
    MMP_USHORT 	usPrimaryW;
    MMP_USHORT 	usPrimaryH;
    MMP_USHORT 	usThumbnailW;
    MMP_USHORT 	usThumbnailH;
	MMP_USHORT 	usOrientation;
	MMP_USHORT 	usPad; // for compression
	MMP_UBYTE 	*usImgDesc;
#ifdef USING_AIT_FS
    sTIME 		CreateTime;
#endif  	
} MMPD_EXIF_CONTEXT;


void MMPD_EXIF_Init(void);
MMP_ERR  MMPD_EXIF_Encode(MMPD_EXIF_CONTEXT *ExifCtx);
MMP_ERR  MMPD_EXIF_Decode(MMPD_EXIF_CONTEXT *ExifCtx, MMP_UBYTE *ubJpegBuf, MMP_ULONG ulJpegSize);

/// @}
#endif // _INCLUDES_H_
/// @end_ait_only


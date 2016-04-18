//==============================================================================
//
//  File        : mmp_graphics_inc.h
//  Description : INCLUDE File for the Firmware Graphic Driver.
//  Author      : Ben Lu
//  Revision    : 1.0
//
//==============================================================================

#ifndef _MMP_GRAPHICS_INC_H_
#define _MMP_GRAPHICS_INC_H_

//==============================================================================
//
//                              MACRO DEFINE
//
//==============================================================================

#define GRAPHICS_SEM_TIMEOUT	(0x10000)//(0x100)
#define GRAPHICS_WHILE_TIMEOUT	(0x5000000)//(0x100000)

//==============================================================================
//
//                              ENUMERATION
//
//==============================================================================

typedef enum _MMP_GRAPHICS_COLORDEPTH
{
    MMP_GRAPHICS_COLORDEPTH_8 		            = 1,
    MMP_GRAPHICS_COLORDEPTH_16 		            = 2,
    MMP_GRAPHICS_COLORDEPTH_24 		            = 3,
    MMP_GRAPHICS_COLORDEPTH_32 		            = 4,
    MMP_GRAPHICS_COLORDEPTH_YUV422_UYVY 	    = 5,
    MMP_GRAPHICS_COLORDEPTH_YUV422_VYUY         = 6,
    MMP_GRAPHICS_COLORDEPTH_YUV422_YUYV         = 7,
    MMP_GRAPHICS_COLORDEPTH_YUV422_YVYU         = 8,
	MMP_GRAPHICS_COLORDEPTH_YUV420 	            = 9,
	MMP_GRAPHICS_COLORDEPTH_YUV420_INTERLEAVE   = 10,
    MMP_GRAPHICS_COLORDEPTH_ARGB3454            = 11,
    MMP_GRAPHICS_COLORDEPTH_ARGB4444            = 12,
    MMP_GRAPHICS_COLORDEPTH_UNSUPPORT
} MMP_GRAPHICS_COLORDEPTH;

typedef enum _MMP_GRAPHICS_ROTATE_TYPE
{
    MMP_GRAPHICS_ROTATE_NO_ROTATE = 0,
    MMP_GRAPHICS_ROTATE_RIGHT_90,
    MMP_GRAPHICS_ROTATE_RIGHT_180,
    MMP_GRAPHICS_ROTATE_RIGHT_270
} MMP_GRAPHICS_ROTATE_TYPE;

typedef enum _MMP_GRAPHICS_KEYCOLOR
{
    MMP_GRAPHICS_FG_COLOR = 0,
    MMP_GRAPHICS_BG_COLOR
} MMP_GRAPHICS_KEYCOLOR;

typedef enum _MMP_GRAPHICS_RECTFILLTYPE 
{
    MMP_GRAPHICS_SOLID_FILL = 0,
    MMP_GRAPHICS_LINE_FILL
} MMP_GRAPHICS_RECTFILLTYPE;

typedef enum _MMP_GRAPHICS_DELAY_TYPE
{
    MMP_GRAPHICS_DELAY_CHK_SCA_BUSY = 0,
    MMP_GRAPHICS_DELAY_CHK_LINE_END
} MMP_GRAPHICS_DELAY_TYPE;

typedef enum _MMP_GRAPHICS_ROP
{
    MMP_GRAPHICS_ROP_BLACKNESS     = 0,
    MMP_GRAPHICS_ROP_NOTSRCERASE   = 0x01,	// ~(S+D)
    MMP_GRAPHICS_ROP_NOTSRCCOPY    = 0x03,	// ~S
    MMP_GRAPHICS_ROP_SRCERASE      = 0x04,	// S.~D
    MMP_GRAPHICS_ROP_DSTINVERT     = 0x05,	// ~D
    MMP_GRAPHICS_ROP_SRCINVERT     = 0x06,	// S^D
    MMP_GRAPHICS_ROP_SRCAND        = 0x08,	// S.D
    MMP_GRAPHICS_ROP_MERGEPAINT    = 0x0B,	// ~S+D
    MMP_GRAPHICS_ROP_SRCCOPY       = 0x0C,	// S
    MMP_GRAPHICS_ROP_SRCPAINT      = 0x0E,	// S+D
    MMP_GRAPHICS_ROP_WHITENESS     = 0x0F
} MMP_GRAPHICS_ROP;

//==============================================================================
//
//                              STRUCTURES
//
//==============================================================================

typedef struct _MMP_GRAPHICS_BUF_ATTR {
    MMP_USHORT                      usWidth;
    MMP_USHORT                      usHeight;
    MMP_USHORT                      usLineOffset;
    MMP_GRAPHICS_COLORDEPTH        	colordepth;
	MMP_ULONG                       ulBaseAddr;
    MMP_ULONG                       ulBaseUAddr;
    MMP_ULONG                       ulBaseVAddr;
} MMP_GRAPHICS_BUF_ATTR;

typedef struct _MMP_GRAPHICS_RECT {
    MMP_USHORT          			usLeft;
    MMP_USHORT          			usTop;
    MMP_USHORT          			usWidth;
    MMP_USHORT          			usHeight;
} MMP_GRAPHICS_RECT;

typedef struct _MMP_GRAPHICS_DRAWRECT_ATTR {
    MMP_GRAPHICS_RECTFILLTYPE 		type;
    MMP_BOOL            			bUseRect;
    MMP_USHORT          			usWidth;        // Buffer Width
    MMP_USHORT          			usHeight;       // Buffer Height
    MMP_USHORT          			usLineOfst;     // Buffer LineOffset
	MMP_ULONG           			ulBaseAddr;     // Buffer Base Address
    MMP_GRAPHICS_COLORDEPTH 		colordepth;
    MMP_ULONG           			ulColor;
    MMP_ULONG           			ulPenSize;
    MMP_GRAPHICS_ROP   				ropcode;
} MMP_GRAPHICS_DRAWRECT_ATTR;

typedef void GraphicScaleCallBackFunc(void);

#endif // _MMP_GRAPHICS_INC_H_

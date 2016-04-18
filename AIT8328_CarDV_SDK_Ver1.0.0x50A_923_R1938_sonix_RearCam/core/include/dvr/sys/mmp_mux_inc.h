//------------------------------------------------------------------------------
//
//  File        : mmp_mux_inc.h
//  Description : Header file of 3GP mux configuration
//  Author      : Alterman
//  Revision    : 1.0
//
//------------------------------------------------------------------------------

#ifndef _MMP_MUX_INC_H_
#define _MMP_MUX_INC_H_

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================

//==============================================================================
//
//                              ENUMERATION
//
//==============================================================================

// Check compress buffer stage
typedef enum _MMP_3GPRECD_COMPBUF_FREESIZE_STAGE {
	MMP_3GPRECD_COMPBUF_LESS_1MB = 0,
	MMP_3GPRECD_COMPBUF_LESS_2MB,
	MMP_3GPRECD_COMPBUF_LESS_3MB,
	MMP_3GPRECD_COMPBUF_LESS_4MB,
	MMP_3GPRECD_COMPBUF_LESS_5MB,
	MMP_3GPRECD_COMPBUF_SAFE
} MMP_3GPRECD_COMPBUF_FREESIZE_STAGE;

// Video encode using mode
typedef enum _MMP_VIDRECD_USEMODE {
	MMP_VIDRECD_USEMODE_RECD = 0,  // Video record file
	MMP_VIDRECD_USEMODE_CB2AP, 
	MMP_VIDRECD_USEMODE_MAX
} MMP_VIDRECD_USEMODE;

// File type for video record
typedef enum _MMP_3GPRECD_FILETYPE {
	MMP_3GPRECD_FILETYPE_VIDRECD = 0,  // Video record file
	MMP_3GPRECD_FILETYPE_EMERGENCY,    // Emergency file
	MMP_3GPRECD_FILETYPE_UVCRECD,      // UVC stream file
	MMP_3GPRECD_FILETYPE_REFIXRECD,
	MMP_3GPRECD_FILETYPE_UVCEMERG,     // UVC emergent record
	MMP_3GPRECD_FILETYPE_DUALENC,      // Dual recording
	MMP_3GPRECD_FILETYPE_WIFIFRONT,	   // Wifi front stream
	MMP_3GPRECD_FILETYPE_WIFIREAR,	   // Wifi rear stream
	MMP_3GPRECD_FILETYPE_MAX
} MMP_3GPRECD_FILETYPE;

//==============================================================================
//
//                              STRUCTURE
//
//==============================================================================

// Frame rate representation
typedef struct MMP_VID_FPS {
    MMP_ULONG   ulResol;
    MMP_ULONG   ulIncr;
    MMP_ULONG   ulIncrx1000;
} MMP_VID_FPS;

#endif // _MMP_MUX_INC_H_


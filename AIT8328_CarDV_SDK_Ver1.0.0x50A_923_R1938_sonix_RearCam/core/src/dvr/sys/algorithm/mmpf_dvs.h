#ifndef _MMPF_DVS_H_
#define _MMPF_DVS_H_

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================

#include "includes_fw.h"

//==============================================================================
//
//                              STRUCTURE
//
//==============================================================================

typedef struct _MMPF_DVS_CONFIG {
	MMP_SSHORT height;              // input frame height
	MMP_SSHORT width;				// input frame width

	MMP_SSHORT level1;				// 1st order MV weight
	MMP_SSHORT level2;				// 2nd order MV weight
	MMP_SSHORT level3;				// 3rd order MV weight

	MMP_SSHORT unit_width;			// width should < unit_width after scale
	MMP_SSHORT shift_range;			// grab range for x and y direction
	MMP_SLONG compensate_scale;		// scale for MV

	MMP_SLONG TH_gmv_reliability;	// not do ME if reliability < this value
	MMP_SLONG TH_NonSmooth;			// smooth block if Avg. SAD < this value
	MMP_SLONG TH_refine_region;		// decrease global_x/y every n frames
	MMP_SLONG refine_region_conut;	// region count
	MMP_SLONG TH_non_smooth_block_count; // 

    // global variables =================================================
	MMP_SSHORT scale;				// scale for ME

	MMP_SSHORT Y_width;             // Y_width  = width  / scale;
	MMP_SSHORT Y_height;            // Y_height  = height  / scale;

	MMP_SSHORT X_Bnum;				// X_Bnum = Y_width / 16;
	MMP_SSHORT Y_Bnum;				// Y_Bnum = Y_height / 16;

	MMP_SSHORT window_width;		// output frame width		// be always fixed
	MMP_SSHORT window_height;		// output frame height		// be always fixed

	MMP_SSHORT X_point;				// X_point = shift_range	// be always fixed
	MMP_SSHORT Y_point;				// Y_point = shift_range	// be always fixed

	MMP_SSHORT global_X;			// compensation offset to X_point
	MMP_SSHORT global_Y;			// compensation offset to Y_point

	MMP_SBYTE *vector_X;			// MV x for every block in current frame
	MMP_SBYTE *vector_Y;			// MV y for every block in current frame
	MMP_SBYTE *vector_X_p;			// previous MV x for every block in current frame
	MMP_SBYTE *vector_Y_p;			// previous MV y for every block in current frame

	MMP_SSHORT gmv_x;				// GMV buffer // for dbg
	MMP_SSHORT gmv_y;				// GMV buffer // for dbg

	// local variables =================================================
	MMP_SSHORT (*histogram)[64];    // histogram of block motion vectors

	MMP_SSHORT X_start;				// current block left
	MMP_SSHORT Y_start;				// current block top

	MMP_SLONG histogram_count;
	MMP_SLONG gmv_reliability;
	MMP_SSHORT non_smooth_block_count;	// non-smooth block count in a frame

	MMP_SSHORT compensate_X;
	MMP_SSHORT compensate_Y;

    MMP_USHORT  downsample_ratio;
    MMP_ULONG   buffer_cache_start;
    MMP_ULONG   buffer_cache_size;
} MMPF_DVS_CONFIG;

//==============================================================================
//
//                              FUNCTION PROTOTYPE
//
//==============================================================================

MMP_ERR MMPF_DVS_Initialize(MMP_USHORT usWidth, MMP_USHORT usHeight, MMP_ULONG ulHWMVBufAddr,
            				MMP_ULONG ulPageStart, MMP_ULONG PageSizeSum, MMP_USHORT usGrabRange,
            				MMP_USHORT usDownsample, MMP_USHORT usMVUnitSize);
MMP_ERR MMPF_DVS_GetGrabLocation(MMP_USHORT *grab_x, MMP_USHORT *grab_y);
MMP_ERR MMPF_DVS_GetGrabCurOrigin(MMP_USHORT *grab_x, MMP_USHORT *grab_y);
MMP_ERR MMPF_DVS_ConfigDVSWorkingBuffer(MMP_BOOL bCachable);
MMP_ERR MMPF_DVS_TriggerDVS(void);

#endif
//==============================================================================
//
//  File        : mmp_bayerscal_inc.h
//  Description : INCLUDE File for the Firmware Bayer Scaler Control driver function
//  Author      : Eroy Yang
//  Revision    : 1.0
//
//==============================================================================

#ifndef _MMP_BAYERSCAL_INC_H_
#define _MMP_BAYERSCAL_INC_H_

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================

#include "mmp_scal_inc.h"

//==============================================================================
//
//                              ENUMERATION
//
//==============================================================================

typedef enum _MMP_BAYER_SCALER_MODE
{
    MMP_BAYER_SCAL_BYPASS = 0,
    MMP_BAYER_SCAL_DOWN,
    MMP_BAYER_SCAL_RAW_FETCH,
    MMP_BAYER_SCAL_MAX_MODE
} MMP_BAYER_SCALER_MODE;

//==============================================================================
//
//                              STRUCTURE
//
//==============================================================================

typedef struct _MMP_BAYER_SCALER_INFO 
{
    MMP_SCAL_FIT_RANGE sFitRange;
    MMP_SCAL_GRAB_CTRL sGrabCtl;
} MMP_BAYER_SCALER_INFO;

#endif // _MMP_BAYERSCAL_INC_H_

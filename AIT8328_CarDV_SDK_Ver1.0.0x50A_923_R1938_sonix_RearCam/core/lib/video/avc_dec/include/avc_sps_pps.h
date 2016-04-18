#ifndef _AVC_SPS_PPS_H_
#define _AVC_SPS_PPS_H_

#include "mmpf_typedef.h"
#include "avc_api.h"

#define MAX_SPS_NUM 1   //32
#define MAX_PPS_NUM 1   //256

MMP_LONG    decode_H264_init_header(AVC_DATA_BUF *buf);
MMP_ULONG	nalu_to_rbsp(MMP_UBYTE *buf, MMP_LONG len);

#endif
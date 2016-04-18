/*
 *  Copyright (c) 2006 Alpha Imaging Technology Corp. All Rights Reserved
 *
 */

#ifndef _AVC_UTILS_H_
#define _AVC_UTILS_H_

#include "data_type.h"

typedef void (mc_func)(uint8_t *src,uint8_t *dest,const int32_t image_width,int32_t dest_width);
typedef mc_func* mc_func_ptr;

typedef void (intrapred_func)(uint8_t *data, const int32_t width);
typedef intrapred_func* intrapred_func_ptr;

typedef void (colortrans_func)(uint8_t *output, uint8_t *y, uint8_t *u, uint8_t *v,
					   int32_t y_width, int32_t uv_width, int32_t out_width, int32_t out_height);
typedef colortrans_func* color_func_ptr;

#endif
/*
 *  Copyright (c) 2006 Alpha Imaging Technology Corp. All Rights Reserved
 *
 */

#ifndef _AVC_BS_H_
#define _AVC_BS_H_

#include "data_type.h"

typedef struct
{
    uint32_t *tail;
	uint32_t *start;
    uint32_t bufA;
    uint32_t bufB;
    uint32_t initpos;
    uint32_t buf_length;
	uint32_t bitpos;				///< to record the bit position in the register
    uint32_t bits_count;
    uint32_t eof;
    uint32_t golomb_zeros;
} BS;

void        avc_bs_init(BS *dec, void *bs, int32_t bs_len);
uint32_t    avc_bs_pos(BS *dec);
uint32_t    avc_bs_eof(BS *dec);
uint32_t    avc_bs_show(BS *dec, int32_t nbits);
void        avc_bs_skip(BS *dec, int32_t nbits);
uint32_t    avc_bs_read(BS *dec, int32_t nbits );
uint32_t    avc_bs_read1(BS *dec);
void        avc_bs_align(BS *dec);
int         avc_bs_read_ue(BS *dec);
int         avc_bs_read_se(BS *dec);
int         avc_bs_read_te(BS *dec, int32_t x);

#endif
#ifndef _ov_interface_h_
#define _ov_interface_h_

#include "includes_fw.h"
#if (OGG_EN)

#define AIT_PLATFORM    1

#include "ivorbiscodec.h"
#include "codec_internal.h"
#include "ivorbisfile.h"


//#define HEADER_BUF_SIZE         1024*30
//#define OGG_TEMP_RAM_SIZE       1024*14
#define HEADER_BUF_SIZE         1024*38
#define OGG_TEMP_RAM_SIZE       1024*8
#define MALLOC_BUF_SIZE         1024*6
//2008-08-12 Eric add
#define BITSTREAM_BUF_SIZE      1024*16

extern void *header_buffer_alloc(int size);
extern void *temp_ram_alloc(int size);
extern void *bitstream_buf_alloc(int size);
extern void *malloc_buffer_alloc(int size);
extern long FillReadBuffer(unsigned char *readBuf, int read_size);
extern long _get_data(OggVorbis_File *vf);
extern void _seek_helper(OggVorbis_File *vf,ogg_int64_t offset);
extern void InitSWStack(void);
extern int OGGDec_SetWorkingBuf(int *buf);
extern int OGGDec_SetWorkingBuf_NonCacheable(int *buf);
#endif
#endif
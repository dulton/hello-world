
#ifndef _OV_FILE_H_
#define _OV_FILE_H_
#include "includes_fw.h"
#if (OGG_EN)

#include "ivorbiscodec.h"
#include "ogg_interface.h"

#ifndef AIT_PLATFORM   
#include <stdio.h>
#include "mmp_err.h"
#else
#include "includes_fw.h"

#endif

#define  NOTOPEN   0
#define  PARTOPEN  1
#define  OPENED    2
#define  STREAMSET 3 /* serialno and link set, but not to current link */
#define  LINKSET   4 /* serialno and link set to current link */
#define  INITSET   5

#ifndef AIT_PLATFORM   
// For AIT platform simulation
typedef unsigned char   MMP_BOOL;
typedef char            MMP_BYTE;
typedef short           MMP_SHORT;
typedef int             MMP_LONG;
typedef unsigned char   MMP_UBYTE;
typedef unsigned short  MMP_USHORT;
typedef unsigned int    MMP_ULONG;
typedef unsigned long   MMP_U_LONG;
typedef unsigned long long  MMP_ULONG64;
typedef void*           MMP_HANDLE;

#define INPUT_BUF_SIZE      4096

//#define     DECODE_OP_START             (0x01)
//#define     DECODE_OP_PAUSE             (0x02)
//#define     DECODE_OP_RESUME            (0x03)
//#define     DECODE_OP_STOP              (0x04)

#endif

/* The function prototypes for the callbacks are basically the same as for
 * the stdio functions fread, fseek, fclose, ftell.
 * The one difference is that the FILE * arguments have been replaced with
 * a void * - this is to be used as a pointer to whatever internal data these
 * functions might need. In the stdio case, it's just a FILE * cast to a void *
 *
 * If you use other functions, check the docs for these functions and return
 * the right values. For seek_func(), you *MUST* return -1 if the stream is
 * unseekable
 */
#ifndef AIT_PLATFORM   
typedef struct {    
  size_t (*read_func)  (void *ptr, size_t size, size_t nmemb, void *datasource);
  int    (*seek_func)  (void *datasource, ogg_int64_t offset, int whence);
  int    (*close_func) (void *datasource);
  long   (*tell_func)  (void *datasource);
} ov_callbacks;
#else
typedef struct {    
  MMP_ERR (*read_func)  (MMP_ULONG ulFileID, void *pData, MMP_ULONG NumBytes, MMP_ULONG *read_count);
  MMP_ERR (*seek_func)  (MMP_ULONG ulFileID, MMP_LONG64 llOffset, MMP_LONG lOrigin);
  MMP_ERR (*close_func) (MMP_ULONG ulFileID);
  MMP_ERR (*tell_func)  (MMP_ULONG ulFileID, MMP_LONG64 *llFilePos);
} ov_callbacks;
#endif

typedef struct OggVorbis_File {
#ifndef AIT_PLATFORM     
  void            *datasource; /* Pointer to a FILE *, etc. */
#else
  unsigned long    datasource; /* File ID  */
#endif  
  int              seekable;
  ogg_int64_t      offset;
  ogg_int64_t      end;
  ogg_sync_state   *oy;

  /* If the FILE handle isn't seekable (eg, a pipe), only the current
     stream appears */
  int              links;
  ogg_int64_t     *offsets;
  ogg_int64_t     *dataoffsets;
  ogg_uint32_t    *serialnos;
  ogg_int64_t     *pcmlengths;
  vorbis_info     vi;
  vorbis_comment  vc;

  /* Decoding working state local storage */
  ogg_int64_t      pcm_offset;
  int              ready_state;
  ogg_uint32_t     current_serialno;
  int              current_link;

  ogg_int64_t      bittrack;
  ogg_int64_t      samptrack;

  ogg_stream_state *os; /* take physical pages, weld into a logical
                          stream of packets */
  vorbis_dsp_state *vd; /* central working state for the packet->PCM decoder */

  ov_callbacks callbacks;

} OggVorbis_File;

extern int OGGDec_clear(OggVorbis_File *vf);
#ifndef AIT_PLATFORM  
extern int OGGDec_initialize(void *f,OggVorbis_File *vf,char *initial,long ibytes);
extern int OGGDec_initialize_callbacks(void *datasource, OggVorbis_File *vf,
		char *initial, long ibytes, ov_callbacks callbacks);
#else
extern int OGGDec_initialize(unsigned long f,OggVorbis_File *vf,char *initial,long ibytes);
extern int OGGDec_initialize_callbacks(unsigned long datasource, OggVorbis_File *vf,
		char *initial, long ibytes, ov_callbacks callbacks);
#endif

extern int ov_raw_seek(OggVorbis_File *vf,ogg_int64_t pos);
extern int OGGDec_time_seek_page(OggVorbis_File *vf,ogg_int64_t pos);
extern ogg_int64_t OGGDec_time_tell(OggVorbis_File *vf);
extern vorbis_info *OGGDec_info(OggVorbis_File *vf,int link);
extern vorbis_comment *OGGDec_comment(OggVorbis_File *vf,int link);
extern int OGGDec_fetch_and_process_packet(OggVorbis_File *vf, int readp, int spanp);
extern ogg_int64_t ov_pcm_total(OggVorbis_File *vf,int i);

#ifndef AIT_PLATFORM   
//For AIT platform simulation
extern MMP_USHORT	MMPF_Audio_TransferDataFromCard(OggVorbis_File *vf);
#endif


int _fetch_headers(OggVorbis_File *vf,
			  vorbis_info *vi,
			  vorbis_comment *vc,
			  ogg_uint32_t *serialno,
			  ogg_page *og_ptr);
			  
ogg_int64_t _get_prev_page(OggVorbis_File *vf,ogg_page *og);			  

int _bisect_forward_serialno(OggVorbis_File *vf,
				    ogg_int64_t begin,
				    ogg_int64_t searched,
				    ogg_int64_t end,
				    ogg_uint32_t currentno,
				    long m);
				    
void _prefetch_all_offsets(OggVorbis_File *vf, ogg_int64_t dataoffset);				    
#endif
#endif



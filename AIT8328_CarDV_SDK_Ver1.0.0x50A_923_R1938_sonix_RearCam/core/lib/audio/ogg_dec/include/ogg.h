
#ifndef _OGG_H
#define _OGG_H
#include "includes_fw.h"
#if (OGG_EN)
//#include "os_types.h"
#include "mmpf_audio_typedef.h"

#define LIMIT_TO_64kHz  1

typedef struct ogg_buffer_state{
  struct ogg_buffer    *unused_buffers;
  struct ogg_reference *unused_references;
  int                   outstanding;
  int                   shutdown;
} ogg_buffer_state;

typedef struct ogg_buffer {
  unsigned char      *data;
  long                size;
  int                 refcount;

  union {
    ogg_buffer_state  *owner;
    struct ogg_buffer *next;
  } ptr;
} ogg_buffer;

typedef struct ogg_reference {
  ogg_buffer    *buffer;
  long           begin;
  long           length;

  struct ogg_reference *next;
} ogg_reference;

typedef struct oggpack_buffer {
  int            headbit;
  unsigned char *headptr;
  long           headend;

  /* memory management */
  ogg_reference *head;
  ogg_reference *tail;

  /* render the byte/bit counter API constant time */
  long              count; /* doesn't count the tail */
} oggpack_buffer;

typedef struct oggbyte_buffer {
  ogg_reference *baseref;

  ogg_reference *ref;
  unsigned char *ptr;
  long           pos;
  long           end;
} oggbyte_buffer;

typedef struct ogg_sync_state {
  /* decode memory management pool */
  ogg_buffer_state *bufferpool;

  /* stream buffers */
  ogg_reference    *fifo_head;
  ogg_reference    *fifo_tail;
  long              fifo_fill;

  /* stream sync management */
  int               unsynced;
  int               headerbytes;
  int               bodybytes;

} ogg_sync_state;

typedef struct ogg_stream_state {
  ogg_reference *header_head;
  ogg_reference *header_tail;
  ogg_reference *body_head;
  ogg_reference *body_tail;

  int            e_o_s;    /* set when we have buffered the last
                              packet in the logical bitstream */
  int            b_o_s;    /* set after we've written the initial page
                              of a logical bitstream */
  long           serialno;
  long           pageno;
  ogg_int64_t    packetno; /* sequence number for decode; the framing
                              knows where there's a hole in the data,
                              but we need coupling so that the codec
                              (which is in a seperate abstraction
                              layer) also knows about the gap */
  ogg_int64_t    granulepos;

  int            lacing_fill;
  ogg_uint32_t   body_fill;

  /* decode-side state data */
  int            holeflag;
  int            spanflag;
  int            clearflag;
  int            laceptr;
  ogg_uint32_t   body_fill_next;

} ogg_stream_state;

typedef struct {
  ogg_reference *packet;
  long           bytes;
  long           b_o_s;
  long           e_o_s;
  ogg_int64_t    granulepos;
  ogg_int64_t    packetno;     /* sequence number for decode; the framing
                                  knows where there's a hole in the data,
                                  but we need coupling so that the codec
                                  (which is in a seperate abstraction
                                  layer) also knows about the gap */
} ogg_packet;

typedef struct {
  ogg_reference *header;
  int            header_len;
  ogg_reference *body;
  long           body_len;
} ogg_page;



/* Ogg BITSTREAM PRIMITIVES: decoding **************************/
extern ogg_sync_state *ogg_sync_create(void);
extern int      ogg_sync_destroy(ogg_sync_state *oy);
extern int      ogg_sync_reset(ogg_sync_state *oy);
extern unsigned char *ogg_sync_bufferin(ogg_sync_state *oy, long size);
extern int      ogg_sync_wrote(ogg_sync_state *oy, long bytes);
extern long     ogg_sync_pageseek(ogg_sync_state *oy,ogg_page *og);
extern int      ogg_sync_pageout(ogg_sync_state *oy, ogg_page *og);
extern int      ogg_stream_pagein(ogg_stream_state *os, ogg_page *og);
extern int      ogg_stream_packetout(ogg_stream_state *os,ogg_packet *op);
extern int      ogg_stream_packetpeek(ogg_stream_state *os,ogg_packet *op);
extern ogg_stream_state *ogg_stream_create(int serialno);
extern int      ogg_stream_destroy(ogg_stream_state *os);
extern int      ogg_stream_reset(ogg_stream_state *os);
extern int      ogg_stream_reset_serialno(ogg_stream_state *os,int serialno);
extern int      ogg_sync_reset(ogg_sync_state *oy);


extern ogg_uint32_t ogg_page_serialno(ogg_page *og);

extern int      ogg_page_release(ogg_page *og);
extern void     ogg_reset_chunksize(void);
#endif 
#endif /* _OGG_H */
#ifndef __OGGFLAC_MEMORY_API_H__
#define __OGGFLAC_MEMORY_API_H__

#include "config_fw.h"

#define OGGFLAC_FRAMING_STATIC_ALLOC

#ifdef OGGFLAC_FRAMING_STATIC_ALLOC
//#define OGGFLAC_FRAMING_GRAN_OPEN
#endif

#if (FLAC_P_EN)
extern int* oggflac_framing_body_workingbuf;
extern int* oggflac_framing_lacing_workingbuf;
extern int* oggflac_framing_data_workingbuf;
#else
extern int oggflac_framing_body_workingbuf[];
extern int oggflac_framing_lacing_workingbuf[];
extern int oggflac_framing_data_workingbuf[];
#endif

extern unsigned int oggflac_framing_body_workingbuf_straddr;    
extern unsigned int oggflac_framing_lacing_workingbuf_straddr;  
extern unsigned int oggflac_framing_data_workingbuf_straddr; 

extern int oggflac_framing_body_workingbuf_used;
extern int oggflac_framing_lacing_workingbuf_used;
extern int oggflac_framing_data_workingbuf_used;

extern unsigned int oggflac_framing_body_workingbuf_size;    //byte
extern unsigned int oggflac_framing_lacing_workingbuf_size;  //byte
extern unsigned int oggflac_framing_data_workingbuf_size;  //byte

void oggflac_framing_body_workingbuf_init(void* BufStrAddr, unsigned int BufSize);
void oggflac_framing_lacing_workingbuf_init(void* BufStrAddr, unsigned int BufSize);
void oggflac_framing_data_workingbuf_init(void* BufStrAddr, unsigned int BufSize);

int oggflac_framing_workingbuf_init(void);


void *oggflac_body_malloc(unsigned int size);
void *oggflac_lacing_malloc(unsigned int size);
void *oggflac_data_malloc(unsigned int size);

void oggflac_body_free(void *ptr);
void oggflac_lacing_free(void *ptr);
void oggflac_data_free(void *ptr);

void *oggflac_body_realloc(void* ptr, unsigned int size);
void *oggflac_lacing_realloc(void* ptr, unsigned int size);
void *oggflac_data_realloc(void* ptr, unsigned int size);


#ifdef OGGFLAC_FRAMING_GRAN_OPEN
extern int oggflac_framing_granule_workingbuf[];
extern unsigned int oggflac_framing_granule_workingbuf_straddr; 
extern int oggflac_framing_granule_workingbuf_used;
extern unsigned int oggflac_framing_granule_workingbuf_size; //byte
void oggflac_framing_granule_workingbuf_init(void* BufStrAddr, unsigned int BufSize);
void *oggflac_granule_malloc(unsigned int size);
void oggflac_granule_free(void *ptr);
void *oggflac_granule_realloc(void* ptr, unsigned int size);
#endif


#endif //__OGGFLAC_MEMORY_API_H__


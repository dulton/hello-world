
#ifndef __BITS_H__
#define __BITS_H__

#include "aac_comm.h"

#define ARM

#define BYTE_NUMBIT 8
#define bit2byte(a) ((a+7)/BYTE_NUMBIT)

typedef struct _bitfile
{
    /* bit input */
    uint32_t bufa;
    uint32_t bufb;
    uint32_t bits_left;
    uint32_t buffer_size; /* size of the buffer in bytes */
    uint32_t bytes_used;
    uint8_t no_more_reading;
    uint8_t error;
    uint32_t *tail;
    uint32_t *start;
    void *buffer;
} bitfile;


#if defined (_WIN32) && !defined(_WIN32_WCE)
#define BSWAP(a) __asm mov eax,a __asm bswap eax __asm mov a, eax
#elif defined(LINUX) || defined(DJGPP)
#define BSWAP(a) __asm__ ( "bswapl %0\n" : "=r" (a) : "0" (a) )
#else
#define BSWAP(a) \
    ((a) = ( ((a)&0xff)<<24) | (((a)&0xff00)<<8) | (((a)>>8)&0xff00) | (((a)>>24)&0xff))
#endif

static uint32_t bitmask[] = {
    0x0, 0x1, 0x3, 0x7, 0xF, 0x1F, 0x3F, 0x7F, 0xFF, 0x1FF,
    0x3FF, 0x7FF, 0xFFF, 0x1FFF, 0x3FFF, 0x7FFF, 0xFFFF,
    0x1FFFF, 0x3FFFF, 0x7FFFF, 0xFFFFF, 0x1FFFFF, 0x3FFFFF,
    0x7FFFFF, 0xFFFFFF, 0x1FFFFFF, 0x3FFFFFF, 0x7FFFFFF,
    0xFFFFFFF, 0x1FFFFFFF, 0x3FFFFFFF, 0x7FFFFFFF
    /* added bitmask 32, correct?!?!?! */
    , 0xFFFFFFFF
};

void aac_initbits(bitfile *ld, const void *buffer, const uint32_t buffer_size);

uint8_t aac_byte_align(bitfile *ld);
uint32_t aac_get_processed_bits(bitfile *ld);
void aac_flushbits_ex(bitfile *ld, uint32_t bits);
//uint8_t *aac_getbitbuffer(bitfile *ld, uint32_t bits);

/* circumvent memory alignment errors on ARM */
static INLINE uint32_t getdword(void *mem)
{
    uint32_t tmp;

    ((uint8_t*)&tmp)[0] = ((uint8_t*)mem)[3];
    ((uint8_t*)&tmp)[1] = ((uint8_t*)mem)[2];
    ((uint8_t*)&tmp)[2] = ((uint8_t*)mem)[1];
    ((uint8_t*)&tmp)[3] = ((uint8_t*)mem)[0];  
    
    return tmp;

}

static INLINE uint32_t aac_showbits(bitfile *ld, uint32_t bits)
{
    if (bits <= ld->bits_left)
    {
        return (ld->bufa >> (ld->bits_left - bits)) & bitmask[bits];
    }

    bits -= ld->bits_left;
    return ((ld->bufa & bitmask[ld->bits_left]) << bits) | (ld->bufb >> (32 - bits));
}

static INLINE void aac_flushbits(bitfile *ld, uint32_t bits)
{
    uint32_t tmp;
        
    /* do nothing if error */
    if (ld->error != 0)
        return;

    if (bits < ld->bits_left)
    {
        ld->bits_left -= bits;
    } else {
//        aac_flushbits_ex(ld, bits);

            ld->bufa = ld->bufb;
            if (ld->no_more_reading == 0)
            {
//                tmp = getdword(ld->tail);
                ((uint8_t*)&tmp)[0] = ((uint8_t*)ld->tail)[3];
                ((uint8_t*)&tmp)[1] = ((uint8_t*)ld->tail)[2];
                ((uint8_t*)&tmp)[2] = ((uint8_t*)ld->tail)[1];
                ((uint8_t*)&tmp)[3] = ((uint8_t*)ld->tail)[0]; 

                ld->tail++;
            } else {
                tmp = 0;
            }
            ld->bufb = tmp;
            ld->bits_left += (32 - bits);
            ld->bytes_used += 4;
            if (ld->bytes_used == ld->buffer_size)
                ld->no_more_reading = 1;
            if (ld->bytes_used > ld->buffer_size)
                ld->error = 1;

    }
}

/* return next n bits (right adjusted) */
static INLINE uint32_t aac_getbits(bitfile *ld, uint32_t n)
{
    uint32_t ret,bit;
    uint32_t tmp;

//    if (ld->no_more_reading || n == 0)
 //       return 0;

//    ret = aac_showbits(ld, n);
    
    if (n <= ld->bits_left)
    {
        ret = ((ld->bufa >> (ld->bits_left - n)) & bitmask[n]);
    }
    else
    {
        bit = n - ld->bits_left;
        ret = ((ld->bufa & bitmask[ld->bits_left]) << bit) | (ld->bufb >> (32 - bit));
    }
   
//    aac_flushbits(ld, n);

    if (ld->error == 0)
    {
        if (n < ld->bits_left)
        {
            ld->bits_left -= n;
        } else {
//            aac_flushbits_ex(ld, n);

            ld->bufa = ld->bufb;
            if (ld->no_more_reading == 0)
            {
//                tmp = getdword(ld->tail);
                ((uint8_t*)&tmp)[0] = ((uint8_t*)ld->tail)[3];
                ((uint8_t*)&tmp)[1] = ((uint8_t*)ld->tail)[2];
                ((uint8_t*)&tmp)[2] = ((uint8_t*)ld->tail)[1];
                ((uint8_t*)&tmp)[3] = ((uint8_t*)ld->tail)[0]; 
                ld->tail++;
            } else {
                tmp = 0;
            }
            ld->bufb = tmp;
            ld->bits_left += (32 - n);
            ld->bytes_used += 4;
            if (ld->bytes_used == ld->buffer_size)
                ld->no_more_reading = 1;
            if (ld->bytes_used > ld->buffer_size)
                ld->error = 1;
        }
    }
   
    return ret;
}

static INLINE uint8_t aac_get1bit(bitfile *ld)
{
    uint8_t r;
    uint32_t tmp;   

    if (ld->bits_left > 0)
    {
        ld->bits_left--;
        r = (uint8_t)((ld->bufa >> ld->bits_left) & 1);
        return r;
    }

    /* bits_left == 0 */
//    r = (uint8_t)aac_getbits(ld, 1);

//    if (ld->no_more_reading)
//        return 0;

//    ret = aac_showbits(ld, n);
    r = ((ld->bufa & bitmask[ld->bits_left]) << 1) | (ld->bufb >> (32 - 1));
    
   
//    aac_flushbits(ld, n);

    if (ld->error == 0)
    {
        if (ld->bits_left > 1)
        {
            ld->bits_left -= 1;
        } else {
//            aac_flushbits_ex(ld, n);

            ld->bufa = ld->bufb;
            if (ld->no_more_reading == 0)
            {
//                tmp = getdword(ld->tail);
                ((uint8_t*)&tmp)[0] = ((uint8_t*)ld->tail)[3];
                ((uint8_t*)&tmp)[1] = ((uint8_t*)ld->tail)[2];
                ((uint8_t*)&tmp)[2] = ((uint8_t*)ld->tail)[1];
                ((uint8_t*)&tmp)[3] = ((uint8_t*)ld->tail)[0]; 

                ld->tail++;
            } else {
                tmp = 0;
            }
            ld->bufb = tmp;
            ld->bits_left += (32 - 1);
            ld->bytes_used += 4;
            if (ld->bytes_used == ld->buffer_size)
                ld->no_more_reading = 1;
            if (ld->bytes_used > ld->buffer_size)
                ld->error = 1;
        }
    }
   
    return r;
}


#endif

#include "ait_config.h"
#if (AAC_HOST_PARSER_EN == 1)
#include "aac_comm.h"
#include "structs.h"
#include "bits.h"
#include "huffman.h"
#include "codebook/hcb.h"


/* static function declarations */
static INLINE void huffman_sign_bits(bitfile *ld, int16_t *sp, uint8_t len);
static INLINE int16_t huffman_getescape(bitfile *ld, int16_t sp);

#if 0
static uint8_t huffman_2step_quad(uint8_t cb, bitfile *ld, int16_t *sp);
static uint8_t huffman_2step_quad_sign(uint8_t cb, bitfile *ld, int16_t *sp);
static uint8_t huffman_2step_pair(uint8_t cb, bitfile *ld, int16_t *sp);
static uint8_t huffman_2step_pair_sign(uint8_t cb, bitfile *ld, int16_t *sp);
static uint8_t huffman_binary_quad(uint8_t cb, bitfile *ld, int16_t *sp);
static uint8_t huffman_binary_quad_sign(uint8_t cb, bitfile *ld, int16_t *sp);
static uint8_t huffman_binary_pair(uint8_t cb, bitfile *ld, int16_t *sp);
static uint8_t huffman_binary_pair_sign(uint8_t cb, bitfile *ld, int16_t *sp);
#endif

static int16_t huffman_codebook(uint8_t i);

int16_t huffman_scale_factor(bitfile *ld)
{
    uint16_t offset = 0;

    while (hcb_sf[offset][1])
    {
        uint8_t b = aac_get1bit(ld);

        offset += hcb_sf[offset][b];

        if (offset > 240)
        {
            /* printf("ERROR: offset into hcb_sf = %d >240!\n", offset); */
            return -1;
        }
    }

    return hcb_sf[offset][0];
}


hcb *hcb_table[] = {
    0, hcb1_1, hcb2_1, 0, hcb4_1, 0, hcb6_1, 0, hcb8_1, 0, hcb10_1, hcb11_1
};

hcb_2_quad *hcb_2_quad_table[] = {
    0, hcb1_2, hcb2_2, 0, hcb4_2, 0, 0, 0, 0, 0, 0, 0
};

hcb_2_pair *hcb_2_pair_table[] = {
    0, 0, 0, 0, 0, 0, hcb6_2, 0, hcb8_2, 0, hcb10_2, hcb11_2
};

hcb_bin_pair *hcb_bin_table[] = {
    0, 0, 0, 0, 0, hcb5, 0, hcb7, 0, hcb9, 0, 0
};

uint8_t hcbN[] = { 0, 5, 5, 0, 5, 0, 5, 0, 5, 0, 6, 5 };

/* defines whether a huffman codebook is unsigned or not */
/* Table 4.6.2 */
uint8_t unsigned_cb[] = { 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0,
  /* codebook 16 to 31 */ 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

int hcb_2_quad_table_size[] = { 0, 114, 86, 0, 185, 0, 0, 0, 0, 0, 0, 0 };
int hcb_2_pair_table_size[] = { 0, 0, 0, 0, 0, 0, 126, 0, 83, 0, 210, 373 };
int hcb_bin_table_size[] = { 0, 0, 0, 161, 0, 161, 0, 127, 0, 337, 0, 0 };

static INLINE void huffman_sign_bits(bitfile *ld, int16_t *sp, uint8_t len)
{
    uint8_t i;

    for (i = 0; i < len; i++)
    {
        if(sp[i])
        {
            if(aac_get1bit(ld) & 1)
            {
                sp[i] = -sp[i];
            }
        }
    }
}

static INLINE int16_t huffman_getescape(bitfile *ld, int16_t sp)
{
    uint8_t neg, i;
    int32_t j;
	int32_t off;

    if (sp < 0)
    {
        if (sp != -16)
            return sp;
        neg = 1;
    } else {
        if (sp != 16)
            return sp;
        neg = 0;
    }

    for (i = 4; ; i++)
    {
        if (aac_get1bit(ld) == 0)
        {
            break;
        }
    }

    off = aac_getbits(ld, i);

    j = off | (1<<i);
    if (neg)
        j = -j;

    return j;
}
#if 0
static uint8_t huffman_2step_quad(uint8_t cb, bitfile *ld, int16_t *sp)
{
    uint32_t cw;
    uint16_t offset = 0;
    uint8_t extra_bits;

    cw = aac_showbits(ld, hcbN[cb]);
    offset = hcb_table[cb][cw].offset;
    extra_bits = hcb_table[cb][cw].extra_bits;

    if (extra_bits)
    {
        /* we know for sure it's more than hcbN[cb] bits long */
        aac_flushbits(ld, hcbN[cb]);
        offset += (uint16_t)aac_showbits(ld, extra_bits);
        aac_flushbits(ld, hcb_2_quad_table[cb][offset].bits - hcbN[cb]);
    } else {
        aac_flushbits(ld, hcb_2_quad_table[cb][offset].bits);
    }

    if (offset > hcb_2_quad_table_size[cb])
    {
        /* printf("ERROR: offset into hcb_2_quad_table = %d >%d!\n", offset,
           hcb_2_quad_table_size[cb]); */
        return 10;
    }

    sp[0] = hcb_2_quad_table[cb][offset].x;
    sp[1] = hcb_2_quad_table[cb][offset].y;
    sp[2] = hcb_2_quad_table[cb][offset].v;
    sp[3] = hcb_2_quad_table[cb][offset].w;

    return 0;
}

static uint8_t huffman_2step_quad_sign(uint8_t cb, bitfile *ld, int16_t *sp)
{
    uint8_t err = huffman_2step_quad(cb, ld, sp);
    huffman_sign_bits(ld, sp, QUAD_LEN);

    return err;
}

static uint8_t huffman_2step_pair(uint8_t cb, bitfile *ld, int16_t *sp)
{
    uint32_t cw;
    uint16_t offset = 0;
    uint8_t extra_bits;

    cw = aac_showbits(ld, hcbN[cb]);
    offset = hcb_table[cb][cw].offset;
    extra_bits = hcb_table[cb][cw].extra_bits;

    if (extra_bits)
    {
        /* we know for sure it's more than hcbN[cb] bits long */
        aac_flushbits(ld, hcbN[cb]);
        offset += (uint16_t)aac_showbits(ld, extra_bits);
        aac_flushbits(ld, hcb_2_pair_table[cb][offset].bits - hcbN[cb]);
    } else {
        aac_flushbits(ld, hcb_2_pair_table[cb][offset].bits);
    }

    if (offset > hcb_2_pair_table_size[cb])
    {
        /* printf("ERROR: offset into hcb_2_pair_table = %d >%d!\n", offset,
           hcb_2_pair_table_size[cb]); */
        return 10;
    }

    sp[0] = hcb_2_pair_table[cb][offset].x;
    sp[1] = hcb_2_pair_table[cb][offset].y;

    return 0;
}

static uint8_t huffman_2step_pair_sign(uint8_t cb, bitfile *ld, int16_t *sp)
{
    uint8_t err = huffman_2step_pair(cb, ld, sp);
    huffman_sign_bits(ld, sp, PAIR_LEN);

    return err;
}

static uint8_t huffman_binary_quad(uint8_t cb, bitfile *ld, int16_t *sp)
{
    uint16_t offset = 0;

    while (!hcb3[offset].is_leaf)
    {
        uint8_t b = aac_get1bit(ld);

        offset += hcb3[offset].data[b];
    }

    if (offset > hcb_bin_table_size[cb])
    {
        /* printf("ERROR: offset into hcb_bin_table = %d >%d!\n", offset,
           hcb_bin_table_size[cb]); */
        return 10;
    }

    sp[0] = hcb3[offset].data[0];
    sp[1] = hcb3[offset].data[1];
    sp[2] = hcb3[offset].data[2];
    sp[3] = hcb3[offset].data[3];

    return 0;
}

static uint8_t huffman_binary_quad_sign(uint8_t cb, bitfile *ld, int16_t *sp)
{
    uint8_t err = huffman_binary_quad(cb, ld, sp);
    huffman_sign_bits(ld, sp, QUAD_LEN);

    return err;
}

static uint8_t huffman_binary_pair(uint8_t cb, bitfile *ld, int16_t *sp)
{
    uint16_t offset = 0;

    while (!hcb_bin_table[cb][offset].is_leaf)
    {
        uint8_t b = aac_get1bit(ld);

        offset += hcb_bin_table[cb][offset].data[b];
    }

    if (offset > hcb_bin_table_size[cb])
    {
        /* printf("ERROR: offset into hcb_bin_table = %d >%d!\n", offset,
           hcb_bin_table_size[cb]); */
        return 10;
    }

    sp[0] = hcb_bin_table[cb][offset].data[0];
    sp[1] = hcb_bin_table[cb][offset].data[1];

    return 0;
}

static uint8_t huffman_binary_pair_sign(uint8_t cb, bitfile *ld, int16_t *sp)
{
    uint8_t err = huffman_binary_pair(cb, ld, sp);
    huffman_sign_bits(ld, sp, PAIR_LEN);

    return err;
}

#endif

static int16_t huffman_codebook(uint8_t i)
{
    static const uint32_t data = 16428320;
    if (i == 0) return (int16_t)(data >> 16) & 0xFFFF;
    else        return (int16_t)data & 0xFFFF;
}

uint8_t huffman_spectral_data(uint8_t cb, bitfile *ld, int16_t *sp)
{
    uint32_t cw;
    uint32_t offset = 0;
    uint32_t extra_bits;
    uint8_t err;
//    uint32_t i;
    uint32_t bits;
    int8_t  *temp_ptr;
    
    switch (cb)
    {
    case 1: /* 2-step method for data quadruples */
    case 2:
//        return huffman_2step_quad(cb, ld, sp);

//        cw = aac_showbits(ld, hcbN[cb]);
        bits = hcbN[cb];
        if (bits <= ld->bits_left)
        {
            cw = (ld->bufa >> (ld->bits_left - bits)) & bitmask[bits];
        }
        else
        {
            bits -= ld->bits_left;
            cw = ((ld->bufa & bitmask[ld->bits_left]) << bits) | (ld->bufb >> (32 - bits));
        }
        
        offset = hcb_table[cb][cw].offset;
        extra_bits = hcb_table[cb][cw].extra_bits;
        
        if (extra_bits)
        {
            /* we know for sure it's more than hcbN[cb] bits long */
            aac_flushbits(ld, hcbN[cb]);
            offset += (uint16_t)aac_showbits(ld, extra_bits);
            aac_flushbits(ld, hcb_2_quad_table[cb][offset].bits - hcbN[cb]);
        } else {
            aac_flushbits(ld, hcb_2_quad_table[cb][offset].bits);
        }
        
        if (offset > hcb_2_quad_table_size[cb])
        {
            /* printf("ERROR: offset into hcb_2_quad_table = %d >%d!\n", offset,
               hcb_2_quad_table_size[cb]); */
            return 10;
        }
            temp_ptr = (int8_t  *)&hcb_2_quad_table[cb][offset].x;
            
            sp[0] = (*temp_ptr++);
            sp[1] = (*temp_ptr++);
            sp[2] = (*temp_ptr++);
            sp[3] = (*temp_ptr++);
            
//            sp[0] = hcb_2_quad_table[cb][offset].x;
//            sp[1] = hcb_2_quad_table[cb][offset].y;
//            sp[2] = hcb_2_quad_table[cb][offset].v;
//            sp[3] = hcb_2_quad_table[cb][offset].w;
            
            return 0;
        

    case 3: /* binary search for data quadruples */
//        return huffman_binary_quad_sign(cb, ld, sp);

//    uint8_t err = huffman_binary_quad(cb, ld, sp);
        while (!hcb3[offset].is_leaf)
        {
            uint8_t b = aac_get1bit(ld);
        
            offset += hcb3[offset].data[b];
        }
        
        if (offset > hcb_bin_table_size[cb])
        {
            /* printf("ERROR: offset into hcb_bin_table = %d >%d!\n", offset,
               hcb_bin_table_size[cb]); */
            err = 10;
        }
        else
        {
        
            temp_ptr = (int8_t  *)&hcb3[offset].data[0];
            
            sp[0] = (*temp_ptr++);
            sp[1] = (*temp_ptr++);
            sp[2] = (*temp_ptr++);
            sp[3] = (*temp_ptr++);            
            
//            sp[0] = hcb3[offset].data[0];
//            sp[1] = hcb3[offset].data[1];
//            sp[2] = hcb3[offset].data[2];
//            sp[3] = hcb3[offset].data[3];
            
            err = 0;
        }
        
//    huffman_sign_bits(ld, sp, QUAD_LEN);
#if 0
        for (i = 0; i < QUAD_LEN; i++)
        {
            if(sp[i])
            {
                if(aac_get1bit(ld) & 1)
                {
                    sp[i] = -sp[i];
                }
            }
        }
#else
            if(sp[0])
            {
                if(aac_get1bit(ld) & 1)
                {
                    sp[0] = -sp[0];
                }
            }

            if(sp[1])
            {
                if(aac_get1bit(ld) & 1)
                {
                    sp[1] = -sp[1];
                }
            }

            if(sp[2])
            {
                if(aac_get1bit(ld) & 1)
                {
                    sp[2] = -sp[2];
                }
            }
            
            if(sp[3])
            {
                if(aac_get1bit(ld) & 1)
                {
                    sp[3] = -sp[3];
                }
            }            


#endif
    return err;


    case 4: /* 2-step method for data quadruples */
//        return huffman_2step_quad_sign(cb, ld, sp);

//    uint8_t err = huffman_2step_quad(cb, ld, sp);
//        cw = aac_showbits(ld, hcbN[cb]);
        bits = hcbN[cb];
        if (bits <= ld->bits_left)
        {
            cw = (ld->bufa >> (ld->bits_left - bits)) & bitmask[bits];
        }
        else
        {
            bits -= ld->bits_left;
            cw = ((ld->bufa & bitmask[ld->bits_left]) << bits) | (ld->bufb >> (32 - bits));
        }
        
        offset = hcb_table[cb][cw].offset;
        extra_bits = hcb_table[cb][cw].extra_bits;
        
        if (extra_bits)
        {
            /* we know for sure it's more than hcbN[cb] bits long */
            aac_flushbits(ld, hcbN[cb]);
            offset += (uint16_t)aac_showbits(ld, extra_bits);
            aac_flushbits(ld, hcb_2_quad_table[cb][offset].bits - hcbN[cb]);
        } else {
            aac_flushbits(ld, hcb_2_quad_table[cb][offset].bits);
        }
        
        if (offset > hcb_2_quad_table_size[cb])
        {
            /* printf("ERROR: offset into hcb_2_quad_table = %d >%d!\n", offset,
               hcb_2_quad_table_size[cb]); */
            err = 10;
        }
        else
        {
            temp_ptr = (int8_t  *)&hcb_2_quad_table[cb][offset].x;
            
            sp[0] = (*temp_ptr++);
            sp[1] = (*temp_ptr++);
            sp[2] = (*temp_ptr++);
            sp[3] = (*temp_ptr++);                    
        
//            sp[0] = hcb_2_quad_table[cb][offset].x;
//            sp[1] = hcb_2_quad_table[cb][offset].y;
//            sp[2] = hcb_2_quad_table[cb][offset].v;
//            sp[3] = hcb_2_quad_table[cb][offset].w;
            
            err = 0;
         }   
//    huffman_sign_bits(ld, sp, QUAD_LEN);
#if 0
        for (i = 0; i < QUAD_LEN; i++)
        {
            if(sp[i])
            {
                if(aac_get1bit(ld) & 1)
                {
                    sp[i] = -sp[i];
                }
            }
        }
#else
            if(sp[0])
            {
                if(aac_get1bit(ld) & 1)
                {
                    sp[0] = -sp[0];
                }
            }

            if(sp[1])
            {
                if(aac_get1bit(ld) & 1)
                {
                    sp[1] = -sp[1];
                }
            }

            if(sp[2])
            {
                if(aac_get1bit(ld) & 1)
                {
                    sp[2] = -sp[2];
                }
            }
            
            if(sp[3])
            {
                if(aac_get1bit(ld) & 1)
                {
                    sp[3] = -sp[3];
                }
            }            

#endif        
    return err;
    
    case 5: /* binary search for data pairs */
//        return huffman_binary_pair(cb, ld, sp);

        while (!hcb_bin_table[cb][offset].is_leaf)
        {
            uint8_t b = aac_get1bit(ld);
        
            offset += hcb_bin_table[cb][offset].data[b];
        }
        
        if (offset > hcb_bin_table_size[cb])
        {
            /* printf("ERROR: offset into hcb_bin_table = %d >%d!\n", offset,
               hcb_bin_table_size[cb]); */
            return 10;
        }
        
        temp_ptr = (int8_t  *)&hcb_bin_table[cb][offset].data[0];
        
        sp[0] = (*temp_ptr++);
        sp[1] = (*temp_ptr++);        
        
//        sp[0] = hcb_bin_table[cb][offset].data[0];
//        sp[1] = hcb_bin_table[cb][offset].data[1];
        
        return 0;

    case 6: /* 2-step method for data pairs */
//        return huffman_2step_pair(cb, ld, sp);
//        cw = aac_showbits(ld, hcbN[cb]);
        bits = hcbN[cb];
        if (bits <= ld->bits_left)
        {
            cw = (ld->bufa >> (ld->bits_left - bits)) & bitmask[bits];
        }
        else
        {
            bits -= ld->bits_left;
            cw = ((ld->bufa & bitmask[ld->bits_left]) << bits) | (ld->bufb >> (32 - bits));
        }
        
        offset = hcb_table[cb][cw].offset;
        extra_bits = hcb_table[cb][cw].extra_bits;
        
        if (extra_bits)
        {
            /* we know for sure it's more than hcbN[cb] bits long */
            aac_flushbits(ld, hcbN[cb]);
            offset += (uint16_t)aac_showbits(ld, extra_bits);
            aac_flushbits(ld, hcb_2_pair_table[cb][offset].bits - hcbN[cb]);
        } else {
            aac_flushbits(ld, hcb_2_pair_table[cb][offset].bits);
        }
        
        if (offset > hcb_2_pair_table_size[cb])
        {
            /* printf("ERROR: offset into hcb_2_pair_table = %d >%d!\n", offset,
               hcb_2_pair_table_size[cb]); */
            return 10;
        }
  
        temp_ptr = (int8_t  *)&hcb_2_pair_table[cb][offset].x;
        
        sp[0] = (*temp_ptr++);
        sp[1] = (*temp_ptr++);        
                
//        sp[0] = hcb_2_pair_table[cb][offset].x;
//        sp[1] = hcb_2_pair_table[cb][offset].y;
        
        return 0;


    case 7: /* binary search for data pairs */
    case 9:
//        return huffman_binary_pair_sign(cb, ld, sp);

//    uint8_t err = huffman_binary_pair(cb, ld, sp);
        while (!hcb_bin_table[cb][offset].is_leaf)
        {
            uint8_t b = aac_get1bit(ld);
        
            offset += hcb_bin_table[cb][offset].data[b];
        }
        
        if (offset > hcb_bin_table_size[cb])
        {
            /* printf("ERROR: offset into hcb_bin_table = %d >%d!\n", offset,
               hcb_bin_table_size[cb]); */
            err = 10;
        }
        else
        {
        temp_ptr = (int8_t  *)&hcb_bin_table[cb][offset].data[0];
        
        sp[0] = (*temp_ptr++);
        sp[1] = (*temp_ptr++);            
//            sp[0] = hcb_bin_table[cb][offset].data[0];
//            sp[1] = hcb_bin_table[cb][offset].data[1];
        
            err = 0;
        }
        
//    huffman_sign_bits(ld, sp, PAIR_LEN);
#if 0
        for (i = 0; i < PAIR_LEN; i++)
        {
            if(sp[i])
            {
                if(aac_get1bit(ld) & 1)
                {
                    sp[i] = -sp[i];
                }
            }
        }
#else
            if(sp[0])
            {
                if(aac_get1bit(ld) & 1)
                {
                    sp[0] = -sp[0];
                }
            }

            if(sp[1])
            {
                if(aac_get1bit(ld) & 1)
                {
                    sp[1] = -sp[1];
                }
            }

#endif
    return err;

    case 8: /* 2-step method for data pairs */
    case 10:
//        return huffman_2step_pair_sign(cb, ld, sp);

//    uint8_t err = huffman_2step_pair(cb, ld, sp);
//        cw = aac_showbits(ld, hcbN[cb]);
        bits = hcbN[cb];
        if (bits <= ld->bits_left)
        {
            cw = (ld->bufa >> (ld->bits_left - bits)) & bitmask[bits];
        }
        else
        {
            bits -= ld->bits_left;
            cw = ((ld->bufa & bitmask[ld->bits_left]) << bits) | (ld->bufb >> (32 - bits));
        }
        
        offset = hcb_table[cb][cw].offset;
        extra_bits = hcb_table[cb][cw].extra_bits;
        
        if (extra_bits)
        {
            /* we know for sure it's more than hcbN[cb] bits long */
            aac_flushbits(ld, hcbN[cb]);
            offset += (uint16_t)aac_showbits(ld, extra_bits);
            aac_flushbits(ld, hcb_2_pair_table[cb][offset].bits - hcbN[cb]);
        } else {
            aac_flushbits(ld, hcb_2_pair_table[cb][offset].bits);
        }
        
        if (offset > hcb_2_pair_table_size[cb])
        {
            /* printf("ERROR: offset into hcb_2_pair_table = %d >%d!\n", offset,
               hcb_2_pair_table_size[cb]); */
            err = 10;
        }
        else
        {
        temp_ptr = (int8_t  *)&hcb_2_pair_table[cb][offset].x;
        
        sp[0] = (*temp_ptr++);
        sp[1] = (*temp_ptr++);                    
//            sp[0] = hcb_2_pair_table[cb][offset].x;
//            sp[1] = hcb_2_pair_table[cb][offset].y;
        
            err = 0;
        }
//    huffman_sign_bits(ld, sp, PAIR_LEN);
#if 0
        for (i = 0; i < PAIR_LEN; i++)
        {
            if(sp[i])
            {
                if(aac_get1bit(ld) & 1)
                {
                    sp[i] = -sp[i];
                }
            }
        }
#else
            if(sp[0])
            {
                if(aac_get1bit(ld) & 1)
                {
                    sp[0] = -sp[0];
                }
            }

            if(sp[1])
            {
                if(aac_get1bit(ld) & 1)
                {
                    sp[1] = -sp[1];
                }
            }

#endif
    return err;

    case 12: {
//        uint8_t err = huffman_2step_pair(11, ld, sp);

        cw = aac_showbits(ld, hcbN[11]);
        offset = hcb_table[11][cw].offset;
        extra_bits = hcb_table[11][cw].extra_bits;
        
        if (extra_bits)
        {
            /* we know for sure it's more than hcbN[cb] bits long */
            aac_flushbits(ld, hcbN[cb]);
            offset += (uint16_t)aac_showbits(ld, extra_bits);
            aac_flushbits(ld, hcb_2_pair_table[11][offset].bits - hcbN[11]);
        } else {
            aac_flushbits(ld, hcb_2_pair_table[11][offset].bits);
        }
        
        if (offset > hcb_2_pair_table_size[11])
        {
            /* printf("ERROR: offset into hcb_2_pair_table = %d >%d!\n", offset,
               hcb_2_pair_table_size[cb]); */
            err = 10;
        }
        else
        {
//            sp[0] = hcb_2_pair_table[11][offset].x;
//            sp[1] = hcb_2_pair_table[11][offset].y;
            
            err = 0;
        }

        sp[0] = huffman_codebook(0); sp[1] = huffman_codebook(1); 
        return err; }
    case 11:
    {
//        uint8_t err = huffman_2step_pair_sign(11, ld, sp);
//    uint8_t err = huffman_2step_pair(cb, ld, sp);
        cw = aac_showbits(ld, hcbN[11]);
        offset = hcb_table[11][cw].offset;
        extra_bits = hcb_table[11][cw].extra_bits;
        
        if (extra_bits)
        {
            /* we know for sure it's more than hcbN[cb] bits long */
            aac_flushbits(ld, hcbN[11]);
            offset += (uint16_t)aac_showbits(ld, extra_bits);
            aac_flushbits(ld, hcb_2_pair_table[11][offset].bits - hcbN[11]);
        } else {
            aac_flushbits(ld, hcb_2_pair_table[11][offset].bits);
        }
        
        if (offset > hcb_2_pair_table_size[11])
        {
            /* printf("ERROR: offset into hcb_2_pair_table = %d >%d!\n", offset,
               hcb_2_pair_table_size[cb]); */
            err = 10;
        }
        else
        {
        temp_ptr = (int8_t  *)&hcb_2_pair_table[11][offset].x;
        
        sp[0] = (*temp_ptr++);
        sp[1] = (*temp_ptr++);                            
//            sp[0] = hcb_2_pair_table[11][offset].x;
//            sp[1] = hcb_2_pair_table[11][offset].y;
        
            err = 0;
        }
//    huffman_sign_bits(ld, sp, PAIR_LEN);
#if 0
        for (i = 0; i < PAIR_LEN; i++)
        {
            if(sp[i])
            {
                if(aac_get1bit(ld) & 1)
                {
                    sp[i] = -sp[i];
                }
            }
        }
#else
            if(sp[0])
            {
                if(aac_get1bit(ld) & 1)
                {
                    sp[0] = -sp[0];
                }
            }

            if(sp[1])
            {
                if(aac_get1bit(ld) & 1)
                {
                    sp[1] = -sp[1];
                }
            }

#endif
//    return err;

        sp[0] = huffman_getescape(ld, sp[0]);
        sp[1] = huffman_getescape(ld, sp[1]);
        return err;
    }
    default:
        /* Non existent codebook number, something went wrong */
        return 11;
    }

    return 0;
}

#endif

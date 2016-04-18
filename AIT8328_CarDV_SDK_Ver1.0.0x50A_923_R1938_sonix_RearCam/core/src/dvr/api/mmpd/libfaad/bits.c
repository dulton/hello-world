
#include "aac_comm.h"
#include "structs.h"
#include "bits.h"

#include "ait_utility.h"

/* initialize buffer, call once before first getbits or showbits */
void aac_initbits(bitfile *ld, const void *_buffer, const uint32_t buffer_size)
{
    uint32_t tmp;
    static uint8_t  bit_buffer[1536];

    if (ld == 0)
        return;

//    memset(ld, 0, sizeof(bitfile));

    if (buffer_size == 0 || _buffer == 0)
    {
        ld->error = 1;
        ld->no_more_reading = 1;
        return;
    }

//    ld->buffer = malloc((buffer_size+12)*sizeof(uint8_t));
    ld->buffer = &bit_buffer;
//    memset(ld->buffer, 0, (buffer_size+12)*sizeof(uint8_t));
    MEMCPY(ld->buffer, _buffer, buffer_size*sizeof(uint8_t));

    ld->buffer_size = buffer_size;

    tmp = getdword((uint32_t*)ld->buffer);
    ld->bufa = tmp;

    tmp = getdword((uint32_t*)ld->buffer + 1);
    ld->bufb = tmp;

    ld->start = (uint32_t*)ld->buffer;
    ld->tail = ((uint32_t*)ld->buffer + 2);

    ld->bits_left = 32;

    ld->bytes_used = 0;
    ld->no_more_reading = 0;
    ld->error = 0;
}


uint32_t aac_get_processed_bits(bitfile *ld)
{
    return (uint32_t)(8 * (4*(ld->tail - ld->start) - 4) - (ld->bits_left));
}

uint8_t aac_byte_align(bitfile *ld)
{
    uint8_t remainder = (uint8_t)((32 - ld->bits_left) % 8);

    if (remainder)
    {
        aac_flushbits(ld, 8 - remainder);
        return (8 - remainder);
    }
    return 0;
}

void aac_flushbits_ex(bitfile *ld, uint32_t bits)
{
    uint32_t tmp;

    ld->bufa = ld->bufb;
    if (ld->no_more_reading == 0)
    {
        tmp = getdword(ld->tail);
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





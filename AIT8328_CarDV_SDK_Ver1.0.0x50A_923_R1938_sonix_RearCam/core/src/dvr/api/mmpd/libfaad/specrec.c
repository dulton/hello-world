
#include "aac_comm.h"
#include "structs.h"
#include "specrec.h"
#include "syntax.h"
#include "iq_table.h"
//#include "ms.h"
#include "is.h"
#include "pns.h"
//#include "tns.h"
#include "drc.h"


/* static function declarations */

ALIGN static const uint8_t num_swb_960_window[] =
{
    40, 40, 45, 49, 49, 49, 46, 46, 42, 42, 42, 40
};

ALIGN static const uint8_t num_swb_1024_window[] =
{
    41, 41, 47, 49, 49, 51, 47, 47, 43, 43, 43, 40
};

ALIGN static const uint8_t num_swb_128_window[] =
{
    12, 12, 12, 14, 14, 14, 15, 15, 15, 15, 15, 15
};

ALIGN static const uint16_t swb_offset_1024_96[] =
{
    0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56,
    64, 72, 80, 88, 96, 108, 120, 132, 144, 156, 172, 188, 212, 240,
    276, 320, 384, 448, 512, 576, 640, 704, 768, 832, 896, 960, 1024
};

ALIGN static const uint16_t swb_offset_128_96[] =
{
    0, 4, 8, 12, 16, 20, 24, 32, 40, 48, 64, 92, 128
};

ALIGN static const uint16_t swb_offset_1024_64[] =
{
    0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56,
    64, 72, 80, 88, 100, 112, 124, 140, 156, 172, 192, 216, 240, 268,
    304, 344, 384, 424, 464, 504, 544, 584, 624, 664, 704, 744, 784, 824,
    864, 904, 944, 984, 1024
};

ALIGN static const uint16_t swb_offset_128_64[] =
{
    0, 4, 8, 12, 16, 20, 24, 32, 40, 48, 64, 92, 128
};

ALIGN static const uint16_t swb_offset_1024_48[] =
{
    0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 48, 56, 64, 72,
    80, 88, 96, 108, 120, 132, 144, 160, 176, 196, 216, 240, 264, 292,
    320, 352, 384, 416, 448, 480, 512, 544, 576, 608, 640, 672, 704, 736,
    768, 800, 832, 864, 896, 928, 1024
};

ALIGN static const uint16_t swb_offset_128_48[] =
{
    0, 4, 8, 12, 16, 20, 28, 36, 44, 56, 68, 80, 96, 112, 128
};

ALIGN static const uint16_t swb_offset_1024_32[] =
{
    0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 48, 56, 64, 72,
    80, 88, 96, 108, 120, 132, 144, 160, 176, 196, 216, 240, 264, 292,
    320, 352, 384, 416, 448, 480, 512, 544, 576, 608, 640, 672, 704, 736,
    768, 800, 832, 864, 896, 928, 960, 992, 1024
};

ALIGN static const uint16_t swb_offset_1024_24[] =
{
    0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 52, 60, 68,
    76, 84, 92, 100, 108, 116, 124, 136, 148, 160, 172, 188, 204, 220,
    240, 260, 284, 308, 336, 364, 396, 432, 468, 508, 552, 600, 652, 704,
    768, 832, 896, 960, 1024
};


ALIGN static const uint16_t swb_offset_128_24[] =
{
    0, 4, 8, 12, 16, 20, 24, 28, 36, 44, 52, 64, 76, 92, 108, 128
};

ALIGN static const uint16_t swb_offset_1024_16[] =
{
    0, 8, 16, 24, 32, 40, 48, 56, 64, 72, 80, 88, 100, 112, 124,
    136, 148, 160, 172, 184, 196, 212, 228, 244, 260, 280, 300, 320, 344,
    368, 396, 424, 456, 492, 532, 572, 616, 664, 716, 772, 832, 896, 960, 1024
};

ALIGN static const uint16_t swb_offset_128_16[] =
{
    0, 4, 8, 12, 16, 20, 24, 28, 32, 40, 48, 60, 72, 88, 108, 128
};

ALIGN static const uint16_t swb_offset_1024_8[] =
{
    0, 12, 24, 36, 48, 60, 72, 84, 96, 108, 120, 132, 144, 156, 172,
    188, 204, 220, 236, 252, 268, 288, 308, 328, 348, 372, 396, 420, 448,
    476, 508, 544, 580, 620, 664, 712, 764, 820, 880, 944, 1024
};

ALIGN static const uint16_t swb_offset_128_8[] =
{
    0, 4, 8, 12, 16, 20, 24, 28, 36, 44, 52, 60, 72, 88, 108, 128
};

ALIGN static const uint16_t *swb_offset_1024_window[] =
{
    swb_offset_1024_96,      /* 96000 */
    swb_offset_1024_96,      /* 88200 */
    swb_offset_1024_64,      /* 64000 */
    swb_offset_1024_48,      /* 48000 */
    swb_offset_1024_48,      /* 44100 */
    swb_offset_1024_32,      /* 32000 */
    swb_offset_1024_24,      /* 24000 */
    swb_offset_1024_24,      /* 22050 */
    swb_offset_1024_16,      /* 16000 */
    swb_offset_1024_16,      /* 12000 */
    swb_offset_1024_16,      /* 11025 */
    swb_offset_1024_8        /* 8000  */
};

ALIGN static const  uint16_t *swb_offset_128_window[] =
{
    swb_offset_128_96,       /* 96000 */
    swb_offset_128_96,       /* 88200 */
    swb_offset_128_64,       /* 64000 */
    swb_offset_128_48,       /* 48000 */
    swb_offset_128_48,       /* 44100 */
    swb_offset_128_48,       /* 32000 */
    swb_offset_128_24,       /* 24000 */
    swb_offset_128_24,       /* 22050 */
    swb_offset_128_16,       /* 16000 */
    swb_offset_128_16,       /* 12000 */
    swb_offset_128_16,       /* 11025 */
    swb_offset_128_8         /* 8000  */
};

#define bit_set(A, B) ((A) & (1<<(B)))

/* 4.5.2.3.4 */
/*
  - determine the number of windows in a window_sequence named num_windows
  - determine the number of window_groups named num_window_groups
  - determine the number of windows in each group named window_group_length[g]
  - determine the total number of scalefactor window bands named num_swb for
    the actual window type
  - determine swb_offset[swb], the offset of the first coefficient in
    scalefactor window band named swb of the window actually used
  - determine sect_sfb_offset[g][section],the offset of the first coefficient
    in section named section. This offset depends on window_sequence and
    scale_factor_grouping and is needed to decode the spectral_data().
*/
uint8_t window_grouping_info(faacDecHandle hDecoder, ic_stream *ics)
{
    uint32_t i, g;

    uint8_t sf_index = hDecoder->sf_index;

    switch (ics->window_sequence) {
    case ONLY_LONG_SEQUENCE:
    case LONG_START_SEQUENCE:
    case LONG_STOP_SEQUENCE:
        ics->num_windows = 1;
        ics->num_window_groups = 1;
        ics->window_group_length[ics->num_window_groups-1] = 1;

        if (hDecoder->frameLength == 1024)
            ics->num_swb = num_swb_1024_window[sf_index];
        else /* if (hDecoder->frameLength == 960) */
            ics->num_swb = num_swb_960_window[sf_index];

        /* preparation of sect_sfb_offset for long blocks */
        /* also copy the last value! */
        for (i = 0; i < ics->num_swb; i++)
        {
            ics->sect_sfb_offset[0][i] = swb_offset_1024_window[sf_index][i];
            ics->swb_offset[i] = swb_offset_1024_window[sf_index][i];
        }
        ics->sect_sfb_offset[0][ics->num_swb] = hDecoder->frameLength;
        ics->swb_offset[ics->num_swb] = hDecoder->frameLength;

        return 0;
    case EIGHT_SHORT_SEQUENCE:
        ics->num_windows = 8;
        ics->num_window_groups = 1;
        ics->window_group_length[ics->num_window_groups-1] = 1;
        ics->num_swb = num_swb_128_window[sf_index];

        for (i = 0; i < ics->num_swb; i++)
            ics->swb_offset[i] = swb_offset_128_window[sf_index][i];
        ics->swb_offset[ics->num_swb] = hDecoder->frameLength/8;

        for (i = 0; i < ics->num_windows-1; i++) {
            if (bit_set(ics->scale_factor_grouping, 6-i) == 0)
            {
                ics->num_window_groups += 1;
                ics->window_group_length[ics->num_window_groups-1] = 1;
            } else {
                ics->window_group_length[ics->num_window_groups-1] += 1;
            }
        }

        /* preparation of sect_sfb_offset for short blocks */
        for (g = 0; g < ics->num_window_groups; g++)
        {
            uint16_t width;
            uint8_t sect_sfb = 0;
            uint16_t offset = 0;

            for (i = 0; i < ics->num_swb; i++)
            {
                if (i+1 == ics->num_swb)
                {
                    width = (hDecoder->frameLength/8) - swb_offset_128_window[sf_index][i];
                } else {
                    width = swb_offset_128_window[sf_index][i+1] -
                        swb_offset_128_window[sf_index][i];
                }
                width *= ics->window_group_length[g];
                ics->sect_sfb_offset[g][sect_sfb++] = offset;
                offset += width;
            }
            ics->sect_sfb_offset[g][sect_sfb] = offset;
        }
        return 0;
    default:
        return 1;
    }
}


uint8_t reconstruct_single_channel(faacDecHandle hDecoder, ic_stream *ics,
                                   element *sce, int16_t *spec_data)
{
    uint8_t output_channels;
//    real_t *spec_coef = (real_t *)spec_coef1_ptr;

    /* determine whether some mono->stereo tool is used */
    output_channels = 1;

    if (hDecoder->element_output_channels[hDecoder->fr_ch_ele] == 0)
    {
        /* element_output_channels not set yet */
        hDecoder->element_output_channels[hDecoder->fr_ch_ele] = output_channels;
    } else if (hDecoder->element_output_channels[hDecoder->fr_ch_ele] != output_channels) {
        /* element inconsistency */
        return 21;
    }

    return 0;
}



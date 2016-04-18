
#ifndef __STRUCTS_H__
#define __STRUCTS_H__

//#include "cfft.h"
#include "aac_comm.h"
#ifdef SBR_DEC
#include "sbr_dec.h"
#endif

#define MAX_CHANNELS        2
#define MAX_SYNTAX_ELEMENTS 2
#define MAX_WINDOW_GROUPS    8
#define MAX_SFB             51


typedef struct
{
    uint8_t present;

    uint8_t num_bands;
    uint8_t pce_instance_tag;
    uint8_t excluded_chns_present;
    uint8_t band_top[17];
    uint8_t prog_ref_level;
    uint8_t dyn_rng_sgn[17];
    uint8_t dyn_rng_ctl[17];
    uint8_t exclude_mask[MAX_CHANNELS];
    uint8_t additional_excluded_chns[MAX_CHANNELS];

    real_t ctrl1;
    real_t ctrl2;
} drc_info;

typedef struct
{
    uint8_t element_instance_tag;
    uint8_t object_type;
    uint8_t sf_index;
    uint8_t num_front_channel_elements;
    uint8_t mono_mixdown_present;
    uint8_t mono_mixdown_element_number;
    uint8_t stereo_mixdown_present;
    uint8_t stereo_mixdown_element_number;
    uint8_t front_element_is_cpe[2];
    uint8_t front_element_tag_select[2];
    uint8_t channels;

    /* extra added values */
    uint8_t num_front_channels;

    uint8_t sce_channel[2];
    uint8_t cpe_channel[2];
} program_config;

typedef struct
{
    uint16_t syncword;
    uint8_t id;
    uint8_t layer;
    uint8_t protection_absent;
    uint8_t profile;
    uint8_t sf_index;
    uint8_t private_bit;
    uint8_t channel_configuration;
    uint8_t original;
    uint8_t home;
    uint8_t emphasis;
    uint8_t copyright_identification_bit;
    uint8_t copyright_identification_start;
    uint16_t aac_frame_length;
    uint16_t adts_buffer_fullness;
    uint8_t no_raw_data_blocks_in_frame;
    uint16_t crc_check;

    /* control param */
    uint8_t old_format;
} adts_header;

typedef struct
{
    uint8_t copyright_id_present;
    int8_t copyright_id[10];
    uint8_t original_copy;
    uint8_t home;
    uint8_t bitstream_type;
    uint32_t bitrate;
    uint8_t num_program_config_elements;
    uint32_t adif_buffer_fullness;

    /* maximum of 16 PCEs */
    program_config pce[2];
} adif_header;

typedef struct
{
    uint8_t number_pulse;
    uint8_t pulse_start_sfb;
    uint8_t pulse_offset[4];
    uint8_t pulse_amp[4];
} pulse_info;

typedef struct
{
    uint8_t n_filt[8];
    uint8_t coef_res[8];
    uint8_t length[8][4];
    uint8_t order[8][4];
    uint8_t direction[8][4];
    uint8_t coef_compress[8][4];
    uint8_t coef[8][4][32];
} tns_info;

typedef struct
{
    uint8_t max_sfb;

    uint8_t num_swb;
    uint8_t num_window_groups;
    uint8_t num_windows;
    uint8_t window_sequence;
    uint8_t window_group_length[8];
    uint8_t window_shape;
    uint8_t scale_factor_grouping;
    uint16_t sect_sfb_offset[8][MAX_SFB];
    uint16_t swb_offset[52];

    uint8_t sect_cb[8][MAX_SFB];
    uint16_t sect_start[8][MAX_SFB];
    uint16_t sect_end[8][MAX_SFB];
    uint8_t sfb_cb[8][MAX_SFB];
    uint8_t num_sec[8]; /* number of sections in a group */

    uint8_t global_gain;
    int16_t scale_factors[8][51]; /* [0..255] */

    uint8_t ms_mask_present;
    uint8_t ms_used[MAX_WINDOW_GROUPS][MAX_SFB];

    uint8_t noise_used;

    uint8_t pulse_data_present;
    uint8_t tns_data_present;
    uint8_t gain_control_data_present;
    uint8_t predictor_data_present;

    pulse_info pul;
    tns_info tns;
//    pred_info pred;
} ic_stream; /* individual channel stream */

typedef struct
{
    uint8_t ele_id;

    uint8_t channel;
    int16_t paired_channel;

    uint8_t element_instance_tag;
    uint8_t common_window;

    ic_stream *ics1;
    ic_stream *ics2;
} element; /* syntax element (SCE, CPE, LFE) */


typedef struct faacDecConfiguration
{
    uint8_t defObjectType;
    uint32_t defSampleRate;
    uint8_t outputFormat;
    uint8_t downMatrix;
    uint8_t useOldADTSFormat;
} faacDecConfiguration, *faacDecConfigurationPtr;

typedef struct faacDecFrameInfo
{
    uint32_t bytesconsumed;
    uint32_t samples;
    uint8_t channels;
    uint8_t error;
    uint32_t samplerate;

    /* SBR: 0: off, 1: on; normal, 2: on; downsampled */
    uint8_t sbr;

    /* MPEG-4 ObjectType */
    uint8_t object_type;

    /* AAC header type; MP4 will be signalled as RAW also */
    uint8_t header_type;

    /* channel configuration */
    uint8_t num_front_channels;

    uint8_t channel_position[MAX_CHANNELS];
} faacDecFrameInfo;

typedef struct
{
    uint8_t adts_header_present;
    uint8_t adif_header_present;
    uint8_t sf_index;
    uint8_t object_type;
    uint8_t channelConfiguration;
    uint16_t frameLength;
    uint8_t postSeekResetFlag;

    uint32_t frame;

    uint8_t downMatrix;
    uint8_t first_syn_ele;
    uint8_t has_lfe;
    /* number of channels in current frame */
    uint8_t fr_channels;
    /* number of elements in current frame */
    uint8_t fr_ch_ele;

    /* element_output_channels:
       determines the number of channels the element will output
    */
    uint8_t element_output_channels[MAX_SYNTAX_ELEMENTS];
    /* element_alloced:
       determines whether the data needed for the element is allocated or not
    */
    uint8_t element_alloced[MAX_SYNTAX_ELEMENTS];
    /* alloced_channels:
       determines the number of channels where output data is allocated for
    */
    uint8_t alloced_channels;

//    uint8_t window_shape_prev[MAX_CHANNELS];
    drc_info *drc;
#ifdef SBR_DEC
    int8_t sbr_present_flag;
    sbr_info *sbr[MAX_SYNTAX_ELEMENTS];
    int8_t   ps_used[MAX_SYNTAX_ELEMENTS];           //eric add
#endif    

    /* Program Config Element */
    uint8_t pce_set;
    program_config pce;
    uint8_t element_id[MAX_CHANNELS];
    uint8_t internal_channel[MAX_CHANNELS];

    /* Configuration data */
    faacDecConfiguration config;

} faacDecStruct, *faacDecHandle;


#endif



#ifndef AACPLUS_PARSER_H
#define AACPLUS_PARSER_H
#include "aacplus_FFR_bitbuffer.h"

typedef struct
{
    unsigned char element_instance_tag;
    unsigned char object_type;
    unsigned char sf_index;
    unsigned char num_front_channel_elements;
    unsigned char mono_mixdown_present;
    unsigned char mono_mixdown_element_number;
    unsigned char stereo_mixdown_present;
    unsigned char stereo_mixdown_element_number;
    unsigned char front_element_is_cpe[2];
    unsigned char front_element_tag_select[2];
    unsigned char channels;

    /* extra added values */
    unsigned char num_front_channels;

    unsigned char sce_channel[2];
    unsigned char cpe_channel[2];
} program_config;

typedef struct
{
    unsigned char copyright_id_present;
    signed char copyright_id[10];
    unsigned char original_copy;
    unsigned char home;
    unsigned char bitstream_type;
    unsigned int bitrate;
    unsigned char num_program_config_elements;
    unsigned int adif_buffer_fullness;

    /* maximum of 16 PCEs */
    program_config pce[2];
} adif_header;

typedef struct
{
    int syncword;
    int id;
    int layer;
    int protection_absent;
    int profile;
    int sf_index;
    int private_bit;
    int channel_configuration;
    int original;
    int home;
    int emphasis;
    int copyright_identification_bit;
    int copyright_identification_start;
    int aac_frame_length;
    int adts_buffer_fullness;
    int no_raw_data_blocks_in_frame;
    int crc_check;

    /* control param */
    int old_format;
} adts_header;

int get_sample_rate(const  int sr_index);
void get_adif_header(adif_header *adif, HANDLE_BIT_BUF hBitBuf);
int adts_frame(adts_header *adts, HANDLE_BIT_BUF hBitBuf);
void aac_byte_align(HANDLE_BIT_BUF hBitBuf);
#endif
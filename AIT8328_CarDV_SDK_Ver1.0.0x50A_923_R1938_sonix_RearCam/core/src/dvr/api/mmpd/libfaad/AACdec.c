
#include "aac_comm.h"
#include "structs.h"
#include "AACdec.h"
#include "syntax.h"
#include "drc.h"

#include "ait_utility.h"

#if (AAC_HOST_PARSER_EN == 1)
faacDecStruct sDecoder;
extern MMP_ULONG m_ulAACPlusNotSupport;

uint32_t AACAPI faacDecGetCapabilities(void)
{
    uint32_t cap = 0;

    /* can't do without it */
    cap += LC_DEC_CAP;
    cap += FIXED_POINT_CAP;

    return cap;
}

faacDecHandle AACAPI faacDecOpen(void)
{
    faacDecHandle hDecoder_temp;    
    hDecoder_temp = (faacDecHandle)&sDecoder;

//    if ((hDecoder = (faacDecHandle)malloc(sizeof(faacDecStruct))) == 0)
//        return 0;

    MEMSET(hDecoder_temp, 0, sizeof(faacDecStruct));

    hDecoder_temp->config.outputFormat  = AAC_FMT_16BIT;
    hDecoder_temp->config.defObjectType = LC;
    hDecoder_temp->config.defSampleRate = 44100; /* Default: 44.1kHz */
    hDecoder_temp->config.downMatrix = 0;
    hDecoder_temp->adts_header_present = 0;
    hDecoder_temp->adif_header_present = 0;
    hDecoder_temp->frameLength = 1024;

    hDecoder_temp->frame = 0;

//    for (i = 0; i < MAX_CHANNELS; i++)
//    {
//        hDecoder_temp->window_shape_prev[i] = 0;
//    }

    hDecoder_temp->drc = drc_init(REAL_CONST(1.0), REAL_CONST(1.0));

    return hDecoder_temp;
}

faacDecConfigurationPtr AACAPI faacDecGetCurrentConfiguration(faacDecHandle hDecoder)
{
    if (hDecoder)
    {
        faacDecConfigurationPtr config = &(hDecoder->config);

        return config;
    }

    return 0;
}

uint8_t AACAPI faacDecSetConfiguration(faacDecHandle hDecoder,
                                    faacDecConfigurationPtr config)
{
    if (hDecoder && config)
    {
        /* check if we can decode this object type */
        if (can_decode_ot(config->defObjectType) < 0)
            return 0;
        hDecoder->config.defObjectType = config->defObjectType;

        /* samplerate: anything but 0 should be possible */
        if (config->defSampleRate == 0)
            return 0;
        hDecoder->config.defSampleRate = config->defSampleRate;

        /* check output format */
        if ((config->outputFormat < 1) || (config->outputFormat > 9))
            return 0;
        hDecoder->config.outputFormat = config->outputFormat;

        if (config->downMatrix > 1)
            hDecoder->config.downMatrix = config->downMatrix;

        /* OK */
        return 1;
    }

    return 0;
}
int32_t AACAPI faacDecInit(faacDecHandle hDecoder, uint8_t *buffer,
                            uint32_t buffer_size,
                            uint32_t *samplerate, uint8_t *channels)
{
    uint32_t bits = 0;
    bitfile ld;
    adif_header adif;
    adts_header adts;

    if ((hDecoder == 0) || (samplerate == 0) || (channels == 0))
        return -1;

    hDecoder->sf_index = get_sr_index(hDecoder->config.defSampleRate);
    hDecoder->object_type = hDecoder->config.defObjectType;
    *samplerate = get_sample_rate(hDecoder->sf_index);
    *channels = 1;

    if (buffer != 0)
    {
        aac_initbits(&ld, buffer, buffer_size);

        /* Check if an ADIF header is present */
        if ((buffer[0] == 'A') && (buffer[1] == 'D') &&
            (buffer[2] == 'I') && (buffer[3] == 'F'))
        {
            hDecoder->adif_header_present = 1;

            get_adif_header(&adif, &ld);
            aac_byte_align(&ld);

            hDecoder->sf_index = adif.pce[0].sf_index;
            hDecoder->object_type = adif.pce[0].object_type + 1;

            *samplerate = get_sample_rate(hDecoder->sf_index);
            *channels = adif.pce[0].channels;

            MEMCPY(&(hDecoder->pce), &(adif.pce[0]), sizeof(program_config));
            hDecoder->pce_set = 1;

            bits = bit2byte(aac_get_processed_bits(&ld));

        /* Check if an ADTS header is present */
        } else if (aac_showbits(&ld, 12) == 0xfff) {
            
            hDecoder->adts_header_present = 1;

            adts.old_format = hDecoder->config.useOldADTSFormat;
            adts_frame(&adts, &ld);

            hDecoder->sf_index = adts.sf_index;
            hDecoder->object_type = adts.profile + 1;

            *samplerate = get_sample_rate(hDecoder->sf_index);
            *channels = (adts.channel_configuration > 6) ?
                2 : adts.channel_configuration;
        }

        if (ld.error)
        {
            return -1;
        }
    }
    hDecoder->channelConfiguration = *channels;

    if (can_decode_ot(hDecoder->object_type) < 0)
    {
        m_ulAACPlusNotSupport = 1;
        return -1;
    }

    return bits;
}


static void create_channel_config(faacDecHandle hDecoder, faacDecFrameInfo *hInfo)
{
    hInfo->num_front_channels = 0;
//    hInfo->num_side_channels = 0;
//    hInfo->num_back_channels = 0;
//    hInfo->num_lfe_channels = 0;
//   memset(hInfo->channel_position, 0, MAX_CHANNELS*sizeof(uint8_t));

    if (hDecoder->downMatrix)
    {
        hInfo->num_front_channels = 2;
        hInfo->channel_position[0] = FRONT_CHANNEL_LEFT;
        hInfo->channel_position[1] = FRONT_CHANNEL_RIGHT;
        return;
    }

    /* check if there is a PCE */
    if (hDecoder->pce_set)
    {
        uint8_t i, chpos = 0;
        uint8_t chdir;

        hInfo->num_front_channels = hDecoder->pce.num_front_channels;
//        hInfo->num_side_channels = hDecoder->pce.num_side_channels;
//        hInfo->num_back_channels = hDecoder->pce.num_back_channels;
//        hInfo->num_lfe_channels = hDecoder->pce.num_lfe_channels;

        chdir = hInfo->num_front_channels;
        if (chdir & 1)
        {
            hInfo->channel_position[chpos++] = FRONT_CHANNEL_CENTER;
            chdir--;
        }
        for (i = 0; i < chdir; i += 2)
        {
            hInfo->channel_position[chpos++] = FRONT_CHANNEL_LEFT;
            hInfo->channel_position[chpos++] = FRONT_CHANNEL_RIGHT;
        }

    } else {
        switch (hDecoder->channelConfiguration)
        {
        case 1:
            hInfo->num_front_channels = 1;
            hInfo->channel_position[0] = FRONT_CHANNEL_CENTER;
            break;
        case 2:
            hInfo->num_front_channels = 2;
            hInfo->channel_position[0] = FRONT_CHANNEL_LEFT;
            hInfo->channel_position[1] = FRONT_CHANNEL_RIGHT;
            break;
        default: /* channelConfiguration == 0 || channelConfiguration > 7 */
            {
                uint8_t i;
                uint8_t ch = hDecoder->fr_channels - hDecoder->has_lfe;
                if (ch & 1) /* there's either a center front or a center back channel */
                {
                    uint8_t ch1 = (ch-1)/2;
                    if (hDecoder->first_syn_ele == ID_SCE)
                    {
                        hInfo->num_front_channels = ch1 + 1;
                        hInfo->channel_position[0] = FRONT_CHANNEL_CENTER;
                        for (i = 1; i <= ch1; i+=2)
                        {
                            hInfo->channel_position[i] = FRONT_CHANNEL_LEFT;
                            hInfo->channel_position[i+1] = FRONT_CHANNEL_RIGHT;
                        }

                    } else {
                        hInfo->num_front_channels = ch1;
                        for (i = 0; i < ch1; i+=2)
                        {
                            hInfo->channel_position[i] = FRONT_CHANNEL_LEFT;
                            hInfo->channel_position[i+1] = FRONT_CHANNEL_RIGHT;
                        }

                        hInfo->channel_position[ch-1] = BACK_CHANNEL_CENTER;
                    }
                } else {
                    uint8_t ch1 = (ch)/2;
                    hInfo->num_front_channels = ch1;
                    if (ch1 & 1)
                    {
                        hInfo->channel_position[0] = FRONT_CHANNEL_CENTER;
                        for (i = 1; i <= ch1; i+=2)
                        {
                            hInfo->channel_position[i] = FRONT_CHANNEL_LEFT;
                            hInfo->channel_position[i+1] = FRONT_CHANNEL_RIGHT;
                        }

                    } else {
                        for (i = 0; i < ch1; i+=2)
                        {
                            hInfo->channel_position[i] = FRONT_CHANNEL_LEFT;
                            hInfo->channel_position[i+1] = FRONT_CHANNEL_RIGHT;
                        }

                    }
                }
                
            }
            break;
        }
    }
}

void AACAPI faacDecDecode(faacDecHandle hDecoder,
                            faacDecFrameInfo *hInfo,
                            uint8_t *buffer, uint32_t buffer_size)
{
    uint8_t channels = 0;
    uint8_t output_channels = 0;
    bitfile ld;
    uint32_t bitsconsumed;
    uint16_t frame_len;
    uint32_t sr;

    /* safety checks */
    if ((hDecoder == 0) || (hInfo == 0) || (buffer == 0))
    {
        return ;
    }

    frame_len = hDecoder->frameLength;

    MEMSET(hInfo, 0, sizeof(faacDecFrameInfo));
    MEMSET(hDecoder->internal_channel, 0, MAX_CHANNELS*sizeof(hDecoder->internal_channel[0]));

    /* initialize the bitstream */
    aac_initbits(&ld, buffer, buffer_size);

    if (hDecoder->adts_header_present)
    {
        adts_header adts;

        adts.old_format = hDecoder->config.useOldADTSFormat;
        if ((hInfo->error = adts_frame(&adts, &ld)) > 0)
            return;

        /* MPEG2 does byte_alignment() here,
         * but ADTS header is always multiple of 8 bits in MPEG2
         * so not needed to actually do it.
         */
    }

    /* decode the complete bitstream */
    raw_data_block(hDecoder, hInfo, &ld, &hDecoder->pce, hDecoder->drc);

    channels = hDecoder->fr_channels;

    /* safety check */
    if (channels == 0 || channels > MAX_CHANNELS)
    {
        /* invalid number of channels */
        hInfo->error = 12;
        PRINTF("channels: %d\r\n", channels);
        //return;
    }

    /* no more bit reading after this */
    bitsconsumed = aac_get_processed_bits(&ld);
    hInfo->bytesconsumed = bit2byte(bitsconsumed);
    if (ld.error)
    {
        hInfo->error = 14;
        return;
    }

    if (!hDecoder->adts_header_present && !hDecoder->adif_header_present)
    {
        if (channels != hDecoder->channelConfiguration)
            hDecoder->channelConfiguration = channels;
    }

    output_channels = channels;

    /* Make a channel configuration based on either a PCE or a channelConfiguration */
    create_channel_config(hDecoder, hInfo);

    /* number of samples in this frame */
    hInfo->samples = frame_len*output_channels;
    /* number of channels in this frame */
    hInfo->channels = output_channels;
    /* samplerate */
    sr = get_sample_rate(hDecoder->sf_index);
    if(hDecoder->sbr_present_flag == 1)
        sr = sr*2;        
    hInfo->samplerate = sr;
    /* object type */
    hInfo->object_type = hDecoder->object_type;
    /* sbr */
    hInfo->sbr = NO_SBR;
    /* header type */
    hInfo->header_type = RAW;
    if (hDecoder->adif_header_present)
        hInfo->header_type = ADIF;
    if (hDecoder->adts_header_present)
        hInfo->header_type = ADTS;

    /* check if frame has channel elements */
    if (channels == 0)
    {
        hDecoder->frame++;
        return ;
    }

    hDecoder->postSeekResetFlag = 0;

    hDecoder->frame++;

    if (hDecoder->frame <= 1)
        hInfo->samples = 0;

    //2009-02-26 Eric modify : skip channel configuration and channel number check   
    if((hInfo->error == 21) || (hInfo->error == 12))
        hInfo->error = 0;    

}
#endif

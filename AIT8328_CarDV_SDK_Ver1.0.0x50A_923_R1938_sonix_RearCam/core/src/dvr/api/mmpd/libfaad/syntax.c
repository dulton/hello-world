/*
   Reads the AAC bitstream as defined in 14496-3 (MPEG-4 Audio)
*/
#include "aac_comm.h"
#include "structs.h"
#include "AACdec.h"
#include "syntax.h"
#include "specrec.h"
#include "huffman.h"
#include "bits.h"
//#include "pulse.h"
#include "drc.h"
#include "sbr_syntax.h"

#include "ait_utility.h"

#if (AAC_HOST_PARSER_EN == 1)
__align(4) int16_t spectrum1[1024]; 

/* static function declarations */
static void decode_sce_lfe(faacDecHandle hDecoder, faacDecFrameInfo *hInfo, bitfile *ld,
                           uint8_t id_syn_ele);
static void decode_cpe(faacDecHandle hDecoder, faacDecFrameInfo *hInfo, bitfile *ld,
                       uint8_t id_syn_ele);
static uint8_t single_lfe_channel_element(faacDecHandle hDecoder, bitfile *ld,
                                          uint8_t channel, uint8_t *tag);
static uint8_t channel_pair_element(faacDecHandle hDecoder, bitfile *ld,
                                    uint8_t channel, uint8_t *tag);

static uint16_t data_stream_element(faacDecHandle hDecoder, bitfile *ld);
static uint8_t program_config_element(program_config *pce, bitfile *ld);
static uint8_t fill_element(faacDecHandle hDecoder, bitfile *ld, drc_info *drc
#ifdef SBR_DEC
                            ,uint8_t sbr_ele);
#endif
                       
static uint8_t individual_channel_stream(faacDecHandle hDecoder, element *ele,
                                         bitfile *ld, ic_stream *ics, uint8_t scal_flag,
                                         int16_t *spec_data);
static uint8_t ics_info(faacDecHandle hDecoder, ic_stream *ics, bitfile *ld,
                        uint8_t common_window);
static uint8_t section_data(faacDecHandle hDecoder, ic_stream *ics, bitfile *ld);
static uint8_t scale_factor_data(faacDecHandle hDecoder, ic_stream *ics, bitfile *ld);

static uint8_t spectral_data(faacDecHandle hDecoder, ic_stream *ics, bitfile *ld,
                             int16_t *spectral_data);
static uint16_t extension_payload(bitfile *ld, drc_info *drc, uint16_t count);
static uint8_t pulse_data(ic_stream *ics, pulse_info *pul, bitfile *ld);
static void tns_data(ic_stream *ics, tns_info *tns, bitfile *ld);
static uint8_t adts_fixed_header(adts_header *adts, bitfile *ld);
static void adts_variable_header(adts_header *adts, bitfile *ld);
static void adts_error_check(adts_header *adts, bitfile *ld);
static uint8_t dynamic_range_info(bitfile *ld, drc_info *drc);
static uint8_t excluded_channels(bitfile *ld, drc_info *drc);


/* Table 4.4.2 */
/* An MPEG-4 Audio decoder is only required to follow the Program
   Configuration Element in GASpecificConfig(). The decoder shall ignore
   any Program Configuration Elements that may occur in raw data blocks.
   PCEs transmitted in raw data blocks cannot be used to convey decoder
   configuration information.
*/
static uint8_t program_config_element(program_config *pce, bitfile *ld)
{
    uint32_t i;
    uint8_t  temp, num_side_channel_elements, num_back_channel_elements, num_lfe_channel_elements;
    uint8_t  num_assoc_data_elements, num_valid_cc_elements;

    MEMSET(pce, 0, sizeof(program_config));

    pce->channels = 0;

    pce->element_instance_tag = (uint8_t)aac_getbits(ld, 4);

    pce->object_type = (uint8_t)aac_getbits(ld, 2);

    pce->sf_index = (uint8_t)aac_getbits(ld, 4);

    pce->num_front_channel_elements = (uint8_t)aac_getbits(ld, 4);

    num_side_channel_elements = (uint8_t)aac_getbits(ld, 4);

    num_back_channel_elements = (uint8_t)aac_getbits(ld, 4);

    num_lfe_channel_elements = (uint8_t)aac_getbits(ld, 2);

    num_assoc_data_elements = (uint8_t)aac_getbits(ld, 3);

    num_valid_cc_elements = (uint8_t)aac_getbits(ld, 4);


    pce->mono_mixdown_present = aac_get1bit(ld);
    if (pce->mono_mixdown_present == 1)
    {
        pce->mono_mixdown_element_number = (uint8_t)aac_getbits(ld, 4);
    }

    pce->stereo_mixdown_present = aac_get1bit(ld);

    if (pce->stereo_mixdown_present == 1)
    {
        pce->stereo_mixdown_element_number = (uint8_t)aac_getbits(ld, 4);
    }
#if 1
    temp = aac_get1bit(ld);
    if (temp == 1)
    {
        aac_getbits(ld, 3);
    }    
#else    
    pce->matrix_mixdown_idx_present = aac_get1bit(ld);

    if (pce->matrix_mixdown_idx_present == 1)
    {
        pce->matrix_mixdown_idx = (uint8_t)aac_getbits(ld, 2);
        pce->pseudo_surround_enable = aac_get1bit(ld);
    }
#endif
    for (i = 0; i < pce->num_front_channel_elements; i++)
    {
        pce->front_element_is_cpe[i] = aac_get1bit(ld);
        pce->front_element_tag_select[i] = (uint8_t)aac_getbits(ld, 4);

        if (pce->front_element_is_cpe[i] & 1)
        {
            pce->cpe_channel[pce->front_element_tag_select[i]] = pce->channels;
            pce->num_front_channels += 2;
            pce->channels += 2;
        } else {
            pce->sce_channel[pce->front_element_tag_select[i]] = pce->channels;
            pce->num_front_channels++;
            pce->channels++;
        }
    }

#if 1
    for (i = 0; i < num_side_channel_elements; i++)
    {
        aac_getbits(ld, 5);
    }

    for (i = 0; i < num_back_channel_elements; i++)
    {
        aac_getbits(ld, 5);
    }

    for (i = 0; i < num_lfe_channel_elements; i++)
    {
        aac_getbits(ld, 4);
    }

#else
    for (i = 0; i < pce->num_side_channel_elements; i++)
    {
        pce->side_element_is_cpe[i] = aac_get1bit(ld);
        pce->side_element_tag_select[i] = (uint8_t)aac_getbits(ld, 4);

        if (pce->side_element_is_cpe[i] & 1)
        {
            pce->cpe_channel[pce->side_element_tag_select[i]] = pce->channels;
            pce->num_side_channels += 2;
            pce->channels += 2;
        } else {
            pce->sce_channel[pce->side_element_tag_select[i]] = pce->channels;
            pce->num_side_channels++;
            pce->channels++;
        }
    }

    for (i = 0; i < pce->num_back_channel_elements; i++)
    {
        pce->back_element_is_cpe[i] = aac_get1bit(ld);
        pce->back_element_tag_select[i] = (uint8_t)aac_getbits(ld, 4);

        if (pce->back_element_is_cpe[i] & 1)
        {
            pce->cpe_channel[pce->back_element_tag_select[i]] = pce->channels;
            pce->channels += 2;
            pce->num_back_channels += 2;
        } else {
            pce->sce_channel[pce->back_element_tag_select[i]] = pce->channels;
            pce->num_back_channels++;
            pce->channels++;
        }
    }

    for (i = 0; i < pce->num_lfe_channel_elements; i++)
    {
        pce->lfe_element_tag_select[i] = (uint8_t)aac_getbits(ld, 4);

        pce->sce_channel[pce->lfe_element_tag_select[i]] = pce->channels;
        pce->num_lfe_channels++;
        pce->channels++;
    }
#endif

    for (i = 0; i < num_assoc_data_elements; i++)
        aac_getbits(ld, 4);


    for (i = 0; i < num_valid_cc_elements; i++)
        aac_getbits(ld, 5);
    

    aac_byte_align(ld);

//    pce->comment_field_bytes = (uint8_t)aac_getbits(ld, 8);
    temp = aac_getbits(ld, 8);

//    for (i = 0; i < pce->comment_field_bytes; i++)
    for (i = 0; i < temp; i++)
    {
//        pce->comment_field_data[i] = (uint8_t)aac_getbits(ld, 8);
        aac_getbits(ld, 8);
    }
//    pce->comment_field_data[i] = 0;

    if (pce->channels > MAX_CHANNELS)
        return 22;

    return 0;
}

static void decode_sce_lfe(faacDecHandle hDecoder,
                           faacDecFrameInfo *hInfo, bitfile *ld,
                           uint8_t id_syn_ele)
{
    uint8_t channels = hDecoder->fr_channels;
    uint8_t tag = 0;

    if (channels+1 > MAX_CHANNELS)
    {
        hInfo->error = 12;
        return;
    }
    if (hDecoder->fr_ch_ele+1 > MAX_SYNTAX_ELEMENTS)
    {
        hInfo->error = 13;
        return;
    }

    /* for SCE hDecoder->element_output_channels[] is not set here because this
       can become 2 when some form of Parametric Stereo coding is used
    */

    /* save the syntax element id */
    hDecoder->element_id[hDecoder->fr_ch_ele] = id_syn_ele;

    /* decode the element */
    hInfo->error = single_lfe_channel_element(hDecoder, ld, channels, &tag);

    /* map output channels position to internal data channels */
    if (hDecoder->element_output_channels[hDecoder->fr_ch_ele] == 2)
    {
        /* this might be faulty when pce_set is true */
        hDecoder->internal_channel[channels] = channels;
        hDecoder->internal_channel[channels+1] = channels+1;
    } else {
        if (hDecoder->pce_set)
            hDecoder->internal_channel[hDecoder->pce.sce_channel[tag]] = channels;
        else
            hDecoder->internal_channel[channels] = channels;
    }

    hDecoder->fr_channels += hDecoder->element_output_channels[hDecoder->fr_ch_ele];
    hDecoder->fr_ch_ele++;
}

static void decode_cpe(faacDecHandle hDecoder, faacDecFrameInfo *hInfo, bitfile *ld,
                       uint8_t id_syn_ele)
{
    uint8_t channels = hDecoder->fr_channels;
    uint8_t tag = 0;

    if (channels+2 > MAX_CHANNELS)
    {
        hInfo->error = 12;
        return;
    }
    if (hDecoder->fr_ch_ele+1 > MAX_SYNTAX_ELEMENTS)
    {
        hInfo->error = 13;
        return;
    }

    /* for CPE the number of output channels is always 2 */
    if (hDecoder->element_output_channels[hDecoder->fr_ch_ele] == 0)
    {
        /* element_output_channels not set yet */
        hDecoder->element_output_channels[hDecoder->fr_ch_ele] = 2;
    } else if (hDecoder->element_output_channels[hDecoder->fr_ch_ele] != 2) {
        /* element inconsistency */
        hInfo->error = 21;
        return;
    }

    /* save the syntax element id */
    hDecoder->element_id[hDecoder->fr_ch_ele] = id_syn_ele;

    /* decode the element */
    hInfo->error = channel_pair_element(hDecoder, ld, channels, &tag);

    /* map output channel position to internal data channels */
    if (hDecoder->pce_set)
    {
        hDecoder->internal_channel[hDecoder->pce.cpe_channel[tag]] = channels;
        hDecoder->internal_channel[hDecoder->pce.cpe_channel[tag]+1] = channels+1;
    } else {
        hDecoder->internal_channel[channels] = channels;
        hDecoder->internal_channel[channels+1] = channels+1;
    }

    hDecoder->fr_channels += 2;
    hDecoder->fr_ch_ele++;
}

void raw_data_block(faacDecHandle hDecoder, faacDecFrameInfo *hInfo,
                    bitfile *ld, program_config *pce, drc_info *drc)
{
    uint8_t id_syn_ele;

    hDecoder->fr_channels = 0;
    hDecoder->fr_ch_ele = 0;
    hDecoder->first_syn_ele = 25;
    hDecoder->has_lfe = 0;

        /* Table 4.4.3: raw_data_block() */
        while ((id_syn_ele = (uint8_t)aac_getbits(ld, LEN_SE_ID)) != ID_END)
        {
            switch (id_syn_ele) {
            case ID_SCE:
                if (hDecoder->first_syn_ele == 25) hDecoder->first_syn_ele = id_syn_ele;
                decode_sce_lfe(hDecoder, hInfo, ld, id_syn_ele);
                if (hInfo->error > 0)
                    return;
                break;
            case ID_CPE:
                if (hDecoder->first_syn_ele == 25) hDecoder->first_syn_ele = id_syn_ele;
                decode_cpe(hDecoder, hInfo, ld, id_syn_ele);
                if (hInfo->error > 0)
                    return;
                break;
            case ID_LFE:
                hDecoder->has_lfe++;
                decode_sce_lfe(hDecoder, hInfo, ld, id_syn_ele);
                if (hInfo->error > 0)
                    return;
                break;
            case ID_CCE: /* not implemented yet, but skip the bits */
                hInfo->error = 6;

                if (hInfo->error > 0)
                    return;
                break;
            case ID_DSE:
                data_stream_element(hDecoder, ld);
                break;
            case ID_PCE:
                /* 14496-4: 5.6.4.1.2.1.3: */
                /* program_configuration_element()'s in access units shall be ignored */
                program_config_element(pce, ld);
                //if ((hInfo->error = program_config_element(pce, ld)) > 0)
                //    return;
                //hDecoder->pce_set = 1;
                break;
            case ID_FIL:
                /* one sbr_info describes a channel_element not a channel! */
                /* if we encounter SBR data here: error */
                /* SBR data will be read directly in the SCE/LFE/CPE element */
//2009-02-26 eric modify            
//                if ((hInfo->error = fill_element(hDecoder, ld, drc
                if ((fill_element(hDecoder, ld, drc
#ifdef SBR_DEC
                    , 0
#endif
                    )) > 0)
                    return;
                break;
            }
        }

    /* new in corrigendum 14496-3:2002 */    
        aac_byte_align(ld);
    

    return;
}

/* Table 4.4.4 and */
/* Table 4.4.9 */
static uint8_t single_lfe_channel_element(faacDecHandle hDecoder, bitfile *ld,
                                          uint8_t channel, uint8_t *tag)
{
    uint8_t retval = 0;
    element sce = {0};
    ic_stream stream = {0};   
    ic_stream *ics = &stream;   
   
//    int8_t *temp = (int8_t *)ics1_ptr;
//    ic_stream *ics = (ic_stream *)temp;

//    ALIGN int16_t spec_data[1024] = {0};
    int16_t *spec_data = (int16_t*)&spectrum1[0];

    sce.element_instance_tag = (uint8_t)aac_getbits(ld, LEN_TAG);

    *tag = sce.element_instance_tag;
    sce.channel = channel;
    sce.paired_channel = -1;
    sce.ics1 = (ic_stream *)&ics; 
    
    MEMSET(spec_data, 0, 2048);

    retval = individual_channel_stream(hDecoder, &sce, ld, ics, 0, spec_data);
    if (retval > 0)
        return retval;

    /* noiseless coding is done, spectral reconstruction is done now */
    retval = reconstruct_single_channel(hDecoder, ics, &sce, spec_data);
    if (retval > 0)
        return retval;

    return 0;
}

/* Table 4.4.5 */
static uint8_t channel_pair_element(faacDecHandle hDecoder, bitfile *ld,
                                    uint8_t channels, uint8_t *tag)
{
//    int16_t spec_data1[1024] = {0};
//    int16_t spec_data2[1024] = {0};

    int16_t *spec_data1 = (int16_t*)&spectrum1[0];
//    int16_t *spec_data2 = (int16_t*)&spectrum2[0];    

    element cpe = {0};
    
    ic_stream test1, test2 = {0};
    
//    int8_t  *temp1 = (int8_t *)ics1_ptr;    
//    ic_stream *ics1 = (ic_stream *)temp1;
    ic_stream *ics1 = (ic_stream *)&test1;
    ic_stream *ics2 = (ic_stream *)&test2;
    uint8_t result;
    uint8_t *temp_ptr;

//    cpe.ics1 = (ic_stream *)ics1_ptr;
    cpe.ics1 = (ic_stream *)ics1;
    cpe.ics2 = (ic_stream *)ics2;

    cpe.channel        = channels;
    cpe.paired_channel = channels+1;

    cpe.element_instance_tag = (uint8_t)aac_getbits(ld, LEN_TAG);

    *tag = cpe.element_instance_tag;

    MEMSET(spec_data1,0, 2048);

    if ((cpe.common_window = aac_get1bit(ld)) & 1)
    {
        /* both channels have common ics information */
        if ((result = ics_info(hDecoder, ics1, ld, cpe.common_window)) > 0)
            return result;

        ics1->ms_mask_present = (uint8_t)aac_getbits(ld, 2);

        if (ics1->ms_mask_present == 1)
        {
            uint8_t g, sfb;
            for (g = 0; g < ics1->num_window_groups; g++)
            {
                temp_ptr = (uint8_t *)&ics1->ms_used[g][0];
                for (sfb = 0; sfb < ics1->max_sfb; sfb++)
                {
                    *temp_ptr++ = aac_get1bit(ld);
                }
            }
        }

        MEMCPY(ics2, ics1, sizeof(ic_stream));
    } else {
        ics1->ms_mask_present = 0;
    }

    if ((result = individual_channel_stream(hDecoder, &cpe, ld, ics1,
        0, spec_data1)) > 0)
    {
        return result;
    }

    if ((result = individual_channel_stream(hDecoder, &cpe, ld, ics2,
        0, spec_data1)) > 0)
    {
        return result;
    }

    /* noiseless coding is done, spectral reconstruction is done now */
//    if ((result = reconstruct_channel_pair(hDecoder, ics1, ics2, &cpe,
//        spec_data1, spec_data2)) > 0)
//    {
//        return result;
//    }

    return 0;
}

/* Table 4.4.6 */
static uint8_t ics_info(faacDecHandle hDecoder, ic_stream *ics, bitfile *ld,
                        uint8_t common_window)
{
    uint8_t retval = 0, temp;

    /* ics->ics_reserved_bit = */ aac_get1bit(ld);
    ics->window_sequence = (uint8_t)aac_getbits(ld, 2);
    ics->window_shape = aac_get1bit(ld);

    if (ics->window_sequence == EIGHT_SHORT_SEQUENCE)
    {
        ics->max_sfb = (uint8_t)aac_getbits(ld, 4);
        ics->scale_factor_grouping = (uint8_t)aac_getbits(ld, 7);
    } else {
        ics->max_sfb = (uint8_t)aac_getbits(ld, 6);
    }

    /* get the grouping information */
    if ((retval = window_grouping_info(hDecoder, ics)) > 0)
        return retval;

    /* should be an error */
    /* check the range of max_sfb */
    if (ics->max_sfb > ics->num_swb)
        return 16;

    if (ics->window_sequence != EIGHT_SHORT_SEQUENCE)
    {
        if ((ics->predictor_data_present = aac_get1bit(ld)) & 1)
        {
            if (hDecoder->object_type == MAIN) /* MPEG2 style AAC predictor */
            {
                uint8_t sfb;

                temp = min(ics->max_sfb, max_pred_sfb(hDecoder->sf_index));

                if (aac_get1bit(ld) & 1)
                {
                    aac_getbits(ld, 5);
                }

                for (sfb = 0; sfb < temp; sfb++)
                {
                    aac_get1bit(ld);
                }
            }
        }
    }

    return retval;
}

/* Table 4.4.7 */
static uint8_t pulse_data(ic_stream *ics, pulse_info *pul, bitfile *ld)
{
    uint8_t i;
    uint8_t *temp_ptr1, *temp_ptr2;

    pul->number_pulse = (uint8_t)aac_getbits(ld, 2);

    pul->pulse_start_sfb = (uint8_t)aac_getbits(ld, 6);

    /* check the range of pulse_start_sfb */
    if (pul->pulse_start_sfb > ics->num_swb)
        return 16;

    temp_ptr1 = (uint8_t *)&pul->pulse_offset[0];
    temp_ptr2 = (uint8_t *)&pul->pulse_amp[0];    
    for (i = 0; i < pul->number_pulse+1; i++)
    {
        *temp_ptr1++ = (uint8_t)aac_getbits(ld, 5);
        *temp_ptr2++ = (uint8_t)aac_getbits(ld, 4);
    }

    return 0;
}


/* Table 4.4.10 */
static uint16_t data_stream_element(faacDecHandle hDecoder, bitfile *ld)
{
    uint8_t byte_aligned;
    uint32_t i, count;

    /* element_instance_tag = */ aac_getbits(ld, LEN_TAG);
    byte_aligned = aac_get1bit(ld);

    count = (uint16_t)aac_getbits(ld, 8);

    if (count == 255)
    {
        count += (uint16_t)aac_getbits(ld, 8);
    }
    if (byte_aligned)
        aac_byte_align(ld);

    for (i = 0; i < count; i++)
    {
        aac_getbits(ld, LEN_BYTE);
    }

    return count;
}
#define EXT_SBR_DATA     13
#define EXT_SBR_DATA_CRC 14

extern uint32_t m_ulAACPlusObjectType;

/* Table 4.4.11 */
static uint8_t fill_element(faacDecHandle hDecoder, bitfile *ld, drc_info *drc
#ifdef SBR_DEC
                            ,uint8_t sbr_ele)
#endif
{
    uint32_t count;
    uint8_t bs_extension_type;
    
    count = (uint32_t)aac_getbits(ld, 4);

    if (count == 15)
    {
        count += (uint16_t)aac_getbits(ld, 8) - 1;
    }
#if 0
    if (count > 0)
    {
            while (count > 0)
            {
                count -= extension_payload(ld, drc, count);
            }
    }
#else
    if (count > 0)
    {
        bs_extension_type = (uint8_t)aac_showbits(ld, 4);

        if ((bs_extension_type == EXT_SBR_DATA) ||
            (bs_extension_type == EXT_SBR_DATA_CRC))
        {
            if (!hDecoder->sbr[sbr_ele])
            {
                hDecoder->sbr[sbr_ele] = sbrDecodeInit(hDecoder->frameLength,
                    hDecoder->element_id[sbr_ele], 2*get_sample_rate(hDecoder->sf_index)
                    );
            }
//eric add
            hDecoder->sbr_present_flag = 1;
            m_ulAACPlusObjectType = 5;            // AACPLUS_V1
            /* parse the SBR data */
            hDecoder->sbr[sbr_ele]->ret = sbr_extension_data(ld, hDecoder->sbr[sbr_ele], count);
            if (hDecoder->sbr[sbr_ele]->ps_used)
            {
                hDecoder->ps_used[sbr_ele] = 1;   
                hDecoder->fr_channels = 2;
                m_ulAACPlusObjectType = 6;        // AACPLUS_V2
            }
        }
        else {
//2009-02-26 eric modify            
#if 0            
            while (count > 0)
            {
                count -= extension_payload(ld, drc, count);
            }
#else
            uint32_t i, data;
            
            data = aac_getbits(ld, 4);
            
            for(i=count; i!=1; i--)
                data = aac_getbits(ld, 8);    
            
#endif            
        }

    }

#endif
    return 0;
}


/* Table 4.4.24 */
static uint8_t individual_channel_stream(faacDecHandle hDecoder, element *ele,
                                         bitfile *ld, ic_stream *ics, uint8_t scal_flag,
                                         int16_t *spec_data)
{
    uint8_t result;

    ics->global_gain = (uint8_t)aac_getbits(ld, 8);

    if (!ele->common_window && !scal_flag)
    {
        if ((result = ics_info(hDecoder, ics, ld, ele->common_window)) > 0)
            return result;
    }

    if ((result = section_data(hDecoder, ics, ld)) > 0)
        return result;

    if ((result = scale_factor_data(hDecoder, ics, ld)) > 0)
        return result;

    if (!scal_flag)
    {
        /**
         **  NOTE: It could be that pulse data is available in scalable AAC too,
         **        as said in Amendment 1, this could be only the case for ER AAC,
         **        though. (have to check this out later)
         **/
        /* get pulse data */
        if ((ics->pulse_data_present = aac_get1bit(ld)) & 1)
        {
            if ((result = pulse_data(ics, &(ics->pul), ld)) > 0)
                return result;
        }

        /* get tns data */
        if ((ics->tns_data_present = aac_get1bit(ld)) & 1)
        {
                tns_data(ics, &(ics->tns), ld);
        }

        /* get gain control data */
        if ((ics->gain_control_data_present = aac_get1bit(ld)) & 1)
        {
            return 1;
        }
    }

        /* decode the spectral data */
        if ((result = spectral_data(hDecoder, ics, ld, spec_data)) > 0)
        {
            return result;
        }

    return 0;
}

/* Table 4.4.25 */
static uint8_t section_data(faacDecHandle hDecoder, ic_stream *ics, bitfile *ld)
{
    uint32_t g;
    uint8_t sect_esc_val, sect_bits;
    uint8_t *temp_ptr;

    if (ics->window_sequence == EIGHT_SHORT_SEQUENCE)
        sect_bits = 3;
    else
        sect_bits = 5;
    sect_esc_val = (1<<sect_bits) - 1;

#if 0
    printf("\ntotal sfb %d\n", ics->max_sfb);
    printf("   sect    top     cb\n");
#endif

    for (g = 0; g < ics->num_window_groups; g++)
    {
        uint32_t k = 0;
        uint32_t i = 0;

        while (k < ics->max_sfb)
        {
            uint32_t sfb;
            uint8_t sect_len_incr;
            uint16_t sect_len = 0;
            uint8_t sect_cb_bits = 4;

            /* if "aac_getbits" detects error and returns "0", "k" is never
               incremented and we cannot leave the while loop */
            if ((ld->error != 0) || (ld->no_more_reading))
                return 14;

            ics->sect_cb[g][i] = (uint8_t)aac_getbits(ld, sect_cb_bits);

            if (ics->sect_cb[g][i] == NOISE_HCB)
                ics->noise_used = 1;

            sect_len_incr = (uint8_t)aac_getbits(ld, sect_bits);

            while ((sect_len_incr == sect_esc_val) /* &&
                (k+sect_len < ics->max_sfb)*/)
            {
                sect_len += sect_len_incr;
                sect_len_incr = (uint8_t)aac_getbits(ld, sect_bits);
            }

            sect_len += sect_len_incr;

            ics->sect_start[g][i] = k;
            ics->sect_end[g][i] = k + sect_len;

            if (k + sect_len >= 8*15)
                return 15;
            if (i >= 8*15)
                return 15;

            temp_ptr = (uint8_t *)& ics->sfb_cb[g][k];
            for (sfb = k; sfb < k + sect_len; sfb++)
                *temp_ptr++ = ics->sect_cb[g][i];

#if 0
            printf(" %6d %6d %6d\n",
                i,
                ics->sect_end[g][i],
                ics->sect_cb[g][i]);
#endif

            k += sect_len;
            i++;
        }
        ics->num_sec[g] = i;
    }

#if 0
    printf("\n");
#endif

    return 0;
}

/*
 *  decode_scale_factors()
 *   decodes the scalefactors from the bitstream
 */
/*
 * All scalefactors (and also the stereo positions and pns energies) are
 * transmitted using Huffman coded DPCM relative to the previous active
 * scalefactor (respectively previous stereo position or previous pns energy,
 * see subclause 4.6.2 and 4.6.3). The first active scalefactor is
 * differentially coded relative to the global gain.
 */
static uint8_t decode_scale_factors(ic_stream *ics, bitfile *ld)
{
    uint32_t g, sfb;
    int16_t t;
    int8_t noise_pcm_flag = 1;

    int16_t scale_factor = ics->global_gain;
    int16_t is_position = 0;
    int16_t noise_energy = ics->global_gain - 90;

    for (g = 0; g < ics->num_window_groups; g++)
    {
        for (sfb = 0; sfb < ics->max_sfb; sfb++)
        {
            switch (ics->sfb_cb[g][sfb])
            {
            case ZERO_HCB: /* zero book */
                ics->scale_factors[g][sfb] = 0;
                break;
            case INTENSITY_HCB: /* intensity books */
            case INTENSITY_HCB2:

                /* decode intensity position */
                t = huffman_scale_factor(ld);
                is_position += (t - 60);
                ics->scale_factors[g][sfb] = is_position;

                break;
            case NOISE_HCB: /* noise books */

                /* decode noise energy */
                if (noise_pcm_flag)
                {
                    noise_pcm_flag = 0;
                    t = (int16_t)aac_getbits(ld, 9) - 256;
                } else {
                    t = huffman_scale_factor(ld);
                    t -= 60;
                }
                noise_energy += t;
                ics->scale_factors[g][sfb] = noise_energy;

                break;
            default: /* spectral books */

                /* ics->scale_factors[g][sfb] must be between 0 and 255 */

                ics->scale_factors[g][sfb] = 0;

                /* decode scale factor */
                t = huffman_scale_factor(ld);
                scale_factor += (t - 60);
                if (scale_factor < 0 || scale_factor > 255)
                    return 4;
                ics->scale_factors[g][sfb] = scale_factor;

                break;
            }
        }
    }

    return 0;
}

/* Table 4.4.26 */
static uint8_t scale_factor_data(faacDecHandle hDecoder, ic_stream *ics, bitfile *ld)
{
    uint8_t ret = 0;

    ret = decode_scale_factors(ics, ld);

    return ret;
}

/* Table 4.4.27 */
static void tns_data(ic_stream *ics, tns_info *tns, bitfile *ld)
{
    uint32_t w, filt, i;
    uint8_t start_coef_bits, coef_bits;
    uint8_t n_filt_bits = 2;
    uint8_t length_bits = 6;
    uint8_t order_bits = 5;
    uint8_t *n_filt_ptr, *coef_res_ptr, *length_ptr, *order_ptr, *direction_ptr, *comp_ptr, *coef_ptr;
    uint8_t nfilt_w, tns_order;

    if (ics->window_sequence == EIGHT_SHORT_SEQUENCE)
    {
        n_filt_bits = 1;
        length_bits = 4;
        order_bits = 3;
    }
    
    n_filt_ptr = (uint8_t *)&tns->n_filt[0];
    coef_res_ptr = (uint8_t *)&tns->coef_res[0]; 
    for (w = 0; w < ics->num_windows; w++)
    {
        nfilt_w = (uint8_t)aac_getbits(ld, n_filt_bits);
        *n_filt_ptr++ = nfilt_w;

        if (nfilt_w)
        {
            if ((*coef_res_ptr++ = aac_get1bit(ld)) & 1)
            {
                start_coef_bits = 4;
            } else {
                start_coef_bits = 3;
            }
        }

        length_ptr = (uint8_t *)&tns->length[w][0];
        order_ptr = (uint8_t *)&tns->order[w][0];
        direction_ptr = (uint8_t *)&tns->direction[w][0];
        comp_ptr = (uint8_t *)&tns->coef_compress[w][0];
        for (filt = 0; filt < nfilt_w; filt++)
        {
            *length_ptr++ = (uint8_t)aac_getbits(ld, length_bits);
            
            tns_order = (uint8_t)aac_getbits(ld, order_bits);
            *order_ptr++ = tns_order;

            if (tns_order)
            {
                *direction_ptr++ = aac_get1bit(ld);
                *comp_ptr++ = aac_get1bit(ld);

                coef_bits = start_coef_bits - tns->coef_compress[w][filt];
                
                coef_ptr = (uint8_t *)&tns->coef[w][filt][0];
                for (i = 0; i < tns_order; i++)
                {
                    *coef_ptr++ = (uint8_t)aac_getbits(ld, coef_bits);
                }
            }
        }
    }
}


/* Table 4.4.29 */
static uint8_t spectral_data(faacDecHandle hDecoder, ic_stream *ics, bitfile *ld,
                             int16_t *spectral_data)
{
    int32_t i;
    uint32_t g;
    uint32_t inc, k, p = 0;
    uint8_t groups = 0;
    uint8_t sect_cb;
    uint8_t result;
    uint16_t nshort = hDecoder->frameLength/8;

    for(g = 0; g < ics->num_window_groups; g++)
    {
        p = groups*nshort;

        for (i = 0; i < ics->num_sec[g]; i++)
        {
            sect_cb = ics->sect_cb[g][i];

            inc = (sect_cb >= FIRST_PAIR_HCB) ? 2 : 4;

            switch (sect_cb)
            {
            case ZERO_HCB:
            case NOISE_HCB:
            case INTENSITY_HCB:
            case INTENSITY_HCB2:
                p += (ics->sect_sfb_offset[g][ics->sect_end[g][i]] -
                    ics->sect_sfb_offset[g][ics->sect_start[g][i]]);
                break;
            default:
                for (k = ics->sect_sfb_offset[g][ics->sect_start[g][i]];
                     k < ics->sect_sfb_offset[g][ics->sect_end[g][i]]; k += inc)
                {
                    if ((result = huffman_spectral_data(sect_cb, ld, &spectral_data[p])) > 0)
                        return result;
                    p += inc;
                }
                break;
            }
        }
        groups += ics->window_group_length[g];
    }

    return 0;
}

/* Table 4.4.30 */
static uint16_t extension_payload(bitfile *ld, drc_info *drc, uint16_t count)
{
    uint16_t i, n, dataElementLength;
    uint8_t dataElementLengthPart;
    uint8_t align = 4, data_element_version, loopCounter;

    uint8_t extension_type = (uint8_t)aac_getbits(ld, 4);

    switch (extension_type)
    {
    case EXT_DYNAMIC_RANGE:
        drc->present = 1;
        n = dynamic_range_info(ld, drc);
        return n;
    case EXT_FILL_DATA:
        /* fill_nibble = */ aac_getbits(ld, 4); /* must be ?000?*/
        for (i = 0; i < count-1; i++)
        {
            /* fill_byte[i] = */ aac_getbits(ld, 8); /* must be ?0100101?*/
        }
        return count;
    case EXT_DATA_ELEMENT:
        data_element_version = (uint8_t)aac_getbits(ld, 4);

        switch (data_element_version)
        {
        case ANC_DATA:
            loopCounter = 0;
            dataElementLength = 0;
            do {
                dataElementLengthPart = (uint8_t)aac_getbits(ld, 8);
                dataElementLength += dataElementLengthPart;
                loopCounter++;
            } while (dataElementLengthPart == 255);

            for (i = 0; i < dataElementLength; i++)
            {
                /* data_element_byte[i] = */ aac_getbits(ld, 8);

                return (dataElementLength+loopCounter+1);
            }
        default:
            align = 0;
        }
    case EXT_FIL:
    default:
        aac_getbits(ld, align);
  
        for (i = 0; i < count-1; i++)
        {
            /* other_bits[i] = */ aac_getbits(ld, 8);
        }
        return count;
    }
}

/* Table 4.4.31 */
static uint8_t dynamic_range_info(bitfile *ld, drc_info *drc)
{
    uint32_t i, n = 1;
    uint8_t band_incr;

    drc->num_bands = 1;

    if (aac_get1bit(ld) & 1 )
    {
        drc->pce_instance_tag = (uint8_t)aac_getbits(ld, 4);

        /* drc->drc_tag_reserved_bits = */ aac_getbits(ld, 4);

        n++;
    }

    drc->excluded_chns_present = aac_get1bit(ld);

    if (drc->excluded_chns_present == 1)
    {
        n += excluded_channels(ld, drc);
    }

    if (aac_get1bit(ld) & 1 )
    {
        band_incr = (uint8_t)aac_getbits(ld, 4);

        /* drc->drc_bands_reserved_bits = */ aac_getbits(ld, 4);

        n++;
        drc->num_bands += band_incr;

        for (i = 0; i < drc->num_bands; i++);
        {
            drc->band_top[i] = (uint8_t)aac_getbits(ld, 8);
            n++;
        }
    }

    if (aac_get1bit(ld) & 1)
    {
        drc->prog_ref_level = (uint8_t)aac_getbits(ld, 7);

        /* drc->prog_ref_level_reserved_bits = */ aac_get1bit(ld);

        n++;
    }

    for (i = 0; i < drc->num_bands; i++)
    {
        drc->dyn_rng_sgn[i] = aac_get1bit(ld);

        drc->dyn_rng_ctl[i] = (uint8_t)aac_getbits(ld, 7);

        n++;
    }

    return n;
}

/* Table 4.4.32 */
static uint8_t excluded_channels(bitfile *ld, drc_info *drc)
{
    uint32_t i, n = 0;
    uint8_t num_excl_chan = 7;

    for (i = 0; i < 7; i++)
    {
        drc->exclude_mask[i] = aac_get1bit(ld);
    }
    n++;

    while ((drc->additional_excluded_chns[n-1] = aac_get1bit(ld)) == 1)
    {
        for (i = num_excl_chan; i < num_excl_chan+7; i++)
        {
            drc->exclude_mask[i] = aac_get1bit(ld);
        }
        n++;
        num_excl_chan += 7;
    }

    return n;
}

/* Annex A: Audio Interchange Formats */

/* Table 1.A.2 */
void get_adif_header(adif_header *adif, bitfile *ld)
{
    uint8_t i;

    /* adif_id[0] = */ aac_getbits(ld, 8);
    
    /* adif_id[1] = */ aac_getbits(ld, 8);

    /* adif_id[2] = */ aac_getbits(ld, 8);

    /* adif_id[3] = */ aac_getbits(ld, 8);
    adif->copyright_id_present = aac_get1bit(ld);

    if(adif->copyright_id_present)
    {
        for (i = 0; i < 72/8; i++)
        {
            adif->copyright_id[i] = (int8_t)aac_getbits(ld, 8);
        }
        adif->copyright_id[i] = 0;
    }
    adif->original_copy  = aac_get1bit(ld);

    adif->home = aac_get1bit(ld);

    adif->bitstream_type = aac_get1bit(ld);

    adif->bitrate = aac_getbits(ld, 23);

    adif->num_program_config_elements = (uint8_t)aac_getbits(ld, 4);


    for (i = 0; i < adif->num_program_config_elements + 1; i++)
    {
        if(adif->bitstream_type == 0)
        {
            adif->adif_buffer_fullness = aac_getbits(ld, 20);
        } else {
            adif->adif_buffer_fullness = 0;
        }

        program_config_element(&adif->pce[i], ld);
    }
}

/* Table 1.A.5 */
uint8_t adts_frame(adts_header *adts, bitfile *ld)
{
    /* aac_byte_align(ld); */
    if (adts_fixed_header(adts, ld))
        return 5;
    adts_variable_header(adts, ld);
    adts_error_check(adts, ld);

    return 0;
}

/* Table 1.A.6 */
static uint8_t adts_fixed_header(adts_header *adts, bitfile *ld)
{
    uint16_t i;
    uint8_t sync_err = 1;

    /* try to recover from sync errors */
    for (i = 0; i < 768; i++)
    {
        adts->syncword = (uint16_t)aac_showbits(ld, 12);
        if (adts->syncword != 0xFFF)
        {
            aac_getbits(ld, 8);
        } else {
            sync_err = 0;
            aac_getbits(ld, 12);

            break;
        }
    }
    if (sync_err)
        return 5;

    adts->id = aac_get1bit(ld);

    adts->layer = (uint8_t)aac_getbits(ld, 2);

    adts->protection_absent = aac_get1bit(ld);

    adts->profile = (uint8_t)aac_getbits(ld, 2);

    adts->sf_index = (uint8_t)aac_getbits(ld, 4);

    adts->private_bit = aac_get1bit(ld);

    adts->channel_configuration = (uint8_t)aac_getbits(ld, 3);

    adts->original = aac_get1bit(ld);

    adts->home = aac_get1bit(ld);


    if (adts->old_format == 1)
    {
        /* Removed in corrigendum 14496-3:2002 */
        if (adts->id == 0)
        {
            adts->emphasis = (uint8_t)aac_getbits(ld, 2);
        }
    }

    return 0;
}

/* Table 1.A.7 */
static void adts_variable_header(adts_header *adts, bitfile *ld)
{
    adts->copyright_identification_bit = aac_get1bit(ld);

    adts->copyright_identification_start = aac_get1bit(ld);

    adts->aac_frame_length = (uint16_t)aac_getbits(ld, 13);

    adts->adts_buffer_fullness = (uint16_t)aac_getbits(ld, 11);

    adts->no_raw_data_blocks_in_frame = (uint8_t)aac_getbits(ld, 2);

}

/* Table 1.A.8 */
static void adts_error_check(adts_header *adts, bitfile *ld)
{
    if (adts->protection_absent == 0)
    {
        adts->crc_check = (uint16_t)aac_getbits(ld, 16);
    }
}
#endif

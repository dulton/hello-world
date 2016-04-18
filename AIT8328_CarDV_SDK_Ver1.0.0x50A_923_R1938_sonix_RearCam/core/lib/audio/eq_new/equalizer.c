/*==================================================================================*/
/*      3-band adjustable audio equalizer engine                                    */
/*                                                                                  */
/*      Company : Alpha Imaging Technology Co. Ltd.                                 */
/*      Version : Fixed point V1.0                                                  */
/*      Date    : 2007-2-27                                                        */
/*      Auther  : Eric Huang                                                        */
/*==================================================================================*/

#include "equalizer.h"
#include "eq_coef.h"

// dB value
short BandVol_db[NUM_OF_BAND] = {0,0,0};
short BandVol_db_norm[NUM_OF_BAND];

//Pointer for all EQ filter
int *pEQFilter[MAX_EQ_NUM];

//Pool of EQ filter bank
__align(4) char WorkRAM[MAX_EQ_NUM * FILTER_BANK_SIZE];

int norm;

extern short VOL_TABLE[40];

int *eq_init(int eq_index, short vol_idx)
{
    int *pFilter;
    int FilterAddr;
    int max_db, i, start;
    // Filter gain value
    int FilterGain[NUM_OF_BAND];
    int gVolume;

    start = (int)&WorkRAM[0];
    gVolume = VOL_TABLE[vol_idx];

    if(eq_index > MAX_EQ_NUM)
        return (int *)-1;
    FilterAddr = start + eq_index * FILTER_BANK_SIZE;

    pFilter = (int *)FilterAddr;

    for(i=0; i<NUM_OF_BAND; i++)
        BandVol_db[i] = 0;

    max_db = 0;
    norm = GainTab[-max_db + GAIN_TAB_OFFSET];

    for(i=0; i<NUM_OF_BAND; i++)
        FilterGain[i] = 0;


    //Init filters
//3 bands
    pFilter[0]  = Filter_62Hz[20];   pFilter[1]  = Filter_62Hz[21];     pFilter[2] =  FilterGain[0];
    pFilter[3]  = Filter_125Hz[20];  pFilter[4]  = Filter_125Hz[21];    pFilter[5] =  FilterGain[1];
    pFilter[6]  = Filter_2000Hz[20]; pFilter[7]  = Filter_2000Hz[21];   pFilter[8] =  FilterGain[2];
    pFilter[9] =  gVolume;           pFilter[10] = norm;

    return pFilter;
}


int eq_setting(int *pFilter, int filter_idx0, int band_gain0, int filter_idx1, int band_gain1, int filter_idx2, int band_gain2)
{
    int max_db, i;
    const int *pBand;
    int filter_idx[3];
    // Filter gain value
    int FilterGain[NUM_OF_BAND];

    // Set center frequency for each band
    filter_idx[0] = filter_idx0;
    filter_idx[1] = filter_idx1;
    filter_idx[2] = filter_idx2;

    // Set dB gain for selected band
    BandVol_db[0] = band_gain0;
    BandVol_db[1] = band_gain1;
    BandVol_db[2] = band_gain2;

    // Re-calculate norm
    max_db = -1000;
    for(i=0; i<NUM_OF_BAND; i++)
    {
       if(BandVol_db[i] > max_db)
        max_db = BandVol_db[i];
     }

    norm = GainTab[-max_db + GAIN_TAB_OFFSET];

    // Re-calculate FilterGain[i] for each band
    for(i=0; i<NUM_OF_BAND; i++)
    {

       BandVol_db_norm[i] = BandVol_db[i] - max_db;
       if(!BandVol_db_norm[i])
            FilterGain[i] = (short)(32767 - norm);
       else
           FilterGain[i] = GainTab[BandVol_db_norm[i] + GAIN_TAB_OFFSET] - norm;

     }

    for(i=0; i<NUM_OF_BAND; i++)
    {
        pBand = EQFilter[filter_idx[i]>>4];
        pBand += 2*(filter_idx[i] & 0x0f);

        //Set selected filter
        pFilter[3*i] = pBand[0];
        pFilter[3*i+1]  = pBand[1];

        //Update the normalized gain for each band
        pFilter[3*i+2] =  FilterGain[i];
    }
    pFilter[3*NUM_OF_BAND+1] = norm;

    return 0;
}

int eq_set_one_band(int *pFilter, int band_idx, int filter_idx, int band_gain)
{
    int max_db, i;
    const int *pBand;
    // Filter gain value
    int FilterGain[NUM_OF_BAND];

    if(band_idx > 2)
        return -1;
    else
    // Set dB gain for selected band
        BandVol_db[band_idx] = band_gain;

    // Re-calculate norm
    max_db = -1000;
    for(i=0; i<NUM_OF_BAND; i++)
    {
       if(BandVol_db[i] > max_db)
        max_db = BandVol_db[i];
     }

    norm = GainTab[-max_db + GAIN_TAB_OFFSET];

    // Re-calculate FilterGain[i] for each band
    for(i=0; i<NUM_OF_BAND; i++)
    {

       BandVol_db_norm[i] = BandVol_db[i] - max_db;
       if(!BandVol_db_norm[i])
           FilterGain[i] = (short)(32767 - norm);
       else
           FilterGain[i] = GainTab[BandVol_db_norm[i] + GAIN_TAB_OFFSET] - norm;

     }

    pBand = EQFilter[filter_idx >> 4];
    pBand += 2*(filter_idx & 0x0f);
    //Set selected filter
    pFilter[3*band_idx] = pBand[0];
    pFilter[3*band_idx+1]  = pBand[1];

    for(i=0; i<NUM_OF_BAND; i++)
    {
        //Update the normalized gain for each band
        pFilter[3*i+2] =  FilterGain[i];
    }
    pFilter[3*NUM_OF_BAND+1] = norm;

    return 0;
}


//Default EQ setting
void eq_default_setting(short volume)
{
        // EQ_NONE :
        pEQFilter[EQ_NONE] = eq_init(EQ_NONE, volume);
        eq_setting(pEQFilter[EQ_NONE], BAND_125Hz+Q_1_41, 0, BAND_1000Hz+Q_1_41, 0, BAND_4000Hz+Q_1_41, 0);

        // EQ_CLASSIC :
        pEQFilter[EQ_CLASSIC] = eq_init(EQ_CLASSIC, volume);
        // band 0: 125Hz, +4db / band 1: 1KHz, 0db / band 2: 8KHz, +6db
        eq_setting(pEQFilter[EQ_CLASSIC], BAND_125Hz+Q_1_41, 4, BAND_1000Hz+Q_1_41, 0, BAND_8000Hz+Q_1_41, 6);

        // EQ_JAZZ :
        pEQFilter[EQ_JAZZ] = eq_init(EQ_JAZZ, volume);
        // band 0: 125Hz, +8db / band 1: 1KHz, 1db / band 2: 8KHz, -3db
        eq_setting(pEQFilter[EQ_JAZZ], BAND_125Hz+Q_1_41, 8, BAND_1000Hz+Q_1_41, 1, BAND_8000Hz+Q_1_41, -3);

        // EQ_POP :
        pEQFilter[EQ_POP] = eq_init(EQ_POP, volume);
        // band 0: 125Hz, -4db / band 1: 1KHz, 3db / band 2: 8KHz, -4db
        eq_setting(pEQFilter[EQ_POP], BAND_125Hz+Q_1_41, -4, BAND_1000Hz+Q_1_41, 3, BAND_8000Hz+Q_1_41, -4);

        // EQ_ROCK :
        pEQFilter[EQ_ROCK] = eq_init(EQ_ROCK, volume);
        // band 0: 250Hz, 7db / band 1: 2KHz, -2db / band 2: 8KHz, 4db
        eq_setting(pEQFilter[EQ_ROCK], BAND_250Hz+Q_1_41, 7, BAND_2000Hz+Q_1_41, -2, BAND_16000Hz+Q_1_41, 6);

        // EQ_BASS3 :
        pEQFilter[EQ_BASS3] = eq_init(EQ_BASS3, volume);
        // band 0: 31Hz, 2db / band 1: 62Hz, 3db / band 2: 125Hz, 4db
        eq_setting(pEQFilter[EQ_BASS3], BAND_125Hz+Q_0_26, 6, BAND_1000Hz+Q_1_41, 0, BAND_8000Hz+Q_1_41, -2);

        // EQ_BASS9 :
        pEQFilter[EQ_BASS9] = eq_init(EQ_BASS9, volume);
        // band 0: 125Hz, 9db / band 1: 500Hz, 5db / band 2: 16KHz, 9db
        eq_setting(pEQFilter[EQ_BASS9], BAND_62Hz+Q_0_26, 9, BAND_1000Hz+Q_1_41, 5, BAND_16000Hz, 9);

}

void eq_set_volume(short Vol)
{
    int *pFilter;
    int i;
    int vol_idx;

    pFilter = pEQFilter[0];
    pFilter[3*NUM_OF_BAND] = VOL_TABLE[Vol];
    
    //2007-02-26 eric modify

  vol_idx = Vol;

    // Set volume for all EQ type
    for(i=1; i<MAX_EQ_NUM; i++)
    {
        pFilter = pEQFilter[i];
        pFilter[3*NUM_OF_BAND] = VOL_TABLE[vol_idx];
    }
}


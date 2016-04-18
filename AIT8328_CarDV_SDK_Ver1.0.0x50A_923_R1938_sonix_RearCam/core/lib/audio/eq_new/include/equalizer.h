#ifndef EQUALIZER_H
#define EQUALIZER_H

#define ARM_OPT             1

#define NUM_OF_CHANNEL      2
#define NUM_OF_BAND         10
#define GAIN_STEPS          40
#define FILTER_BANK_SIZE    (NUM_OF_BAND*3)*sizeof(int)


//#define GAIN_FAST           0x4000
//#define GAIN_FAST1          (0x00008000 - GAIN_FAST)
#define GAIN_FAST           0x1800
#define GAIN_FAST1          (0x00008000 - GAIN_FAST)
//#define GAIN_SLOW           0x0010
//#define GAIN_SLOW1          0x7ff0
#define GAIN_SLOW           0x0008
#define GAIN_SLOW1          (0x00008000 - GAIN_SLOW)

#define LIMIT_TH            32400L

//Center frequency of each band
#define BAND_60Hz       0x00
#define BAND_170Hz      0x01
#define BAND_310Hz      0x02
#define BAND_600Hz      0x03
#define BAND_1000Hz     0x04
#define BAND_3000Hz     0x05
#define BAND_6000Hz     0x06
#define BAND_12000Hz    0x07
#define BAND_1400Hz     0x08
#define BAND_1600Hz     0x09


//Q range
#define Q_115_4         0x01                 // Q = 115.4152  , BW = 0.0125 octave  
#define Q_57_7          0x02                 // Q = 57.70707  , BW = 0.025 octave
#define Q_28_8          0x03                 // Q = 28.85245  , BW = 0.05 octave
#define Q_14_4          0x04                 // Q = 14.42406  , BW = 0.1 octave
#define Q_7_20          0x05                 // Q = 7.207702  , BW = 0.2 octave
#define Q_4_80          0x06                 // Q = 4.800330  , BW = 0.3 octave
#define Q_3_59          0x07                 // Q = 3.595210  , BW = 0.4 octave
#define Q_2_87          0x08                 // Q = 2.870999  , BW = 0.5 octave
#define Q_2_38          0x09                 // Q = 2.387250  , BW = 0.6 octave
#define Q_2_04          0x0a                 // Q = 2.040914  , BW = 0.7 octave
#define Q_1_78          0x0b                 // Q = 1.780469  , BW = 0.8 octave
#define Q_1_57          0x0c                 // Q = 1.577293  , BW = 0.9 octave
#define Q_1_41          0x0d                 // Q = 1.414213  , BW = 1.0 octave
#define Q_0_66          0x0e                 // Q = 0.666667  , BW = 2.0 octave
#define Q_0_004         0x0f                 // Q = 0.004040  , BW = 3.0 octave
#define Q_0_003         0x10                 // Q = 0.266667  , BW = 4.0 octave

//Gain value
#define DECAY_20dB      0
#define DECAY_19dB      1
#define DECAY_18dB      2
#define DECAY_17dB      3
#define DECAY_16dB      4
#define DECAY_15dB      5
#define DECAY_14dB      6
#define DECAY_13dB      7
#define DECAY_12dB      8
#define DECAY_11dB      9
#define DECAY_10dB      10
#define DECAY_9dB       11
#define DECAY_8dB       12
#define DECAY_7dB       13
#define DECAY_6dB       14
#define DECAY_5dB       15
#define DECAY_4dB       16
#define DECAY_3dB       17
#define DECAY_2dB       18
#define DECAY_1dB       19
#define NO_GAIN         20
#define GAIN_1dB        21
#define GAIN_2dB        22
#define GAIN_3dB        23
#define GAIN_4dB        24
#define GAIN_5dB        25
#define GAIN_6dB        26
#define GAIN_7dB        27
#define GAIN_8dB        28
#define GAIN_9dB        29
#define GAIN_10dB       30
#define GAIN_11dB       31
#define GAIN_12dB       32
#define GAIN_13dB       33
#define GAIN_14dB       34
#define GAIN_15dB       35
#define GAIN_16dB       36
#define GAIN_17dB       37
#define GAIN_18dB       38
#define GAIN_19dB       39
#define GAIN_20dB       40




int     *MMPF_AUDIO_GraphicEqInit(void);
void    MMPF_AUDIO_SetGraphicEqVol(int volume);
int     MMPF_AUDIO_SetEQBand(int *pFilter, int band_idx, int Q_idx, int Gain_idx);
void    MMPF_AUDIO_Graphic_EQ(short *inBuffer, short *outBuffer, int *pFilter, int numSamples);


#endif
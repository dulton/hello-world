/*
   individual channel stream info
 */

#ifndef CHANNELINFO_H
#define CHANNELINFO_H

#include "aacplus_defines.h"
#include "aacplus_overlapadd.h"
#include "aacplus_pulsedata.h"
#include "aacplus_streaminfo.h"
#include "aacplus_pns.h"
#include "aacplus_FFR_bitbuffer.h"



#define MAX_WINDOWS   8
#define MAX_SFB_LONG  64
#define MAX_SFB_SHORT 16
#define MAX_QUANTIZED_VALUE 8191
// Eric add for MAIN profile support
#define MAX_SFB       51

enum
{
  OnlyLongSequence = 0,
  LongStartSequence,
  EightShortSequence,
  LongStopSequence,

  ProfileMain = 0,
  ProfileLowComplexity,
  ProfileSSR,
  ProfileReserved
};

typedef struct
{
  Word16 CommonWindow;
  Word16 ElementInstanceTag;
  Word16 GlobalGain;
} CRawDataInfo;

// Eric add for MAIN profile support
typedef struct
{
    UWord8 limit;
    UWord8 predictor_reset;
    UWord8 predictor_reset_group_number;
    UWord8 prediction_used[MAX_SFB];
} CPredInfo;

typedef struct
{
  Word16 Valid;

  Word16 IcsReservedBit;
  Word16 WindowShape;
  Word16 WindowSequence;
  Word16 MaxSfBands;
  Word16 ScaleFactorGrouping;

  Word16 TotalSfBands;
  Word16 SamplingRateIndex;
  Word16 Profile;

  Word16 WindowGroups;
  Word16 WindowGroupLength[8];

// Eric add for MAIN profile support
//  Word16 swb_offset[52];
  Word16 PredictorDataPresent;  
  UWord16 swb_offset_max;
  UWord8  sfb_cb[8][8*15];
  CPredInfo PredInfo;
  
} CIcsInfo;

enum
{
  JointStereoMaximumGroups = 8,
  JointStereoMaximumBands = 64
};

typedef struct
{
  Word16 MsMaskPresent;
  Flag MsUsed[JointStereoMaximumBands]; /*!< every byte contains flags for up to 8 groups */
} CJointStereoData;

typedef struct
{
  Word16 StartBand;
  Word16 StopBand;

  Word16 Direction;
  Word16 Resolution;

  Word16 Order;
  Word16 Coeff[MaximumOrder];
} CFilter;

typedef struct
{
  Flag TnsDataPresent;
  Word16 NumberOfFilters[MaximumWindows];
  CFilter Filter[MaximumWindows][MaximumFilters];
} CTnsData;

typedef struct
{
  const Word32 *pLongWindow[2];
  const Word32 *pShortWindow[2];

  COverlapAddData OverlapAddData;

} CAacDecoderStaticChannelInfo;

typedef struct
{
  Word16 aSpecScale[MAX_WINDOWS];
  Word16 aSfbScale[MAX_WINDOWS * MAX_SFB_SHORT];
  Word16 aScaleFactor[MAX_WINDOWS * MAX_SFB_SHORT];
  Word8  aCodeBook[MAX_WINDOWS * MAX_SFB_SHORT];
} CAacDecoderDynamicData;

typedef struct
{
  CJointStereoData JointStereoData; /*! Common MS-mask for a channel-pair */
  CPnsInterChannelData PnsInterChannelData;
} CAacDecoderDynamicCommonData;

// Eric add for MAIN profile support 
/* used to save the prediction state */
typedef struct {
    Word16 r[2];
    Word16 COR[2];
    Word16 VAR[2];
} CPredData;
typedef struct
{
  Word16 *pSpecScale;
  Word16 *pSfbScale;
  Word16 *pScaleFactor;
  Word8  *pCodeBook;
  Word32 *pSpectralCoefficient;

  CIcsInfo IcsInfo;
  CTnsData TnsData;
  CPulseData PulseData;
  CRawDataInfo RawDataInfo;
  CJointStereoData * pJointStereoData;

  CPnsData PnsData;
  CPnsInterChannelData       *pPnsInterChannelData;
  CPnsStaticInterChannelData *pPnsStaticInterChannelData;
// Eric add for MAIN profile support 
  CPredData                  *pIntraChannelPredictData;
} CAacDecoderChannelInfo;

Word16 IcsRead(HANDLE_BIT_BUF bs,CIcsInfo *pIcsInfo);
void IcsReset(CIcsInfo *pIcsInfo, CStreamInfo *pStreamInfo);

Word16 IsLongBlock(CIcsInfo *pIcsInfo);
//Word16 IsShortBlock(CIcsInfo *pIcsInfo);
//Word16 IsMainProfile(CIcsInfo *pIcsInfo);
Word16  GetWindowsPerFrame(CIcsInfo *pIcsInfo);
const Word16 *GetScaleFactorBandOffsets(CIcsInfo *pIcsInfo);
//Word32 SamplingRateFromIndex(Word16 index);
//Word16 GetFreqLine (CIcsInfo *pIcsInfo, Word8 sfb);
//Word32 GetSamplingFrequency(CIcsInfo *pIcsInfo);
Word16 GetMaximumTnsBands(CIcsInfo *pIcsInfo);


#endif /* #ifndef CHANNELINFO_H */

/*
   pulse data tool
 */

#ifndef PULSEDATA_H
#define PULSEDATA_H

#include "aacplus_bitstream.h"
#include "aacplus_FFR_bitbuffer.h"


enum
{
  MaximumLines = 4
};

typedef struct
{
  Flag  PulseDataPresent;
  short NumberPulse;
  short PulseStartBand;
  short PulseOffset[MaximumLines];
  short PulseAmp[MaximumLines];
} CPulseData;

void CPulseData_Read(HANDLE_BIT_BUF bs,
                     CPulseData *PulseData);

void CPulseData_Apply(CPulseData *PulseData,
                      const short *pScaleFactorBandOffsets,
                      short *coef);


#endif /* #ifndef PULSEDATA_H */

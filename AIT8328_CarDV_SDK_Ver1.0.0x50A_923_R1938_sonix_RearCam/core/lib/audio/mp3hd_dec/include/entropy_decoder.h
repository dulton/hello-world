#ifndef _entropy_decoder_h
#define _entropy_decoder_h

/**************************  mp3HD Codec  **************************************
*                                                                              *
*                                (C) copyright                                 *
*              Fraunhofer-Institut für Digitale Medientechnologie              *
*                    Deutsche Thomson Hannover                                 *
*                                    (2009)                                    *
*                             All Rights Reserved                              *
*                                                                              *
*                                                                              *
*   filename             :  entropy_common.h                                   *
*   project              :  mp3HD  Codec                                       *
*   date                 :  2009-01-19                                         *
*   author               :  Peter Steinborn                                    *
*                                                                              *
*   contents/description :  Lossless entropy coding module (Golomb Coding)     *
*                                                                              *
*******************************************************************************/

/* standard includes */
#include "entropy_common.h"
#include "bitStreamR.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**
* TYPE DEFINITIONS
*/

typedef struct LosslessDecoder LOSSLESS_DECODER;  /**< Generates a type for struct LosslessDecoder */
typedef LOSSLESS_DECODER * H_LOSSLESS_DECODER;    /**< Generates a type for a handle on type LOSSLESS_DECODER */

/**
* STRUCTURES
*/

/**
* \brief Struct for the Entropy Decoder
*/
struct LosslessDecoder{
    H_GOLOMB_BUFFER hGolombDecoder;   /**< Handle on struct GolombBuffer, set to 0x0 if not needed */
};

/*
* PROTOTYPES
*/

/**
* Init function for entropy decoder
*
* \param bufferLength determines the size of the internal buffer
* \param blockSize amount of input samples per block
* \retval *hhLosslessDecoder handle on handle to struct LosslessDecoder
*/
BOOL          InitLosslessDecoder
(H_LOSSLESS_DECODER *hhLosslessDecoder /* handle on handle to struct LosslessDecoder */
);

/**
* Destroy function for entropy decoder. Deallocates all the memory used for the struct LosslessDecoder
*
* \param hLosslessDecoder handle on struct LosslessDecoder
*/
void          DestroyLosslessDecoder
(H_LOSSLESS_DECODER hLosslessDecoder  /* handle on struct LosslessDecoder */);


/************************************** DECODER ****************************************/
/**
* Main decoding function (called blockwise)
*
* \param hLosslessDecoder handle on struct LosslessDecoder
* \param inputChars pointer to array of input bytes
* \retval sampleArray pointer to array of bufferLength output samples
* \param Golomb parameter struct
*/
BOOL          LosslessDecode
(H_LOSSLESS_DECODER hLosslessDecoder,   /* handle on struct LosslessDecoder */
 BITBUFFER *inputChars,
 FIXP_DBL *sampleArray                      /* poiner to array of bufferLength output samples */
);

/**
* Set entropy decoder to fixed starting values
*
* \param myBuffer handle on struct Golomb buffer
*/
BOOL ResetEntropyDecoder(H_GOLOMB_BUFFER myBuffer);

#ifdef __cplusplus
}
#endif /*__cplusplus*/
#endif


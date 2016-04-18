#ifndef RV_BACKEND_TYPES_H
#define RV_BACKEND_TYPES_H

#include "rv_format_info.h"

#ifdef __cplusplus
extern "C" {
#endif  /* #ifdef __cplusplus */

typedef struct rv_backend_init_params_struct
{
    UINT16 usOuttype;
    UINT16 usPels;
    UINT16 usLines;
    UINT16 usPadWidth;   /* number of columns of padding on right to get 16 x 16 block*/
    UINT16 usPadHeight;  /* number of rows of padding on bottom to get 16 x 16 block*/

    UINT16 pad_to_32;   /* to keep struct member alignment independent of */
                        /* compiler options */
    UINT32 ulInvariants;/* ulInvariants specifies the invariant picture header bits */
    INT32  bPacketization;
    UINT32 ulStreamVersion;
} rv_backend_init_params;

typedef struct rv_backend_in_params_struct
{
    UINT32 dataLength;
    INT32  bInterpolateImage;
    UINT32 numDataSegments;
    rv_segment *pDataSegments;
    UINT32 flags;
    UINT32 timestamp;
} rv_backend_in_params;

typedef struct rv_backend_out_params_struct
{
    UINT32 numFrames;
    UINT32 notes;
    UINT32 timestamp;
    UINT32 width;
    UINT32 height;
} rv_backend_out_params;


/* definitions for output parameter notes */
#define RV_DECODE_MORE_FRAMES           0x00000001
#define RV_DECODE_DONT_DRAW             0x00000002
#define RV_DECODE_KEY_FRAME             0x00000004
#define RV_DECODE_B_FRAME               0x00000008
#define RV_DECODE_DEBLOCKING_FILTER     0x00000010
#define RV_DECODE_FRU_FRAME             0x00000020
#define RV_DECODE_SCRAMBLED_BUFFER      0x00000040
#define RV_DECODE_LAST_FRAME            0x00000200

/* definitions for decoding opaque data in bitstream header */
/* Defines match ilvcmsg.h so that ulSPOExtra == rv10init.invariants */
#define RV40_SPO_FLAG_UNRESTRICTEDMV        0x00000001  /* ANNEX D */
#define RV40_SPO_FLAG_EXTENDMVRANGE         0x00000002  /* IMPLIES NEW VLC TABLES */
#define RV40_SPO_FLAG_ADVMOTIONPRED         0x00000004  /* ANNEX F */
#define RV40_SPO_FLAG_ADVINTRA              0x00000008  /* ANNEX I */
#define RV40_SPO_FLAG_INLOOPDEBLOCK         0x00000010  /* ANNEX J */
#define RV40_SPO_FLAG_SLICEMODE             0x00000020  /* ANNEX K */
#define RV40_SPO_FLAG_SLICESHAPE            0x00000040  /* 0: free running; 1: rect */
#define RV40_SPO_FLAG_SLICEORDER            0x00000080  /* 0: sequential; 1: arbitrary */
#define RV40_SPO_FLAG_REFPICTSELECTION      0x00000100  /* ANNEX N */
#define RV40_SPO_FLAG_INDEPENDSEGMENT       0x00000200  /* ANNEX R */
#define RV40_SPO_FLAG_ALTVLCTAB             0x00000400  /* ANNEX S */
#define RV40_SPO_FLAG_MODCHROMAQUANT        0x00000800  /* ANNEX T */
#define RV40_SPO_FLAG_BFRAMES               0x00001000  /* SETS DECODE PHASE */
#define RV40_SPO_BITS_DEBLOCK_STRENGTH      0x0000e000  /* deblocking strength */
#define RV40_SPO_BITS_NUMRESAMPLE_IMAGES    0x00070000  /* max of 8 RPR images sizes */
#define RV40_SPO_FLAG_FRUFLAG               0x00080000  /* FRU BOOL: if 1 then OFF; */
#define RV40_SPO_FLAG_FLIP_FLIP_INTL        0x00100000  /* FLIP-FLOP interlacing; */
#define RV40_SPO_FLAG_INTERLACE             0x00200000  /* de-interlacing prefilter has been applied; */
#define RV40_SPO_FLAG_MULTIPASS             0x00400000  /* encoded with multipass; */
#define RV40_SPO_FLAG_INV_TELECINE          0x00800000  /* inverse-telecine prefilter has been applied; */
#define RV40_SPO_FLAG_VBR_ENCODE            0x01000000  /* encoded using VBR; */
#define RV40_SPO_BITS_DEBLOCK_SHIFT            13
#define RV40_SPO_BITS_NUMRESAMPLE_IMAGES_SHIFT 16

#define OUT_OF_DATE_DECODER                 0x00000001
#define OK_VERSION                          0x00000000

#define CORRUPTED_BITSTREAM                 0x00
#define OK_DECODE                           0x0f
#define INCOMPLETE_FRAME                    0xffff
#define MALLOC_FAILURE                      0x1111

#define RV10_DITHER_PARAMS                  0x00001001
#define RV10_POSTFILTER_PARAMS              0x00001002
#define RV10_ADVANCED_MP_PARAMS             0x0001003
#define RV10_TEMPORALINTERP_PARAMS          0x00001004

#ifdef __cplusplus
}
#endif  /* #ifdef __cplusplus */

#endif /* RV_BACKEND_TYPES_H */

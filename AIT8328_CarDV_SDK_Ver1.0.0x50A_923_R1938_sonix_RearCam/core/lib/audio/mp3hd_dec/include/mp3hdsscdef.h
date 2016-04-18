/***************************************************************************\
 *
 *               (C) copyright Fraunhofer - IIS (1998)
 *               (C) copyright Thomson Hannover (2008)
 *                        All Rights Reserved
 *
 *   $Id$
 *   filename: mp3hdsscdef.h
 *   project : ---
 *   author  : Peter Stienborn
 *   date    : 2008-12-01
 *   contents/description: ssc definitions (Structured Status Code)
 *
 *
\***************************************************************************/

#ifndef __MP3HDSSCDEF_H__
#define __MP3HDSSCDEF_H__

#include "machine_type.h"
#include "mp3sscdef.h"

  /* typedef signed INT INT; */

/*------------------------- includes --------------------------------------*/

/*-------------------------- defines --------------------------------------*/

/*-------------------------------------------------------------------------*\
 *
 *  Standard error/return values are 32 bit values layed out as follows:
 *
 *   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
 *   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
 *  +---+-+-+-----------------------+-------------------------------+
 *  |Sev|C|R|     Handler           |               Code            |
 *  +---+-+-+-----------------------+-------------------------------+
 *
 *  where
 *
 *      Sev - is the severity code
 *
 *          00 - Success
 *          01 - Informational
 *          10 - Warning
 *          11 - Error
 *
 *      C       - is the Customer code flag
 *
 *      R       - is a reserved bit
 *
 *      Handler - is the handler code
 *
 *      Code    - is the facility's status code
 *
\*-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------*\
 *
 *  SSC classes (handler)
 *
\*-------------------------------------------------------------------------*/
/* defined in mp3sscdef.h
#define SSC_HANDLER_GEN 0x00000000L

#define SSC_I_GEN (SSC_SEV_INFO    | SSC_HANDLER_GEN)
#define SSC_W_GEN (SSC_SEV_WARNING | SSC_HANDLER_GEN)
#define SSC_E_GEN (SSC_SEV_ERROR   | SSC_HANDLER_GEN)
*/
/*-------------------------------------------------------------------------*/

/* defined in mp3sscdef.h
#define SSC_HANDLER_IO 0x00010000L

#define SSC_I_IO (SSC_SEV_INFO    | SSC_HANDLER_IO)
#define SSC_W_IO (SSC_SEV_WARNING | SSC_HANDLER_IO)
#define SSC_E_IO (SSC_SEV_ERROR   | SSC_HANDLER_IO)
*/
/*-------------------------------------------------------------------------*/

/* defined in mp3sscdef.h
#define SSC_HANDLER_MPGA 0x01010000L

#define SSC_I_MPGA (SSC_SEV_INFO    | SSC_HANDLER_MPGA)
#define SSC_W_MPGA (SSC_SEV_WARNING | SSC_HANDLER_MPGA)
#define SSC_E_MPGA (SSC_SEV_ERROR   | SSC_HANDLER_MPGA)
*/
/*-------------------------------------------------------------------------*/

#define SSC_HANDLER_CD 0x01100000L

#define SSC_I_CD (SSC_SEV_INFO    | SSC_HANDLER_CD)
#define SSC_W_CD (SSC_SEV_WARNING | SSC_HANDLER_CD)
#define SSC_E_CD (SSC_SEV_ERROR   | SSC_HANDLER_CD)

/*-------------------------------------------------------------------------*/

#define SSC_HANDLER_HD 0x01110000L

#define SSC_I_HD (SSC_SEV_INFO    | SSC_HANDLER_HD)
#define SSC_W_HD (SSC_SEV_WARNING | SSC_HANDLER_HD)
#define SSC_E_HD (SSC_SEV_ERROR   | SSC_HANDLER_HD)


/*-------------------------------------------------------------------------*/

#define SSC_HANDLER_ENCGEN 0x02000000L

#define SSC_I_ENCGEN (SSC_SEV_INFO    | SSC_HANDLER_ENCGEN)
#define SSC_W_ENCGEN (SSC_SEV_WARNING | SSC_HANDLER_ENCGEN)
#define SSC_E_ENCGEN (SSC_SEV_ERROR   | SSC_HANDLER_ENCGEN)

/*-------------------------------------------------------------------------*/

#define SSC_HANDLER_ENCMP3HD 0x02020000L

#define SSC_I_ENCMP3HD (SSC_SEV_INFO    | SSC_HANDLER_ENCMP3HD)
#define SSC_W_ENCMP3HD (SSC_SEV_WARNING | SSC_HANDLER_ENCMP3HD)
#define SSC_E_ENCMP3HD (SSC_SEV_ERROR   | SSC_HANDLER_ENCMP3HD)

/*-------------------------------------------------------------------------*/

#define SSC_HANDLER_ENCMP3 0x02220000L

#define SSC_I_ENCMP3 (SSC_SEV_INFO    | SSC_HANDLER_ENCMP3)
#define SSC_W_ENCMP3 (SSC_SEV_WARNING | SSC_HANDLER_ENCMP3)
#define SSC_E_ENCMP3 (SSC_SEV_ERROR   | SSC_HANDLER_ENCMP3)

/*-------------------------------------------------------------------------*\
 *
 *  SSC codes
 *
\*-------------------------------------------------------------------------*/


/* use define and not enum, because enum in some cases is only 16 bit wide */
/*
#define  SSC_OK                                     (0x00000000L          )
                                                    
#define  SSC_E_WRONGPARAMETER                       ((SSC_E_GEN  | 1L))
#define  SSC_E_OUTOFMEMORY                          ((SSC_E_GEN  | 2L))
#define  SSC_E_INVALIDHANDLE                        ((SSC_E_GEN  | 3L))
                                                    
#define  SSC_E_IO_GENERIC                           ((SSC_W_IO   | 1L))
#define  SSC_E_IO_OPENFAILED                        ((SSC_W_IO   | 2L))
#define  SSC_E_IO_CLOSEFAILED                       ((SSC_W_IO   | 3L))
#define  SSC_E_IO_READFAILED                        ((SSC_W_IO   | 4L))
                                                    
#define  SSC_I_MPGA_CRCERROR                        ((SSC_I_MPGA | 1L))
#define  SSC_I_MPGA_NOMAINDATA                      ((SSC_I_MPGA | 2L))
#define  SSC_I_MPGA_ANCNOBACKPNTR                   ((SSC_I_MPGA | 3L))
                                                    
#define  SSC_E_MPGA_GENERIC                         ((SSC_E_MPGA | 1L))
#define  SSC_E_MPGA_WRONGLAYER                      ((SSC_E_MPGA | 2L))
#define  SSC_E_MPGA_BUFFERTOOSMALL                  ((SSC_E_MPGA | 3L))
                                                    
#define  SSC_W_MPGA_SYNCSEARCHED                    ((SSC_W_MPGA | 1L))
#define  SSC_W_MPGA_SYNCLOST                        ((SSC_W_MPGA | 2L))
#define  SSC_W_MPGA_SYNCNEEDDATA                    ((SSC_W_MPGA | 3L))
#define  SSC_W_MPGA_SYNCEOF                         ((SSC_W_MPGA | 4L))
*/
#define  SSC_E_MPGA_INPUTBUFFERTOOSMALL             ((SSC_E_MPGA | 4L))

#define  SSC_W_GEN_LayerNotSupported                ((SSC_W_GEN  | 64L))

#define  SSC_E_CD_GENERIC                           ((SSC_E_CD | 1L))
#define  SSC_E_CD_NOID3                             ((SSC_E_CD | 2L))
#define  SSC_E_CD_WRONGLAYER                        ((SSC_E_CD | 3L))
#define  SSC_E_CD_BUFFERTOOSMALL                    ((SSC_E_CD | 4L))
#define  SSC_E_CD_WRONGDECODE                       ((SSC_E_CD | 5L))
#define  SSC_E_CD_ENDOFFILE                         ((SSC_E_CD | 6L))
#define  SSC_E_CD_WRONGHD3ID                        ((SSC_E_CD | 7L))

#define  SSC_E_CD_WRONGFORMAT                       ((SSC_E_CD | 10L))
#define  SSC_E_CD_NODECODERHANDLE                   ((SSC_E_CD | 11L))
#define  SSC_E_CD_CORRUPTFAT                        ((SSC_E_CD | 12L))
#define  SSC_E_CD_WRONGBUFFERSIZE                   ((SSC_E_CD | 13L))
#define  SSC_E_CD_WRONGSUPERFRAMESIZE               ((SSC_E_CD | 14L))
#define  SSC_E_CD_INITLOWROMMAPPING                 ((SSC_E_CD | 17L))
#define  SSC_E_CD_GETHD3IDDATAERROR                 ((SSC_E_CD | 18L))
#define  SSC_E_CD_WINDOWSHAPEFAILED                 ((SSC_E_CD | 19L))
#define  SSC_E_CD_WRONGDECODINGSTEREOSHEME          ((SSC_E_CD | 20L))
#define  SSC_E_CD_STREAMINGNOTSUPPORTED             ((SSC_E_CD | 21L))
#define  SSC_E_CD_IRREGULARSCRAMBLINGFLAG           ((SSC_E_CD | 22L))
#define  SSC_E_CD_GETMP3DATAFAILED                  ((SSC_E_CD | 23L))

#define  SSC_E_CD_WRONGSTREAMBUFFERCREATE           ((SSC_E_CD | 800L))
#define  SSC_E_CD_WRONGSTREAMPOSITIONSET            ((SSC_E_CD | 801L))
#define  SSC_E_CD_WRONGBITSTREAMPOS                 ((SSC_E_CD | 802L))
#define  SSC_E_CD_WRONGBITSTREAMREAD                ((SSC_E_CD | 803L))
#define  SSC_E_CD_BITSTREAMREADERROR                ((SSC_E_CD | 804L))
#define  SSC_E_CD_BITSTREAMREADHEADERERROR          ((SSC_E_CD | 805L))
#define  SSC_E_CD_INITLOSSLESSDECODER               ((SSC_E_CD | 806L))
#define  SSC_E_CD_STREAMINGMODEUNSUPPORTED          ((SSC_E_CD | 807L))
#define  SSC_E_CD_SUPERFRAMESIZEUNSUPPORTED         ((SSC_E_CD | 808L))
#define  SSC_E_CD_CANTREADMP3HEADER                 ((SSC_E_CD | 809L))
#define  SSC_E_CD_WRONG_REENTRYPOINT                ((SSC_E_CD | 810L))

#define  SSC_E_CD_MP3INFOFRAME_STRUCTNOTALLOCATED           ((SSC_E_CD | 900L))
#define  SSC_E_CD_MP3INFOFRAME_FIRSTMP3FRAMEBUFFERNOTVALID  ((SSC_E_CD | 901L)) 
#define  SSC_E_CD_MP3INFOFRAME_NOMP3FRAME                   ((SSC_E_CD | 902L))
#define  SSC_E_CD_MP3INFOFRAME_NOVALIDXINGHEADER            ((SSC_E_CD | 903L))
#define  SSC_E_CD_MP3INFOFRAME_NOMP3HDINFOFRAMEFOUND        ((SSC_E_CD | 904L))
#define  SSC_E_CD_MP3INFOFRAME_NOTSUPPORTEDFRAMEIDNUMBER    ((SSC_E_CD | 905L))

#define  SSC_E_CD_MP3INFOFRAME_FIRSTMP3FRAMESIZE            ((SSC_E_CD | 996L))
#define  SSC_E_CD_MP3INFOFRAME_FILEPOINTERISNULL            ((SSC_E_CD | 997L))
#define  SSC_E_CD_MP3INFOFRAME_FILEWRITEERROR               ((SSC_E_CD | 998L))
#define  SSC_E_CD_MP3INFOFRAME_FSEEKERROR                   ((SSC_E_CD | 999L))

#define  SSC_W_CD_GOLOMBDECODE                      ((SSC_W_CD | 1L))
#define  SSC_W_CD_WRONGSEEEKPOSITIONPOINTER         ((SSC_W_CD | 2L))
#define  SSC_W_CD_WRONGSEEEKPOSITIONPARAMETER       ((SSC_W_CD | 3L))  
#define  SSC_W_CD_MP3CRCUNSUPPORTED                 ((SSC_W_CD | 4L))
#define  SSC_W_CD_MP3INFOFRAME_REDUCEDCOMMENT       ((SSC_W_CD | 5L))                                                                                                       
#define  SSC_W_CD_INITFAILED                        ((SSC_W_CD | 6L))
#define  SSC_W_CD_TOOFEWBYTESCOPIED                 ((SSC_W_CD | 7L)) 
#define  SSC_W_CD_ID3TAGNOTSUPPORTED                ((SSC_W_CD | 8L))
#define  SSC_W_CD_INVALIDMP3HDID                    ((SSC_W_CD | 9L))
#define  SSC_W_CD_UNSUPPORTEDMP3HDSTREAM            ((SSC_W_CD | 10L))
#define  SSC_W_CD_CANTCREATELAYER                   ((SSC_W_CD | 11L))
#define  SSC_W_CD_NOID3TAGFOUND                     ((SSC_W_CD | 12L))
#define  SSC_W_CD_INVALIDMP3FRAME                   ((SSC_W_CD | 13L))
#define  SSC_W_CD_NOMP3INFOFRAME                    ((SSC_W_CD | 14L))
#define  SSC_W_CD_NOMP3HDIDFRAME                    ((SSC_W_CD | 15L))
#define  SSC_W_CD_WRONGMP3HDVERSION                 ((SSC_W_CD | 16L))
#define  SSC_W_CD_WRONGHASH                         ((SSC_W_CD | 17L))
#define  SSC_W_CD_SEEKINGNOTSUPPORTED               ((SSC_W_CD | 18L))
#define  SSC_W_CD_NOMP3HDSTREAM                     ((SSC_W_CD | 19L))
#define  SSC_W_CD_LAYERCANTBEMUTED                  ((SSC_W_CD | 20L))

#define  SSC_I_CD_SKIPFRAME                         ((SSC_I_CD | 1L))
#define  SSC_I_CD_ID3FRAMENOTFOUND                  ((SSC_I_CD | 2L))
#define  SSC_I_CD_GETNEXTMP3HDINFOFRAMEBUFFER       ((SSC_I_CD | 3L))
#define  SSC_I_CD_ENDOFCDLAYERREACHED               ((SSC_I_CD | 4L))
#define  SSC_I_CD_SUPERFRAMESIZEKNOWN               ((SSC_I_CD | 5L))
#define  SSC_I_CD_NOMP3FAT                          ((SSC_I_CD | 6L))
#define  SSC_I_CD_FUNCTIONNEEDSINITDONE             ((SSC_I_CD | 7L))
                                                    
#define  SSC_E_HD_GENERIC                           ((SSC_E_HD | 1L))
#define  SSC_E_HD_WRONGLAYER                        ((SSC_E_HD | 2L))
#define  SSC_E_HD_BUFFERTOOSMALL                    ((SSC_E_HD | 3L))

#define  SSC_W_HD_CANTINITLAYER                     ((SSC_W_HD | 1L))


/*-------------------------------------------------------------------------*\
 *
 *  SSC Encoder codes
 *
\*-------------------------------------------------------------------------*/


/* ----------- SSC_HANDLER_ENCGEN -----------------------------------------*/

/* ----------- SSC_I_ENCGEN -----------------------------------------------*/

/* ----------- SSC_W_ENCGEN -----------------------------------------------*/
#define SSC_W_ENCGEN_INVALID_ENCODER_STATE        ((SSC_W_ENCGEN | 1L))

/* ----------- SSC_E_ENCGEN -----------------------------------------------*/
#define SSC_E_ENCGEN_INVALID_FILE_POINTER         ((SSC_E_ENCGEN | 1L))
#define SSC_E_ENCGEN_INVALID_MP3HD_FILE           ((SSC_E_ENCGEN | 2L))
#define SSC_E_ENCGEN_TMP_FILE_WRITE_FAILED        ((SSC_E_ENCGEN | 3L))
#define SSC_E_ENCGEN_TMP_FILE_READ_FAILED         ((SSC_E_ENCGEN | 4L))
#define SSC_E_ENCGEN_TMP_FILE_IO_ERROR            ((SSC_E_ENCGEN | 5L))
#define SSC_E_ENCGEN_MEMORY_ALLOCATION_FAILED     ((SSC_E_ENCGEN | 6L))
#define SSC_E_ENCGEN_INVALID_OBJECT               ((SSC_E_ENCGEN | 7L))
#define SSC_E_ENCGEN_IO_WRITE_ERROR               ((SSC_E_ENCGEN | 8L))
#define SSC_E_ENCGEN_INVALID_PARAMETER            ((SSC_E_ENCGEN | 9L))


/*-------------------------------------------------------------------------*/

/* ----------- SSC_HANDLER_ENCMP3HD ---------------------------------------*/

/* ----------- SSC_I_ENCMP3HD ---------------------------------------------*/
#define SSC_I_ENCMP3HD_ALL_SAMPLES_READ              ((SSC_I_ENCMP3HD | 1L))

/* ----------- SSC_W_ENCMP3HD ---------------------------------------------*/
#define SSC_W_ENCMP3HD_MP3_BIT_RATE_NOT_SUPPORTED    ((SSC_W_ENCMP3HD | 1L))
#define SSC_W_ENCMP3HD_MP3_VBR_MODE_NOT_SUPPORTED    ((SSC_W_ENCMP3HD | 2L))
#define SSC_W_ENCMP3HD_FAT_EXISTS                    ((SSC_W_ENCMP3HD | 3L))
#define SSC_W_ENCMP3HD_FAT_REDUCED_ENTRIES           ((SSC_W_ENCMP3HD | 4L))
#define SSC_W_ENCMP3HD_FILE_TOO_SMALL_FOR_A_FAT      ((SSC_W_ENCMP3HD | 5L))
#define SSC_W_ENCMP3HD_ID3_TAG_IS_FULL               ((SSC_W_ENCMP3HD | 6L))
#define SSC_W_ENCMP3HD_INFO_COMMENT_SIZE_REDUCED     ((SSC_W_ENCMP3HD | 7L))
#define SSC_W_ENCMP3HD_MP3_BUFFER_DOESNT_START_WITH_SYNC ((SSC_W_ENCMP3HD | 8L))
#define SSC_W_ENCMP3HD_TOO_MANY_REMAINING_BYTES_IN_THE_INFILE ((SSC_W_ENCMP3HD | 9L))
#define SSC_W_ENCMP3HD_INVALID_ID3_FRAME_ID          ((SSC_W_ENCMP3HD | 10L))

/* ----------- SSC_E_ENCMP3HD ---------------------------------------------*/
#define SSC_E_ENCMP3HD_INVALID_SAMPLE_RESOLUTION     ((SSC_E_ENCMP3HD | 1L))
#define SSC_E_ENCMP3HD_INVALID_NUMBER_OF_CHANNELS    ((SSC_E_ENCMP3HD | 2L))
#define SSC_E_ENCMP3HD_INVALID_SAMPLE_RATE           ((SSC_E_ENCMP3HD | 3L))
#define SSC_E_ENCMP3HD_LAYER_INIT_FAILED             ((SSC_E_ENCMP3HD | 4L))
#define SSC_E_ENCMP3HD_INFOFRAME_INIT_FAILED         ((SSC_E_ENCMP3HD | 5L))
#define SSC_E_ENCMP3HD_NUMBER_OF_INPUT_SAMPLES_EXCEEDED ((SSC_E_ENCMP3HD | 6L))
#define SSC_E_ENCMP3HD_FILE_TOO_BIG_FOR_A_FAT        ((SSC_E_ENCMP3HD | 7L))
#define SSC_E_ENCMP3HD_CANT_OPEN_PCMQUEUE            ((SSC_E_ENCMP3HD | 8L))
#define SSC_E_ENCMP3HD_INVALID_STEREO_MODE           ((SSC_E_ENCMP3HD | 9L))
#define SSC_E_ENCMP3HD_MUTE_SAMPLES_TOO_BIG          ((SSC_E_ENCMP3HD | 10L))
#define SSC_E_ENCMP3HD_MUTE_FRAMES_TOO_BIG           ((SSC_E_ENCMP3HD | 11L))
#define SSC_E_ENCMP3HD_CANT_WRITE_MP3HD_HEADER       ((SSC_E_ENCMP3HD | 12L))
#define SSC_E_ENCMP3HD_CANT_WRITE_LAYER_HEADER       ((SSC_E_ENCMP3HD | 13L))
#define SSC_E_ENCMP3HD_CANT_FILL_INPUT_QUEUE         ((SSC_E_ENCMP3HD | 14L))
#define SSC_E_ENCMP3HD_CANT_ADD_FAT_ENTRY            ((SSC_E_ENCMP3HD | 15L))
#define SSC_E_ENCMP3HD_ENCODING_FAILED               ((SSC_E_ENCMP3HD | 16L))
#define SSC_E_ENCMP3HD_WRITING_FAT_FAILED            ((SSC_E_ENCMP3HD | 17L))
#define SSC_E_ENCMP3HD_ID3_TAG_SIZE_TOO_BIG          ((SSC_E_ENCMP3HD | 18L))
#define SSC_E_ENCMP3HD_CANT_WRITE_MP3_INFO_FRAME     ((SSC_E_ENCMP3HD | 19L))
#define SSC_E_ENCMP3HD_CANT_UPDATE_MP3HD_HEADER      ((SSC_E_ENCMP3HD | 20L))
#define SSC_E_ENCMP3HD_UPDATE_OF_ID3V2_TAG_FAILED    ((SSC_E_ENCMP3HD | 21L))
#define SSC_E_ENCMP3HD_ID3V2_INPUT_NOT_ACCEPTED      ((SSC_E_ENCMP3HD | 22L))
#define SSC_E_ENCMP3HD_MP3HD_ID3V2_FRAME_ID_EXISTS   ((SSC_E_ENCMP3HD | 23L))


/*-------------------------------------------------------------------------*/

/* ----------- SSC_HANDLER_ENCMP3 -----------------------------------------*/

/* ----------- SSC_I_ENCMP3 -----------------------------------------------*/
#define SSC_I_ENCMP3_NO_FREE_INPUT_BUFFER           (( SSC_I_ENCMP3 | 1L))

/* ----------- SSC_W_ENCMP3 -----------------------------------------------*/

/* ----------- SSC_E_ENCMP3 -----------------------------------------------*/
#define SSC_E_ENCMP3_INIT_FAILED                    (( SSC_E_ENCMP3 | 1L))
#define SSC_E_ENCMP3_ENCODING_ERROR                 (( SSC_E_ENCMP3 | 2L))
#define SSC_E_ENCMP3_WRONG_MP3SENCVERSION           (( SSC_E_ENCMP3 | 3L))

/*
#define SSC UINT
*/


/*-------------------------------------------------------------------------*/
#endif

#include "amr_include.h"

/*************************************************************************
*
*  Function:   AMRDec_Speech_Decode_Frame_init
*  Purpose:    initializes state memory and filter structure
*  In:         none
*  Out:        none
*
***************************************************************************/
extern	int AMRDec_Speech_Decode_Frame_init (void);

/*************************************************************************
*
*  Function:   AMRDec_UnpackBits
*  Purpose:    Unpack and re-arrange bits from file storage format to the
*              format required by speech decoder.
*  In:         as below
*  Out:        received frame type
*
***************************************************************************/
extern	enum RXFrameType AMRDec_UnpackBits (
    signed char  q,              /* i : Q-bit (i.e. BFI)        */
	short ft,             /* i : frame type (i.e. mode)  */
    unsigned char packed_bits[],  /* i : sorted & packed bits    */
	enum Mode *dec_mode,       /* o : mode information        */
    short bits[]          /* o : serial bits             */
);

/*************************************************************************
*
*  Function:   AMRDec_DecideMode
*  Purpose:    decide rx frame mode.
*  In:         rx_type is received frame type
*  Out:        dec_mode is the frame mode received
*
***************************************************************************/
extern	void AMRDec_DecideMode(enum RXFrameType rx_type,enum Mode *dec_mode);

/*************************************************************************
*
*  Function:   AMRDec_decoder_homing_frame_test_first
*  Purpose:    The encoded serial bits are converted to all parameters
*              of the corresponding mode.
*  In:         input_frame[]  one frame of encoded serial bits
*              mode           mode type
*  Out:        none
*
***************************************************************************/
extern	short AMRDec_decoder_homing_frame_test_first (short input_frame[], enum Mode dec_mode);

/*************************************************************************
*
*  Function:   AMRDec_Speech_Decode_Frame
*  Purpose:    Decode one frame
*  In:         as below
*  Out:        0 if success
*
***************************************************************************/
extern	int AMRDec_Speech_Decode_Frame (
//    void *state, /* io: post filter states                */
    enum Mode dec_mode,               /* i : AMR dec_mode                          */
    short *dec_serial,               /* i : serial bit stream                 */
    enum RXFrameType frame_type,    /* i : Frame type                        */
    short *dec_synth                 /* o : synthesis speech (postfiltered    */
                                  /*     output)                           */
);
/*************************************************************************
*
*  Function:   AMRDec_decoder_homing_frame_test
*  Purpose:    The encoded serial bits are converted to all parameters
*              of the corresponding mode.
*  In:         none
*  Out:        none
*
***************************************************************************/
extern	short AMRDec_decoder_homing_frame_test (short input_frame[], enum Mode dec_mode);

/*************************************************************************
*
*  Function:   AMRDec_Speech_Decode_Frame_reset
*  Purpose:    Resets state memory
*  In:         none
*  Out:        0
*
***************************************************************************/
extern	int AMRDec_Speech_Decode_Frame_reset (void);


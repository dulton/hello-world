#include "amr_include.h"

#define PRMNO_MR475 17
#define PRMNO_MR515 19


/*************************************************************************
*
*  Function:   AMREnc_Speech_Encode_Frame_init
*  Purpose:    initializes state memory and filter structure
*  In:         dtx is set when voice detection is used.
*  Out:        returns 0 on success
*
***************************************************************************/
extern	int AMREnc_Speech_Encode_Frame_init (
                              int dtx,
                              char *id);

/*************************************************************************
*
*  Function:   AMREnc_sid_sync_init
*  Purpose:    initialize one instance of the sid_sync module
*  In:         none
*  Out:        returns 0 on success
*
***************************************************************************/
extern	int AMREnc_sid_sync_init (void);


/*************************************************************************
*
*  Function:   AMREnc_encoder_homing_frame_test
*  Purpose:    Checks if all samples of the input frame matches the encoder
*              homing frame pattern.
*  In:         input_frame[]  one frame of speech samples
*  Out:        none
*
***************************************************************************/
extern	short AMREnc_encoder_homing_frame_test (short input_frame[]);


/*************************************************************************
*
*  Function:   AMREnc_Speech_Encode_Frame
*  Purpose:    AMR encode one frame
*  In:         as below
*  Out:        returns 0 on success
*
***************************************************************************/
extern	int AMREnc_Speech_Encode_Frame (
//    void *state, /* i/o : post filter states          */
    enum Mode mode,               /* i   : speech coder mode           */
    short *new_speech,           /* i   : speech input                */
    short *serial,               /* o   : serial bit stream           */
    enum Mode *usedMode           /* o   : used speech coder mode */
    );

/*************************************************************************
*
*  Function:   AMREnc_sid_sync
*  Purpose:    Synchronize SID state.
*  In:         mode is AMR encode mode (MR475 or MRDTX or...)
*  Out:        tx_frame_type represents transmission frame type.
*
***************************************************************************/
extern void AMREnc_sid_sync (enum Mode mode,
               enum TXFrameType *tx_frame_type);

               
/*************************************************************************
*
*  Function:   AMREnc_PackBits
*  Purpose:    Sorts speech bits according decreasing subjective importance
*              and packs into octets according to AMR file storage format
*              as specified in RFC 3267 (Sections 5.1 and 5.3).
*  In:         as below
*  Out:        packed size
*
***************************************************************************/
extern	short AMREnc_PackBits(
    enum Mode used_mode,       /* i : actual AMR mode             */
    enum Mode mode,            /* i : requested AMR (speech) mode */
    enum TXFrameType fr_type,  /* i : frame type                  */
    short bits[],             /* i : serial bits                 */
    unsigned char packed_bits[]       /* o : sorted&packed bits          */
);



/*************************************************************************
*
*  Function:   AMREnc_Speech_Encode_Frame_reset
*  Purpose:    Reset state memory
*  In:         none
*  Out:        none
*
***************************************************************************/
extern	int AMREnc_Speech_Encode_Frame_reset (void);

/*************************************************************************
*
*  Function:   AMREnc_sid_sync_reset
*  Purpose:    Reset SID state
*  In:         none
*  Out:        none
*
***************************************************************************/
extern	int AMREnc_sid_sync_reset (void);





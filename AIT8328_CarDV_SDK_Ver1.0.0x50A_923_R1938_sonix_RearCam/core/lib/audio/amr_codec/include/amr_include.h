#ifndef	_AMR_INCLUDE_H_
#define	_AMR_INCLUDE_H_

#define AMR_L_FRAME      160       /* Frame size                               */
#define MAX_SERIAL_SIZE 244    /* max. num. of serial bits                 */
#define MAX_PACKED_SIZE (MAX_SERIAL_SIZE / 8 + 2)
#define SERIAL_FRAMESIZE (1+MAX_SERIAL_SIZE+5)
#define EHF_MASK 0x0008        /* encoder homing frame pattern             */

#define	AMR_DTX_ENABLE			1
#define	AMR_DTX_DISABLE			0

#define M            10        /* Order of LP filter                       */


enum Mode { MR475 = 0,
            MR515,            
            MR59,
            MR67,
            MR74,
            MR795,
            MR102,
            MR122,            
	    MRDTX,   
            N_MODES     /* number of (SPC) modes */

        };
        
enum TXFrameType { TX_SPEECH_GOOD = 0,
                   TX_SID_FIRST,
                   TX_SID_UPDATE,
                   TX_NO_DATA,
                   TX_SPEECH_DEGRADED,
                   TX_SPEECH_BAD,
                   TX_SID_BAD,
                   TX_ONSET,
                   TX_N_FRAMETYPES     /* number of frame types */
};

enum RXFrameType { RX_SPEECH_GOOD = 0,
                   RX_SPEECH_DEGRADED,
                   RX_ONSET,
                   RX_SPEECH_BAD,
                   RX_SID_FIRST,
                   RX_SID_UPDATE,
                   RX_SID_BAD,
                   RX_NO_DATA,
                   RX_N_FRAMETYPES     /* number of frame types */
};



#endif
#ifndef _AVC_CONFIG_H_
#define _AVC_CONFIG_H_

#define _AVC_HW_DEC_    (1)
#define _ARM_
#define _ARM_ASM_

#if _AVC_HW_DEC_ == 1

#define AVC_DEC_ISR_MODE (1)

#define DEFAULT_VLD_MODE                HW_VLD_MODE
#define DEFAULT_FMO_MODE                FMO_MODE
#define DEFAULT_DBLK_TRIGGER_MODE       DBLK_TRIGGER_BY_HW
#define MAX_REC_ROW_NUM                 3

#endif

#endif
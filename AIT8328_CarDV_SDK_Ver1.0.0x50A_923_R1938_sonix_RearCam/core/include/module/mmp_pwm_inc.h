//------------------------------------------------------------------------------
//
//  File        : mmp_pwm_inc.h
//  Description : Header file of PWM common information
//  Author      : Alterman
//  Revision    : 1.0
//
//------------------------------------------------------------------------------
#ifndef _MMP_PWM_INC_H_
#define _MMP_PWM_INC_H_

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================

#include "mmpf_typedef.h"

//==============================================================================
//
//                              CONSTANTS
//
//==============================================================================

//==============================================================================
//
//                              MACRO
//
//==============================================================================

#define PWM_SET_ID(id)          (id)
#define PWM_SET_PAD(pad)        (pad << 8)
#define PWM_GET_ID(pin)         (pin & 0x1F)
#define PWM_GET_PAD(pin)        ((pin & 0x0100) >> 8)

//==============================================================================
//
//                              DATA TYPES
//
//==============================================================================

typedef void PwmCallBackFunc(MMP_ULONG sr);

//==============================================================================
//
//                              ENUMERATION
//
//==============================================================================

/* PWM clock is combined with two sets of pulse configurations, A & B */
typedef enum _MMP_PWM_PULSE_ID {
    MMP_PWM_PULSE_ID_A = 0,
    MMP_PWM_PULSE_ID_B
} MMP_PWM_PULSE_ID;

/* PWM interrupts */
typedef enum _MMP_PWM_INT {
    MMP_PWM_INT_PULSE_A = 0x0,  ///< one pulse of A
    MMP_PWM_INT_PULSE_B,        ///< one pulse of B
    MMP_PWM_INT_ONE_ROUND       ///< one round of A/B pulses
} MMP_PWM_INT;

/* PWM engine ID and pads */
typedef enum _MMP_PWM_PIN {
    MMP_PWM0_PIN_AGPIO1     = PWM_SET_ID(0) |PWM_SET_PAD(0),
    MMP_PWM1_PIN_AGPIO2     = PWM_SET_ID(1) |PWM_SET_PAD(0),
    MMP_PWM2_PIN_AGPIO3     = PWM_SET_ID(2) |PWM_SET_PAD(0),
    MMP_PWM3_PIN_AGPIO4     = PWM_SET_ID(3) |PWM_SET_PAD(0),
    MMP_PWM4_PIN_BGPIO6     = PWM_SET_ID(4) |PWM_SET_PAD(0),
    MMP_PWM5_PIN_BGPIO7     = PWM_SET_ID(5) |PWM_SET_PAD(0),
    MMP_PWM6_PIN_BGPIO8     = PWM_SET_ID(6) |PWM_SET_PAD(0),
    MMP_PWM7_PIN_BGPIO9     = PWM_SET_ID(7) |PWM_SET_PAD(0),
    MMP_PWM8_PIN_BGPIO10    = PWM_SET_ID(8) |PWM_SET_PAD(0),
    MMP_PWM9_PIN_BGPIO11    = PWM_SET_ID(9) |PWM_SET_PAD(0),
    MMP_PWM10_PIN_PLCD0     = PWM_SET_ID(10)|PWM_SET_PAD(0),
    MMP_PWM11_PIN_PLCD1     = PWM_SET_ID(11)|PWM_SET_PAD(0),
    MMP_PWM12_PIN_PLCD2     = PWM_SET_ID(12)|PWM_SET_PAD(0),
    MMP_PWM13_PIN_PLCD3     = PWM_SET_ID(13)|PWM_SET_PAD(0),
    MMP_PWM14_PIN_PLCD4     = PWM_SET_ID(14)|PWM_SET_PAD(0),
    MMP_PWM15_PIN_PLCD5     = PWM_SET_ID(15)|PWM_SET_PAD(0),
    MMP_PWM16_PIN_PLCD1_CS  = PWM_SET_ID(16)|PWM_SET_PAD(0),
    MMP_PWM17_PIN_PLCD2_CS  = PWM_SET_ID(17)|PWM_SET_PAD(0),
    MMP_PWM4_PIN_CGPIO12    = PWM_SET_ID(4) |PWM_SET_PAD(1),
    MMP_PWM5_PIN_CGPIO13    = PWM_SET_ID(5) |PWM_SET_PAD(1),
    MMP_PWM6_PIN_CGPIO14    = PWM_SET_ID(6) |PWM_SET_PAD(1),
    MMP_PWM7_PIN_CGPIO15    = PWM_SET_ID(7) |PWM_SET_PAD(1),
    MMP_PWM8_PIN_CGPIO16    = PWM_SET_ID(8) |PWM_SET_PAD(1),
    MMP_PWM9_PIN_CGPIO17    = PWM_SET_ID(9) |PWM_SET_PAD(1),
    MMP_PWM10_PIN_PLCD8     = PWM_SET_ID(10)|PWM_SET_PAD(1),
    MMP_PWM11_PIN_PLCD9     = PWM_SET_ID(11)|PWM_SET_PAD(1),
    MMP_PWM12_PIN_PLCD10    = PWM_SET_ID(12)|PWM_SET_PAD(1),
    MMP_PWM13_PIN_PLCD11    = PWM_SET_ID(13)|PWM_SET_PAD(1),
    MMP_PWM14_PIN_PLCD12    = PWM_SET_ID(14)|PWM_SET_PAD(1),
    MMP_PWM15_PIN_PLCD13    = PWM_SET_ID(15)|PWM_SET_PAD(1),
    MMP_PWM_PIN_MAX
} MMP_PWM_PIN;

//==============================================================================
//
//                              STRUCTURES
//
//==============================================================================

/* Pulse configuration for one set */
typedef struct _MMP_PWM_ATTR {
    MMP_UBYTE   ubID;           ///< PWM controller engine ID
    MMP_PWM_PULSE_ID uPulseID;  ///< attribute for PWM pulse ID (A/B)
    MMP_ULONG   ulClkDuty_T0;
    MMP_ULONG   ulClkDuty_T1;
    MMP_ULONG   ulClkDuty_T2;
    MMP_ULONG   ulClkDuty_T3;
    MMP_ULONG   ulClkDuty_Peroid;
    MMP_UBYTE   ubNumOfPulses;  ///< number of pulses
} MMP_PWM_ATTR;

#endif // _MMP_PWM_INC_H_


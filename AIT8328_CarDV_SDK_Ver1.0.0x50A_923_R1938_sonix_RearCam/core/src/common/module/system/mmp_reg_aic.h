//==============================================================================
//
//  File        : mmp_reg_aic.h
//  Description : INCLUDE File for the AIC module register map.
//  Author      : Jerry Tsao
//  Revision    : 1.0
//
//==============================================================================

#ifndef _MMP_REG_AIC_
#define _MMP_REG_AIC_

#include "mmp_register.h"

/** @addtogroup MMPH_reg
@{
*/

#if (CHIP == MCR_V2)
//-----------------------------
// AIC Structure (0xFFFF F000)
//-----------------------------
typedef struct _AITS_AIC {

    AIT_REG_D   AIC_SMR[64];        // Source Mode Register             // 0x000 - 0x0FF
    AIT_REG_D   AIC_IVR;            // IRQ Vector Register              // 0x100
    AIT_REG_D   AIC_FVR;            // FIQ Vector Register              // 0x104
    AIT_REG_D   AIC_ISR;            // Interrupt Status Register        // 0x108
    AIT_REG_D   AIC_IPR_LSB;        // Interrupt Pending Register 0~31  // 0x10C
    AIT_REG_D   AIC_IMR_LSB;        // Interrupt Mask Register 0~31     // 0x110
    AIT_REG_D   AIC_CISR;           // Core Interrupt Status Register   // 0x114
    AIT_REG_D   AIC_IMR_MSB;        // Interrupt Mask Register 63~32    // 0x118
    AIT_REG_D                           _x11C;
    AIT_REG_D   AIC_IECR_LSB;       // Interrupt Enable Cmd Reg 0~31    // 0x120
    AIT_REG_D   AIC_IDCR_LSB;       // Interrupt Disable Cmd Reg 0~31   // 0x124
    AIT_REG_D   AIC_ICCR_LSB;       // Interrupt Clear Cmd Reg 0~31     // 0x128
    AIT_REG_D   AIC_ISCR_LSB;       // Interrupt Set Cmd Reg 0~31       // 0x12C
    AIT_REG_D   AIC_EOICR;          // End of Interrupt Cmd Reg         // 0x130
    AIT_REG_D                           _x134;
    AIT_REG_D   AIC_DBR;            // Debug Mode                       // 0x138
    AIT_REG_D   AIC_IPR_MSB;        // Interrupt Pending Register 32~63 // 0x13C
    AIT_REG_D   AIC_IECR_MSB;       // Interrupt Enable Cmd Reg 32~63   // 0x140
    AIT_REG_D   AIC_IDCR_MSB;       // Interrupt Disable Cmd Reg 32~63  // 0x144
    AIT_REG_D   AIC_ICCR_MSB;       // Interrupt Clear Cmd Reg 32~63    // 0x148
    AIT_REG_D   AIC_ISCR_MSB;       // Interrupt Set Cmd Reg 32~63      // 0x14C
    AIT_REG_D                           _x150[0x2C];
    AIT_REG_D   AIC_SVR[64];        // Source Vector Register           // 0x200 - 0x2FF
} AITS_AIC, *AITPS_AIC;
#endif

//==============================================================================
//
//                              MACRO DEFINE
//
//==============================================================================

// -------- AIC_SMR[]: Interrupt Source Mode Registers --------
#define AIC_INT_MASK                ((unsigned short)0x0080)            // Interrupt direct to IRQ or FIQ
#define AIC_PRIOR_MASK              ((unsigned short)0x0007)            // Priority
#define AIC_SRCTYPE_MASK            ((unsigned short)0x0060)            // Source Type Definition

// Interrupt Priority
#define AIC_PRIOR_LOWEST            ((unsigned int)0x0000)              // Lowest priority level
#define AIC_PRIOR_HIGHEST           ((unsigned int)0x0007)              // Highest priority level

// Interrupt Direction
#define AIC_INT_TO_FIQ              ((unsigned short)0x0080)            // Interrupt is route to FIQ
#define AIC_INT_TO_IRQ              ((unsigned short)0x0000)            // Interrupt is route to IRQ

// Interrupts Sensibility
#define AIC_SRCTYPE_LOW_LEVEL_SENSITIVE     ((unsigned short)0x0000)    // Low Level
#define AIC_SRCTYPE_NEGATIVE_EDGE_TRIGGERED ((unsigned short)0x0020)    // Negative Edge
#define AIC_SRCTYPE_HIGH_LEVEL_SENSITIVE    ((unsigned short)0x0040)    // High Level
#define AIC_SRCTYPE_POSITIVE_EDGE_TRIGGERED ((unsigned short)0x0060)    // Positive Edge

// -------- AIC_ISR: Interrupt Status Register --------
#define AIC_IRQID_MASK              ((unsigned short)0x1F)              // Current source interrupt

// -------- AIC_CISR: Interrupt Core Status Register --------
#define AIC_NFIQ                    ((unsigned short)0x01)              // Core FIQ Status
#define AIC_NIRQ                    ((unsigned short)0x02)              // Core IRQ Status

// -------- AIC_DBR : ICE Debug Mode --------
#define AIC_DBG_EN                  ((unsigned short)0x1 << 0)          // Debug Mode Enable

// -------- AIC_SMR[], AIC_SVR[]: Interrupt Source --------
#define AIC_SRC_VIF                 0x00
#define AIC_SRC_ISP                 0x01
#define AIC_SRC_JPG                 0x02
#define AIC_SRC_SCAL                0x03
#define AIC_SRC_GRA                 0x04
#define AIC_SRC_IBC                 0x05
#define AIC_SRC_RAW                 0x06
#define AIC_SRC_UART                0x08
#define AIC_SRC_TC0                 0x09
#define AIC_SRC_TC1                 0x0A
#define AIC_SRC_TC2                 0x0B
#define AIC_SRC_USB                 0x0C
#define AIC_SRC_SDRAM               0x0D
#define AIC_SRC_HDMI_PHY            0x0E
#define AIC_SRC_WD_INNER            0x0F
#define AIC_SRC_GPIO                0x10
#define AIC_SRC_I2CS                0x11
#define AIC_SRC_SD                  0x12
#define AIC_SRC_CCIR                0x13
#define AIC_SRC_I2STIMER            0x14
#define AIC_SRC_H264DEC             0x15
#define AIC_SRC_H264                0x15
#define AIC_SRC_I2S_FIFO            0x16
#define AIC_SRC_I2S                 0x17
#define AIC_SRC_I2CM                0x18
#define AIC_SRC_SPI                 0x19
#define AIC_SRC_IRDA                0x1A
#define AIC_SRC_PWM                 0x1B
#define AIC_SRC_DMA                 0x1C
#define AIC_SRC_GBL                 0x1D
#define AIC_SRC_SM                  0x1E
#define AIC_SRC_SIF                 0x1F
#define AIC_SRC_TC3                 0x20
#define AIC_SRC_TC4                 0x21
#define AIC_SRC_TC5                 0x22
#define AIC_SRC_LDC                 0x23
#define AIC_SRC_WD_OUTER            0x24
#define AIC_SRC_CPU2CPU             0x25
#define AIC_SRC_MCI                 0x26
#define AIC_SRC_AUDIO               0x27
#define AIC_SRC_AFE_FIFO            0x28
#define AIC_SRC_DSPY                0x29
#define AIC_SRC_HDMI                0x2A
#define AIC_SRC_USB_DMA             0x2B
#define AIC_SRC_CALI                0x2C

#define AIC_SRC_TC(id)              ((id < 3) ? (AIC_SRC_TC0 + id) : \
                                    (AIC_SRC_TC3 + id - 3))

#endif // _MMP_REG_AIC_

//==============================================================================
//
//  File        : reg_retina.h
//  Description : Retina regsiter definition header file
//  Author      : Jerry Tsao
//  Revision    : 1.0
//
//==============================================================================

#ifndef _REG_RETINA_H_
#define _REG_RETINA_H_

typedef volatile unsigned char  AIT_REG_B;
typedef volatile unsigned short AIT_REG_W;
typedef volatile unsigned int   AIT_REG_D;

#include "config_fw.h"

//------------------------------
// Register Naming Abbreviation
//------------------------------

// AUD          : audio
// ADJST        : adjust
// ACC          : accumulate
// AF           : auto focus
// AWB          : auto white balance
// AE           : auto exporsure
// AZOOM        : auto zoom
// BLD          : blend
// CTL          : control
// CLK          : clock
// COMPEN       : compensation
// COMP         : compress
// CSR          : control and status register
// CONV         : convert
// CNT          : count
// CFG          : configuration
// COLR         : color
// CMD          : command
// DBL          : double
// DBLK         : deblocking
// DLINE        : delay line
// DNSAMP       : down sample
// DNLD         : download
// DEFT         : defect
// Dn           : divided by n
// DSPY         : display
// EN,DIS       : enable, disable
// END          : finish, the end
// FRM          : frame
// FMT          : format
// FOUT         : FIFO out
// GAMA         : gamma
// GBL          : global
// PANL         : panel
// PAL          : palette
// ENC,DEC      : encode, decode
// FREQ         : frequency
// SHFT         : shift
// HSTGRM       : histogram
// H,V          : horizontal, vertical
// INT          : interrupt
// IN,OUT       : input, output
// IDX          : index
// IMG          : image
// LCL          : local
// LS           : lenshading
// LPF          : low-pass filter
// LRP          : linear interpolation
// MTX          : matrix
// MDL          : module
// NO           : number id
// NUM          : count number
// NR           : noise reduction
// OVLY         : overlay
// OFST         : offset
// PWR          : power
// PAR          : parity
// PARM         : paramter
// PHL          : peripheral
// PRIO         : priority
// PREVW        : preview
// POLAR        : polarity
// PST,NEG      : positive, negative
// PIXL         : pixel
// QLTY         : quality
// RC,WC        : read cycle, cycle
// RW,RD,WR     : read/write, read, write
// RX           : receive
// RST          : reset
// ROTE         : rotate
// SRC          : source id
// STG          : storage
// ST,ED        : start and end point
// SENSR        : sensor
// SERL,PARL    : serial, parellel
// SR           : status register
// SCAL         : scaling
// SIN,SOUT     : scaling in, scaling out
// SPC          : space
// TBL          : table
// TOUT         : timeout
// TX           : transfer
// UPBND        : upper bound
// UPD          : update
// VID          : video
// WT           : weight
// W,H          : width , height
// WD           : watchdog
// X,Y          : coordinate X, Y

// Peripheral Register of Retina

//==============================================================================
//
//                Retina Peripheral Control Register Definitions
//
//==============================================================================
//-----------------------------
// AIC Structure (0xFFFF F000)
//-----------------------------
typedef struct _AITS_AIC {
	#if (CHIP == MCR_V2)
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
	#endif
} AITS_AIC, *AITPS_AIC;

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
#if (CHIP == MCR_V2)
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
#endif

//==============================================================================
//
//                              Retina Register Base Address
//
//==============================================================================

#define AITC_BASE_AIC               ((AITPS_AIC)0xFFFFF000) // AIC Base Address

//==============================================================================
//
//                              CONSTANTS
//
//==============================================================================

extern unsigned int gCpuFreqKHz;

#define RTNA_CLK_M                  (gCpuFreqKHz/1000)
#define RTNA_CPU_CLK_M              (RTNA_CLK_M/1)
#define RTNA_PHL_CLK_M              (RTNA_CPU_CLK_M/2)

#endif // _REG_RETINA_H_

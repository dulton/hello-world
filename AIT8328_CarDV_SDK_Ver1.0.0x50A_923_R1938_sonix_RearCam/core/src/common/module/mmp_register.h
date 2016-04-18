//==============================================================================
//
//  File        : mmp_register.h
//  Description : INCLUDE File for the Retina register map.
//  Author      : Penguin Torng
//  Revision    : 1.0
//
//==============================================================================

#ifndef _MMP_REGISTER_H_
#define _MMP_REGISTER_H_

///@ait_only

#if defined(BUILD_FW)
#include "config_fw.h"
#else
#include "ait_config.h"
#endif

/** @addtogroup MMPH_reg
@{
*/

typedef volatile unsigned char  AIT_REG_B;
typedef volatile unsigned short AIT_REG_W;
typedef volatile unsigned int   AIT_REG_D;

// ********************************
//   Register Naming Abbreviation
// ********************************

// 1ST          : first
// AUD          : audio
// ADDR         : address (new)
// ADJST        : adjust
// ACC          : accumulate
// AF           : auto focus
// AWB          : auto white balance
// AE           : auto exporsure
// AZOOM        : auto zoom
// BLD          : blend
// BS           : bit stream
// BUF          : buffer
// CFG          : configuration
// CLK          : clock
// CMD          : command
// CPLT         : complete
// CNT          : count, counter
// COLR         : color
// COMPEN       : compensation
// COMP         : compress
// CONV         : convert
// CSR          : control and status register
// CTL          : control (new)
// CUR          : current (new)
// DBL          : double
// DBLK         : deblocking
// DEC          : decode (new)
// DLINE        : delay line
// DESC         : descriptor
// DNSAMP       : down sample
// DNLD         : download
// DEFT         : defect
// Dn           : divided by n
// DSI          : display serial interface
// DSPY         : display
// EN,DIS       : enable, disable
// END          : finish, the end
// EXCL         : exclude, excluding
// EXT          : extension
// FRM          : frame
// FMT          : format
// FOUT         : FIFO out
// GAMA         : gamma
// GBL          : global
// PANL         : panel
// PAL          : palette
// PRED         : prediction (new)
// ENC,DEC      : encode, decode
// FINISH       : finish, consider using DONE if possible(new)
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
// LOWBD        : lower bound (new)
// LPF          : low-pass filter
// LRP          : linear interpolation
// MTX          : matrix
// MDL          : module
// NO           : number id
// NUM          : count number
// NR           : noise reduction
// OPT          : option (new)
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
// RECD         : record
// RES          : resolution (new)
// RC,WC        : read cycle, cycle
// RW,RD,WR     : read/write, read, write
// RX           : receive
// RST          : reset
// ROTE         : rotate
// RSL          : result
// SEL          : select (new)
// SZ           : size
// SK           : skip
// SRC          : source id
// STG          : storage
// STS          : status
// ST,ED        : start and end point
// SENSR        : sensor
// SERL,PARL    : serial, parellel
// SR           : status register
// SCAL         : scaling
// SIN,SOUT     : scaling in, scaling out
// SPC          : space
// TBL          : table
// TGT          : target
// TH           : threshold (new)
// TMP          : temp
// TSTP         : timestamp
// TYP          : type
// TX           : transfer
// XFER         : transfer
// U            : Cb (new)
// UPBND        : upper bound (new)
// UPD          : update
// V            : Cr (new), verticial
// VAL          : value (new)
// VID          : video
// WT           : weight
// W,H          : width , height
// WC           : Write cycle, RC
// WD           : watchdog
// WR           : write
// X,Y          : coordinate X, Y
//
// ADDR         : address
// BD           : bound
// BS           : bit stream
// BUF          : buffer
// CUR          : current
// CTL          : control
// FINISH       : finish, consider using DONE if possible
// HDR          : header
// LOWBD        : lower bound
// OPT          : option
// RES          : resolution
// SEL          : select
// TH           : threshold
// UPBD         : upper bound
// VAL          : value
// ===== naming that is not defined before =====
// BITSTREAM some of them uses BS

// *********************************************************************
//                              Retina Peripheral Base Address
// *********************************************************************

#define AITC_BASE_OPR               ((AIT_REG_B*)               0x00000000)

#if (CHIP == MCR_V2)
#define AITC_BASE_H264DEC_CTL       ((AITPS_H264DEC_CTL)        0x80000000) // H264DEC  Base Address
#define AITC_BASE_H264DEC_REF       ((AITPS_H264DEC_REF)        0x80000010) // H264DEC  Base Address
#define AITC_BASE_H264DEC_REC       ((AITPS_H264DEC_REC)        0x80000060) // H264DEC  Base Address
#define AITC_BASE_H264DEC_DBLK      ((AITPS_H264DEC_DBLK)       0x80000070) // H264DEC  Base Address
#define AITC_BASE_H264DEC_VLD       ((AITPS_H264DEC_VLD)        0x80000080) // H264DEC  Base Address
#define AITC_BASE_H264DEC_PARSE_EP3 ((AITPS_H264DEC_PARSE_EP3)  0x800000F0) // H264DEC  Base Address
#define AITC_BASE_H264DEC_MB_DATA0  ((AITPS_H264DEC_MB_DATA)    0x80000100) // H264DEC  Base Address
#define AITC_BASE_H264DEC_MB_DATA1  ((AITPS_H264DEC_MB_DATA)    0x80000180) // H264DEC  Base Address
#define AITC_BASE_H264DEC_DBLK_ROT  ((AITPS_H264DEC_DBLK_ROT)   0x800002D8) // H264DEC  Base Address
#define AITC_BASE_H264ENC           ((AITPS_H264ENC)            0x80000400) // H264ENC  Base Address
#define AITC_BASE_PWM               ((AITPS_PWM)                0x80000800) // PWM      Base Address
#define AITC_BASE_USBCTL            ((AITPS_USB_CTL)            0x80001000) // USB      Base Address
#define AITC_BASE_USBDMA            ((AITPS_USB_DMA)            0x80001400) // USB DMA  Base Address
#define AITC_BASE_HDMI              ((AITPS_HDMI)               0x80002000) // HDMI     Base Address
#define AITC_BASE_HDR               ((AITPS_HDR)                0x800024D0) // HDR      Base Address
#define AITC_BASE_DSPY              ((AITPS_DSPY)               0x80002800) // LCD      Base Address
#define AITC_BASE_TV                ((AITPS_TV)                 0x80002870) // TV       Base Address
#define AITC_BASE_DDR3              ((AITPS_DRAM_DDR3)          0x80003000) // DDR3     Base Address
#define AITC_BASE_CCIR              ((AITPS_CCIR)          		0x80003200) // CCIR     Base Address
#define AITC_BASE_SD2               ((AITPS_SD)                 0x80003500) // SD2      Base Address
#define AITC_BASE_I2S2			    ((AITPS_I2S)                0x80003600) // I2S2     Base Address
#define AITC_BASE_I2CS              ((AITPS_I2CS)               0x80003800) // I2CS     Base Address
#define AITC_BASE_CORE              ((AITPS_CORE)               0x80003A00) // CPU Core Base Address
#define AITC_BASE_PLL_JITTER_CAL    ((AITPS_PLL_JITTER_CAL)     0x80003A60) // PLL JITTER Calculation Base Address
#define AITC_BASE_SARADC_POR        ((AITPS_SARADC_POR)         0x80003A80) // SARADC POR Base Address
#define AITC_BASE_RTC               ((AITPS_RTC)                0x80003A90) // RTC      Base Address
#define AITC_BASE_HDMI_PHY          ((AITPS_HDMI_PHY)           0x80003AA0) // HDMI PHY Base Address
#define AITC_BASE_CPU_SAHRE         ((AITPS_CPU_SHARE)          0x80003AC0) // CPU Share Register Base Address
#define AITC_BASE_LDC               ((AITPS_LDC)                0x80004000) // LDC      Base Address
#define AITC_BASE_LDC_LUT           ((AITPS_LDC_LUT)            0x80004200) // LDC LUT  Base Address
#define AITC_BASE_SCAL              ((AITPS_SCAL)               0x80004500) // Scaler   Base Address
#define AITC_BASE_GRA               ((AITPS_GRA)                0x80005000) // GRA      Base Address
#define AITC_BASE_PAD               ((AITPS_PAD)                0x80005100) // PAD      Base Address
#define AITC_BASE_SPIS              ((AITPS_SPIS)               0x80005200) // SPIS     Base Address
#define AITC_BASE_UARTB             ((AITPS_UARTB)              0x80005300) // Uart     Base Address
#define AITC_BASE_GPIOCTL           ((AITPS_GPIO_CNT)           0x80005500) // GPIO Conter Base Address
#define AITC_BASE_IBC               ((AITPS_IBC)                0x80005600) // IBC      Base Address
#define AITC_BASE_MONITOR           ((AITPS_MONITOR)            0x80005900) // MONITOR  Base Address
#define AITC_BASE_I2S1              ((AITPS_I2S)                0x80005A00) // I2S1     Base Address
#define AITC_BASE_IRDA              ((AITPS_IRDA)               0x80005B00) // IRDA     Base Address
#define AITC_BASE_GBL               ((AITPS_GBL)                0x80005D00) // Global   Base Address
#define AITC_BASE_NAND              ((AITPS_NAND)               0x80005E00) // NAND(SM) Base Address
#define AITC_BASE_SD1               ((AITPS_SD)                 0x80005F00) // SD1      Base Address
#define AITC_BASE_VIF               ((AITPS_VIF)                0x80006000) // VIF      Base Address
#define AITC_BASE_MIPI              ((AITPS_MIPI)               0x80006110) // MIPI     Base Address
#define AITC_BASE_JPG               ((AITPS_JPG)                0x80006200) // JPEG     Base Address
#define AITC_BASE_SD0               ((AITPS_SD)                 0x80006300) // SD0      Base Address
#define AITC_BASE_DADC_EXT          ((AITPS_DADC_EXT)           0x80006500) // Audio ADC Digital Filter Coefficient  Base Address
#define AITC_BASE_GPIO              ((AITPS_GPIO)               0x80006600) // GPIO     Base Address
#define AITC_BASE_SIF               ((AITPS_SIF)                0x80006700) // SIF      Base Address
#define AITC_BASE_HINT              ((AITPS_HINT)               0x80006800) // HINT     Base Address
#define AITC_BASE_RAWPROC           ((AITPS_RAWPROC)            0x80006B00) // RAWPROC  Base Address
#define AITC_BASE_ICOB              ((AITPS_ICOB)               0x80006C00) // Icon     Base Address
#define AITC_BASE_SPIB              ((AITPS_SPIB)               0x80006D00) // PSPI     Base Address
#define AITC_BASE_DRAM              ((AITPS_DRAM)               0x80006E00) // DRAM     Base Address
#define AITC_BASE_ISP               ((AITPS_ISP)                0x80007000) // ISP      Base Address
#define AITC_BASE_BAYERSCAL         ((AITPS_BAYER_SCAL)	        0x80007100) // BAYER SCAL Base Address
#define AITC_BASE_DMA               ((AITPS_DMA)                0x80007600) // DMA      Base Address
#define AITC_BASE_MCI               ((AITPS_MCI)                0x80007700) // MCI      Base Address
#define AITC_BASE_I2S0              ((AITPS_I2S)                0x80007800) // I2S0     Base Address
#define AITC_BASE_I2CM              ((AITPS_I2CM)               0x80007A00) // I2CM     Base Address
#define AITC_BASE_TBL_Q             ((AIT_REG_B*)               0x80007C00) // Q-Table  Base Address
#define AITC_BASE_AUD               ((AITPS_AUD)                0x80007F00) // AFE0     Base Address
#define AITC_BASE_ICON_LUT			((AITPS_ICON_LUT)       	0x80013000) // Icon LUT	Base Address
#define AITC_BASE_RAW_OFST_TBL      ((AIT_REG_B*)           	0x80013400) // Raw Offset Table  Base Address
#endif

//Common Register Base Address
#define AITC_BASE_WD                ((AITPS_WD)                 0xFFFF8000) // WatchDog Base Address
#define AITC_BASE_TCB               ((AITPS_TCB)                0xFFFE0000) // Timer    Base Address
#define AITC_BASE_TC0               ((AITPS_TC)                 0xFFFE0000) // TC0      Base Address
#define AITC_BASE_TC1               ((AITPS_TC)                 0xFFFE0040) // TC1      Base Address
#define AITC_BASE_TC2               ((AITPS_TC)                 0xFFFE0080) // TC2      Base Address
#define AITC_BASE_TC3               ((AITPS_TC)                 0xFFFE0100) // TC3      Base Address
#define AITC_BASE_TC4               ((AITPS_TC)                 0xFFFE0140) // TC4      Base Address
#define AITC_BASE_TC5               ((AITPS_TC)                 0xFFFE0180) // TC5      Base Address
#define AITC_BASE_AIC               ((AITPS_AIC)				0xFFFFF000) // AIC 		Base Address

////////////////////////////////////
// OPR base address
#define GBL_BASE                    ((MMP_ULONG)AITC_BASE_GBL)
#define DSPY_BASE                   ((MMP_ULONG)AITC_BASE_DSPY)
#define TV_BASE                     ((MMP_ULONG)AITC_BASE_TV)
#define DRAM_BASE                   ((MMP_ULONG)AITC_BASE_DRAM)
#define AUD_BASE                    ((MMP_ULONG)AITC_BASE_AUD)
#define H264DEC_BASE    		    ((MMP_ULONG)AITC_BASE_H264DEC_CTL)
#define H264REF_BASE    		    ((MMP_ULONG)AITC_BASE_H264DEC_REF)
#define H264REC_BASE    		    ((MMP_ULONG)AITC_BASE_H264DEC_REC)
#define H264DBLK_BASE    		    ((MMP_ULONG)AITC_BASE_H264DEC_DBLK)
#define H264VLD_BASE                ((MMP_ULONG)AITC_BASE_H264DEC_VLD)
#define H264ENC_BASE                ((MMP_ULONG)AITC_BASE_H264ENC)
#define HDMI_BASE                   ((MMP_ULONG)AITC_BASE_HDMI)
#define HDMI_PHY_BASE               ((MMP_ULONG)AITC_BASE_HDMI_PHY)
#define H264DBLKROT_BASE            ((MMP_ULONG)AITC_BASE_H264DEC_DBLK_ROT)
#define I2S0_BASE                   ((MMP_ULONG)AITC_BASE_I2S0)
#define I2S1_BASE                   ((MMP_ULONG)AITC_BASE_I2S1)
#define I2S2_BASE					((MMP_ULONG)AITC_BASE_I2S2)

/// @}

#endif // _MMP_REGISTER_H_

///@end_ait_only

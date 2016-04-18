//==============================================================================
//
//  File        : mmp_reg_hdmi.h
//  Description : INCLUDE File for the Retina register map.
//  Author      : Will Chen
//  Revision    : 1.0
//
//==============================================================================

#ifndef _MMP_REG_HDMI_H_
#define _MMP_REG_HDMI_H_

#include "mmp_register.h"

//------------------------------
// HDMI Structure (0x80002000)
//------------------------------
typedef struct _AITS_HDMI {
    AIT_REG_B   HDMI_CTL_1;                             //0x00
    AIT_REG_B   x01[3];
    AIT_REG_B   HDMI_CTL_2;                             //0x04
        /*-DEFINE-------------------------------------------------------*/
        #define     HDMI_SELECT_HDMI                    0x80
        #define     HDMI_SELECT_DVI                     0x00
        #define     HDMI_TWO_CH_AUDIO                   0x40
        #define     HDMI_MULTI_CH_AUDIO                 0x00
        /*--------------------------------------------------------------*/
    AIT_REG_B   HDMI_CTL_3;                             //0x05
        /*-DEFINE-------------------------------------------------------*/
        #define     HDMI_VIDEO_LOW_BIT                  0x00
        #define     HDMI_VIDEO_MID_BIT                  0x01
        #define     HDMI_VIDEO_HIGH_BIT                 0x02
        #define     HDMI_VIDEO_SEL_FOR_CH2(_a)          ((_a) << 4)
        #define     HDMI_VIDEO_SEL_FOR_CH1(_a)          ((_a) << 2)
        #define     HDMI_VIDEO_SEL_FOR_CH0(_a)          (_a)
        /*--------------------------------------------------------------*/
    AIT_REG_B   HDMI_CTL_4;                             //0x06
        /*-DEFINE-------------------------------------------------------*/
        #define     HDMI_CH_0                           0x00
        #define     HDMI_CH_1                           0x01
        #define     HDMI_CH_2                           0x02
        #define     HDMI_AUDIO_MAP_TO_AFE2(_a)          ((_a) << 4)
        #define     HDMI_AUDIO_MAP_TO_AFE1(_a)          ((_a) << 2)
        #define     HDMI_AUDIO_MAP_TO_AFE0(_a)          (_a)
        /*--------------------------------------------------------------*/
    AIT_REG_B   HDMI_CTL_5;                             //0x07
        /*-DEFINE-------------------------------------------------------*/
        #define     HDMI_AUDIO_FULL_FILL                0x08
        #define     HDMI_AUDIO_RECEIVED                 0x00
        #define     HDMI_AUDIO_24BIT                    0x00
        #define     HDMI_AUDIO_28BIT                    0x01
        #define     HDMI_AUDIO_32BIT                    0x07
        /*--------------------------------------------------------------*/
    AIT_REG_B   HDMI_CTL_6;                             //0x08
        /*-DEFINE-------------------------------------------------------*/
        #define     HDMI_NORMAL_MODE                    0x80
        #define     HDMI_POWER_DOWN                     0x00
        #define     HDMI_VIDEO_MUTE                     0x04
        #define     HDMI_VIDEO_NORMAL                   0x00
        #define     HDMI_AUDIO_MUTE                     0x02
        #define     HDMI_AUDIO_NORMAL                   0x00
        #define     HDMI_SET_AV_MUTE                    0x01
        #define     HDMI_CLEAR_AV_MUTE                  0x00
        /*--------------------------------------------------------------*/
    AIT_REG_B   HDMI_CTL_7;                             //0x09
        /*-DEFINE-------------------------------------------------------*/
        #define     HDMI_FILL_INVALID                   0x80
        #define     HDMI_REMOVE_INPUT_SAMPLE            0x40
        #define     HDMI_FORCE_INPUT_ZERO               0x20
        #define     HDMI_HOLD_LAST_VALUE                0x10
        #define     HDMI_RGB_VIDEO_INPUT                0x08
        #define     HDMI_VIDEO_QUANT_FULL               0x04
        #define     HDMI_YUV444_VIDEO_INPUT             0x02
        #define     HDMI_YUV422_VIDEO_INPUT             0x01
        /*--------------------------------------------------------------*/
    AIT_REG_B   HDMI_CTL_8;                             //0x0A
        /*-DEFINE-------------------------------------------------------*/
        #define     HDMI_VIDEO_MUTE_FOR_CH2             0x80
        #define     HDMI_VIDEO_MUTE_FOR_CH1             0x40
        #define     HDMI_VIDEO_MUTE_FOR_CH0             0x20
        #define     HDMI_GRAPHIC_TEST_EN                0x04
        #define     HDMI_PURE_TEST                      0x00
        #define     HDMI_COLORBAR_TEST                  0x01
        #define     HDMI_GRADUAL_TEST                   0x02
        /*--------------------------------------------------------------*/
    AIT_REG_B   HDMI_CTL_9;                             //0x0B
    AIT_REG_B   x0C[4];
    AIT_REG_B   HDMI_CTL_10;                            //0x10
        /*-DEFINE-------------------------------------------------------*/
        #define     HDMI_PACKET_MIX_MODE_EN             0x80
        #define     HDMI_EXTERNAL_ECC_EN                0x40
        #define     HDMI_AUDIO_DATA_ISLAND_HBLANK_EN    0x08
        #define     HDMI_I_DATA_ISLAND_HBLANK_EN        0x04
        #define     HDMI_AUDIO_DATA_ISLAND_VBLANK_EN    0x02
        #define     HDMI_I_DATA_ISLAND_VBLANK_EN        0x01
        /*--------------------------------------------------------------*/
    AIT_REG_B   HDMI_SKEW_NUM_VBLANK;                   //0x11
    AIT_REG_B   HDMI_PACKET_NUM_VBLANK;                 //0x12
    AIT_REG_B   HDMI_TYPE_I_DATA_NUM;                   //0x13
    AIT_REG_B   HDMI_SKEW_NUM_HBLANK;                   //0x14
    AIT_REG_B   HDMI_PACKET_NUM_HBLANK;                 //0x15
    AIT_REG_B   HDMI_SPACING_NUM;                       //0x16
    AIT_REG_B   HDMI_SYNC_POLARITY;                     //0x17
    AIT_REG_B   HDMI_V_RESOLUTION_MSB;                  //0x18
    AIT_REG_B   HDMI_V_RESOLUTION_LSB;                  //0x19
    AIT_REG_B   HDMI_H_RESOLUTION_MSB;                  //0x1A
    AIT_REG_B   HDMI_H_RESOLUTION_LSB;                  //0x1B
    AIT_REG_B   HDMI_V_BACK_PORCH_MSB;                  //0x1C
    AIT_REG_B   HDMI_V_BACK_PORCH_LSB;                  //0x1D
    AIT_REG_B   HDMI_H_BACK_PORCH_MSB;                  //0x1E
    AIT_REG_B   HDMI_H_BACK_PORCH_LSB;                  //0x1F
    AIT_REG_B   HDMI_PACKET_1_STS;                      //0x20
    AIT_REG_B   HDMI_PACKET_2_STS;                      //0x21
    AIT_REG_B   HDMI_CTL_28;                            //0x22
        /*-DEFINE-------------------------------------------------------*/
        #define     HDMI_ACR_CTS_EXTRA_EN               0x80
        #define     HDMI_PACKET_1_AUTO_REPEAT           0x02
        #define     HDMI_PACKET_0_AUTO_REPEAT           0x01
        /*--------------------------------------------------------------*/
    AIT_REG_B   HDMI_CTL_29;                            //0x23
        /*-DEFINE-------------------------------------------------------*/
        #define     HDMI_NULL_PACKET_EN                 0x80
        #define     HDMI_GC_TRANSMISSION_EN             0x40
        #define     HDMI_AVI_INFOFRAME_EN               0x20
        #define     HDMI_ADO_INFOFRAME_EN               0x10
        #define     HDMI_ACR_TRANSMISSION_EN            0x08
        #define     HDMI_AUDIO_TRANSMISSION_EN          0x04
        #define     HDMI_PACKET_1_TRANSMISSION_EN       0x02
        #define     HDMI_PACKET_0_TRANSMISSION_EN       0x01
        /*--------------------------------------------------------------*/
    AIT_REG_B   HDMI_CTL_30;                            //0x24
    AIT_REG_B   HDMI_CTL_31;                            //0x25
        /*-DEFINE-------------------------------------------------------*/
        #define     HDMI_AVMUTE_LEVEL_SENSITIVE         0x20
        #define     HDMI_AVMUTE_EDGE_SENSITIVE          0x00
        #define     HDMI_AVMUTE_FOR_EACH_GC             0x10
        #define     HDMI_AVMUTE_ONLY_ONCE               0x00
        #define     HDMI_ACTIVE_FIELD_FINISHED_TIME     0x00
        #define     HDMI_VSYNC_ACTIVE_TIME              0x01
        #define     HDMI_VSYNC_INACTIVE_TIME            0x02
        #define     HDMI_ACTIVE_FIELD_BEGIN_TIME        0x03
        #define     HDMI_TRANSMISSION_WINDOW_START(_a)  ((_a) << 2)
        #define     HDMI_TRANSMISSION_WINDOW_END(_a)    (_a)
        /*--------------------------------------------------------------*/
    AIT_REG_B   x26[2];
    AIT_REG_B   HDMI_ACR_PARAMETER[6];                  //0x28
    AIT_REG_B   x2E[2];
    AIT_REG_B   HDMI_AVI_INFOFRAME[14];                 //0x30
    AIT_REG_B   x3E[2];
    AIT_REG_B   HDMI_ADO_INFOFRAME[6];                  //0x40
    AIT_REG_B   x46[2];
    AIT_REG_B   HDMI_IEC60958_CH_STS[24];               //0x48
    AIT_REG_B   HDMI_PROGRAMABLE_PACKET_0[31];          //0x60
    AIT_REG_B   HDMI_ECC_PACKET_0[5];                   //0x7F
    AIT_REG_B   HDMI_PROGRAMABLE_PACKET_1[31];          //0x84
    AIT_REG_B   HDMI_ECC_PACKET_1[5];                   //0xA3
    AIT_REG_B   xA8[8];
    AIT_REG_B   HDMI_AUDIO_PACKER_HEADER[4];            //0xB0
    
} AITS_HDMI, *AITPS_HDMI;

//--------------------------------
// HDMI PHY structure (0x80003AA0)
//--------------------------------
typedef struct _AITS_HDMI_PHY {
    AIT_REG_B   HDMI_PHY_TMDS_CTL[7];                       // 0x3AA0
        /*-DEFINE-----------------------------------------------------*/
        /* HDMI_PHY_TMDS_CTL[0] 0x3AA0 */
        #define TDMS_CURRENT_SRC_EN         0x10
        #define TDMS_CURRENT_SRC_DIS        0x00
        #define HDMI_PHY_DATA_OUTPUT_EN     0x20
        #define HDMI_PHY_DATA_OUTPUT_DIS    0x00
        #define TDMS_BUF_POWER_DOWN         0x80
        #define TDMS_BUF_POWER_UP           0x00
        
        /* HDMI_PHY_TMDS_CTL[1] 0x3AA1 */
        #define CK_PN_SWAP_EN               0x01
        #define CK_PN_SWAP_DIS              0x00
        #define CH0_PN_SWAP_EN              0x02
        #define CH0_PN_SWAP_DIS             0x00
        #define CH1_PN_SWAP_EN              0x04
        #define CH1_PN_SWAP_DIS             0x00
        #define CH2_PN_SWAP_EN              0x08
        #define CH2_PN_SWAP_DIS             0x00
        #define HDMI_PHY_RISING_TRIGGER     0x10
        #define HDMI_PHY_FALLING_TRIGGER    0x00
        #define TEST_PARALLEL_DATA_EN       0x20
        #define TEST_PARALLEL_DATA_DIS      0x00
        /* HDMI_PHY_TMDS_CTL[3] 0x3AA3 */
        #define TDMS_TEST_PATTERN_DIS       0x00
        #define TDMS_TEST_PATTERN_EN        0x10
		/*------------------------------------------------------------*/
    AIT_REG_B   HDMI_PHY_LDO_CTL;                           // 0x3AA7
    AIT_REG_D   HDMI_PHY_BIST_CTL;                          // 0x3AA8
        /*-DEFINE-----------------------------------------------------*/
        #define HDMI_LOOPBACK_TEST_DIS      0x00000000
        #define HDMI_LOOPBACK_TEST_EN       0x00000004
        /*------------------------------------------------------------*/
    AIT_REG_W   HDMI_PHY_BANDGAP_CTL;                       // 0x3AAC
        /*-DEFINE-----------------------------------------------------*/
        #define BS_PWR_DOWN                 0x0040
        #define BS_PWR_UP                   0x0000
        #define BG_PWR_DOWN                 0x0080
        #define BG_PWR_UP                   0x0000
        #define BG_OUTPUT                   0x0000
        #define R_DIVIDER_OUTPUT            0x8000
        /*------------------------------------------------------------*/
    AIT_REG_B   HDMI_PHY_PLL_CTL[5];                        // 0x3AAE
        /*-DEFINE-----------------------------------------------------*/
        /* HDMI_PHY_PLL_CTL[0] 0x3AAE */
        #define LOCK_DETECTION              0x00
        #define REPORT_LOCK_STATE           0x01
        #define PERFECT_CP_CURRENT_EN       0x02
        #define PERFECT_CP_CURRENT_DIS      0x00
        #define VCO_TEST_EN                 0x10
        #define VCO_TEST_DIS                0x00
        #define FALLING_TRIGGER_TEST        0x00
        #define RISING_TRIGGER_TEST         0x20
        #define HDMI_PHY_RST_PLL            0x40
        #define HDMI_PHY_PWR_DOWN_PLL       0x80
        
        /* HDMI_PHY_PLL_CTL[1] 0x3AAF */
        #define HDMI_PHY_PHASE0             0x00
        #define HDMI_PHY_PHASE1             0x01
        #define HDMI_PHY_PHASE2             0x02
		/*------------------------------------------------------------*/
    AIT_REG_B   					_x3AB3;
    AIT_REG_D   HDMI_PHY_DATA_LANE_SAMPLE;                  // 0x3AB4
    AIT_REG_D   HDMI_PHY_SR;                                // 0x3AB8
    AIT_REG_B   HDMI_PHY_INT_CPU_EN;                        // 0x3ABC
    AIT_REG_B   HDMI_PHY_INT_CPU_SR;                        // 0x3ABD
    AIT_REG_B   HDMI_PHY_INT_HOST_EN;                       // 0x3ABE
    AIT_REG_B   HDMI_PHY_INT_HOST_SR;                       // 0x3ABF
        /*-DEFINE-----------------------------------------------------*/
        #define HDMI_PLUGIN_INT             0x01
        /*------------------------------------------------------------*/
} AITS_HDMI_PHY, *AITPS_HDMI_PHY;

#if !defined(BUILD_FW)
#define HDMI_CTL_1               	(HDMI_BASE  +(MMP_ULONG)(&(((AITPS_HDMI)0)->HDMI_CTL_1        )))
#define HDMI_CTL_2               	(HDMI_BASE  +(MMP_ULONG)(&(((AITPS_HDMI)0)->HDMI_CTL_2        )))
#define HDMI_CTL_3             		(HDMI_BASE  +(MMP_ULONG)(&(((AITPS_HDMI)0)->HDMI_CTL_3        )))
#define HDMI_CTL_4               	(HDMI_BASE  +(MMP_ULONG)(&(((AITPS_HDMI)0)->HDMI_CTL_4        )))
#define HDMI_CTL_5               	(HDMI_BASE  +(MMP_ULONG)(&(((AITPS_HDMI)0)->HDMI_CTL_5        )))
#define HDMI_CTL_6               	(HDMI_BASE  +(MMP_ULONG)(&(((AITPS_HDMI)0)->HDMI_CTL_6        )))
#define HDMI_CTL_7               	(HDMI_BASE  +(MMP_ULONG)(&(((AITPS_HDMI)0)->HDMI_CTL_7        )))
#define HDMI_CTL_8               	(HDMI_BASE  +(MMP_ULONG)(&(((AITPS_HDMI)0)->HDMI_CTL_8        )))
#define HDMI_CTL_9              	(HDMI_BASE  +(MMP_ULONG)(&(((AITPS_HDMI)0)->HDMI_CTL_9        )))
#define HDMI_CTL_10                	(HDMI_BASE  +(MMP_ULONG)(&(((AITPS_HDMI)0)->HDMI_CTL_10       )))

#define HDMI_SKEW_NUM_VBLANK      	(HDMI_BASE  +(MMP_ULONG)(&(((AITPS_HDMI)0)->HDMI_SKEW_NUM_VBLANK        )))
#define HDMI_PACKET_NUM_VBLANK     	(HDMI_BASE  +(MMP_ULONG)(&(((AITPS_HDMI)0)->HDMI_PACKET_NUM_VBLANK      )))
#define HDMI_TYPE_I_DATA_NUM       	(HDMI_BASE  +(MMP_ULONG)(&(((AITPS_HDMI)0)->HDMI_TYPE_I_DATA_NUM        )))
#define HDMI_SKEW_NUM_HBLANK      	(HDMI_BASE  +(MMP_ULONG)(&(((AITPS_HDMI)0)->HDMI_SKEW_NUM_HBLANK        )))
#define HDMI_PACKET_NUM_HBLANK    	(HDMI_BASE  +(MMP_ULONG)(&(((AITPS_HDMI)0)->HDMI_PACKET_NUM_HBLANK      )))
#define HDMI_SPACING_NUM           	(HDMI_BASE  +(MMP_ULONG)(&(((AITPS_HDMI)0)->HDMI_SPACING_NUM            )))
#define HDMI_SYNC_POLARITY      	(HDMI_BASE  +(MMP_ULONG)(&(((AITPS_HDMI)0)->HDMI_SYNC_POLARITY          )))
#define HDMI_V_RESOLUTION_MSB  		(HDMI_BASE  +(MMP_ULONG)(&(((AITPS_HDMI)0)->HDMI_V_RESOLUTION_MSB       )))
#define HDMI_V_RESOLUTION_LSB    	(HDMI_BASE  +(MMP_ULONG)(&(((AITPS_HDMI)0)->HDMI_V_RESOLUTION_LSB       )))
#define HDMI_H_RESOLUTION_MSB    	(HDMI_BASE  +(MMP_ULONG)(&(((AITPS_HDMI)0)->HDMI_H_RESOLUTION_MSB       )))
#define HDMI_H_RESOLUTION_LSB    	(HDMI_BASE  +(MMP_ULONG)(&(((AITPS_HDMI)0)->HDMI_H_RESOLUTION_LSB       )))
#define HDMI_V_BACK_PORCH_MSB    	(HDMI_BASE  +(MMP_ULONG)(&(((AITPS_HDMI)0)->HDMI_V_BACK_PORCH_MSB       )))
#define HDMI_V_BACK_PORCH_LSB    	(HDMI_BASE  +(MMP_ULONG)(&(((AITPS_HDMI)0)->HDMI_V_BACK_PORCH_LSB       )))
#define HDMI_H_BACK_PORCH_MSB   	(HDMI_BASE  +(MMP_ULONG)(&(((AITPS_HDMI)0)->HDMI_H_BACK_PORCH_MSB       )))
#define HDMI_H_BACK_PORCH_LSB      	(HDMI_BASE  +(MMP_ULONG)(&(((AITPS_HDMI)0)->HDMI_H_BACK_PORCH_LSB       )))

#define HDMI_PACKET_1_STS         	(HDMI_BASE  +(MMP_ULONG)(&(((AITPS_HDMI)0)->HDMI_PACKET_1_STS  )))
#define HDMI_PACKET_0_STS        	(HDMI_BASE  +(MMP_ULONG)(&(((AITPS_HDMI)0)->HDMI_PACKET_0_STS  )))
#define HDMI_CTL_28                	(HDMI_BASE  +(MMP_ULONG)(&(((AITPS_HDMI)0)->HDMI_CTL_28        )))
#define HDMI_CTL_29               	(HDMI_BASE  +(MMP_ULONG)(&(((AITPS_HDMI)0)->HDMI_CTL_29        )))
#define HDMI_CTL_30             	(HDMI_BASE  +(MMP_ULONG)(&(((AITPS_HDMI)0)->HDMI_CTL_30        )))
#define HDMI_CTL_31              	(HDMI_BASE  +(MMP_ULONG)(&(((AITPS_HDMI)0)->HDMI_CTL_31        )))

#define HDMI_ACR_PARAMETER        	(HDMI_BASE  +(MMP_ULONG)(&(((AITPS_HDMI)0)->HDMI_ACR_PARAMETER[0]          )))
#define HDMI_AVI_INFOFRAME       	(HDMI_BASE  +(MMP_ULONG)(&(((AITPS_HDMI)0)->HDMI_AVI_INFOFRAME[0]          )))
#define HDMI_ADO_INFOFRAME       	(HDMI_BASE  +(MMP_ULONG)(&(((AITPS_HDMI)0)->HDMI_ADO_INFOFRAME[0]          )))
#define HDMI_IEC60958_CH_STS     	(HDMI_BASE  +(MMP_ULONG)(&(((AITPS_HDMI)0)->HDMI_IEC60958_CH_STS[0]        )))
#define HDMI_PROGRAMABLE_PACKET_0 	(HDMI_BASE  +(MMP_ULONG)(&(((AITPS_HDMI)0)->HDMI_PROGRAMABLE_PACKET_0[0]   )))
#define HDMI_ECC_PACKET_0          	(HDMI_BASE  +(MMP_ULONG)(&(((AITPS_HDMI)0)->HDMI_ECC_PACKET_0[0]           )))
#define HDMI_PROGRAMABLE_PACKET_1  	(HDMI_BASE  +(MMP_ULONG)(&(((AITPS_HDMI)0)->HDMI_PROGRAMABLE_PACKET_1[0]   )))
#define HDMI_ECC_PACKET_1          	(HDMI_BASE  +(MMP_ULONG)(&(((AITPS_HDMI)0)->HDMI_ECC_PACKET_1[0]           )))
#define HDMI_AUDIO_PACKER_HEADER   	(HDMI_BASE  +(MMP_ULONG)(&(((AITPS_HDMI)0)->HDMI_AUDIO_PACKER_HEADER[0]    )))

#define HDMI_PHY_TMDS_CTL0        	(HDMI_PHY_BASE +(MMP_ULONG)(&(((AITPS_HDMI_PHY)0)->HDMI_PHY_TMDS_CTL[0] )))
#define HDMI_PHY_TMDS_CTL1       	(HDMI_PHY_BASE +(MMP_ULONG)(&(((AITPS_HDMI_PHY)0)->HDMI_PHY_TMDS_CTL[1] )))
#define HDMI_PHY_TMDS_CTL4        	(HDMI_PHY_BASE +(MMP_ULONG)(&(((AITPS_HDMI_PHY)0)->HDMI_PHY_TMDS_CTL[4] )))
#define HDMI_PHY_TMDS_CTL5       	(HDMI_PHY_BASE +(MMP_ULONG)(&(((AITPS_HDMI_PHY)0)->HDMI_PHY_TMDS_CTL[5] )))
#define HDMI_PHY_BIST_CTL        	(HDMI_PHY_BASE +(MMP_ULONG)(&(((AITPS_HDMI_PHY)0)->HDMI_PHY_BIST_CTL    )))
#define HDMI_PHY_BANDGAP_CTL        (HDMI_PHY_BASE +(MMP_ULONG)(&(((AITPS_HDMI_PHY)0)->HDMI_PHY_BANDGAP_CTL )))
#define HDMI_PHY_PLL_CTL0           (HDMI_PHY_BASE +(MMP_ULONG)(&(((AITPS_HDMI_PHY)0)->HDMI_PHY_PLL_CTL[0]  )))
#define HDMI_PHY_PLL_CTL1           (HDMI_PHY_BASE +(MMP_ULONG)(&(((AITPS_HDMI_PHY)0)->HDMI_PHY_PLL_CTL[1]  )))
#define HDMI_PHY_PLL_CTL3           (HDMI_PHY_BASE +(MMP_ULONG)(&(((AITPS_HDMI_PHY)0)->HDMI_PHY_PLL_CTL[3]  )))
#define HDMI_PHY_PLL_CTL4           (HDMI_PHY_BASE +(MMP_ULONG)(&(((AITPS_HDMI_PHY)0)->HDMI_PHY_PLL_CTL[4]  )))
#define HDMI_PHY_PLL_CTL5           (HDMI_PHY_BASE +(MMP_ULONG)(&(((AITPS_HDMI_PHY)0)->HDMI_PHY_PLL_CTL[5]  )))
#define HDMI_PHY_INT_CPU_EN         (HDMI_PHY_BASE +(MMP_ULONG)(&(((AITPS_HDMI_PHY)0)->HDMI_PHY_INT_CPU_EN  )))
#define HDMI_PHY_INT_CPU_SR         (HDMI_PHY_BASE +(MMP_ULONG)(&(((AITPS_HDMI_PHY)0)->HDMI_PHY_INT_CPU_SR  )))
#define HDMI_PHY_INT_HOST_EN        (HDMI_PHY_BASE +(MMP_ULONG)(&(((AITPS_HDMI_PHY)0)->HDMI_PHY_INT_HOST_EN )))
#define HDMI_PHY_INT_HOST_SR        (HDMI_PHY_BASE +(MMP_ULONG)(&(((AITPS_HDMI_PHY)0)->HDMI_PHY_INT_HOST_SR )))

#endif // end of !defined(BUILD_FW)

#endif // end of _MMP_REG_HDMI_H_
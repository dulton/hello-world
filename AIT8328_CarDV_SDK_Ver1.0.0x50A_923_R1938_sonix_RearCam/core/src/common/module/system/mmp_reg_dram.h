//==============================================================================
//
//  File        : mmp_reg_dram.h
//  Description : Dram controller register map
//  Author      : Hans
//  Revision    : 1.0
//
//==============================================================================
/** @addtogroup MMPH_reg
@{
*/
#ifndef _MMP_REG_DRAM_H_
#define _MMP_REG_DRAM_H_

#include "mmp_register.h"

// *********************************
//   DRAM  Structure (0x8000 6E00)
// *********************************
#if (CHIP == MCR_V2)
typedef struct _AITS_DRAM {
  	AIT_REG_D   DRAM_INT_CPU_EN;                                        // 0x6E00
    AIT_REG_D   DRAM_INT_CPU_SR;										// 0x6E04
    AIT_REG_D   DRAM_INT_HOST_EN;                                       // 0x6E08
    AIT_REG_D   DRAM_INT_HOST_SR;                                       // 0x6E0C
        /*-DEFINE-----------------------------------------------------*/
        #define DRAM_ARF_DONE            	0x01
        #define DRAM_WAKUP_DONE             0x02
        #define DRAM_EXIT_DPD_DONE          0x04
        #define DRAM_DPD_DONE               0x08
        #define DRAM_LME_DONE               0x10
        #define DRAM_LM_DONE                0x20
        #define DRAM_EXIT_SRF_DONE          0x40
        #define DRAM_SRF_DONE               0x80
        #define DRAM_PRE_ALL_DONE           0x0100
        #define DRAM_INIT_DONE              0x0200
        #define DRAM_CMD_FINISH             0x0400
        #define DRAM_CLBT_DONE_INT       			0x0800
        #define DRAM_EXT_LOAD_MODE_3_CMD_DONE_INT	0x1000
        #define DRAM_EXT_LOAD_MODE_2_CMD_DONE_INT	0x2000
        #define DRAM_CLBT_DONE_ACK_INT       		0x4000
        #define DRAM_CLBT_EXIT_MODE_ACK_INT     	0x8000
        /*------------------------------------------------------------*/
    AIT_REG_B   DRAM_CTL_0;												// 0x6E10
        /*-DEFINE-----------------------------------------------------*/
        #define BANK_SIZE_1M	            0x00
        #define BANK_SIZE_2M	            0x10
        #define BANK_SIZE_4M	            0x20
        #define BANK_SIZE_8M	            0x30
        #define DDR_MODE					0x80
        #define DDR2_MODE                   0x40
        #define DDR3_MODE                   0x08
        #define DRAM_CAS_LATENCY_1			0x01        
        #define DRAM_CAS_LATENCY_2			0x02        
        #define DRAM_CAS_LATENCY_3			0x03
        #define DRAM_CAS_LATENCY_4			0x04        
        #define DRAM_CAS_LATENCY_5			0x05        
        #define DRAM_CAS_LATENCY_6			0x06        
        #define DRAM_CAS_LATENCY_7			0x07        
        /*------------------------------------------------------------*/
    AIT_REG_B   DRAM_CTL_1;												// 0x6E11
        /*-DEFINE-----------------------------------------------------*/
        #define DRAM_BUS_32		            0x1
        #define DRAM_BUS_16	            	0x0
        #define DRAM_BANK_SEQ               0x4
        #define DRAM_BANK_INTLV           	0x0
        #define DRAM_LOAD_MODE_FIRST        0x80
        #define DRAM_EXT_MODE_FIRST         0x00
        /*------------------------------------------------------------*/
    AIT_REG_B   DRAM_CTL_2;												// 0x6E12
        /*-DEFINE-----------------------------------------------------*/
        #define DRAM_COL_128				0x0 //2^7
        #define DRAM_COL_256				0x1 //2^8
        #define DRAM_COL_512				0x2 //2^9
        #define DRAM_COL_1024				0x3 //2^10
        #define DRAM_COL_2048				0x4 //2^11
        
        #define DRAM_ROW_1024	       		0x00 //2^10
        #define DRAM_ROW_2048	       		0x10 //2^11
        #define DRAM_ROW_4096	       		0x20 //2^12
        #define DRAM_ROW_8192	       		0x30 //2^13
        #define DRAM_ROW_16384	       		0x40 //2^14
        /*------------------------------------------------------------*/
    AIT_REG_B   						_x13;
    AIT_REG_B   DRAM_CTL_4;												// 0x6E14
        /*-DEFINE-----------------------------------------------------*/
        #define DRAM_SW_INIT_ST		       	0x2
        #define DRAM_INIT_ST		       	0x1
        #define DRAM_INIT_CLEAN				0x00
        #define DRAM_CLBT_EN				0x4
        #define DRAM_AUTO_CLBT_MODE			0x8
        /*------------------------------------------------------------*/
    AIT_REG_B	DRAM_SW_REST_CTL;										// 0x6E15
    AIT_REG_B	DRAM_AUTO_CLBT_TIMER;									// 0x6E16
    AIT_REG_B   						_x17;
    AIT_REG_W   DRAM_FUNC;												// 0x6E18
        /*-DEFINE-----------------------------------------------------*/
        #define DRAM_ARF_EN			       	0x0001
        #define DRAM_APD_EN			       	0x0002
        #define DRAM_PMP_EN			       	0x0004
        #define DRAM_SPRE_EN      			0x0008
        #define DRAM_NA_EN      			0x0010
        #define DRAM_CLK_GATE_EN      		0x0020
        #define DRAM_CMD_ISSUE_MODE    		0x0040
        #define DRAM_MCI_REQ_DIS   			0x0080
        #define DRAM_EH_SPRE_EN			    0x0100
		#define DRAM_DDR_RW_WAIT_EN	    	0x0400
        /*------------------------------------------------------------*/
    AIT_REG_B   DRAM_SIGNAL_CTL;                                        // 0x6E1A
	    /*-DEFINE-----------------------------------------------------*/
	    #define DRAM_CLK_OUT_INV_EN         0x01
	    #define DRAM_CLK_OUT_DLY_EN			0x02
	    #define DRAM_CLK_IN_INV_EN          0x04
	    #define DRAM_CLK_IN_DLY_EN			0x08
	    #define DRAM_CYC_DLY_EN  			0x10
	    #define DRAM_DFI_RD_EN              0x20
	    #define DRAM_DFI_RD_RESYNC_EN       0x40
	    /*------------------------------------------------------------*/
    AIT_REG_B   DRAM_DFI_CTL;											// 0x6E1B
	    /*-DEFINE-----------------------------------------------------*/
        #define DRAM_RD_ASYNC_EN         	0x01
        #define DRAM_RD_DIRTYDROP_EN        0x02
        #define DRAM_SW_RST                 0x04
        #define DRAM_DLY_RDQS_EN            0x08
        #define DRAM_VLD_ST_EN              0x10
        #define DRAM_INIT_EN                0x20
        /*------------------------------------------------------------*/
    AIT_REG_W   DRAM_ARF_CYC;											// 0x6E1C
    AIT_REG_B   DRAM_ARF_QUEUE_DEPTH;									// 0x6E1E
        /*-DEFINE-----------------------------------------------------*/
        #define DRAM_DDR_CMD_DLY_1T			0x00
        #define DRAM_DDR_CMD_DLY_1P5T		0x10
        #define DRAM_DDR_CMD_DLY_2T			0x20
        #define DRAM_DDR2_CMD_DLY_1T		0x00
        #define DRAM_DDR2_CMD_DLY_1P5T		0x40
        #define DRAM_DDR2_CMD_DLY_2T		0x80
        #define DRAM_DDR2_CMD_DLY_2P5T		0xC0
        /*-DEFINE-----------------------------------------------------*/
    AIT_REG_B   DRAM_DDR_DDR2_RD_DLY;									// 0x6E1F
        /*-DEFINE-----------------------------------------------------*/
        #define DRAM_DDR_DQ_RD_DLY_0T		0x00
        #define DRAM_DDR_DQ_RD_DLY_0P5T		0x01
        #define DRAM_DDR_DQ_RD_DLY_1T		0x02
        #define DRAM_DDR_DQ_RD_DLY_1P5T		0x03
        #define DRAM_DDR_DQ_RD_DLY_2T		0x04
        #define DRAM_DDR_DQ_RD_DLY_2P5T		0x05
        #define DRAM_DDR_DQ_RD_DLY_3T		0x06
        #define DRAM_DDR_DQ_RD_DLY_3P5T		0x07
        #define DRAM_DDR_DQ_RD_DLY_4T		0x08
        #define DRAM_DDR_DQ_RD_DLY_4P5T		0x09
        #define DRAM_DDR_DQ_RD_DLY_5T		0x0A
        #define DRAM_DDR2_DQ_RD_DLY_0T		0x00
        #define DRAM_DDR2_DQ_RD_DLY_0P5T	0x10
        #define DRAM_DDR2_DQ_RD_DLY_1T		0x20
        #define DRAM_DDR2_DQ_RD_DLY_1P5T	0x30
        #define DRAM_DDR2_DQ_RD_DLY_2T		0x40
        #define DRAM_DDR2_DQ_RD_DLY_2P5T	0x50
        #define DRAM_DDR2_DQ_RD_DLY_3T		0x60
        #define DRAM_DDR2_DQ_RD_DLY_3P5T	0x70
        #define DRAM_DDR2_DQ_RD_DLY_4T		0x80
        #define DRAM_DDR2_DQ_RD_DLY_4P5T	0x90
        #define DRAM_DDR2_DQ_RD_DLY_5T		0xA0
        /*-DEFINE-----------------------------------------------------*/
    AIT_REG_D   DRAM_CTL_CYC_0;											// 0x6E20;
        /*-DEFINE-----------------------------------------------------*/
        #define DRAM_TRAS_CYC(_a)			(_a)
        #define DRAM_TRC_SHFT		       	4
        #define DRAM_TRC_CYC(_a)			(MMP_ULONG)(_a << DRAM_TRC_SHFT)
        // 6E21
        #define DRAM_TRCD_SHFT		       	8
        #define DRAM_TRCD_CYC(_a)			(MMP_ULONG)(_a << DRAM_TRCD_SHFT)
        #define DRAM_TRFC_SHFT		       	12
        #define DRAM_TRFC_CYC(_a)			(MMP_ULONG)((_a & 0xF) << DRAM_TRFC_SHFT)
        // 6E22
        #define DRAM_TRP_SHFT		       	16
        #define DRAM_TRP_CYC(_a)			(MMP_ULONG)(_a << DRAM_TRP_SHFT)
        #define DRAM_TRRD_SHFT		       	20
        #define DRAM_TRRD_CYC(_a)			(MMP_ULONG)(_a << DRAM_TRRD_SHFT)
        // 6E23
        #define DRAM_TXSR_SHFT		       	24
        #define DRAM_TXSR_CYC(_a)			(MMP_ULONG)((_a & 0xF) << DRAM_TXSR_SHFT)
        #define DRAM_TMRD_SHFT		       	28
        #define DRAM_TMRD_CYC(_a)			((MMP_ULONG)_a << DRAM_TMRD_SHFT)
        /*------------------------------------------------------------*/
    AIT_REG_B   DRAM_CTL_CYC_1;											// 0x6E24
        /*-DEFINE-----------------------------------------------------*/
        #define DRAM_TWR_CYC(_a)			(_a)
        #define DRAM_TRTP_SHFT		        4
        #define DRAM_TRTP_CYC(_a)		    (_a << DRAM_TRTP_SHFT)
        /*------------------------------------------------------------*/
    AIT_REG_B   DRAM_CTL_CYC_DDR;                                       // 0x6E25
        /*-DEFINE-----------------------------------------------------*/
        #define DRAM_TXP_CYC(_a)			((MMP_ULONG)_a)
        #define DRAM_TWTR_SHFT		       	4
        #define DRAM_TWTR_CYC(_a)			((MMP_ULONG)_a << DRAM_TWTR_SHFT)
        /*------------------------------------------------------------*/
    AIT_REG_W   DRAM_DDR2_TIMING_CTRL;                                  // 0x6E26
    AIT_REG_W   DRAM_MODE_REG;											// 0x6E28
    AIT_REG_W   DRAM_EXT_MODE_REG;										// 0x6E2A
    AIT_REG_B   DRAM_RDDATA_CLK_DLY;									// 0x6E2C
    AIT_REG_B   DRAM_CLK_OUT_DLY;										// 0x6E2D
    AIT_REG_B   DRAM_RDDATA_RDY_DLY;									// 0x6E2E
    AIT_REG_B   DRAM_EXT_LOAD_MODE;										// 0x6E2F
        /*-DEFINE-----------------------------------------------------*/
        #define DRAM_EXT_LOAD_MODE_3_EN		0x02
        #define DRAM_EXT_LOAD_MODE_2_EN		0x01
        /*-DEFINE-----------------------------------------------------*/
    AIT_REG_W   DRAM_CMD_CTL0;											// 0x6E30
        /*-DEFINE-----------------------------------------------------*/
        #define DRAM_ARF_ST					0x0001
        #define DRAM_PUP_RDY_ST				0x0002
        #define DRAM_EXIT_DPD_ST			0x0004
        #define DRAM_DPD_ST					0x0008
        #define DRAM_LME_ST					0x0010
        #define DRAM_LM_ST					0x0020
        #define DRAM_EXIT_SRF_ST			0x0040
        #define DRAM_SRF_ST					0x0080
        #define DRAM_PRE_ALL_ST				0x0100
        /*------------------------------------------------------------*/
    AIT_REG_B   DRAM_CMD_CTL1;											// 0x6E32
    AIT_REG_B   DRAM_CMD_CTL2;											// 0x6E33
    AIT_REG_B   DRAM_INIT_CYC;                                       	// 0x6E34
    AIT_REG_B   DRAM_DPD_CYC;                                       	// 0x6E35
    AIT_REG_W   DRAM_CTL_CYC_EXT;                                       // 0x6E36
        /*-DEFINE-----------------------------------------------------*/
        #define DRAM_TRAS_EXT_CYC(_a)       ((_a & 0x10) >> 4)
        #define DRAM_TRC_EXT_SHFT           1
        #define DRAM_TRC_EXT_CYC(_a)        (MMP_USHORT)(((_a & 0x10) >> 4) << DRAM_TRC_EXT_SHFT)
        #define DRAM_TRCD_EXT_SHFT          2
        #define DRAM_TRCD_EXT_CYC(_a)       (MMP_USHORT)(((_a & 0x10) >> 4) << DRAM_TRCD_EXT_SHFT)
        #define DRAM_TRFC_EXT_SHFT          3
        #define DRAM_TRFC_EXT_CYC(_a)       (MMP_USHORT)(((_a & 0x10) >> 4) << DRAM_TRFC_EXT_SHFT)
        #define DRAM_TRP_EXT_SHFT           4
        #define DRAM_TRP_EXT_CYC(_a)        (MMP_USHORT)(((_a & 0x10) >> 4) << DRAM_TRP_EXT_SHFT)
        #define DRAM_TRRD_EXT_SHFT          5
        #define DRAM_TRRD_EXT_CYC(_a)       (MMP_USHORT)(((_a & 0x10) >> 4) << DRAM_TRRD_EXT_SHFT)
        #define DRAM_TXSR_EXT_SHFT          6
        #define DRAM_TXSR_EXT_CYC(_a)       (MMP_USHORT)(((_a & 0x10) >> 4) << DRAM_TXSR_EXT_SHFT)
        #define DRAM_TMRD_EXT_SHFT          7
        #define DRAM_TMRD_EXT_CYC(_a)       (MMP_USHORT)(((_a & 0x10) >> 4) << DRAM_TMRD_EXT_SHFT)
        #define DRAM_TWR_EXT_SHFT           8
        #define DRAM_TWR_EXT_CYC(_a)        (MMP_USHORT)(((_a & 0x10) >> 4) << DRAM_TWR_EXT_SHFT)
        #define DRAM_TXP_EXT_SHFT           10
        #define DRAM_TXP_EXT_CYC(_a)        (MMP_USHORT)(((_a & 0x10) >> 4) << DRAM_TXP_EXT_SHFT)
        #define DRAM_TWTR_EXT_SHFT          11
        #define DRAM_TWTR_EXT_CYC(_a)       (MMP_USHORT)(((_a & 0x10) >> 4) << DRAM_TWTR_EXT_SHFT)
        #define DRAM_TADJ_EXT_SHFT          12
        #define DRAM_TADJ_EXT_CYC(_a)       (MMP_USHORT)(((_a & 0x10) >> 4) << DRAM_TADJ_EXT_SHFT)
        #define DRAM_TOIT_EXT_SHFT          13
        #define DRAM_TOIT_EXT_CYC(_a)       (MMP_USHORT)(((_a & 0x10) >> 4) << DRAM_TOIT_EXT_SHFT)
		#define DRAM_TRFC_EXT_SHFT_2        14
		#define DRAM_TRFC_EXT_CYC_2(_a)     (MMP_USHORT)(((_a & 0x60) >> 5) << DRAM_TRFC_EXT_SHFT_2)         
        /*------------------------------------------------------------*/
    AIT_REG_B	DRAM_CMD_QUEUE_SR;										//0x6E38
    AIT_REG_B   DRAM_RW_LATENCY0;                                       //0x6E39    
    	/*-DEFINE-----------------------------------------------------*/
 		#define DRAM_WR_FRACDLY_DIS         0x00
 		#define DRAM_WR_FRACDLY_EN          0x01
 		#define DRAM_RD_FRACDLY_DIS         0x00
 		#define DRAM_RD_FRACDLY_EN          0x02
 	AIT_REG_B   DRAM_RW_LATENCY1;                                       //0x6E3A
 		/*-DEFINE-----------------------------------------------------*/	
 		#define DRAM_WL_DLY(_a)			    (MMP_ULONG)(_a)      
        #define DRAM_RL_DLY(_a)			    (MMP_ULONG)(_a << 3)
    	/*------------------------------------------------------------*/
    AIT_REG_B   						_x3B[0x15];  
    AIT_REG_B	DRAM_DFI_UPDATE_MIN_CYC;								//0x6E50
    AIT_REG_B	DRAM_DFI_UPDATE_MAX_CYC;								//0x6E51  
    AIT_REG_B   						_x52[0xE];
    AIT_REG_B	DRAM_DDR_DLL_REST_LB_CTL;								// 0x6E60
		/*-DEFINE-----------------------------------------------------*/				
		#define DRAM_DDR_DLL1_RST           	0x01
		#define DRAM_DDR_DLL1_PWR_DOWN      	0x02
		#define DRAM_DQBLK0_W_R_LB_TEST_MODE_EN	0x10
		#define DRAM_DQBLK1_W_R_LB_TEST_MODE_EN	0x20
		#define DRAM_DQBLK2_W_R_LB_TEST_MODE_EN	0x40
		#define DRAM_DQBLK3_W_R_LB_TEST_MODE_EN	0x80
		/*------------------------------------------------------------*/
	AIT_REG_B	DRAM_DDR_DQPLK_PD_CTL;									// 0x6E61
		/*-DEFINE-----------------------------------------------------*/
		#define DRAM_DQBLK_DDR_MODE			0x00
		#define DRAM_DQBLK_SDR_MODE			0xF0
		#define DRAM_DQBLK_PWR_DN			0x0F
		#define DRAM_DQBLK_NML_OPT			0x00
		/*-DEFINE-----------------------------------------------------*/
	AIT_REG_B   DRAM_DDR2_MODE_CLK_EN;									// 0x6E62
	AIT_REG_B	DRAM_DDR2_ODT_EN;										// 0x6E63
		/*-DEFINE-----------------------------------------------------*/
        #define DDR2_DQBLK0_ODT_EN			0x01;
        #define DDR2_DQBLK1_ODT_EN			0x02;
        #define DDR2_DQBLK2_ODT_EN			0x04;
        #define DDR2_DQBLK3_ODT_EN			0x08;
        /*------------------------------------------------------------*/
    AIT_REG_B   						_x64[3];    
    AIT_REG_B	DRAM_DDR_DLL1_CTL;										// 0x6E67
    	/*-DEFINE-----------------------------------------------------*/
        #define DDR3_DQ0_ODT_EN             0x01
        #define DDR3_DQ1_ODT_EN             0x02
        #define DDR3_DQ2_ODT_EN             0x03
        #define DDR3_DQ3_ODT_EN             0x08
        /*------------------------------------------------------------*/
	AIT_REG_B	DRAM_DDR_DLL1_OPT;										// 0x6E68
	AIT_REG_B	DRAM_DDR_DLL1_LOCK_CTL;									// 0x6E69
		/*-DEFINE-----------------------------------------------------*/
        #define DDR_LOCK_REF_CLK_CNT(_a)    (_a & 0x1F)
        /*------------------------------------------------------------*/
	AIT_REG_B	DRAM_DDR_DLL1_LOCK_SR;									// 0x6E6A
	AIT_REG_B	DRAM_DDR_CLOCK_OPT;										// 0x6E6B
		/*-DEFINE-----------------------------------------------------*/
        #define DQCLK_DUTY_BYPASS      		0x08
        /*------------------------------------------------------------*/
    AIT_REG_B	DRAM_DDR_DLL1_CTL_OPT_HIGH_BIT;							// 0x6E6C
	AIT_REG_B   						_x6D[0x03];						// 0x6E6D~0x6F reserved
	AIT_REG_W   DRAM_DDR_CLK_MACRO_DLY;									// 0x6E70
	AIT_REG_B   DRAM_DDR_DDR2_OPR_CTL;									// 0x6E72
		/*-DEFINE-----------------------------------------------------*/
        #define DRAM_DDR_TOPCLK_EN     		0x08
        #define DRAM_DDR_TOPCLK_DDR_MODE    0x00
        #define DRAM_DDR_TOPCLK_SDR_MODE    0x10
        #define DRAM_DDR_BOTCLK_EN     		0x20
        #define DRAM_DDR_BOTCLK_DDR_MODE    0x00
        #define DRAM_DDR_BOTCLK_SDR_MODE    0x40
        /*------------------------------------------------------------*/
	AIT_REG_B   						_x73;
	AIT_REG_D   DRAM_SRR_DATA_BYTE;										// 0x6E74
	AIT_REG_B   						_x78[8];                        // 0x6E78~0x6E7F
	AIT_REG_B	DRAM_DQBLK0_RD_QCDL_CTL;								// 0x6E80
    AIT_REG_B	DRAM_DQBLK0_WR_QCDL_CTL;								// 0x6E81
    AIT_REG_B	DRAM_DQBLK0_RDAS_CTL;									// 0x6E82
    AIT_REG_B	DRAM_DQBLK1_RD_QCDL_CTL;								// 0x6E83
    AIT_REG_B	DRAM_DQBLK1_WR_QCDL_CTL;								// 0x6E84
    AIT_REG_B	DRAM_DQBLK1_RDAS_CTL;									// 0x6E85
    AIT_REG_B	DRAM_DQBLK2_RD_QCDL_CTL;								// 0x6E86
    AIT_REG_B	DRAM_DQBLK2_WR_QCDL_CTL;								// 0x6E87
    AIT_REG_B	DRAM_DQBLK2_RDAS_CTL;									// 0x6E88
    AIT_REG_B	DRAM_DQBLK3_RD_QCDL_CTL;								// 0x6E89
    AIT_REG_B	DRAM_DQBLK3_WR_QCDL_CTL;								// 0x6E8A
    AIT_REG_B	DRAM_DQBLK3_RDAS_CTL;									// 0x6E8B
    AIT_REG_B	DRAM_DQBLK_0_1_W_R_QCDL_RDAS_CTL_HIGH_BIT;				// 0x6E8C
    AIT_REG_B	DRAM_DQBLK_1_2_W_R_QCDL_RDAS_CTL_HIGH_BIT;				// 0x6E8D
    AIT_REG_B	DRAM_DQBLK_2_3_W_R_QCDL_RDAS_CTL_HIGH_BIT;				// 0x6E8E
    AIT_REG_B	DRAM_SHD_REG_CTL;										// 0x6E8F
    
    AIT_REG_B	DRAM_DDR_DQBLK0_SYNC_DLY_TUNE;							// 0x6E90
    AIT_REG_B	DRAM_DDR_DQBLK0_CTL_OPT;								// 0x6E91
    AIT_REG_B	DRAM_DDR_DQBLK0_CTL0;									// 0x6E92
    AIT_REG_B	DRAM_DDR_DQBLK0_CTL1;									// 0x6E93
    AIT_REG_B	DRAM_DDR_DQBLK0_PAD_CTL_OPT;							// 0x6E94
    
    AIT_REG_B	DRAM_DDR_DQBLK1_SYNC_DLY_TUNE;							// 0x6E95
    AIT_REG_B	DRAM_DDR_DQBLK1_CTL_OPT;								// 0x6E96
    AIT_REG_B	DRAM_DDR_DQBLK1_CTL0;									// 0x6E97
    AIT_REG_B	DRAM_DDR_DQBLK1_CTL1;									// 0x6E98
    AIT_REG_B	DRAM_DDR_DQBLK1_PAD_CTL_OPT;							// 0x6E99
    
    AIT_REG_B	DRAM_DDR_DQBLK2_SYNC_DLY_TUNE;							// 0x6E9A
    AIT_REG_B	DRAM_DDR_DQBLK2_CTL_OPT;								// 0x6E9B
    AIT_REG_B	DRAM_DDR_DQBLK2_CTL0;									// 0x6E9C
    AIT_REG_B	DRAM_DDR_DQBLK2_CTL1;									// 0x6E9D
    AIT_REG_B	DRAM_DDR_DQBLK2_PAD_CTL_OPT;							// 0x6E9E
    
    AIT_REG_B	DRAM_DDR_DQBLK3_SYNC_DLY_TUNE;							// 0x6E9F
    AIT_REG_B	DRAM_DDR_DQBLK3_CTL_OPT;								// 0x6EA0
    AIT_REG_B	DRAM_DDR_DQBLK3_CTL0;									// 0x6EA1
    AIT_REG_B	DRAM_DDR_DQBLK3_CTL1;									// 0x6EA2
    AIT_REG_B	DRAM_DDR_DQBLK3_PAD_CTL_OPT;							// 0x6EA3
    
    AIT_REG_B	DRAM_TOP_DDR_CLK_MACRO_CLK_CTL;							// 0x6EA4
    AIT_REG_B	DRAM_BOTTOM_DDR_CLK_MACRO_CLK_CTL;						// 0x6EA5
    AIT_REG_B	DRAM_TOP_ZQ_PMOS;						                // 0x6EA6
    AIT_REG_B   						_xA7[9];
    AIT_REG_B	DRAM_EXT_LOAD_MODE_2_DATA_0;							// 0x6EB0
    AIT_REG_B	DRAM_EXT_LOAD_MODE_2_DATA_1;							// 0x6EB1
    AIT_REG_B	DRAM_EXT_LOAD_MODE_3_DATA_0;							// 0x6EB2
    AIT_REG_B	DRAM_EXT_LOAD_MODE_3_DATA_1;							// 0x6EB3
    AIT_REG_B	DRAM_CLBT_DRIVE_MODE_ADJ_DATA;							// 0x6EB4
    AIT_REG_B	DRAM_DDR2_INT_CNT0;										// 0x6EB5
    AIT_REG_B	DRAM_DDR2_INT_CNT1;										// 0x6EB6
    AIT_REG_B	DRAM_DDR2_OPT;											// 0x6EB7
    	/*-DEFINE-----------------------------------------------------*/
    	#define DRAM_DDR_DDR2_DLL_EN        0x01
    	#define DRAM_DDR_DFIPHY_ODT_EN      0x04
    	#define DRAM_DDR2_RD_SYNC_EN        0x08
        #define DRAM_DDR_DFIPHY_CLK_EN     	0x10
        #define DRAM_DDR_DFIPHY_CL5_EN     	0x20
        // ++MCR_V2_MP Only
        #define DRAM_DDR_USE_DDR3_RD_CTL    0x80
        // --MCR_V2_MP Only
        /*------------------------------------------------------------*/
    AIT_REG_B   DRAM_DDR2_ODT_OPT;                                      // 0x6EB8
    AIT_REG_B   						_xB9[0x1];                      // 0x6EB9
    AIT_REG_B   DRAM_DDR_DQBLK0_DLY_CTL[0x05];                          // 0x6EBA~0x6EBE
    	/*-DEFINE-----------------------------------------------------*/
    	// ++MCR_V2_MP Only
    	// --MCR_V2_MP Only
    	/*------------------------------------------------------------*/
    AIT_REG_B   						_xBF[0x1];                      // 0x6EBF
    
    AIT_REG_B   DRAM_DDR_DQBLK0_ODT_CTL0;								// 0x6EC0
    AIT_REG_B   DRAM_DDR_DQBLK0_ODT_CTL1;								// 0x6EC1
    AIT_REG_B	DRAM_DDR_DQBLK0_ODT_SR;                                 // 0x6EC2
    AIT_REG_B   DRAM_DDR_DQBLK0_CAL_EN;                                 // 0x6EC3
    AIT_REG_B   DRAM_DDR_DQBLK0_RG_CTL;                                 // 0x6EC4
    AIT_REG_B   DRAM_DDR_DQBLK0_WL_CTL;                                 // 0x6EC5
    AIT_REG_B   DRAM_DDR_DQBLK0_ZQ_CTL;                                 // 0x6EC6
    AIT_REG_B   DRAM_DDR_DQBLK0_ZQ_RSL;                                 // 0x6EC7
    
    AIT_REG_B   DRAM_DDR_DQBLK1_ODT_CTL0;								// 0x6EC8
    AIT_REG_B   DRAM_DDR_DQBLK1_ODT_CTL1;								// 0x6EC9
    AIT_REG_B	DRAM_DDR_DQBLK1_ODT_SR;                                 // 0x6ECA
    AIT_REG_B   DRAM_DDR_DQBLK1_CAL_EN;                                 // 0x6ECB
    AIT_REG_B   DRAM_DDR_DQBLK1_RG_CTL;                                 // 0x6ECC
    AIT_REG_B   DRAM_DDR_DQBLK1_WL_CTL;                                 // 0x6ECD
    AIT_REG_B   DRAM_DDR_DQBLK1_ZQ_CTL;                                 // 0x6ECE
    AIT_REG_B   DRAM_DDR_DQBLK1_ZQ_RSL;                                 // 0x6ECF
    
    AIT_REG_B   DRAM_DDR_DQBLK2_ODT_CTL0;								// 0x6ED0
    AIT_REG_B   DRAM_DDR_DQBLK2_ODT_CTL1;								// 0x6ED1
    AIT_REG_B	DRAM_DDR_DQBLK2_ODT_SR;                                 // 0x6ED2
    AIT_REG_B   DRAM_DDR_DQBLK2_CAL_EN;                                 // 0x6ED3
    	/*-DEFINE-----------------------------------------------------*/
        #define DRAM_RL_CAL_EN     			0x01
		#define DRAM_WL_CAL_EN              0x02
		#define DRAM_ZQ_CAL_EN              0x04
        /*------------------------------------------------------------*/
    AIT_REG_B   DRAM_DDR_DQBLK2_RG_CTL;                                 // 0x6ED4
    AIT_REG_B   DRAM_DDR_DQBLK2_WL_CTL;                                 // 0x6ED5
    AIT_REG_B   DRAM_DDR_DQBLK2_ZQ_CTL;                                 // 0x6ED6
    AIT_REG_B   DRAM_DDR_DQBLK2_ZQ_RSL;                                 // 0x6ED7
    
    AIT_REG_B   DRAM_DDR_DQBLK3_ODT_CTL0;								// 0x6ED8
    AIT_REG_B   DRAM_DDR_DQBLK3_ODT_CTL1;								// 0x6ED9
    AIT_REG_B	DRAM_DDR_DQBLK3_ODT_SR;                                 // 0x6EDA
    AIT_REG_B   DRAM_DDR_DQBLK3_CAL_EN;                                 // 0x6EDB
    AIT_REG_B   DRAM_DDR_DQBLK3_RG_CTL;                                 // 0x6EDC
    AIT_REG_B   DRAM_DDR_DQBLK3_WL_CTL;                                 // 0x6EDD
    AIT_REG_B   DRAM_DDR_DQBLK3_ZQ_CTL;                                 // 0x6EDE
    AIT_REG_B   DRAM_DDR_DQBLK3_ZQ_RSL;                                 // 0x6EDF
    
    AIT_REG_B   						_xE0[0x10];          			// 0x6EE0~6EEF
    AIT_REG_B   DRAM_DDR_RD_CNT_BYTE0;                                  // 0x6EF0
    AIT_REG_B   DRAM_DDR_RD_CNT_BYTE1;                                  // 0x6EF1
    AIT_REG_B   DRAM_DDR_RD_CNT_BYTE2;                                  // 0x6EF2
    AIT_REG_B   DRAM_DDR_RD_CNT_BYTE3;                                  // 0x6EF3
    AIT_REG_B   DRAM_DDR_RD_FIFO_SR;                                    // 0x6EF4
    AIT_REG_B   						_xF5;       					// 0x6EF5
    AIT_REG_B   DRAM_DDR_RD_FIFO_FULL;                                  // 0x6EF6
    AIT_REG_B   						_xF7;       					// 0x6EF7
    AIT_REG_B   DRAM_DDR_DFI_MAX_CYCLE_CTL;                             // 0x6EF8
    AIT_REG_B   DRAM_DDR_DFI_MAX_CYCLE_LOW;                             // 0x6EF9
    AIT_REG_B   DRAM_DDR_DFI_MAX_CYCLE_HIGH;                            // 0x6EFA
    AIT_REG_B   						_xFB[0x3];          			// 0x6EFB~6EFD
    AIT_REG_B   DRAM_DDR_TEST_MODE;										// 0x6EFE
    AIT_REG_B   DRAM_ASIC_PROBE_SEL;									// 0x6EFF
    
} AITS_DRAM, *AITPS_DRAM;

// *********************************
//   DRAM DDR3  Structure (0x8000 3000)
// *********************************
typedef struct _AITS_DRAM_DDR3 {
	AIT_REG_D   DRAM_DDR3_INT_CPU_EN;                                   // 0x3000
    AIT_REG_D   DRAM_DDR3_INT_CPU_SR;									// 0x3004
    AIT_REG_D   DRAM_DDR3_INT_HOST_EN;                                  // 0x3008
    AIT_REG_D   DRAM_DDR3_INT_HOST_SR;                                  // 0x300C
    
    AIT_REG_B   DRAM_DDR3_MR0_LOW_CFG;                                  // 0x3010
    	/*-DEFINE-----------------------------------------------------*/
        #define DRAM_CAS_LATENCY(_a) 	    (_a << 4)
        /*------------------------------------------------------------*/
    AIT_REG_B   DRAM_DDR3_MR0_HIGH_CFG;                                 // 0x3011
    	/*-DEFINE-----------------------------------------------------*/
    	#define DRAM_DDR3_DLL_RST           0x01
        #define DRAM_WR_RECOVERY(_a) 	    (_a << 1)
        /*------------------------------------------------------------*/
    AIT_REG_B   DRAM_DDR3_MR1_LOW_CFG;                                  // 0x3012
    	/*-DEFINE-----------------------------------------------------*/
    	#define DRAM_ADD_LATENCY1           0x08
    	#define DRAM_ADD_LATENCY2           0x10
    	#define DRAM_ADD_LATENCY3           0x18
        /*------------------------------------------------------------*/
    AIT_REG_B   DRAM_DDR3_MR1_HIGH_CFG;                                 // 0x3013
    AIT_REG_B   DRAM_DDR3_MR2_LOW_CFG;                                  // 0x3014
    AIT_REG_B   DRAM_DDR3_MR2_HIGH_CFG;                                 // 0x3015
	AIT_REG_B   DRAM_DDR3_MR3_LOW_CFG;                                  // 0x3016
	AIT_REG_B   						_x17;          			        // 0x3017
	AIT_REG_B   DRAM_DDR3_MR0_LOW_RB;                                   // 0x3018
    AIT_REG_B   DRAM_DDR3_MR0_HIGH_RB;                                  // 0x3019
    AIT_REG_B   DRAM_DDR3_MR1_LOW_RB;                                   // 0x301A
    AIT_REG_B   DRAM_DDR3_MR1_HIGH_RB;                                  // 0x301B
    AIT_REG_B   DRAM_DDR3_MR2_LOW_RB;                                   // 0x301C
    AIT_REG_B   DRAM_DDR3_MR2_HIGH_RB;                                  // 0x301D
	AIT_REG_B   DRAM_DDR3_MR3_LOW_RB;                                   // 0x301E
	AIT_REG_B   						_x1F;          			        // 0x301F
	
	AIT_REG_B   DRAM_DDR3_OPT1;                                          // 0x3020
	AIT_REG_B   DRAM_DDR3_OPT2;                                          // 0x3021
	AIT_REG_B   DRAM_DDR3_SW_DIRECT_CTL;                                 // 0x3022
	AIT_REG_B   DRAM_DDR3_CMD_REQUEST;                                   // 0x3023
		/*-DEFINE-----------------------------------------------------*/
		#define DRAM_WRLVL_EN			    0x01
		#define DRAM_RDLVL_EN			    0x02		     
		/*------------------------------------------------------------*/
	AIT_REG_D   DRAM_DDR3_TIMING_CTL0;                                   // 0x3024
		/*-DEFINE-----------------------------------------------------*/
		// 0x3024
        #define DRAM_RSTW_CYC(_a)			(_a)		      
        // 0x3025
        #define DRAM_DLLK_CYC(_a)			(MMP_ULONG)(_a << 8)	      
        #define DRAM_CKEI_CYC(_a)			(MMP_ULONG)(_a << 12)
        // 0x3026
        #define DRAM_ZQCS_CYC(_a)			(MMP_ULONG)(_a << 16)	      
        #define DRAM_ZQCL_CYC(_a)			(MMP_ULONG)(_a << 18)
        #define DRAM_ZQCI_CYC(_a)			(MMP_ULONG)(_a << 21)
        // 0x3027
        #define DRAM_XP_CYC(_a)				(MMP_ULONG)(_a << 24)	      
        #define DRAM_XPDLL_CYC(_a)			(MMP_ULONG)(_a << 26)    
        #define DRAM_XPR_CYC(_a)			(MMP_ULONG)(_a << 29)    
        /*------------------------------------------------------------*/
	AIT_REG_D   DRAM_DDR3_TIMING_CTL1;                                  // 0x3028
		// 0x3028
        #define DRAM_REF_CYC(_a)			(_a)		      
        // 0x3029
        #define DRAM_MOD_CYC(_a)			(MMP_ULONG)(_a << 8)	      
        #define DRAM_MRD_CYC(_a)			(MMP_ULONG)(_a << 12)
        // 0x302A
        #define DRAM_CKSRX_CYC(_a)			(MMP_ULONG)(_a << 16)	      
        #define DRAM_CKESRE_CYC(_a)			(MMP_ULONG)(_a << 19)
        #define DRAM_PD_CYC(_a)				(MMP_ULONG)(_a << 22)
        // 0x302B
        #define DRAM_ODTLON_CYC(_a)			(MMP_ULONG)(_a << 24)	      
        #define DRAM_ODTLOFF_CYC(_a)		(MMP_ULONG)(_a << 27)    
        #define DRAM_ODTH_CYC(_a)			(MMP_ULONG)(((MMP_ULONG)_a) << 30) 
        /*------------------------------------------------------------*/   
	AIT_REG_B   DRAM_DDR3_TIMING_CTL2;                                  // 0x302C
		// 0x302C
        #define DRAM_RPA_CYC(_a)			(_a)	
        /*------------------------------------------------------------*/
	AIT_REG_B   						_x2D[0x03];          			// 0x302D
	
	AIT_REG_B   DRAM_DDR3_RW_ERR_SR;                                    // 0x3030
	AIT_REG_B   						_x31;          			        // 0x3031
	AIT_REG_B   DRAM_DDR3_RW_OPT;                                       // 0x3032  
		// 0x302C
        #define DRAM_DDR3_AP_EN             0x01
        #define DRAM_DDR3_ACT_NA_EN         0x02
        #define DRAM_DDR3_ACT_PULLIN_EN     0x04
        #define DRAM_DDR3_PRE_INT_EN        0x08
        #define DRAM_DDR3_PRE_MODE_EN       0x10
        /*------------------------------------------------------------*/
	AIT_REG_B   DRAM_DDR3_RW_TIMING_CTL0;                               // 0x3033
		/*-DEFINE-----------------------------------------------------*/
        #define DRAM_RAS_CYC(_a)			(_a)		      
        #define DRAM_RC_CYC(_a)			    (MMP_ULONG)(_a << 4)
        /*------------------------------------------------------------*/
    AIT_REG_D   DRAM_DDR3_RW_TIMING_CTL1;                               // 0x3034    
        // 0x3034
        #define DRAM_ATWA_CYC(_a)			(MMP_ULONG)(_a)	      
        #define DRAM_ATRA_CYC(_a)			(MMP_ULONG)(_a << 4)
        // 0x3035
        #define DRAM_WTP_CYC(_a)			(MMP_ULONG)(_a << 8)	      
        #define DRAM_RTP_CYC(_a)			(MMP_ULONG)(_a << 12)
        // 0x3036
        #define DRAM_WRA_CYC(_a)			(MMP_ULONG)(_a << 16)	      
        #define DRAM_RDA_CYC(_a)			(MMP_ULONG)(_a << 20)  
        // 0x3037
        #define DRAM_AWTP_CYC(_a)			(MMP_ULONG)(_a << 24)      
        #define DRAM_ARTP_CYC(_a)			(MMP_ULONG)(_a << 28)  
        /*------------------------------------------------------------*/
	AIT_REG_D   DRAM_DDR3_RW_TIMING_CTL2;                               // 0x3038
		/*-DEFINE-----------------------------------------------------*/	
        // 0x3038
        #define DRAM_FAW_W_CYC(_a)			(MMP_ULONG)(_a)	      
        #define DRAM_FAW_N_CYC(_a)			(MMP_ULONG)(_a << 4)
        // 0x3039
        #define DRAM_RCD_CYC(_a)			(MMP_ULONG)(_a << 8)	      
        #define DRAM_RRD_CYC(_a)			(MMP_ULONG)(_a << 10)
        // 0x303A
        #define DRAM_RP_CYC(_a)				(MMP_ULONG)(_a << 16)	      
        #define DRAM_RW_RPA_CYC(_a)			(MMP_ULONG)(_a << 19)    
        // 0x303B
		#define DRAM_WR2RDS_CYC(_a)			(MMP_ULONG)(_a << 24)   
        #define DRAM_RD2WRS_CYC(_a)			(MMP_ULONG)(_a << 27)      
        #define DRAM_CCDS_CYC(_a)			(MMP_ULONG)(_a << 30)
        /*------------------------------------------------------------*/  
	AIT_REG_B   DRAM_DDR3_RW_TIMING_CTL3;                               // 0x303C
		/*-DEFINE-----------------------------------------------------*/
        #define DRAM_WR2RDD_CYC(_a)			(MMP_ULONG)(_a)	      
        #define DRAM_RD2WRD_CYC(_a)			(MMP_ULONG)(_a << 3)
        #define DRAM_CCDD_CYC(_a)			(MMP_ULONG)(_a << 6)   
        /*------------------------------------------------------------*/
	AIT_REG_B   						_x3D[0x05];          			// 0x303D
	
	AIT_REG_B   DRAM_DDR3_DQ_SEL;                                       // 0x3042
	AIT_REG_B   						_x43;                           // 0x3043
	AIT_REG_B   DRAM_DDR3_RDL_MASK;                                     // 0x3044
	AIT_REG_B   DRAM_DDR3_RDL_INVERSE_MASK;                             // 0x3045
	AIT_REG_B   DRAM_DDR3_RDL_IDLE_SR;                                  // 0x3046
	AIT_REG_B   DRAM_DDR3_RDL_GATE_TRAIN_ST;                            // 0x3047
	AIT_REG_B   DRAM_DDR3_RDL_DATA_EYE_TRAIN_ST;                        // 0x3048
	AIT_REG_B   DRAM_DDR3_RDL_LOAD_PERIOD;                              // 0x3049
	AIT_REG_B   DRAM_DDR3_RDL_READ_PERIOD;                              // 0x304A
	AIT_REG_B   DRAM_DDR3_RDL_MPRR_XTIME;                               // 0x304B
	AIT_REG_B   DRAM_DDR3_WRL_SETUP_TIME;                               // 0x304C
	AIT_REG_B   DRAM_DDR3_WRL_LOAD_PERIOD;                              // 0x304D
	AIT_REG_B   DRAM_DDR3_WRL_STROBE_PERIOD;                            // 0x304E
	AIT_REG_B   DRAM_DDR3_WRL_RECOVERY_NORMAL_TIME;                     // 0x304F
	
	AIT_REG_B   DRAM_DDR3_AFE_DQS_DLY_L0;                               // 0x3050
	AIT_REG_B   DRAM_DDR3_AFE_DQS_DLY_L1;                               // 0x3051
	AIT_REG_B   						_x52[0x02];                     // 0x3052
	AIT_REG_B   DRAM_DDR3_AFE_RD_DQS_DL;                                // 0x3054
	AIT_REG_B   DRAM_DDR3_AFE_RD_GATE_DH_L0;                            // 0x3055
	AIT_REG_B   DRAM_DDR3_AFE_RD_GATE_DH_L1;                            // 0x3056
	AIT_REG_B   						_x57[0x02];                     // 0x3057
	AIT_REG_B   DRAM_DDR3_AFE_RD_GATE_DL;                               // 0x3059
	AIT_REG_B   DRAM_DDR3_AFE_WR_DQS_DH_L0;                             // 0x305A
	AIT_REG_B   DRAM_DDR3_AFE_WR_DQS_DH_L1;                             // 0x305B
	AIT_REG_B   						_x5C[0x02];                     // 0x305C
	AIT_REG_B   DRAM_DDR3_AFE_WR_DQS_DL;                                // 0x305E
	AIT_REG_B   DRAM_DDR3_AFE_RW_DLY_UPDATE_EN;                         // 0x305F
	
	AIT_REG_B   DRAM_DDR3_RDL_OPT;                                      // 0x3060
		/*-DEFINE-----------------------------------------------------*/
		#define DRAM_DATA_EYE_EN      	   0x02
		#define DRAM_GATE_TRAINING_EN      0x04
		/*------------------------------------------------------------*/
	AIT_REG_B   DRAM_DDR3_RDL_PT;                                       // 0x3061
		/*-DEFINE-----------------------------------------------------*/
		#define DRAM_DE_SCAN_MASK          0x0F
		#define DRAM_DE_SCAN_NUM(_a)       (_a)
		#define DRAM_GT_SCAN_MASK          0xF0
		#define DRAM_GT_SCAN_NUM(_a)       (_a << 4)
		/*------------------------------------------------------------*/
	AIT_REG_B   DRAM_DDR3_WRL_PT;                                       // 0x3062
		/*-DEFINE-----------------------------------------------------*/
		#define DRAM_SCAN_NUM_MASK          0x0F
		#define DRAM_SCAN_NUM(_a)           (_a & 0x0F)
		/*------------------------------------------------------------*/
	AIT_REG_B   						_x63;                           // 0x3063
	AIT_REG_B   DRAM_DDR3_RDL_GATE_INIT_L0;                             // 0x3064
	AIT_REG_B   DRAM_DDR3_RDL_GATE_INIT_L1;                             // 0x3065
	AIT_REG_B   DRAM_DDR3_RDL_GATE_INIT_HIGH;                           // 0x3066
		/*-DEFINE-----------------------------------------------------*/
		// ++MCR_V2_MP Only
		#define DRAM_RDL_GATE_INIT_L0_H(_a) (_a)
        #define DRAM_RDL_GATE_INIT_L0_MASK  0x03
        #define DRAM_RDL_GATE_INIT_L1_H(_a) (_a << 2)  
        #define DRAM_RDL_GATE_INIT_L1_MASK  0x0C  
        // --MCR_V2_MP Only         
        /*------------------------------------------------------------*/
	AIT_REG_B   						_x67[0x01];                     // 0x3067
	AIT_REG_B   DRAM_DDR3_RDL_GATE_STEP_L0;                             // 0x3068
	AIT_REG_B   DRAM_DDR3_RDL_GATE_STEP_L1;                             // 0x3069
	AIT_REG_B   						_x6A[0x02];                     // 0x306A
	AIT_REG_B   DRAM_DDR3_RDL_DETS1_INIT_L0;                            // 0x306C
	AIT_REG_B   DRAM_DDR3_RDL_DETS1_INIT_L1;                            // 0x306D
	AIT_REG_B   DRAM_DDR3_RDL_DETS1_INIT_HIGH;                          // 0x306E
		/*-DEFINE-----------------------------------------------------*/
		// ++MCR_V2_MP Only
        #define DRAM_RDL_DETS1_INIT_L0_H(_a) (_a)  
        #define DRAM_RDL_DETS1_INIT_L0_MASK  0x03
        #define DRAM_RDL_DETS1_INIT_L1_H(_a) (_a << 2)    
        #define DRAM_RDL_DETS1_INIT_L1_MASK  0x0C  
        // --MCR_V2_MP Only         
        /*------------------------------------------------------------*/
	AIT_REG_B   						_x6F[0x01];                     // 0x306F
	
	AIT_REG_B   DRAM_DDR3_RDL_DETS1_STEP_L0;                            // 0x3070
	AIT_REG_B   DRAM_DDR3_RDL_DETS1_STEP_L1;                            // 0x3071
	AIT_REG_B   						_x72[0x02];                     // 0x3072
	AIT_REG_B   DRAM_DDR3_RDL_DETS2_INIT_L0;                            // 0x3074
	AIT_REG_B   DRAM_DDR3_RDL_DETS2_INIT_L1;                            // 0x3075
	AIT_REG_B   DRAM_DDR3_RDL_DETS2_INIT_HIGH;                          // 0x3076
		/*-DEFINE-----------------------------------------------------*/
		// ++MCR_V2_MP Only
        #define DRAM_RDL_DETS2_INIT_L0_H(_a) (_a)  
        #define DRAM_RDL_DETS2_INIT_L0_MASK  0x03
        #define DRAM_RDL_DETS2_INIT_L1_H(_a) (_a << 2)    
        #define DRAM_RDL_DETS2_INIT_L1_MASK  0x0C  
        // --MCR_V2_MP Only         
        /*------------------------------------------------------------*/
	AIT_REG_B   						_x77[0x01];                     // 0x3077
	AIT_REG_B   DRAM_DDR3_RDL_DETS2_STEP_L0;                            // 0x3078
	AIT_REG_B   DRAM_DDR3_RDL_DETS2_STEP_L1;                            // 0x3079
	AIT_REG_B   						_x7A[0x02];                     // 0x307A
	AIT_REG_B   DRAM_DDR3_WRL_DLY_INIT_L0;                              // 0x307C
	AIT_REG_B   DRAM_DDR3_WRL_DLY_INIT_L1;                              // 0x307D
	AIT_REG_B   DRAM_DDR3_WRL_DLY_INIT_HIGH;                            // 0x307E
		/*-DEFINE-----------------------------------------------------*/
		// ++MCR_V2_MP Only
        #define DRAM_WRL_DLY_INIT_L0_H(_a) (_a)  
        #define DRAM_WRL_DLY_INIT_L0_MASK  0x03
        #define DRAM_WRL_DLY_INIT_L1_H(_a) (_a << 2)    
        #define DRAM_WRL_DLY_INIT_L1_MASK  0x0C  
        // --MCR_V2_MP Only         
        /*------------------------------------------------------------*/
	AIT_REG_B   						_x7F[0x01];                     // 0x307F
	
	AIT_REG_B   DRAM_DDR3_WRL_DLY_STEP_L0;                              // 0x3080
	AIT_REG_B   DRAM_DDR3_WRL_DLY_STEP_L1;                              // 0x3081
	AIT_REG_B   						_x82[0x02];                     // 0x3082~0x3083
	AIT_REG_B   DRAM_DDR3_AFE_DLY_CTL;                              	// 0x3084
		/*-DEFINE-----------------------------------------------------*/
		#define DRAM_DE_DONE                0x00
		#define DRAM_GT_DONE             	0x02
		#define DRAM_WRLVL_DONE             0x03
		#define DRAM_RESULT_MASK            0x07
		/*------------------------------------------------------------*/
	AIT_REG_B   						_x85[0x03];                     // 0x3085~0x3087
	AIT_REG_D   DRAM_RDL_WRL_RES_SUM0;                              	// 0x3088
	AIT_REG_D   DRAM_RDL_WRL_RES_SUM1;                              	// 0x308C
	AIT_REG_D   DRAM_RDL_DATA_EYE_RD;	                              	// 0x3090
	AIT_REG_B   						_x94[0x0C];                     // 0x3094~0x309F
	AIT_REG_B   DRAM_CALIB_INT_CPU_EN;                                  // 0x30A0
	AIT_REG_B   DRAM_CALIB_INT_CPU_SR;                                  // 0x30A1
	AIT_REG_B   DRAM_CALIB_INT_HOST_EN;                                 // 0x30A2
	AIT_REG_B   DRAM_CALIB_INT_HOST_SR;                                 // 0x30A3
		/*-DEFINE-----------------------------------------------------*/
		#define DRAM_DLL_SHIFT_OVER_THRES   0x01
		#define DRAM_DYNA_CALIB_STEP1       0x02
		#define DRAM_DYNA_CALIB_STEP2       0x04
		#define DRAM_DYNA_CALIB_FAIL        0x08  
		/*------------------------------------------------------------*/
	AIT_REG_B   DRAM_CALIB_SW_TRI;                            			// 0x30A4
	AIT_REG_B   DRAM_CALIB_CTL;	                            			// 0x30A5
	AIT_REG_B   DRAM_DLL_LATCH_SW_TRI;	                            	// 0x30A6
	AIT_REG_B   DRAM_DLL_DETECT_TH;	                            		// 0x30A7
	AIT_REG_B   DRAM_CALIB_DONE_FLAG;	                            	// 0x30A8
	AIT_REG_B   						_xA9[0x07];                     // 0x30A9~30AF
	AIT_REG_B   DRAM_MAN_RDGATE_INIT_LAN0_S1;	                        // 0x30B0
	AIT_REG_B   DRAM_MAN_RDGATE_INIT_LAN1_S1;	                        // 0x30B1
	AIT_REG_B   DRAM_MAN_RDGATE_STEP_LAN0_S1;	                        // 0x30B2
	AIT_REG_B   DRAM_MAN_RDGATE_STEP_LAN1_S1;	                        // 0x30B3
	AIT_REG_B   DRAM_MAN_RDEYE_INIT_LAN0_S1;	                        // 0x30B4
	AIT_REG_B   DRAM_MAN_RDEYE_INIT_LAN1_S1;	                        // 0x30B5
	AIT_REG_B   DRAM_MAN_RDEYE_STEP_LAN0_S1;	                        // 0x30B6
	AIT_REG_B   DRAM_MAN_RDEYE_STEP_LAN1_S1;	                        // 0x30B7
	AIT_REG_B   DRAM_MAN_WRLVL_INIT_LAN0_S1;	                        // 0x30B8
	AIT_REG_B   DRAM_MAN_WRLVL_INIT_LAN1_S1;	                        // 0x30B9
	AIT_REG_B   DRAM_MAN_WRLVL_STEP_LAN0_S1;	                        // 0x30BA
	AIT_REG_B   DRAM_MAN_WRLVL_STEP_LAN1_S1;	                        // 0x30BB
	AIT_REG_B   DRAM_MAN_RDGATE_INIT_LAN0_S2;	                        // 0x30BC
	AIT_REG_B   DRAM_MAN_RDGATE_INIT_LAN1_S2;	                        // 0x30BD
	AIT_REG_B   DRAM_MAN_RDGATE_STEP_LAN0_S2;	                        // 0x30BE
	AIT_REG_B   DRAM_MAN_RDGATE_STEP_LAN1_S2;	                        // 0x30BF
	AIT_REG_B   DRAM_MAN_RDEYE_INIT_LAN0_S2;	                        // 0x30C0
	AIT_REG_B   DRAM_MAN_RDEYE_INIT_LAN1_S2;	                        // 0x30C1
	AIT_REG_B   DRAM_MAN_RDEYE_STEP_LAN0_S2;	                        // 0x30C2
	AIT_REG_B   DRAM_MAN_RDEYE_STEP_LAN1_S2;	                        // 0x30C3
	AIT_REG_B   DRAM_MAN_WRLVL_INIT_LAN0_S2;	                        // 0x30C4
	AIT_REG_B   DRAM_MAN_WRLVL_INIT_LAN1_S2;	                        // 0x30C5
	AIT_REG_B   DRAM_MAN_WRLVL_STEP_LAN0_S2;	                        // 0x30C6
	AIT_REG_B   DRAM_MAN_WRLVL_STEP_LAN1_S2;	                        // 0x30C7
	AIT_REG_B   						_xC8[0x08];                     // 0x30C8~30CF
	AIT_REG_B   DRAM_CALIB_RESULT_S1;	                        		// 0x30D0
	AIT_REG_B   						_xD1[0x01];                     // 0x30D1
	AIT_REG_B   DRAM_CALIB_RDGATE_RST_LAN0_S2;	                        // 0x30D2
	AIT_REG_B   DRAM_CALIB_RDGATE_RST_LAN1_S2;	                        // 0x30D3
	AIT_REG_B   DRAM_CALIB_RDEYE_RST_LAN0_S2;	                        // 0x30D4
	AIT_REG_B   DRAM_CALIB_RDEYE_RST_LAN1_S2;	                        // 0x30D5
	AIT_REG_B   DRAM_CALIB_WRLVL_RST_LAN0_S2;	                        // 0x30D6
	AIT_REG_B   DRAM_CALIB_WRLVL_RST_LAN1_S2;	                        // 0x30D7
	AIT_REG_B   DRAM_CALIB_RDGATE_DLY_LAN0_L;	                        // 0x30D8
	AIT_REG_B   DRAM_CALIB_RDGATE_DLY_LAN0_H;	                        // 0x30D9
	AIT_REG_B   DRAM_CALIB_RDGATE_DLY_LAN1_L;	                        // 0x30DA
	AIT_REG_B   DRAM_CALIB_RDGATE_DLY_LAN1_H;	                        // 0x30DB
	AIT_REG_B   DRAM_CALIB_RDEYE_DLY_LAN0_L;	                        // 0x30DC
	AIT_REG_B   DRAM_CALIB_RDEYE_DLY_LAN0_H;	                        // 0x30DD
	AIT_REG_B   DRAM_CALIB_RDEYE_DLY_LAN1_L;	                        // 0x30DE
	AIT_REG_B   DRAM_CALIB_RDEYE_DLY_LAN1_H;	                        // 0x30DF
	AIT_REG_B   DRAM_CALIB_WRLVL_DLY_LAN0_L;	                        // 0x30E0
	AIT_REG_B   DRAM_CALIB_WRLVL_DLY_LAN0_H;	                        // 0x30E1
	AIT_REG_B   DRAM_CALIB_WRLVL_DLY_LAN1_L;	                        // 0x30E2
	AIT_REG_B   DRAM_CALIB_WRLVL_DLY_LAN1_H;	                        // 0x30E3
	AIT_REG_B   						_xE4[0x0C];                     // 0x30E4~30EF
	// ++MCR_V2_MP Only
	AIT_REG_B   DRAM_RD_WRLVL_DBG_EN;	                        		// 0x30F0
	AIT_REG_B   DRAM_DQBLK_WRLVL_OPR_EN;	                        	// 0x30F1	
	AIT_REG_B   DRAM_DQBLK_RDLVL_OPR_EN;	                        	// 0x30F2
	AIT_REG_B   DRAM_DQBLK_WRLVL_OPR_STB;	                        	// 0x30F3
	AIT_REG_B   DRAM_DQ0_WRLVL_DLY_OPR;                        			// 0x30F4
	AIT_REG_B   DRAM_DQ1_WRLVL_DLY_OPR;                        			// 0x30F5
	AIT_REG_B   DRAM_DQ2_WRLVL_DLY_OPR;                        			// 0x30F6
	AIT_REG_B   DRAM_DQ3_WRLVL_DLY_OPR;                        			// 0x30F7	
	AIT_REG_B   DRAM_DQ30_WRLVL_DLY_EXT_OPR;                        	// 0x30F8	
	AIT_REG_B   						_xF9[0x01];                     // 0x30F9
	AIT_REG_B   DRAM_DQ0_GT_DLY_OPR;                        			// 0x30FA
	AIT_REG_B   DRAM_DQ1_GT_DLY_OPR;                        			// 0x30FB
	AIT_REG_B   DRAM_DQ2_GT_DLY_OPR;                        			// 0x30FC
	AIT_REG_B   DRAM_DQ3_GT_DLY_OPR;                        			// 0x30FD	
	AIT_REG_B   DRAM_DQ30_GT_DLY_EXT_OPR;                        		// 0x30FE	
	AIT_REG_B   DRAM_DQ30_WRLVL_RESP_OPR;                        		// 0x30FF
	// --MCR_V2_MP Only	

} AITS_DRAM_DDR3, *AITPS_DRAM_DDR3;
#endif

#if !defined(BUILD_FW)
#define DRAM_INT_CPU_SR                 (DRAM_BASE  +(MMP_ULONG)(&(((AITPS_DRAM)0)->DRAM_INT_CPU_SR   )))
#define DRAM_CTL_0				        (DRAM_BASE  +(MMP_ULONG)(&(((AITPS_DRAM)0)->DRAM_CTL_0        )))
#define DRAM_CTL_1				        (DRAM_BASE  +(MMP_ULONG)(&(((AITPS_DRAM)0)->DRAM_CTL_1        )))
#define DRAM_CTL_2				        (DRAM_BASE  +(MMP_ULONG)(&(((AITPS_DRAM)0)->DRAM_CTL_2        )))
#define DRAM_CTL_4				        (DRAM_BASE  +(MMP_ULONG)(&(((AITPS_DRAM)0)->DRAM_CTL_4        )))
#define DRAM_FUNC				        (DRAM_BASE  +(MMP_ULONG)(&(((AITPS_DRAM)0)->DRAM_FUNC         )))
#define DRAM_ARF_CYC  		        	(DRAM_BASE  +(MMP_ULONG)(&(((AITPS_DRAM)0)->DRAM_ARF_CYC      )))
#define DRAM_ARF_QUEUE_DEPTH        	(DRAM_BASE  +(MMP_ULONG)(&(((AITPS_DRAM)0)->DRAM_ARF_QUEUE_DEPTH)))
#define DRAM_CTL_CYC_0		        	(DRAM_BASE  +(MMP_ULONG)(&(((AITPS_DRAM)0)->DRAM_CTL_CYC_0	  )))
#define DRAM_CTL_CYC_1		        	(DRAM_BASE  +(MMP_ULONG)(&(((AITPS_DRAM)0)->DRAM_CTL_CYC_1	  )))
#define DRAM_MODE_REG               	(DRAM_BASE  +(MMP_ULONG)(&(((AITPS_DRAM)0)->DRAM_MODE_REG     )))
#define DRAM_EXT_MODE_REG             	(DRAM_BASE  +(MMP_ULONG)(&(((AITPS_DRAM)0)->DRAM_EXT_MODE_REG )))
#define DRAM_RDDATA_CLK_DLY         	(DRAM_BASE  +(MMP_ULONG)(&(((AITPS_DRAM)0)->DRAM_RDDATA_CLK_DLY)))
#define DRAM_CLK_OUT_DLY	        	(DRAM_BASE  +(MMP_ULONG)(&(((AITPS_DRAM)0)->DRAM_CLK_OUT_DLY  )))
#define DRAM_DPD_ARF_CYC            	(DRAM_BASE  +(MMP_ULONG)(&(((AITPS_DRAM)0)->DRAM_DPD_ARF_CYC  )))
#define DRAM_INT_HOST_SR		        (DRAM_BASE  +(MMP_ULONG)(&(((AITPS_DRAM)0)->DRAM_INT_HOST_SR )))
#define DRAM_CMD_CTL		        	(DRAM_BASE  +(MMP_ULONG)(&(((AITPS_DRAM)0)->DRAM_CMD_CTL0      )))
#define DRAM_SIGNAL_CTL                 (DRAM_BASE  +(MMP_ULONG)(&(((AITPS_DRAM)0)->DRAM_SIGNAL_CTL             ))) 
#define DRAM_DDR_PHY_DLL1_CTL2          (DRAM_BASE  +(MMP_ULONG)(&(((AITPS_DRAM)0)->DRAM_DDR_PHY_DLL1_CTL2      )))
#define DRAM_DDR_PHY_CLK_OPT            (DRAM_BASE  +(MMP_ULONG)(&(((AITPS_DRAM)0)->DRAM_DDR_PHY_CLK_OPT        ))) 
#define DRAM_DDR_PHY_DLL1_CTL           (DRAM_BASE  +(MMP_ULONG)(&(((AITPS_DRAM)0)->DRAM_DDR_PHY_DLL1_CTL       )))
#define DRAM_DDR_DLL_CTL                (DRAM_BASE  +(MMP_ULONG)(&(((AITPS_DRAM)0)->DRAM_DDR_DLL_CTL            ))) 
#define DRAM_DDR_OPR_CTL                (DRAM_BASE  +(MMP_ULONG)(&(((AITPS_DRAM)0)->DRAM_DDR_OPR_CTL            )))
#define DRAM_DDR_MACRO_DELAY            (DRAM_BASE  +(MMP_ULONG)(&(((AITPS_DRAM)0)->DRAM_DDR_MACRO_DELAY        )))
#define DRAM_CTL_CYC_EXT                (DRAM_BASE  +(MMP_ULONG)(&(((AITPS_DRAM)0)->DRAM_CTL_CYC_EXT            )))
#endif

/// @}
#endif // _MMP_REG_DRAM_H_

#ifndef _MMP_REG_COLOR_H_
#define _MMP_REG_COLOR_H_

#if (CHIP == MCR_V2)
//-----------------------------
// HDR Structure (0x8000 24D0)
//-----------------------------
typedef struct _AITS_HDR {
    AIT_REG_B   HDR_CTL;                        						// 0x00
        /*-DEFINE-----------------------------------------------------*/
        #define HDR_FS_CHANGE_FRAME_IDX     0x10
        #define HDR_ACC_SELECT		        0x08
        #define HDR_INTERLACE_MODE_EN       0x04
        #define HDR_FRAME_IDX_EN       		0x02
        #define HDR_ENABLE        			0x01 
        /*------------------------------------------------------------*/
    AIT_REG_B   HDR_GAP_PARAM;											// 0x01
        /*-DEFINE-----------------------------------------------------*/
        #define HDR_GAP_PARAM_MASK			0x0F
        /*------------------------------------------------------------*/
    AIT_REG_W	HDR_THD_PARAM;											// 0x02
        /*-DEFINE-----------------------------------------------------*/
        #define HDR_THD_PARAM_MASK			0x03FF
        /*------------------------------------------------------------*/
    AIT_REG_W  	HDR_RATIO_PARAM;										// 0x04
    AIT_REG_W	HDR_MAX_X_THD_PARAM;									// 0x06
    AIT_REG_W	HDR_MAX_Y_THD_PARAM;									// 0x08
    AIT_REG_W	HDR_MERGE_WIDTH;										// 0x0A
        /*-DEFINE-----------------------------------------------------*/
        #define HDR_MERGE_WIDTH_MASK		0x1FFF
        /*------------------------------------------------------------*/
	AIT_REG_B                           _xDC[0x4];
} AITS_HDR, *AITPS_HDR;

//-----------------------------
// ISP Structure (0x8000 7000)
//-----------------------------
typedef struct _AITS_ISP {
    AIT_REG_B   ISP_FROM_VIF_COLOR_ID;                        			// 0x00
        /*-DEFINE-----------------------------------------------------*/
        #define ISP_VIF_COLORID_MASK        0x30
        #define ISP_VIF_COLORID_11          0x30
        #define ISP_VIF_COLORID_10          0x20
        #define ISP_VIF_COLORID_01          0x10
        #define ISP_VIF_COLORID_00          0x00 
        /*------------------------------------------------------------*/
    AIT_REG_B                          _x01[0x6];
    AIT_REG_B	BAYER_DNSAMP_CTL;										// 0x07
        /*-DEFINE-----------------------------------------------------*/
        #define BAYER_DNSAMP_RATIO2_H		0x01
        #define BAYER_DNSAMP_RATIO2_V   	0x02
        /*------------------------------------------------------------*/
    AIT_REG_B                          _x08[0x2];
    AIT_REG_B	ISP_MISC_CTL;											// 0x0A
	    /*-DEFINE-----------------------------------------------------*/
	    #define	ISP_RAW_MODE_EN			    0x80

        #define ISP_RAWF_COLORID_11         0x60
        #define ISP_RAWF_COLORID_10         0x40
        #define ISP_RAWF_COLORID_01         0x20
        #define ISP_RAWF_COLORID_00         0x00
	    #define	ISP_RAWF_COLORID_FMT_MASK	0x60
        #define ISP_RAWF_COLORID(id)		( (id << 5) & ISP_RAWF_COLORID_FMT_MASK)
	    
	    #define ISP_RAWF_DST_DEF_INPUT   	0x00
	    #define ISP_RAWF_DST_BEFORE_HDR		0x01
	    #define ISP_RAWF_DST_BEFORE_BAYER	0x02
	    #define ISP_RAWF_DST_AFTER_BAYER	0x03
	    #define	ISP_RAWF_DST_MASK		    0x07
        /*------------------------------------------------------------*/
    AIT_REG_B							_x0B[0x5];					    // 0x0B
    
    AIT_REG_B                           _x10[0x60];
    
    AIT_REG_B                           _x70[0x5];
    AIT_REG_B   ISP_EXTRA_LN_SUB_CNT_BAYER;                             // 0x75
        /*-DEFINE-----------------------------------------------------*/
        #define ISP_EXTRA_LN_SUB_CNT_MASK   		0x3F
        /*------------------------------------------------------------*/
    AIT_REG_W   ISP_EXTRA_LN_GAP_BAYER;                                 // 0x76
    AIT_REG_B   ISP_COLOR_SYS_CTL;                                      // 0x78
        /*-DEFINE-----------------------------------------------------*/
        #define ISP_EXTRA_LN_GAP_CTL_BY_SCAL_BUSY   0x01
        #define ISP_PIPELINE_FRM_ST_EN              0x02
        #define ISP_RAW_FETCH_FRM_SYNC_EN           0x04
        #define ISP_BYPASS_YUV_PATH_EN				0x08
        /*------------------------------------------------------------*/
    AIT_REG_B   ISP_EXTRA_LN_SUB_CNT;                                   // 0x79
    AIT_REG_W   ISP_EXTRA_LN_GAP;                                       // 0x7A
    AIT_REG_B                           _x7C[0x4];
    
    AIT_REG_B                           _x80[0x40];
    
    AIT_REG_D   ISP_INT_HOST_EN;                                        // 0xC0
    AIT_REG_D   ISP_INT_HOST_SR;                                        // 0xC4
    AIT_REG_D   ISP_INT_CPU_EN;                                         // 0xC8
    AIT_REG_D   ISP_INT_CPU_SR;                                         // 0xCC
        /*-DEFINE-----------------------------------------------------*/
        #define ISP_INT_VIF_FRM_ST                  0x00000001
        #define ISP_INT_BAYERSCA_FRM_ST             0x00000002
        #define ISP_INT_CI_FRM_ST                   0x00000004
        #define ISP_INT_COLOR2SCA_FRM_ST            0x00000008
        #define ISP_INT_RAWF_FRM_ST                 0x00000010
        #define ISP_INT_YUV_PATH_FRM_ST				0x00000020
        
        #define ISP_INT_BAYERSCA_FRM_END            0x00000100
        #define ISP_INT_CI_FRM_END                  0x00000200
        #define ISP_INT_COLOR_FRM_END               0x00000400
        #define ISP_INT_HDR_FRM_END               	0x00000800
        
        #define ISP_INT_AE_WINDOW_ACC_DONE          0x00010000
        #define ISP_INT_AWB_256WINDOW_ACC_DONE      0x00020000
        #define ISP_INT_AWB_OLD_WINDOW_ACC_DONE     0x00040000
        #define ISP_INT_AF_WINDOW_ACC_DONE          0x00080000
        #define ISP_INT_FLICKER_WINDOW_ACC_DONE     0x00100000
        #define ISP_INT_ROI_BAYER_WINDOW_ACC_DONE   0x00200000
        #define ISP_INT_ROI_Y_WINDOW_ACC_DONE       0x00400000
        #define ISP_INT_RGB_HIST_ACC_DONE           0x00800000
        
        #define ISP_INT_BAYERSCA_BUF_OVF            0x01000000
        /*------------------------------------------------------------*/
} AITS_ISP, *AITPS_ISP;

//-------------------------------------
// Bayer Scaler structure (0x8000 7100)
//-------------------------------------
typedef struct _AITS_BAYER_SCAL {
    AIT_REG_B                           _x0[0xC];
    AIT_REG_W   BAYER_SCAL_BUF_USAGE_CNT;                  	// 0x0C
    AIT_REG_B   BAYER_SCAL_LINE_SUB_CNT;                  	// 0x0E
    AIT_REG_B   BAYER_SCAL_CTL;			                  	// 0x0F    
        /*-DEFINE-----------------------------------------------------*/
        #define BAYER_SCAL_BYPASS                   0x01
        #define BAYER_SCAL_CLR_BUF_USAGE            0x80
        /*------------------------------------------------------------*/
    AIT_REG_W   BAYER_SCAL_GRAB_IN_H_ST;			        // 0x10
    AIT_REG_W   BAYER_SCAL_GRAB_IN_H_ED;			        // 0x12
    AIT_REG_W   BAYER_SCAL_GRAB_IN_V_ST;			        // 0x14
    AIT_REG_W   BAYER_SCAL_GRAB_IN_V_ED;			        // 0x16
    AIT_REG_W   BAYER_SCAL_GRAB_OUT_H_ST;			        // 0x18
    AIT_REG_W   BAYER_SCAL_GRAB_OUT_H_ED;			        // 0x1A
    AIT_REG_W   BAYER_SCAL_GRAB_OUT_V_ST;			        // 0x1C
    AIT_REG_W   BAYER_SCAL_GRAB_OUT_V_ED;			        // 0x1E 

    AIT_REG_W   BAYER_SCAL_H_M;						        // 0x20
    AIT_REG_W   BAYER_SCAL_H_N;						        // 0x22
    AIT_REG_W   BAYER_SCAL_V_M;						        // 0x24
    AIT_REG_W   BAYER_SCAL_V_N;						        // 0x26
    	/*-DEFINE-----------------------------------------------------*/
        #define BAYER_SCAL_NM_MAX                 	0x03FF
        /*------------------------------------------------------------*/
    AIT_REG_W   BAYER_SCAL_H_OFST;						    // 0x28 
    AIT_REG_W   BAYER_SCAL_V_OFST;						    // 0x2A
    AIT_REG_W   BAYER_SCAL_SYNC_BUF_LINEGAP;				// 0x2C
    AIT_REG_B                           _x2E[0x2];
         
} AITS_BAYER_SCAL, *AITPS_BAYER_SCAL;
#endif

#endif

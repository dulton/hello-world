//==============================================================================
//
//  File        : sensor_imx214.h
//  Description : Firmware Sensor Control File
//  Author      : Eric zhang
//  Revision    : 1.0
//
//=============================================================================

#ifndef	_SENSOR_IMX214_H_
#define	_SENSOR_IMX214_H_

//==============================================================================
//
//                              MACRO DEFINE
//
//==============================================================================

#define SENSOR_IF 					(SENSOR_IF_MIPI_4_LANE)

#define RES_IDX_4208x3120_25P_HDR 	(0) 	// mode 0, 4208 * 3120 25P, Video  (4:3), HDR mode)
#define RES_IDX_4208x3120_25P    	(1) 	// mode 1, 4208 * 3120 25P, Video  (4:3),
#define RES_IDX_3840x2160_30P_HDR 	(2) 	// mode 2, 3840 * 2160 30P, Video  (16:9), HDR mode)
#define RES_IDX_3840x2160_30P    	(3) 	// mode 3, 3840 * 2160 30P, Video  (16:9),
#define RES_IDX_1920x1080_30P_HDR 	(4) 	// mode 4, 1920 * 1080 30P, Video  (16:9), HDR mode
#define RES_IDX_1920x1080_50P 	    (5) 	// mode 5, 1920 * 1080 50P, Video  (16:9), 
#define RES_IDX_1920x1080_30P 	    (6) 	// mode 6, 1920 * 1080 30P, Video  (16:9), 
#define RES_IDX_1440x1080_30P 	    (7) 	// mode 7, 1440 * 1080 30P, Video  (4:3), cropped from 1920x1080 30P
#define RES_IDX_1280x720_100P 	    (8) 	// mode 8, 1280 * 720 100P, Video  (16:9), 
#define RES_IDX_1280x720_60P 	    (9) 	// mode 9, 1280 * 720 60P, Video  (16:9)
#define RES_IDX_1280x720_30P 	    (10) 	// mode 10, 1280 * 720 30P, Video  (16:9) 
//jeff 2015-5-8 add for capture
#define RES_IDX_4208x3120_10P    	(11) 	// mode 11, 4208 * 3120 10P, Video  (4:3),  jeff 2015-5-8 add for capture
#define RES_IDX_2016x1512_30P    	(12) 	// mode 12, 2016 * 1512 30P, Video  (16:9),
#define RES_IDX_1920x1080_60P    	(13) 	// mode 13, 1920 * 1080 60P, Video  (16:9),

#define MAX_SENSOR_GAIN    	 		(8)

//==============================================================================
//
//                              MACRO DEFINE (Resolution For UI)
//
//==============================================================================

#ifndef SENSOR_DRIVER_MODE_NOT_SUUPORT
#define SENSOR_DRIVER_MODE_NOT_SUUPORT  (0xFFFF)
#endif

// Index 0
#define SENSOR_DRIVER_MODE_VGA_30P_RESOLUTION           (RES_IDX_3840x2160_30P)         	        // 640*360  30P
#define SENSOR_DRIVER_MODE_VGA_50P_RESOLUTION           (SENSOR_DRIVER_MODE_NOT_SUUPORT)            // 640*360  50P
#define SENSOR_DRIVER_MODE_VGA_60P_RESOLUTION           (SENSOR_DRIVER_MODE_NOT_SUUPORT)            // 640*360  60P
#define SENSOR_DRIVER_MODE_VGA_100P_RESOLUTION          (SENSOR_DRIVER_MODE_NOT_SUUPORT)            // 640*360  100P
#define SENSOR_DRIVER_MODE_VGA_120P_RESOLUTION          (SENSOR_DRIVER_MODE_NOT_SUUPORT)            // 640*360  120P
    
// Index 5
#define SENSOR_DRIVER_MODE_HD_24P_RESOLUTION        	(SENSOR_DRIVER_MODE_NOT_SUUPORT)            // 1280*720  24P
#define SENSOR_DRIVER_MODE_HD_30P_RESOLUTION            (RES_IDX_1280x720_30P)                      // 1280*720  30P
#define SENSOR_DRIVER_MODE_HD_50P_RESOLUTION            (SENSOR_DRIVER_MODE_NOT_SUUPORT)            // 1280*720  50P
#define SENSOR_DRIVER_MODE_HD_60P_RESOLUTION            (RES_IDX_1280x720_60P)                      // 1280*720  60P
#define SENSOR_DRIVER_MODE_HD_100P_RESOLUTION           (RES_IDX_1280x720_100P)                     // 1280*720  100P

// Index 10
#define SENSOR_DRIVER_MODE_HD_120P_RESOLUTION           (SENSOR_DRIVER_MODE_NOT_SUUPORT)            // 1280*720  120P
#define SENSOR_DRIVER_MODE_FULL_HD_15P_RESOLUTION       (SENSOR_DRIVER_MODE_NOT_SUUPORT)            // 1920*1080 15P
#define SENSOR_DRIVER_MODE_FULL_HD_24P_RESOLUTION       (SENSOR_DRIVER_MODE_NOT_SUUPORT)    		// 1920*1080 24P
#define SENSOR_DRIVER_MODE_FULL_HD_25P_RESOLUTION       (SENSOR_DRIVER_MODE_NOT_SUUPORT)            // 1920*1080 25P
#define SENSOR_DRIVER_MODE_FULL_HD_30P_RESOLUTION       (RES_IDX_1920x1080_30P)         			// 1920*1080 30P

// Index 15
#define SENSOR_DRIVER_MODE_FULL_HD_50P_RESOLUTION       (RES_IDX_1920x1080_50P)                     // 1920*1080 50P
#define SENSOR_DRIVER_MODE_FULL_HD_60P_RESOLUTION       (RES_IDX_1920x1080_60P)            			// 1920*1080 60P
#define SENSOR_DRIVER_MODE_SUPER_HD_30P_RESOLUTION 		(SENSOR_DRIVER_MODE_NOT_SUUPORT)    	    // 2304*1296 30P
#define SENSOR_DRIVER_MODE_SUPER_HD_25P_RESOLUTION      (SENSOR_DRIVER_MODE_NOT_SUUPORT)    // 2304*1296 25P
#define SENSOR_DRIVER_MODE_SUPER_HD_24P_RESOLUTION      (SENSOR_DRIVER_MODE_NOT_SUUPORT)    // 2304*1296 24P

// Index 20
#define SENSOR_DRIVER_MODE_1440_30P_RESOLUTION			(SENSOR_DRIVER_MODE_NOT_SUUPORT)			// 2560*1440 30P
#define SENSOR_DRIVER_MODE_2D7K_15P_RESOLUTION          (SENSOR_DRIVER_MODE_NOT_SUUPORT)            // 2704*1524 15P
#define SENSOR_DRIVER_MODE_2D7K_30P_RESOLUTION          (SENSOR_DRIVER_MODE_NOT_SUUPORT)            // 2704*1524 30P
#define SENSOR_DRIVER_MODE_4K2K_15P_RESOLUTION          (SENSOR_DRIVER_MODE_NOT_SUUPORT)            // 3840*2160 15P
#define SENSOR_DRIVER_MODE_4K2K_30P_RESOLUTION          (RES_IDX_3840x2160_30P)                     // 3840*2160 30P

// Index 25
#define SENSOR_DRIVER_MODE_4TO3_VGA_30P_RESOLUTION      (RES_IDX_3840x2160_30P)           			// 640*480   30P 
#define SENSOR_DRIVER_MODE_4TO3_1D2M_30P_RESOLUTION     (RES_IDX_3840x2160_30P)            			// 1280*960  30P
#define SENSOR_DRIVER_MODE_4TO3_1D5M_30P_RESOLUTION     (RES_IDX_3840x2160_30P)            			// 1440*1080 30P
#define SENSOR_DRIVER_MODE_4TO3_3M_15P_RESOLUTION       (RES_IDX_3840x2160_30P)            			// 2016*1512 15P
#define SENSOR_DRIVER_MODE_4TO3_3M_30P_RESOLUTION       (RES_IDX_3840x2160_30P)            			// 4208*3120 10P

// Index 30
#define SENSOR_DRIVER_MODE_4TO3_5M_15P_RESOLUTION       (RES_IDX_3840x2160_30P)            			// 2560*1920 15P
#define SENSOR_DRIVER_MODE_4TO3_5M_30P_RESOLUTION       (RES_IDX_3840x2160_30P)            			// 2560*1920 30P
#define SENSOR_DRIVER_MODE_4TO3_8M_15P_RESOLUTION       (RES_IDX_3840x2160_30P)            			// 3264*2448 15P
#define SENSOR_DRIVER_MODE_4TO3_8M_30P_RESOLUTION       (RES_IDX_3840x2160_30P)            			// 3264*2448 30P
#define SENSOR_DRIVER_MODE_4TO3_10M_15P_RESOLUTION      (RES_IDX_3840x2160_30P)            			// 3648*2736 15P

// Index 35
#define SENSOR_DRIVER_MODE_4TO3_10M_30P_RESOLUTION      (RES_IDX_3840x2160_30P)            			// 3648*2736 30P
#define SENSOR_DRIVER_MODE_4TO3_12M_15P_RESOLUTION      (RES_IDX_3840x2160_30P)            			// 4032*3024 15P
#define SENSOR_DRIVER_MODE_4TO3_12M_30P_RESOLUTION      (RES_IDX_3840x2160_30P)            			// 4032*3024 30P
#define SENSOR_DRIVER_MODE_4TO3_14M_15P_RESOLUTION      (RES_IDX_4208x3120_25P)            			// 4352*3264 15P
#define SENSOR_DRIVER_MODE_4TO3_14M_30P_RESOLUTION      (RES_IDX_4208x3120_25P)            			// 4352*3264 30P

// For Camera Preview
#if (LCD_MODEL_RATIO_X == 16) && (LCD_MODEL_RATIO_Y == 9)
#define SENSOR_DRIVER_MODE_BEST_CAMERA_PREVIEW_RESOLUTION   	(RES_IDX_4208x3120_25P)
#else
#define SENSOR_DRIVER_MODE_BEST_CAMERA_PREVIEW_RESOLUTION   	(RES_IDX_4208x3120_25P)
#endif

#define SENSOR_DRIVER_MODE_BEST_CAMERA_CAPTURE_16TO9_RESOLUTION (RES_IDX_4208x3120_25P)
#define SENSOR_DRIVER_MODE_BEST_CAMERA_CAPTURE_4TO3_RESOLUTION  (RES_IDX_4208x3120_25P)

#define SENSOR_DRIVER_MODE_FULL_HD_30P_RESOLUTION_HDR   (RES_IDX_1920x1080_30P_HDR)

#endif //_SENSOR_IMX214_H_


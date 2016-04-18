#ifndef LDWS_H
#define LDWS_H

#include "iimage.h"

#ifdef __cplusplus
extern "C" {
#endif

#if (defined(_RUN_ON_ECLIPSE_) | defined(ARM_SIMU_ONLY))
#define printc(...)   printf(__VA_ARGS__)
#endif


enum
{
	LDWS_ERR_NONE = 0,
	LDWS_ERR_NULL_PARAMETER,
	LDWS_ERR_NOT_READY,
	LDWS_ERR_IC_CHECK
};

enum
{
	LDWS_STATE_NODETECT = -1,
	LDWS_STATE_NORMAL = 0,
	LDWS_STATE_DEPARTURE_LEFT,
	LDWS_STATE_DEPARTURE_RIGHT
};

typedef enum
{
	LDWS_SETUP_CENTER = 0,
	LDWS_SETUP_LEFT,
	LDWS_SETUP_RIGHT
}LdwsSetupPosi;

typedef struct _ldws_params_t {
	MyPOINT left_lane[2];
	MyPOINT right_lane[2];
	int     state;
} ldws_params_t;

typedef struct _ldws_tuning_params {
//	unsigned short roi_y_start_ratioX256;
//	unsigned short roi_y_end_ratioX256;
	unsigned short laneCalibrationImageWidth;
	unsigned short laneCalibrationImageHeight;
	unsigned short laneCalibrationUpPointX;
	unsigned short laneCalibrationUpPointY;
	unsigned short laneCalibrationLeftPointX;
	unsigned short laneCalibrationLeftPointY;
	unsigned short laneCalibrationRightPointX;
	unsigned short laneCalibrationRightPointY;
	unsigned short departureHighThr1;
	unsigned short departureHighThr2;
	unsigned short departureMiddleThr1;
	unsigned short departureMiddleThr2;
	unsigned short departureLowThr1;
	unsigned short departureLowThr2;
	unsigned short minLaneRatioX256;
	unsigned short maxLaneRatioX256;
	unsigned short alarmShowCnt;
	
} ldws_tuning_params;


typedef struct _ldws_debug_info_t
{
	float 	fSlpL;
	float 	fSlpR;
	int		iLnVldL;
	int		iLnVldR;

	int 	result_interFilter;
	int 	point1_x;
	int		point1_y;
	int		point2_x;
	int		point2_y;
	int		point3_x;
	int		point3_y;
	int		point4_x;
	int		point4_y;
	int		interFilter_w;
	int		interFilter_m;
	int		interFilter_M;
	int		left_slope_var_fail;
	int 	right_slope_var_fail;
	int 	left_lane_init_dbg_flag1;
	int 	left_lane_init_dbg_flag2;
	int 	left_lane_init_dbg_flag3;
	int 	left_lane_init_dbg_flag4;
	int 	left_lane_init_dbg_flag5;
	int 	right_lane_init_dbg_flag1;
	int 	right_lane_init_dbg_flag2;
	int 	right_lane_init_dbg_flag3;
	int 	right_lane_init_dbg_flag4;
	int 	right_lane_init_dbg_flag5;
} ldws_debug_info;

extern void* LDWS_malloc(unsigned int size);
extern int  ldws_init(int width, int height, unsigned char* working_buf_ptr, int working_buf_len, ldws_tuning_params tuing_parms, LdwsSetupPosi SetupPosi);
extern int  ldws_set_roi(int height, int roi_y1, int roi_y2);
extern void ldws_exit(void);
extern int  ldws_get_params(ldws_params_t *params);
extern int  ldws_process(IIMAGE *image);
extern unsigned int get_ldws_workingbuf_size_with_flag(int width, int height, int flag);
extern unsigned int get_ldws_workingbuf_size(int width, int height);
extern int ldws_get_camera_offset(void);
extern int ldws_left_lane_vld(void);
extern int ldws_right_lane_vld(void);
extern void ldws_set_left_lane_sensitivity(unsigned short sensitivity_factor_X256);
extern void ldws_set_right_lane_sensitivity(unsigned short sensitivity_factor_X256);
extern void ldws_set_integral_info(unsigned long* integral_image);
extern unsigned int ldws_GetLibVersion(unsigned int* ver);
extern void ldws_get_debug_info(ldws_debug_info *debug_info);

#ifdef __cplusplus
}
#endif

#endif

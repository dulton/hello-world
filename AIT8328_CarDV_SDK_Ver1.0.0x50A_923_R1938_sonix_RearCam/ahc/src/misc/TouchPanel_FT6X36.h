
#ifndef _TOUCH_PANEL_ZT2003_
#define _TOUCH_PANEL_ZT2003_

#include "customer_config.h"
#include "dsc_key.h"

/*===========================================================================
 * Macro define
 *===========================================================================*/

/*register address*/
#define FTS_REG_CHIP_ID						0xA3    			//chip ID 
#define FTS_REG_FW_VER						0xA6   			//FW  version 
#define FTS_REG_VENDOR_ID					0xA8   			// TP vendor ID 
#define FTS_REG_POINT_RATE					0x88   			//report rate	


#define TOUCH_LEVEL                                0
#define NON_TOUCH_LEVEL                            1


#define CFG_MAX_TOUCH_POINTS	2
#define MT_MAX_TOUCH_POINTS	9

#define PRESS_MAX	0xFF
#define FT_PRESS		0x7F

#define Proximity_Max	32

#define FT_FACE_DETECT_ON		0xc0
#define FT_FACE_DETECT_OFF		0xe0

#define FT_FACE_DETECT_ENABLE	1
#define FT_FACE_DETECT_DISABLE	0
#define FT_FACE_DETECT_REG		0xB0

#define FT6X06_NAME 	"ft6x06_ts"

#define FT_MAX_ID	0x0F
#define FT_TOUCH_STEP	6
#define FT_FACE_DETECT_POS		1
#define FT_TOUCH_X_H_POS		3
#define FT_TOUCH_X_L_POS		4
#define FT_TOUCH_Y_H_POS		5
#define FT_TOUCH_Y_L_POS		6
#define FT_TOUCH_EVENT_POS		3
#define FT_TOUCH_ID_POS			5

#define POINT_READ_BUF	(3 + FT_TOUCH_STEP * CFG_MAX_TOUCH_POINTS)

/*register address*/
#define FT6x06_REG_FW_VER		0xA6
#define FT6x06_REG_POINT_RATE	0x88
#define FT6x06_REG_THGROUP	0x80



#define ZT2003_DEBOUNCE_TIME                       120
#define ZT2003_CONTINUOUS_TIME                     100
#define ZT2003_PRESS_LEVEL                         0
#define ZT2003_INIT_VALUE                          1

#define FT6X36_ADC_STEPS_X                         (DISP_WIDTH)
#define FT6X36_ADC_STEPS_Y                         (DISP_HEIGHT)

#define BUTTON_CONFIG_TOUCHPANEL_NUM               1
#define BUTTON_CONFIG_TOUCHPANEL                   {TOUCH_S14_PRESS,   TOUCH_S14_RELEASE,   TOUCH_S14_MOVE, TOUCH_S14_RELEASE, MMP_GPIO21, 0, ZT2003_DEBOUNCE_TIME, ZT2003_CONTINUOUS_TIME, ZT2003_PRESS_LEVEL, ZT2003_INIT_VALUE, 0, 0, GPIOID_TOUCH, KEY_GPIO_ID_INVALID, 0, 0,	KEY_GPIO_ID_INVALID,KEY_GPIO_ID_INVALID, "TOUCH" },

/*===========================================================================
 * Function prototype
 *===========================================================================*/
#if 0
int TouchPanel_Init(void);
int TouchPanelCheckUpdate(MMP_ULONG* pulKeyEvent, MMP_ULONG* pulPosition, MMP_UBYTE dir);
int TouchPanelReadPosition(MMP_USHORT* pX, MMP_USHORT* pY, MMP_UBYTE dir);
int TouchPanelReadVBAT1(MMP_USHORT* pX, MMP_USHORT* pY, MMP_UBYTE dir);
void TouchPanel_ResetCtrl(MMP_BOOL bOn);
#endif
#endif // _TOUCH_PANEL_ZT2003_


#ifndef _TOUCH_PANEL_ZT2003_
#define _TOUCH_PANEL_ZT2003_

#include "customer_config.h"
#include "dsc_key.h"

/*===========================================================================
 * Macro define
 *===========================================================================*/

#define TOUCH_LEVEL                                0
#define NON_TOUCH_LEVEL                            1

#define ZT2003_DEBOUNCE_TIME					   120
#define ZT2003_CONTINUOUS_TIME                     100
#define ZT2003_PRESS_LEVEL                         0
#define ZT2003_INIT_VALUE                          1

#define ZT2003_ADC_STEPS						   4095

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
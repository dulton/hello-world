
#ifndef _TOUCH_PANEL_MSG213X_H_
#define _TOUCH_PANEL_MSG213X_H_

#include "customer_config.h"
#include "dsc_key.h"

/*===========================================================================
 * Macro define
 *===========================================================================*/

#define BUTTON_CONFIG_TOUCHPANEL_NUM               0
#define BUTTON_CONFIG_TOUCHPANEL
/*===========================================================================
 * Function prototype
 *===========================================================================*/

int TouchPanel_Init(void);
int TouchPanelCheckUpdate(MMP_ULONG* pulKeyEvent, MMP_ULONG* pulPosition, MMP_UBYTE dir);
int TouchPanelReadVBAT1(MMP_USHORT* pX, MMP_USHORT* pY, MMP_UBYTE dir);
void TouchPanelTransformPosition(MMP_USHORT* pX, MMP_USHORT* pY);

#define NEED_CHECK_RELEASE      1
int TouchPanelCheckRelease(void);
#endif // _TOUCH_PANEL_MSG213X_H_
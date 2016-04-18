/*===========================================================================
 * Include files
 *===========================================================================*/

#include "mmph_hif.h"
#include "mmpf_pio.h"
#include "mmpf_i2cm.h"
//#include "TouchPanel_Zt2003.h"
#include "AIT_init.h"
#include "AHC_utility.h"
#include "mmpf_touchpanel.h"
#include "mmp_reg_pad.h"

/*===========================================================================
 * Macro define
 *===========================================================================*/ 
#if  (DSC_TOUCH_PANEL == TOUCH_PANEL_DUMMY)

MMP_ERR TouchPanel_Init(struct _3RD_PARTY_TOUCHPANEL *pthis)
{
    return MMP_ERR_NONE;
}

void TouchPanel_TransformPosition(MMP_USHORT* pX, MMP_USHORT* pY, MMP_UBYTE dir) // Set dir = 0 as lcd normal, 1: reverse.
{
		*pX = 0;
		*pY = 0;
}
	
int TouchPanel_ReadPosition(struct _3RD_PARTY_TOUCHPANEL *pthis, MMP_USHORT* pX, MMP_USHORT* pY, MMP_UBYTE dir)
{
		*pX = 0;
		*pY = 0;
		return 0;
}


MMP_ERR TouchPanel_ReadVBAT1(struct _3RD_PARTY_TOUCHPANEL *pthis, MMP_USHORT* pX, MMP_USHORT* pY, MMP_UBYTE dir )
{
		*pX = 0;
		*pY = 0;
		return MMP_ERR_NONE;
}
void TouchPanel_ResetCtrl(struct _3RD_PARTY_TOUCHPANEL *pthis, MMP_BOOL bOn)
{
			
}

MMP_BOOL TouchPanel_Attached(struct _3RD_PARTY_TOUCHPANEL *pthis)
{
		return AHC_FALSE;
}

MMP_ERR TouchPanel_IOControl(struct _3RD_PARTY_TOUCHPANEL *pthis, TOUCHPANEL_ATTRIBUTE *ptouchpanel_attr, MMP_UBYTE cmd, MMP_UBYTE* arg)
{
		return MMP_ERR_NONE;
}

struct _3RD_PARTY_TOUCHPANEL TouchPanel_Obj =
{
    //0
        TouchPanel_Init,
        //TouchPanel_CheckUpdate,
        TouchPanel_ReadPosition,
        TouchPanel_ReadVBAT1,
    //    TouchPanel_TransformPosition,
        TouchPanel_ResetCtrl,

    //5

        TouchPanel_IOControl,
        TouchPanel_Attached,

};

int Touch_Panel_Module_Init(void)
{
    printc("[Touch_Panel_Module_Init]\r\n");
    MMPF_3RDParty_TouchPanel_Register(&TouchPanel_Obj);
    return 0;
}

#pragma arm section code = "initcall6", rodata = "initcall6", rwdata = "initcall6",  zidata = "initcall6"
#pragma O0
ait_module_init(Touch_Panel_Module_Init);
#pragma
#pragma arm section rodata, rwdata ,  zidata

#endif


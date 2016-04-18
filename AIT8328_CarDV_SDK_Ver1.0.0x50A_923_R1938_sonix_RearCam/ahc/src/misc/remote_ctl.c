//------------------------------------------------------------------------------
//
//  File        : remote_ctl.c
//  Description : 
//  Author      : scofield
//  Revision    : 1.0
//
//------------------------------------------------------------------------------
/**

*/

#if 0		 
void _____HEADER_____(){}
#endif

#include "remote_ctl.h"
#include "mmpf_pio.h"
#include "dsc_key.h"
#include "remote_ctl_cansonic89.h"
//#include "mmpf_timingmark.h"//Andy Liu TBD    


#if 0		 
void _____VARIABLES_____(){}
#endif
//------------------------------------------------------------------------------
//
//                              VARIABLES
//
//------------------------------------------------------------------------------
/**	
*/
#if (RMT_CTL_TYPE != RMT_CTL_DISABLE)

MMP_USHORT m_usKeyCommend[MAX_NUMBER_COMMAND] = 
{	DEVICE_POWER_PRESS, 
	KEYPAD_NONE,
	KEYPAD_S08S09_PRESS,
	KEYPAD_S13_PRESS,
	KEYPAD_S11_PRESS,
	//----------------------------- 5
	KEYPAD_S05_PRESS,
	KEYPAD_S06_PRESS,
	KEYPAD_S07_PRESS,
	KEYPAD_S10_PRESS,
	KEYPAD_S03_PRESS,
	//----------------------------- 10
	KEYPAD_S02_PRESS,
	KEYPAD_S04_PRESS,
	KEYPAD_S01_PRESS,
	KEYPAD_S12_PRESS,
	KEYPAD_NONE,
	//----------------------------- 15
	DEVICE_POWER_PRESS, 
	KEYPAD_NONE,
	KEYPAD_S09_RELEASE,
	KEYPAD_S13_RELEASE,
	KEYPAD_S11_RELEASE,
	//----------------------------- 20
	KEYPAD_S05_RELEASE,
	KEYPAD_S06_RELEASE,
	KEYPAD_S07_RELEASE,
	KEYPAD_S10_RELEASE,
	KEYPAD_S03_RELEASE,
	//----------------------------- 25
	KEYPAD_S02_RELEASE,
	KEYPAD_S04_RELEASE,
	KEYPAD_S01_RELEASE,
	KEYPAD_S12_RELEASE,
	KEYPAD_NONE 
};

void RMT_CTL_ParseCommand(RMT_CTL_COMMAND cmd);
//------------------------------------------------------------------------------
//  Function    : RMT_CTL_ConfigKeyDef
//  Description : 
//------------------------------------------------------------------------------

MMP_ERR RMT_CTL_ConfigKeyDef(MMP_USHORT* usKeyDef)
{
	MMP_ULONG ulIndex;
	
	for( ulIndex = 0 ; ulIndex < MAX_NUMBER_COMMAND ; ulIndex++)
	{
		if(usKeyDef != NULL)
			m_usKeyCommend[ulIndex] = *usKeyDef;
		else
			m_usKeyCommend[ulIndex] = 0xFFFF;
			
		usKeyDef++;
	}

	return MMP_ERR_NONE;
}

//------------------------------------------------------------------------------
//  Function    : RMT_CTL_Initalize
//  Description : 
//------------------------------------------------------------------------------
void RMT_CTL_Initalize(void)
{
	
	
	#if (RMT_CTL_TYPE == RMT_CTL_CANSONIC_89)
	RMT_CTL_TIME interval;
	RMT_CTL_Initalize_89(RMT_CTL_GPIO, RMT_CTL_TIMER, (RmtCtlCallBackFunc*)RMT_CTL_ParseCommand);

	interval.ulPreemptiveSig	= PREEMPTIVE_SIGNAL; 
	interval.ulLogical0Sig		= LOGICAL_0_SIGNAL;
	interval.ulLogical1Sig		= LOGICAL_1_SIGNAL;
	interval.ulRepeatedSig		= REPEAT_SIGNAL;
	interval.ulSigTolerance		= MAX_TIME_TOLERANCE;
	interval.ulRepeatedInterval = REPEAT_TIME;
	interval.ulRepeatedIntervalTolerance = REPEAT_TIME_TOLERANCE;
	
	RMT_CTL_InitailizeInterval_89(&interval); 
	#endif
}

//------------------------------------------------------------------------------
//  Function    : RMT_CTL_ParseCommand
//  Description : 
//------------------------------------------------------------------------------

void RMT_CTL_ParseCommand(RMT_CTL_COMMAND cmd)
{
	
	if( (cmd < MAX_NUMBER_COMMAND) &&
		(cmd >= 0) ){
	
		if(m_usKeyCommend[cmd] != 0xFFFF){
			SetKeyPadEvent(m_usKeyCommend[cmd]);
		}
	}
}

#endif

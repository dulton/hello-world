//------------------------------------------------------------------------------
//
//  File        : fdtc_ctl.h
//  Description : face detection and smile detecton
//  Author      : ChenChu Hsu
//  Revision    : 0.0
//
//------------------------------------------------------------------------------
#ifndef _remote_ctl_H_
#define _remote_ctl_H_

#include "includes_fw.h"
#include "mmpf_typedef.h"
#include "mmpf_pio.h"
#include "customer_config.h"

//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================

#if (RMT_CTL_TYPE == RMT_CTL_CANSONIC_89)

#define RMT_CTL_TIMER				(3)
//#define RMT_CTL_GPIO				(0x105)

#define PREEMPTIVE_SIGNAL			(13500)	///< (us)
#define LOGICAL_0_SIGNAL			(1120)	///< (us)
#define LOGICAL_1_SIGNAL			(2250)	///< (us)
#define REPEAT_SIGNAL				(11250)	///< (us)
#define MAX_TIME_TOLERANCE			(500)	///< (us)
/**	@brief	the interval of repeated signal is 110 ms. */
#define REPEAT_TIME					(110000)///< (us)
#define REPEAT_TIME_TOLERANCE		(10000)	///< (us)

#endif

#if (RMT_CTL_TYPE != RMT_CTL_DISABLE)

#define MAX_NUMBER_COMMAND			(30)
#define COMMAND_RELEASE_OFFSET		(15)


typedef enum _RMT_CTL_COMMAND{
	/**	@brief	when you add one command of press, you must add one command of release. 
				Add 1 to the parameter of MAX_NUMBER_COMMAND and COMMAND_RELEASE_OFFSET.
				
				ex: RMT_CTL_COMMAND_PRESS = 0
					RMT_CTL_COMMAND_RELEASE = 0 + COMMAND_RELEASE_OFFSET . 
				
				*/
	RMT_CTL_COMMAND_POWER_KEY_PRESS = 0,	///< DEVICE_POWER_PRESS
	RMT_CTL_COMMAND_PLAY_PRESS,				///< 
	RMT_CTL_COMMAND_CAPTURE_PRESS,
	RMT_CTL_COMMAND_RECORD_PRESS,
	RMT_CTL_COMMAND_DISPLAY_PRESS,			///< KEYPAD_S07_PRESS
	//----------------------------- 5
	RMT_CTL_COMMAND_MENU_PRESS,
	RMT_CTL_COMMAND_SELECT_PRESS,			///< KEYPAD_S06_PRESS
	RMT_CTL_COMMAND_TELE_PRESS_PRESS,		///< KEYPAD_S07_PRESS
	RMT_CTL_COMMAND_WIDE_PRESS_PRESS,		///< KEYPAD_S10_PRESS
	RMT_CTL_COMMAND_RIGHT_KEY_PRESS,		///< KEYPAD_S03_PRESS
	//----------------------------- 10
	RMT_CTL_COMMAND_LEFT_KEY_PRESS,			///< KEYPAD_S02_PRESS
	RMT_CTL_COMMAND_UP_KEY_PRESS,			///< KEYPAD_S04_PRESS
	RMT_CTL_COMMAND_DOWN_KEY_PRESS,			///< KEYPAD_S01_PRESS
	RMT_CTL_COMMAND_FLASH_PRESS,			///< KEYPAD_S12_PRESS
	RMT_CTL_COMMAND_DEFAULT_PRESS,
	//----------------------------- 15
	
	RMT_CTL_COMMAND_POWER_KEY_RELEASE,
	RMT_CTL_COMMAND_PLAY_RELEASE,
	RMT_CTL_COMMAND_CAPTURE_RELEASE,
	RMT_CTL_COMMAND_RECORD_RELEASE,
	RMT_CTL_COMMAND_DISPLAY_RELEASE,
	//----------------------------- 20
	
	RMT_CTL_COMMAND_MENU_RELEASE,
	RMT_CTL_COMMAND_SELECT_RELEASE,
	RMT_CTL_COMMAND_TELE_RELEASE,
	RMT_CTL_COMMAND_WIDE_RELEASE,
	RMT_CTL_COMMAND_RIGHT_KEY_RELEASE,
	//----------------------------- 25
	RMT_CTL_COMMAND_LEFT_KEY_RELEASE,
	RMT_CTL_COMMAND_UP_KEY_RELEASE,
	RMT_CTL_COMMAND_DOWN_KEY_RELEASE,
	RMT_CTL_COMMAND_FLASH_RELEASE,
	RMT_CTL_COMMAND_DEFAULT_RELEASE

}RMT_CTL_COMMAND;

void RMT_CTL_Initalize(void);

#endif // #if (RMT_CTL_TYPE != RMT_CTL_DISABLE)
#endif
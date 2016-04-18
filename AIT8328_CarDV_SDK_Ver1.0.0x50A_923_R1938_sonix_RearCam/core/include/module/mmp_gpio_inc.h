//------------------------------------------------------------------------------
//
//  File        : mmp_gpio_inc.h
//  Description : Header file of GPIO common information
//  Author      : Alterman
//  Revision    : 1.0
//
//------------------------------------------------------------------------------
#ifndef _MMP_GPIO_INC_H_
#define _MMP_GPIO_INC_H_

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================

#include "mmp_lib.h"
#include "mmp_reg_pad.h"

//==============================================================================
//
//                              CONSTANTS
//
//==============================================================================

#define GPIO_LOW        (0)
#define GPIO_HIGH       (1)

//==============================================================================
//
//                              DATA TYPES
//
//==============================================================================

typedef void GpioCallBackFunc(MMP_ULONG, ...);

//==============================================================================
//
//                              ENUMERATION
//
//==============================================================================

/*
 * [NOTE]
 * Please refer GPIO user/register guide to complete the following enum list.
 */
typedef enum _MMP_GPIO_PIN {
    MMP_GPIO0 = 0,
    MMP_GPIO1,
    MMP_GPIO2,
    MMP_GPIO3,
    MMP_GPIO4,
    MMP_GPIO5,
    MMP_GPIO6,
    MMP_GPIO7,
    MMP_GPIO8,
    MMP_GPIO9,
    MMP_GPIO10,
    MMP_GPIO11,
    MMP_GPIO12,
    MMP_GPIO13,
    MMP_GPIO14,
    MMP_GPIO15,
    MMP_GPIO16,
    MMP_GPIO17,
    MMP_GPIO18,
    MMP_GPIO19,
    MMP_GPIO20,
    MMP_GPIO21,
    MMP_GPIO22,
    MMP_GPIO23,
    MMP_GPIO24,
    MMP_GPIO25,
    MMP_GPIO26,
    MMP_GPIO27,
    MMP_GPIO28,
    MMP_GPIO29,
    MMP_GPIO30,
    MMP_GPIO31,
    MMP_GPIO32,
    MMP_GPIO33,
    MMP_GPIO34,
    MMP_GPIO35,
    MMP_GPIO36,
    MMP_GPIO37,
    MMP_GPIO38,
    MMP_GPIO39,
    MMP_GPIO40,
    MMP_GPIO41,
    MMP_GPIO42,
    MMP_GPIO43,
    MMP_GPIO44,
    MMP_GPIO45,
    MMP_GPIO46,
    MMP_GPIO47,
    MMP_GPIO48,
    MMP_GPIO49,
    MMP_GPIO50,
    MMP_GPIO51,
    MMP_GPIO52,
    MMP_GPIO53,
    MMP_GPIO54,
    MMP_GPIO55,
    MMP_GPIO56,
    MMP_GPIO57,
    MMP_GPIO58,
    MMP_GPIO59,
    MMP_GPIO60,
    MMP_GPIO61,
    MMP_GPIO62,
    MMP_GPIO63,
    MMP_GPIO64,
    MMP_GPIO65,
    MMP_GPIO66,
    MMP_GPIO67,
    MMP_GPIO68,
    MMP_GPIO69,
    MMP_GPIO70,
    MMP_GPIO71,
    MMP_GPIO72,
    MMP_GPIO73,
    MMP_GPIO74,
    MMP_GPIO75,
    MMP_GPIO76,
    MMP_GPIO77,
    MMP_GPIO78,
    MMP_GPIO79,
    MMP_GPIO80,
    MMP_GPIO81, // input only
    MMP_GPIO82, // input only
    MMP_GPIO83, // input only
    MMP_GPIO84, // input only
    MMP_GPIO85, // input only
    MMP_GPIO86, // input only
    MMP_GPIO87, // input only
    MMP_GPIO88, // input only
    MMP_GPIO89, // input only
    MMP_GPIO90, // input only
    MMP_GPIO91, // input only
    MMP_GPIO92, // input only
    MMP_GPIO93, // input only
    MMP_GPIO94, // input only
    MMP_GPIO95, // input only
    MMP_GPIO96, // input only
    MMP_GPIO97,
    MMP_GPIO98,
    MMP_GPIO99,
    MMP_GPIO100,
    MMP_GPIO101,
    MMP_GPIO102,
    MMP_GPIO103,
    MMP_GPIO104,
    MMP_GPIO105,
    MMP_GPIO106,
    MMP_GPIO107,
    MMP_GPIO108,
    MMP_GPIO109,
    MMP_GPIO110,
    MMP_GPIO111,
    MMP_GPIO112,
    MMP_GPIO113,
    MMP_GPIO114,
    MMP_GPIO115,
    MMP_GPIO116,
    MMP_GPIO117,
    MMP_GPIO118,
    MMP_GPIO119,
    MMP_GPIO120,
    MMP_GPIO121,
    MMP_GPIO122,
    MMP_GPIO123,
    MMP_GPIO124,
    MMP_GPIO125,
    MMP_GPIO126,
    MMP_GPIO127,
    MMP_GPIO_NUM,
    MMP_GPIO_MAX = 0xFFF
} MMP_GPIO_PIN;

/*
 * GPIO interrupt enable.
 */
typedef enum _MMP_GPIO_TRIG {
	GPIO_H2L_EDGE_TRIG  = 0,    ///< High to low
    GPIO_L2H_EDGE_TRIG  = 1,    ///< Low to high
    GPIO_H_LEVEL_TRIG   = 2,    ///< High level
    GPIO_L_LEVEL_TRIG   = 3,    ///< Low level
    GPIO_TRIG_INVALID
} MMP_GPIO_TRIG;

/*
 * GPIO output mode
 */
typedef enum _MMP_GPIO_MODE {
	GPIO_MODE_INPUT   = 0,
    GPIO_MODE_OUTPUT  = 1
} MMP_GPIO_MODE;

#endif // _MMP_GPIO_INC_H_


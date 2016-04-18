#ifndef _TOUCH_PANEL_GSL1680_
#define _TOUCH_PANEL_GSL1680_

#include "customer_config.h"
#include "dsc_key.h"

/*===========================================================================
 * Macro define
 *===========================================================================*/
//register address 

#define GSL_DATA_REG		0x80
#define GSL_STATUS_REG		0xe0
#define GSL_PAGE_REG		0xf0

#define PRESS_MAX    		255
#define MAX_FINGERS 		2//5
#define MAX_CONTACTS 		1  //10
#define DMA_TRANS_LEN		0x20

#define TOUCH_PANEL_GSL_I2C_SLAVE_ADDRESS 	    0x80>>1  //0xba>>1  // for AIT I2C HW setting, slave address 0x90 should shif 1 bit (>> 1)

#define RESOLUTION_INDEX                    1
#define TRIGGER_INDEX                       6
#define MAX_TOUCH_POINT                     5


#define FILTER_MAX	6

#define GSL_ADC_STEPS_X                   (DISP_WIDTH)
#define GSL_ADC_STEPS_Y                   (DISP_HEIGHT)
/////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct gsl_ts_data {
	MMP_UBYTE x_index;
	MMP_UBYTE y_index;
	MMP_UBYTE z_index;
	MMP_UBYTE id_index;
	MMP_UBYTE touch_index;
	MMP_UBYTE data_reg;
	MMP_UBYTE status_reg;
	MMP_UBYTE data_size;
	MMP_UBYTE touch_bytes;
	MMP_UBYTE update_data;
	MMP_UBYTE touch_meta_data;
	MMP_UBYTE finger_size;
}GSL_TS_DATA, *PGSL_TS_DATA;


typedef struct gsl_ts {
	GSL_TS_DATA *dd;
	MMP_USHORT *touch_data;
}GSL_TS, *PGSL_TS;

//////////////////////////////////////////////////////////////////////////////////////////////////////


#endif // _TOUCH_PANEL_GT911_

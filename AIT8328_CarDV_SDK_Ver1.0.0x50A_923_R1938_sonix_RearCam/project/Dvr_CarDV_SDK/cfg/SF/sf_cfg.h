
#ifndef _SF_CFG_H_
#define _SF_CFG_H_

//==============================================================================
//
//  File        : sf_cfg.h
//  Description : SPI Nor Flash configure file
//  Author      : Rony Yeh
//  Revision    : 1.0
//==============================================================================

#include "config_fw.h"
#include "mmpf_typedef.h"

//==============================================================================
//
//                              Define
//
//==============================================================================

#if (USING_SF_IF) 
#define SPIMAXMODULENO                  (15) 
//If use new module, create a new define for the module
#define SPI_NorFlash_DefaultSetting		(1) // this should be always enable for default setting
// WINBOND
#define SPI_NorFlash_WINBOND_25X10		(1)
#define SPI_NorFlash_WINBOND_25Q32      (1)     // 4M, 4K/32K
#define SPI_NorFlash_WINBOND_25Q64		(1)
#define SPI_NorFlash_WINBOND_25Q128 	(1)
#define SPI_NorFlash_WINBOND_25Q256FV   (1)
// MXIC
#define SPI_NorFlash_MXIC_25L512		(1)
#define SPI_NorFlash_MXIC_25L12835F     (1)
#define SPI_NorFlash_KH25L3233F         (1)     // 4M, 4K/32K
#define SPI_NorFlash_MXIC_25L3206       (!SPI_NorFlash_KH25L3233F & 1)  // 4M, 4K/64K, Same ID with with KH25L3233F
#define SPI_NorFlash_MXIC_25L6406E      (1)     // 8M
// ESMT
#define SPI_NorFlash_ESMT_64L           (0)
// EON
#define SPI_NorFlash_EN25Q32A           (0)
#define SPI_NorFlash_EN25Q128A          (0)
// GD
#define SPI_NorFlash_GD_25Q64B          (1)
#define SPI_NorFlash_GD_25Q128B			(1)

#endif

//==============================================================================
//
//                              Extern Variable
//   
//==============================================================================


//==============================================================================
//
//                              STRUCTURES
//
//==============================================================================


#endif //_SF_CFG_H_

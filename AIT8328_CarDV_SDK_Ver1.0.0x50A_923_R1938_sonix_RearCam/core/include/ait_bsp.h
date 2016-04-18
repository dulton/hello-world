/** @file ait_bsp.h

All BSP dependent configuration in this file.

@brief BSP dependent configuration compiler options
@author Philip Lin
@since 10-Jul-08
@version
- 1.0 Original version
*/

#ifndef AIT_BSP_H
#define AIT_BSP_H

#include "mmp_lib.h"

/** @name System
@{ */

    /*-----------macro declaration-------------------------------------*/

    /** @brief Host CPU clock in mHz. The demo kit uses 66mHz ARM7.
        The customer should modify this value.*/
    #define HOST_CLK_M  144

    /** @brief The clock cycles for waiting in the ARM7.
        It's used in busy waiting to wait a certain period of time.*/
    #define HOST_WHILE_CYCLE 4

    /** @brief The chip ID which the current software supports
    */
    #define	P_V2        (1)
    #define MCR_V2      (2)

    /** @} */ // end of system

void	MMPC_System_WaitCount(MMP_ULONG count);
void	MMPC_System_WaitMs(MMP_ULONG ms);
void	MMPC_System_WaitUs(MMP_ULONG us);

/** @brief This is the host interface clock in mHz.
	Usually it runs under 82x CPU clock / 2.*/
	
#ifdef BUILD_CE
    #define RTNA_CLK_MHZ      (192)
    #define RTNA_CPU_CLK_MHZ  (RTNA_CLK_MHZ/1)
#endif

#if ((HOST_CLK_M*2)>RTNA_CPU_CLK_MHZ)
#define MMPC_System_WaitMmp(cycle)        MMPC_System_WaitCount(((HOST_CLK_M * cycle) / (RTNA_CPU_CLK_MHZ * HOST_WHILE_CYCLE)) + 1);
#else
#define MMPC_System_WaitMmp(cycle)
#endif

#endif /* AIT_BSP_H */

//==============================================================================
//
//  File        : bsp.h
//  Description : Board support package source code
//  Author      : Jerry Tsao
//  Revision    : 1.0
//
//==============================================================================

#ifndef _BSP_H_
#define _BSP_H_

//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================

/** @addtogroup BSP
@{
*/

typedef void (*ARM_EXCEPTION_HANDLERCB)(void);

MMP_ERR MMPF_BSP_Initialize(void);
MMP_ERR MMPF_BSP_InitializeInt(void);
MMP_ERR MMPF_BSP_InitializeTimer(void);
MMP_ERR MMPF_BSP_IntHandler(void);
void MMPF_MMU_Initialize(unsigned int *pTranslationTable,unsigned int *pCoarsePageTable);
void MMPF_MMU_Deinitialize(void);

MMP_ERR MMPF_Register_ARM_ExceptionHandlerCB(ARM_EXCEPTION_HANDLERCB pARMExceptionHandlerCB);
void ARM_ExceptionHandler(MMP_ULONG exceptID, MMP_ULONG lr);

//==============================================================================
//
//                              ARMUL Definition
//
//==============================================================================

#if defined(ARMUL)

typedef volatile unsigned int ARMUL_REG;// Hardware register definition

typedef struct _ARMULS_INT {
    ARMUL_REG       IRQST;              // 0x000
    ARMUL_REG       IRQRAWST;
    ARMUL_REG       IRQENSET;
    ARMUL_REG       IRQENCLR;
    ARMUL_REG       IRQSOFT;
    ARMUL_REG       reserved0[59];
    ARMUL_REG       FIQST;              // 0x100
    ARMUL_REG       FIQRAWST;
    ARMUL_REG       FIQENSET;
    ARMUL_REG       FIQENCLR;
} ARMULS_INT, *ARMULPS_INT;

typedef struct _ARMULS_TMR {
    ARMUL_REG       T1LOAD;             // 0x00
    ARMUL_REG       T1VAL;
    ARMUL_REG       T1CTRL;
    ARMUL_REG       T1CLR;
    ARMUL_REG       reserved0[4];
    ARMUL_REG       T2LOAD;             // 0x20
    ARMUL_REG       T2VAL;
    ARMUL_REG       T2CTRL;
    ARMUL_REG       T2CLR;
} ARMULS_TMR, *ARMULPS_TMR;

#define ARMULC_BASE_INT             ((ARMULPS_INT)0x0A000000)
#define ARMULC_BASE_TMR             ((ARMULPS_TMR)0x0A800000)

#define ARMULR_INT_IRQST            ((ARMUL_REG*)0x0A000000)
#define ARMULR_INT_IRQRAWST         ((ARMUL_REG*)0x0A000004)
#define ARMULR_INT_IRQENSET         ((ARMUL_REG*)0x0A000008)
#define ARMULR_INT_IRQENCLR         ((ARMUL_REG*)0x0A00000C)
#define ARMULR_INT_IRQSOFT          ((ARMUL_REG*)0x0A000010)
#define ARMULR_INT_FIQST            ((ARMUL_REG*)0x0A000100)
#define ARMULR_INT_FIQRAWST         ((ARMUL_REG*)0x0A000104)
#define ARMULR_INT_FIQENSET         ((ARMUL_REG*)0x0A000108)
#define ARMULR_INT_FIQENCLR         ((ARMUL_REG*)0x0A00010C)
    #define ARMULC_INT_SRC_FIQ          ((unsigned int) 1 <<  0)
    #define ARMULC_INT_SRC_PRG          ((unsigned int) 1 <<  1)
    #define ARMULC_INT_SRC_RX           ((unsigned int) 1 <<  2)
    #define ARMULC_INT_SRC_TX           ((unsigned int) 1 <<  3)
    #define ARMULC_INT_SRC_T1           ((unsigned int) 1 <<  4)
    #define ARMULC_INT_SRC_T2           ((unsigned int) 1 <<  5)

#define ARMULR_TMR_T1LOAD           ((ARMUL_REG*)0x0A800000)
#define ARMULR_TMR_T1VAL            ((ARMUL_REG*)0x0A800004)
#define ARMULR_TMR_T1CTRL           ((ARMUL_REG*)0x0A800008)
#define ARMULR_TMR_T1CLR            ((ARMUL_REG*)0x0A80000C)
#define ARMULR_TMR_T2LOAD           ((ARMUL_REG*)0x0A800020)
#define ARMULR_TMR_T2VAL            ((ARMUL_REG*)0x0A800024)
#define ARMULR_TMR_T2CTRL           ((ARMUL_REG*)0x0A800028)
#define ARMULR_TMR_T2CLR            ((ARMUL_REG*)0x0A80002C)
    #define ARMULC_TMR_CTRL_DIV1        ((unsigned int) 0 <<  2)
    #define ARMULC_TMR_CTRL_DIV16       ((unsigned int) 1 <<  2)
    #define ARMULC_TMR_CTRL_DIV256      ((unsigned int) 2 <<  2)
    #define ARMULC_TMR_CTRL_FREERUN     ((unsigned int) 0 <<  6)
    #define ARMULC_TMR_CTRL_PERIOD      ((unsigned int) 1 <<  6)
    #define ARMULC_TMR_CTRL_EN          ((unsigned int) 1 <<  7)

#endif // ARMUL

/** @} */ // end of BSP

#endif //_BSP_H_


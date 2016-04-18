/// @ait_only
//==============================================================================
//
//  File        : mmpd_spi.c
//  Description : Ritina Programmable Serial Peripheral Interface Module Control driver function
//  Author      : Penguin Torng
//  Revision    : 1.0
//
//==============================================================================

/**
*  @file mmpd_spi.c
*  @brief The PSPI Module Control functions
*  @author Penguin Torng
*  @version 1.0
*/

#include 	"mmp_lib.h"
#include 	"mmpd_spi.h"
#include 	"ait_utility.h"
#include 	"mmph_hif.h"
#include 	"mmpd_system.h"
#include 	"mmp_reg_spi.h"
#include 	"mmp_reg_gbl.h"

/** @addtogroup MMPD_Spi
 *  @{
 */
#if (CHIP == DIAMOND)
//==============================================================================
//
//                              VARIABLES
//
//==============================================================================

//==============================================================================
//
//                              FUNCTION PROTOTYPES
//
//==============================================================================

//------------------------------------------------------------------------------
//  Function    : MMPD_SPI_SetAttributes
//  Description :
//------------------------------------------------------------------------------
/** @brief The function sets the attributes to the specified spi channel with its spi ID

The function sets the attributes to the specified spi channel with its icon ID. These attributes include
spi master/slave mode, signal type, and related clock. It is implemented by programming PSPI
Controller registers to set those attributes.

  @param[in] spiID the SPI ID
  @param[in] spiattribute the SPI attribute
  @return It reports the status of the operation.
*/
MMP_ERR MMPD_SPI_SetAttributes(MMPD_SPI_ID spiID, MMPD_SPI_ATTRIBUTE *spiattribute)
{
    MMP_USHORT  offset;

    MMPD_System_EnableClock(MMPD_SYSTEM_CLK_MODULE_SPI, MMP_TRUE);

#if (CHIP == DIAMOND)
    MMPH_HIF_RegSetL(GBL_MIO_SPI_CTL, MMPH_HIF_RegGetL(GBL_MIO_SPI_CTL) | 
    									GBL_SPI_PADMAP(spiattribute->padCtl, spiID) | 
    									GBL_SPI_PADSET_EN(spiattribute->padCtl)) ;
#endif

    offset = spiID * SPI_OFFSET;

    MMPH_HIF_RegSetB(SPI_CTL + offset, SPI_TXFIFO_CLR+SPI_RXFIFO_CLR);
    if (spiattribute->mode == MMPD_SPI_MASTER_MODE) {
        MMPH_HIF_RegSetW(SPI_CFG + offset, MASTER_RX_PAD_CLK | SPI_MASTER_MODE | spiattribute->usSignalCtl);

        PRINTF("MASTER : SPI_CFG = 0x%X\r\n", MMPH_HIF_RegGetW(SPI_CFG + offset));

        MMPH_HIF_RegSetB(SPI_WORD_LEN + offset, spiattribute->ubWordLength - 1);
        MMPH_HIF_RegSetB(SPI_CLK_DIV + offset, spiattribute->ubSclkDiv);
        MMPH_HIF_RegSetW(SPI_DLY_CYCLE + offset, spiattribute->ubPspiDelay);
        MMPH_HIF_RegSetW(SPI_WAIT_CYCLE + offset, spiattribute->ubPspiWait);
    }
    else {
        MMPH_HIF_RegSetW(SPI_CFG + offset, MASTER_RX_PAD_CLK|spiattribute->usSignalCtl);

        PRINTF("SLAVE : SPI_CFG = 0x%X\r\n", MMPH_HIF_RegGetW(SPI_CFG + offset));

        MMPH_HIF_RegSetB(SPI_WORD_LEN + offset, spiattribute->ubWordLength - 1);
    }

    return  MMP_ERR_NONE;
}


//------------------------------------------------------------------------------
//  Function    : MMPD_SPI_Operation
//  Description :
//------------------------------------------------------------------------------
/** @brief The function sets the attributes to the specified spi channel with its spi ID

The function sets the attributes to the specified spi channel with its icon ID. These attributes include
spi master/slave mode, signal type, and related clock. It is implemented by programming PSPI
Controller registers to set those attributes.

  @param[in] spiID the SPI ID
  @param[in] spiop the SPI operation
  @return It reports the status of the operation.
*/
MMP_ERR MMPD_SPI_Operation(MMPD_SPI_ID spiID, MMPD_SPI_OPERATION *spiop)
{
    MMP_USHORT  offset;
    MMP_BOOL    tx_en = MMP_FALSE, rx_en = MMP_FALSE;
    MMP_BOOL    tx_dma_mode = MMP_FALSE, rx_dma_mode = MMP_FALSE;

    MMP_USHORT  tmp;
    MMP_USHORT  outbyte, transferbyte, i;
    MMP_UBYTE   *ptr, *ptr2;
    MMP_USHORT  byte_count;

    if (spiop->dir & MMPD_SPI_TX) {
        tx_en = MMP_TRUE;
        if (spiop->ulTxDmaAddr != 0) {
            tx_dma_mode = MMP_TRUE;
        }
    }
    if (spiop->dir & MMPD_SPI_RX) {
        rx_en = MMP_TRUE;
        if (spiop->ulRxDmaAddr != 0) {
            rx_dma_mode = MMP_TRUE;
        }
    }

    offset = spiID * SPI_OFFSET;

    if (tx_en == MMP_TRUE) {
        if (rx_en == MMP_TRUE) {
            // tx/rx at the same time
            if (tx_dma_mode == MMP_TRUE && rx_dma_mode == MMP_TRUE) {
                // clear interrupt
                MMPH_HIF_RegSetL(SPI_INT_HOST_SR + offset, SPI_TXDMA_DONE | SPI_RXDMA_DONE | SPI_SLAVE_ERR);
                if (!(MMPH_HIF_RegGetW(SPI_CFG + offset) & SPI_MASTER_MODE)) {
                    MMPH_HIF_RegSetL(SPI_INT_HOST_SR + offset, SPI_SLAVE_ERR);
                }

                MMPH_HIF_RegSetL(SPI_TXDMA_ADDR + offset, spiop->ulTxDmaAddr);
                MMPH_HIF_RegSetL(SPI_RXDMA_ADDR + offset, spiop->ulRxDmaAddr);
                MMPH_HIF_RegSetW(SPI_TXDMA_SIZE + offset, spiop->usTransferSize - 1);
                MMPH_HIF_RegSetW(SPI_RXDMA_SIZE + offset, spiop->usTransferSize - 1);

                tmp = MMPH_HIF_RegGetW(SPI_CFG + offset) | (TX_XCH_MODE|SPI_TX_EN | SPI_RX_EN);
                MMPH_HIF_RegSetW(SPI_CFG + offset, tmp);

                MMPH_HIF_RegSetW(SPI_CTL + offset, SPI_RX_DMA_START | SPI_TX_DMA_START);

                if (!(MMPH_HIF_RegGetW(SPI_CFG + offset) & TX_NON_XCH_MODE) ){
                    if (MMPH_HIF_RegGetW(SPI_CFG+ offset) & SPI_MASTER_MODE){
                    MMPH_HIF_RegSetW(SPI_XCH_CTL + offset, XCH_START);
                    }
                }
                while (!(MMPH_HIF_RegGetL(SPI_INT_HOST_SR + offset) & SPI_TXDMA_DONE));
                while (!(MMPH_HIF_RegGetL(SPI_INT_HOST_SR + offset) & SPI_FIFO_TX_DONE));
                while (!(MMPH_HIF_RegGetL(SPI_INT_HOST_SR + offset) & SPI_RXDMA_DONE));

            }

            // fifo mode
            else {
                if (!(MMPH_HIF_RegGetW(SPI_CFG + offset) & SPI_MASTER_MODE)) {
                    MMPH_HIF_RegSetL(SPI_INT_HOST_SR + offset, SPI_SLAVE_ERR);
                }
				tmp = (MMPH_HIF_RegGetW(SPI_CFG + offset) |(SPI_RX_EN + SPI_TX_EN+TX_NON_XCH_MODE));
                MMPH_HIF_RegSetW(SPI_CFG + offset, tmp);
                outbyte = spiop->usTransferSize;
                ptr = spiop->ubTxFifoPtr;
                ptr2 = spiop->ubRxFifoPtr;

                if (MMPH_HIF_RegGetW(SPI_WORD_LEN) >= 16) {
                    MMPH_HIF_RegSetB(SPI_TXFIFO_DATA + offset, *ptr++);
                    MMPH_HIF_RegSetB(SPI_TXFIFO_DATA + offset, *ptr++);
                    MMPH_HIF_RegSetB(SPI_TXFIFO_DATA + offset, *ptr++);
                    outbyte -= 3;
                }
                else if (MMPH_HIF_RegGetW(SPI_WORD_LEN) >= 8) {
                    MMPH_HIF_RegSetB(SPI_TXFIFO_DATA + offset, *ptr++);
                    MMPH_HIF_RegSetB(SPI_TXFIFO_DATA + offset, *ptr++);
                    outbyte -= 2;
                }
                else {
                    MMPH_HIF_RegSetB(SPI_TXFIFO_DATA + offset, *ptr++);
                    outbyte -= 1;
                }

                while (outbyte) {
                    transferbyte = 32 - MMPH_HIF_RegGetB(SPI_TXFIFO_SPC + offset);
                    transferbyte =  (transferbyte > outbyte) ?  outbyte : transferbyte;
                    if (transferbyte) {
                        for (i = 0; i < transferbyte; i++) {
                            MMPH_HIF_RegSetB(SPI_TXFIFO_DATA + offset, *ptr++);
                            if(MMPH_HIF_RegGetB(SPI_RXFIFO_SPC + offset)) {
                                *ptr2++ = MMPH_HIF_RegGetB(SPI_RXFIFO_DATA + offset);
                            }
                        }
                    }
                    else {
                        if (!(MMPH_HIF_RegGetW(SPI_CFG + offset) & SPI_MASTER_MODE)) {
                            if (MMPH_HIF_RegGetL(SPI_INT_HOST_SR + offset) & SPI_TXFIFO_UF) {
                                PRINTF("Slave TX underflow\r\n");
                                return MMP_SPI_ERR_TX_UNDERFLOW;
                            }
                        }
                    }
                    outbyte -= transferbyte;
                }
                while (!(MMPH_HIF_RegGetL(SPI_INT_HOST_SR + offset) & SPI_FIFO_TX_DONE));
                while(MMPH_HIF_RegGetB(SPI_RXFIFO_SPC + offset)) {
                      *ptr2++ = MMPH_HIF_RegGetB(SPI_RXFIFO_DATA + offset);
                }
            }

        }
        else {
            // tx only
            if (tx_dma_mode == MMP_TRUE) {
                // tx only, dma mode
                // clear interrupt
                MMPH_HIF_RegSetL(SPI_INT_HOST_SR + offset, SPI_TXDMA_DONE);
                if (!(MMPH_HIF_RegGetW(SPI_CFG + offset) & SPI_MASTER_MODE)) {
                    MMPH_HIF_RegSetL(SPI_INT_HOST_SR + offset, SPI_SLAVE_ERR);
                }

                MMPH_HIF_RegSetL(SPI_TXDMA_ADDR + offset, spiop->ulTxDmaAddr);
                MMPH_HIF_RegSetW(SPI_TXDMA_SIZE + offset, spiop->usTransferSize - 1);

                tmp = (MMPH_HIF_RegGetW(SPI_CFG + offset) & ~SPI_RX_EN) | (SPI_TX_EN|TX_XCH_MODE);
                MMPH_HIF_RegSetW(SPI_CFG + offset, tmp);

                MMPH_HIF_RegSetW(SPI_CTL + offset, SPI_TX_DMA_START);
                if (!(MMPH_HIF_RegGetW(SPI_CFG + offset) & TX_NON_XCH_MODE) ){
                    if (MMPH_HIF_RegGetW(SPI_CFG+ offset) & SPI_MASTER_MODE){                    
                    MMPH_HIF_RegSetW(SPI_XCH_CTL + offset, XCH_START);
                    }
                }
                while (!(MMPH_HIF_RegGetL(SPI_INT_HOST_SR + offset) & SPI_TXDMA_DONE));
                while (!(MMPH_HIF_RegGetL(SPI_INT_HOST_SR + offset) & SPI_FIFO_TX_DONE));
            }
            else {
                // tx only, fifo mode
                if (!(MMPH_HIF_RegGetW(SPI_CFG + offset) & SPI_MASTER_MODE)) {
                    MMPH_HIF_RegSetL(SPI_INT_HOST_SR + offset, SPI_SLAVE_ERR);
                }

				tmp = (MMPH_HIF_RegGetW(SPI_CFG + offset) & ~SPI_RX_EN) | (SPI_TX_EN+TX_NON_XCH_MODE);
                MMPH_HIF_RegSetW(SPI_CFG + offset, tmp);

                outbyte = spiop->usTransferSize;
                ptr = spiop->ubTxFifoPtr;

                if (MMPH_HIF_RegGetW(SPI_WORD_LEN) >= 16) {
                    MMPH_HIF_RegSetB(SPI_TXFIFO_DATA + offset, *ptr++);
                    MMPH_HIF_RegSetB(SPI_TXFIFO_DATA + offset, *ptr++);
                    MMPH_HIF_RegSetB(SPI_TXFIFO_DATA + offset, *ptr++);
                    outbyte -= 3;
                }
                else if (MMPH_HIF_RegGetW(SPI_WORD_LEN) >= 8) {
                    MMPH_HIF_RegSetB(SPI_TXFIFO_DATA + offset, *ptr++);
                    MMPH_HIF_RegSetB(SPI_TXFIFO_DATA + offset, *ptr++);
                    outbyte -= 2;
                }
                else {
                    MMPH_HIF_RegSetB(SPI_TXFIFO_DATA + offset, *ptr++);
                    outbyte -= 1;
                }
                if (!(MMPH_HIF_RegGetW(SPI_CFG + offset) & TX_NON_XCH_MODE) ){
                    if (MMPH_HIF_RegGetW(SPI_CFG+ offset) & SPI_MASTER_MODE){                
                    MMPH_HIF_RegSetW(SPI_XCH_CTL, XCH_START);
                    }
                }
                while (outbyte) {
                    transferbyte = 32 - MMPH_HIF_RegGetB(SPI_TXFIFO_SPC + offset);
                    transferbyte =  (transferbyte > outbyte) ?  outbyte : transferbyte;
                    if (transferbyte) {
                        for (i = 0; i < transferbyte; i++) {
                            MMPH_HIF_RegSetB(SPI_TXFIFO_DATA + offset, *ptr++);
                        }
                    }
                    else {
                        if (!(MMPH_HIF_RegGetW(SPI_CFG + offset) & SPI_MASTER_MODE)) {
                            if (MMPH_HIF_RegGetL(SPI_INT_HOST_SR + offset) & SPI_TXFIFO_UF) {
                                PRINTF("Slave TX underflow\r\n");
                                return MMP_SPI_ERR_TX_UNDERFLOW;
                            }
                        }
                    }
                    outbyte -= transferbyte;
                }

                while (!(MMPH_HIF_RegGetL(SPI_INT_HOST_SR + offset) & SPI_FIFO_TX_DONE));
            }
        }
    }
    else {
        // rx only
        if (rx_dma_mode == MMP_TRUE) {
            // rx only ,dma mode
            // clear interrupt status
            MMPH_HIF_RegSetL(SPI_INT_HOST_SR + offset, SPI_RXDMA_DONE);
            if (!(MMPH_HIF_RegGetW(SPI_CFG + offset) & SPI_MASTER_MODE)) {
                MMPH_HIF_RegSetL(SPI_INT_HOST_SR + offset, SPI_SLAVE_ERR);
            }
            else {
                MMPH_HIF_RegSetW(SPI_TXDMA_SIZE + offset, spiop->usTransferSize - 1);
            }
            MMPH_HIF_RegSetL(SPI_RXDMA_ADDR + offset, spiop->ulRxDmaAddr);
            MMPH_HIF_RegSetW(SPI_RXDMA_SIZE + offset, spiop->usTransferSize - 1);

            tmp = (MMPH_HIF_RegGetW(SPI_CFG + offset) & ~SPI_TX_EN) | (SPI_RX_EN|TX_XCH_MODE);
            MMPH_HIF_RegSetW(SPI_CFG + offset, tmp);

            MMPH_HIF_RegSetW(SPI_CTL + offset, SPI_RX_DMA_START);
            if (!(MMPH_HIF_RegGetW(SPI_CFG + offset) & TX_NON_XCH_MODE) ){
            if (MMPH_HIF_RegGetW(SPI_CFG + offset) & SPI_MASTER_MODE) {
                MMPH_HIF_RegSetW(SPI_XCH_CTL + offset, XCH_START);
            }
			}
            while (!(MMPH_HIF_RegGetL(SPI_INT_HOST_SR + offset) & SPI_RXDMA_DONE));

        }
        else {
            // rx only fifo mode
            if (!(MMPH_HIF_RegGetW(SPI_CFG + offset) & SPI_MASTER_MODE)) {
                MMPH_HIF_RegSetL(SPI_INT_HOST_SR + offset, SPI_SLAVE_ERR);

    			tmp = (MMPH_HIF_RegGetW(SPI_CFG + offset) & ~SPI_TX_EN) | (SPI_RX_EN+TX_NON_XCH_MODE);
                MMPH_HIF_RegSetW(SPI_CFG + offset, tmp);

                outbyte = spiop->usTransferSize;
                ptr = spiop->ubRxFifoPtr;
                while (outbyte) {
                    transferbyte = MMPH_HIF_RegGetB(SPI_RXFIFO_SPC + offset);
                    transferbyte =  (transferbyte > outbyte) ?  outbyte : transferbyte;
                    for (i = 0; i < transferbyte; i++) {
                        *ptr++ = MMPH_HIF_RegGetB(SPI_RXFIFO_DATA + offset);
                    }
                    outbyte -= transferbyte;
                }
            }
            else {
                // for master mode, enable tx fifo, too
                if (MMPH_HIF_RegGetW(SPI_WORD_LEN + offset) >= 16)
                    byte_count = 3;
                else if (MMPH_HIF_RegGetW(SPI_WORD_LEN + offset) >= 8)
                    byte_count = 2;
                else
                    byte_count = 1;

    			tmp = MMPH_HIF_RegGetW(SPI_CFG + offset) | (SPI_TX_EN | SPI_RX_EN|TX_NON_XCH_MODE);
                MMPH_HIF_RegSetW(SPI_CFG + offset, tmp);

                outbyte = spiop->usTransferSize;
                ptr = spiop->ubRxFifoPtr;
                if (byte_count == 3) {
                    while (outbyte) {
                        transferbyte = 32 - MMPH_HIF_RegGetB(SPI_TXFIFO_SPC + offset);
                        transferbyte =  (transferbyte > outbyte) ?  outbyte : transferbyte;
                        for (i = 0; i < transferbyte; i+=3) {
                            MMPH_HIF_RegSetB(SPI_TXFIFO_DATA + offset, 0xFF);
                            MMPH_HIF_RegSetB(SPI_TXFIFO_DATA + offset, 0xFF);
                            MMPH_HIF_RegSetB(SPI_TXFIFO_DATA + offset, 0xFF);
                        }
                        while (MMPH_HIF_RegGetB(SPI_RXFIFO_SPC + offset) != transferbyte);
                        for (i = 0; i < transferbyte; i+=3) {
                            *ptr++ = MMPH_HIF_RegGetB(SPI_RXFIFO_DATA + offset);
                            *ptr++ = MMPH_HIF_RegGetB(SPI_RXFIFO_DATA + offset);
                            *ptr++ = MMPH_HIF_RegGetB(SPI_RXFIFO_DATA + offset);
                        }
                        outbyte -= transferbyte;
                    }
                }
                else if (byte_count == 2) {
                    while (outbyte) {
                        transferbyte = 32 - MMPH_HIF_RegGetB(SPI_TXFIFO_SPC + offset);
                        transferbyte =  (transferbyte > outbyte) ?  outbyte : transferbyte;
                        for (i = 0; i < transferbyte; i+=2) {
                            MMPH_HIF_RegSetB(SPI_TXFIFO_DATA + offset, 0xFF);
                            MMPH_HIF_RegSetB(SPI_TXFIFO_DATA + offset, 0xFF);
                        }
                        while (MMPH_HIF_RegGetB(SPI_RXFIFO_SPC + offset) != transferbyte);
                        for (i = 0; i < transferbyte; i+=2) {
                            *ptr++ = MMPH_HIF_RegGetB(SPI_RXFIFO_DATA + offset);
                            *ptr++ = MMPH_HIF_RegGetB(SPI_RXFIFO_DATA + offset);
                        }
                        outbyte -= transferbyte;
                    }
                }
                else if (byte_count == 1) {
                    while (outbyte) {
                        transferbyte = 32 - MMPH_HIF_RegGetB(SPI_TXFIFO_SPC + offset);
                        transferbyte =  (transferbyte > outbyte) ?  outbyte : transferbyte;
                        for (i = 0; i < transferbyte; i++) {
                            MMPH_HIF_RegSetB(SPI_TXFIFO_DATA + offset, 0xFF);
                        }
                        while (MMPH_HIF_RegGetB(SPI_RXFIFO_SPC + offset) != transferbyte);
                        for (i = 0; i < transferbyte; i++) {
                            *ptr++ = MMPH_HIF_RegGetB(SPI_RXFIFO_DATA + offset);
                        }
                        outbyte -= transferbyte;
                    }
                }
            }
        }
    }

    return  MMP_ERR_NONE;
}
#endif

/// @}
/// @end_ait_only


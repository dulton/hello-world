// ****************************************************************************
// CP15 Register 0
// 	Read: ID code | cache type
//	Write: Unpredictable

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_ReadIDCode
//* \brief Read ID code register
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_ARM_ReadIDCode()
{
	register unsigned int id;
 	__asm("MRC p15, 0, id, c0, c0, 0");
 	return id;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_ReadCacheType
//* \brief Read cache type
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_ARM_ReadCacheType()
{
	register unsigned int type;
 	__asm("MRC p15, 0, type, c0, c0, 1");
 	return type;
}

// ****************************************************************************
// CP15 Register 1
// 	Read: Control
//	Write: Control

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_ReadControl
//* \brief Read Control register
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_ARM_ReadControl()
{
	register unsigned int ctl;
 	__asm("MRC p15, 0, ctl, c1, c0, 0");
 	return ctl;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_WriteControl
//* \brief Write Control register
//*----------------------------------------------------------------------------
__inline void AT91F_ARM_WriteControl(unsigned int ctl)
{
 	__asm("MCR p15, 0, ctl, c1, c0, 0");
}

// ****************************************************************************
// CP15 Register 2
// 	Read: Translation table Base
//	Write: Translation table Base

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_ReadTTB
//* \brief Read Translation table base register
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_ARM_ReadTTB()
{
	register unsigned int ttb;
 	__asm("MRC p15, 0, ttb, c2, c0, 0");
 	return ttb;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_WriteTTB
//* \brief Write Translation table base  register
//*----------------------------------------------------------------------------
__inline void AT91F_ARM_WriteTTB(unsigned int ttb)
{
 	__asm("MCR p15, 0, (ttb & 0xFFFFC000), c2, c0, 0");
}

// ****************************************************************************
// CP15 Register 3
// 	Read: Read domain access control
//	Write: Write domain access control

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_ReadDomain
//* \brief Read domain access control
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_ARM_ReadDomain()
{
	register unsigned int domain;
 	__asm("MRC p15, 0, domain, c3, c0, 0");
 	return domain;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_WriteDomain
//* \brief Write domain access control
//*----------------------------------------------------------------------------
__inline void AT91F_ARM_WriteDomain(unsigned int domain)
{
 	__asm("MCR p15, 0, domain, c3, c0, 0");
}

// ****************************************************************************
// CP15 Register 5
// 	Read: Read Fault Status
//	Write: Write Fault Status

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_ReadDataFSR
//* \brief Read data FSR value
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_ARM_ReadDataFSR()
{
	register unsigned int dataFSR;
 	__asm("MRC p15, 0, dataFSR, c5, c0, 0");
 	return dataFSR;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_WriteDataFSR
//* \brief Write data FSR value
//*----------------------------------------------------------------------------
__inline void AT91F_ARM_WriteDataFSR(unsigned int dataFSR)
{
 	__asm("MCR p15, 0, dataFSR, c5, c0, 0");
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_ReadPrefetchFSR
//* \brief Read prefetch FSR value
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_ARM_ReadPrefetchFSR()
{
	register unsigned int prefetchFSR;
 	__asm("MRC p15, 0, prefetchFSR, c5, c0, 1");
 	return prefetchFSR;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_WritePrefetchFSR
//* \brief Write prefetch FSR value
//*----------------------------------------------------------------------------
__inline void AT91F_ARM_WritePrefetchFSR(unsigned int prefetchFSR)
{
 	__asm("MCR p15, 0, prefetchFSR, c5, c0, 1");
}

// ****************************************************************************
// CP15 Register 6
// 	Read: Read Fault Address
//	Write: Write Fault Address

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_ReadFAR
//* \brief Read FAR data
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_ARM_ReadFAR()
{
	register unsigned int dataFAR;
 	__asm("MRC p15, 0, dataFAR, c6, c0, 0");
 	return dataFAR;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_WriteFAR
//* \brief Write FAR data
//*----------------------------------------------------------------------------
__inline void AT91F_ARM_WriteFAR(unsigned int dataFAR)
{
 	__asm("MCR p15, 0, dataFAR, c6, c0, 0");
}

// ****************************************************************************
// CP15 Register 7
// 	Read: Unpredictable
//	Write: Cache operations

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_InvalidateIDCache
//* \brief Invalidate ICache and DCache
//*----------------------------------------------------------------------------
__inline void AT91F_ARM_InvalidateIDCache()
{
	register unsigned int sbz = 0;
 	__asm("MCR p15, 0, sbz, c7, c7, 0");
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_InvalidateICache
//* \brief Invalidate ICache
//*----------------------------------------------------------------------------
__inline void AT91F_ARM_InvalidateICache()
{
	register unsigned int sbz = 0;
 	__asm("MCR p15, 0, sbz, c7, c5, 0");
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_InvalidateICacheMVA
//* \brief Invalidate ICache single entry (using MVA)
//*----------------------------------------------------------------------------
__inline void AT91F_ARM_InvalidateICacheMVA(unsigned int mva)
{
 	__asm("MCR p15, 0, (mva & 0xFFFFFFE0), c7, c5, 1");
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_PrefetchICacheLine
//* \brief Prefetch ICache line (using MVA)
//*----------------------------------------------------------------------------
__inline void AT91F_ARM_PrefetchICacheLine(unsigned int mva)
{
 	__asm("MCR p15, 0, (mva & 0xFFFFFFE0), c7, c13, 1");
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_InvalidateDCache
//* \brief Invalidate DCache
//*----------------------------------------------------------------------------
__inline void AT91F_ARM_InvalidateDCache()
{
	register unsigned int sbz = 0;
 	__asm("MCR p15, 0, sbz, c7, c6, 0");
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_InvalidateDCacheMVA
//* \brief Invalidate DCache single entry (using MVA)
//*----------------------------------------------------------------------------
__inline void AT91F_ARM_InvalidateDCacheMVA(unsigned int mva)
{
   	__asm("MCR p15, 0, (mva & 0xFFFFFFE0), c7, c6, 1");
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_CleanDCacheMVA
//* \brief Clean DCache single entry (using MVA)
//*----------------------------------------------------------------------------
__inline void AT91F_ARM_CleanDCacheMVA(unsigned int mva)
{
 	__asm("MCR p15, 0, (mva & 0xFFFFFFE0), c7, c10, 1");
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_CleanInvalidateDCacheMVA
//* \brief Clean and Invalidate DCache single entry (using MVA)
//*----------------------------------------------------------------------------
__inline void AT91F_ARM_CleanInvalidateDCacheMVA(unsigned int mva)
{
 	__asm("MCR p15, 0, (mva & 0xFFFFFFE0), c7, c14, 1");
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_InvalidateDCacheIDX
//* \brief Invalidate DCache single entry (using index)
//*----------------------------------------------------------------------------
__inline void AT91F_ARM_InvalidateDCacheIDX(unsigned int index)
{
    /* ARM926EJ-S */
    // 16K cache
 	__asm("MCR p15, 0, (index & 0xC0000FE0), c7, c6, 2");
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_CleanDCacheIDX
//* \brief Clean DCache single entry (using index)
//*----------------------------------------------------------------------------
__inline void AT91F_ARM_CleanDCacheIDX(unsigned int index)
{
    /* ARM926EJ-S */
    // 16K cache
 	__asm("MCR p15, 0, (index & 0xC0000FE0), c7, c10, 2");
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_CleanInvalidateDCacheIDX
//* \brief Clean and Invalidate DCache single entry (using index)
//*----------------------------------------------------------------------------
__inline void AT91F_ARM_CleanInvalidateDCacheIDX(unsigned int index)
{
    /* ARM926EJ-S */
    // 16K cache
 	__asm("MCR p15, 0, (index & 0xC0000FE0), c7, c14, 2");
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_DrainWriteBuffer
//* \brief Drain Write Buffer
//*----------------------------------------------------------------------------
__inline void AT91F_ARM_DrainWriteBuffer()
{
	register unsigned int sbz = 0;
 	__asm("MCR p15, 0, sbz, c7, c10, 4");
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_WaitForInterrupt
//* \brief Wait for interrupt
//*----------------------------------------------------------------------------
__inline void AT91F_ARM_WaitForInterrupt()
{
	register unsigned int sbz = 0;
	__asm("MCR p15, 0, sbz, c7, c0, 4");
}

// ****************************************************************************
// CP15 Register 8
// 	Read: Unpredictable
//	Write: TLB operations

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_InvalidateIDTLB
//* \brief Invalidate TLB(s)
//*----------------------------------------------------------------------------
__inline void AT91F_ARM_InvalidateIDTLB()
{
	register unsigned int sbz = 0;
 	__asm("MCR p15, 0, sbz, c8, c7, 0");
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_InvalidateITLB
//* \brief Invalidate I TLB
//*----------------------------------------------------------------------------
__inline void AT91F_ARM_InvalidateITLB()
{
	register unsigned int sbz = 0;
 	__asm("MCR p15, 0, sbz, c8, c5, 0");
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_InvalidateITLBMVA
//* \brief Invalidate I TLB single entry (using MVA)
//*----------------------------------------------------------------------------
__inline void AT91F_ARM_InvalidateITLBMVA(unsigned int mva)
{
 	__asm("MCR p15, 0, (mva & 0xFFFFFE00), c8, c5, 1");
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_InvalidateDTLB
//* \brief Invalidate D TLB
//*----------------------------------------------------------------------------
__inline void AT91F_ARM_InvalidateDTLB()
{
	register unsigned int sbz = 0;
 	__asm("MCR p15, 0, sbz, c8, c6, 0");
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_InvalidateDTLBMVA
//* \brief Invalidate D TLB single entry (using MVA)
//*----------------------------------------------------------------------------
__inline void AT91F_ARM_InvalidateDTLBMVA(unsigned int mva)
{
 	__asm("MCR p15, 0, (mva & 0xFFFFFE00), c8, c6, 1");
}

// ****************************************************************************
// CP15 Register 9
// 	Read: Cache lockdown
//	Write: Cache lockdown

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_ReadDCacheLockdown
//* \brief Read D Cache lockdown
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_ARM_ReadDCacheLockdown()
{
	register unsigned int index;
 	__asm("MRC p15, 0, index, c9, c0, 0");
    return index;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_WriteDCacheLockdown
//* \brief Write D Cache lockdown
//*----------------------------------------------------------------------------
__inline void AT91F_ARM_WriteDCacheLockdown(
	unsigned int index)
{
	__asm("MCR p15, 0, index, c9, c0, 0");
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_ReadICacheLockdown
//* \brief Read I Cache lockdown
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_ARM_ReadICacheLockdown()
{
	register unsigned int index;
	__asm("MRC p15, 0, index, c9, c0, 1");
	return index;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_WriteICacheLockdown
//* \brief Write I Cache lockdown
//*----------------------------------------------------------------------------
__inline void AT91F_ARM_WriteICacheLockdown(
	unsigned int index)
{
	__asm("MCR p15, 0, index, c9, c0, 1");
}

// ****************************************************************************
// CP15 Register 10
// 	Read: TLB lockdown
//	Write: TLB lockdown

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_ReadDTLBLockdown
//* \brief Read D TLB lockdown
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_ARM_ReadDTLBLockdown()
{
	register unsigned int lockdown;
 	__asm("MRC p15, 0, lockdown, c10, c0, 0");
    return lockdown;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_WriteDTLBLockdown
//* \brief Write D TLB lockdown
//*----------------------------------------------------------------------------
__inline void AT91F_ARM_WriteDTLBLockdown(
	unsigned int lockdown)
{
	__asm("MCR p15, 0, lockdown, c10, c0, 0");
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_ReadITLBLockdown
//* \brief Read I TLB lockdown
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_ARM_ReadITLBLockdown()
{
	register unsigned int lockdown;
	__asm("MRC p15, 0, lockdown, c10, c0, 1");
	return lockdown;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_WriteITLBLockdown
//* \brief Write I TLB lockdown
//*----------------------------------------------------------------------------
__inline void AT91F_ARM_WriteITLBLockdown(
	unsigned int lockdown)
{
	__asm("MCR p15, 0, lockdown, c10, c0, 1");
}

// ****************************************************************************
// CP15 Register 13
// 	Read: Read FCSE PID
//	Write: Write FCSE PID

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_ReadFCSEPID
//* \brief Read FCSE PID
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_ARM_ReadFCSEPID()
{
	register unsigned int pid;
 	__asm("MRC p15, 0, pid, c13, c0, 0");
 	return pid;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_WriteFCSEPID
//* \brief Write FCSE PID
//*----------------------------------------------------------------------------
__inline void AT91F_ARM_WriteFCSEPID(unsigned int pid)
{
 	__asm("MCR p15, 0, (pid & 0xFF000000), c13, c0, 0");
}

// ****************************************************************************
// CP15 Register 15
// 	Read: Read Test and Debug Registers
//	Write: Write Test and Debug Registers

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_WriteDbgOverride
//* \brief Write Debug Override Register
//*----------------------------------------------------------------------------
__inline void AT91F_ARM_WriteDbgOverride(unsigned int pid)
{
 	__asm("MCR p15, 0, pid, c15, c0, 0");
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_ReadDbgOverride
//* \brief Read Debug Override Register
//*----------------------------------------------------------------------------
__inline unsigned int AT91F_ARM_ReadDbgOverride()
{
	register unsigned int pid;
 	__asm("MRC p15, 0, pid, c15, c0, 0");
 	return pid;
}

//*----------------------------------------------------------------------------
//* \fn    AT91F_ARM_DisableBlkLevelClkGating
//* \brief Disable Block Level Clock Gating
//*----------------------------------------------------------------------------
__inline void AT91F_ARM_DisableBlkLevelClkGating()
{
	register unsigned int pid = 0x8000;
 	__asm("MCR p15, 0, pid, c15, c0, 0");
}


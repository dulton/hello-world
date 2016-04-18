//==============================================================================
//
//  File        : aitu_ringbuf.c
//  Description : Generic Ring buffer Utility
//  Author      : Alterman
//  Revision    : 1.0
//
//==============================================================================

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================

#include "aitu_ringbuf.h"
#include "os_wrap.h"

//==============================================================================
//
//                              LOCAL FUNCTIONS
//
//==============================================================================
//------------------------------------------------------------------------------
//  Function    : _Advance_ReadPtr
//------------------------------------------------------------------------------
/** 
    @brief  Advance the read pointer of ring buffer. The wrap count may
            increase 1 if wrapping.
    @param[in] ring : ring buffer handler
    @param[in] size : advance size
    @return none
*/
static void _Advance_ReadPtr(AUTL_RINGBUF *ring, MMP_ULONG size)
{
    OS_CRITICAL_INIT();

    OS_ENTER_CRITICAL();
    ring->ptr.rd += size;
    if (ring->ptr.rd >= ring->size) {
        ring->ptr.rd -= ring->size;
        ring->ptr.rd_wrap++;
    }
    OS_EXIT_CRITICAL();
}

//------------------------------------------------------------------------------
//  Function    : _Advance_WritePtr
//------------------------------------------------------------------------------
/** 
    @brief  Advance the write pointer of ring buffer. The wrap count may
            increase 1 if wrapping.
    @param[in] ring : ring buffer handler
    @param[in] size : advance size
    @return none
*/
static void _Advance_WritePtr(AUTL_RINGBUF *ring, MMP_ULONG size)
{
    OS_CRITICAL_INIT();

    OS_ENTER_CRITICAL();
    ring->ptr.wr += size;
    if (ring->ptr.wr >= ring->size) {
        ring->ptr.wr -= ring->size;
        ring->ptr.wr_wrap++;
    }
    OS_EXIT_CRITICAL();
}

//------------------------------------------------------------------------------
//  Function    : _Copy_CurrentPtr
//------------------------------------------------------------------------------
/** 
    @brief  Get all of current pointers of ring buffer.
    @param[in] ring : ring buffer handler
    @param[in] ptr  : ring buffer pointers
    @return none
*/
static void _Copy_CurrentPtr(AUTL_RINGBUF *ring, AUTL_RINGBUF_PTR *ptr)
{
    OS_CRITICAL_INIT();

    /* Copy all of pointers */
    OS_ENTER_CRITICAL();
    *ptr = ring->ptr;
    OS_EXIT_CRITICAL();
}

//==============================================================================
//
//                              PUBLIC FUNCTIONS
//
//==============================================================================
//------------------------------------------------------------------------------
//  Function    : AUTL_RingBuf_Init
//------------------------------------------------------------------------------
/** 
    @brief  Initialize the ring buffer with the specified buffer address, buffer
            size, and user data. Also, reset all read/write pointers to 0.
    @param[in] ring : ring buffer handler
    @param[in] buf  : buffer address
    @param[in] size : buffer size
    @return 0 for success, others for failed.
*/
int AUTL_RingBuf_Init(AUTL_RINGBUF *ring, void *buf, MMP_ULONG size)
{
    if (!ring)
        return RINGBUF_ERR_NULL_HANDLE;

    ring->buf   = buf;
    ring->size  = size;

    /* Reset ring buffer pointers */
    ring->ptr.rd        = 0;
    ring->ptr.wr        = 0;
    ring->ptr.rd_wrap   = 0;
    ring->ptr.wr_wrap   = 0;

    return RINGBUF_SUCCESS;
}

//------------------------------------------------------------------------------
//  Function    : AUTL_RingBuf_Fork
//------------------------------------------------------------------------------
/** 
    @brief  Fork a new ring buffer handler from exist one.
    @param[in] src  : one existed ring buffer
    @param[in] dst  : ring buffer to be forked
    @return 0 for success, others for failed.
*/
int AUTL_RingBuf_Fork(AUTL_RINGBUF *src, AUTL_RINGBUF *dst)
{
    if (!src || !dst)
        return RINGBUF_ERR_NULL_HANDLE;

    dst->buf   = src->buf;
    dst->size  = src->size;

    /* Copy current ring buffer pointers */
    _Copy_CurrentPtr(src, &(dst->ptr));

    return RINGBUF_SUCCESS;
}

//------------------------------------------------------------------------------
//  Function    : AUTL_RingBuf_StrictCommitRead
//------------------------------------------------------------------------------
/** 
    @brief  Advance the read pointer of ring buffer. The wrap count may
            increase 1 if wrapping. Before advancing, this function will check
            if size is reasonable.
    @param[in] ring : ring buffer handler
    @param[in] size : advance size
    @return 0 for success, others for failed.
*/
int AUTL_RingBuf_StrictCommitRead(AUTL_RINGBUF *ring, MMP_ULONG size)
{
    MMP_ULONG data_size;

    if (!ring)
        return RINGBUF_ERR_NULL_HANDLE;

    AUTL_RingBuf_DataAvailable(ring, &data_size);
    if (size > data_size)
        return RINGBUF_ERR_UNDERFLOW;

    _Advance_ReadPtr(ring, size);

    return RINGBUF_SUCCESS;
}

//------------------------------------------------------------------------------
//  Function    : AUTL_RingBuf_StrictCommitWrite
//------------------------------------------------------------------------------
/** 
    @brief  Advance the write pointer of ring buffer. The wrap count may
            increase 1 if wrapping. Before advancing, this function will check
            if size is reasonable.
    @param[in] ring : ring buffer handler
    @param[in] size : advance size
    @return 0 for success, others for failed.
*/
int AUTL_RingBuf_StrictCommitWrite(AUTL_RINGBUF *ring, MMP_ULONG size)
{
    MMP_ULONG space;

    if (!ring)
        return RINGBUF_ERR_NULL_HANDLE;

    AUTL_RingBuf_SpaceAvailable(ring, &space);
    if (size > space)
        return RINGBUF_ERR_OVERFLOW;

    _Advance_WritePtr(ring, size);

    return RINGBUF_SUCCESS;
}

//------------------------------------------------------------------------------
//  Function    : AUTL_RingBuf_CommitRead
//------------------------------------------------------------------------------
/** 
    @brief  Advance the read pointer of ring buffer. The wrap count may
            increase 1 if wrapping. If you have already make sure the size is
            reasonable, you can use this function, instread of the strict one.
    @param[in] ring : ring buffer handler
    @param[in] size : advance size
    @return 0 for success, others for failed.
*/
int AUTL_RingBuf_CommitRead(AUTL_RINGBUF *ring, MMP_ULONG size)
{
    if (!ring)
        return RINGBUF_ERR_NULL_HANDLE;

    _Advance_ReadPtr(ring, size);

    return RINGBUF_SUCCESS;
}

//------------------------------------------------------------------------------
//  Function    : AUTL_RingBuf_CommitWrite
//------------------------------------------------------------------------------
/** 
    @brief  Advance the write pointer of ring buffer. The wrap count may
            increase 1 if wrapping. If you have already make sure the size is
            reasonable, you can use this function, instread of the strict one.
    @param[in] ring : ring buffer handler
    @param[in] size : advance size
    @return 0 for success, others for failed.
*/
int AUTL_RingBuf_CommitWrite(AUTL_RINGBUF *ring, MMP_ULONG size)
{
    if (!ring)
        return RINGBUF_ERR_NULL_HANDLE;

    _Advance_WritePtr(ring, size);

    return RINGBUF_SUCCESS;
}

//------------------------------------------------------------------------------
//  Function    : AUTL_RingBuf_SpaceAvailable
//------------------------------------------------------------------------------
/** 
    @brief  Get how many free space available in the ring buffer.
    @param[in] ring   : ring buffer handler
    @param[out] space : return the available(unwritten) space
    @return 0 for success, others for failed.
*/
int AUTL_RingBuf_SpaceAvailable(AUTL_RINGBUF *ring, MMP_ULONG *space)
{
    AUTL_RINGBUF_PTR ptr;

    *space = 0;

    if (!ring)
        return RINGBUF_ERR_NULL_HANDLE;

    /* Copy current ring buffer pointers */
    _Copy_CurrentPtr(ring, &ptr);

    if (ptr.rd_wrap == ptr.wr_wrap) {
        if (ptr.wr >= ptr.rd)
            *space = ring->size - ptr.wr + ptr.rd;
        else
            return RINGBUF_ERR_UNDERFLOW;
    }
    else if (ptr.rd_wrap < ptr.wr_wrap) {
        if (ptr.rd >= ptr.wr)
            *space = ptr.rd - ptr.wr;
        else
            return RINGBUF_ERR_OVERFLOW;
    }
    else {
        return RINGBUF_ERR_UNDERFLOW;
    }

    return RINGBUF_SUCCESS;
}

//------------------------------------------------------------------------------
//  Function    : AUTL_RingBuf_DataAvailable
//------------------------------------------------------------------------------
/** 
    @brief  Get how many data available in the ring buffer.
    @param[in] ring       : ring buffer handler
    @param[out] data_size : return the available(written) data size
    @return 0 for success, others for failed.
*/
int AUTL_RingBuf_DataAvailable(AUTL_RINGBUF *ring, MMP_ULONG *data_size)
{
    AUTL_RINGBUF_PTR ptr;

    *data_size = 0;

    if (!ring)
        return RINGBUF_ERR_NULL_HANDLE;

    /* Copy current ring buffer pointers */
    _Copy_CurrentPtr(ring, &ptr);

    if (ptr.rd_wrap == ptr.wr_wrap) {
        if (ptr.wr >= ptr.rd)
            *data_size = ptr.wr - ptr.rd;
        else
            return RINGBUF_ERR_UNDERFLOW;
    }
    else if (ptr.rd_wrap < ptr.wr_wrap) {
        if (ptr.rd >= ptr.wr)
            *data_size = ring->size - ptr.rd + ptr.wr;
        else
            return RINGBUF_ERR_OVERFLOW;
    }
    else {
        return RINGBUF_ERR_UNDERFLOW;
    }

    return RINGBUF_SUCCESS;
}

//------------------------------------------------------------------------------
//  Function    : AUTL_RingBuf_Empty
//------------------------------------------------------------------------------
/** 
    @brief  Check if ring buffer empty.
    @param[in] ring : ring buffer handler
    @return MMP_TRUE if ring buffer empty, MMP_FALSE for non-empty.
*/
MMP_BOOL AUTL_RingBuf_Empty(AUTL_RINGBUF *ring)
{
    AUTL_RINGBUF_PTR ptr;

    if (!ring)
        return MMP_FALSE;

    /* Copy current ring buffer pointers */
    _Copy_CurrentPtr(ring, &ptr);

    if ((ptr.rd == ptr.wr) && (ptr.rd_wrap == ptr.wr_wrap))
        return MMP_TRUE;

    return MMP_FALSE;
}

//------------------------------------------------------------------------------
//  Function    : AUTL_RingBuf_Full
//------------------------------------------------------------------------------
/** 
    @brief  Check if ring buffer full.
    @param[in] ring : ring buffer handler
    @return MMP_TRUE if ring buffer full, MMP_FALSE if there's still free space.
*/
MMP_BOOL AUTL_RingBuf_Full(AUTL_RINGBUF *ring)
{
    AUTL_RINGBUF_PTR ptr;

    if (!ring)
        return MMP_FALSE;

    /* Copy current ring buffer pointers */
    _Copy_CurrentPtr(ring, &ptr);

    if ((ptr.rd == ptr.wr) && ((ptr.rd_wrap + 1) == ptr.wr_wrap))
        return MMP_TRUE;

    return MMP_FALSE;
}


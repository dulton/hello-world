// Sample code for memory pool
//
//////////////////////////////////////////////////////////////////////
#if 0
#include "includes_fw.h"
#include "UartShell.h"
#include "MemPool.h"

#define POOL_BUFFER_SIZE		(4*1024*1024)    // 4MB

#define SMALL_POOL_BUFFER_SIZE		(800*1024)

#define POOL_SIZE_THRESHOLD     40*1024


										 
MMP_UBYTE byPoolBuffer[POOL_BUFFER_SIZE];

//-------------------------------------------------------------------------------------------------------------
MEM_POOL_HANDLE hPoolGlobal = NULL;
MEM_POOL_HANDLE hPoolSmall  = NULL;


/*******************************************************************************
 *
 *   Pool
 *
*******************************************************************************/
BOOL
MemPoolInit()
{
	VOID* pbySmallPoolBuffer;

	// Create large pool	
	hPoolGlobal              = MemPoolCreate( byPoolBuffer, POOL_BUFFER_SIZE, "Large" );
	
	if( hPoolGlobal == NULL )
	{
		return FALSE;
	}

	// Allocate memory for small pool
	pbySmallPoolBuffer = MemPoolAlloc( hPoolGlobal, SMALL_POOL_BUFFER_SIZE , MEM_ALLOC_DEFAULT );

	if( pbySmallPoolBuffer == NULL )
	{
		return FALSE;
	}

	// Create small pool
	hPoolSmall = MemPoolCreate( pbySmallPoolBuffer, SMALL_POOL_BUFFER_SIZE, "Small" );

	if( hPoolSmall == NULL )
	{
		return FALSE;
	}

	
}

BOOL
MemPoolRelease()
{
	// Destroy small pool
	if( hPoolSmall != NULL )
	{
		MemPoolDestroy(hPoolSmall);
	}

	// Destroy large pool	
	if( hPoolGlobal != NULL )
	{
		MemPoolDestroy(hPoolGlobal);
	}

	return TRUE;
}

MEM_POOL_HANDLE
MemPoolGetPoolBySize( ULONG ulSize )
{

	MEM_POOL_HANDLE hPool;
	
	if( ulSize > POOL_SIZE_THRESHOLD )
	{
		hPool = hPoolGlobal;
	}
	else
	{
		hPool = hPoolSmall;
	}	
	
	return hPool;
}

void* 
MemPoolAllocEx( MEM_POOL_HANDLE hPool, ULONG ulSize, ULONG ulFlag, CHAR* szFilename, ULONG ulLineNumber )
{


	if( hPool == NULL )
	{
		hPool = MemPoolGetPoolBySize(ulSize);
	}
	
	return MemPoolAllocImp( hPool, ulSize , ulFlag, szFilename, ulLineNumber );		
	
}

void* 
MemPoolCallocEx( MEM_POOL_HANDLE hPool, ULONG ulNumber, ULONG ulSize, ULONG ulFlag, CHAR* szFilename, ULONG ulLineNumber )
{

	if( hPool == NULL )
	{
		hPool = MemPoolGetPoolBySize(ulSize);
	}

	return MemPoolCallocImp( hPool, ulNumber, ulSize , ulFlag, szFilename, ulLineNumber );		
	
}

void* 
MemPoolReallocEx( MEM_POOL_HANDLE hPool, VOID* pAllocatedBuffer, ULONG ulSize, CHAR* szFilename, ULONG ulLineNumber )
{

	if( hPool == NULL )
	{
		hPool = MemPoolGetPoolBySize(ulSize);
	}
	
	return MemPoolReallocImp( hPool, pAllocatedBuffer , ulSize, szFilename, ulLineNumber );		
	
}


#ifdef DISABLE_MEM_ALLOC_LOG

#define PoolAlloc(POOL,SIZE,FLAG)           MemPoolAllocEx(POOL,SIZE,FLAG,0,0)
#define PoolCalloc(POOL,NUM,SIZE,FLAG)      MemPoolCallocEx(POOL,NUM,SIZE,FLAG,0,0)
#define PoolRealloc(POOL,BUFFER,SIZE)       MemPoolReallocEx(POOL,BUFFER,SIZE,0,0)


#else

#define PoolAlloc(POOL,SIZE,FLAG)           MemPoolAllocEx(POOL,SIZE,FLAG,__FILE__,__LINE__)
#define PoolCalloc(POOL,NUM,SIZE,FLAG)      MemPoolCallocEx(POOL,NUM,SIZE,FLAG,__FILE__,__LINE__)
#define PoolRealloc(POOL,BUFFER,SIZE)       MemPoolReallocEx(POOL,BUFFER,SIZE,__FILE__,__LINE__)
#define PoolFree(POOL, BUFFER )             MemPoolFreeEx(POOL,BUFFER)

#endif

//-------------------------------------------------------------------------------------------------------------

void
UartCmdPoolCreate( char* szParam )
{
	VOID* pbySmallPoolBuffer;
	
	hPoolGlobal              = MemPoolCreate( byPoolBuffer, POOL_BUFFER_SIZE, "Large" );

	pbySmallPoolBuffer = MemPoolAlloc( hPoolGlobal, SMALL_POOL_BUFFER_SIZE , MEM_ALLOC_DEFAULT );

	hPoolSmall         = MemPoolCreate( pbySmallPoolBuffer, SMALL_POOL_BUFFER_SIZE, "Small" );

}

void
UartCmdPoolClose( char* szParam )
{
	MemPoolDestroy(hPoolGlobal);
	MemPoolDestroy(hPoolSmall);	
}

void
UartCmdPoolAllocA( char* szParam )
{

	
    MMP_ULONG ulSize = 512;
	
	sscanfl( szParam, "%d", &ulSize );	

	if( ulSize > POOL_SIZE_THRESHOLD )
	{
		MemPoolAlloc( hPoolGlobal, ulSize , MEM_ALLOC_DEFAULT );		
	}
	else
	{
		MemPoolAlloc( hPoolSmall, ulSize , MEM_ALLOC_DEFAULT );			
	}
	
}


void
UartCmdPoolAllocB( char* szParam )
{

	
    MMP_ULONG ulSize = 512;
	MMP_ULONG ulFlag = 0;
	MMP_ULONG ulAlign = 0;
	MMP_ULONG ulTopOrBottom = 0;	

	sscanfl( szParam, "%d %d %d", &ulSize, &ulAlign, &ulTopOrBottom );

	switch( ulAlign )
	{
	case 8:
	    ulFlag = MEM_ALLOC_ALIGNMENT_8B;
		break;
	case 16:
	    ulFlag = MEM_ALLOC_ALIGNMENT_16B;	
		break;
	case 32:
	    ulFlag = MEM_ALLOC_ALIGNMENT_32B;	
		break;
	case 64:
	    ulFlag = MEM_ALLOC_ALIGNMENT_64B;	
		break;

	}

	printc( "ulTopOrBottom %d \n", ulTopOrBottom );
	if( ulTopOrBottom == 1)
	{
		ulFlag |= MEM_ALLOC_FROM_BOTTOM;
	}


	if( ulSize > POOL_SIZE_THRESHOLD )
	{
		MemPoolAlloc( hPoolGlobal, ulSize , MEM_ALLOC_DEFAULT|ulFlag );		
	}
	else
	{
		MemPoolAlloc( hPoolSmall, ulSize , MEM_ALLOC_DEFAULT|ulFlag );			
	}
	
}



void
UartCmdPoolAllocC( char* szParam )
{

    MMP_ULONG ulSize = 512;
	
	sscanfl( szParam, "%d", &ulSize );	

	PoolAlloc( NULL, ulSize , MEM_ALLOC_DEFAULT );		
	
}


void
UartCmdPoolAllocFromBottom( char* szParam )
{

    MMP_ULONG ulSize = 512;
	
	sscanfl( szParam, "%d", &ulSize );	

	if( ulSize > POOL_SIZE_THRESHOLD )
	{
		MemPoolAlloc( hPoolGlobal, ulSize , MEM_ALLOC_FROM_BOTTOM );		
	}
	else
	{
		MemPoolAlloc( hPoolSmall, ulSize , MEM_ALLOC_FROM_BOTTOM );			
	}
	
}



void
UartCmdPoolFree( char* szParam )
{
    MMP_ULONG ulAddress;
    MMP_ULONG ulSize = 512;
	
	sscanfl( szParam, "%X ", &ulAddress );
	
	ulSize = MemPoolGetBufferSize((void*)ulAddress);

	if( ulSize > POOL_SIZE_THRESHOLD )
	{
		MemPoolFree( hPoolGlobal, (void*)ulAddress);		
	}
	else
	{
		MemPoolFree( hPoolSmall, (void*)ulAddress );			
	}

}

void
UartCmdPoolDump( char* szParam )
{
	MemPoolDumpInfo( hPoolGlobal, 0 );
	MemPoolDumpInfo( hPoolSmall, 0 );	
}

void
UartCmdPoolLeakDetectStart( char* szParam )
{
	MemPoolLeakDetectStart(hPoolGlobal);
	MemPoolLeakDetectStart(hPoolSmall);
		
	
}

void
UartCmdPoolLeakDetectStop( char* szParam )
{
	MemPoolLeakDetectStop(hPoolGlobal);
	MemPoolLeakDetectStop(hPoolSmall);	
}

void
UartCmdCallFunction( char* szParam )
{
    int iCount;
	int iVal[10];
	iCount = sscanfl( szParam, " %x %x %x %x %x %x %x %x", &iVal[0], &iVal[1], &iVal[2], &iVal[3], &iVal[4], &iVal[5], &iVal[6], &iVal[7] );


	printc( "iCount = %d\n", iCount );
	
}

/*******************************************************************************
 *
 *   UART COMMAND LIST SAMPLE
 *
*******************************************************************************/

UARTCOMMAND sUartCommandSample[] =
{
	{ "call" , "function arg1 arg2 ... "      , "call a function"  , UartCmdCallFunction  },
	{ "poolcreate" , "function arg1 arg2 ... "      , "call a function"  , UartCmdPoolCreate  },	
	{ "pooldel" , "function arg1 arg2 ... "      , "call a function"  , UartCmdPoolClose  },
	{ "allocA" , "function arg1 arg2 ... "      , "call a function"  , UartCmdPoolAllocA  },
	{ "allocB" , "function arg1 arg2 ... "      , "call a function"  , UartCmdPoolAllocB  },
	{ "allocC" , "function arg1 arg2 ... "      , "call a function"  , UartCmdPoolAllocC  },
	{ "allocbtm" , "function arg1 arg2 ... "      , "call a function"  , UartCmdPoolAllocFromBottom  },	
	{ "free" , "function arg1 arg2 ... "      , "call a function"  , UartCmdPoolFree  },
	{ "pdump" , "function arg1 arg2 ... "      , "call a function"  , UartCmdPoolDump  },
	{ "mlb" , "function arg1 arg2 ... "      , "call a function"  , UartCmdPoolLeakDetectStart  },
	{ "mle" , "function arg1 arg2 ... "      , "call a function"  , UartCmdPoolLeakDetectStop  },	
	{0,0,0,0}
};


void
RegisterUartCmdSample(void)
{
	UartRegisterUartCmdList(sUartCommandSample);
}
#else

void MemPoolInitDummy() {
}
#endif
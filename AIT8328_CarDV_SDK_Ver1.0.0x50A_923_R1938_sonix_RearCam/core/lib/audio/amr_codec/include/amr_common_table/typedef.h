
/*
********************************************************************************
*
*      GSM AMR-NB speech codec   R98   Version 7.6.0   December 12, 2001
*                                R99   Version 3.3.0                
*                                REL-4 Version 4.1.0                
*
********************************************************************************
*
*      File             : typedef.c
*      Purpose          : Basic types.
*
********************************************************************************
*/
#ifndef typedef_h
#define typedef_h

typedef signed char Word8;
typedef unsigned char UWord8;
typedef short Word16;
typedef unsigned short UWord16;
typedef int Word32;
typedef unsigned int UWord32;
typedef int Flag;

#define ARM
#define MMS_IO		// define MMS (RFC3267) format
#ifdef MMS_IO
#define READTABLE
#define TABLE_LEN (10500+6)	//women_795.cod
//#define TOTALFRAMES	500
#endif

#ifndef NULL
#define NULL	(void *)0
#endif

#endif

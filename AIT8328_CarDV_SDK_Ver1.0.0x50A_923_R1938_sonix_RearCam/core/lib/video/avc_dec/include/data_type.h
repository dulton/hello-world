#ifndef _DATA_TYPE_H_
#define _DATA_TYPE_H_

typedef volatile unsigned char  AIT_REG_B;
typedef volatile unsigned short AIT_REG_W;
typedef volatile unsigned int   AIT_REG_D;


#define int8_t				signed char
#define uint8_t				unsigned char
#define int16_t				short
#define uint16_t			unsigned short
#define int32_t				int
#define uint32_t			unsigned int
#define int64_t 			long long
#define uint64_t			unsigned long long
#define INT_MAX				2147483647 

#ifndef NULL

#define	NULL            0

#endif


typedef unsigned char       MMP_BOOL;
typedef char                MMP_BYTE;
typedef short               MMP_SHORT;
typedef int                 MMP_LONG;
typedef unsigned long long  MMP_ULONG64;
typedef unsigned char       MMP_UBYTE;
typedef unsigned short      MMP_USHORT;
typedef unsigned int        MMP_ULONG;
typedef unsigned long       MMP_U_LONG;

#endif
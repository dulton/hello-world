#ifndef _FLAC_TYPEDEF_H_
#define	_FLAC_TYPEDEF_H_

#define	FLAC_API

//-------------------------------------
    /* exact-width signed integer types */
typedef   signed          char flac_int8_t;
typedef   signed short     int flac_int16_t;
typedef   signed           int flac_int32_t;
typedef   signed       __int64 flac_int64_t;

    /* exact-width unsigned integer types */
typedef unsigned          char flac_uint8_t;
typedef unsigned short     int flac_uint16_t;
typedef unsigned           int flac_uint32_t;
typedef unsigned       __int64 flac_uint64_t;


typedef flac_int16_t FLAC__int16;
typedef flac_int32_t FLAC__int32;
typedef flac_int64_t FLAC__int64;

typedef flac_uint8_t  FLAC__uint8;
typedef flac_uint16_t FLAC__uint16;
typedef flac_uint32_t FLAC__uint32;
typedef flac_uint64_t FLAC__uint64;

typedef int FLAC__bool;

typedef unsigned char FLAC__byte;
//-------------------------------------

#endif  //end of #ifndef _FLAC_TYPEDEF_H_
//==============================================================================
//
//  File        : mmp_dsc_inc.h
//  Description : INCLUDE File for the DSC Driver Function
//  Author      : Penguin Torng
//  Revision    : 1.0
//
//==============================================================================

#ifndef _MMP_DSC_INC_H_
#define _MMP_DSC_INC_H_

//==============================================================================
//
//                              INCLUDE FILE
//
//==============================================================================

#include "includes_fw.h"

//==============================================================================
//
//                              MACRO DEFINE
//
//==============================================================================

#ifndef DSC_MAX_FILE_NAME_SIZE
#define DSC_MAX_FILE_NAME_SIZE			(256)
#endif

#define DSC_BUF_ALIGN_BASE				(256)

#define DSC_ROTATE_CAPTURE_BY_DMA		(0)
#define DSC_ROTATE_CAPTURE_BY_RAW		(1)
#define DSC_ROTATE_CAPTURE_FLOW			(DSC_ROTATE_CAPTURE_BY_DMA)

#define DSC_MJPEG_FPS_CTRL_EN			(0)

#define DSC_SUPPORT_BASELINE_MP_FILE 	(0) // Support Baseline Multi-Picture Format, Only for SportCam 

#define DSC_MAX_EXIF_NODE_NUM			(3)

#define DSC_QTABLE_ARRAY_SIZE			(64)

/**
@bref	the header structure of JPEG

0xFF:	Maker prefix

0xFFD8: SOI, Start of Image, Start of compressed data
0xFFDB: DQT, Define Quantization Table, Quantization table definition
0xFFC4: DHT, Define Huffman Table, Huffman table definition
0xFFDD: DRI, Define Restart Interoperability, Restart Interoperability definition
0xFFC0: SOF, Start of Frame, Frame header
0xFFCC: DAC
0xFFDA: SOS, Start of Scan, Scan header
0xFFDC: DNL
0xFFDE: DHP
0xFFDF: EXP
0xFFD9: EOI, End of Image, End of compressed data
0xFFFE: COM, comment
0xFF01: TEM, 

RES 		=> 0xFF02~0xFFBF
SOF range 	=> 0xFFC0~0xFFC3, 0xFFC5~0xFFC7, 0xFFCD~0xFFCF
RST0~RST7 	=> 0xFFD0~0xFFD7
JPG0~JPG13 	=> 0xFFF0~0xFFFD
*/

#define JPG_EXIF_PREFIX				0xFF

#define JPG_EXIF_SOF 	        	0xFFC0
#define JPG_EXIF_SOF1 	        	0xFFC1
#define JPG_EXIF_SOF2 	        	0xFFC2
#define JPG_EXIF_DHT 	        	0xFFC4
#define JPG_EXIF_DAC 	        	0xFFCC
#define JPG_EXIF_JPG				0xFFC8
#define JPG_EXIF_RST 	        	0xFFD0
#define JPG_EXIF_SOI		 		0xFFD8
#define JPG_EXIF_EOI 	        	0xFFD9
#define JPG_EXIF_SOS 	        	0xFFDA
#define JPG_EXIF_DQT 	        	0xFFDB
#define JPG_EXIF_DNL	        	0xFFDC
#define JPG_EXIF_DRI 	        	0xFFDD
#define JPG_EXIF_DHP 	        	0xFFDE
#define JPG_EXIF_EXP 	        	0xFFDF
#define JPG_EXIF_APP0        		0xFFE0
#define JPG_EXIF_APP1	    		0xFFE1
#define JPG_EXIF_APP2	    		0xFFE2
#define JPG_EXIF_COM				0xFFFE

/* Exif Tag ID */

// IFD0 for Primary Jpeg
#define IFD0_IMAGE_WIDTH 			0x0100
#define IFD0_IMAGE_HEIGHT 			0x0101
#define IFD0_MAKE 					0x010F
#define	IFD0_MODEL					0x0110
#define	IFD0_ORIENTATION			0x0112
#define	IFD0_X_RESOLUTION			0x011A
#define	IFD0_Y_RESOLUTION			0x011B
#define	IFD0_RESOLUTION_UNIT		0x0128
#define	IFD0_SOFTWARE				0x0131
#define	IFD0_DATE_TIME				0x0132
#define	IFD0_ARTIST					0x013B
#define	IFD0_YCBCR_POSITION			0x0213
#define	IFD0_COPYRIGHT				0x8298
#define IFD0_SEMINFO				0x8546
#define IFD0_EXIF_OFFSET       	 	0x8769
#define IFD0_GPS_OFFSET	 			0x8825
#define IFD0_SENSINGMETHOD			0x9217
#define IFD0_XPTITLE				0x9C9B
#define IFD0_MAKERNOTESAFETY		0xC635

// GPS IFD Tag
#define GPS_VERSION		 			0x0000
#define GPS_LATITUDE_REF 			0x0001
#define GPS_LATITUDE	 			0x0002
#define GPS_LONGITUDE_REF			0x0003
#define GPS_LONGITUDE				0x0004
#define GPS_ALTITUDE_REF			0x0005
#define GPS_ALTITUDE				0x0006
#define GPS_TIME_STAMP				0x0007
#define GPS_SPEED_REF				0x000C
#define GPS_SPEED					0x000D

// Exif IFD Tag
#define EXIFIFD_EXPOSURETIME		0x829A
#define EXIFIFD_EXPO_PROGRAM		0x8822
#define EXIFIFD_GPSINFO				0x8825
#define EXIFIFD_ISOSPEEDRATINGS		0x8827
#define EXIFIFD_VERSION				0x9000
#define EXIFIFD_DATETIMEORIGINAL   	0x9003
#define EXIFIFD_DATETIMECREATE    	0x9004
#define EXIFIFD_COMPONENTS			0x9101
#define EXIFIFD_SHUTTER_SPEED		0x9201
#define EXIFIFD_APERTURE			0x9202
#define EXIFIFD_BRIGHTNESS	 		0x9203
#define EXIFIFD_EXPOSUREBIAS		0x9204
#define EXIFIFD_MAX_APERTURE        0x9205
#define EXIFIFD_METERINGMODE		0x9207
#define EXIFIFD_LIGHTSOURCE			0x9208
#define EXIFIFD_FLASH				0x9209
#define EXIFIFD_FOCAL_LENGTH    	0x920A
#define EXIFIFD_COLORSPACE			0xA001
#define EXIFIFD_PIXL_X_DIMENSION	0xA002
#define EXIFIFD_PIXL_Y_DIMENSION	0xA003
#define EXIFIFD_INTEROP_OFFSET		0xA005
#define EXIFIFD_SCENETYPE			0xA301
#define EXIFIFD_WHITEBLANCE			0xA403
#define EXIFIFD_SCENE_CAP_TYPE		0xA406
#define EXIFIFD_GAINCONTROL			0xA407
#define EXIFIFD_CONTRAST			0xA408
#define EXIFIFD_SATURATION			0xA409
#define EXIFIFD_SHARPNESS			0xA40A

// Interoperability IFD Tag
#define INTEROP_INDEX				0x0001
#define INTEROP_VERSION				0x0002
#define INTEROP_RELAIMG_FILEFMT		0x1000
#define INTEROP_RELAIMG_WIDTH		0x1001
#define INTEROP_RELAIMG_HEIGHT		0x1002

// IFD1 for Thumbnail	
#define IFD1_IMAGE_WIDTH 			0x0100
#define IFD1_IMAGE_HEIGHT			0x0101
#define	IFD1_COMPRESSION			0x0103
#define	IFD1_THUMBNAIL_OFFSET		0x0201
#define	IFD1_THUMBNAIL_LENGTH		0x0202

#if (DSC_SUPPORT_BASELINE_MP_FILE)
// Index IFD for MPF
#define MPF_IDXIFD_MPF_VERSION		0xB000
#define MPF_IDXIFD_IMG_NUM			0xB001
#define MPF_IDXIFD_MP_ENTRY			0xB002
#define MPF_IDXIFD_IND_UNIQUE_ID_LIST	0xB003
#define MPF_IDXIFD_CAP_FRAME_NUM		0xB004

// MPF Entry ID
#define MPF_ENTRY_ID_PRIMARY_IMG	0
#define MPF_ENTRY_ID_LARGE_THUMB1	1
#define MPF_ENTRY_ID_LARGE_THUMB2	2
#endif

// IFD Index
#define IFD_IDX_IFD0				0
#define IFD_IDX_IFD1				1
#define IFD_IDX_GPSIFD				2
#define IFD_IDX_EXIFIFD				3
#define IFD_IDX_INTEROPIFD			4
#define IFD_IDX_MPF_INDEX_IFD		5
#define IFD_IDX_MPF_ATTR_IFD		6

// Exif Node Index
#define EXIF_NODE_ID_PRIMARY		0
#define EXIF_NODE_ID_LARGE_THUMB1	1
#define EXIF_NODE_ID_LARGE_THUMB2	2

//==============================================================================
//
//                              ENUMERATION
//
//==============================================================================

typedef enum _MMP_EXIF_TIF_TAG_TYPE 
{
    EXIF_TYPE_UNKNOWN    = 0,
    EXIF_TYPE_BYTE       = 1,
    EXIF_TYPE_ASCII      = 2,
    EXIF_TYPE_SHORT      = 3,
    EXIF_TYPE_LONG       = 4,
    EXIF_TYPE_RATIONAL   = 5,
    EXIF_TYPE_SBYTE      = 6,
    EXIF_TYPE_UNDEFINED  = 7,
    EXIF_TYPE_SSHORT     = 8,
    EXIF_TYPE_SLONG      = 9,
    EXIF_TYPE_SRATIONAL  = 10,
    EXIF_TYPE_FLOAT      = 11,
    EXIF_TYPE_DOUBLE     = 12
} MMP_EXIF_TIF_TAG_TYPE;

typedef enum _MMP_DSC_JPEG_FMT
{
    MMP_DSC_JPEG_FMT420 = 0,
    MMP_DSC_JPEG_FMT422,
    MMP_DSC_JPEG_FMT422_V,
    MMP_DSC_JPEG_FMT444,
    MMP_DSC_JPEG_FMT411,
    MMP_DSC_JPEG_FMT411_V,
    MMP_DSC_JPEG_FMT_NOT_3_COLOR,
    MMP_DSC_JPEG_FMT_NOT_BASELINE,
    MMP_DSC_JPEG_FMT_FORMAT_ERROR
} MMP_DSC_JPEG_FMT;

typedef enum _MMP_DSC_THUMB_INPUT_SRC
{
    MMP_DSC_THUMB_SRC_DECODED_JPEG = 0,		///< The input raw data of Thumbnail is Jpeg of DRAM. 
	MMP_DSC_THUMB_SRC_THUMB_PIPE,			///< The input raw data of Thumbnail is from 3rd pipe.
    MMP_DSC_THUMB_SRC_NONE
} MMP_DSC_THUMB_INPUT_SRC;

typedef enum _MMP_DSC_CAPTURE_ROTATE_TYPE
{
    MMP_DSC_CAPTURE_NO_ROTATE = 0,
    MMP_DSC_CAPTURE_ROTATE_RIGHT_90,
    MMP_DSC_CAPTURE_ROTATE_RIGHT_180,
    MMP_DSC_CAPTURE_ROTATE_RIGHT_270
} MMP_DSC_CAPTURE_ROTATE_TYPE;

typedef enum _MMP_DSC_PREVIEW_PATH
{
    MMP_DSC_STD_PREVIEW = 0,	       	 	///< Standard preview path
    MMP_DSC_RAW_PREVIEW		        		///< Raw preview path
} MMP_DSC_PREVIEW_PATH;

typedef enum _MMP_DSC_CALLBACK_TYPE
{
    MMP_DSC_CALLBACK_CAPTURE_START = 0,
    MMP_DSC_CALLBACK_STORECARD_START,
    MMP_DSC_CALLBACK_STORECARD_END,
    MMP_DSC_CALLBACK_STORECARD_ERROR
} MMP_DSC_CALLBACK_TYPE;

#if (DSC_SUPPORT_BASELINE_MP_FILE)
typedef enum _MMP_DSC_LARGE_THUMB_RES
{
    MMP_DSC_LARGE_THUMB_RES_FHD = 0,
    MMP_DSC_LARGE_THUMB_RES_VGA,
	MMP_DSC_LARGE_THUMB_RES_NUM
} MMP_DSC_LARGE_THUMB_RES;
#endif

typedef enum _MMP_DSC_JPEG_SETTYPE 
{
	MMP_DSC_JPEGCMD_NEWONE  = 0,    		///< New one stream JPEG
	MMP_DSC_JPEGCMD_SETCOMPBUF,     		///< Set compress buffer info
	MMP_DSC_JPEGCMD_STREAMEN,       		///< JPEG encode for streaming
	MMP_DSC_JPEGCMD_CHECKENC, 				///< Check JPEG encode done
	MMP_DSC_JPEGCMD_SET_FPS,			    ///< Set frame rate
    MMP_DSC_JPEGCMD_SET_LINKPIPE		    ///< Set link pipe
} MMP_DSC_JPEG_SETTYPE;

typedef enum _MMP_DSC_PAN_DIRECTION
{
    MMP_DSC_PAN_LEFT = 0,          			///< Picture pan left
    MMP_DSC_PAN_RIGHT,             			///< Picture pan right
    MMP_DSC_PAN_UP,                			///< Picture pan up
    MMP_DSC_PAN_DOWN               			///< Picture pan down
} MMP_DSC_PAN_DIRECTION;

typedef enum _MMP_DSC_ENC_EVENT 
{
    MMP_DSC_ENC_EVENT_ENC_DONE 		= 0,
    MMP_DSC_ENC_EVENT_COMPBUF_OVERFLOW,
    MMP_DSC_ENC_EVENT_LINEBUF_OVERLAP,
    MMP_DSC_ENC_EVENT_MAX
} MMP_DSC_ENC_EVENT;

typedef enum _MMP_DSC_DEC_EVENT 
{
    MMP_DSC_DEC_EVENT_DEC_DONE 		= 0,
    MMP_DSC_DEC_EVENT_MARKER_ERR,
    MMP_DSC_DEC_EVENT_HUFFMAN_ERR,
    MMP_DSC_DEC_EVENT_NONBASELINE,
    MMP_DSC_DEC_EVENT_MAX
} MMP_DSC_DEC_EVENT;

typedef enum _MMP_DSC_JPEG_QUALITY
{
    MMP_DSC_CAPTURE_HIGH_QUALITY = 0,  ///< Capture high quality Jpeg
    MMP_DSC_CAPTURE_NORMAL_QUALITY,    ///< Capture normal quality Jpeg
    MMP_DSC_CAPTURE_LOW_QUALITY        ///< Capture low quality Jpeg
} MMP_DSC_JPEG_QUALITY;

// Rate Control Index
typedef enum _MMP_DSC_JPEG_RC_ID 
{
    MMP_DSC_JPEG_RC_ID_CAPTURE = 0,
    MMP_DSC_JPEG_RC_ID_MJPEG_1ST_STREAM,
    MMP_DSC_JPEG_RC_ID_MJPEG_2ND_STREAM,
    MMP_DSC_JPEG_RC_ID_NUM
} MMP_DSC_JPEG_RC_ID;

typedef enum _MMP_DSC_JPEG_QTABLE
{
    MMP_DSC_JPEG_QT_AIT_HIGH = 0,
    MMP_DSC_JPEG_QT_AIT_NORMAL,
    MMP_DSC_JPEG_QT_AIT_LOW,
    MMP_DSC_JPEG_QT_CUSTOMER,
    MMP_DSC_JPEG_QT_NUM
} MMP_DSC_JPEG_QTABLE;
//==============================================================================
//
//                              STRUCTURE
//
//==============================================================================

typedef struct _MMP_DSC_CAPTURE_BUF 
{
    MMP_ULONG				ulCompressStart;    // Internal compressed buffer start address. 
    MMP_ULONG				ulCompressEnd;	    // Internal compressed buffer end address. 
    MMP_ULONG				ulLineStart;	    // 1st line buffer start address.
    MMP_ULONG				ul2ndLineStart;	    // 2nd line buffer start address.
} MMP_DSC_CAPTURE_BUF;

typedef struct _MMP_DSC_DECODE_BUF 
{
    MMP_ULONG  				ulDecompressStart;	// Compress buffer start address.
    MMP_ULONG  				ulDecompressEnd;    // Compress buffer end address.
    MMP_ULONG  				ulStorageTmpStart;  // EXIF/Data buffer start address.
    MMP_ULONG  				ulStorageTmpSize;   // EXIF/Data buffer size.
    MMP_ULONG  				ulLineBufStart;     // Line buffer start address.
	MMP_ULONG  				ulLineBufSize;      // Line buffer size.
} MMP_DSC_DECODE_BUF;

typedef struct _MMP_DSC_JPEGINFO 
{
    MMP_BYTE            	bJpegFileName[DSC_MAX_FILE_NAME_SIZE * 3];  	///< Input path (FIFO mode): SD card -> CPU -> FIFO -> Compressed buf -> JPEG
    MMP_USHORT          	usJpegFileNameLength;   						///< Input path (FIFO mode): SD card -> CPU -> FIFO -> Compressed buf -> JPEG
    MMP_ULONG          		ulJpegBufAddr;          						///< Input path (Memory mode) : Memory or External memory -> JPEG
    MMP_ULONG           	ulJpegBufSize;          						///< Input path (Memory mode) : Memory or External memory -> JPEG
    MMP_BOOL           		bValid; 										///< bValid is true means that jpeg is the supported format.
    MMP_DSC_JPEG_FMT 		jpgFormat;                 						///< This indicate the JPEG color format.
	MMP_ULONG          		ulFileSize;               						///< This indicate the JPEG file size.
    /* Primary Attribute */
    MMP_USHORT          	usPrimaryWidth;                					///< This indicate the primary JPEG width.
    MMP_USHORT          	usPrimaryHeight;               					///< This indicate the primary JPEG height.
	MMP_ULONG				ulPrimaryOffset;								///< This indicate the primary offset.
	MMP_ULONG				ulPrimarySize;									///< This indicate the primary size.    
	/* Thumbnail Attribute */
	MMP_BOOL 	      		bDecodeThumbnail;       						///< This indicate the decode thumbnail or not.
    MMP_USHORT          	usThumbWidth;           						///< This indicate the thumbnail width.
    MMP_USHORT        		usThumbHeight;          						///< This indicate the thumbnail height.
	MMP_ULONG				ulThumbOffset;									///< This indicate the thumbnail offset.
	MMP_ULONG				ulThumbSize;									///< This indicate the thumbnail size.
	#if (DSC_SUPPORT_BASELINE_MP_FILE)
	/* Large Thumbnail Attribute */
	MMP_BOOL 	      		bDecodeLargeThumb;       						///< This indicate the decode large thumbnail or not.
    MMP_USHORT          	usLargeThumbWidth;           					///< This indicate the large thumbnail width.
    MMP_USHORT        		usLargeThumbHeight;          					///< This indicate the large thumbnail height.
	MMP_ULONG				ulLargeThumbOffset;								///< This indicate the large thumbnail offset.
	MMP_ULONG				ulLargeThumbSize;								///< This indicate the large thumbnail size.	
	#endif
} MMP_DSC_JPEG_INFO;

typedef MMP_ERR (MMP_DSC_CALLBACK) (void*);
typedef int JpegEncCallBackFunc(void*);
typedef int JpegDecCallBackFunc(void*);

//==============================================================================
//
//                              EXTERN VARIABLE
//
//==============================================================================

extern MMP_DSC_CALLBACK *MMP_DSC_StartCaptureCallback;
extern MMP_DSC_CALLBACK *MMP_DSC_StartStoreCardCallback;
extern MMP_DSC_CALLBACK *MMP_DSC_EndStoreCardCallback;
extern MMP_DSC_CALLBACK *MMP_DSC_ErrorStoreCardCallback;

#endif //_MMP_DSC_INC_H_
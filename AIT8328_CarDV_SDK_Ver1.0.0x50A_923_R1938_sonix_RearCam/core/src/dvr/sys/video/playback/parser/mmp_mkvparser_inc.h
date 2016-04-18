#ifndef _MMP_MKV_PARSER_INC_H_
#define _MMP_MKV_PARSER_INC_H_

#define MKV_INT_ONLY	//Host code need this

#define EBML_SUPPORT_VERSION	1
#define EBML_MAX_ID_LENGTH	4
#define EBML_MAX_SIZE_LENGTH	8
#define MATROSKA_VERSION	2
#define MATROSKA_DOCTYPE	"matroska"

#define MKV_MAX_STRING_LEN		1023

#define MAXU64			0xffffffffffffffffull
#define ONE			1ull

typedef long long		MMP_LONG64;

#ifdef MKV_INT_ONLY
typedef MMP_LONG64			MMP_MKV_FLOAT;
#else
typedef double			MMP_MKV_FLOAT;
#endif
 
#define MKV_FS_BLK_SIZE		DEPACK_CACHE_SIZE		//file operation alignment

#define MKV_STR_BUF_SIZE	1024		//length of each string pool

#define MKV_MAX_FILES		MAX_DEPACK_CACHE		//maximum opened files
#define MKV_MAX_TRACKS		4		//maximum tracks
#define MKV_MAX_LACING_FRAME	16

#define MKV_EOF			0

typedef struct mkv_file{
	MMP_UBYTE		ubAlloc;

	MMP_ULONG		ulFilePos;			// current read position in buffer

	MMP_ULONG		ulFileSize;
	
	MMP_LONG		lFileIdx;		//OSCI private data
	MMP_LONG		lError;			//error occurs when error != 0
	
	MMP_BYTE		*pStrPool;
	MMP_ULONG		*pStrBufPos;
}MMPF_MKV_FILE;

typedef struct mkv_track {
	MMP_ULONG64	ullNumber;
	MMP_ULONG64	ullType;
	MMP_ULONG64	ullTrackOverlay;
	MMP_ULONG64	ullUID;
	MMP_ULONG64	ullMinCache;
	MMP_ULONG64	ullMaxCache;
	MMP_ULONG64	ullDefaultDuration;
	MMP_MKV_FLOAT	fTimecodeScale;
	void		*pCodecPrivate;
	MMP_ULONG64	ullCodecPrivatePos;
	MMP_ULONG	ulCodecPrivateSize;
	MMP_ULONG	ulCompMethod;
	void		*pCompMethodPrivate;
	MMP_ULONG	ulCompMethodPrivateSize;
	MMP_ULONG	ulMaxBlockAdditionID;

	MMP_ULONG	ulFlagEnabled;
	MMP_ULONG	ulFlagDefault;
	MMP_ULONG	ulFlagLacing;
	MMP_ULONG	ulFlagForced;
	MMP_ULONG	ulDecodeAll;
	MMP_ULONG	ulCompEnabled;

	union av{
		struct video {
			MMP_ULONG	ulStereoMode;
			MMP_ULONG	ulDisplayUnit;
			MMP_ULONG	ulAspectRatioType;
			MMP_ULONG	ulPixelWidth;
			MMP_ULONG	ulPixelHeight;
			MMP_ULONG	ulDisplayWidth;
			MMP_ULONG	ulDisplayHeight;
			MMP_ULONG	ulCropL, ulCropT, ulCropR, ulCropB;
			MMP_ULONG	ulColorSpace;
			MMP_MKV_FLOAT	fGammaValue;

			MMP_ULONG	ulInterlaced;
		} VIDEO;
		struct audio {
			MMP_MKV_FLOAT	fSamplingFreq;
			MMP_MKV_FLOAT	fOutputSamplingFreq;
			MMP_ULONG	ulChannels;
			MMP_ULONG	ulBitDepth;
		} AUDIO;
	} AV;

	/* various strings */
	MMP_BYTE		*pName;
	MMP_BYTE		pLanguage[4];
	MMP_BYTE		*pCodecID;
	MMP_BYTE		*pCodecName;
	MMP_BOOL        bUseflag;
	
	//for dumpping
	MMPF_MKV_FILE	*pMkvFile;				//file to handle dump track
	MMP_ULONG64	ullTrackTime;			//keep current track time
	MMP_ULONG64	ullTrackTimeOffset;		//keep block time

	MMPF_MKV_FILE	*pBSFile;				//file to handle dump track
	MMP_ULONG64		ullFrameBlockStart;
	MMP_ULONG64		ullFramePos;
	MMP_ULONG		ulFrameSize;
	MMP_ULONG		ulFrameIndex;
	MMP_ULONG64		ullHintPos;
	MMP_ULONG64     ullSampleStart;

	MMP_ULONG	ulFrames;
	MMP_ULONG	ulCurFrame;
	MMP_ULONG	ulLacingSize[MKV_MAX_LACING_FRAME];
	MMPF_MKV_FILE	*pSeekFile;			//used for parsing cues
} MMPF_MKV_TRACK;

typedef struct segment_info {
#if 0
	MMP_BYTE		pUID[16];
	MMP_BYTE		pPrevUID[16];
	MMP_BYTE		pNextUID[16];
#endif
	MMP_BYTE		*pFileName;
	MMP_BYTE		*pPrevFileName;
	MMP_BYTE		*pNextFileName;
	MMP_BYTE		*pTitle;
	MMP_BYTE		*pMuxingApp;
	MMP_BYTE		*pWritingApp;
	MMP_ULONG64		ullTimecodeScale;
	MMP_ULONG64		ullDuration;
	MMP_ULONG64		ullDateUTC;
	MMP_ULONG64		ullDateUTCValid;
} MMPF_MKV_SEG_INFO;

#define	TARGET_TRACK	  0
#define	TARGET_CHAPTER	  1
#define	TARGET_ATTACHMENT 2
#define	TARGET_EDITION	  3

typedef struct cue_point{
	MMP_ULONG64	ullTime;
	MMP_ULONG64	ullPosition;
	MMP_ULONG64	ullBlock;
	MMP_ULONG64	ullTrackNum;
}MMPF_MKV_CUE_POINT;

typedef struct mkv_context {
	MMP_BYTE		*pFileName;
	
	// parser config
	MMP_ULONG		ulFlags;

	MMPF_MKV_FILE		*pFile;
	
	//MMPF_MKV_STRPOOL	strpool;
	MMP_ULONG		ulStrBufPos;
	MMP_BYTE		bStrPool[MKV_STR_BUF_SIZE];

	// pointers to key elements
	MMP_ULONG64		ullSegPos;
	MMP_ULONG64		ullSeekHeadPos;
	MMP_ULONG64		ullSegInfoPos;
	MMP_ULONG64		ullClustersPos;
	MMP_ULONG64		ullTracksPos;
	MMP_ULONG64		ullCuesPos;
	MMP_ULONG64		ullAttachmentsPos;
	MMP_ULONG64		ullChaptersPos;
	MMP_ULONG64		ullTagsPos;

	MMP_ULONG64		ullClustersSize;
	MMP_ULONG64		ullCuesSize;

	// file info
	MMP_ULONG64		ullFirstTimecode;

	// SegmentInfo
	MMPF_MKV_SEG_INFO	seg;

	// Tracks
	MMP_ULONG		ullNumTracks;
	MMPF_MKV_TRACK		tracks[MKV_MAX_TRACKS];

} MMPF_MKV_CONTEXT;

typedef struct ebml_element{
	MMP_ULONG 	ulID;
	MMP_ULONG64 	ullStart;
	MMP_ULONG64	ullSize;
	MMP_ULONG64 	ullDataPos;
} MMPF_MKV_EBML_ELEMENT;

//for V_MS/VFW/FOURCC codec type
typedef struct{
	MMP_ULONG biSize;
	MMP_ULONG  biWidth;
	MMP_ULONG  biHeight;
	MMP_SHORT  biPlanes;
	MMP_SHORT  biBitCount;
	MMP_ULONG biCompression;
	MMP_ULONG biSizeImage;
	MMP_ULONG  biXPelsPerMeter;
	MMP_ULONG  biYPelsPerMeter;
	MMP_ULONG biClrUsed;
	MMP_ULONG biClrImportant;
}BitmapInfoHeader;

/**************************************** Matroska ID List *****************************************/

/* top-level */
#define EBML_ID				0x1A45DFA3
#define MKV_SEGMENT			0x18538067

/* Table 1 */
#define EBML_VERSION			0x4286
#define EBML_READVERSION		0x42F7
#define EBML_MAXIDLENGTH		0x42F2
#define EBML_MAXSIZELENGTH		0x42F3
#define EBML_DOCTYPE			0x4282
#define EBML_DOCTYPEVERSION		0x4287
#define EBML_DOCTYPEREADVERSION		0x4285

/* general EBML types */
#define EBML_VOID			0xEC
#define EBML_CRC32			0xBF

/* Table 2 */
#define MKV_SEG_INFO			0x1549A966		// -> Table 3
#define MKV_SEEK_HEAD			0x114D9B74		// -> Table 4
#define MKV_CLUSTER			0x1F43B675		// -> Table 16
#define MKV_TRACKS			0x1654AE6B		// -> Table 6
#define MKV_CUES			0x1C53BB6B		// -> Table 18
#define MKV_ATTACHMENTS			0x1941A469		// -> Table 26
#define MKV_CHAPTERS			0x1043A770		// -> Table 21
#define MKV_TAGS			0x1254C367		// -> Table 28

/* Table 3 */
#define MKV_SEG_UID			0x73A4
#define MKV_SEG_FILENAME		0x7384
#define MKV_PREV_UID			0x3CB923
#define MKV_PREV_FILENAME		0x3C83AB
#define MKV_NEXT_UID			0x3EB923
#define MKV_NEXT_FILENAME		0x3E83BB
#define MKV_TIMECODE_SCALE		0x2AD7B1
#define MKV_DURATION			0x4489
#define MKV_TITLE			0x7BA9
#define MKV_MUXING_APP			0x4D80
#define MKV_WRITING_APP			0x5741
#define MKV_DATEUTC			0x4461

/* Table 4 */
#define MKV_SEEK			0x4DBB			// -> Table 5

/* Table 5 */
#define MKV_SEEK_ID			0x53AB
#define MKV_SEEK_POSITION		0x53AC

/* Table 6 */
#define MKV_TRACK_ENTRY			0xAE			// -> Table 7

/* Table 7 */
#define MKV_TRACK_NUMBER		0xD7
#define MKV_TRACK_UID			0x73C5
#define MKV_TRACK_TYPE			0x83			// -> Table 13
#define MKV_TRACK_FLAG_ENABLED		0xB9
#define MKV_TRACK_FLAG_DEFAULT		0x88
#define MKV_TRACK_FLAG_FORCED		0x55AA
#define MKV_TRACK_FLAG_LACING		0x9C
#define MKV_TRACK_MIN_CACHE		0x6DE7
#define MKV_TRACK_MAX_CACHE		0x6DF8
#define MKV_TRACK_DEFAULT_DURATION	0x23E383
#define MKV_TRACK_TIMECODE_SCALE	0x23314F
#define MKV_TRACK_NAME			0x536E
#define MKV_TRACK_LANGUAGE		0x22B59C
#define MKV_TRACK_CODEC_ID		0x86
#define MKV_TRACK_CODEC_PRIVATE		0x63A2
#define MKV_TRACK_CODEC_NAME		0x258688
#define MKV_TRACK_ATTACH_LINK		0x7446
#define MKV_TRACK_VIDEO_INFO		0xE0			// -> Table 8
#define MKV_TRACK_AUDIO_INFO		0xE1			// -> Table 9
#define MKV_TRACK_CONTENT_ENCODINGS	0x6D80			// -> Table 10

#define MKV_TRACK_CODEC_INFOURL 		0x3B4040
#define MKV_TRACK_CODEC_DOWNLOADURL	0x26B240
#define MKV_TRACK_CODEC_DECODEALL	0xAA
#define MKV_TRACK_MAX_BLK_ADDID		0x55EE

/* Table 8 */
#define MKV_VIDEO_PIXEL_WIDTH		0xB0
#define MKV_VIDEO_PIXEL_HEIGHT		0xBA
#define MKV_VIDEO_PIXEL_CROP_BUTTOM	0x54AA
#define MKV_VIDEO_PIXEL_CROP_TOP	0x54BB
#define MKV_VIDEO_PIXEL_CROP_LEFT	0x54CC
#define MKV_VIDEO_PIXEL_CROP_RIGHT	0x54DD
#define MKV_VIDEO_DISPLAY_WIDTH		0x54B0
#define MKV_VIDEO_DISPLAY_HEIGHT	0x54BA
#define MKV_VIDEO_DISPLAY_UNIT		0x54B2

#define MKV_VIDEO_FRAME_RATE		0x2383E3
#define MKV_VIDEO_FLAG_INTERLACED	0x9A
#define MKV_VIDEO_STEREO_MODE		0x53B9
#define MKV_VIDEO_ASPECT_RATIO		0x54B3
#define MKV_VIDEO_COLORSPACE		0x2EB524
#define MKV_VIDEO_GAMMA_VALUE		0x2FB523

/* Table 9 */
#define MKV_AUDIO_SAMPLING_FREQ 	0xB5
#define MKV_AUDIO_OUTPUT_SAMPLING_FREQ	0x78B5
#define MKV_AUDIO_CHANNELS		0x9F
#define MKV_AUDIO_BIT_DEPTH		0x6264

/* Table 10 */
#define MKV_TRACK_CONTENTENCODING	0x6240			// -> Table 11


/* Table 11 */
#define MKV_CONTENT_ENC_ORDER		0x5031
#define MKV_CONTENT_ENC_SCOPE		0x5032			// -> Table 14
#define MKV_CONTENT_ENC_TYPE		0x5033
#define MKV_CONTENT_COMPRESSION		0x5034			// -> Table 12
#define MKV_CONTENT_ENCRYPTION		0x5035

/* Table 12 */
#define MKV_CONTENT_COMP_ALGO		0x4254			// -> Table 15
#define MKV_CONTENT_COMP_SETTINGS	0x4255

/* Table 13 */
#define MKV_TRACKTYPE_NONE		0x00
#define MKV_TRACKTYPE_VIDEO		0x01
#define MKV_TRACKTYPE_AUDIO		0x02
#define MKV_TRACKTYPE_COMPLEX		0x03
#define MKV_TRACKTYPE_LOGO		0x10
#define MKV_TRACKTYPE_SUBTITLE		0x11
#define MKV_TRACKTYPE_BUTTON		0x12
#define MKV_TRACKTYPE_CONTROL		0x20

/* Table 14 */
#define MKV_CONTENT_ENC_ALL		0x01
#define MKV_CONTENT_ENC_CODEC_PRIVATE	0x02
#define MKV_CONTENT_ENC			0x04

/* Table 15 */
#define MKV_COMP_ALGO_ZLIB		0x00
#define MKV_COMP_ALGO_BZLIB		0x01
#define MKV_COMP_ALGO_LZOLX		0x02
#define MKV_COMP_ALGO_HEADER_STRIP	0x03

/* Table 16 */
#define MKV_CLUSTER_TIME_CODE		0xE7
#define MKV_CLUSTER_POSITION		0xA7
#define MKV_CLUSTER_PREV_SIZE		0xAB
#define MKV_CLUSTER_BLOCK_GROUP		0xA0			// -> Table 17
#define MKV_CLUSTER_SIMPLE_BLOCK	0xA3

/* Table 17 */
#define MKV_BLOCK			0xA1
#define MKV_REFERENCE_BLOCK		0xFB
#define MKV_BLOCK_DURATION		0x9B

/* Table 18 */
#define MKV_CUE_POINT			0xBB			// -> Table 19

/* Table 19 */
#define MKV_CUE_TIME			0xB3
#define MKV_CUE_TRACK_POSITIONS		0xB7			// -> Table 20

/* Table 20 */
#define MKV_CUE_TRACK			0xF7
#define MKV_CUE_CLUSTER_POSITION	0xF1
#define MKV_CUE_BLOCK_NUMBER		0x5378

/* Table 21 */
#define MKV_EDITION_ENTRY		0x45B9			// -> Table 22

/* Table 22 */
#define MKV_EDITION_UID			0x45BC
#define MKV_EDITION_FLAG_HIDDEN		0x45BD
#define MKV_EDITION_FLAG_DEFAULT	0x45DB
#define MKV_EDITION_FLAG_ORDERED	0x45DD
#define MKV_CHAPTER_ATOM		0xB6			// -> Table 23

/* Table 23 */
#define MKV_CHAPTER_UID			0x73C4
#define MKV_CHAPTER_TIME_START		0x91
#define MKV_CHAPTER_TIME_END		0x92
#define MKV_CHAPTER_FLAG_HIDDEN		0x98
#define MKV_CHAPTER_FLAG_ENABLED	0x4598
#define MKV_CHAPTER_SEG_UID		0x6E67
#define MKV_CHAPTER_SEG_EDITION_UID	0x6EBC
#define MKV_CHAPTER_TRACKS		0x8F			// -> Table 24
#define MKV_CHAPTER_DISPLAY		0x80			// -> Table 25

/* Table 24 */
#define MKV_CHAPTER_TRACK_NUM		0x89

/* Table 25 */
#define MKV_CHAPTER_STRING		0x85
#define MKV_CHAPTER_LANG		0x437C
#define MKV_CHAPTER_COUNTRY		0x437E
#define MKV_CHAPTER_PHY_SEQUIV		0x63C3

/* Table 26 */
#define MKV_ATTACHED_FILE		0x61A7			// -> Table 27

#define MKV_FILE_DESCRIPTION		0x467E
#define MKV_FILE_NAME			0x466E
#define MKV_FILE_MIMETYPE		0x4660
#define MKV_FILE_DATA			0x465C
#define MKV_FILE_UID			0x46AE

/* Table 28 */
#define MKV_TAG				0x7373			// -> Table 29

/* Table 29 */
#define MKV_TAG_TARGETS			0x63C0			// -> Table 30
#define MKV_SIMPLETAG			0x67C8			// -> Table 31

/* Table 30 */
#define MKV_TAG_TARGETS_TYPE_VALUE	0x68CA
#define MKV_TAG_TARGETS_TYPE		0x63CA
#define MKV_TAG_TARGETS_TRACK_UID 	0x63C5
#define MKV_TAG_TARGETS_CHAPTER_UID	0x63C4
#define MKV_TAG_TARGETS_ATTACH_UID	0x63C6
#define MKV_TAG_TARGETS_EDITION_UID	0x63C9

/* Table 31 */
#define MKV_TAG_NAME			0x45A3
#define MKV_TAG_LANG			0x447A
#define MKV_TAG_ORINGIN			0x4484
#define MKV_TAG_STRING			0x4487
#define MKV_TAG_BINARY			0x4485
#define MKV_TAG_DEFAULT			0x44B4

#define FOURCC_TAG(a,b,c,d) (a | (b << 8) | (c << 16) | (d << 24))


typedef enum {
	MKV_TRACK_NONE     = 0x0,
	MKV_TRACK_VIDEO    = 0x1,
	MKV_TRACK_AUDIO    = 0x2,
	MKV_TRACK_COMPLEX  = 0x3,
	MKV_TRACK_LOGO     = 0x10,
	MKV_TRACK_SUBTITLE = 0x11,
	MKV_TRACK_CONTROL  = 0x20
} mkv_track_type;

typedef enum {
	MKV_COMP_ZLIB        = 0,
	MKV_COMP_BZLIB       = 1,
	MKV_COMP_LZO         = 2,
	MKV_COMP_HEADERSTRIP = 3
} mkv_track_enc_algo;

typedef enum {
	MKV_LACING_NONE	= 0,
	MKV_LACING_XIPH = 1,
	MKV_LACING_FIXED = 2,
	MKV_LACING_EBML = 3
} mkv_lacing_type;



#ifdef BUILD_FW
//#define MKV_DEBUG
#else
//#define MKV_DEBUG
#endif

#ifdef BUILD_FW

void debug_putc(int c){
	char buf[2] = { 0, 0 };
	buf[0] = (char) c;
	DBG_S(3, buf);
}

#include <stdarg.h>
void
print_msg(const char *fmt, ...)
{
	static const char digits[] = "0123456789abcdef";
	va_list ap;
	char buf[32];
	char *s;
	unsigned long long u;
	int num_hex;
	int c, i, pad;

	va_start(ap, fmt);
	while ((c = *fmt++) != 0) {
		if (c == '%') {
			num_hex = 8;
			c = *fmt++;
			/* ignore long */
			if (c == 'l'){
				c = *fmt++;
				if (c == 'l'){
					c = *fmt++;
					num_hex = 16;
				}
			}				
			switch (c) {
			case 'c':
				debug_putc(va_arg(ap, int));
				continue;
			case 's':
				s = va_arg(ap, char *);
				if (s == NULL)
					s = "<NULL>";
				for (; *s; s++) {
					debug_putc((int)*s);
				}
				continue;
			case 'd':
				c = 'u';
			case 'f':
			case 'u':
			case 'x':
			case 'X':
				if(num_hex == 8)
					u = va_arg(ap, unsigned);
				else
					u = va_arg(ap, unsigned long long);
				s = buf;
				if (c == 'u') {
					do
						*s++ = digits[u % 10U];
					while (u /= 10U);
				} else {
					pad = 0;
					for (i = 0; i < num_hex; i++) {
						if (pad)
							*s++ = '0';
						else {
							*s++ = digits[u % 16U];
							if ((u /= 16U) == 0){
								pad = 1;
								//champ
								break;
							}
						}
					}
				}
				while (--s >= buf)
					debug_putc((int)*s);
				continue;
			}
		}
		if (c == '\n')
			debug_putc('\r');
		debug_putc((int)c);
	}
	va_end(ap);
}
#else
#define print_msg(x, ...)
#endif

#ifdef MKV_DEBUG
#define MMPF_MKV_Message	print_msg
#else
#define MMPF_MKV_Message(x, ...)
#endif

#ifdef BUILD_FW

MMP_ERR MMPF_MKVPSR_SeekByTime(const MMP_LONG ulTime, MMP_ULONG ulSeekOption);

static MMPF_MKV_FILE		m_Files[MKV_MAX_FILES];
static MMPF_MKV_CONTEXT m_MkvContext;
static MMPF_MKV_TRACK *m_pMkvVideoTrack;
static MMPF_MKV_TRACK *m_pMkvAudioTrack;


typedef enum {
	MKV_NONE_OP	= 0,
	MKV_SEEK_OP,
	MKV_FF_OP
} mkv_op_states;
MMP_UBYTE m_MkvOpState;

#else // End of #ifdef BUILD_FW

// For Host Parser
static MMPF_MKV_FILE		m_gFiles;
static MMPF_MKV_CONTEXT m_gMkvContext;
static MMP_UBYTE      m_ASCBuffer[512];
static MMP_UBYTE      m_TempBSBuffer[512];

#endif

static MMP_ULONG fourcc_tag[][2] = {
    { MMP_VID_VIDEO_H264, FOURCC_TAG('H', '2', '6', '4') },
    { MMP_VID_VIDEO_H264, FOURCC_TAG('h', '2', '6', '4') },
    { MMP_VID_VIDEO_H264, FOURCC_TAG('X', '2', '6', '4') },
    { MMP_VID_VIDEO_H264, FOURCC_TAG('x', '2', '6', '4') },
    { MMP_VID_VIDEO_H264, FOURCC_TAG('a', 'v', 'c', '1') },
    { MMP_VID_VIDEO_H264, FOURCC_TAG('V', 'S', 'S', 'H') },
    { MMP_VID_VIDEO_H263, FOURCC_TAG('H', '2', '6', '3') },
    { MMP_VID_VIDEO_H263, FOURCC_TAG('X', '2', '6', '3') },
    { MMP_VID_VIDEO_H263, FOURCC_TAG('T', '2', '6', '3') },
    { MMP_VID_VIDEO_H263, FOURCC_TAG('L', '2', '6', '3') },
    { MMP_VID_VIDEO_H263, FOURCC_TAG('V', 'X', '1', 'K') },
    { MMP_VID_VIDEO_H263, FOURCC_TAG('Z', 'y', 'G', 'o') },
    { MMP_VID_VIDEO_MP4V, FOURCC_TAG('F', 'M', 'P', '4') },
    { MMP_VID_VIDEO_MP4V, FOURCC_TAG('D', 'I', 'V', 'X') },
    { MMP_VID_VIDEO_MP4V, FOURCC_TAG('D', 'X', '5', '0') },
    { MMP_VID_VIDEO_MP4V, FOURCC_TAG('X', 'V', 'I', 'D') },
    { MMP_VID_VIDEO_MP4V, FOURCC_TAG('M', 'P', '4', 'S') },
    { MMP_VID_VIDEO_MP4V, FOURCC_TAG('M', '4', 'S', '2') },
    { MMP_VID_VIDEO_MP4V, FOURCC_TAG( 4 ,  0 ,  0 ,  0 ) }, /* some broken avi use this */
    { MMP_VID_VIDEO_MP4V, FOURCC_TAG('D', 'I', 'V', '1') },
    { MMP_VID_VIDEO_MP4V, FOURCC_TAG('B', 'L', 'Z', '0') },
    { MMP_VID_VIDEO_MP4V, FOURCC_TAG('m', 'p', '4', 'v') },
    { MMP_VID_VIDEO_MP4V, FOURCC_TAG('U', 'M', 'P', '4') },
    { MMP_VID_VIDEO_MP4V, FOURCC_TAG('W', 'V', '1', 'F') },
    { MMP_VID_VIDEO_MP4V, FOURCC_TAG('S', 'E', 'D', 'G') },
    { MMP_VID_VIDEO_MP4V, FOURCC_TAG('R', 'M', 'P', '4') },
    { MMP_VID_VIDEO_MP4V, FOURCC_TAG('3', 'I', 'V', '2') },
    { MMP_VID_VIDEO_MP4V, FOURCC_TAG('F', 'F', 'D', 'S') },
    { MMP_VID_VIDEO_MP4V, FOURCC_TAG('F', 'V', 'F', 'W') },
    { MMP_VID_VIDEO_MP4V, FOURCC_TAG('D', 'C', 'O', 'D') },
    { MMP_VID_VIDEO_MP4V, FOURCC_TAG('M', 'V', 'X', 'M') },
    { MMP_VID_VIDEO_MP4V, FOURCC_TAG('P', 'M', '4', 'V') },
    { MMP_VID_VIDEO_MP4V, FOURCC_TAG('S', 'M', 'P', '4') },
    { MMP_VID_VIDEO_MP4V, FOURCC_TAG('D', 'X', 'G', 'M') },
    { MMP_VID_VIDEO_MP4V, FOURCC_TAG('V', 'I', 'D', 'M') },
    { MMP_VID_VIDEO_MP4V, FOURCC_TAG('M', '4', 'T', '3') },
    { MMP_VID_VIDEO_MP4V, FOURCC_TAG('G', 'E', 'O', 'X') },
    { MMP_VID_VIDEO_MP4V, FOURCC_TAG('H', 'D', 'X', '4') }, /* flipped video */
    { MMP_VID_VIDEO_MP4V, FOURCC_TAG('D', 'M', 'K', '2') },
    { MMP_VID_VIDEO_MP4V, FOURCC_TAG('D', 'I', 'G', 'I') },
    { MMP_VID_VIDEO_MP4V, FOURCC_TAG('I', 'N', 'M', 'C') },
    { MMP_VID_VIDEO_MP4V, FOURCC_TAG('E', 'P', 'H', 'V') }, /* Ephv MPEG-4 */
    { MMP_VID_VIDEO_MP4V, FOURCC_TAG('E', 'M', '4', 'A') },
    { MMP_VID_VIDEO_MP4V, FOURCC_TAG('M', '4', 'C', 'C') }, /* Divio MPEG-4 */
    { MMP_VID_VIDEO_MP4V, FOURCC_TAG('S', 'N', '4', '0') },
	{ MMP_VID_VIDEO_NONE, 0 }
};

/*
static void  errorjmp(void* ptr, const MMP_BYTE *fmt, ...) {
	//ERROR handling
}
*/



#if 0 // for IDE function name list by section
void _____MKV_Parser_File_Operations_API_____(){}
#endif

#ifdef BUILD_FW
static MMPF_MKV_FILE* MMPF_MKV_FileOpen(
	void
){
	MMP_LONG i;
	MMPF_MKV_FILE *pMkvFile;

	//pMkvFile = mkv_osci_mem_alloc(sizeof(MMPF_MKV_FILE));	
	//mkv_osci_mem_set(pMkvFile, 0, sizeof(MMPF_MKV_FILE));
	for(i = 0; i < MKV_MAX_FILES; i++){
		if(m_Files[i].ubAlloc == 0){
			pMkvFile = &(m_Files[i]);
			pMkvFile->lFileIdx = i;
			pMkvFile->ubAlloc = 1;
			break;
		}
		if(i == (MKV_MAX_FILES-1)){
			pMkvFile->lFileIdx = -1;
		}
	}

	//pMkvFile->lFileIdx = MMPF_MKV_CacheFileOpen();
	if(pMkvFile->lFileIdx < 0){
		pMkvFile = NULL;
	}else{
		pMkvFile->pStrPool = m_MkvContext.bStrPool;
		pMkvFile->pStrBufPos = &(m_MkvContext.ulStrBufPos);
		pMkvFile->ulFilePos = 0;
		pMkvFile->ulFileSize = gVidFileInfo.file_size;
	}

	return pMkvFile;
}
#else
static MMPF_MKV_FILE* MMPD_MKV_FileOpen(
	MMP_ULONG file_size
){
	MMP_LONG i = 0;
	MMPF_MKV_FILE *pMkvFile;

	pMkvFile = &(m_gFiles);
	pMkvFile->lFileIdx = i;
	pMkvFile->ubAlloc = 1;

	pMkvFile->pStrPool = m_gMkvContext.bStrPool;
	pMkvFile->pStrBufPos = &(m_gMkvContext.ulStrBufPos);
	pMkvFile->ulFilePos = 0;
	pMkvFile->ulFileSize = file_size;

	return pMkvFile;
}
#endif

static MMP_ULONG MMPF_MKV_FileSize(
	MMPF_MKV_FILE* pMkvFile
){
	return pMkvFile->ulFileSize;
}

#ifdef BUILD_FW
static void MMPF_MKV_FileClose(
	MMPF_MKV_FILE* pMkvFile
){
	pMkvFile->ubAlloc = 0;
}
#endif

static MMP_ULONG64 MMPF_MKV_FilePosition(
	MMPF_MKV_FILE *pMkvFile
){
	return (MMP_ULONG64)(pMkvFile->ulFilePos);
}

// 512-alignment/multiple read
static void MMPF_MKV_FileReadBytes(
	MMPF_MKV_FILE *pMkvFile,
	void *buffer, 
	MMP_LONG len
){
	MMP_ERR error;
	
    #ifdef BUILD_FW
	MMPF_VIDPSR_SwapDepackCacheIdx(pMkvFile->lFileIdx);
	error = MMPF_VIDPSR_ReadFile( (MMP_UBYTE*)buffer, pMkvFile->ulFilePos, len);
	#else
	//MMPD_3GPPSR_ReadFile((MMP_UBYTE*)buffer, pMkvFile->ulFilePos, len);//TBD
	#endif
	
	if(error != MMP_ERR_NONE){
		MMPF_MKV_Message("[%s] MMPF_MKV_CacheFileRead error!!!!\n", __func__ );
	}

	pMkvFile->ulFilePos += len;
}

static MMP_LONG MMPF_MKV_FileReadByte(
	MMPF_MKV_FILE *pMkvFile
){
	MMP_UBYTE ubByteBuf[2];
	MMPF_MKV_FileReadBytes(pMkvFile, ubByteBuf, 1);
	return (MMP_LONG)ubByteBuf[0];
}

// 512-alignment/multiple read
static void MMPF_MKV_FileSkipBytes(
	MMPF_MKV_FILE *pMkvFile,
	MMP_ULONG64 ullLen
){
	pMkvFile->ulFilePos += (MMP_ULONG)ullLen;
}

static void MMPF_MKV_FileSeek(
	MMPF_MKV_FILE *pMkvFile,
	MMP_ULONG64 ullOffset
){
	pMkvFile->ulFilePos = (MMP_ULONG)ullOffset;
}

#ifdef BUILD_FW
static void MMP_MKV_ResetTrackPos(
            MMPF_MKV_CONTEXT *pMkvCtx,
            MMPF_MKV_FILE *pMkvFile
){
    pMkvFile->ulFilePos = pMkvCtx->ullClustersPos;
}
#endif

#if 0 // for IDE function name list by section
void _____MKV_Floating_Point_API_____(){}
#endif

static MMP_MKV_FLOAT MMPF_MKV_FloatGet(
	MMP_LONG lValue
){
#ifdef MKV_INT_ONLY
	MMP_MKV_FLOAT  fValue;
	fValue = (MMP_LONG64)lValue << 32;
	return fValue;
#else
	return lValue;
#endif
}

static MMP_LONG64 MMPF_MKV_FloatMult(MMP_MKV_FLOAT scale, MMP_LONG64 tc) {
#ifdef MKV_INT_ONLY
	//             x1 x0
	//             y1 y0
	//    --------------
	//             x0*y0
	//          x1*y0
	//          x0*y1
	//       x1*y1
	//    --------------
	//       .. r1 r0 ..
	//
	//    r = ((x0*y0) >> 32) + (x1*y0) + (x0*y1) + ((x1*y1) << 32)
	MMP_ULONG x0, x1, y0, y1;
	MMP_ULONG64 p;
	MMP_BYTE sign = 0;

	if (scale < 0)
		sign = !sign, scale = -scale;
	if (tc < 0)
		sign = !sign, tc = -tc;

	x0 = (MMP_ULONG)scale;
	x1 = (MMP_ULONG)((MMP_ULONG64)scale >> 32);
	y0 = (MMP_ULONG)tc;
	y1 = (MMP_ULONG)((MMP_ULONG64)tc >> 32);

	p = (MMP_ULONG64)x0*y0 >> 32;
	p += (MMP_ULONG64)x0*y1;
	p += (MMP_ULONG64)x1*y0;
	p += (MMP_ULONG64)(x1*y1) << 32;

	return p;
#else
	return (MMP_LONG64)(scale * tc);
#endif
}

#if 0 // for IDE function name list by section
void _____MKV_EBML_Parse_API_____(){}
#endif

static MMP_ULONG64 MMPF_MKV_EBML_ReadVuint(
	MMPF_MKV_FILE *pMkvFile,
	MMP_LONG *pMask,
	MMP_LONG ulMode
){
	
	MMP_ULONG ulByteVal;
	MMP_ULONG ulVintLen = 1;
	MMP_ULONG ulLenCnt = 0;
	MMP_ULONG ulVintMask = 0;
	MMP_ULONG ulVintMaskShift = 0;
	MMP_ULONG64 ullVintVal = 0;
	static MMP_ULONG ulLeadZero[16] = { 4, 3, 2, 2, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0 };

	ulByteVal = MMPF_MKV_FileReadByte(pMkvFile);
	if (ulByteVal == MKV_EOF)
		return 0; // XXX should errorjmp()?

	if (ulByteVal == 0){
		//errorjmp(pMkvCtx,"Invalid first byte of EBML integer: 0");
	}

	ulVintLen += ulLeadZero[ (ulByteVal >> 4) & 0xF ];
	if(ulVintLen == 5)	//the first 4 bits are 0'b
		ulVintLen += ulLeadZero[ ulByteVal & 0xF ];
	ulVintMaskShift = ( 9 - ulVintLen );
	if(ulMode == 0)
		ulVintMaskShift--;
	ulVintMask = ( 1 << ulVintMaskShift ) - 1;

	ullVintVal = (ulByteVal & ulVintMask);
	for(ulLenCnt = 1; ulLenCnt < ulVintLen; ulLenCnt++){
		ulByteVal = MMPF_MKV_FileReadByte(pMkvFile);
		if (ulByteVal == MKV_EOF){
			//errorjmp(pMkvCtx,"Got EOF while reading EBML unsigned integer");
		}
		ullVintVal = (ullVintVal << 8) | ulByteVal;
	}
	
	*pMask = ulVintLen - 1;
	return ullVintVal;
	// NOT REACHED
}

static MMP_LONG64 MMPF_MKV_EBML_ReadVsint(
	MMPF_MKV_FILE *pMkvFile
){
	static MMP_LONG64 ullBias[8] = { (ONE<<6)-1, (ONE<<13)-1, (ONE<<20)-1, (ONE<<27)-1, (ONE<<34)-1, (ONE<<41)-1, (ONE<<48)-1, (ONE<<55)-1 };

	MMP_LONG ullMask;
	MMP_LONG64 ullVal = MMPF_MKV_EBML_ReadVuint(pMkvFile,&ullMask,0);

	return ullVal - ullBias[ullMask];
}

static MMP_ULONG MMPF_MKV_EBML_ReadID(
	MMPF_MKV_FILE *pMkvFile
){
	MMP_LONG lLenMinus1 = -1;
	MMP_ULONG ulID;
	ulID = (MMP_ULONG)MMPF_MKV_EBML_ReadVuint(pMkvFile, &lLenMinus1, 1);
	
	if(lLenMinus1 < 0){
		//error: read error
		return -1;
	}else if(lLenMinus1 < 4){
		return ulID;
	}else{
		//error: id length > 4
		return -1;
	}
}

static MMP_ULONG64 MMPF_MKV_EBML_ReadSize(
	MMPF_MKV_FILE *pMkvFile
){
	MMP_LONG lMask;
	MMP_ULONG64 ullVal = MMPF_MKV_EBML_ReadVuint(pMkvFile, &lMask, 0);

	// see if it's unspecified
	if (ullVal == (MAXU64 >> (57-lMask*7))){
		//errorjmp(pMkvCtx,"Unspecified element size is not supported here.");
	}

	return ullVal;
}


static MMP_ULONG64 MMPF_MKV_EBML_ReadUint(
	MMPF_MKV_FILE *pMkvFile,
	MMP_ULONG len
){
	MMP_LONG c;
	MMP_ULONG m = len;
	MMP_ULONG64 v = 0;

	if (len == 0)
		return v;
	if (len > 8){
		//errorjmp(pMkvCtx,"Unsupported integer size in readUInt: %u",len);
	}

	do {
		c = MMPF_MKV_FileReadByte(pMkvFile);
		if (c == MKV_EOF){
			//errorjmp(pMkvCtx,"Got EOF while reading EBML unsigned integer");
		}
		v = (v << 8) | c;
	} while (--m);

	return v;
}

static MMP_LONG64 MMPF_MKV_EBML_ReadSint(
	MMPF_MKV_FILE *pMkvFile,
	MMP_ULONG ulLen
){
	MMP_LONG64 llVal = MMPF_MKV_EBML_ReadUint(pMkvFile, ulLen);
	MMP_LONG lShift = 64 - (ulLen << 3);
	return (llVal << lShift) >> lShift;
}

static MMP_MKV_FLOAT MMPF_MKV_EBML_ReadFloat(
	MMPF_MKV_FILE *pMkvFile,
	MMP_ULONG len
){
#ifdef MKV_INT_ONLY
	MMP_MKV_FLOAT f;
	MMP_LONG shift;
#else
	union {
		MMP_ULONG ui;
		MMP_ULONG64 ull;
		float f;
		double d;
	} u;
#endif

	if (len!=4 && len!=8){
		//errorjmp(pMkvCtx,"Invalid float size in read float: %u",len);
	}

#ifdef MKV_INT_ONLY
	if (len == 4) {
		MMP_ULONG  ui = (MMP_ULONG)MMPF_MKV_EBML_ReadUint(pMkvFile, (MMP_ULONG)len);
		f = (ui & 0x7fffff) | 0x800000;
		if (ui & 0x80000000)
			f = -f;
		shift = (ui >> 23) & 0xff;
		if (shift == 0) // assume 0
zero:
			shift = 0, f = 0;
		else if (shift == 255)
inf:
			if (ui & 0x80000000)
				f = 0x8000000000000000ull;
			else
				f = 0x7fffffffffffffffll;
		else {
			shift += -127 + 9;
			if (shift > 39)
				goto inf;
shift:
			if (shift < 0)
				f = f >> -shift;
			else if (shift > 0)
				f = f << shift;
		}
	} else if (len == 8) {
		MMP_ULONG64  ui = MMPF_MKV_EBML_ReadUint(pMkvFile,(MMP_ULONG)len);
		f = (ui & 0xfffffffffffffll) | 0x10000000000000ll ;
		if (ui & 0x80000000)
			f = -f;
		shift = (MMP_LONG)((ui >> 52) & 0x7ff);
		if (shift == 0) // assume 0
			goto zero;
		else if (shift == 2047)
			goto inf;
		else {
			shift += -1023 - 20;
			if (shift > 10)
				goto inf;
			goto shift;
		}
	}

	return f;
#else
	if (len == 4) {
		u.ui = (MMP_ULONG)MMPF_MKV_EBML_ReadUint(pMkvFile, (MMP_ULONG)len);
		return u.f;
	}

	if (len == 8) {
		u.ull = MMPF_MKV_EBML_ReadUint(pMkvFile, (MMP_ULONG)len);
		return u.d;
	}

	return 0;
#endif
}

static MMP_BYTE* MMPF_MKV_EBML_ReadString(
	MMPF_MKV_FILE *pMkvFile,
	MMP_ULONG64 ullLen
){
	MMP_BYTE *pBuffer;
	
	pBuffer = pMkvFile->pStrPool + *(pMkvFile->pStrBufPos);
	MMPF_MKV_FileReadBytes(pMkvFile, pBuffer, ullLen);
	pBuffer[ullLen] = '\0';
	*(pMkvFile->pStrBufPos) += (ullLen+1);
	
	return pBuffer;
}

static void MMPF_MKV_EBML_ReadLangCC(
	MMPF_MKV_FILE *pMkvFile,
	MMP_ULONG64 ullLen,
	MMP_BYTE bLcc[4]
){
	MMP_ULONG  ulTodo = ullLen > 3 ? 3 : (MMP_LONG)ullLen;

	bLcc[0] = bLcc[1] = bLcc[2] = bLcc[3] = 0;
	MMPF_MKV_FileReadBytes(pMkvFile, bLcc, ulTodo);
	MMPF_MKV_FileSkipBytes(pMkvFile, ullLen - ulTodo);
}

static MMP_ULONG MMPF_MKV_EBML_ReadElement(
	MMPF_MKV_FILE *pMkvFile,
	MMPF_MKV_EBML_ELEMENT *pElement,
	MMP_ULONG64 ullStart,
	MMP_ULONG64 ullLen
){
	if((ullStart + ullLen) <= MMPF_MKV_FilePosition(pMkvFile))
		return 0;

	pElement->ullStart = MMPF_MKV_FilePosition(pMkvFile);
	pElement->ulID = MMPF_MKV_EBML_ReadID(pMkvFile);
	pElement->ullSize = MMPF_MKV_EBML_ReadSize(pMkvFile);
	pElement->ullDataPos = MMPF_MKV_FilePosition(pMkvFile);
	
	return 1;
}

#if 0 // for IDE function name list by section
void _____MKV_Parse_API_____(){}
#endif

static void MMPF_MKV_ParseEBMLHeader(
	MMPF_MKV_FILE *pMkvFile,
	MMP_ULONG64 ullStart,
	MMP_ULONG64 ullLen
){
	MMP_BYTE *pDocType;
	MMP_ULONG64 ullVal;
	MMPF_MKV_EBML_ELEMENT element;
	
	MMPF_MKV_Message("%s start %llX len %llX\n", __func__, ullStart, ullLen);
	
	while(MMPF_MKV_EBML_ReadElement(pMkvFile, &element, ullStart, ullLen)){
		switch(element.ulID){
			case EBML_VERSION:
				ullVal = MMPF_MKV_EBML_ReadUint(pMkvFile, (MMP_ULONG)element.ullSize);
				MMPF_MKV_Message("EBML_VERSION: %lld\n", ullVal);
				break;
			case EBML_READVERSION:
				ullVal = MMPF_MKV_EBML_ReadUint(pMkvFile, (MMP_ULONG)element.ullSize);
				if(ullVal > EBML_SUPPORT_VERSION){
					//error
				}
				MMPF_MKV_Message("EBML_READVERSION: %lld\n", ullVal);
				break;
			case EBML_MAXIDLENGTH:
				ullVal = MMPF_MKV_EBML_ReadUint(pMkvFile, (MMP_ULONG)element.ullSize);
				if (ullVal > EBML_MAX_ID_LENGTH){
				}
				MMPF_MKV_Message("EBML_MAXIDLENGTH: %lld\n", ullVal);
				break;
			case EBML_MAXSIZELENGTH:
				ullVal = MMPF_MKV_EBML_ReadUint(pMkvFile, (MMP_ULONG)element.ullSize);
				if (ullVal > EBML_MAX_SIZE_LENGTH){
				}
				MMPF_MKV_Message("EBML_MAXSIZELENGTH: %lld\n", ullVal);
				break;
			case EBML_DOCTYPE:
				pDocType = MMPF_MKV_EBML_ReadString(pMkvFile, element.ullSize);
				MMPF_MKV_Message("EBML_DOCTYPE: %s\n", pDocType);
				break;
			case EBML_DOCTYPEVERSION:
				ullVal = MMPF_MKV_EBML_ReadUint(pMkvFile, (MMP_ULONG)element.ullSize);
				MMPF_MKV_Message("EBML_DOCTYPEVERSION: %lld\n", ullVal);
				break;
			case EBML_DOCTYPEREADVERSION:
				ullVal = MMPF_MKV_EBML_ReadUint(pMkvFile, (MMP_ULONG)element.ullSize);
				if (ullVal > MATROSKA_VERSION){
				}
				MMPF_MKV_Message("EBML_DOCTYPEREADVERSION: %lld\n", ullVal);
				break;
			default:
				MMPF_MKV_FileSkipBytes(pMkvFile, element.ullSize);
				MMPF_MKV_Message("unknown EBML header type:%d len:%lld\n", element.ulID, element.ullSize);
				break;
		}
	}
}

static void MMPF_MKV_ParseSegmentInfo(
	MMPF_MKV_CONTEXT *pMkvCtx,
	MMP_ULONG64 ullStart,
	MMP_ULONG64 ullLen
){
	MMPF_MKV_FILE *pMkvFile = pMkvCtx->pFile;
	MMPF_MKV_EBML_ELEMENT element;
	MMP_MKV_FLOAT fDuration = MMPF_MKV_FloatGet(0);

	while(MMPF_MKV_EBML_ReadElement(pMkvFile, &element, ullStart, ullLen)){
		switch(element.ulID){
			case MKV_SEG_UID:
				//useless
				//MMPF_MKV_FileReadBytes(pMkvFile, pMkvCtx->seg.pUID, element.ullSize);
				MMPF_MKV_FileSkipBytes(pMkvFile, element.ullSize);
				break;
			case MKV_SEG_FILENAME:
				pMkvCtx->seg.pFileName = MMPF_MKV_EBML_ReadString(pMkvFile, element.ullSize);
				MMPF_MKV_Message("MKV_SEG_FILENAME: %s\n", pMkvCtx->seg.pFileName );
				break;
			case MKV_PREV_UID:
				//useless
				//MMPF_MKV_FileReadBytes(pMkvFile, pMkvCtx->seg.pPrevUID, element.ullSize);
				MMPF_MKV_FileSkipBytes(pMkvFile, element.ullSize);
				break;
			case MKV_PREV_FILENAME:
				pMkvCtx->seg.pPrevFileName = MMPF_MKV_EBML_ReadString(pMkvFile, element.ullSize);
				MMPF_MKV_Message("MKV_PREV_FILENAME: %s\n", pMkvCtx->seg.pPrevFileName);
				break;
			case MKV_NEXT_UID:
				//useless
				//MMPF_MKV_FileReadBytes(pMkvFile, pMkvCtx->seg.pNextUID, element.ullSize);
				MMPF_MKV_FileSkipBytes(pMkvFile, element.ullSize);
				break;
			case MKV_NEXT_FILENAME:
				pMkvCtx->seg.pNextFileName = MMPF_MKV_EBML_ReadString(pMkvFile, element.ullSize);
				MMPF_MKV_Message("MKV_NEXT_FILENAME: %s\n", pMkvCtx->seg.pNextFileName);
				break;
			case MKV_TIMECODE_SCALE:
				pMkvCtx->seg.ullTimecodeScale = MMPF_MKV_EBML_ReadUint(pMkvFile, (MMP_ULONG)element.ullSize);
				MMPF_MKV_Message("MKV_TIMECODE_SCALE: %lld\n", pMkvCtx->seg.ullTimecodeScale);
				break;
			case MKV_DURATION:
				fDuration = MMPF_MKV_EBML_ReadFloat(pMkvFile, (MMP_ULONG)element.ullSize);
				MMPF_MKV_Message("MKV_DURATION %lf\n", fDuration);
				break;
			case MKV_TITLE:
				pMkvCtx->seg.pTitle = MMPF_MKV_EBML_ReadString(pMkvFile, element.ullSize);
				MMPF_MKV_Message("MKV_TITLE: %s\n", pMkvCtx->seg.pTitle);
				break;
			case MKV_MUXING_APP:
				pMkvCtx->seg.pMuxingApp = MMPF_MKV_EBML_ReadString(pMkvFile, element.ullSize);
				MMPF_MKV_Message("MKV_MUXING_APP: %s\n", pMkvCtx->seg.pMuxingApp);
				break;
			case MKV_WRITING_APP:
				pMkvCtx->seg.pWritingApp = MMPF_MKV_EBML_ReadString(pMkvFile, element.ullSize);
				MMPF_MKV_Message("MKV_WRITING_APP: %s\n", pMkvCtx->seg.pWritingApp);
				break;
			case MKV_DATEUTC:
				pMkvCtx->seg.ullDateUTC = MMPF_MKV_EBML_ReadUint(pMkvFile, (MMP_ULONG)element.ullSize);
				MMPF_MKV_Message("MKV_DATEUTC: %lld\n", pMkvCtx->seg.ullDateUTC);
				break;
			default:
				MMPF_MKV_Message("unknown segment_info element type:%X len:%llX %llX\n", element.ulID, element.ullSize, element.ullStart );
				MMPF_MKV_FileSkipBytes(pMkvFile, element.ullSize);
		}
	}
	
	pMkvCtx->seg.ullDuration = MMPF_MKV_FloatMult(fDuration, pMkvCtx->seg.ullTimecodeScale);
	MMPF_MKV_Message("MKV_DURATION: %lld \n", pMkvCtx->seg.ullDuration);
}

static void MMPF_MKV_ParseSeekEntry(
	MMPF_MKV_CONTEXT *pMkvCtx,
	MMP_ULONG64 ullStart,
	MMP_ULONG64 ullLen
){
	MMPF_MKV_FILE *pMkvFile = pMkvCtx->pFile;
	MMPF_MKV_EBML_ELEMENT element;
	MMP_ULONG ulSeekID;
	MMP_ULONG64 ullSeekPosition;

	while(MMPF_MKV_EBML_ReadElement(pMkvFile, &element, ullStart, ullLen)){
		switch(element.ulID){
			case MKV_SEEK_ID:
				//MMPF_MKV_Message("MKV_SEEK_ID len: %llX %llX\n", element.ullSize, element.ullStart );
				ulSeekID = (MMP_ULONG)MMPF_MKV_EBML_ReadUint(pMkvFile, (MMP_ULONG)element.ullSize);
				break;
			case MKV_SEEK_POSITION:
				//MMPF_MKV_Message("MKV_SEEK_POSITION len: %llX %llX\n", element.ullSize, element.ullStart );
				ullSeekPosition = MMPF_MKV_EBML_ReadUint(pMkvFile, (MMP_ULONG)element.ullSize);
				break;
			default:
				MMPF_MKV_Message("unknown seekhead element type:%X len:%llX %llX\n", element.ulID, element.ullSize, element.ullStart );
				MMPF_MKV_FileSkipBytes(pMkvFile, element.ullSize);			
		}
	}
	
	ullSeekPosition += pMkvCtx->ullSegPos;	//add segment start offset, since all addresses is relative to segment start address
	switch(ulSeekID){
		case MKV_SEG_INFO:
			MMPF_MKV_Message("%s MKV_SEG_INFO ", __func__);
			pMkvCtx->ullSegInfoPos = ullSeekPosition;
			break;
		case MKV_SEEK_HEAD:
			MMPF_MKV_Message("%s MKV_SEEK_HEAD ", __func__);
			if(pMkvCtx->ullSeekHeadPos == 0)
				pMkvCtx->ullSeekHeadPos = ullSeekPosition;
			break;
		case MKV_CLUSTER:
			MMPF_MKV_Message("%s MKV_CLUSTER ", __func__);
			if(pMkvCtx->ullClustersPos == 0)
				pMkvCtx->ullClustersPos = ullSeekPosition;
			break;
		case MKV_TRACKS:
			MMPF_MKV_Message("%s MKV_TRACKS ", __func__);
			if(pMkvCtx->ullTracksPos == 0)
				pMkvCtx->ullTracksPos = ullSeekPosition;
			break;
		case MKV_CUES:
			MMPF_MKV_Message("%s MKV_CUES ", __func__);
			pMkvCtx->ullCuesPos = ullSeekPosition;
			break;
		case MKV_ATTACHMENTS:
			MMPF_MKV_Message("%s MKV_ATTACHMENTS ", __func__);
			pMkvCtx->ullAttachmentsPos = ullSeekPosition;
			break;
		case MKV_CHAPTERS:
			MMPF_MKV_Message("%s MKV_CHAPTERS ", __func__);
			pMkvCtx->ullChaptersPos = ullSeekPosition;
			break;
		case MKV_TAGS:
			MMPF_MKV_Message("%s MKV_TAGS ", __func__);
			pMkvCtx->ullTagsPos = ullSeekPosition;
			break;
		default:
			MMPF_MKV_Message("unknown seekid:%X ", ulSeekID);
			break;
	}
	MMPF_MKV_Message("at position %llX\n", ullSeekPosition);
}

static void MMPF_MKV_ParseSeekHead(
	MMPF_MKV_CONTEXT *pMkvCtx,
	MMP_ULONG64 ullStart,
	MMP_ULONG64 ullLen
){
	MMPF_MKV_FILE *pMkvFile = pMkvCtx->pFile;
	MMPF_MKV_EBML_ELEMENT element;
	while(MMPF_MKV_EBML_ReadElement(pMkvFile, &element, ullStart, ullLen)){
		switch(element.ulID){
			case MKV_SEEK:
				MMPF_MKV_Message("MKV_SEEK len: %llX %llX\n", element.ullSize, element.ullStart );
				//MMPF_MKV_FileSkipBytes(pMkvCtx, element.ullSize);
				MMPF_MKV_ParseSeekEntry(pMkvCtx, element.ullDataPos, element.ullSize);
				break;
			default:
				//error?
				MMPF_MKV_Message("unknown seekhead element type:%X len:%llX %llX\n", element.ulID, element.ullSize, element.ullStart );
				MMPF_MKV_FileSkipBytes(pMkvFile, element.ullSize);
		}
	}
}

static void MMPF_MKV_ParseTrackVideoInfo(
	MMPF_MKV_CONTEXT *pMkvCtx,
	MMPF_MKV_TRACK *pTrack,
	MMP_ULONG64 ullStart,
	MMP_ULONG64 ullLen
){
	MMPF_MKV_FILE *pMkvFile = pMkvCtx->pFile;
	MMPF_MKV_EBML_ELEMENT element;
	
	pTrack->AV.VIDEO.ulCropB = 0;
	pTrack->AV.VIDEO.ulCropT = 0;
	pTrack->AV.VIDEO.ulCropL = 0;
	pTrack->AV.VIDEO.ulCropR = 0;
	
	while(MMPF_MKV_EBML_ReadElement(pMkvFile, &element, ullStart, ullLen)){
		switch(element.ulID){
			case MKV_VIDEO_PIXEL_WIDTH:
				pTrack->AV.VIDEO.ulPixelWidth = MMPF_MKV_EBML_ReadUint(pMkvFile, (MMP_ULONG)element.ullSize);
				if(pTrack->AV.VIDEO.ulDisplayWidth == 0)
					pTrack->AV.VIDEO.ulDisplayWidth = pTrack->AV.VIDEO.ulPixelWidth;
				MMPF_MKV_Message("MKV_VIDEO_PIXEL_WIDTH %d\n", pTrack->AV.VIDEO.ulPixelWidth);
				break;
			case MKV_VIDEO_PIXEL_HEIGHT:
				pTrack->AV.VIDEO.ulPixelHeight = MMPF_MKV_EBML_ReadUint(pMkvFile, (MMP_ULONG)element.ullSize);
				if(pTrack->AV.VIDEO.ulDisplayHeight == 0)
					pTrack->AV.VIDEO.ulDisplayHeight = pTrack->AV.VIDEO.ulPixelHeight;
				MMPF_MKV_Message("MKV_VIDEO_PIXEL_HEIGHT %d\n", pTrack->AV.VIDEO.ulPixelHeight);
				break;
			case MKV_VIDEO_PIXEL_CROP_BUTTOM:
				pTrack->AV.VIDEO.ulCropB = MMPF_MKV_EBML_ReadUint(pMkvFile, (MMP_ULONG)element.ullSize);
				MMPF_MKV_Message("MKV_VIDEO_PIXEL_CROP_BUTTOM %d\n", pTrack->AV.VIDEO.ulCropB);
				break;
			case MKV_VIDEO_PIXEL_CROP_TOP:
				pTrack->AV.VIDEO.ulCropT = MMPF_MKV_EBML_ReadUint(pMkvFile, (MMP_ULONG)element.ullSize);
				MMPF_MKV_Message("MKV_VIDEO_PIXEL_CROP_TOP %d\n", pTrack->AV.VIDEO.ulCropT);
				break;
			case MKV_VIDEO_PIXEL_CROP_LEFT:
				pTrack->AV.VIDEO.ulCropL = MMPF_MKV_EBML_ReadUint(pMkvFile, (MMP_ULONG)element.ullSize);
				MMPF_MKV_Message("MKV_VIDEO_PIXEL_CROP_LEFT %d\n", pTrack->AV.VIDEO.ulCropL);
				break;
			case MKV_VIDEO_PIXEL_CROP_RIGHT:
				pTrack->AV.VIDEO.ulCropR = MMPF_MKV_EBML_ReadUint(pMkvFile, (MMP_ULONG)element.ullSize);
				MMPF_MKV_Message("MKV_VIDEO_PIXEL_CROP_RIGHT %d\n", pTrack->AV.VIDEO.ulCropR);
				break;
			case MKV_VIDEO_DISPLAY_WIDTH:
				pTrack->AV.VIDEO.ulDisplayWidth = MMPF_MKV_EBML_ReadUint(pMkvFile, (MMP_ULONG)element.ullSize);
				MMPF_MKV_Message("MKV_VIDEO_DISPLAY_WIDTH %d\n", pTrack->AV.VIDEO.ulDisplayWidth);
				break;
			case MKV_VIDEO_DISPLAY_HEIGHT:
				pTrack->AV.VIDEO.ulDisplayHeight = MMPF_MKV_EBML_ReadUint(pMkvFile, (MMP_ULONG)element.ullSize);
				MMPF_MKV_Message("MKV_VIDEO_DISPLAY_HEIGHT %d\n", pTrack->AV.VIDEO.ulDisplayHeight);
				break;
			case MKV_VIDEO_DISPLAY_UNIT:
				pTrack->AV.VIDEO.ulDisplayUnit = MMPF_MKV_EBML_ReadUint(pMkvFile, (MMP_ULONG)element.ullSize);
				MMPF_MKV_Message("MKV_VIDEO_DISPLAY_UNIT %X\n", pTrack->AV.VIDEO.ulDisplayUnit);
				break;
			case MKV_VIDEO_FLAG_INTERLACED:
				pTrack->AV.VIDEO.ulInterlaced = MMPF_MKV_EBML_ReadUint(pMkvFile, (MMP_ULONG)element.ullSize);
				MMPF_MKV_Message("MKV_VIDEO_FLAG_INTERLACED %X\n", pTrack->AV.VIDEO.ulInterlaced);
				break;
			case MKV_VIDEO_STEREO_MODE:
				pTrack->AV.VIDEO.ulStereoMode = MMPF_MKV_EBML_ReadUint(pMkvFile, (MMP_ULONG)element.ullSize);
				MMPF_MKV_Message("MKV_VIDEO_STEREO_MODE %X\n", pTrack->AV.VIDEO.ulStereoMode);
				break;
			case MKV_VIDEO_ASPECT_RATIO:
				pTrack->AV.VIDEO.ulAspectRatioType = MMPF_MKV_EBML_ReadUint(pMkvFile, (MMP_ULONG)element.ullSize);
				MMPF_MKV_Message("MKV_VIDEO_ASPECT_RATIO %X\n", pTrack->AV.VIDEO.ulAspectRatioType);
				break;
			case MKV_VIDEO_GAMMA_VALUE:
				pTrack->AV.VIDEO.fGammaValue = MMPF_MKV_EBML_ReadFloat(pMkvFile, (MMP_ULONG)element.ullSize);
				MMPF_MKV_Message("MKV_VIDEO_GAMMA_VALUE %lf\n", pTrack->AV.VIDEO.fGammaValue);
				break;
			case MKV_VIDEO_COLORSPACE:
				pTrack->AV.VIDEO.ulColorSpace = MMPF_MKV_EBML_ReadUint(pMkvFile, (MMP_ULONG)element.ullSize);
				MMPF_MKV_Message("MKV_VIDEO_COLORSPACE %X\n", pTrack->AV.VIDEO.ulColorSpace);
				break;
			default:
				//error?
				MMPF_MKV_Message("unknown track video info element type:%X len:%llX %llX\n", element.ulID, element.ullSize, element.ullStart );
				MMPF_MKV_FileSkipBytes(pMkvFile, element.ullSize);		
		}
	}
}

static void MMPF_MKV_ParseTrackAudioInfo(
	MMPF_MKV_CONTEXT *pMkvCtx,
	MMPF_MKV_TRACK *pTrack,
	MMP_ULONG64 ullStart,
	MMP_ULONG64 ullLen
){
	MMPF_MKV_FILE *pMkvFile = pMkvCtx->pFile;
	MMPF_MKV_EBML_ELEMENT element;
	
	pTrack->AV.AUDIO.fSamplingFreq = MMPF_MKV_FloatGet(8000);
	pTrack->AV.AUDIO.ulChannels = 1;
	
	while(MMPF_MKV_EBML_ReadElement(pMkvFile, &element, ullStart, ullLen)){
		switch(element.ulID){
			case MKV_AUDIO_SAMPLING_FREQ:
				pTrack->AV.AUDIO.fSamplingFreq = MMPF_MKV_EBML_ReadFloat(pMkvFile, (MMP_ULONG)element.ullSize);
				MMPF_MKV_Message("MKV_AUDIO_SAMPLING_FREQ %lf\n", pTrack->AV.AUDIO.fSamplingFreq);
				break;
			case MKV_AUDIO_OUTPUT_SAMPLING_FREQ:
				pTrack->AV.AUDIO.fOutputSamplingFreq = MMPF_MKV_EBML_ReadFloat(pMkvFile, (MMP_ULONG)element.ullSize);
				MMPF_MKV_Message("MKV_AUDIO_OUTPUT_SAMPLING_FREQ %lf\n", pTrack->AV.AUDIO.fOutputSamplingFreq);
				break;
			case MKV_AUDIO_CHANNELS:
				pTrack->AV.AUDIO.ulChannels = MMPF_MKV_EBML_ReadUint(pMkvFile, (MMP_ULONG)element.ullSize);
				MMPF_MKV_Message("MKV_AUDIO_CHANNELS %d\n", pTrack->AV.AUDIO.ulChannels);
				break;
			case MKV_AUDIO_BIT_DEPTH:
				pTrack->AV.AUDIO.ulBitDepth = MMPF_MKV_EBML_ReadUint(pMkvFile, (MMP_ULONG)element.ullSize);
				MMPF_MKV_Message("MKV_AUDIO_BIT_DEPTH %d\n", pTrack->AV.AUDIO.ulBitDepth);
				break;
			default:
				//error?
				MMPF_MKV_Message("unknown track audio info element type:%X len:%llX %llX\n", element.ulID, element.ullSize, element.ullStart );
				MMPF_MKV_FileSkipBytes(pMkvFile, element.ullSize);		
		}
	}
}

static void MMPF_MKV_ParseTrackEntry(
	MMPF_MKV_CONTEXT *pMkvCtx,
	MMP_ULONG64 ullStart,
	MMP_ULONG64 ullLen
){
	MMPF_MKV_FILE *pMkvFile = pMkvCtx->pFile;
	MMPF_MKV_EBML_ELEMENT element;
	MMPF_MKV_TRACK *pTrack = &(pMkvCtx->tracks[pMkvCtx->ullNumTracks++]);

	
	//MMPF_MKV_TRACK* pTrack = mkv_osci_mem_alloc(sizeof(MMPF_MKV_TRACK));
	MEMSET0( &(pMkvCtx->tracks[pMkvCtx->ullNumTracks]) );
	
	pTrack->ulFlagEnabled = 1;
	pTrack->ulFlagDefault = 1;
	pTrack->ulFlagForced = 0;
	pTrack->ulFlagLacing = 0;
	pTrack->ullMinCache = 0;
	pTrack->fTimecodeScale = MMPF_MKV_FloatGet(1);
	
	if( (pMkvCtx->ullNumTracks+1) == MKV_MAX_TRACKS){
		//error
	}

	while(MMPF_MKV_EBML_ReadElement(pMkvFile, &element, ullStart, ullLen)){
		switch(element.ulID){
			case MKV_TRACK_NUMBER:
				pTrack->ullNumber = MMPF_MKV_EBML_ReadUint(pMkvFile, (MMP_ULONG)element.ullSize);
				MMPF_MKV_Message("MKV_TRACK_NUMBER %llX %llX %llX\n", pTrack->ullNumber, element.ullSize, element.ullStart);
				break;
			case MKV_TRACK_UID:
				pTrack->ullUID = MMPF_MKV_EBML_ReadUint(pMkvFile, (MMP_ULONG)element.ullSize);
				MMPF_MKV_Message("MKV_TRACK_UID %llX %llX %llX\n", pTrack->ullUID, element.ullSize, element.ullStart);
				break;
			case MKV_TRACK_TYPE:
				pTrack->ullType = MMPF_MKV_EBML_ReadUint(pMkvFile, (MMP_ULONG)element.ullSize);
				MMPF_MKV_Message("MKV_TRACK_TYPE %llX %llX %llX\n", pTrack->ullType, element.ullSize, element.ullStart);
				break;
			case MKV_TRACK_FLAG_ENABLED:
				pTrack->ulFlagEnabled = (MMP_ULONG)MMPF_MKV_EBML_ReadUint(pMkvFile, (MMP_ULONG)element.ullSize);
				MMPF_MKV_Message("MKV_TRACK_FLAG_ENABLED %X %llX %llX\n",  pTrack->ulFlagEnabled, element.ullSize, element.ullStart);
				break;
			case MKV_TRACK_FLAG_DEFAULT:
				pTrack->ulFlagDefault = MMPF_MKV_EBML_ReadUint(pMkvFile, (MMP_ULONG)element.ullSize);
				MMPF_MKV_Message("MKV_TRACK_FLAG_DEFAULT %X %llX %llX\n", pTrack->ulFlagDefault, element.ullSize, element.ullStart);
				break;
			case MKV_TRACK_FLAG_LACING:
				pTrack->ulFlagLacing = MMPF_MKV_EBML_ReadUint(pMkvFile, (MMP_ULONG)element.ullSize);
				MMPF_MKV_Message("MKV_TRACK_FLAG_LACING %X %llX %llX\n", pTrack->ulFlagLacing, element.ullSize, element.ullStart);
				break;
			case MKV_TRACK_FLAG_FORCED:
				pTrack->ulFlagForced = MMPF_MKV_EBML_ReadUint(pMkvFile, (MMP_ULONG)element.ullSize);
				MMPF_MKV_Message("MKV_TRACK_FLAG_FORCED %X %llX %llX\n", pTrack->ulFlagForced, element.ullSize, element.ullStart);
				break;
			case MKV_TRACK_MIN_CACHE:
				pTrack->ullMinCache = MMPF_MKV_EBML_ReadUint(pMkvFile, (MMP_ULONG)element.ullSize);
				MMPF_MKV_Message("MKV_TRACK_MIN_CACHE %llX %llX %llX\n", pTrack->ullMinCache, element.ullSize, element.ullStart);
				break;
			case MKV_TRACK_MAX_CACHE:
				pTrack->ullMaxCache = MMPF_MKV_EBML_ReadUint(pMkvFile, (MMP_ULONG)element.ullSize);
				MMPF_MKV_Message("MKV_TRACK_MAX_CACHE %llX %llX %llX\n", pTrack->ullMaxCache, element.ullSize, element.ullStart);
				break;
			case MKV_TRACK_DEFAULT_DURATION:
				pTrack->ullDefaultDuration = MMPF_MKV_EBML_ReadUint(pMkvFile, (MMP_ULONG)element.ullSize);
				MMPF_MKV_Message("MKV_TRACK_DEFAULT_DURATION %lld %llX %llX\n", pTrack->ullDefaultDuration, element.ullSize, element.ullStart);
				break;
			case MKV_TRACK_TIMECODE_SCALE:
				pTrack->fTimecodeScale = MMPF_MKV_EBML_ReadFloat(pMkvFile, (MMP_ULONG)element.ullSize);
				MMPF_MKV_Message("MKV_TRACK_TIMECODE_SCALE(double): %lf %llX %llX\n", pTrack->fTimecodeScale, element.ullSize, element.ullStart);
				break;
			case MKV_TRACK_NAME:
				pTrack->pName = MMPF_MKV_EBML_ReadString(pMkvFile, element.ullSize);
				MMPF_MKV_Message("MKV_TRACK_NAME: %s %llX %llX\n", pTrack->pName, element.ullSize, element.ullStart);
				break;
			case MKV_TRACK_LANGUAGE:
				MMPF_MKV_EBML_ReadLangCC(pMkvFile, element.ullSize, pTrack->pLanguage);
				break;
			case MKV_TRACK_CODEC_ID:
				pTrack->pCodecID = MMPF_MKV_EBML_ReadString(pMkvFile, element.ullSize);
				MMPF_MKV_Message("MKV_TRACK_CODEC_ID: %s %llX %llX\n", pTrack->pCodecID, element.ullSize, element.ullStart);
				break;
			case MKV_TRACK_CODEC_PRIVATE:
				pTrack->ullCodecPrivatePos = element.ullDataPos;
				pTrack->ulCodecPrivateSize = element.ullSize;
				MMPF_MKV_Message("MKV_TRACK_CODEC_PRIVATE len:%llX %llX\n", element.ullSize, element.ullStart );
				MMPF_MKV_FileSkipBytes(pMkvFile, element.ullSize);
				break;
			case MKV_TRACK_CODEC_NAME:
				pTrack->pCodecName = MMPF_MKV_EBML_ReadString(pMkvFile, element.ullSize);
				MMPF_MKV_Message("MKV_TRACK_CODEC_NAME %s %llX %llX\n", pTrack->pCodecName , element.ullSize, element.ullStart);
				break;
			case MKV_TRACK_ATTACH_LINK:
				MMPF_MKV_Message("MKV_TRACK_ATTACH_LINK len:%llX %llX\n", element.ullSize, element.ullStart );
				MMPF_MKV_FileSkipBytes(pMkvFile, element.ullSize);			
				break;
			case MKV_TRACK_VIDEO_INFO:
				MMPF_MKV_Message("MKV_TRACK_VIDEO_INFO len:%llX %llX\n", element.ullSize, element.ullStart );
				//MMPF_MKV_FileSkipBytes(pMkvCtx, element.ullSize);			
				MMPF_MKV_ParseTrackVideoInfo(pMkvCtx, pTrack, element.ullDataPos, element.ullSize);
				break;
			case MKV_TRACK_AUDIO_INFO:
				MMPF_MKV_Message("MKV_TRACK_AUDIO_INFO len:%llX %llX\n", element.ullSize, element.ullStart );
				//MMPF_MKV_FileSkipBytes(pMkvCtx, element.ullSize);			
				MMPF_MKV_ParseTrackAudioInfo(pMkvCtx, pTrack, element.ullDataPos, element.ullSize);
				break;
			case MKV_TRACK_CONTENT_ENCODINGS:
				//TODO
				MMPF_MKV_Message("MKV_TRACK_CONTENT_ENCODINGS len:%llX %llX\n", element.ullSize, element.ullStart );
				MMPF_MKV_FileSkipBytes(pMkvFile, element.ullSize);			
				break;
			case MKV_TRACK_CODEC_DECODEALL:
				pTrack->ulDecodeAll = MMPF_MKV_EBML_ReadUint(pMkvFile, (MMP_ULONG)element.ullSize);
				MMPF_MKV_Message("MKV_TRACK_CODEC_DECODEALL %X %llX %llX\n", pTrack->ulDecodeAll, element.ullSize, element.ullStart);
				break;
			case MKV_TRACK_MAX_BLK_ADDID:
				pTrack->ulMaxBlockAdditionID = MMPF_MKV_EBML_ReadUint(pMkvFile, (MMP_ULONG)element.ullSize);
				MMPF_MKV_Message("MKV_TRACK_MAX_BLK_ADDID %X %llX %llX\n", pTrack->ulMaxBlockAdditionID, element.ullSize, element.ullStart);
				break;
			default:
				//error?
				MMPF_MKV_Message("unknown trackentry element type:%X len:%llX %llX\n", element.ulID, element.ullSize, element.ullStart );
				MMPF_MKV_FileSkipBytes(pMkvFile, element.ullSize);			
		}
	}
	
}

static void MMPF_MKV_ParseTracks(
	MMPF_MKV_CONTEXT *pMkvCtx,
	MMP_ULONG64 ullStart,
	MMP_ULONG64 ullLen
){
	MMPF_MKV_FILE *pMkvFile = pMkvCtx->pFile;
	MMPF_MKV_EBML_ELEMENT element;
	while(MMPF_MKV_EBML_ReadElement(pMkvFile, &element, ullStart, ullLen)){
		switch(element.ulID){
			case MKV_TRACK_ENTRY:
				MMPF_MKV_Message("MKV_TRACK_ENTRY len: %llX %llX\n", element.ullSize, element.ullStart );
				MMPF_MKV_ParseTrackEntry(pMkvCtx, element.ullDataPos, element.ullSize);
				break;
			default:
				//error?
				MMPF_MKV_Message("unknown tracks element type:%X len:%llX %llX\n", element.ulID, element.ullSize, element.ullStart );
				MMPF_MKV_FileSkipBytes(pMkvFile, element.ullSize);			
		}
	}
}

//try to parse all data excepts clusters
static void MMPF_MKV_ParseSegment(
	MMPF_MKV_CONTEXT *pMkvCtx,
	MMP_ULONG64 ullStart,
	MMP_ULONG64 ullLen
){
	MMP_UBYTE ubClusterReached = 0;
	MMPF_MKV_FILE *pMkvFile = pMkvCtx->pFile;
	MMPF_MKV_EBML_ELEMENT element;

	MMPF_MKV_Message("%s start %llX len %llX\n", __func__, ullStart, ullLen);

	//TODO: check position
	while(MMPF_MKV_EBML_ReadElement(pMkvFile, &element, ullStart, ullLen) && ubClusterReached == 0){
		switch(element.ulID){
			case MKV_SEG_INFO:
				MMPF_MKV_Message("MKV_SEG_INFO len: %llX %llX\n", element.ullSize, element.ullStart );
				if(pMkvCtx->ullSegInfoPos == 0)
					pMkvCtx->ullTracksPos = element.ullStart;
				MMPF_MKV_ParseSegmentInfo(pMkvCtx, element.ullDataPos, element.ullSize);
				break;
			case MKV_SEEK_HEAD:
				MMPF_MKV_Message("MKV_SEEK_HEAD len: %llX %llX\n", element.ullSize, element.ullStart );
				MMPF_MKV_ParseSeekHead(pMkvCtx, element.ullDataPos, element.ullSize);
				break;
			case MKV_CLUSTER:
				//keep position and parse on demand
				if(pMkvCtx->ullClustersPos == 0)
					pMkvCtx->ullClustersPos = element.ullStart;
				MMPF_MKV_Message("MKV_CLUSTER len: %llX %llX\n", element.ullSize, element.ullStart );
				//MMPF_MKV_FileSkipBytes(pMkvCtx, element.ullSize);
				//MMPF_MKV_ParseCluster(pMkvCtx, pMkvFile, element.ullDataPos, element.ullSize);
				ubClusterReached = 1;	//break;
				break;
			case MKV_TRACKS:
				if(pMkvCtx->ullTracksPos == 0)
					pMkvCtx->ullTracksPos = element.ullDataPos;
				MMPF_MKV_Message("MKV_TRACKS len: %llX %llX\n", element.ullSize, element.ullStart );
				MMPF_MKV_ParseTracks(pMkvCtx, element.ullDataPos, element.ullSize);
				break;
			case MKV_CUES:
				if(pMkvCtx->ullCuesPos == 0)
					pMkvCtx->ullCuesPos = element.ullStart;
				MMPF_MKV_Message("MKV_CUES len: %llX %llX\n", element.ullSize, element.ullStart );
				MMPF_MKV_FileSkipBytes(pMkvFile, element.ullSize);
				//ubClusterReached = 1;	//break;
				break;
			case MKV_ATTACHMENTS:
				if(pMkvCtx->ullAttachmentsPos == 0)
					pMkvCtx->ullAttachmentsPos = element.ullStart;
				MMPF_MKV_Message("MKV_ATTACHMENTS len: %llX %llX\n", element.ullSize, element.ullStart );
				MMPF_MKV_FileSkipBytes(pMkvFile, element.ullSize);
				break;
			case MKV_CHAPTERS:
				if(pMkvCtx->ullChaptersPos == 0)
					pMkvCtx->ullChaptersPos = element.ullStart;
				MMPF_MKV_Message("MKV_CHAPTERS len: %llX %llX...........................\n", element.ullSize, element.ullStart );
				MMPF_MKV_FileSkipBytes(pMkvFile, element.ullSize);
				break;
			case MKV_TAGS:
				if(pMkvCtx->ullTagsPos == 0)
					pMkvCtx->ullTagsPos = element.ullStart;
				MMPF_MKV_Message("MKV_TAGS len: %llX %llX\n", element.ullSize, element.ullStart );
				MMPF_MKV_FileSkipBytes(pMkvFile, element.ullSize);
				break;
			default:
				MMPF_MKV_Message("unknown segment element type:%X len:%llX %llX.....................\n", element.ulID, element.ullSize, element.ullStart );
				MMPF_MKV_FileSkipBytes(pMkvFile, element.ullSize);
		}
	}
	
	//Here we assume that SegmentInfo, SeekHead, Tracks are parsed before Clusters
	pMkvCtx->ullClustersSize = ullLen - pMkvCtx->ullClustersPos;
	if( pMkvCtx->ullCuesPos > 0 ){
		pMkvCtx->ullCuesSize = ullLen - pMkvCtx->ullCuesPos;
		if((pMkvCtx->ullCuesPos - pMkvCtx->ullClustersPos) < pMkvCtx->ullClustersSize)
			pMkvCtx->ullClustersSize = (pMkvCtx->ullCuesPos - pMkvCtx->ullClustersPos);
	}
	if( pMkvCtx->ullAttachmentsPos > 0){
		if((pMkvCtx->ullAttachmentsPos - pMkvCtx->ullClustersPos) < pMkvCtx->ullClustersSize)
			pMkvCtx->ullClustersSize = (pMkvCtx->ullAttachmentsPos - pMkvCtx->ullClustersPos);
		if( pMkvCtx->ullCuesPos > 0 && (pMkvCtx->ullAttachmentsPos - pMkvCtx->ullCuesPos) < pMkvCtx->ullCuesSize)
			pMkvCtx->ullCuesSize = (pMkvCtx->ullAttachmentsPos - pMkvCtx->ullCuesPos);
	}
	if( pMkvCtx->ullTagsPos > 0) {
		if((pMkvCtx->ullTagsPos - pMkvCtx->ullClustersPos) < pMkvCtx->ullClustersSize)
			pMkvCtx->ullClustersSize = (pMkvCtx->ullTagsPos - pMkvCtx->ullClustersPos);
		if( pMkvCtx->ullCuesPos > 0 && (pMkvCtx->ullTagsPos - pMkvCtx->ullCuesPos) < pMkvCtx->ullCuesSize)
			pMkvCtx->ullCuesSize = (pMkvCtx->ullTagsPos - pMkvCtx->ullCuesPos);
	}
	if( pMkvCtx->ullChaptersPos > 0 ){ 
		if((pMkvCtx->ullChaptersPos - pMkvCtx->ullClustersPos) < pMkvCtx->ullClustersSize)
			pMkvCtx->ullClustersSize = (pMkvCtx->ullChaptersPos - pMkvCtx->ullClustersPos);
		if( pMkvCtx->ullCuesPos > 0 && (pMkvCtx->ullChaptersPos - pMkvCtx->ullCuesPos) < pMkvCtx->ullCuesSize)
			pMkvCtx->ullCuesSize = (pMkvCtx->ullChaptersPos - pMkvCtx->ullCuesPos);
	}

	MMPF_MKV_Message("%s clusters_pos:%llX clusters_size:%llX cues_size:%llX\n", __func__, pMkvCtx->ullClustersPos, pMkvCtx->ullClustersSize, pMkvCtx->ullCuesSize);
}

//to parse mkv headers
static MMP_LONG MMPF_MKV_Parse(
	MMPF_MKV_CONTEXT *pMkvCtx
){
	MMP_UBYTE ubSegmentParsed = 0, ubEBMLParsed = 0;
	MMPF_MKV_FILE *pMkvFile = pMkvCtx->pFile;
	MMP_ULONG64 ullStart = MMPF_MKV_FilePosition(pMkvFile);
	MMP_LONG64 llLen = MMPF_MKV_FileSize(pMkvFile);
	MMPF_MKV_EBML_ELEMENT element;

	if(llLen <= 0){
		//Error
		return -1;
	}
	
	MMPF_MKV_Message("%s mkv file size:%lld\n", __func__, llLen);

	while(MMPF_MKV_EBML_ReadElement(pMkvFile, &element, ullStart, llLen) && ubSegmentParsed == 0){
		switch(element.ulID){
			case EBML_ID:
				MMPF_MKV_Message("ebml header len: %llX\n", element.ullSize);
				MMPF_MKV_ParseEBMLHeader(pMkvCtx->pFile, element.ullDataPos, element.ullSize);
				ubEBMLParsed = 1;
				break;
			case MKV_SEGMENT:
				pMkvCtx->ullSegPos = element.ullDataPos;		//keep start address of segment's data
				MMPF_MKV_Message("segment len: %llX\n", element.ullSize);
				//MMPF_MKV_FileSkipBytes(pMkvCtx, element.ullSize);
				MMPF_MKV_ParseSegment(pMkvCtx, element.ullDataPos, element.ullSize);
				ubSegmentParsed = 1;
				break;
			default:
				MMPF_MKV_Message("unknown type:%X len:%llX\n", element.ulID, element.ullSize);
				if(!ubEBMLParsed)
				    return -1;
				ubSegmentParsed = 1;
				break;
		}
	}
	
	return 0;	//return -1 when error occurs
}

#ifdef BUILD_FW

#if 0 // for IDE function name list by section
void _____MKV_Track_Operations_API_____(){}
#endif

#if (CHIP == P_V2)
static MMP_ULONG m_H264LenByte;
static MMP_ULONG m_H264ReadByte = 0;
#endif

//return the size of codec private, and codec private is copy to destination buffer
static MMP_LONG MMPF_MKV_TrackOpen(
	MMPF_MKV_CONTEXT *pMkvCtx,
	MMPF_MKV_TRACK *pTrack,
	MMP_BYTE *pBuf,
	MMP_ULONG ulBuflen
){
    #if (CHIP == P_V2)
    MMP_UBYTE   *buf = (MMP_UBYTE*)pBuf;
    MMP_UBYTE   temp;
    MMP_ULONG   sps_num,pps_num,size, file_offset,tmp_length;
    MMP_ULONG   i;
    #endif
    
	pTrack->pMkvFile = MMPF_MKV_FileOpen();
	pTrack->pBSFile = MMPF_MKV_FileOpen();
	
	if(pTrack->ulCodecPrivateSize > 0 && pTrack->ullCodecPrivatePos > 0){
		MMPF_MKV_FileSeek(pTrack->pMkvFile, pTrack->ullCodecPrivatePos);
		
		#if (CHIP == MCR_V2)
		MMPF_MKV_FileReadBytes(pTrack->pMkvFile, pBuf, pTrack->ulCodecPrivateSize);
		#endif
		//swyang
		#if (CHIP == P_V2)
		if(strncmp(pTrack->pCodecID, "V_MPEG4/ISO/AVC", 15) == 0){
		    tmp_length = 0;
		    
		    MMPF_MKV_FileReadBytes(pTrack->pMkvFile, buf, 8);
		    
		    buf += 4;
		    temp = *buf;
		    
		    m_H264LenByte = (temp&0x3) + 1; // NALU Length size
            buf++;
            
            sps_num = *(buf) & 0x1F;
            buf++;
            
            size = ((*buf) << 8) + (*(buf + 1));
            buf += 2;
            
            VAR_L(3, m_H264LenByte);
            VAR_L(3, sps_num);
            VAR_L(3, size);
            
            buf = (MMP_UBYTE*)pBuf;
                            
            buf[0] = 0x00; buf[1] = 0x00; buf[2] = 0x00; buf[3] = 0x01;
            
            tmp_length = size + 4;
            
            MMPF_VIDPSR_ReadFile( buf+4, (MMP_ULONG)pTrack->ullCodecPrivatePos + 8, size);
            
            buf += (size+4);
            
            file_offset = pTrack->ullCodecPrivatePos + 8 + size;
            
            //just leave the rest SPS data for avc_decode_nalu()
            for(i=1;i<sps_num;i++)
            {
                MMPF_VIDPSR_ReadFile(buf, file_offset, 2);
                
                file_offset += 2;
                    
                size = ((*buf) << 8) + (*(buf + 1));
                
                MMPF_VIDPSR_ReadFile(buf, file_offset, size);
                
                file_offset += size;
            }
            
            MMPF_VIDPSR_ReadFile(buf, file_offset, 1);
                    
            pps_num     = *(buf);
            file_offset += 1;
            
            MMPF_VIDPSR_ReadFile(buf, file_offset, 2);
                
            size = ((*buf) << 8) + (*(buf + 1));
            file_offset += 2;
            
            tmp_length += (size + 4);
            
            buf[0] = 0x00; buf[1] = 0x00; buf[2] = 0x00; buf[3] = 0x01;
            
            MMPF_VIDPSR_ReadFile(buf+4, file_offset, size);
            
            pTrack->pMkvFile->ulFilePos += pTrack->ulCodecPrivateSize;
		}
		else {
		    MMPF_MKV_FileReadBytes(pTrack->pMkvFile, pBuf, pTrack->ulCodecPrivateSize);
		}
		#endif
	}

	MMPF_MKV_FileSeek(pTrack->pMkvFile, pMkvCtx->ullClustersPos);
	MMPF_MKV_FileSeek(pTrack->pBSFile, pMkvCtx->ullClustersPos);
	MMPF_MKV_Message("%s: %llX\n", __func__, MMPF_MKV_FilePosition(pTrack->pMkvFile) );

    #if (CHIP == MCR_V2)
	return pTrack->ulCodecPrivateSize;
	#endif
	#if (CHIP == P_V2)
    if(strncmp(pTrack->pCodecID, "V_MPEG4/ISO/AVC", 15) == 0){
        return tmp_length;
    }
    else {
        return pTrack->ulCodecPrivateSize;
    }
    #endif
}

MMP_LONG MMPF_MKV_TrackClose(
	MMPF_MKV_CONTEXT *pMkvCtx,
	MMPF_MKV_TRACK *pTrack
){
	MMPF_MKV_FileClose(pTrack->pMkvFile);
	pTrack->pMkvFile = NULL;
	return 0;
}

MMP_ULONG MMPF_MKV_TrackGetTime(
	MMPF_MKV_CONTEXT *pMkvCtx,
	MMPF_MKV_TRACK *pTrack
){
	return (MMP_ULONG)(( MMPF_MKV_FloatMult(pTrack->fTimecodeScale, pTrack->ullTrackTime + pTrack->ullTrackTimeOffset) * (pMkvCtx->seg.ullTimecodeScale))/1000000);
}

MMP_LONG MMPF_MKV_TrackReadFrame(
	MMPF_MKV_CONTEXT *pMkvCtx,
	MMPF_MKV_TRACK *pTrack,
	MMP_UBYTE *pBuf,
	MMP_ULONG ulRead
){
	MMPF_MKV_FILE *pFile = pTrack->pBSFile;	
	MMPF_MKV_FileSeek(pFile, pTrack->ullFramePos);
	MMPF_MKV_FileReadBytes(pFile, pBuf, ulRead);
	pTrack->ullFramePos = MMPF_MKV_FilePosition(pFile);	//for large frame, may read several times
	return ulRead;
}


//get a frame of specified track. otherwise, skip the frame
MMP_LONG MMPF_MKV_TrackSeekFrame(
	MMPF_MKV_CONTEXT *pMkvCtx,
	MMPF_MKV_TRACK *pTrack,
	MMP_ULONG64 start,
	MMP_ULONG64 len,
	MMP_UBYTE   *ubKeyFrame,
	MMP_ULONG ulID
){
	MMPF_MKV_FILE *pMkvFile = pTrack->pMkvFile;
	MMP_ULONG64 ullTrack;
	MMP_LONG timecode;
	MMP_UBYTE flags;
	MMP_UBYTE framecnt;
	MMP_ULONG64 data_len;
	MMP_ULONG *pLacingSize = pTrack->ulLacingSize;

	MMP_LONG m, i;

	ullTrack = MMPF_MKV_EBML_ReadVuint(pMkvFile, &m, 0);
	
	if(ullTrack != pTrack->ullNumber){
		MMPF_MKV_Message("%s 1 %llx %lx\n", __func__, len, m);
		MMPF_MKV_FileSkipBytes(pMkvFile, len - (MMP_ULONG64)(m+1));
		return 0;
	}
	
	pTrack->ulFrameIndex++;
	
	timecode = MMPF_MKV_EBML_ReadSint(pMkvFile, 2);
	flags = MMPF_MKV_EBML_ReadUint(pMkvFile, 1);
	MMPF_MKV_Message("%s block track #: %lld timecode %d flags: %X\n", __func__, ullTrack, timecode, flags);
	
	if((flags & 0x80) && (pTrack->bUseflag == 0)){
	    pTrack->bUseflag = 1;
	}
	
    if(ulID == MKV_CLUSTER_SIMPLE_BLOCK || pTrack->bUseflag) {
        *ubKeyFrame = (flags & 0x80) >> 7;
    }
    
	data_len = 0;
	switch((flags & 0x06) >> 1){
		case MKV_LACING_FIXED:
			framecnt = MMPF_MKV_EBML_ReadUint(pMkvFile, 1);
			MMPF_MKV_Message("%s f framecnt = %d\n", __func__, framecnt+1);
			framecnt ++;
			data_len = ((start + len) - (MMPF_MKV_FilePosition(pMkvFile) + data_len))/framecnt;
			for(i = 0; i < framecnt; i++)
				pLacingSize[i] = data_len;
			data_len = 0;	//reset data_len, it will be added later;
			break;
		case MKV_LACING_NONE:
			framecnt = 1;
			break;
		case MKV_LACING_XIPH:
			framecnt = MMPF_MKV_EBML_ReadUint(pMkvFile, 1);
			MMPF_MKV_Message("%s x framecnt = %d\n", __func__, framecnt+1);
			for(i = 0; i < framecnt; i++){
				MMP_UBYTE val;
				pLacingSize[i] = 0;
				val = 255;
				do{
					val = MMPF_MKV_EBML_ReadUint(pMkvFile, 1);
					pLacingSize[i] += val;
				}while(val == 255);
				MMPF_MKV_Message("%s frame#%d size %X\n", __func__, i, pLacingSize[i]);
				data_len += pLacingSize[i];
			}
			pLacingSize[i] = (start + len) - (MMPF_MKV_FilePosition(pMkvFile) + data_len);
			MMPF_MKV_Message("%s x frame#%d size %X\n", __func__, i, pLacingSize[i]);
			//exit(0);
			framecnt ++;
			break;
		case MKV_LACING_EBML:
			framecnt = MMPF_MKV_EBML_ReadUint(pMkvFile, 1);
			MMPF_MKV_Message("%s l framecnt = %d\n", __func__, framecnt+1);
			i = 0;
			data_len = pLacingSize[0] = MMPF_MKV_EBML_ReadVuint(pMkvFile, &m, 0);
			MMPF_MKV_Message("%s l frame#%d size %X\n", __func__, i, pLacingSize[i]);
			for(i = 1; i < framecnt; i++){
				pLacingSize[i] = pLacingSize[i-1] + MMPF_MKV_EBML_ReadVsint(pMkvFile);
				data_len += pLacingSize[i];
				MMPF_MKV_Message("%s frame#%d size %X\n", __func__, i, pLacingSize[i]);
			}
			pLacingSize[i] = (start + len) - (MMPF_MKV_FilePosition(pMkvFile) + data_len);
			MMPF_MKV_Message("%s l frame#%d size %X\n", __func__, i, pLacingSize[i]);
			framecnt ++;
			break;
	}
	data_len += (start + len) - (MMPF_MKV_FilePosition(pMkvFile) + data_len);
	pTrack->ulFrames = framecnt;
	pTrack->ullTrackTimeOffset = timecode;

	MMPF_MKV_Message("%s data_len %llX %llX\n", __func__, data_len, MMPF_MKV_FilePosition(pMkvFile));

	pTrack->ullFramePos = MMPF_MKV_FilePosition(pMkvFile);
	pTrack->ulFrameSize = data_len;

	MMPF_MKV_Message("%s 2 %llx %llx\n", __func__, data_len, m);
	MMPF_MKV_FileSkipBytes(pMkvFile, data_len);
	
	return data_len;
}



//return number of read bytes
MMP_LONG MMPF_MKV_TrackSeekNextFrame(
	MMPF_MKV_CONTEXT *pMkvCtx,
	MMPF_MKV_TRACK *pTrack,
	MMP_ULONG key
){
	MMPF_MKV_EBML_ELEMENT element;
	MMP_ULONG64 ullGroupStart, ullGroupSize;
	MMPF_MKV_FILE *pMkvFile = pTrack->pMkvFile;
	MMP_ULONG ulFrameSize = 0;
	MMP_ULONG ulKeyOnly = key;
	MMP_UBYTE ubKeyFrame = 1;

	if(pMkvFile == NULL)	//track not opened
		return -1;
	
	MMPF_MKV_Message("get nextframe from %llX\n", MMPF_MKV_FilePosition(pMkvFile) );
	
	pTrack->ulFrames = 0;
	pTrack->ulFrameSize = 0;
	
	if(pTrack->ullType == MKV_TRACK_VIDEO){
	    pTrack->ullSampleStart = MMPF_MKV_FilePosition(pMkvFile);
	}
	
	while(MMPF_MKV_EBML_ReadElement(pMkvFile, &element, pMkvCtx->ullClustersPos, pMkvCtx->ullClustersSize)){
		switch(element.ulID){
			case MKV_CLUSTER:
				//just bypass id/size
				break;
			case MKV_CLUSTER_TIME_CODE:	//time info.
				pTrack->ullTrackTime = MMPF_MKV_EBML_ReadUint(pMkvFile, (MMP_ULONG)element.ullSize);
				pTrack->ullTrackTimeOffset = 0;
				break;
			case MKV_CLUSTER_BLOCK_GROUP:
				ullGroupStart = element.ullDataPos;
				ullGroupSize = element.ullSize;
				pTrack->ullFrameBlockStart = element.ullStart;
				MMPF_MKV_Message("%s gstart %llx gsize %llx\n", __func__, ullGroupStart, ullGroupSize);
				while(MMPF_MKV_EBML_ReadElement(pMkvFile, &element, ullGroupStart, ullGroupSize)){
					switch(element.ulID){
						case MKV_BLOCK:
							ulFrameSize = MMPF_MKV_TrackSeekFrame(pMkvCtx, pTrack, element.ullDataPos, element.ullSize, &ubKeyFrame, element.ulID);
							if(ulFrameSize == 0)
								pTrack->ullHintPos = ullGroupStart;
							break;
						case MKV_BLOCK_DURATION:	//time info.
							MMPF_MKV_Message("%s :MKV_BLOCK_DURATION %llx, %llx\n", __func__, element.ullDataPos, element.ullSize);
							MMPF_MKV_FileSkipBytes(pMkvFile, element.ullSize);
							//vsint = MMPF_MKV_EBML_ReadSint(pMkvFile, (MMP_ULONG)element.ullSize);
							break;
						case MKV_REFERENCE_BLOCK:
							MMPF_MKV_Message("%s :MKV_REFERENCE_BLOCK %llx, %llx\n", __func__, element.ullDataPos, element.ullSize);
							ubKeyFrame = 0;
							MMPF_MKV_FileSkipBytes(pMkvFile, element.ullSize);
							break;
						default:
							MMPF_MKV_Message("%s :unknown block group %lx %llx, %llx\n", __func__,element.ulID, element.ullDataPos, element.ullSize);
							MMPF_MKV_FileSkipBytes(pMkvFile, element.ullSize);
							break;
					}
				}
				break;
			case MKV_CLUSTER_SIMPLE_BLOCK:
				pTrack->ullFrameBlockStart = element.ullStart;
				ulFrameSize = MMPF_MKV_TrackSeekFrame(pMkvCtx, pTrack, element.ullDataPos, element.ullSize, &ubKeyFrame, element.ulID);
				break;
			default:
				//skip non-block elements
				MMPF_MKV_Message("%s :unknown block %lx %llx, %llx\n", __func__, element.ulID, element.ullDataPos, element.ullSize);
				MMPF_MKV_FileSkipBytes(pMkvFile, element.ullSize);
		}
		if(ulFrameSize > 0){
			if(ulKeyOnly != 0 && ubKeyFrame == 0){
				ulFrameSize = 0;
				ubKeyFrame = 1;
				continue;
			}else{
				//got required frame
				break;
			}
		}			
	}
	
	return ulFrameSize;
}

#if 1
//TODO: define error condition return number
//fail return -1, success return 0
MMP_LONG MMPF_MKV_TrackSeekTime(
	MMPF_MKV_CONTEXT *pMkvCtx,
	MMPF_MKV_TRACK *pTrack,
	MMP_ULONG ms,	//search in millisecond
	MMP_BOOL bNextKey,
	MMP_BOOL bNearest
){
	MMPF_MKV_FILE *pCuesFile;
	MMPF_MKV_EBML_ELEMENT element;
	MMPF_MKV_CUE_POINT Cues[2];
	MMP_LONG lCurrIdx, lPrevIdx, lTmpIdx;
	MMP_UBYTE ubReached = 0;
	MMP_ULONG64 ullSeekTime = ((MMP_ULONG64)ms )* 1000000;
	MMP_ULONG64	ullTargetTime = 0;
	MMP_ULONG64 ullTargetPosition = 0;
	
	//used for parsing CuePoint;
	MMP_ULONG64 ullPointStart, ullPointSize;
	MMP_ULONG64 track_num, position, block;
	
	if(pTrack->pMkvFile == NULL){	//track not opened
		print_msg("%s: track is not opened\n", __func__);
		return -1;
	}

	// make use of CUES to get seek start
	if(pMkvCtx->ullCuesPos == 0){
		print_msg("%s: cues is not present\n", __func__);
		return -1;
	}

	print_msg("seektime try to seek for %lld ms\n", ms);
	print_msg("cue_pos %llX cue_size %llX\n", pMkvCtx->ullCuesPos, pMkvCtx->ullCuesSize);

	//TODO: reduce open/close freq.
	//seek position in file
	pCuesFile = MMPF_MKV_FileOpen();
	MMPF_MKV_FileSeek(pCuesFile, pMkvCtx->ullCuesPos);
	
	lCurrIdx = 0;
	lPrevIdx = 1;
	lTmpIdx = -1;
	
	Cues[lCurrIdx].ullTrackNum = 0xFFFFFFFF;
	Cues[lPrevIdx].ullTrackNum = 0xFFFFFFFF;
	
	while(ubReached == 0 && MMPF_MKV_EBML_ReadElement(pCuesFile, &element, pMkvCtx->ullCuesPos, pMkvCtx->ullCuesSize)){
		switch(element.ulID){
			case MKV_CUES:
				//just bypass id/size
				break;
			case MKV_CUE_POINT:
				//just bypass id/size
				//if last cue point is matched, keep it in previous 
				if(Cues[lCurrIdx].ullTrackNum == pTrack->ullNumber){
					if(( MMPF_MKV_FloatMult(pTrack->fTimecodeScale, Cues[lCurrIdx].ullTime) * (pMkvCtx->seg.ullTimecodeScale)) > ullSeekTime){
						#if 0
						if(Cues[lPrevIdx].ullTrackNum == pTrack->ullNumber){
							if( (Cues[lCurrIdx].ullTime - ullSeekTime) > (ullSeekTime - Cues[lPrevIdx].ullTime) ){
								ullTargetPosition = Cues[lPrevIdx].ullPosition;
								//MMPF_MKV_FileSeek(t->pMkvFile, pPrevCue->position);
								ullTargetTime = Cues[lPrevIdx].ullTime;
							} else {
								ullTargetPosition = Cues[lCurrIdx].ullPosition;
								//MMPF_MKV_FileSeek(t->pMkvFile, pCurrCue->position);
								ullTargetTime = Cues[lCurrIdx].ullTime;
							} 
						}else{
							ullTargetPosition = Cues[lPrevIdx].ullPosition;
							ullTargetTime = Cues[lPrevIdx].ullTime;
							//MMPF_MKV_FileSeek(t->pMkvFile, pCurrCue->position);
						}
						#else
						if(Cues[lPrevIdx].ullTrackNum == pTrack->ullNumber){
						    if(!bNextKey && !bNearest){
							    ullTargetPosition = Cues[lPrevIdx].ullPosition;
							    ullTargetTime = Cues[lPrevIdx].ullTime;
							} else if(bNextKey){
							    ullTargetPosition = Cues[lCurrIdx].ullPosition;
							    ullTargetTime = Cues[lCurrIdx].ullTime;
							} else {
							    MMP_ULONG64 ullPrev = 0;
							    MMP_ULONG64 ullNext = 0;
							    ullPrev =  ullSeekTime - (MMPF_MKV_FloatMult(pTrack->fTimecodeScale, Cues[lPrevIdx].ullTime) * (pMkvCtx->seg.ullTimecodeScale));
							    ullNext =  (MMPF_MKV_FloatMult(pTrack->fTimecodeScale, Cues[lCurrIdx].ullTime) * (pMkvCtx->seg.ullTimecodeScale)) - ullSeekTime;
							    if(ullPrev < ullNext) {
							        ullTargetPosition = Cues[lPrevIdx].ullPosition;
							        ullTargetTime = Cues[lPrevIdx].ullTime;
							    } else {
							        ullTargetPosition = Cues[lCurrIdx].ullPosition;
							        ullTargetTime = Cues[lCurrIdx].ullTime;
							    }
							}
						}else{
							ullTargetPosition = Cues[lCurrIdx].ullPosition;
							ullTargetTime = Cues[lCurrIdx].ullTime;
						}
						#endif
						print_msg("seektime to position %llX %lld\n", ullTargetPosition, ullTargetTime);
						pTrack->ullTrackTime = ullTargetTime;
						pTrack->ullTrackTimeOffset = 0;
						MMPF_MKV_FileSeek(pTrack->pMkvFile, ullTargetPosition);
						ubReached = 1;
						break;
					}
					lTmpIdx = lPrevIdx;
					lPrevIdx = lCurrIdx;
					lCurrIdx = lTmpIdx;
					//clear track_num for matching check
					Cues[lCurrIdx].ullTrackNum = 0xFFFFFFFF;	
				}
				break;
			case MKV_CUE_TIME:
				Cues[lCurrIdx].ullTime = MMPF_MKV_EBML_ReadUint(pCuesFile, (MMP_ULONG)element.ullSize);
				break;
			case MKV_CUE_TRACK_POSITIONS:
				MMPF_MKV_Message("[%s] MKV_CUE_TRACK_POSITIONS\n", __func__);
				ullPointStart = element.ullDataPos;
				ullPointSize = element.ullSize;
				while(MMPF_MKV_EBML_ReadElement(pCuesFile, &element, ullPointStart, ullPointSize)){
					switch(element.ulID){
						case MKV_CUE_TRACK:
							track_num = MMPF_MKV_EBML_ReadUint(pCuesFile, (MMP_ULONG)element.ullSize);
							MMPF_MKV_Message("MKV_CUE_TRACK %lld  %llX %llX \n", track_num, element.ullSize, element.ullStart);
							break;
						case MKV_CUE_CLUSTER_POSITION:
							position = MMPF_MKV_EBML_ReadUint(pCuesFile, (MMP_ULONG)element.ullSize);
							MMPF_MKV_Message("MKV_CUE_CLUSTER_POSITION %llX %llX %llX \n", position, element.ullSize, element.ullStart);
							break;
						case MKV_CUE_BLOCK_NUMBER:
							block = MMPF_MKV_EBML_ReadUint(pCuesFile, (MMP_ULONG)element.ullSize);
							MMPF_MKV_Message("MKV_CUE_BLOCK_NUMBER %llX %llX %llX \n", block, element.ullSize, element.ullStart);
							break;
						default:
							//error?
							MMPF_MKV_FileSkipBytes(pCuesFile, element.ullSize);			
					}
					
					//cue_point matched
					if(track_num == pTrack->ullNumber){
						Cues[lCurrIdx].ullTrackNum = track_num;
						Cues[lCurrIdx].ullPosition = (position + pMkvCtx->ullSegPos);
						Cues[lCurrIdx].ullBlock = block;
						MMPF_MKV_Message("find matched cue for track %lld time %lld at %llX %llX %llX\n", Cues[lCurrIdx].ullTrackNum, Cues[lCurrIdx].ullTime, Cues[lCurrIdx].ullPosition, position, pMkvCtx->ullSegPos);
					}
				}
				break;
			default:
				//skip non-cue elements
				MMPF_MKV_FileSkipBytes(pCuesFile, element.ullSize);
		}	
	}
	if(ubReached == 0){
		pTrack->ullTrackTime = Cues[lPrevIdx].ullTime;
		pTrack->ullTrackTimeOffset = 0;
		MMPF_MKV_FileSeek(pTrack->pMkvFile, Cues[lPrevIdx].ullPosition);
	}
	
	pTrack->ullSampleStart = MMPF_MKV_FilePosition(pTrack->pMkvFile);
	
	MMPF_MKV_FileClose(pCuesFile);
	
	return 0;
}
#endif

MMP_ERR MMPF_MKV_TrackSeekKeyFrame(
    MMPF_MKV_CONTEXT *pMkvCtx,
    MMPF_MKV_TRACK *pTrack,
    MMP_BOOL bBackward
)
{
    MMPF_MKV_FILE *pCuesFile;
	MMPF_MKV_EBML_ELEMENT element, element2, element3;
	MMPF_MKV_CUE_POINT Cues[2];
	MMP_LONG lCurrIdx, lPrevIdx, lTmpIdx;
	MMP_UBYTE ubReached = 0;
	MMP_ULONG64	ullTargetTime = 0;
	MMP_ULONG64 ullTargetPosition = 0;
	MMP_ULONG   ulStatus = 0;
	MMP_ERR     error = MMP_ERR_NONE;
	
	//used for parsing CuePoint;
	MMP_ULONG64 track_num, position, block, track_curpos, PreCuePos, FirstCuePos;
	
	if(pTrack->pMkvFile == NULL){	//track not opened
		return -1;
	}

	// make use of CUES to get seek start
	if(pMkvCtx->ullCuesPos == 0){
		return -1;
	}

	//TODO: reduce open/close freq.
	//seek position in file
	pCuesFile = MMPF_MKV_FileOpen();
	MMPF_MKV_FileSeek(pCuesFile, pMkvCtx->ullCuesPos);
	
	track_curpos = pTrack->ullSampleStart;
	
	lCurrIdx = 0;
	lPrevIdx = 1;
	lTmpIdx = -1;
	
	Cues[lCurrIdx].ullTrackNum = 0xFFFFFFFF;
	Cues[lPrevIdx].ullTrackNum = 0xFFFFFFFF;
	
	PreCuePos = pMkvCtx->ullCuesPos;
	
	while(ubReached == 0){
	    
	    ulStatus = MMPF_MKV_EBML_ReadElement(pCuesFile, &element, pMkvCtx->ullCuesPos, pMkvCtx->ullCuesSize);
	    if(ulStatus == 0) break;
	    
		switch(element.ulID){
			case MKV_CUES:
				//just bypass id/size
				FirstCuePos = MMPF_MKV_FilePosition(pCuesFile);
				break;
			case MKV_CUE_POINT:
				while(MMPF_MKV_EBML_ReadElement(pCuesFile, &element2, element.ullDataPos, element.ullSize)) {
	                switch(element2.ulID){
	                    case MKV_CUE_TIME:
	                        Cues[lCurrIdx].ullTime = MMPF_MKV_EBML_ReadUint(pCuesFile, (MMP_ULONG)element2.ullSize);
	                        break;
	                    case MKV_CUE_TRACK_POSITIONS:
	                        while(MMPF_MKV_EBML_ReadElement(pCuesFile, &element3, element2.ullDataPos, element2.ullSize)){
	                            switch(element3.ulID){
            						case MKV_CUE_TRACK:
            							track_num = MMPF_MKV_EBML_ReadUint(pCuesFile, (MMP_ULONG)element3.ullSize);
            							break;
            						case MKV_CUE_CLUSTER_POSITION:
            							position = MMPF_MKV_EBML_ReadUint(pCuesFile, (MMP_ULONG)element3.ullSize);
            							break;
            						case MKV_CUE_BLOCK_NUMBER:
            							block = MMPF_MKV_EBML_ReadUint(pCuesFile, (MMP_ULONG)element3.ullSize);
            							break;
            						default:
            							//error?
            							MMPF_MKV_FileSkipBytes(pCuesFile, element3.ullSize);			
            					}
	                        }
	                        if(track_num == pTrack->ullNumber){
        						Cues[lCurrIdx].ullTrackNum = track_num;
        						Cues[lCurrIdx].ullPosition = (position + pMkvCtx->ullSegPos);
        						Cues[lCurrIdx].ullBlock = block;
        					}
	                        break;
	                    default:
				            //skip non-cue elements
				            MMPF_MKV_FileSkipBytes(pCuesFile, element2.ullSize);
	                    
	                }
				}
				if(Cues[lCurrIdx].ullTrackNum == pTrack->ullNumber){
					if(Cues[lCurrIdx].ullPosition > track_curpos || (bBackward && Cues[lCurrIdx].ullPosition >= track_curpos)){
						
						if((Cues[lPrevIdx].ullTrackNum == pTrack->ullNumber) && bBackward){
							ullTargetPosition = Cues[lPrevIdx].ullPosition;
							ullTargetTime = Cues[lPrevIdx].ullTime;
						}else{
							ullTargetPosition = Cues[lCurrIdx].ullPosition;
							ullTargetTime = Cues[lCurrIdx].ullTime;
						}

						pTrack->ullTrackTime = ullTargetTime;
						pTrack->ullTrackTimeOffset = 0;
						MMPF_MKV_FileSeek(pTrack->pMkvFile, ullTargetPosition);
						ubReached = 1;
						if(bBackward){
    					    if(PreCuePos <= FirstCuePos)
    					        ubReached = 2;
    					} else {
    					    if((pMkvCtx->ullCuesPos + pMkvCtx->ullCuesSize) <= MMPF_MKV_FilePosition(pCuesFile)){
    					        ubReached = 2;
    					    }
    					}
    					break;
					}
					    
					lTmpIdx = lPrevIdx;
					lPrevIdx = lCurrIdx;
					lCurrIdx = lTmpIdx;
					
					PreCuePos = MMPF_MKV_FilePosition(pCuesFile) - element.ullSize - (element.ullDataPos - element.ullStart);
					
					//clear track_num for matching check
					Cues[lCurrIdx].ullTrackNum = 0xFFFFFFFF;	
				}
				break;
			default:
				//skip non-cue elements
				MMPF_MKV_FileSkipBytes(pCuesFile, element.ullSize);
		}
	}
	if(ubReached == 0){
		pTrack->ullTrackTime = Cues[lPrevIdx].ullTime;
		pTrack->ullTrackTimeOffset = 0;
		MMPF_MKV_FileSeek(pTrack->pMkvFile, Cues[lPrevIdx].ullPosition);
		error = MMP_VIDPSR_ERR_EOS;
	}
	if(ubReached == 2)
	    error = MMP_VIDPSR_ERR_EOS;
	pTrack->ullSampleStart = MMPF_MKV_FilePosition(pTrack->pMkvFile);
	    
	MMPF_MKV_FileClose(pCuesFile);
	
	return error;
}

MMP_LONG MMPF_MKV_GetNumTracks(
	MMPF_MKV_CONTEXT *pMkvCtx
){
	return pMkvCtx->ullNumTracks;
}



static MMPF_MKV_TRACK* MMPF_MKV_GetTrackInfo(
	MMPF_MKV_CONTEXT *pMkvCtx,
	MMP_LONG lTrackIdx
){
	if(lTrackIdx >= pMkvCtx->ullNumTracks)
		return NULL;
	else
		return &(pMkvCtx->tracks[lTrackIdx]);
}

#if 0 // for IDE function name list by section
void _____Demuxer_____(){}
#endif

static MMP_ERR MMPF_MKVPSR_DecodeMP4ASC(MMP_UBYTE tmp1, MMP_UBYTE tmp2)
{
	mp4ASC.objectTypeIndex = (MMP_UBYTE)(tmp1 >> 3);
	mp4ASC.samplingFrequencyIndex = ((tmp1 & 0x07) << 1) | (tmp2 >> 7);
	mp4ASC.channelsConfiguration = (tmp2 & 0x78) >> 3;
	// print out the result for debuging
	#if	1
		MMPF_MKV_Message("[%s] mp4ASC.objectTypeIndex:%ld\n", __func__, mp4ASC.objectTypeIndex);
		MMPF_MKV_Message("[%s] mp4ASC.channelsConfiguration:%ld\n", __func__, mp4ASC.channelsConfiguration);
	#endif

	mp4ASC.samplingFrequency = MMPF_3GPPSR_GetSampleRate(mp4ASC.samplingFrequencyIndex);
	mp4ASC.sample256_time = MMPF_3GPPSR_Get256SampleTime(mp4ASC.samplingFrequencyIndex);
	if (mp4ASC.samplingFrequency == 0) {
		return	MMP_VIDPSR_ERR_UNSUPPORTED_SETTING;
	}
	
	if (mp4ASC.channelsConfiguration > 7 ) {
		return	MMP_VIDPSR_ERR_UNSUPPORTED_SETTING;
	}

	if (mp4ASC.objectTypeIndex == 2) {
		mp4ASC.frameLengthFlag = (tmp2 & 0x04) >> 2;

		if (mp4ASC.frameLengthFlag == 1) {
			return	MMP_VIDPSR_ERR_UNSUPPORTED_SETTING;
		}    
	}
	else {
		return	MMP_VIDPSR_ERR_UNSUPPORTED_SETTING;
	}
	#if	1
		MMPF_MKV_Message("[%s] mp4ASC.samplingFrequency: %ld\n", __func__, mp4ASC.samplingFrequency);
	#endif
    return MMP_ERR_NONE;
}

MMP_ERR MMPF_MKVPSR_LoadAudioData(MMP_UBYTE *dataptr, MMP_ULONG Size, MMP_ULONG *LoadedSize)
{
	MMPF_MKV_TRACK *pTrack = m_pMkvAudioTrack;
    typedef struct TEMP_AUDIO_HEADER {
        MMP_ULONG ulTime;
        MMP_ULONG ulSize;
        MMP_USHORT ulFlag;
        MMP_USHORT ulFlag2;
    }TEMP_AUDIO_HEADER;
    
    MMP_ULONG           LoadSize;
    MMP_USHORT          usHeaderSizeToWrite = 0;
    
    static MMP_USHORT usHeadherReduntSize = 0;//How many header bytes left last time
    static TEMP_AUDIO_HEADER curHeader;
    #ifndef SIZEOF_AUDIO_HEADER
    #define SIZEOF_AUDIO_HEADER (12)
    #endif
    
    if ( (m_ulPreAudioReduntSize == 0) && (m_bAudioEOS) ) {
        *LoadedSize = 0;
        return MMP_ERR_NONE;
    }
    
    LoadSize = 0;

	if(m_ulPreAudioReduntSize == 0) {
		if(pTrack->ulCurFrame >= pTrack->ulFrames){
			m_ulPreAudioReduntSize = MMPF_MKV_TrackSeekNextFrame( &m_MkvContext, pTrack, 0);
    		pTrack->ulCurFrame = 0;
    	}else{
    		m_ulPreAudioReduntSize = 1;
    	}
    
    	if(m_ulPreAudioReduntSize > 0){
	        curHeader.ulTime = MMPF_MKV_TrackGetTime( &m_MkvContext, pTrack);
	        curHeader.ulTime += (MMP_LONG)(((pTrack->ulCurFrame*pTrack->ullDefaultDuration) + 500000)/1000000);
			usHeadherReduntSize = SIZEOF_AUDIO_HEADER;
			if(pTrack->ulFrames > 1)
				m_ulPreAudioReduntSize = pTrack->ulLacingSize[pTrack->ulCurFrame++];
			else
				pTrack->ulCurFrame++;

			m_ulPreAudioReduntSize += SIZEOF_AUDIO_HEADER;
	    
			curHeader.ulSize = m_ulPreAudioReduntSize;

			curHeader.ulFlag2 ++;
			curHeader.ulFlag2 = (MMP_SHORT)(MMP_ULONG)dataptr;
	    
			curHeader.ulFlag = BUFFERFLAG_EOF;
		}else{
	        curHeader.ulTime = MMPF_MKV_TrackGetTime( &m_MkvContext, pTrack);
			usHeadherReduntSize = SIZEOF_AUDIO_HEADER;
			m_ulPreAudioReduntSize = SIZEOF_AUDIO_HEADER;
			print_msg("[%s] MMP_VIDPSR_ERR_EOS Audio\n", __func__);
			curHeader.ulFlag2 ++;
			curHeader.ulFlag2 = (MMP_SHORT)(MMP_ULONG)dataptr;
			curHeader.ulFlag = BUFFERFLAG_EOF;
			curHeader.ulFlag |= BUFFERFLAG_EOS;
			m_bAudioEOS = MMP_TRUE;
		}
	}
	
	if (m_ulPreAudioReduntSize) {
		// write header
		if (usHeadherReduntSize > 0) { // there is header to write
			MMP_USHORT i;
			if (usHeadherReduntSize >= Size) {
				usHeaderSizeToWrite = Size;
			} else {
				usHeaderSizeToWrite = usHeadherReduntSize;
			}
			m_ulPreAudioReduntSize -= usHeaderSizeToWrite;
			for (i = SIZEOF_AUDIO_HEADER - usHeadherReduntSize;
				i < SIZEOF_AUDIO_HEADER - usHeadherReduntSize + usHeaderSizeToWrite; i++) {
				*(dataptr ++) = *(((MMP_UBYTE*)(&curHeader)) + i);
			}
			usHeadherReduntSize -= usHeaderSizeToWrite;
		}

		Size -= usHeaderSizeToWrite;
		if(Size > 0){
			if (m_ulPreAudioReduntSize >= Size) {
				LoadSize = Size;
			}else {
				LoadSize = m_ulPreAudioReduntSize;
			}
			m_ulPreAudioReduntSize -= LoadSize;

			//read in audio stream
			MMPF_MKV_TrackReadFrame( &m_MkvContext, pTrack, dataptr, LoadSize);

		}
		*LoadedSize = LoadSize + usHeaderSizeToWrite;

		if ( m_ulPreAudioReduntSize == 0){
			m_ulBuffedAudioAtoms+=1;
		}
	}else{
		*LoadedSize = 0;
	}
    return MMP_ERR_NONE;
}

MMP_ERR MMPF_MKVPSR_FillAudioBuffer(MMP_ULONG dest_addr,MMP_ULONG ulSize, MMP_USHORT *usTransferSize)
{
    MMP_ULONG   file_return_size2 = 1;
    MMP_ERR     error = MMP_ERR_NONE;
    
    *usTransferSize = 0;
    
    while (ulSize > *usTransferSize && file_return_size2 != 0) {
        
        error = MMPF_MKVPSR_LoadAudioData((MMP_UBYTE *)(dest_addr+ *usTransferSize),
                                  ulSize - *usTransferSize, &file_return_size2);
        if (error == MMP_ERR_NONE)
            *usTransferSize += file_return_size2;
        else
            return error;
    }
    
    return error;
}

MMP_ERR MMPF_MKVPSR_GetSingleFrame(  MMP_ULONG dest_addr,MMP_ULONG lSizeToLoad,
                                            MMP_ULONG seg1     ,MMP_ULONG seg2
){
    MMP_ULONG   ulRingTailing = 0;
    MMP_UBYTE   *buf_start = m_VideoStreamBuf;
    
 	if (lSizeToLoad > seg1 ) {
 		MMPF_MKV_TrackReadFrame( &m_MkvContext, m_pMkvVideoTrack, (MMP_UBYTE*)dest_addr, seg1);
 		MMPF_MKV_TrackReadFrame( &m_MkvContext, m_pMkvVideoTrack, buf_start, lSizeToLoad - seg1);
	
	#if RING_BUF_TAILING > 0
		MMPF_BS_AccessVideoHeader(m_pBsBufInfo->buf + lSizeToLoad - seg1, (MMP_UBYTE*)&ulRingTailing, 4, RING_WRITE);
	#endif
	}
	else 
	{
	    #if (CHIP == MCR_V2)
	    {
 		MMPF_MKV_TrackReadFrame( &m_MkvContext, m_pMkvVideoTrack,(MMP_UBYTE*) dest_addr, lSizeToLoad);
 		}
 		#endif
	    #if (CHIP == P_V2) 
	    {
	        if (gVidFileInfo.video_info.format == MMP_VID_VIDEO_H264) {
	            MMP_UBYTE *buf = (MMP_UBYTE*)dest_addr;
	            
	            buf[0] = 0x00; buf[1] = 0x00; buf[2] = 0x00; buf[3] = 0x01;
	            
	            MMPF_MKV_TrackReadFrame( &m_MkvContext, m_pMkvVideoTrack,(MMP_UBYTE*) (dest_addr + 4), lSizeToLoad);
	        }
	        else {
	            MMPF_MKV_TrackReadFrame( &m_MkvContext, m_pMkvVideoTrack,(MMP_UBYTE*) dest_addr, lSizeToLoad);
	        }
	    }
	    #endif
            
        if ( m_bBsSupportLargeFrame ) {
			#if RING_BUF_TAILING > 0
			if (!m_3GPPdata.bLargeFrame) {
				MMPF_BS_AccessVideoHeader((MMP_UBYTE*)(dest_addr + lSizeToLoad), 
					(MMP_UBYTE*)&ulRingTailing, 4, RING_WRITE);
			}
			#endif
		}
	}
	return 0;   
}

MMP_ERR MMPF_MKVPSR_GetNextSample(MMPF_MKV_CONTEXT *pMkvCtx, MMPF_MKV_TRACK *pTrack, MMP_SHORT nAdvance)
{
	MMP_ULONG ulSize;
	
	ulSize = MMPF_MKV_TrackSeekNextFrame( pMkvCtx, pTrack, nAdvance);
	
	if(ulSize == 0)
		return MMP_VIDPSR_ERR_EOS;

	return MMP_ERR_NONE;
}

MMP_ERR MMPF_MKVPSR_GetNextKeySample(MMPF_MKV_CONTEXT *pMkvCtx, MMPF_MKV_TRACK *pTrack, MMP_SHORT nAdvance)
{
    MMP_ERR error = MMP_ERR_NONE;
    MMP_BOOL bBackward = 0;
    MMP_ULONG ulSize;
    MMP_SHORT Half = 0;
    
    if(nAdvance < 0) bBackward = 1;
    Half = nAdvance / 2;
    while((nAdvance != 0) && (error == MMP_ERR_NONE)){
        error = MMPF_MKV_TrackSeekKeyFrame(pMkvCtx, pTrack, bBackward);
        if(bBackward)
            nAdvance++;
        else
            nAdvance--;
        if(nAdvance == Half) {
        }
    }
    ulSize = MMPF_MKV_TrackSeekNextFrame(pMkvCtx, pTrack, 1);
    
    if(ulSize == 0)
		return MMP_VIDPSR_ERR_EOS;
    
    return error;
}

//get frame size and set m_3GPPdata.usBSOffset, m_3GPPdata.usEOFPadding;
static MMP_ERR MMPF_MKVPSR_SeekNextVideoFrame(MMP_ULONG *RequestFrame)
{
    MMP_ERR error = MMP_ERR_NONE;

    /*MMP_ULONG64	ullTrackTime = m_pMkvVideoTrack->ullTrackTime;
    MMP_ULONG64 ullTrackTimeOffset = m_pMkvVideoTrack->ullTrackTimeOffset;
    MMP_ULONG64 ullLastStart = m_pMkvVideoTrack->ullFrameBlockStart;
    MMP_ULONG64 ullLastPos= m_pMkvVideoTrack->ullFramePos;
    MMP_ULONG ulLastSize = m_pMkvVideoTrack->ulFrameSize;*/
	MMPF_3GPP_SEEKSTATUS *pSeek = &m_seek[m_ubVideoCurTrack];

    *RequestFrame = 0;
    if (pSeek->mode == MMPF_M_SEEK_NORMAL) {
        
    	switch(m_MkvOpState){
    		case MKV_SEEK_OP:
		        error = MMPF_MKVPSR_GetNextSample(&m_MkvContext, m_pMkvVideoTrack, 1);       
    			break;
    		case MKV_NONE_OP:
		        error = MMPF_MKVPSR_GetNextSample(&m_MkvContext, m_pMkvVideoTrack, 0);        
    			break;
    	}
    	m_MkvOpState = MKV_NONE_OP;
    }
    else if (pSeek->mode == MMPF_M_FF_KEY) {
    	m_MkvOpState = MKV_FF_OP;
        //error = MMPF_MKVPSR_GetNextSample(&m_MkvContext, m_pMkvVideoTrack, 1);        
        error = MMPF_MKVPSR_GetNextKeySample(&m_MkvContext, m_pMkvVideoTrack, pSeek->sSpeed/100);
        if (error == MMP_VIDPSR_ERR_EOS) {
            //error = MMP_ERR_NONE; // continue to play so not return EOS
            pSeek->mode = MMPF_M_SEEK_NONE;
            /*m_pMkvVideoTrack->ullFrameBlockStart = ullLastStart;
            m_pMkvVideoTrack->ullFramePos = ullLastPos;
            m_pMkvVideoTrack->ulFrameSize = ulLastSize;
            m_pMkvVideoTrack->ullTrackTime = ullTrackTime;
            m_pMkvVideoTrack->ullTrackTimeOffset = ullTrackTimeOffset;
            MMPF_MKV_FileSeek(m_pMkvVideoTrack->pMkvFile, m_pMkvVideoTrack->ullFrameBlockStart);*/
        }

    }
    else if (pSeek->mode == MMPF_M_SEEK_NONE) {
        return MMP_VIDPSR_ERR_EOS;
    }
    else {
        return MMP_VIDPSR_ERR_INCORRECT_STATE_OPERATION;
    }
	
	#if (CHIP == MCR_V2)
	// calculate the offsets before and after the bitstream in all cases
	m_3GPPdata.usBSOffset = sizeof(MMPF_M_FRAME_INFO);
    // ARM address padding
    m_3GPPdata.usEOFPadding = (4 - ((m_pMkvVideoTrack->ulFrameSize + m_3GPPdata.usBSOffset) % 4)) % 4 + RING_BUF_TAILING;
    #endif
	//#if (CHIP == P_V2)
    //m_3GPPdata[0].usBSOffset   = sizeof(MMPF_M_FRAME_INFO) +  (8 - (sizeof(MMPF_M_FRAME_INFO) % 8));
    //m_3GPPdata[0].usEOFPadding = (8 - ((m_pMkvVideoTrack->ulFrameSize + m_3GPPdata.usBSOffset) % 8)) % 8 + 8;
    //#endif
    
    if ( m_bBsSupportLargeFrame ) {
        // if it's a large frame, use fixed header 0x20;
        if ((m_pMkvVideoTrack->ulFrameSize + m_3GPPdata.usBSOffset + m_3GPPdata.usEOFPadding) > m_ulVideoStreamLen){
            m_3GPPdata.usBSOffset = 0x20;
        }
    }
    
	return error;
}


#if 0 // for IDE function name list by section
void _____MKVPSR_APIS_____(){}
#endif

MMP_ERR MMPF_MKVPSR_Init(void)
{
    MMP_ULONG ulTemp;
	MMPF_3GPP_SEEKSTATUS *pSeek = &m_seek[m_ubVideoCurTrack];

	MEMSET0(&m_seek);
	MEMSET0(&mp4ASC);
    MEMSET0(&m_3GPPdata);

    // Keep the file size for memory mode
    ulTemp = gVidFileInfo.file_size;
    MEMSET0(&gVidFileInfo);
    gVidFileInfo.file_size = ulTemp;
	
	pSeek->sSpeed               = 10; // default set to 1x speed
	m_ulPreAudioReduntSize          = 0;
	gDemuxOption                = 0;
	
	//Open Files
	MMPF_VIDPSR_ResetCache();

	//Parser Initialization
	MEMSET0(&m_MkvContext);
	MEMSET0(&m_Files);
	m_MkvContext.pFile = MMPF_MKV_FileOpen();
	m_pMkvVideoTrack = NULL;
	m_pMkvAudioTrack = NULL;
	m_MkvOpState = MKV_NONE_OP;

	return MMP_ERR_NONE;
}

MMP_ERR MMPF_MKVPSR_Deinit(void)
{
    return MMP_ERR_NONE;
}

MMP_ERR MMPF_MKVPSR_Demux(void)
{
    MMP_ERR                 errEOS = MMP_ERR_NONE;// If it's end of strem this time
    MMP_ERR                 error = MMP_ERR_NONE;
    MMP_ULONG               BackupRequestFrame,RequestFrame = 0;
    MMP_ULONG               SizeToLoad;
    MMP_ULONG               header_addr, frame_addr, avail_size,avail_size1,data_size;
	MMPF_3GPP_SEEKSTATUS 	*pSeek = &m_seek[m_ubVideoCurTrack];    
    
	// - Demux demux audio data
	if ((gDemuxOption & DEMUX_AUDIO)) {
		// clear the demux audio option
		gDemuxOption &= ~DEMUX_AUDIO;
		{
			MMP_USHORT  transfer_size = 0,transfer_size1 = 0;
			MMP_ULONG   addr,addr1;
			MMP_LONG    avail_size,avail_size1;
			MMP_ULONG   ulReturnSize;
                
			ulReturnSize = 0;
                
			//if (gAudioSample.ulOffset != 0) 
			{

				if (MMPF_BS_GetAudioStartAddr(&addr,&addr1,&avail_size,&avail_size1) == MMP_ERR_NONE) {
					if (avail_size1 < 0) { //doesn't need to rewind, plain read
						MMPF_MKVPSR_FillAudioBuffer(addr,avail_size, &transfer_size);
						MMPF_BS_UpdateAudioBufptr(transfer_size,MMP_FALSE);
					}
					else {// not enough size to put all data, have to rewind
						MMPF_MKVPSR_FillAudioBuffer(addr,avail_size, &transfer_size);
						if(transfer_size >= avail_size) {
                            MMPF_BS_UpdateAudioBufptr(transfer_size,MMP_TRUE);

                            MMPF_MKVPSR_FillAudioBuffer(addr1,avail_size1, &transfer_size1);
                            MMPF_BS_UpdateAudioBufptr(transfer_size1,MMP_FALSE);
                            transfer_size += transfer_size1;
                        } else {
                            MMPF_BS_UpdateAudioBufptr(transfer_size,MMP_FALSE);
                        }
					}
				}
			}
		}
	}

	// ===== fill video data =====
	if ((gDemuxOption & DEMUX_VIDEO)) {
		gDemuxOption &= ~DEMUX_VIDEO;
		if (!m_3GPPdata.bFillBufferDone) {
			//if ((!m_3GPPdata.bFilling) && (!m_3GPPdata.bLargeFrame) && (!m_bVideoEOS))
			if ((!m_3GPPdata.bFilling) && (!m_3GPPdata.bLargeFrame))
			{
				#if (CHIP == P_V2)
				m_H264ReadByte = 0;
				#endif
				
				BackupRequestFrame = pSeek->ulRequestFrame;

				if (m_bVideoEOS) {
					return MMP_VIDPSR_ERR_EOS;
				}

				errEOS = MMPF_MKVPSR_SeekNextVideoFrame(&RequestFrame);

				pSeek->ulRequestFrame = BackupRequestFrame;
			
				// ===== Time stamp =====
				WRITE_HEADER->ulRequestTime = MMPF_MKV_TrackGetTime(&m_MkvContext, m_pMkvVideoTrack);
				if ((MMP_ERR_NONE == errEOS) || (MMP_VIDPSR_ERR_EOS == errEOS)) {
					m_3GPPdata.bFilling = MMP_TRUE;
				}
				else {
					return errEOS;
				}
			}
		    
			if ( errEOS == MMP_VIDPSR_ERR_EOS) {
				print_msg("[%s] MMP_VIDPSR_ERR_EOS Video\n", __func__);
				m_bVideoEOS = MMP_TRUE;
			}
			
			#if (CHIP == P_V2)
            if (gVidFileInfo.video_info.format == MMP_VID_VIDEO_H264) {
                //ulRequestTime
                MMP_UBYTE temp_buf[4];
                MMP_ULONG nalu_length = 0;
                MMP_ULONG j;
                MMP_ULONG read_data_count = 0;
                MMP_ULONG frame_offset;
                
                frame_offset 	= m_pMkvVideoTrack->ullFramePos;
                read_data_count = m_H264ReadByte;
                //VAR_L(3,m_pMkvVideoTrack->ulFrameSize);
                /*
                RTNA_DBG_Str (0,"ulFrameSize : ");
                RTNA_DBG_Long(0,m_pMkvVideoTrack->ulFrameSize);
                RTNA_DBG_Str (0,"\r\n");
                
                RTNA_DBG_Str (0,"read_data_count : ");
                RTNA_DBG_Long(0,read_data_count);
                RTNA_DBG_Str (0,"\r\n");
                */
                
                do
                {
                    nalu_length = 0;
                    
                    if (m_pMkvVideoTrack->ulFrameSize != 0) {
                        for(j=0;j<m_H264LenByte;j++) {
                        
                            MMPF_MKV_TrackReadFrame(&m_MkvContext, m_pMkvVideoTrack,(MMP_UBYTE*) (temp_buf), 1);
                        
                            nalu_length = (nalu_length << 8) | temp_buf[0];
                        }
                                        
                        //read_data_count += m_H264LenByte;
                        //frame_offset    += m_H264LenByte;
                    }
                    
                    m_3GPPdata.usEOFPadding = (8 - ((nalu_length + m_3GPPdata.usBSOffset + 4) % 8)) % 8 + 8;
                    
                    data_size 				= nalu_length + m_3GPPdata.usBSOffset + m_3GPPdata.usEOFPadding + 4;
                    /*
                    RTNA_DBG_Str (0,"nalu_length : ");
                    RTNA_DBG_Long(0,nalu_length);
                    RTNA_DBG_Str (0,"\r\n");
                    
                    RTNA_DBG_Str (0,"data_size : ");
                    RTNA_DBG_Long(0,data_size);
                    RTNA_DBG_Str (0,"\r\n");
                    */
                    #if 1
                    {
	                MMP_USHORT              nWrap;
	                error = MMPF_BS_TestVideoBufPtr(RING_WRITE, data_size, &nWrap);
	                
	                if (error == MMP_ERR_NONE) {
                        m_3GPPdata.bLargeFrame = MMP_FALSE;
                        m_3GPPdata.bFilling    = MMP_FALSE;
                    }
                    else if (error == MMP_VIDPSR_ERR_INSUFFICIENT_RESOURCES) {
                    	RTNA_DBG_Str(0,"large frame!!!\r\n");

                        if (m_bBsSupportLargeFrame) {
                            m_3GPPdata.bLargeFrame  = MMP_TRUE;
                        }
                        else {
                            m_3GPPdata.bLargeFrame = MMP_FALSE;
                            m_3GPPdata.bFilling    = MMP_FALSE; //skip this frame
                            return error;
                        }
                    }
                    else {
                    	m_3GPPdata.bFilling    = MMP_TRUE;
                    	m_pMkvVideoTrack->ullFramePos -= m_H264LenByte;
                        return error;
                    }
                    
	                }
	                #endif
	                
                    if (m_pMkvVideoTrack->ulFrameSize != 0) {
	                    read_data_count += m_H264LenByte;
	                    frame_offset    += m_H264LenByte;
                    }
                        
                    error = MMPF_BS_GetVideoStartAddr(&header_addr,
                                                      &frame_addr,
                                                      &avail_size,
                                                      &avail_size1,
                                                      m_3GPPdata.usBSOffset,
                                                      &data_size,
                                                      m_ulBsAlignType);
                    #if 0
                    if((pSeek->mode == MMPF_M_FF_KEY || pSeek->mode == MMPF_M_SEEK_NONE) &&
                        (m_pBsBufInfo->ringData.Atoms > 3)){
                        return MMP_ERR_NONE;
                    }
                    #endif
                    #if 0
                    if (error == MMP_ERR_NONE) {
                        m_3GPPdata.bLargeFrame = MMP_FALSE;
                    }
                    else if (error == MMP_VIDPSR_ERR_INSUFFICIENT_RESOURCES) {
                        if (m_bBsSupportLargeFrame) {
                            m_3GPPdata.bLargeFrame  = MMP_TRUE;
                        }
                        else {
                            m_3GPPdata.bLargeFrame = MMP_FALSE;
                            m_3GPPdata.bFilling    = MMP_FALSE; //skip this frame
                            return error;
                        }
                    }
                    else {
                        return error;
                    }
                    #endif
                    
                    pSeek->ulRequestFrame       = RequestFrame;
                    
                    // ===== Update the final frame size =====
                    if (m_3GPPdata.bLargeFrame) {
                        //WRITE_HEADER->ulSize    = m_pBsBufInfo->length;
                        WRITE_HEADER->ulSize    = avail_size + avail_size1;
                    }
                    else {
                        WRITE_HEADER->ulSize    = data_size;
                    }
                    
                    avail_size -= m_3GPPdata.usBSOffset;
                    
                    WRITE_HEADER->ulBSSize      = nalu_length + 4;//gVideoSample.ulSize;
                	WRITE_HEADER->ulAddress     = frame_addr;
                	WRITE_HEADER->ulFlag        = 0;
                    WRITE_HEADER->ulFrameNum    = m_pMkvVideoTrack->ulFrameIndex;

                	if ( (nalu_length + 4) < (avail_size+avail_size1)) {
                        SizeToLoad = nalu_length;//gVideoSample.ulSize;
                        
                        if ( (read_data_count + SizeToLoad) == m_pMkvVideoTrack->ulFrameSize  ) {
                            WRITE_HEADER->ulFlag |= BUFFERFLAG_EOF;
                            
                            if(m_bVideoEOS) {
                    	        WRITE_HEADER->ulFlag |= BUFFERFLAG_EOS;
                    	        DBG_S(3,"EOS!\r\n");
                    	        WRITE_HEADER->ulFlag |= BUFFERFLAG_FRAME_EOS;
                    	    }
                        }
                        else {
                            WRITE_HEADER->ulFlag |= BUFFERFLAG_SLICE;
                            if(m_bVideoEOS) {
                    	        WRITE_HEADER->ulFlag |= BUFFERFLAG_FRAME_EOS;
                    	    }
                        }
                    }
                    else {
                        SizeToLoad = (avail_size+avail_size1);
                    }

                    //VAR_L(3,header_addr);

                    // copy the write header into the bitstream
                    MMPF_BS_AccessVideoHeader((MMP_UBYTE*)header_addr, (MMP_UBYTE*)WRITE_HEADER, sizeof(MMPF_M_FRAME_INFO),RING_WRITE);

                    if (SizeToLoad != 0) {

                        error = MMPF_MKVPSR_GetSingleFrame(WRITE_HEADER->ulAddress, SizeToLoad, avail_size, avail_size1);

                        if (error != MMP_ERR_NONE) {
                            m_3GPPdata.bFilling         = MMP_FALSE;
                            m_3GPPdata.bFillBufferDone  = MMP_FALSE;
                        
                            return error;
                        }
                        
                        frame_offset    	+= SizeToLoad;
                        read_data_count 	+= SizeToLoad;
                        
                        m_H264ReadByte					= read_data_count;
                        m_pMkvVideoTrack->ullFramePos 	= frame_offset;
                    }
                    
                    m_3GPPdata.bFillBufferDone = MMP_TRUE;
                    
                    MMPF_BS_UpdateVideoBufPtr(RING_WRITE, WRITE_HEADER->ulSize);
                    
                    m_3GPPdata.bFilling         = MMP_FALSE;
                    m_3GPPdata.bFillBufferDone  = MMP_FALSE;
                    
                    //m_3GPPdata.pPreWriteHeader  = (MMPF_M_FRAME_INFO*)(header_addr);
                    m_3GPPdata.pPreWriteHeader.ulRequestTime  = ((MMPF_M_FRAME_INFO*)header_addr)->ulRequestTime;
                    
                }while(read_data_count < m_pMkvVideoTrack->ulFrameSize);
                
                m_H264ReadByte = 0;
            }
            else
            #endif
            {

    		    data_size = m_pMkvVideoTrack->ulFrameSize + m_3GPPdata.usBSOffset + m_3GPPdata.usEOFPadding;
    			error = MMPF_BS_GetVideoStartAddr(&header_addr,
    							&frame_addr,
    							&avail_size,
    							&avail_size1,
    							m_3GPPdata.usBSOffset,
    							&data_size,
    							m_ulBsAlignType);
    			if((pSeek->mode == MMPF_M_FF_KEY || pSeek->mode == MMPF_M_SEEK_NONE) &&
                   (m_pBsBufInfo->ringData.Atoms > 3)){
                    return MMP_ERR_NONE;
                }
    		    
    			if (error == MMP_ERR_NONE) {
    				m_3GPPdata.bLargeFrame = MMP_FALSE;
    			}
    			else if (error == MMP_VIDPSR_ERR_INSUFFICIENT_RESOURCES) {
    				if (m_bBsSupportLargeFrame) {
    					m_3GPPdata.bLargeFrame  = MMP_TRUE;
    				}
    				else {
    					m_3GPPdata.bLargeFrame = MMP_FALSE;
    					m_3GPPdata.bFilling    = MMP_FALSE; //skip this frame
    					return error;
    				}
    			}
    			else {
    				return error;
    			}
    		    
    			pSeek->ulRequestFrame       = RequestFrame;
    		    
    			// ===== Update the final frame size =====
    			if (m_3GPPdata.bLargeFrame) {
    				//WRITE_HEADER->ulSize    = m_pBsBufInfo->length;
    				WRITE_HEADER->ulSize    = avail_size + avail_size1;
    			}
    			else {
    				WRITE_HEADER->ulSize    = data_size;
    			}
    		    
    			avail_size -= m_3GPPdata.usBSOffset;
    		    
    			WRITE_HEADER->ulBSSize      = m_pMkvVideoTrack->ulFrameSize;
    			WRITE_HEADER->ulAddress     = frame_addr;
    			WRITE_HEADER->ulFlag        = 0;
    			WRITE_HEADER->ulFrameNum    = m_pMkvVideoTrack->ulFrameIndex;

    			if (m_pMkvVideoTrack->ulFrameSize < (avail_size+avail_size1)) {
    				SizeToLoad = m_pMkvVideoTrack->ulFrameSize;
    				WRITE_HEADER->ulFlag |= BUFFERFLAG_EOF;
    			
    				if(m_bVideoEOS) {
    					WRITE_HEADER->ulFlag |= BUFFERFLAG_EOS;
                    	WRITE_HEADER->ulFlag |= BUFFERFLAG_FRAME_EOS;
    				}
    			}
    			else {
    				SizeToLoad = (avail_size+avail_size1);
    			}
    		    
    			m_pMkvVideoTrack->ulFrameSize -= SizeToLoad;
    		    
    			// copy the write header into the bitstream
    			MMPF_BS_AccessVideoHeader((MMP_UBYTE*)header_addr, (MMP_UBYTE*)WRITE_HEADER, sizeof(MMPF_M_FRAME_INFO),RING_WRITE);

    			// Put single VOP for video decoder
    			error = MMPF_MKVPSR_GetSingleFrame(WRITE_HEADER->ulAddress, SizeToLoad, avail_size, avail_size1);

    			if (error != MMP_ERR_NONE) {
    				m_3GPPdata.bFilling         = MMP_FALSE;
    				m_3GPPdata.bFillBufferDone  = MMP_FALSE;
    		    
    				return error;
    			}
    		    
    			m_3GPPdata.bFillBufferDone = MMP_TRUE;
 
    			MMPF_BS_UpdateVideoBufPtr(RING_WRITE, WRITE_HEADER->ulSize);
    		    
    			m_3GPPdata.bFilling         = MMP_FALSE;
    			m_3GPPdata.bFillBufferDone  = MMP_FALSE;
    		    
    			//m_3GPPdata.pPreWriteHeader  = (MMPF_M_FRAME_INFO*)(header_addr);
                m_3GPPdata.pPreWriteHeader.ulRequestTime  = WRITE_HEADER->ulRequestTime;
            }
		} // ! fill buffer done
	}
	
	return MMP_ERR_NONE;
}


MMP_VID_FILEVALIDATION MMPF_MKVPSR_Parse(void)
{

	MMP_ULONG       i, ulRead;
	MMPF_MKV_TRACK  *pTrack;
    MMP_UBYTE       *dest_buf = m_VideoStreamBuf;
    #if (CHIP == P_V2)
    MMP_BOOL        bDecodeFirstFrame;
    MMP_ULONG       ulStreamDataLeft;
    #endif
    
    m_ulTempBSSize = 0;
    m_ulWorkingBufSize = 0;
    
	MMPF_MKV_Message("[%s] MKV Init.\n", __func__);
	MMPF_MKVPSR_Init();

    MMPF_Player_GetSupportedFormat(&m_3GPPdata.sSupportedAudioFmt,
                                   &m_3GPPdata.sSupportedVideoFmt);

    m_ulAudioPtr  = 0;
	MMPF_MKV_Parse(&m_MkvContext);
	
	//Currently, only support H.264 & AAC/AC3/MP3
	gVidFileInfo.is_video_seekable[0] = 0;
	m_MkvContext.seg.ullDuration = m_MkvContext.seg.ullDuration / 1000000ull;
	gVidFileInfo.total_file_time = m_MkvContext.seg.ullDuration; // ms
	gVidFileInfo.video_info[0].fps = 0;
	for( i = 0; i < m_MkvContext.ullNumTracks; i++){
		pTrack = MMPF_MKV_GetTrackInfo(&m_MkvContext, i);
		switch(pTrack->ullType){
			case MKV_TRACK_VIDEO:
				m_pMkvVideoTrack = pTrack;
				MMPF_MKV_Message("track: %ld is a video track\n", i);
				
				ulRead = MMPF_MKV_TrackOpen( &m_MkvContext, pTrack, (MMP_BYTE*)m_VideoStreamBuf, m_ulVideoStreamLen);
				gVidFileInfo.is_video_available[0] = 1;
				gVidFileInfo.video_info[0].total_video_time = m_MkvContext.seg.ullDuration; // sec
				//gVidFileInfo.video_info.width = 0;
				//gVidFileInfo.video_info.height = 0;
				gVidFileInfo.video_info[0].width = m_pMkvVideoTrack->AV.VIDEO.ulDisplayWidth;
				gVidFileInfo.video_info[0].height = m_pMkvVideoTrack->AV.VIDEO.ulDisplayHeight;

				if(strncmp(pTrack->pCodecID, "V_MS/VFW/FOURCC", 15) == 0){
					MMP_ULONG ulCodecTagIdx;
					MMP_ULONG64 tmpPos;
					BitmapInfoHeader *bmpInfo;
					bmpInfo = (BitmapInfoHeader*)m_VideoStreamBuf;
					
					for(ulCodecTagIdx = 0; fourcc_tag[ulCodecTagIdx][1] != 0; ulCodecTagIdx++){
						if(bmpInfo->biCompression == fourcc_tag[ulCodecTagIdx][1]){
		                    gVidFileInfo.video_info[0].format   = fourcc_tag[ulCodecTagIdx][0];
						}
					}
					if(gVidFileInfo.video_info[0].format == MMP_VID_VIDEO_NONE)
					    gVidFileInfo.video_info[0].format = MMP_VID_UNSUPPORTED_FILE;
					
					tmpPos = MMPF_MKV_FilePosition(pTrack->pMkvFile);
					
					//read first frame for codec initialization
					MMPF_MKV_TrackSeekNextFrame(&m_MkvContext, pTrack, 0);
					MMPF_MKV_TrackReadFrame(&m_MkvContext, pTrack, (MMP_UBYTE*)m_VideoStreamBuf, pTrack->ulFrameSize);
					ulRead = pTrack->ulFrameSize;
		
                    //roll-back, for first frame
                    MMPF_MKV_FileSeek(pTrack->pMkvFile, tmpPos);
				}else if(strncmp(pTrack->pCodecID, "V_MPEG4/ISO/ASP", 15) == 0){
					print_msg("track: %ld is a MPEG4 track\n", i);
                    gVidFileInfo.video_info[0].format   = MMP_VID_VIDEO_MP4V_SP;
                    if ( (m_3GPPdata.sSupportedVideoFmt & gVidFileInfo.video_info[0].format) == 0 )
                    {
                        return MMP_VID_UNSUPPORTED_FILE;
                    }                    
				}else if(strncmp(pTrack->pCodecID, "V_MPEG4/ISO/AVC", 15) == 0){
					gVidFileInfo.video_info[0].format = MMP_VID_VIDEO_H264;
					print_msg("track: %ld is a H264 track\n", i);

                    if ( (m_3GPPdata.sSupportedVideoFmt & gVidFileInfo.video_info[0].format) == 0 )
                    {
                        return MMP_VID_UNSUPPORTED_FILE;
                    }
					
				}else{
					gVidFileInfo.is_video_available[0] = 0;
				}
				
				if(gVidFileInfo.is_video_available != 0){
					switch(gVidFileInfo.video_info[0].format){
						case MMP_VID_VIDEO_MP4V_SP:
						    
						    #if (CHIP == MCR_V2)
		                    #if (VIDEO_P_EN)
		                    {
		                        extern MMP_HANDLE          *hVideo;
		                        extern MMPF_VIDEO_FUNC     m_vcodec;
		                        
		                        hVideo = MMPF_VIDEO_Open(CODEC_MP4V, &m_vcodec);

		                        if (MMP_ERR_NONE != 
		                            m_vcodec.MMPF_VIDDEC_Init(hVideo, dest_buf, ulRead, gVidFileInfo.video_info[0].width, gVidFileInfo.video_info[0].height, MMP_TRUE)) {
		                                                  
		                            return MMP_VID_UNSUPPORTED_FILE;
		                        }
		                        m_vcodec.MMPF_VIDDEC_GetInfo(hVideo, &gVidFileInfo.video_info[0]);
		                    }
		                    #else //++Old implementation
		                    if (MMP_ERR_NONE != MMPD_VIDPLAYER_InitVideoDec(
									m_video_dest_addr,
									dest_buf, 
									ulRead,
									&gVidFileInfo.video_info.width,
									&gVidFileInfo.video_info.height,
									&gVidFileInfo.video_info.cropping_left, 
									&gVidFileInfo.video_info.cropping_top,
									&gVidFileInfo.video_info.cropping_right,
									&gVidFileInfo.video_info.cropping_bottom,
		                            MMP_VID_VIDEO_MP4V_SP)) {
		                        
		                        return MMP_VID_UNSUPPORTED_FILE;
		                    }
		                    #endif //--Old implementation
		                    #endif
		                    
		                    #if (CHIP == P_V2)
						    #if (VIDEO_P_EN)
                            {
                                if ( MMP_ERR_NONE != MMPF_VideoDecoder_InitCodec(MMP_VID_VIDEO_MP4V,(MMP_ULONG)dest_buf,ulRead,&bDecodeFirstFrame, gVidFileInfo.video_info.width, gVidFileInfo.video_info.height) ) {
                                    DBG_S(3,"Init video failed\r\n");
                                
                                    return MMP_VID_UNSUPPORTED_VIDEO;
                                }
                            }
                            #else
                                if (MMP_ERR_NONE != MMPD_VIDPLAYER_InitMFD(
                                                   m_video_dest_addr,
                                                   dest_buf, 
                                                   ulRead,
                                                   MMP_VID_VIDEO_MP4V,
                                                   &bDecodeFirstFrame)) {
                                    return MMP_VID_UNSUPPORTED_FILE;
                                }
                        
                            #endif
						    #endif
							break;
						case MMP_VID_VIDEO_H264:
						    
						    #if (CHIP == MCR_V2)
							#if (VIDEO_P_EN)
							{
								extern MMP_HANDLE          *hVideo;
								extern MMPF_VIDEO_FUNC     m_vcodec;
			                        
								hVideo = MMPF_VIDEO_Open(CODEC_H264, &m_vcodec);
			                        
								if (MMP_ERR_NONE != m_vcodec.MMPF_VIDDEC_Init(hVideo, dest_buf, ulRead, (MMP_ULONG)m_VideoStreamBuf, m_ulVideoStreamLen, MMP_TRUE)) {
										MMPF_MKV_Message("[%s] MMP_VID_UNSUPPORTED_VIDEO!!!!!!!!!!\n", __func__);
			                            return MMP_VID_UNSUPPORTED_VIDEO;
								}
			                        
								m_vcodec.MMPF_VIDDEC_GetInfo(hVideo,&gVidFileInfo.video_info[0]);
								MMPF_MKV_Message("width: %d\n", gVidFileInfo.video_info[0].width);
								MMPF_MKV_Message("height: %d\n", gVidFileInfo.video_info[0].height);
							}
							#else
							gVidFileInfo.video_info.width    = 0;
							gVidFileInfo.video_info.height   = 0;
			                    
							if (MMP_ERR_NONE != MMPD_VIDPLAYER_InitVideoDec(
									m_video_dest_addr,
									dest_buf, 
									ulRead,
									&gVidFileInfo.video_info.width,
									&gVidFileInfo.video_info.height,
									&gVidFileInfo.video_info.cropping_left, 
									&gVidFileInfo.video_info.cropping_top,
									&gVidFileInfo.video_info.cropping_right,
									&gVidFileInfo.video_info.cropping_bottom,
									MMP_VID_VIDEO_H264)) {
								return MMP_VID_UNSUPPORTED_FILE;
							}
							#endif
							gVidFileInfo.video_info[0].width =  gVidFileInfo.video_info[0].width - 
								gVidFileInfo.video_info[0].cropping_left -
								gVidFileInfo.video_info[0].cropping_right;
			                                                            
							gVidFileInfo.video_info[0].height = gVidFileInfo.video_info[0].height - 
								gVidFileInfo.video_info[0].cropping_top -
								gVidFileInfo.video_info[0].cropping_bottom;
						    #endif
						    #if (CHIP == P_V2)
                            #if (VIDEO_P_EN)
                            {
                                if ( MMP_ERR_NONE != MMPF_VideoDecoder_InitCodec(MMP_VID_VIDEO_H264,(MMP_ULONG)dest_buf,ulRead,&bDecodeFirstFrame, gVidFileInfo.video_info.width, gVidFileInfo.video_info.height) ) {
                                    DBG_S(3,"Init video failed\r\n");
                                
                                    return MMP_VID_UNSUPPORTED_VIDEO;
                                }
                            }
                            #else
                                if (MMP_ERR_NONE != MMPD_VIDPLAYER_InitMFD(
                                                   m_video_dest_addr,
                                                   dest_buf, 
                                                   ulRead,
                                                   MMP_VID_VIDEO_H264,
                                                   &bDecodeFirstFrame)) {
                                    return MMP_VID_UNSUPPORTED_FILE;
                                }
                        
                            #endif
						    #endif
							break;
		                default:
		                	gVidFileInfo.is_video_available[0] = 0;
					}
					
					if(pTrack->ullDefaultDuration != 0)
    				    gVidFileInfo.video_info[0].fps = (1000000000ull+(pTrack->ullDefaultDuration/2))/pTrack->ullDefaultDuration;
    				if(m_MkvContext.ullCuesPos)
    				    gVidFileInfo.is_video_seekable[0] = 1;
				}
				
				break;
			case MKV_TRACK_AUDIO:
				m_pMkvAudioTrack = pTrack;
				MMPF_MKV_Message("track: %ld is a audio track\n", i);
				ulRead = MMPF_MKV_TrackOpen( &m_MkvContext, pTrack, (MMP_BYTE*)m_VideoStreamBuf, m_ulVideoStreamLen);
				gVidFileInfo.is_audio_available = 1;
				gVidFileInfo.audio_info.total_audio_time = (MMP_ULONG)(m_MkvContext.seg.ullDuration); // sec
				gVidFileInfo.audio_info.sampling_rate = (MMP_ULONG) MMPF_MKV_FloatMult(pTrack->AV.AUDIO.fSamplingFreq, 1);
				if(strncmp(pTrack->pCodecID, "A_AAC", 5) == 0){
					gVidFileInfo.audio_info.format = MMP_VID_AUDIO_AAC_LC;
					MMPF_MKVPSR_DecodeMP4ASC(m_VideoStreamBuf[0], m_VideoStreamBuf[1]);					
					gVidFileInfo.audio_info.sampling_rate = mp4ASC.samplingFrequency;
				}
				#if (VAC3_P_EN)
				else if(strncmp(pTrack->pCodecID, "A_AC3", 5) == 0){
					gVidFileInfo.audio_info.format = MMP_VID_AUDIO_AC3;					
				}
				#endif
				#if (VMP3_P_EN)
				else if(strncmp(pTrack->pCodecID, "A_MPEG/L3", 9) == 0){
					gVidFileInfo.audio_info.format = MMP_VID_AUDIO_MP3;
					MMPF_SetMP3Layer(0x03);
				}
				#endif
				else{
					gVidFileInfo.is_audio_available = 0;
				}
				m_ulAudSampleRate = gVidFileInfo.audio_info.sampling_rate;
				break;
		}
	}
	
    #if (CHIP == P_V2)
    if ( gVidFileInfo.is_video_available ) {
        if( bDecodeFirstFrame ) {
            #if 1
            gDemuxOption |= DEMUX_VIDEO;
            MMPF_MKVPSR_Demux();
            #endif
        
        
            #if (VIDEO_P_EN)
            {
                #if 1
                MMPF_M_FRAME_INFO    *header;
                MMP_UBYTE               *bs_buf = (MMP_UBYTE*)m_VideoStreamBuf;
                do{
                    header = (MMPF_M_FRAME_INFO*)bs_buf;
                    
                    MMPF_MFD_DecodeData(header->ulAddress,
                                        header->ulBSSize,
                                        0,
                                        0,
                                        0,
                                        &ulStreamDataLeft,
                                        0,
                                        0,
                                        0,
                                        NULL);
                    
                    bs_buf += header->ulSize;
                    
                }while(header->ulFlag == BUFFERFLAG_SLICE);
                #endif
            }
            #else
            {
                #if 1
                MMP_ULONG               ulDecStatus;
                MMPF_M_FRAME_INFO    *header;
                MMP_UBYTE               *bs_buf = dest_buf;
                
                do{
                    header = (MMPF_M_FRAME_INFO*)bs_buf;
                    
                    MMPD_VIDPLAYER_DecodeMFDData(header->ulAddress,
                                                 header->ulBSSize,//ulStreamSize,
                                                 0,
                                                 0,
                                                 MMP_FALSE,
                                                 0,
                                                 NULL,
                                                 &ulDecStatus);
                    bs_buf += header->ulSize;        
                    
                }while(header->ulFlag == BUFFERFLAG_SLICE);
                #endif
            }
            #endif
        }
        
        #if (VIDEO_P_EN)
        
        MMPF_MFD_GetInfo(&gVidFileInfo.video_info);
        
        #else
        MMPD_VIDPLAYER_GetMFDInfo( &gVidFileInfo.video_info.width,
                                   &gVidFileInfo.video_info.height,
                                   &gVidFileInfo.video_info.cropping_left, 
                                   &gVidFileInfo.video_info.cropping_top,
                                   &gVidFileInfo.video_info.cropping_right,
                                   &gVidFileInfo.video_info.cropping_bottom,
                                   &gVidFileInfo.video_info.bInterlacedSequence);
        #endif
    }
    #endif
	
	return 0;
}

#endif // End of BUILF_FW, including MKVPSR_API, Demuxer API, Track Op API

#if 0 // for IDE function name list by section
void _____MKV_MemMode_SeekByTime_Impl_____(){}
#endif

#ifndef BUILD_FW
/*extern MMP_ULONG            glAudioPlayReadPtr;         ///< The audio read pointer
extern MMP_ULONG            glAudioPlayWritePtr;        ///< The audio write pointer
extern MMP_ULONG            glAudioPlayReadPtrHigh;     ///< The audio read pointer lap
extern MMP_ULONG            glAudioPlayWritePtrHigh;    ///< The audio write pointer lap
extern MMP_ULONG            glAudioPlayBufStart;        ///< Start addr of buf
extern MMP_USHORT           glAudioPlayBufSize;         ///< buf size (should be 2^N)
extern MMP_UBYTE m_ucHandShakeBuf[64];
extern MMP_UBYTE *m_pHandShakeBuf1;*/

//TODO: move to mmpd_mkvparser.c
MMP_ERR MMPD_MKVPSR_SeekByTime_Impl(const MMP_ULONG ulTime, MMP_BOOL bSeekToPrecisePos)
{
#if 0
        MMP_ERR     error = MMP_ERR_NONE;
        MMP_ULONG   RequestFrame, tmpTime;
        MMP_ULONG   ulAudioTime = 0;
        
        MMPD_VIDPLAYER_Flush(gVidFileInfo.is_video_available, 
                             gVidFileInfo.is_audio_available);
        
        (m_ulBuffedAudioAtoms)        = 0;
        (glAudioPlayReadPtr)        = 0;
        (glAudioPlayWritePtr)       = 0;
        (glAudioPlayReadPtrHigh)    = 0;
        (glAudioPlayWritePtrHigh)   = 0;
        m_ulDecodedAudioAtoms = 0;
        MMPF_PSR_Reset();
        
        
        MMPH_HIF_MemCopyHostToDev(((MMP_ULONG)m_pHandShakeBuf1+BUF_AUDIO_ATOM_OFFSET), (MMP_UBYTE*)(&(m_ulBuffedAudioAtoms)),
                              sizeof(m_ulBuffedAudioAtoms));
        MMPH_HIF_MemCopyHostToDev(((MMP_ULONG)m_pHandShakeBuf1+AUDIO_READ_PTR_OFFSET), (MMP_UBYTE*)(&(glAudioPlayReadPtr)),
                                  sizeof(glAudioPlayReadPtr));
        MMPH_HIF_MemCopyHostToDev(((MMP_ULONG)m_pHandShakeBuf1+AUDIO_READ_HIGH_OFFSET), (MMP_UBYTE*)(&(glAudioPlayReadPtrHigh)),
                                  sizeof(glAudioPlayReadPtrHigh));
        MMPH_HIF_MemCopyHostToDev(((MMP_ULONG)m_pHandShakeBuf1+AUDIO_WRITE_PTR_OFFSET), (MMP_UBYTE*)(&(glAudioPlayWritePtr)),
                                  sizeof(glAudioPlayWritePtr));
        MMPH_HIF_MemCopyHostToDev(((MMP_ULONG)m_pHandShakeBuf1+AUDIO_WRITE_HIGH_OFFSET), (MMP_UBYTE*)(&(glAudioPlayWritePtrHigh)),
                                  sizeof(glAudioPlayWritePtrHigh));
        
        if(gVidFileInfo.is_video_available) {
            m_MkvOpState = MKV_SEEK_OP;
            MMPF_MKV_TrackSeekTime(&m_MkvContext,  m_pMkvVideoTrack, ulTime);
                    
            #if 0 //TODO
            if ((error != MMP_ERR_NONE) && (error != MMP_VIDPSR_ERR_EOS)) {
                return error;
            }
            #endif
            
            pSeek->ulRequestFrame   = 0;
            
            if ( !bSeekToPrecisePos ) {
		    	ulAudioTime = MMPF_MKV_TrackGetTime( &m_MkvContext, m_pMkvVideoTrack);
		    }else{
		    	tmpTime = MMPF_MKV_TrackGetTime( &m_MkvContext, m_pMkvVideoTrack);
                while(tmpTime < ulTime)
                {
                    gDemuxOption |= DEMUX_VIDEO;
                    error = MMPF_MKVPSR_Demux();
        	        if(error == MMP_VIDPSR_ERR_EOS) {
        	            break;
        	        }

                    MMPD_VIDPLAYER_DecodeData(&tmpTime);
                    MMPF_BS_ResetBuffer();
                }
                ulAudioTime = tmpTime;
			}
	
            MMPD_VIDPSR_SetParameter(MMPD_VIDPLAY_PARAMETER_VIDEO_FRAMENUM, CUR_DEMUXED_VIDEO_FRAME + 1);
            
            if(!gVidFileInfo.is_audio_available) {
           	    ulAudioTime = ulTime;
       	    }
        }
        else {// audio only
            ulAudioTime = ulTime;//Gason@20090307, memory mode with pure audio seek fail issue
        }
        
    	if(gVidFileInfo.is_audio_available) {
	    	m_pMkvAudioTrack->ulFrames = 0;
		    m_ulPreAudioReduntSize = 0;

	    	if(!gVidFileInfo.is_video_available){
	    		MMPF_MKV_TrackSeekTime(&m_MkvContext,  m_pMkvAudioTrack, ulAudioTime);
	    	}else{
	    		//20091123, champ, TODO: in mkv file, audio lacks cue info, here, we assume that, A/V are interleaved
	    		MMPF_MKV_FileSeek(m_pMkvAudioTrack->pMkvFile, MMPF_MKV_FilePosition(m_pMkvVideoTrack->pMkvFile) );
	    		m_pMkvAudioTrack->ullTrackTime = m_pMkvVideoTrack->ullTrackTime;
	    		m_pMkvVideoTrack->ullTrackTimeOffset = m_pMkvVideoTrack->ullTrackTimeOffset;
	    	}
        }
        
        MMPD_VIDPLAYER_SetCurrentTime(ulAudioTime);
#endif
        return MMP_ERR_NONE;
}

//return the size of codec private, and codec private is copy to destination buffer
static MMP_LONG MMPD_MKV_TrackOpen(
	MMPF_MKV_CONTEXT *pMkvCtx,
	MMPF_MKV_TRACK *pTrack,
	MMP_BYTE *pBuf,
	MMP_ULONG ulBuflen
){
	pTrack->pMkvFile = &m_gFiles;//MMPF_MKV_FileOpen();
	
	if(pTrack->ulCodecPrivateSize > 512){
	    DBG_S(0, "Temp Buffer is not enough!!\r\n");
	    return 0;
	}
	
	if(pTrack->ulCodecPrivateSize > 0 && pTrack->ullCodecPrivatePos > 0){
		MMPF_MKV_FileSeek(pTrack->pMkvFile, pTrack->ullCodecPrivatePos);
		MMPF_MKV_FileReadBytes(pTrack->pMkvFile, pBuf, pTrack->ulCodecPrivateSize);
		
	}

	MMPF_MKV_FileSeek(pTrack->pMkvFile, pMkvCtx->ullClustersPos);

	return pTrack->ulCodecPrivateSize;
}

MMP_LONG MMPD_MKV_TrackSeekFrame(
	MMPF_MKV_CONTEXT *pMkvCtx,
	MMPF_MKV_TRACK *pTrack,
	MMP_ULONG64 start,
	MMP_ULONG64 len
){
	MMPF_MKV_FILE *pMkvFile = pTrack->pMkvFile;
	MMP_ULONG64 ullTrack;
	MMP_LONG timecode;
	MMP_UBYTE flags;
	MMP_UBYTE framecnt;
	MMP_ULONG64 data_len;
	MMP_ULONG *pLacingSize = pTrack->ulLacingSize;

	MMP_LONG m, i;

	ullTrack = MMPF_MKV_EBML_ReadVuint(pMkvFile, &m, 0);
	
	if(ullTrack != pTrack->ullNumber){
		MMPF_MKV_Message("%s 1 %llx %lx\n", __func__, len, m);
		MMPF_MKV_FileSkipBytes(pMkvFile, len - (MMP_ULONG64)(m+1));
		return 0;
	}
	
	pTrack->ulFrameIndex++;
	
	timecode = MMPF_MKV_EBML_ReadSint(pMkvFile, 2);
	flags = MMPF_MKV_EBML_ReadUint(pMkvFile, 1);
	MMPF_MKV_Message("%s block track #: %lld timecode %d flags: %X\n", __func__, ullTrack, timecode, flags);

	data_len = 0;
	switch((flags & 0x06) >> 1){
		case MKV_LACING_FIXED:
			framecnt = MMPF_MKV_EBML_ReadUint(pMkvFile, 1);
			MMPF_MKV_Message("%s f framecnt = %d\n", __func__, framecnt+1);
			framecnt ++;
			data_len = ((start + len) - (MMPF_MKV_FilePosition(pMkvFile) + data_len))/framecnt;
			for(i = 0; i < framecnt; i++)
				pLacingSize[i] = data_len;
			data_len = 0;	//reset data_len, it will be added later;
			break;
		case MKV_LACING_NONE:
			framecnt = 1;
			break;
		case MKV_LACING_XIPH:
			framecnt = MMPF_MKV_EBML_ReadUint(pMkvFile, 1);
			MMPF_MKV_Message("%s x framecnt = %d\n", __func__, framecnt+1);
			for(i = 0; i < framecnt; i++){
				MMP_UBYTE val;
				pLacingSize[i] = 0;
				val = 255;
				do{
					val = MMPF_MKV_EBML_ReadUint(pMkvFile, 1);
					pLacingSize[i] += val;
				}while(val == 255);
				MMPF_MKV_Message("%s frame#%d size %X\n", __func__, i, pLacingSize[i]);
				data_len += pLacingSize[i];
			}
			pLacingSize[i] = (start + len) - (MMPF_MKV_FilePosition(pMkvFile) + data_len);
			MMPF_MKV_Message("%s x frame#%d size %X\n", __func__, i, pLacingSize[i]);
			//exit(0);
			framecnt ++;
			break;
		case MKV_LACING_EBML:
			framecnt = MMPF_MKV_EBML_ReadUint(pMkvFile, 1);
			MMPF_MKV_Message("%s l framecnt = %d\n", __func__, framecnt+1);
			i = 0;
			data_len = pLacingSize[0] = MMPF_MKV_EBML_ReadVuint(pMkvFile, &m, 0);
			MMPF_MKV_Message("%s l frame#%d size %X\n", __func__, i, pLacingSize[i]);
			for(i = 1; i < framecnt; i++){
				pLacingSize[i] = pLacingSize[i-1] + MMPF_MKV_EBML_ReadVsint(pMkvFile);
				data_len += pLacingSize[i];
				MMPF_MKV_Message("%s frame#%d size %X\n", __func__, i, pLacingSize[i]);
			}
			pLacingSize[i] = (start + len) - (MMPF_MKV_FilePosition(pMkvFile) + data_len);
			MMPF_MKV_Message("%s l frame#%d size %X\n", __func__, i, pLacingSize[i]);
			framecnt ++;
			break;
	}
	data_len += (start + len) - (MMPF_MKV_FilePosition(pMkvFile) + data_len);
	pTrack->ulFrames = framecnt;
	pTrack->ullTrackTimeOffset = timecode;

	MMPF_MKV_Message("%s data_len %llX %llX\n", __func__, data_len, MMPF_MKV_FilePosition(pMkvFile));

	pTrack->ullFramePos = MMPF_MKV_FilePosition(pMkvFile);
	pTrack->ulFrameSize = data_len;

	MMPF_MKV_Message("%s 2 %llx %llx\n", __func__, data_len, m);
	MMPF_MKV_FileSkipBytes(pMkvFile, data_len);
	
	return data_len;
}

MMP_LONG MMPD_MKV_TrackSeekNextFrame(
	MMPF_MKV_CONTEXT *pMkvCtx,
	MMPF_MKV_TRACK *pTrack,
	MMP_ULONG key
){
	MMPF_MKV_EBML_ELEMENT element;
	MMP_ULONG64 ullGroupStart, ullGroupSize;
	MMPF_MKV_FILE *pMkvFile = pTrack->pMkvFile;
	MMP_ULONG ulFrameSize = 0;
	MMP_ULONG ulKeyOnly = key;
	MMP_UBYTE ubKeyFrame = 1;

	if(pMkvFile == NULL)	//track not opened
		return -1;
	
	MMPF_MKV_Message("get nextframe from %llX\n", MMPF_MKV_FilePosition(pMkvFile) );
	
	pTrack->ulFrames = 0;
	pTrack->ulFrameSize = 0;
	
	while(MMPF_MKV_EBML_ReadElement(pMkvFile, &element, pMkvCtx->ullClustersPos, pMkvCtx->ullClustersSize)){
		switch(element.ulID){
			case MKV_CLUSTER:
				//just bypass id/size
				break;
			case MKV_CLUSTER_TIME_CODE:	//time info.
				pTrack->ullTrackTime = MMPF_MKV_EBML_ReadUint(pMkvFile, (MMP_ULONG)element.ullSize);
				pTrack->ullTrackTimeOffset = 0;
				break;
			case MKV_CLUSTER_BLOCK_GROUP:
				ullGroupStart = element.ullDataPos;
				ullGroupSize = element.ullSize;
				pTrack->ullFrameBlockStart = element.ullStart;
				MMPF_MKV_Message("%s gstart %llx gsize %llx\n", __func__, ullGroupStart, ullGroupSize);
				while(MMPF_MKV_EBML_ReadElement(pMkvFile, &element, ullGroupStart, ullGroupSize)){
					switch(element.ulID){
						case MKV_BLOCK:
							ulFrameSize = MMPD_MKV_TrackSeekFrame(pMkvCtx, pTrack, element.ullDataPos, element.ullSize);
							if(ulFrameSize == 0)
								pTrack->ullHintPos = ullGroupStart;
							break;
						case MKV_BLOCK_DURATION:	//time info.
							MMPF_MKV_Message("%s :MKV_BLOCK_DURATION %llx, %llx\n", __func__, element.ullDataPos, element.ullSize);
							MMPF_MKV_FileSkipBytes(pMkvFile, element.ullSize);
							//vsint = MMPF_MKV_EBML_ReadSint(pMkvFile, (MMP_ULONG)element.ullSize);
							break;
						case MKV_REFERENCE_BLOCK:
							MMPF_MKV_Message("%s :MKV_REFERENCE_BLOCK %llx, %llx\n", __func__, element.ullDataPos, element.ullSize);
							ubKeyFrame = 0;
							MMPF_MKV_FileSkipBytes(pMkvFile, element.ullSize);
							break;
						default:
							MMPF_MKV_Message("%s :unknown block group %lx %llx, %llx\n", __func__,element.ulID, element.ullDataPos, element.ullSize);
							MMPF_MKV_FileSkipBytes(pMkvFile, element.ullSize);
							break;
					}
				}
				break;
			case MKV_CLUSTER_SIMPLE_BLOCK:
				pTrack->ullFrameBlockStart = element.ullStart;
				ulFrameSize = MMPD_MKV_TrackSeekFrame(pMkvCtx, pTrack, element.ullDataPos, element.ullSize);
				break;
			default:
				//skip non-block elements
				MMPF_MKV_Message("%s :unknown block %lx %llx, %llx\n", __func__, element.ulID, element.ullDataPos, element.ullSize);
				MMPF_MKV_FileSkipBytes(pMkvFile, element.ullSize);
		}
		if(ulFrameSize > 0){
			if(ulKeyOnly != 0 && ubKeyFrame == 0){
				ulFrameSize = 0;
				ubKeyFrame = 1;
				continue;
			}else{
				//got required frame
				break;
			}
		}			
	}
	
	return ulFrameSize;
}

MMP_LONG MMPD_MKV_TrackReadFrame(
	MMPF_MKV_CONTEXT *pMkvCtx,
	MMPF_MKV_TRACK *pTrack,
	MMP_UBYTE *pBuf,
	MMP_ULONG ulRead
){
	MMPF_MKV_FILE *pFile = pTrack->pBSFile;	
	MMPF_MKV_FileSeek(pFile, pTrack->ullFramePos);
	MMPF_MKV_FileReadBytes(pFile, pBuf, ulRead);
	pTrack->ullFramePos = MMPF_MKV_FilePosition(pFile);	//for large frame, may read several times
	return ulRead;
}

static MMPF_MKV_TRACK* MMPD_MKV_GetTrackInfo(
	MMPF_MKV_CONTEXT *pMkvCtx,
	MMP_LONG lTrackIdx
){
	if(lTrackIdx >= pMkvCtx->ullNumTracks)
		return NULL;
	else
		return &(pMkvCtx->tracks[lTrackIdx]);
}

static MMP_ERR MMPF_MKVPSR_DecodeMP4ASC(MMP_UBYTE tmp1, MMP_UBYTE tmp2)
{
	mp4ASC.objectTypeIndex = (MMP_UBYTE)(tmp1 >> 3);
	mp4ASC.samplingFrequencyIndex = ((tmp1 & 0x07) << 1) | (tmp2 >> 7);
	mp4ASC.channelsConfiguration = (tmp2 & 0x78) >> 3;

	mp4ASC.samplingFrequency = MMPF_3GPPSR_GetSampleRate(mp4ASC.samplingFrequencyIndex);
	mp4ASC.sample256_time = MMPF_3GPPSR_Get256SampleTime(mp4ASC.samplingFrequencyIndex);
	if (mp4ASC.samplingFrequency == 0) {
		return	MMP_VIDPSR_ERR_UNSUPPORTED_SETTING;
	}
	
	if (mp4ASC.channelsConfiguration > 7 ) {
		return	MMP_VIDPSR_ERR_UNSUPPORTED_SETTING;
	}

	if (mp4ASC.objectTypeIndex == 2) {
		mp4ASC.frameLengthFlag = (tmp2 & 0x04) >> 2;

		if (mp4ASC.frameLengthFlag == 1) {
			return	MMP_VIDPSR_ERR_UNSUPPORTED_SETTING;
		}    
	}
	else {
		return	MMP_VIDPSR_ERR_UNSUPPORTED_SETTING;
	}
	#if	1
		MMPF_MKV_Message("[%s] mp4ASC.samplingFrequency: %ld\n", __func__, mp4ASC.samplingFrequency);
	#endif
    return MMP_ERR_NONE;
}

MMP_VID_FILEVALIDATION MMPD_MKVPSR_GetMKVInfo(void *pmkvinfo, MMP_BOOL bInitVideoCodec)
{

	MMP_ULONG i, ulRead;
	MMPF_MKV_TRACK *pTrack;
    MMP_UBYTE               *dest_buf = (MMP_UBYTE*)m_ulHostPSRBSAddr;
    MMP_VIDEO_CONTAINER_INFO *pContainInfo = (MMP_VIDEO_CONTAINER_INFO *)pmkvinfo;
    MMP_LONG ulParseOK = 0;
    

	MMPF_MKV_Message("[%s] MKV Init.\n", __func__);

	//Parser Initialization
	MEMSET0(&m_gMkvContext);
	MEMSET0(&m_gFiles);
	m_gMkvContext.pFile = MMPD_MKV_FileOpen(pContainInfo->file_size);
	
	ulParseOK = MMPF_MKV_Parse(&m_gMkvContext);
	if(ulParseOK != 0)
	    return MMP_VID_UNKNOWN_FILE_HEADER;
	
	//Currently, only support H.264 & AAC/AC3/MP3
	pContainInfo->is_video_seekable = 0;
    pContainInfo->drm_type = 0;    
    
	m_gMkvContext.seg.ullDuration /= 1000000ull; //ms
	
	pContainInfo->total_file_time = m_gMkvContext.seg.ullDuration;
	for( i = 0; i < m_gMkvContext.ullNumTracks; i++){
		pTrack = MMPD_MKV_GetTrackInfo(&m_gMkvContext, i);
		switch(pTrack->ullType){
			case MKV_TRACK_VIDEO:
				MMPF_MKV_Message("track: %ld is a video track\n", i);
				
				ulRead = MMPD_MKV_TrackOpen( &m_gMkvContext, pTrack, (MMP_BYTE*)m_TempBSBuffer, 1);
				pContainInfo->is_video_available = 1;
				pContainInfo->video_info.total_video_time = m_gMkvContext.seg.ullDuration; //sec
				pContainInfo->video_info.width = 0;
				pContainInfo->video_info.height = 0;

				if(strncmp(pTrack->pCodecID, "V_MS/VFW/FOURCC", 15) == 0){
					MMP_ULONG ulCodecTagIdx;
					MMP_ULONG64 tmpPos;
					BitmapInfoHeader *bmpInfo;
					bmpInfo = (BitmapInfoHeader*)dest_buf;
					
					for(ulCodecTagIdx = 0; fourcc_tag[ulCodecTagIdx][1] != 0; ulCodecTagIdx++){
						if(bmpInfo->biCompression == fourcc_tag[ulCodecTagIdx][1]){
		                    pContainInfo->video_info.format   = fourcc_tag[ulCodecTagIdx][0];
						}
					}
					if(pContainInfo->video_info.format == MMP_VID_VIDEO_NONE)
					    pContainInfo->video_info.format = MMP_VID_UNSUPPORTED_FILE;
					
					if(bInitVideoCodec) {
    					tmpPos = MMPF_MKV_FilePosition(pTrack->pMkvFile);
    					
    					//read first frame for codec initialization
    					MMPD_MKV_TrackSeekNextFrame(&m_gMkvContext, pTrack, 0);
    					MMPD_MKV_TrackReadFrame(&m_gMkvContext, pTrack, (MMP_UBYTE*)dest_buf, pTrack->ulFrameSize);
    					ulRead = pTrack->ulFrameSize;
    		
                        //roll-back, for first frame
                        MMPF_MKV_FileSeek(pTrack->pMkvFile, tmpPos);
                    }
				}else if(strncmp(pTrack->pCodecID, "V_MPEG4/ISO/ASP", 15) == 0){
					print_msg("track: %ld is a MPEG4 track\n", i);
                    pContainInfo->video_info.format   = MMP_VID_VIDEO_MP4V_SP;
                   
				}else if(strncmp(pTrack->pCodecID, "V_MPEG4/ISO/AVC", 15) == 0){
					pContainInfo->video_info.format = MMP_VID_VIDEO_H264;
					print_msg("track: %ld is a H264 track\n", i);
					
				}else{
					pContainInfo->is_video_available = 0;
				}
				
				if(pContainInfo->is_video_available != 0 && bInitVideoCodec){
					switch(pContainInfo->video_info.format){
						case MMP_VID_VIDEO_MP4V_SP:
							#if 0//TBD
		                    if (MMP_ERR_NONE != MMPD_VIDPLAYER_InitVideoDec(
									m_ulHostPSRBufAddr,
									dest_buf, 
									ulRead,
									&pContainInfo->video_info.width,
									&pContainInfo->video_info.height,
									&pContainInfo->video_info.cropping_left, 
									&pContainInfo->video_info.cropping_top,
									&pContainInfo->video_info.cropping_right,
									&pContainInfo->video_info.cropping_bottom,
		                            MMP_VID_VIDEO_MP4V_SP)) {
		                        
		                        return MMP_VID_UNSUPPORTED_FILE;
		                    }
		                    #endif
							break;
						case MMP_VID_VIDEO_H264:

							pContainInfo->video_info.width    = 0;
							pContainInfo->video_info.height   = 0;

			                #if 0//TBD
							if (MMP_ERR_NONE != MMPD_VIDPLAYER_InitVideoDec(
									m_ulHostPSRBufAddr,
									dest_buf, 
									ulRead,
									&pContainInfo->video_info.width,
									&pContainInfo->video_info.height,
									&pContainInfo->video_info.cropping_left, 
									&pContainInfo->video_info.cropping_top,
									&pContainInfo->video_info.cropping_right,
									&pContainInfo->video_info.cropping_bottom,
									MMP_VID_VIDEO_H264)) {
								return MMP_VID_UNSUPPORTED_FILE;
							}
							#endif
							
							pContainInfo->video_info.width =  pContainInfo->video_info.width - 
								pContainInfo->video_info.cropping_left -
								pContainInfo->video_info.cropping_right;
			                                                            
							pContainInfo->video_info.height = pContainInfo->video_info.height - 
								pContainInfo->video_info.cropping_top -
								pContainInfo->video_info.cropping_bottom;
							break;
		                default:
		                	pContainInfo->is_video_available = 0;
					}
				} else if(pContainInfo->is_video_available != 0) {
				    pContainInfo->video_info.width = pTrack->AV.VIDEO.ulPixelWidth -
				                            pTrack->AV.VIDEO.ulCropL - pTrack->AV.VIDEO.ulCropR;
				    pContainInfo->video_info.height = pTrack->AV.VIDEO.ulPixelHeight -
				                            pTrack->AV.VIDEO.ulCropT - pTrack->AV.VIDEO.ulCropB;
				}
				if(pTrack->ullDefaultDuration != 0)
				    pContainInfo->video_info.fps = (1000000000ull+(pTrack->ullDefaultDuration/2))/pTrack->ullDefaultDuration;
				if(m_gMkvContext.ullCuesPos)
				    pContainInfo->is_video_seekable = 1;
				break;
			case MKV_TRACK_AUDIO:
				
				ulRead = MMPD_MKV_TrackOpen( &m_gMkvContext, pTrack, (MMP_BYTE*)m_ASCBuffer, 1);
				pContainInfo->is_audio_available = 1;
				pContainInfo->audio_info.total_audio_time = (MMP_ULONG)(m_gMkvContext.seg.ullDuration); // sec
				pContainInfo->audio_info.sampling_rate = (MMP_ULONG) MMPF_MKV_FloatMult(pTrack->AV.AUDIO.fSamplingFreq, 1);
				if(strncmp(pTrack->pCodecID, "A_AAC", 5) == 0){
					pContainInfo->audio_info.format = MMP_VID_AUDIO_AAC_LC;
					MMPF_MKVPSR_DecodeMP4ASC(m_ASCBuffer[0], m_ASCBuffer[1]);					
					pContainInfo->audio_info.sampling_rate = mp4ASC.samplingFrequency;
				}else if(strncmp(pTrack->pCodecID, "A_AC3", 5) == 0){
					pContainInfo->audio_info.format = MMP_VID_AUDIO_AC3;					
				}else if(strncmp(pTrack->pCodecID, "A_MPEG/L3", 9) == 0){
					pContainInfo->audio_info.format = MMP_VID_AUDIO_MP3;						
				}else{
					pContainInfo->is_audio_available = 0;
				}
				break;
		}
	}
	
	
	return 0;
}

#endif // End of #ifndef BUILD_FW

#endif	//_MMP_MKV_PARSER_H_


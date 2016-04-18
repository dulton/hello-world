/// @ait_only
//==============================================================================
//
//  File        : mmpd_exif.c
//  Description : EXIF function
//  Author      : Rogers Chen
//  Revision    : 1.0
//
//==============================================================================

/**
*  @file mmpd_exif.c
*  @brief The EXIF functions
*  @author Rogers Chen
*  @version 1.0
*/

#include "ait_utility.h"
#include "mmpd_exif.h"

#if 0		 
void _____NO_USING_START_____(){}
#endif

#if 0

#define DUMP_EXIF_EN 1

#define EXIF_DEC_SIMPLIFY 1

#define EXIF_IMG_DESC  "S/W Ver:1.0" 
#define EXIF_MAKER     "AIT  "
#define EXIF_MODEL     "Demo Kit"
#define EXIF_SOFTWARE  "AIT Corp"
#define EXIF_ARTIST    "AIT Corp"
#define EXIF_COPYRIGHT "AIT Corp"

typedef enum _MMPD_EXIF_TIFTAGTYPE {
    TYPE_UNKNOWN = 0,
    TYPE_BYTE,
    TYPE_ASCII,
    TYPE_SHORT,
    TYPE_LONG,
    TYPE_RATIONAL,
    TYPE_SBYTE,
    TYPE_UNDEFINED = 7,
    TYPE_SSHORT = 8,
    TYPE_SLONG = 9,
    TYPE_SRATIONAL,
    TYPE_FLOAT,
    TYPE_DOUBLE
} MMPD_EXIF_TIFTAGTYPE;

static MMP_ULONG exif_res[2] = {
              72, 1
          } ;
static MMP_USHORT exif_res_unit[2] = {
              0xFFFF, 2
          } ;
static MMP_USHORT ycc_pos[2] = {
              0xFFFF, 1
          } ;
static MMP_USHORT exif_compression[2] = {
              0xFFFF, 0x6
          } ;
static MMP_USHORT exif_colorspace[2] = {
              0xFFFF, 0x1
          } ;
static MMP_USHORT exif_orientation[2] = {
              0xFFFF, 0
          } ;
static MMP_BYTE exif_ver[] = {
              0x30, 0x31, 0x32, 0x30, 0, 0, 0, 0
          } ;
static MMP_BYTE exif_create_time[20] ;


#define set_double(var,val) var[0] = ((val) >> 8 ) & 0xff ; \
                            var[1] = ((val)      ) & 0xff

#define set_triple(var,val) var[0] = ((val) >> 16) & 0xff ; \
                            var[1] = ((val) >> 8 ) & 0xff ; \
                            var[2] = ((val)      ) & 0xff 

#define set_quad(var,val)   var[0] = ((val) >> 24) & 0xff ; \
                            var[1] = ((val) >> 16) & 0xff ; \
                            var[2] = ((val) >> 8 ) & 0xff ; \
                            var[3] = ((val)      ) & 0xff 

#define get_double(var) ((*var << 8) + *(var + 1))
#define get_triple(var) ((*var << 16) +     \
                         (*(var + 1)<<8) + *(var+2))

#define get_quad(var)   ( (*var << 24) + \
                        (*(var + 1) << 16) + \
                        (*(var + 2) <<  8) + *(var + 3))

// sean@2007_11_14 add

#define set_double_ii(var,val)  var[0] = ((val)      ) & 0xff ; \
                                var[1] = ((val) >> 8 ) & 0xff

#define set_triple_ii(var,val)  var[0] = ((val)      ) & 0xff ; \
                                var[1] = ((val) >> 8 ) & 0xff ; \
                                var[2] = ((val) >> 16) & 0xff 

#define set_quad_ii(var,val)    var[0] = ((val)      ) & 0xff ; \
                                var[1] = ((val) >> 8 ) & 0xff ; \
                                var[2] = ((val) >> 16) & 0xff ; \
                                var[3] = ((val) >> 24) & 0xff 
#if (!EXIF_DEC_SIMPLIFY)
#define EXIF_DATA_BUF_SIZE 4096
MMP_UBYTE gExifDataBuffer4Dec[EXIF_DATA_BUF_SIZE]; 
MMP_UBYTE *gExifDataEnd; 
#endif

static MMPD_EXIF_CONTEXT *gExifCtx;
static MMPD_EXIF_DATA gExifData;
static MMP_UBYTE *gExifDataBuffer; 
static MMP_UBYTE *gExifDataPtr; 

static MMPD_EXIF_HEADER ExifHeaderEncode = {
    EXIF_SOI, EXIF_APP1_MARKER, 0xbeef, EXIF_ID, 0x0000
} ;

static MMPD_EXIF_TIFHEADER TifHeaderEncode = {
    TIF_MM_FORMAT, 0x002a, 0x00000008
};

static MMPD_EXIF_TIFIFD IfdImage[3];


//////////////////////////////////////////////////
// Function

MMP_ULONG  (*tif_get_long)(MMP_UBYTE *ptr);
MMP_USHORT (*tif_get_short)(MMP_UBYTE *ptr);
void  (*tif_set_long)(MMP_UBYTE *ptr, MMP_ULONG val);
void  (*tif_set_short)(MMP_UBYTE *ptr, MMP_USHORT val);

static MMP_ULONG tif_get_long_be(MMP_UBYTE *ptr)
{
	return get_quad(ptr) ;
}

static MMP_USHORT tif_get_short_be(MMP_UBYTE *ptr)
{
	return get_double(ptr);
}

static MMP_ULONG tif_get_long_le(MMP_UBYTE *ptr)
{
    MMP_ULONG lle;    
    lle = (*ptr) + (*(ptr + 1) << 8) +  (*(ptr + 2)<<16) + (*(ptr + 3) <<24 );
    return lle;
}

static MMP_USHORT tif_get_short_le(MMP_UBYTE *ptr)
{
    MMP_USHORT sle;    
    sle = (*ptr) + (*(ptr + 1) << 8);
    return sle;   
}


static void tif_set_long_be(MMP_UBYTE *ptr, MMP_ULONG val)
{
	set_quad(ptr,val);
}

static void tif_set_short_be(MMP_UBYTE *ptr, MMP_USHORT val)
{
	set_double(ptr,val);
}


static void tif_set_long_le(MMP_UBYTE *ptr, MMP_ULONG val)
{
	set_quad_ii(ptr,val);
}

static void tif_set_short_le(MMP_UBYTE *ptr, MMP_USHORT val)
{
	set_double_ii(ptr,val);
}

////////////////////////////////////////////////
// for EXIF Encoder

void MMPD_EXIF_Init(void)
{
	MMP_LONG i, j;
	gExifData.ubData = 0 ;
	gExifData.lIfdNum = 0;
	gExifData.lExifIfdFound  = 0;
	gExifData.lExifIfdOffset = 0;
	gExifData.ubThumbnailData = 0 ;
	gExifData.ulThumbnailLen = 0 ;

	for(i=0;i<EXIF_IFD_MAX;i++) {
		gExifData.Ifd[i] = NULL;
	}	

	for(i=0;i<3;i++) {
		IfdImage[i].usFields = 0;
		IfdImage[i].ulNextIfd = 0;
		for(j=0;j<EXIF_TAG_MAX;j++) 
			IfdImage[i].Tag[j] = NULL;
	}	
}


static MMP_ERR MMPD_EXIF_InsertIfd(MMPD_EXIF_TIFIFD *Ifd)
{
	if(!Ifd) {
		return MMP_DSC_ERR_EXIF_ENC;
	}

	if(gExifData.lIfdNum >= EXIF_IFD_MAX) {
		return MMP_DSC_ERR_EXIF_ENC;
	}
	gExifData.Ifd[gExifData.lIfdNum++] = Ifd;
	return MMP_ERR_NONE;
}

static MMP_ERR MMPD_EXIF_InsertTag(MMP_USHORT usIndex, MMPD_EXIF_TIFTAG *Tag)
{
	MMPD_EXIF_TIFIFD *Ifd ;
	if(usIndex >= gExifData.lIfdNum) {
		return MMP_DSC_ERR_EXIF_ENC;
	}

	if( !Tag ) {
		return MMP_DSC_ERR_EXIF_ENC;
	}
	Ifd = gExifData.Ifd[usIndex] ;
	
	if(!Ifd) {
		return MMP_DSC_ERR_EXIF_ENC;
	}

	if(Ifd->usFields >= EXIF_TAG_MAX) {
		return MMP_DSC_ERR_EXIF_ENC;
	}

	Ifd->Tag[Ifd->usFields++] = Tag;
	Ifd->ulNextIfd = 0 ;
	return MMP_ERR_NONE;
}

static MMP_ERR MMPD_EXIF_FillTag(MMPD_EXIF_TIFTAG *Tag, MMP_USHORT usTag, MMP_USHORT usType,void *val)
{
	char *ptr = (char *)val;
	
	if(!Tag) {
		return MMP_DSC_ERR_EXIF_ENC;
	}
	
	Tag->usTag  = usTag;
	Tag->usType = usType;	
	Tag->ulCount = 1;//get_tif_unit(type);
	if(Tag->usType == TYPE_ASCII) {
		
		Tag->ulCount = ( strlen(ptr) + 1 + 3) & (-4) ;
		if(Tag->ulCount < 4)
			Tag->ulCount = 4;
    }
    else if(Tag->usType == TYPE_UNDEFINED){
        Tag->ulCount = strlen(ptr);
    }
    Tag->ulValOffset = (MMP_ULONG)val; // address
    return MMP_ERR_NONE;
}

static MMP_BOOL MMPD_EXIF_FillTime(void)
{
#ifdef USING_AIT_FS
    sprintf(exif_create_time, "%4d-%02d-%02d %02d:%02d:%02d", gExifCtx->CreateTime.year,
            gExifCtx->CreateTime.month, gExifCtx->CreateTime.day, gExifCtx->CreateTime.hour,
            gExifCtx->CreateTime.min, gExifCtx->CreateTime.sec) ;

    return MMP_TRUE;
#else
    STRCPY(exif_create_time, "2008-01-01 01:01:01");

    return MMP_FALSE;
#endif
}

static MMP_LONG MMPD_EXIF_GetTifUnit(MMP_USHORT usType)
{
	switch(usType) {
	case TYPE_BYTE:
	case TYPE_SHORT:
	case TYPE_LONG:
	case TYPE_SLONG:
	//case TYPE_UNDEFINED:
		return 4;
	case TYPE_RATIONAL:
	case TYPE_SRATIONAL:
		return 8;
	default:
		return 1 ;
	}
}

static MMP_ERR MMPD_EXIF_ConfigImage(void)
{
    static MMPD_EXIF_TIFTAG ImageTag[EXIF_TAG_MAX];
    MMPD_EXIF_TIFIFD *Ifd;
    MMP_BOOL bTimeSup = MMP_FALSE;
	
    Ifd = gExifData.Ifd[0] ;
    if(!Ifd){
        return MMP_DSC_ERR_EXIF_ENC;
    }
	
    if(gExifCtx->usOrientation == 0){
        // portrait mode
        exif_orientation[1] = 1;  // Citrine
    }
    else{
        // landscape mode
        exif_orientation[1] = 1;
    }
    bTimeSup = (MMP_BOOL)MMPD_EXIF_FillTime();
    if(gExifCtx->usImgDesc){
        MMPD_EXIF_FillTag(&ImageTag[0], 0x010E, TYPE_ASCII, (void *) gExifCtx->usImgDesc);
    }
    else{
        MMPD_EXIF_FillTag(&ImageTag[0], 0x010E, TYPE_ASCII, (void *) EXIF_IMG_DESC);
    }
    MMPD_EXIF_FillTag(&ImageTag[1], 0x010F, TYPE_ASCII, (void *) EXIF_MAKER);
    MMPD_EXIF_FillTag(&ImageTag[2], 0x0110, TYPE_ASCII, (void *) EXIF_MODEL);
    MMPD_EXIF_FillTag(&ImageTag[3], 0x0112, TYPE_SHORT, (void *) exif_orientation);
    MMPD_EXIF_FillTag(&ImageTag[4], 0x011A, TYPE_RATIONAL, (void *) exif_res);
    MMPD_EXIF_FillTag(&ImageTag[5], 0x011B, TYPE_RATIONAL, (void *) exif_res);
    MMPD_EXIF_FillTag(&ImageTag[6], 0x0128, TYPE_SHORT, (void *) exif_res_unit);
    MMPD_EXIF_FillTag(&ImageTag[7], 0x0131, TYPE_ASCII, (void *) EXIF_SOFTWARE);
    MMPD_EXIF_FillTag(&ImageTag[8], 0x013B, TYPE_ASCII, (void *) EXIF_ARTIST);
    MMPD_EXIF_FillTag(&ImageTag[9], 0x0213, TYPE_SHORT, (void *) ycc_pos);
    MMPD_EXIF_FillTag(&ImageTag[10], 0x8298, TYPE_ASCII, (void *) EXIF_COPYRIGHT);
    MMPD_EXIF_FillTag(&ImageTag[11], 0x8769, TYPE_LONG, (void *) 0); // not decide yet !

    if(bTimeSup){
        MMPD_EXIF_FillTag(&ImageTag[12], 0x0132, TYPE_ASCII, (void *) exif_create_time);
    }
    MMPD_EXIF_InsertTag(0, &ImageTag[0]);
    MMPD_EXIF_InsertTag(0, &ImageTag[1]);
    MMPD_EXIF_InsertTag(0, &ImageTag[2]);
    MMPD_EXIF_InsertTag(0, &ImageTag[3]);
    MMPD_EXIF_InsertTag(0, &ImageTag[4]);
    MMPD_EXIF_InsertTag(0, &ImageTag[5]);
    MMPD_EXIF_InsertTag(0, &ImageTag[6]);
    MMPD_EXIF_InsertTag(0, &ImageTag[7]);
    MMPD_EXIF_InsertTag(0, &ImageTag[8]);
    MMPD_EXIF_InsertTag(0, &ImageTag[9]);
    MMPD_EXIF_InsertTag(0, &ImageTag[10]);
    MMPD_EXIF_InsertTag(0, &ImageTag[11]);

    if(bTimeSup){
        MMPD_EXIF_InsertTag(0, &ImageTag[12]);
    }
    return MMP_ERR_NONE;
}

static MMP_ERR MMPD_EXIF_ConfigThumbnail(void)
{
	static MMPD_EXIF_TIFTAG ThumbnailTag[EXIF_TAG_MAX];
	static MMP_ULONG ulThumbnailW = 0;
	static MMP_ULONG ulThumbnailH = 0;
	MMPD_EXIF_TIFIFD *Ifd;
	
	Ifd = gExifData.Ifd[1];
	if(!Ifd) {
		return MMP_DSC_ERR_EXIF_ENC;
	}
	
	ulThumbnailW = gExifCtx->usThumbnailW;
	MMPD_EXIF_FillTag(&ThumbnailTag[0],0x0100 ,TYPE_LONG,(void *)&ulThumbnailW);
	ulThumbnailH = gExifCtx->usThumbnailH;
	MMPD_EXIF_FillTag(&ThumbnailTag[1],0x0101 ,TYPE_LONG,(void *)&ulThumbnailH);
	MMPD_EXIF_FillTag(&ThumbnailTag[2],0x0103 ,TYPE_SHORT,(void *)exif_compression);
	MMPD_EXIF_FillTag(&ThumbnailTag[3],0x011A ,TYPE_RATIONAL,(void *)exif_res);
	MMPD_EXIF_FillTag(&ThumbnailTag[4],0x011B ,TYPE_RATIONAL,(void *)exif_res);
	MMPD_EXIF_FillTag(&ThumbnailTag[5],0x0128 ,TYPE_SHORT,(void *)exif_res_unit);
	MMPD_EXIF_FillTag(&ThumbnailTag[6],0x0201 ,TYPE_LONG,(void *)0);// not decide yet !
	MMPD_EXIF_FillTag(&ThumbnailTag[7],0x0202 ,TYPE_LONG,(void *)&gExifCtx->ulThumbnailSize); 
	MMPD_EXIF_InsertTag(1,&ThumbnailTag[0]);
	MMPD_EXIF_InsertTag(1,&ThumbnailTag[1]);
	MMPD_EXIF_InsertTag(1,&ThumbnailTag[2]);
	MMPD_EXIF_InsertTag(1,&ThumbnailTag[3]);
	MMPD_EXIF_InsertTag(1,&ThumbnailTag[4]);
	MMPD_EXIF_InsertTag(1,&ThumbnailTag[5]);
	MMPD_EXIF_InsertTag(1,&ThumbnailTag[6]);
	MMPD_EXIF_InsertTag(1,&ThumbnailTag[7]);

	return MMP_ERR_NONE;
}

static MMP_ERR MMPD_EXIF_ConfigCamera(void)
{
	static MMPD_EXIF_TIFTAG CameraTag[EXIF_TAG_MAX];
	static MMP_ULONG PrimaryW = 0;
	static MMP_ULONG PrimaryH = 0;
	MMPD_EXIF_TIFIFD *Ifd;
	
	Ifd = gExifData.Ifd[2];
	if(!Ifd) {
		return MMP_DSC_ERR_EXIF_ENC;
	}
	
    MMPD_EXIF_FillTag(&CameraTag[0], 0x9000, TYPE_UNDEFINED, (void *) &exif_ver);
    MMPD_EXIF_FillTag(&CameraTag[1], 0xA001, TYPE_SHORT, (void *) exif_colorspace); 
    PrimaryW = gExifCtx->usPrimaryW;
    MMPD_EXIF_FillTag(&CameraTag[2], 0xA002, TYPE_LONG, (void *) &PrimaryW);
    PrimaryH = gExifCtx->usPrimaryH;
    MMPD_EXIF_FillTag(&CameraTag[3], 0xA003, TYPE_LONG, (void *) &PrimaryH); 
	
	MMPD_EXIF_InsertTag(2,&CameraTag[0]);
	MMPD_EXIF_InsertTag(2,&CameraTag[1]);
	MMPD_EXIF_InsertTag(2,&CameraTag[2]);
	MMPD_EXIF_InsertTag(2,&CameraTag[3]);
	
	return MMP_ERR_NONE;
}

static MMP_ERR MMPD_EXIF_Build(MMP_ULONG *ulExifLen)
{
	MMP_ULONG ulNotFilled = 0xdeadbeef; 
	MMP_ULONG ulEndIfd = 0; 
	MMP_UBYTE *ubSizeOver4Ptr; 
	MMP_UBYTE *ubExifDataPtr8769 = 0; 
	MMP_UBYTE *ubExifDataPtr0201 = 0; 
	MMP_ULONG i,j;
    MMP_LONG ulSize = 0, ulSizeOver4Offset; 
	MMPD_EXIF_TIFIFD *Ifd ;
	MMPD_EXIF_TIFTAG *Tag ;
	MMP_USHORT usTotalTags = 0;

	*ulExifLen = 0;

	gExifData.ulTifHeaderOffset = 12;
	gExifData.ExifHeader = ExifHeaderEncode;
	gExifData.TifHeader = TifHeaderEncode;


	gExifDataPtr = gExifDataBuffer ;

	tif_set_short((MMP_UBYTE *)gExifDataPtr,ExifHeaderEncode.usSOI); 
	tif_set_short((MMP_UBYTE *)gExifDataPtr+2,ExifHeaderEncode.usApp1Marker); 
	tif_set_short((MMP_UBYTE *)gExifDataPtr+4,ExifHeaderEncode.usApp1Len); 
	tif_set_long ((MMP_UBYTE *)gExifDataPtr+6,ExifHeaderEncode.ulExifID); 
	tif_set_short((MMP_UBYTE *)gExifDataPtr+10,ExifHeaderEncode.usPad); 
	gExifDataPtr += 12;

	tif_set_short((MMP_UBYTE *)gExifDataPtr  ,TifHeaderEncode.usByteOrder); 
	tif_set_short((MMP_UBYTE *)gExifDataPtr+2,TifHeaderEncode.usFixed002a); 
	tif_set_long ((MMP_UBYTE *)gExifDataPtr+4,TifHeaderEncode.ulIfd0Offset); 
	gExifDataPtr += 8;

	// 12 : sizeof(tif_tag_t);
	ulSizeOver4Offset  = gExifData.ulTifHeaderOffset  + 8; //sizeof(tif_header_t)

	for(i=0;i<gExifData.lIfdNum;i++) {
		Ifd = gExifData.Ifd[i];
		if(i==2) {
			MMP_ULONG offset = gExifDataPtr - gExifDataBuffer;
			tif_set_long((MMP_UBYTE *)ubExifDataPtr8769, offset-gExifData.ulTifHeaderOffset);	
		}
		
		usTotalTags = Ifd->usFields;
		ulSizeOver4Offset += 12 * usTotalTags + sizeof(Ifd->ulNextIfd); 
		ulSizeOver4Offset += 2;
		ubSizeOver4Ptr = gExifDataBuffer + ulSizeOver4Offset;

        tif_set_short((MMP_UBYTE *)gExifDataPtr, usTotalTags);
		gExifDataPtr += 2;

        for(j = 0;j < gExifData.Ifd[i]->usFields ;j++){
            Tag = Ifd->Tag[j];
            tif_set_short((MMP_UBYTE *)gExifDataPtr, 	Tag->usTag);
            tif_set_short((MMP_UBYTE *)gExifDataPtr + 2,Tag->usType);
            tif_set_long ((MMP_UBYTE *)gExifDataPtr + 4, Tag->ulCount);

			gExifDataPtr+=8;
			ulSize = Tag->ulCount * MMPD_EXIF_GetTifUnit(Tag->usType);
			if(ulSize > 4) {
				// val_offset is address offset
				memcpy(ubSizeOver4Ptr,(char *)Tag->ulValOffset, ulSize);
				if( (Tag->usType==TYPE_RATIONAL)||(Tag->usType==TYPE_SRATIONAL)) {
					tif_set_long((MMP_UBYTE *)ubSizeOver4Ptr+0,*(ubSizeOver4Ptr+0) );
					tif_set_long((MMP_UBYTE *)ubSizeOver4Ptr+4,*(ubSizeOver4Ptr+4) );
				}
				tif_set_long((MMP_UBYTE *)gExifDataPtr,ulSizeOver4Offset-gExifData.ulTifHeaderOffset);
				ubSizeOver4Ptr += ulSize ;
				ulSizeOver4Offset += ulSize ;
            }
            else{
				// val_offset is value
				if(Tag->usTag==0x8769) {
					ubExifDataPtr8769 = gExifDataPtr ;
					tif_set_long((MMP_UBYTE *)gExifDataPtr, ulNotFilled);
                }
                else if((i == 1) && (Tag->usTag == 0x0201)){
                    ubExifDataPtr0201 = gExifDataPtr ;
                    tif_set_long((MMP_UBYTE *) gExifDataPtr, ulNotFilled);
                }
				else if(Tag->usType == TYPE_ASCII) {
                    memcpy(( char *) gExifDataPtr,(char *)&Tag->ulValOffset,4);
                }
                else{
                    tif_set_long((MMP_UBYTE *) gExifDataPtr, *(MMP_ULONG *) Tag->ulValOffset);
				}
			}
			gExifDataPtr+=4 ;
		}
		// 8 : sizeof(tif_header_t)
		if(i==0) {
            tif_set_long((MMP_UBYTE *) gExifDataPtr, ulSizeOver4Offset - sizeof(Ifd->ulNextIfd) - 8);
        }
        else{
            tif_set_long((MMP_UBYTE *) gExifDataPtr, ulEndIfd);
        }
        gExifDataPtr = gExifDataBuffer + ulSizeOver4Offset ;
    }
    *ulExifLen = (gExifDataPtr - gExifDataBuffer + 3) & (-4); 
    if(ubExifDataPtr0201){
        tif_set_long((MMP_UBYTE *) ubExifDataPtr0201,
                     *ulExifLen - gExifData.ulTifHeaderOffset);
    }

    tif_set_short((MMP_UBYTE *) gExifDataBuffer + 4,
                  (MMP_USHORT) * ulExifLen - 4 + gExifCtx->ulThumbnailSize + 2); // +2 means skip 0xFFD8
    return MMP_ERR_NONE;
}

MMP_ERR MMPD_EXIF_Encode(MMPD_EXIF_CONTEXT *ExifCtx)
{
	MMP_ULONG ulExifLen;
	MMP_USHORT usTifByteOrder;

	MMPD_EXIF_Init();
	
    gExifCtx = ExifCtx ;
    gExifDataBuffer = (MMP_UBYTE *)(ExifCtx->ubBuf + gExifCtx->ulExifHeaderOffset);
    gExifDataPtr = gExifDataBuffer;

    //PRINTF("gExifCtx.buf = 0x%08x\n",           gExifCtx->ubBuf);
    PRINTF("gExifCtx.exif_header_size = %d\n",  gExifCtx->ulExifHeaderSize);
    PRINTF("gExifCtx.exif_header_offset = %d\n",gExifCtx->ulExifHeaderOffset);
    PRINTF("gExifCtx.primary_size  = %d\n",     gExifCtx->ulPrimarySize);
    PRINTF("gExifCtx.primary_offset  = %d\n",   gExifCtx->ulPrimaryOffset);
    PRINTF("gExifCtx.thumbnail_size  = %d\n",   gExifCtx->ulThumbnailSize);
    PRINTF("gExifCtx.thumbnail_offset  = %d\n", gExifCtx->ulThumbnailOffset);
    PRINTF("gExifCtx.valid_size  = %d\n",       gExifCtx->ulValidSize);
    PRINTF("gExifCtx.rgb16_size  = %d\n",       gExifCtx->ulRgb16Size);
    PRINTF("gExifCtx.rgb16_offset  = %d\n",     gExifCtx->ulRgb16Offset);
    PRINTF("gExifCtx.primary_w  = %d\n",        gExifCtx->usPrimaryW);
    PRINTF("gExifCtx.primary_h  = %d\n",        gExifCtx->usPrimaryH);
    PRINTF("gExifCtx.thumbnail_w  = %d\n",      gExifCtx->usThumbnailW);
    PRINTF("gExifCtx.thumbnail_h  = %d\n",      gExifCtx->usThumbnailH);
	PRINTF("gExifCtx.orientation  = %d\n",      gExifCtx->usOrientation);

	// set exif header
	gExifData.ExifHeader = ExifHeaderEncode;
	// set tif header
	gExifData.TifHeader  = TifHeaderEncode;

	usTifByteOrder = gExifData.TifHeader.usByteOrder;

	if(usTifByteOrder == TIF_II_FORMAT) {
		tif_get_long  = tif_get_long_le ;
		tif_get_short = tif_get_short_le;
		tif_set_long  = tif_set_long_le ;
		tif_set_short = tif_set_short_le;
    }
	else if(usTifByteOrder == TIF_MM_FORMAT) {
		tif_get_long  = tif_get_long_be ;
		tif_get_short = tif_get_short_be;
		tif_set_long  = tif_set_long_be ;
		tif_set_short = tif_set_short_be;
	}
	else {
		PRINTF("unknow tif format be/le...\n");
		return 0;
	}
#ifdef USING_AIT_FS
    fs_GetTime(&gExifCtx->CreateTime);
#endif

	// set ifd....
	MMPD_EXIF_InsertIfd( &IfdImage[0]);
	MMPD_EXIF_InsertIfd( &IfdImage[1]);
	MMPD_EXIF_InsertIfd( &IfdImage[2]);
	MMPD_EXIF_ConfigImage();
	MMPD_EXIF_ConfigThumbnail();
	MMPD_EXIF_ConfigCamera();
	MMPD_EXIF_Build(&ulExifLen);
	gExifCtx->ulExifHeaderSize = ulExifLen;
	gExifCtx->ulValidSize += ulExifLen;
	
	return MMP_ERR_NONE;
}

////////////////////////////////////////////////
// for EXIF Decoder
#if (!EXIF_DEC_SIMPLIFY)
static void MMPD_EXIF_DumpTifIfd(MMPD_EXIF_TIFIFD *Ifd)
{
	MMPD_EXIF_TIFTAG *Tag ;
	MMP_USHORT i;
	
	if(!Ifd) {
		return ;
	}
	PRINTF("tag (id  ,type,count   ,value   )-(type str)\n");
	for(i=0; i<Ifd->usFields; i++) {
		Tag = Ifd->Tag[i];
		if(Tag->usType == TYPE_ASCII) {
			MMP_BYTE *str = (char *)Tag->ulValOffset;
			if(Tag->ulCount <= 4) {
				str = (char *)(&Tag->ulValOffset);
			}
			PRINTF("     %04x %04x %08x %08x-(%s)-(%s)\n",
				Tag->usTag,
				Tag->usType,
				Tag->ulCount,
				Tag->ulValOffset,
				TifTagTypeStr[Tag->usType], str);
		} else if( (Tag->usType==TYPE_RATIONAL)||(Tag->usType==TYPE_SRATIONAL)) {
			MMP_ULONG a, b;
			a = tif_get_long((MMP_UBYTE *)Tag->ulValOffset);
			b = tif_get_long((MMP_UBYTE *)Tag->ulValOffset + 4);
			PRINTF("     %04x %04x %08x %08x-(%s)-(%d.%d)\n",
				Tag->usTag,
				Tag->usType,
				Tag->ulCount,
				Tag->ulValOffset,
				TifTagTypeStr[Tag->usType], a, b);

		}
		else if((Tag->usType == TYPE_SHORT) || (Tag->usType == TYPE_SSHORT)) {
			PRINTF("     %04x %04x %08x %08x-(%s)-(%d)\n",
				Tag->usTag,
				Tag->usType,
				Tag->ulCount,
				Tag->ulValOffset,
				TifTagTypeStr[Tag->usType],Tag->ulValOffset /*>> 16*/);

			
		}
		else if((Tag->usType == TYPE_LONG) || (Tag->usType == TYPE_SLONG)) {
			PRINTF("     %04x %04x %08x %08x-(%s)-(%d)\n",
				Tag->usTag,
				Tag->usType,
				Tag->ulCount,
				Tag->ulValOffset,
				TifTagTypeStr[Tag->usType], Tag->ulValOffset);

			
		}
		else {
			PRINTF("     %04x %04x %08x %08x-(%s)\n",
				Tag->usTag,
				Tag->usType,
				Tag->ulCount,
				Tag->ulValOffset,
				TifTagTypeStr[Tag->usType]);
		}
	}
}

static void *MMPD_EXIF_TifMalloc(MMP_LONG ulSize)
{
	MMP_LONG q_size = (ulSize + 127) & (-128) ;

	PRINTF("MMP_LONG q_size = 0x%X\r\n", q_size);
	
    gExifDataPtr = gExifDataEnd;
	gExifDataEnd = gExifDataPtr + q_size;
	
	if(gExifDataEnd <= gExifDataBuffer4Dec + EXIF_DATA_BUF_SIZE) {
		return (void *)gExifDataPtr;
	}	
	else {
		PRINTF("malloc TIF TAG structure failed\n");
		return (void *)NULL ;
	}	
}

static void MMPD_EXIF_GetUsefulInfor(MMPD_EXIF_CONTEXT *ExifCtx)
{
    MMP_LONG i;
	MMPD_EXIF_TIFIFD *Ifd;
	MMPD_EXIF_TIFTAG *Tag;
	
    Ifd = gExifData.Ifd[0];
    if(Ifd) {
        for(i=0; i<Ifd->usFields; i++) {
            Tag = Ifd->Tag[i] ;
            if(Tag->usTag == 0x0112) {
                ExifCtx->usOrientation = Tag->ulValOffset; 
                if(ExifCtx->usOrientation == 1) {
                // lanscape
                    ExifCtx->usOrientation = 1;
                }
                else{
                // portrait
                    ExifCtx->usOrientation = 0;
                }
            }
        }    
    }
	
    Ifd = gExifData.Ifd[1] ;
    if(Ifd) {
        for(i=0; i<Ifd->usFields; i++) {
            Tag = Ifd->Tag[i];
            if(Tag->usTag == 0x0100) {
                ExifCtx->usThumbnailW = Tag->ulValOffset;
            }
            else if(Tag->usTag == 0x0101){
                ExifCtx->usThumbnailH = Tag->ulValOffset;
            }
            else if(Tag->usTag == 0x0103){
                ExifCtx->usPad = Tag->ulValOffset; 
            }
            else if(Tag->usTag == 0x0201){
                ExifCtx->ulThumbnailOffset = Tag->ulValOffset + 12; // add tif header offset
            }
            else if(Tag->usTag == 0x0202){
                ExifCtx->ulThumbnailSize = Tag->ulValOffset;
            }
        }
    }
	
    Ifd = gExifData.Ifd[2] ;
    if(Ifd){
        for(i = 0; i < Ifd->usFields; i++){
            Tag = Ifd->Tag[i];
            if(Tag->usTag == 0xA002){
                ExifCtx->usPrimaryW = Tag->ulValOffset;
            }
            else if(Tag->usTag == 0xA003){
                ExifCtx->usPrimaryH = Tag->ulValOffset;
            }
        }
    }
}

static MMP_UBYTE *MMPD_EXIF_IfdDecode(MMPD_EXIF_TIFIFD *Ifd, MMP_UBYTE *ptr)
{
	MMP_USHORT i;
    MMP_ULONG  ulValSize;
    MMP_UBYTE *ptrVal;
	MMPD_EXIF_TIFTAG *Tag;
	
	Ifd->usFields = tif_get_short(ptr); ptr+=2 ;

	for(i=0; i<Ifd->usFields; i++) {

		Tag = (MMPD_EXIF_TIFTAG *)MMPD_EXIF_TifMalloc(sizeof(MMPD_EXIF_TIFTAG));
		if( !Tag ) {
			PRINTF("malloc TIF TAG structure failed\n");
			return (MMP_UBYTE *)0;
        }
        else{
			Tag->usTag     = tif_get_short(ptr); ptr+=2;
			Tag->usType    = tif_get_short(ptr); ptr+=2;
			Tag->ulCount   = tif_get_long(ptr);  ptr+=4;
			Tag->ulValOffset= tif_get_long(ptr);  ptr+=4;
			ulValSize = Tag->ulCount * MMPD_EXIF_GetTifUnit(Tag->usType);
			if(ulValSize > 4) {
				ptrVal = gExifData.ubData + gExifData.ulTifHeaderOffset + Tag->ulValOffset;
                Tag->ulValOffset = (MMP_ULONG) ptrVal;              
            }
            else{
                if((Tag->usType == TYPE_SHORT) || (Tag->usType == TYPE_SSHORT)){
                    if(gExifData.TifHeader.usByteOrder == TIF_II_FORMAT){
                        Tag->ulValOffset = Tag->ulValOffset & 0xffff;
                    }
                    else{
                        Tag->ulValOffset = (Tag->ulValOffset >> 16) & 0xffff ;
                    }
                }
			}
			Ifd->Tag[i] = Tag;
			if(Tag->usTag == 0x8769) {
				gExifData.lExifIfdFound = 1 ;
				gExifData.lExifIfdOffset = Tag->ulValOffset ;
				PRINTF("Found exif ifd,offset: %08x\n",gExifData.lExifIfdOffset);
			}
			if(gExifData.lIfdNum == 1) {
			// thumbnail IFD-1
				if(Tag->usTag == 0x0201) {
				// jpeg data offset
					gExifData.ubThumbnailData = gExifData.ubData + gExifData.ulTifHeaderOffset + Tag->ulValOffset ;
				}
				if(Tag->usTag == 0x0202) {
					gExifData.ulThumbnailLen = Tag->ulValOffset;
				}
			}
		}
	}
	Ifd->ulNextIfd = tif_get_long(ptr); ptr+=4;
	return ptr;
}

MMP_ERR  MMPD_EXIF_Decode(MMPD_EXIF_CONTEXT *ExifCtx, MMP_UBYTE *ubJpegBuf, MMP_ULONG ulJpegSize)
{
    MMPD_EXIF_TIFIFD *Ifd;
    MMP_UBYTE * ptr = ubJpegBuf, * old_ptr;
    MMP_USHORT usSoi, usTifByteOrder;
    MMP_USHORT usApp1Marker, usApp1Len = 0, usApp0Len = 0;
    MMP_ULONG  ulExifId;
    MMP_ULONG ulCurLen = ulJpegSize;

	MMPD_EXIF_Init();
	
	ExifCtx->ubBuf = ubJpegBuf ;
    ExifCtx->ulExifHeaderSize = 0 ;
    ExifCtx->ulExifHeaderOffset = 0 ;
    ExifCtx->ulPrimarySize = 0 ;
    ExifCtx->ulPrimaryOffset = 0;
    ExifCtx->ulThumbnailSize = 0 ;
    ExifCtx->ulThumbnailOffset = 0;
	ExifCtx->ulValidSize = 0 ;
    ExifCtx->ulRgb16Size = 0 ;
    ExifCtx->ulRgb16Offset = 0;
    ExifCtx->usPrimaryW = 0 ;
    ExifCtx->usPrimaryH = 0 ;
    ExifCtx->usThumbnailW  = 0 ; // Camera is Landscape type
    ExifCtx->usThumbnailH  = 0 ;
    ExifCtx->usOrientation = 0;
    ExifCtx->usPad = 0;
    
	gExifData.ubData = ubJpegBuf;
	gExifData.lExifIfdFound  = 0;
	gExifData.lExifIfdOffset = 0;

	usSoi = get_double(ptr);
	if( usSoi != EXIF_SOI ) {
		PRINTF("no SOI\n");
	} else {
		ptr+=2 ;
	}
find_app1:

    usApp1Marker = get_double(ptr);
    if(usApp1Marker==EXIF_APP0_MARKER) {
        ptr+=2;
        usApp0Len = get_double(ptr); ptr += usApp0Len ;
        goto find_app1 ;
        
    } else if(usApp1Marker==EXIF_APP1_MARKER) {
        ptr += 2 ;
        usApp1Len = get_double(ptr); ptr += 2 ;
        PRINTF("get app1 len : 0x%04x(%d) bytes\n", usApp1Len, usApp1Len);
        
    } else {
        PRINTF("can't find app1 marker\n");
        return MMP_DSC_ERR_EXIF_DEC;        
    }
    ulExifId = get_quad(ptr);
	if(ulExifId!=EXIF_ID) {
		PRINTF("can't find exif id\n");
		return MMP_DSC_ERR_EXIF_DEC;
	}
	ptr+=4;
	// skip Null byte & Pading byte
	do {
		usTifByteOrder = get_double(ptr); ptr+=2;
    } while((usTifByteOrder != TIF_II_FORMAT) && (usTifByteOrder != TIF_MM_FORMAT));

	gExifData.ExifHeader.usSOI = usSoi;
	gExifData.ExifHeader.usApp1Marker = usApp1Marker;
	gExifData.ExifHeader.usApp1Len = usApp1Len;
	gExifData.ExifHeader.ulExifID = ulExifId;
	gExifData.ExifHeader.usPad = 0;

	gExifData.ulTifHeaderOffset = ptr - 2 - ubJpegBuf;

	gExifData.TifHeader.usByteOrder = usTifByteOrder;
	if(usTifByteOrder==TIF_II_FORMAT) {
		tif_get_long  = tif_get_long_le ;
		tif_get_short = tif_get_short_le;
		tif_set_long  = tif_set_long_le ;
		tif_set_short = tif_set_short_le;
	} 
	else if(usTifByteOrder==TIF_MM_FORMAT) {
		tif_get_long  = tif_get_long_be ;
		tif_get_short = tif_get_short_be;
		tif_set_long  = tif_set_long_be ;
		tif_set_short = tif_set_short_be;
	}
	else {
		PRINTF("unknow tif format be/le...\n");
		return MMP_DSC_ERR_EXIF_DEC;
	}

	gExifData.TifHeader.usFixed002a = tif_get_short(ptr); ptr+=2;
	gExifData.TifHeader.ulIfd0Offset = tif_get_long(ptr); ptr+=4;
	if(gExifData.TifHeader.usFixed002a != 0x002a) {
		PRINTF("bad item 0x002a in tif header\n");
	}
	PRINTF("tif_header_offset : %d\n",gExifData.ulTifHeaderOffset);
	PRINTF("ifd0_offset       : %d\n",gExifData.TifHeader.ulIfd0Offset);

	// seek to IFD 0
	ptr = ubJpegBuf + gExifData.ulTifHeaderOffset + gExifData.TifHeader.ulIfd0Offset;
	ulCurLen -= (ptr - ubJpegBuf) ;
	//PRINTF("IFD 0 offset : 0x%08x,left len : %d\n", ptr, ulCurLen);

    //Init gExifDataBuffer4Dec[]
	gExifDataEnd = gExifDataBuffer4Dec;

    while(ulCurLen){
		Ifd = (MMPD_EXIF_TIFIFD *)MMPD_EXIF_TifMalloc(sizeof(MMPD_EXIF_TIFIFD));
		if(!Ifd) {
			PRINTF("malloc IFD structure failed\n");
			return MMP_DSC_ERR_EXIF_DEC;
		}
		old_ptr = ptr;
		ptr = MMPD_EXIF_IfdDecode(Ifd, old_ptr);

		if(!ptr) {
			//tif_free(ifd);
			return MMP_DSC_ERR_EXIF_DEC;
		}
#if DUMP_EXIF_EN		
		PRINTF("--------Dump IFD %d(%s)-----------\n",gExifData.lIfdNum, TifTagTypeStr[gExifData.lIfdNum]);
		MMPD_EXIF_DumpTifIfd(Ifd);
#endif
		
		gExifData.Ifd[gExifData.lIfdNum] = Ifd;
		gExifData.lIfdNum++;

		ulCurLen -= (ptr - old_ptr);
		if(!Ifd->ulNextIfd) {
			//break ;
			if(gExifData.lExifIfdFound == 1) {
				ptr = ubJpegBuf + gExifData.ulTifHeaderOffset + gExifData.lExifIfdOffset;
				gExifData.lExifIfdFound = 2;
				continue;
			} else {
				break;
			}
		} else {
		}
		ptr = ubJpegBuf + gExifData.ulTifHeaderOffset + Ifd->ulNextIfd;
	}
#if 1 //for log primary jpeg size & offset
    if(gExifData.Ifd[1]) {
        if(gExifData.ulThumbnailLen) {
            ExifCtx->ulPrimaryOffset = gExifData.ExifHeader.usApp1Len + 4;
            ExifCtx->ulPrimarySize = ulJpegSize - ExifCtx->ulPrimaryOffset;

        } else {
            ExifCtx->ulPrimarySize = ulJpegSize;
            ExifCtx->ulPrimaryOffset = 0;
        }
    }
    else{
            ExifCtx->ulPrimarySize = ulJpegSize;
            ExifCtx->ulPrimaryOffset = 0;
    }
#endif

	MMPD_EXIF_GetUsefulInfor(ExifCtx);

	return MMP_ERR;
}
#endif

#if (EXIF_DEC_SIMPLIFY)
static void MMPD_EXIF_GetUsefulInfor(MMPD_EXIF_CONTEXT *ExifCtx, MMPD_EXIF_TIFTAG *Tag)
{
    if(gExifData.lIfdNum == 0) {     
        if(Tag->usTag == 0x0112) {
            ExifCtx->usOrientation = Tag->ulValOffset; 
            if(ExifCtx->usOrientation == 1) {
            // lanscape
                ExifCtx->usOrientation = 1;
            }
            else{
            // portrait
                ExifCtx->usOrientation = 0;
            }
        }            
    }
	
    if(gExifData.lIfdNum == 1) {   
        if(Tag->usTag == 0x0100) {
            ExifCtx->usThumbnailW = Tag->ulValOffset;
        }
        else if(Tag->usTag == 0x0101){
            ExifCtx->usThumbnailH = Tag->ulValOffset;
        }
        else if(Tag->usTag == 0x0103){
            ExifCtx->usPad = Tag->ulValOffset; 
        }
        else if(Tag->usTag == 0x0201){
            ExifCtx->ulThumbnailOffset = Tag->ulValOffset + 12; // add tif header offset
        }
        else if(Tag->usTag == 0x0202){
            ExifCtx->ulThumbnailSize = Tag->ulValOffset;
        }      
    }
	
    if(gExifData.lIfdNum == 2) {   
        if(Tag->usTag == 0xA002){
            ExifCtx->usPrimaryW = Tag->ulValOffset;
        }
        else if(Tag->usTag == 0xA003){
            ExifCtx->usPrimaryH = Tag->ulValOffset;
        }      
    }
}

static MMP_UBYTE *MMPD_EXIF_IfdDecode(MMPD_EXIF_CONTEXT *ExifCtx, MMPD_EXIF_TIFIFD *Ifd, MMP_UBYTE *ptr)
{
	MMP_USHORT i;
    MMP_ULONG  ulValSize;
    MMP_UBYTE *ptrVal;
	MMPD_EXIF_TIFTAG Tag;
	
	Ifd->usFields = tif_get_short(ptr); ptr+=2;

	for(i=0; i<Ifd->usFields; i++) {
		Tag.usTag     = tif_get_short(ptr); ptr+=2;
		Tag.usType    = tif_get_short(ptr); ptr+=2;
		Tag.ulCount   = tif_get_long(ptr);  ptr+=4;
		Tag.ulValOffset= tif_get_long(ptr);  ptr+=4;
		ulValSize = Tag.ulCount * MMPD_EXIF_GetTifUnit(Tag.usType);
		if(ulValSize > 4) {
			ptrVal = gExifData.ubData + gExifData.ulTifHeaderOffset + Tag.ulValOffset;
            Tag.ulValOffset = (MMP_ULONG) ptrVal;              
        }
        else{
            if((Tag.usType == TYPE_SHORT) || (Tag.usType == TYPE_SSHORT)){
                if(gExifData.TifHeader.usByteOrder == TIF_II_FORMAT){
                    Tag.ulValOffset = Tag.ulValOffset & 0xffff;
                }
                else{
                    Tag.ulValOffset = (Tag.ulValOffset >> 16) & 0xffff ;
                }
            }
		}
		
		if(Tag.usTag == 0x8769) {
			gExifData.lExifIfdFound = 1 ;
			gExifData.lExifIfdOffset = Tag.ulValOffset ;
			PRINTF("Found exif ifd,offset: %08x\n",gExifData.lExifIfdOffset);
		}
		if(gExifData.lIfdNum == 1) {
		// thumbnail IFD-1
			if(Tag.usTag == 0x0201) {
			// jpeg data offset
				gExifData.ubThumbnailData = gExifData.ubData + gExifData.ulTifHeaderOffset + Tag.ulValOffset ;
			}
			if(Tag.usTag == 0x0202) {
				gExifData.ulThumbnailLen = Tag.ulValOffset;
			}
		}

		MMPD_EXIF_GetUsefulInfor(ExifCtx, &Tag);
		
	}
	Ifd->ulNextIfd = tif_get_long(ptr); ptr+=4;
	return ptr;
}

MMP_ERR MMPD_EXIF_Decode(MMPD_EXIF_CONTEXT *ExifCtx, MMP_UBYTE *ubJpegBuf, MMP_ULONG ulJpegSize)
{
    MMPD_EXIF_TIFIFD Ifd;
    MMP_UBYTE * ptr = ubJpegBuf;
	MMP_UBYTE * old_ptr;
    MMP_USHORT usSoi, usTifByteOrder;
    MMP_USHORT usApp1Marker, usApp1Len = 0, usApp0Len = 0;
    MMP_ULONG  ulExifId;
    MMP_ULONG ulCurLen = ulJpegSize;

	MMPD_EXIF_Init();
	
	ExifCtx->ubBuf = ubJpegBuf ;
    ExifCtx->ulExifHeaderSize = 0 ;
    ExifCtx->ulExifHeaderOffset = 0 ;
    ExifCtx->ulPrimarySize = 0 ;
    ExifCtx->ulPrimaryOffset = 0;
    ExifCtx->ulThumbnailSize = 0 ;
    ExifCtx->ulThumbnailOffset = 0;
	ExifCtx->ulValidSize = 0 ;
    ExifCtx->ulRgb16Size = 0 ;
    ExifCtx->ulRgb16Offset = 0;
    ExifCtx->usPrimaryW = 0 ;
    ExifCtx->usPrimaryH = 0 ;
    ExifCtx->usThumbnailW  = 0 ; // Camera is Landscape type
    ExifCtx->usThumbnailH  = 0 ;
    ExifCtx->usOrientation = 0;
    ExifCtx->usPad = 0;
    
	gExifData.ubData = ubJpegBuf;
	gExifData.lExifIfdFound  = 0;
	gExifData.lExifIfdOffset = 0;

	usSoi = get_double(ptr);
	if( usSoi != EXIF_SOI ) {
		PRINTF("no SOI\n");
	} else {
		ptr+=2 ;
	}
find_app1:

    usApp1Marker = get_double(ptr);
    if(usApp1Marker==EXIF_APP0_MARKER) {
        ptr+=2;
        usApp0Len = get_double(ptr); ptr += usApp0Len ;
        goto find_app1 ;
        
    } else if(usApp1Marker==EXIF_APP1_MARKER) {
        ptr += 2 ;
        usApp1Len = get_double(ptr); ptr += 2 ;
        PRINTF("get app1 len : 0x%04x(%d) bytes\n", usApp1Len, usApp1Len);
        
    } else {
        PRINTF("can't find app1 marker\n");
        return MMP_DSC_ERR_EXIF_DEC;        
    }
    ulExifId = get_quad(ptr);
	if(ulExifId!=EXIF_ID) {
		PRINTF("can't find exif id\n");
		return MMP_DSC_ERR_EXIF_DEC;
	}
	ptr+=4;
	// skip Null byte & Pading byte
	do {
		usTifByteOrder = get_double(ptr); ptr+=2;
    } while((usTifByteOrder != TIF_II_FORMAT) && (usTifByteOrder != TIF_MM_FORMAT));

	gExifData.ExifHeader.usSOI = usSoi;
	gExifData.ExifHeader.usApp1Marker = usApp1Marker;
	gExifData.ExifHeader.usApp1Len = usApp1Len;
	gExifData.ExifHeader.ulExifID = ulExifId;
	gExifData.ExifHeader.usPad = 0;

	gExifData.ulTifHeaderOffset = ptr - 2 - ubJpegBuf;

	gExifData.TifHeader.usByteOrder = usTifByteOrder;
	if(usTifByteOrder==TIF_II_FORMAT) {
		tif_get_long  = tif_get_long_le ;
		tif_get_short = tif_get_short_le;
		tif_set_long  = tif_set_long_le ;
		tif_set_short = tif_set_short_le;
	} 
	else if(usTifByteOrder==TIF_MM_FORMAT) {
		tif_get_long  = tif_get_long_be ;
		tif_get_short = tif_get_short_be;
		tif_set_long  = tif_set_long_be ;
		tif_set_short = tif_set_short_be;
	}
	else {
		PRINTF("unknow tif format be/le...\n");
		return MMP_DSC_ERR_EXIF_DEC;
	}

	gExifData.TifHeader.usFixed002a = tif_get_short(ptr); ptr+=2;
	gExifData.TifHeader.ulIfd0Offset = tif_get_long(ptr); ptr+=4;
	if(gExifData.TifHeader.usFixed002a != 0x002a) {
		PRINTF("bad item 0x002a in tif header\n");
	}
	PRINTF("tif_header_offset : %d\n",gExifData.ulTifHeaderOffset);
	PRINTF("ifd0_offset       : %d\n",gExifData.TifHeader.ulIfd0Offset);

	// seek to IFD 0
	ptr = ubJpegBuf + gExifData.ulTifHeaderOffset + gExifData.TifHeader.ulIfd0Offset;
	ulCurLen -= (ptr - ubJpegBuf) ;
	//PRINTF("IFD 0 offset : 0x%08x,left len : %d\n", ptr, ulCurLen);

    while(ulCurLen){
		old_ptr = ptr;
		ptr = MMPD_EXIF_IfdDecode(ExifCtx, &Ifd, old_ptr);

		if(!ptr) {
			//tif_free(ifd);
			return MMP_DSC_ERR_EXIF_DEC;
		}
		
		gExifData.lIfdNum++;

		ulCurLen -= (ptr - old_ptr);
		if(!Ifd.ulNextIfd) {
			//break ;
			if(gExifData.lExifIfdFound == 1) {
				ptr = ubJpegBuf + gExifData.ulTifHeaderOffset + gExifData.lExifIfdOffset;
				gExifData.lExifIfdFound = 2;
				continue;
			} else {
				break;
			}
		} else {
		}
		ptr = ubJpegBuf + gExifData.ulTifHeaderOffset + Ifd.ulNextIfd;
	}

#if 1 //for log primary jpeg size & offset
    if(ExifCtx->ulThumbnailSize) {
        ExifCtx->ulPrimaryOffset = gExifData.ExifHeader.usApp1Len + 4;
        ExifCtx->ulPrimarySize = ulJpegSize - ExifCtx->ulPrimaryOffset;

    } else {
        ExifCtx->ulPrimarySize = ulJpegSize;
        ExifCtx->ulPrimaryOffset = 0;
    }   
#endif

	return MMP_ERR_NONE;
}
#endif

//////////////////////////////////////////////////
#endif

#if 0		 
void _____NO_USING_END_____(){}
#endif

//////////////////////////////////////////////////
/// @}
/// @end_ait_only


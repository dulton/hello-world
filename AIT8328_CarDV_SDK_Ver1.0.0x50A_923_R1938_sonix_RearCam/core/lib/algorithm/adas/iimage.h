#ifndef IIMAGE_H
#define IIMAGE_H

#ifdef __cplusplus
extern "C" {
#endif
#if (defined(_RUN_ON_ECLIPSE_) | defined(ARM_SIMU_ONLY))
#define printc(...)   printf(__VA_ARGS__)
#endif
#define PI   3.1415926535897932384626433832795

typedef struct _MyPOINT
{
	int x;
	int y;
} MyPOINT;

static __inline MyPOINT point( int x, int y )
{
    MyPOINT p;

    p.x = x;
    p.y = y;

    return p;
}

typedef struct _IIMAGE
{
	int  depth;             /* Pixel depth in bytes: 1~4 byte                   */
    int  width;             /* Image width in pixels.                           */
    int  height;            /* Image height in pixels.                          */
    int  widthOrigin;       /* Image width in pixels.                           */
    int  heightOrigin;      /* Image height in pixels.                          */
    int  imageSize;         /* Image data size in bytes                         */
    int  widthStep;                  /* Size of aligned image row in bytes.     */
    unsigned char *imageData;        /* Pointer to aligned image data with ROI  */
    unsigned char *imageDataOrigin;  /* Pointer to origin image data without ROI*/
} IIMAGE;


extern IIMAGE *iCreateImage(int width, int height, int depth);
extern int iAllocateImage(IIMAGE* image, int width, int height, int depth);
extern void iReleaseImage(IIMAGE **image);
extern void iClearImage(IIMAGE *image);
extern void iSetROI(IIMAGE *image,int x,int y,int width,int height);
extern void iResetROI(IIMAGE *image);
extern unsigned int get_IImage_BufSize(int width, int height, int depth);

#ifdef __cplusplus
}
#endif

#endif

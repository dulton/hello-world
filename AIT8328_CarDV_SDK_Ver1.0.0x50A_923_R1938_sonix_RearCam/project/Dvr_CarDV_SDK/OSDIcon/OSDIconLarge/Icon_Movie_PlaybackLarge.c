/*********************************************************************
*                SEGGER MICROCONTROLLER SYSTEME GmbH                 *
*        Solutions for real time microcontroller applications        *
*                           www.segger.com                           *
**********************************************************************
*
* C-file generated by
*
*        �C/GUI-BitmapConvert V5.04.
*        Compiled Jan  8 2010, 16:46:31
*          (c) 2002-2005  Micrium, Inc.
  www.micrium.com

  (c) 1998-2005  Segger
  Microcontroller Systeme GmbH
  www.segger.com
*
**********************************************************************
*
* Source file: Icon_Movie_Playback
* Dimensions:  24 * 32
* NumColors:   16
*
**********************************************************************
*/

#include <stdlib.h>

#include "GUI.h"

#ifndef GUI_CONST_STORAGE
  #define GUI_CONST_STORAGE const
#endif

/*   Palette
The following are the entries of the palette table.
Every entry is a 32-bit value (of which 24 bits are actually used)
the lower   8 bits represent the Red component,
the middle  8 bits represent the Green component,
the highest 8 bits (of the 24 bits used) represent the Blue component
as follows:   0xBBGGRR
*/

static GUI_CONST_STORAGE GUI_COLOR ColorsIcon_Movie_Playback[] = {
     0xFF00FF,0x0B0B0B,0x151515,0x1C1C1C
    ,0x242424,0x2B2B2B,0x2F2F2F,0x343434
    ,0x393939,0x3E3E3E,0x424242,0x464646
    ,0x4A4A4A,0x4D4D4D,0x000000,0xFFFFFF
};

static GUI_CONST_STORAGE GUI_LOGPALETTE PalIcon_Movie_Playback = {
  16,	/* number of entries */
  1, 	/* Has transparency */
  &ColorsIcon_Movie_Playback[0]
};

static GUI_CONST_STORAGE unsigned char acIcon_Movie_Playback[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x0D, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xDD, 0xD0, 0x00, 0x00,
  0x0C, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xC0, 0x00, 0x00,
  0x0B, 0xFE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEF, 0xB0, 0x00, 0x00,
  0x0B, 0xFE, 0xEE, 0xEF, 0xEE, 0xEE, 0xEE, 0xEE, 0xEF, 0xB0, 0x00, 0x00,
  0x0A, 0xFE, 0xEE, 0xEF, 0xFE, 0xEE, 0xEE, 0xEE, 0xEF, 0xA0, 0xF0, 0xF0,
  0x09, 0xFE, 0xEE, 0xEF, 0xFF, 0xEE, 0xEE, 0xEE, 0xEF, 0x9F, 0xFF, 0xF0,
  0x08, 0xFE, 0xEE, 0xEF, 0xFF, 0xFE, 0xEE, 0xEE, 0xEF, 0x8F, 0xFF, 0xF0,
  0x07, 0xFE, 0xEE, 0xEF, 0xFF, 0xFF, 0xEE, 0xEE, 0xEF, 0x70, 0xF0, 0xF0,
  0x07, 0xFE, 0xEE, 0xEF, 0xFF, 0xFF, 0xFE, 0xEE, 0xEF, 0x70, 0xF0, 0xF0,
  0x06, 0xFE, 0xEE, 0xEF, 0xFF, 0xFF, 0xFF, 0xEE, 0xEF, 0x60, 0xFF, 0xF0,
  0x05, 0xFE, 0xEE, 0xEF, 0xFF, 0xFF, 0xFF, 0xEE, 0xEF, 0x50, 0xFF, 0xF0,
  0x04, 0xFE, 0xEE, 0xEF, 0xFF, 0xFF, 0xFE, 0xEE, 0xEF, 0x40, 0xF0, 0xF0,
  0x04, 0xFE, 0xEE, 0xEF, 0xFF, 0xFF, 0xEE, 0xEE, 0xEF, 0x40, 0xF0, 0xF0,
  0x04, 0xFE, 0xEE, 0xEF, 0xFF, 0xFE, 0xEE, 0xEE, 0xEF, 0x4F, 0xFF, 0xF0,
  0x03, 0xFE, 0xEE, 0xEF, 0xFF, 0xEE, 0xEE, 0xEE, 0xEF, 0x3F, 0xFF, 0xF0,
  0x02, 0xFE, 0xEE, 0xEF, 0xFE, 0xEE, 0xEE, 0xEE, 0xEF, 0x20, 0xF0, 0xF0,
  0x02, 0xFE, 0xEE, 0xEF, 0xEE, 0xEE, 0xEE, 0xEE, 0xEF, 0x20, 0xF0, 0xF0,
  0x02, 0xFE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEF, 0x20, 0xFF, 0xF0,
  0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x10, 0xFF, 0xF0,
  0x01, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x10, 0xF0, 0xF0,
  0x00, 0x00, 0x00, 0x00, 0xF0, 0xF0, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xF0,
  0x00, 0x00, 0x00, 0x00, 0xF0, 0xF0, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xF0,
  0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0,
  0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0,
  0x00, 0x00, 0x00, 0x00, 0xF0, 0xF0, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xF0,
  0x00, 0x00, 0x00, 0x00, 0xF0, 0xF0, 0x00, 0x00, 0x00, 0x00, 0xF0, 0xF0,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

GUI_CONST_STORAGE GUI_BITMAP bmIcon_Movie_Playback = {
  24, /* XSize */
  32, /* YSize */
  12, /* BytesPerLine */
  4, /* BitsPerPixel */
  acIcon_Movie_Playback,  /* Pointer to picture data (indices) */
  &PalIcon_Movie_Playback  /* Pointer to palette */
};

/* *** End of file *** */

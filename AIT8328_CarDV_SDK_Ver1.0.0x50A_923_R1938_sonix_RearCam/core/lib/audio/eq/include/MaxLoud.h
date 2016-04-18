/***********************************************************************************************/
/*  Description :   Function/Parameter definition of MaxLoud algorithm                         */
/*  Version     :   1.0                                                                        */
/*  Auther      :   Eric Huang                                                                 */
/***********************************************************************************************/
#ifndef     MAX_LOUD_H
#define     MAX_LOUD_H

typedef     short   int16;
typedef     int     int32;

#define FRAME_SIZE          1024
#define GAIN_STEP_SIZE      16

//Q.15 format
#define MAX_ENHANCE_15dB    23034
#define MAX_ENHANCE_12dB    16384
#define MAX_ENHANCE_9dB     11544
#define MAX_ENHANCE_6dB     8192
#define MAX_ENHANCE_3dB     6419

#define THRESHOLD1          20480
#define THRESHOLD2          16384
#define THRESHOLD3          12288
#define THRESHOLD4          8192
#define THRESHOLD5          4096

#define REF_GAIN1           0x0680
#define REF_GAIN2           0x0580
#define REF_GAIN3           0x0480
#define REF_GAIN4           0x0400
#define REF_GAIN5           0x0380
#define REF_GAIN6           0x0300

#define REF_POWER1          ((int32)REF_GAIN1 * (int32)REF_GAIN1) << 1
#define REF_POWER2          ((int32)REF_GAIN2 * (int32)REF_GAIN2) << 1
#define REF_POWER3          ((int32)REF_GAIN3 * (int32)REF_GAIN3) << 1
#define REF_POWER4          ((int32)REF_GAIN4 * (int32)REF_GAIN4) << 1
#define REF_POWER5          ((int32)REF_GAIN5 * (int32)REF_GAIN5) << 1
#define REF_POWER6          ((int32)REF_GAIN6 * (int32)REF_GAIN6) << 1

#define abs(a) ((a)<0 ? -(a) : (a))


void MaxLoudness_Init(int32 max_enhance);
void MaxLoudness(int16 *x_in, int16 *x_out, int32 frame_size, int32 channels, int32 volume);


#endif
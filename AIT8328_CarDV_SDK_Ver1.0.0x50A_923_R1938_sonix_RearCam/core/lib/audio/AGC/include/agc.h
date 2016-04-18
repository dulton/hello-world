#ifndef AGC_H
#define AGC_H
typedef struct  
{             
int max;  //analog gain maximum
int min;  //minimum
int init; //default gain (initial gain)

}AGC_dB;


#define P_TARGET_DB		     (-6)
#define NOISE_GATE_DB        (-35)
short GetValueFromdB(int dB); //-80~0 dB
void AGC_initiate (AGC_dB*,int DGAIN_EN);
int  Get_DGAIN(void);
int  AGC_Process(short* data,int channel);
void Set_Noise_Gate(int value);  // 0 < value< (1<<16)-1
unsigned int FuncAGC_GetLibVersion(unsigned short* ver_year, unsigned short* ver_date, unsigned short* ver_time);


#endif



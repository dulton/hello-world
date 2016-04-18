#ifndef AUTOGAINCTL_H
#define AUTOGAINCTL_H

typedef struct  
{             
int max;  //analog gain maximum
int min;  //minimum
int init; //default gain (initial gain)
}AGC_dB;


unsigned int FuncAGC_GetLibVersion(unsigned short* ver_year, unsigned short* ver_date, unsigned short* ver_time);
void AGC_initiate (AGC_dB*,int DGAIN_EN);
int  AGC_Process(short* data,int channel);
int  Get_DGAIN(void);

int  SetNoiseGate_dB (int dB); //-80~0 dB,the actual noise gate is returned.
void NoiseGate_enable(int);
int  SetTargetPower_dB (int value); //-80~0 dB,the actual Target power is returned.
void Set2TargetPower_dB (int valuehigh, int valuelow); //-80~0 dB,the actual Target power is returned.
void SetInputLen_ms(int ms);
void SetDrop_Gain_max(unsigned int value);
int  SetUpGainTime_x16(int ms);//input 1=> 16ms, 2 => 32ms...etc.
int  SetDownGainTime_x16(int ms);//input 1=> 16ms, 2 => 32ms...etc.
void SetVol(AGC_dB*);


#endif /* AUTOGAINCTL_H*/

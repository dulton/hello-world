#ifndef __NOISE_REDUCTION_H
#define __NOISE_REDUCTION_H

typedef struct
{
	//unsigned int noise_floor;
	unsigned int point_number; // can only be 512 or 128
	unsigned int channel;
	int * noise_estimate;
	int * specL;
	int * specR;
}
NR_Struct;

#define NRWorkingBufSize	16*1024 + sizeof(int)//(16*1024)

int NR_Init(char * workingBufferAddress, NR_Struct * nr_struct);
int NoiseReduct(short * input);


#endif
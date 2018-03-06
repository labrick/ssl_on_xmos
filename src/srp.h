
#ifndef SRP_H_
#define SRP_H_
#include "fft.h"
#include "common.h"


struct mic_Array{
    int mic_signal[FRAME_SIZE];
    int mic_real[FRAME_SIZE];
    int mic_Imag[FRAME_SIZE];
};

void showfft(float *real ,float *imag);
void caculate_gccphat(int32_t enframe_data[MIC][FRAME_SIZE] ,float result[MIC_PAIR][FRAME_SIZE]);
void srpphat(float R[MIC_PAIR][FRAME_SIZE] , int8_t TDOA_table[MIC_PAIR][SEARCH_POINT],float *result);
//void point_multi(complex* data1, complex* data2, complex* result, int FRAME_SIZE);
void process_datafft(complex data[FRAME_SIZE]);
//int  do_once_srp(struct mic_Array *mic , int TDOA_table[MIC_PAIR][SEARCH_POINT]);
int32_t find_source_location(int8_t TDOA_table[MIC_PAIR][SEARCH_POINT], float R[MIC_PAIR][FRAME_SIZE]);
#endif

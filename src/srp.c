#include <stdint.h>
#include <math.h>
#include "srp.h"
#include "fft.h"

//float srp_global[SEARCH_POINT];
//struct mic_Array ss[MIC_PAIR];

/*******************************
* ����غ����ļ������
* ���룺��Ҫ�����������������
* ���: ������
*******************************/
void point_multi(complex data1[], complex data2[], complex result[], int N)
{
    int i;
    for(i=0; i<N; i++){
        result[i].real =  data1[i].real * data2[i].real + data1[i].imag * data2[i].imag;
        result[i].imag = - data1[i].real * data2[i].imag + data1[i].imag * data2[i].real;
    }
}
/*******************************
* ��ӡ�õĵ��Գ�����ʾ��������Ϣ
* ���룺����
*******************************/
void showfft(float *real ,float *imag){
    int i;
    for (i=0;i<FRAME_SIZE;i++){
        printf("%f+%fj \n",real[i],imag[i]);
    }
}

/*******************************
* ��fft������й�һ����������
* X(m,:)= X(m,:)./abs(X(m,:));
* ���룺��Ҫ��һ�����鲿��ʵ������
* ���: ��һ�����
*******************************/
void process_datafft(complex data[FRAME_SIZE])
{
    int i;
    float phat;
    for (i=0; i<FRAME_SIZE; i++){
        phat = sqrt(data[i].real*data[i].real + data[i].imag*data[i].imag);
        data[i].real = data[i].real/phat;
        data[i].imag = data[i].imag/phat;
    }
}
        
/*******************************
* ����������srp-phat����
* ���룺����غ������
* ���:srp_global��񣬾���ÿ����
* �����srp-phatֵ
*******************************/
void srpphat(
    float R[MIC_PAIR][FRAME_SIZE],
    int8_t TDOA_table[MIC_PAIR][SEARCH_POINT],
    float result[SEARCH_POINT])
{
    int i,p;
    float srp_local=0;
    int  time_diff=0;
    int center = (FRAME_SIZE/2)-1;
    for(i=0; i<SEARCH_POINT; i++){
        srp_local = 0;
        for(p=0; p<MIC_PAIR; p++){
            time_diff = TDOA_table[p][i] + center;
            srp_local = srp_local + R[p][time_diff];
        }
        result[i] = srp_local;
    }
}


/*******************************
* ���㻥��غ��������
* ���룺mic��һ֡����
* ���������غ������
*
* result[][]�д�ŵ������յ�������
*******************************/
void caculate_gccphat(int32_t enframe_data[MIC][FRAME_SIZE] ,float result[MIC_PAIR][FRAME_SIZE])
{
//    float enframe_data_real[MIC][FRAME_SIZE];
//    float enframe_data_imag[MIC][FRAME_SIZE];
    complex enframe_fft_data[MIC][FRAME_SIZE];
    complex fft_result[MIC_PAIR][FRAME_SIZE];

    for(int8_t i=0; i<MIC; i++){
        for(int32_t j=0; j<FRAME_SIZE; j++){
            enframe_fft_data[i][j].real = enframe_data[i][j];
        }
    }
    printf("do fft ...\n");
    for(int8_t i=0; i<MIC; i++){
//        fftComputeOnce(myFFT, enframe_data[i], enframe_data_real[i], enframe_data_imag[i]);
        fft(FRAME_SIZE, enframe_fft_data[i]);
    }
    
    for(int8_t i=0; i<MIC; i++){
        process_datafft(enframe_fft_data[i]);
    }

   // Calculate the cross-power spectrum = fft(x1).*conj(fft(x2))
    int p = 0;
    for(int8_t i=0; i<MIC; i++){
        for(int8_t j=i+1; j<MIC; j++){
           point_multi(enframe_fft_data[i], enframe_fft_data[j], fft_result[p], FRAME_SIZE); //ss = MIC[first_data]*MIC[j];
           p++;
        }
    }
    for(int8_t p=0; p<MIC_PAIR; p++){
//        ifftComputeOnce(myFFT, result_real, result_imag ,result[p]);
        ifft(FRAME_SIZE, fft_result[p]);
        for(int32_t j=0; j<FRAME_SIZE; j++){
            result[p][j] = fft_result[p][j].real;
        }
    }
}

/**********************************
* srp-phat ������ڣ���ʱ���������
* ֮��Ӧ���������������
* ���룺mic��һ֡���ݺ� tdoa���
* �����srp ������ֵ�������
**********************************/
//int  do_once_srp(struct mic_Array *mic ,int TDOA_table[MIC_PAIR][SEARCH_POINT]){
////    int i;
//    int max_loc;
////    float max_srp;
//    printf("start srp-phat\n");
//    caculate_gccphat(mic,R);
//    srpphat(R, TDOA_table,srp_global);
//    //�ҵ�srp���ֵ
////    max_loc=0;
////    max_srp=srp_global[0];
////    for(i=0;i<SEARCH_POINT;i++){
////        if(srp_global[i] > max_srp){
////            max_srp = srp_global[i];
////            max_loc = i;
////        }
////    }
//    return max_loc;
//}

int32_t find_source_location(int8_t TDOA_table[MIC_PAIR][SEARCH_POINT], float R[MIC_PAIR][FRAME_SIZE])
{
    int max_loc;
    float max_srp;
    float srp_global[SEARCH_POINT];
    srpphat(R, TDOA_table, srp_global);
    for(int32_t i=0; i<SEARCH_POINT; i++){
        if(srp_global[i] > max_srp){
            max_srp = srp_global[i];
            max_loc = i;
        }
    }
    return max_loc;
}

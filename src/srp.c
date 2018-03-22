#include <stdint.h>
#include <math.h>
#include "srp.h"
#include "fft.h"

//float srp_global[SEARCH_POINT];
//struct mic_Array ss[MIC_PAIR];

/*******************************
* 求互相关函数的计算程序
* 输入：需要做点积的两个虚数组
* 输出: 点积结果
*******************************/
void point_multi(complex data1[], complex data2[], complex result[], int N)
{
    int i;
    for(i=0; i<N; i++){
        // data1 * conj(data2)
        result[i].real =  data1[i].real * data2[i].real + data1[i].imag * data2[i].imag;
        result[i].imag = - data1[i].real * data2[i].imag + data1[i].imag * data2[i].real;
    }
}
/*******************************
* 打印用的调试程序，显示出数组信息
* 输入：数组
*******************************/
void showfft(float *real ,float *imag){
    int i;
    for (i=0;i<FRAME_SIZE;i++){
        printf("%f+%fj \n",real[i],imag[i]);
    }
}

/*******************************
* 对fft结果进行归一化处理，就是
* X(m,:)= X(m,:)./abs(X(m,:));
* 输入：需要归一化的虚部和实部数组
* 输出: 归一化结果
*******************************/
void process_datafft(int N, complex data[FRAME_SIZE])
{
    int i;
    float phat;
    for (i=0; i<N; i++){
        phat = sqrt(data[i].real*data[i].real + data[i].imag*data[i].imag);
        data[i].real = data[i].real/phat;
        data[i].imag = data[i].imag/phat;
    }
}

/*******************************
* 计算互相关函数表格函数
* 输入：mic的一帧数据
* 输出：互相关函数表格
*
* result[][]中存放的是最终的输出结果
*******************************/
void caculate_gccphat(complex enframe_data[MIC][FRAME_SIZE], complex result[MIC_PAIR][FRAME_SIZE]) // ,float result[MIC_PAIR][FRAME_SIZE])
{
//    float x = 0;
//    for(int32_t i=0; i<1024; i++, x=x+0.1){
//        enframe_data[0][i].real = sin(x);
//        printf("%f, %f\n", enframe_data[0][i].real, enframe_data[0][i].imag);
//    }
//    printf("do fft once ...\n");
//    fft(1024, enframe_data[0]);
//    process_datafft(1024, enframe_data[0]);
//    printf("do once over\n");
//    for(int32_t i=0; i<1024; i++){
//        printf("%f, %f\n", enframe_data[0][i].real, enframe_data[0][i].imag);
//    }

    printf("do fft ...\n");
    for(int8_t i=0; i<MIC; i++){
//        fftComputeOnce(myFFT, enframe_data[i], enframe_data_real[i], enframe_data_imag[i]);
        fft(FRAME_SIZE, enframe_data[i]);
    }
    for(int8_t i=0; i<MIC; i++){
        process_datafft(FRAME_SIZE, enframe_data[i]);
    }

   // Calculate the cross-power spectrum = fft(x1).*conj(fft(x2))
    int p = 0;
    for(int8_t i=0; i<MIC; i++){
        for(int8_t j=i+1; j<MIC; j++){
           point_multi(enframe_data[i], enframe_data[j], result[p], FRAME_SIZE); //ss = MIC[first_data]*MIC[j];
           p++;
        }
    }
//    for(int32_t i=0; i<FRAME_SIZE; i++){
//        printf("%f, %f\n", result[0][i].real, result[0][i].imag);
//    }
//    printf("ifft below ...\n");
    for(int8_t p=0; p<MIC_PAIR; p++){
        ifft(FRAME_SIZE, result[p]);
        fftshift(result[p], FRAME_SIZE);
    }
//    for(int32_t i=0; i<FRAME_SIZE; i++){
//        printf("%f, %f\n", result[0][i].real, result[0][i].imag);
//    }
//    printf("fftshift below ...\n");
//    for(int8_t p=0; p<MIC_PAIR; p++){
//        fftshift(result[p], FRAME_SIZE);
//    }
//    for(int32_t i=0; i<FRAME_SIZE; i++){
//        printf("%f, %f\n", result[0][i].real, result[0][i].imag);
//    }
}

/**********************************
* srp-phat 函数入口，暂时无输入输出
* 之后应该有输入输出参数
* 输入：mic的一帧数据和 tdoa表格
* 输出：srp 表格最大值的坐标点
**********************************/
//int  do_once_srp(struct mic_Array *mic ,int TDOA_table[MIC_PAIR][SEARCH_POINT]){
////    int i;
//    int max_loc;
////    float max_srp;
//    printf("start srp-phat\n");
//    caculate_gccphat(mic,R);
//    srpphat(R, TDOA_table,srp_global);
//    //找到srp最大值
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

/*******************************
* 遍历搜索的srp-phat程序
* 输入：互相关函数表格
* 输出:srp_global表格，就是每个遍
* 历点的srp-phat值
*******************************/
//void srpphat(
//    float R[MIC_PAIR][FRAME_SIZE],
//    int8_t TDOA_table[MIC_PAIR][SEARCH_POINT],
//    float result[SEARCH_POINT])
//{
//    int i,p;
//    float srp_local=0;
//    int  time_diff=0;
//    int center = (FRAME_SIZE/2)-1;
//    for(i=0; i<SEARCH_POINT; i++){
//        srp_local = 0;
//        for(p=0; p<MIC_PAIR; p++){
//            time_diff = TDOA_table[p][i] + center;
//            srp_local = srp_local + R[p][time_diff];
//        }
//        result[i] = srp_local;
//    }
//}

//int32_t find_source_location(int8_t TDOA_table[MIC_PAIR][SEARCH_POINT], float R[MIC_PAIR][FRAME_SIZE])
//{
//    int max_loc;
//    float max_srp;
//    float srp_global[SEARCH_POINT];
//    srpphat(R, TDOA_table, srp_global);
//    for(int32_t i=0; i<SEARCH_POINT; i++){
//        if(srp_global[i] > max_srp){
//            max_srp = srp_global[i];
//            max_loc = i;
//        }
//    }
//    return max_loc;
//}

/*
 * fft.h
 *
 *  Created on: 2018��3��5��
 *      Author: Brick
 */


#ifndef FFT_H_
#define FFT_H_

typedef struct complex //��������
{
  double real;       //ʵ��
  double imag;       //�鲿
}complex;

#ifndef PI
#define PI 3.1415926535897932384626433832795028841971
#endif

///////////////////////////////////////////
//void conjugate_complex(int n, complex in[], complex out[]);
//void conjugate_complex(int n, complex in[], complex out[]);
//void c_plus(complex a,complex b,complex *c);  // ������
//void c_mul(complex a,complex b,complex *c) ;  // ������
//void c_sub(complex a,complex b,complex *c);   // ��������
//void c_div(complex a,complex b,complex *c);   // ��������
void fft(int N, complex f[]);                   // ����Ҷ�任 ���Ҳ��������f��
void ifft(int N, complex f[]);                  // ����Ҷ��任
//void c_abs(complex f[],double out[],int n);   // ��������ȡģ
void fftshift(complex data[], int count);
#endif /* FFT_H_ */

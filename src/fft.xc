#include "math.h"
#include "fft.h"
//����0.0001����
#include "common.h"

//void conjugate_complex(int n, complex dataIn[], complex dataOut[])
//{
//    int i=0;
//    for(i=0; i<n; i++){
//        dataOut[i].imag = -dataIn[i].imag;
//        dataOut[i].real = dataIn[i].real;
//    }
//}

//void c_abs(complex f[],double out[],int n)
//{
//  int i = 0;
//  double t;
//  for(i=0; i<n; i++)
//  {
//    t = f[i].real * f[i].real + f[i].imag * f[i].imag;
//    out[i] = sqrt(t);
//  }
//}


void c_plus(complex a,complex b,complex *c)
{
    c->real = a.real + b.real;
    c->imag = a.imag + b.imag;
}

void c_sub(complex a,complex b,complex *c)
{
    c->real = a.real - b.real;
    c->imag = a.imag - b.imag;
}

void c_mul(complex a,complex b,complex *c)
{
    c->real = a.real * b.real - a.imag * b.imag;
    c->imag = a.real * b.imag + a.imag * b.real;
}

//void c_div(complex a,complex b,complex *c)
//{
//  c->real = (a.real * b.real + a.imag * b.imag)/(b.real * b.real +b.imag * b.imag);
//  c->imag = (a.imag * b.real - a.real * b.imag)/(b.real * b.real +b.imag * b.imag);
//}
//
//#define SWAP(a,b)  tempr=(a);(a)=(b);(b)=tempr
//
void Wn_i(int n,int i,complex *Wn,char flag)
{
    Wn->real = cos(2*PI*i/n);
    if(flag == 1)
        Wn->imag = -sin(2*PI*i/n);
    else if(flag == 0)
        Wn->imag = -sin(2*PI*i/n);
}

// ����Ҷ�任
void fft(int N, complex f[])
{
    complex t, wn;   // �м����
    int i, j, k, m, n, l, r, M;
    int la, lb, lc;
    // ����ֽ�ļ���M=log2(N)
    for(i=N,M=1; (i=i/2)!=1; M++);
    // ���յ�λ����������ԭ�ź�
    for(i=1,j=N/2; i<=N-2; i++){
        if(i<j){
            t=f[j];
            f[j]=f[i];
            f[i]=t;
        }
        k=N/2;
        while(k<=j){
            j=j-k;
            k=k/2;
        }
        j=j+k;
    }

    /*----FFT�㷨----*/
    for(m=1; m<=M; m++){
        la = pow(2, m); // la=2^m�����m��ÿ�����������ڵ���
        lb = la/2;    // lb�����m��ÿ�������������ε�Ԫ��
        // ͬʱ��Ҳ��ʾÿ�����ε�Ԫ���½ڵ�֮��ľ���
        // ��������
        for(l=1; l<=lb; l++){
            r = (l-1) * pow(2, M-m);
            Wn_i(N, r, &wn, 1);         // wn=Wnr
            // ����ÿ�����飬��������ΪN/la
            for(n=l-1; n<N-1; n=n+la){
                // n,lc�ֱ����һ�����ε�Ԫ���ϡ��½ڵ���
                lc = n + lb;
                c_mul(f[lc], wn, &t);       // t = f[lc] * wn��������
                c_sub(f[n], t, &(f[lc]));   // f[lc] = f[n] - f[lc] * Wnr
                c_plus(f[n], t, &(f[n]));   // f[n] = f[n] + f[lc] * Wnr
            }
        }
    }
    for(i=0; i<N; i++){
        f[i].imag = -f[i].imag;
    }
}

//����Ҷ��任
void ifft(int N, complex f[])
{
    int i=0;
//    conjugate_complex(N, f, f);
    for(i=0; i<N; i++){
        f[i].imag = -f[i].imag;
        f[i].real = f[i].real;
    }
    fft(N, f);
//    conjugate_complex(N, f, f);
    for(i=0; i<N; i++){
        f[i].imag = -f[i].imag;
        f[i].real = f[i].real;
    }
    for(i=0;i<N;i++)
    {
        f[i].imag = (f[i].imag)/N;
        f[i].real = (f[i].real)/N;
    }
}

void fftshift(complex data[], int count)
{
    complex tmp;
    int k = 0;
    int c = (int) floor((float)count/2);
    // For odd and for even numbers of element use different algorithm
    if (count % 2 == 0){
        for (k = 0; k < c; k++){
            tmp = data[k];
            data[k] = data[k+c];
            data[k+c] = tmp;
        }
    }
    else{
        complex tmp = data[0];
        for (k = 0; k < c; k++){
            data[k] = data[c + k + 1];
            data[c + k + 1] = data[k + 1];
        }
        data[c] = tmp;
    }
}

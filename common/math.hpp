#ifndef MATH_H_
#define MATH_H_
#include<math.h>
#include<fftw3.h>
#define MIN(X, Y) ( ((X) < (Y)) ? (X) : (Y) )
#define MAX(X, Y) ( ((X) > (Y)) ? (X) : (Y) )
/**
 * Math helper class that 
 * contains mathematical
 * functions for analysis
 */

double phi(double z);
double phi_inv(double z);
int next_two_power(int n);
float computeMean(float* samples,int dim);
float findMax(float* samples,int dim);
//tuning alpha and beta you can generate different types of windows
float generalized_hamming_window(float alpha,float beta,int n,int N);
//tuning a0,a1,a2 and a3 you can generate different types of windows
float generalized_cosine_window(float a0,float a1,float a2,float a3,int n,int N);
float tukey_window(float alpha,int n,int N);
float* autoCorrelate(float* data,int spt);
float* crossCorrelate(float*,float*,int dim);

#endif

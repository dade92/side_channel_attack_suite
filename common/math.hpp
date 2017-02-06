#ifndef MATH_H_
#define MATH_H_
#include<math.h>

/**
 * Math helper class that 
 * contains mathematical
 * functions for analysis
 */

double phi(double z);
double phi_inv(double z);
int next_two_power(int n);
float crossCorrelate(float*,float*,int start,int end);
float computeMean(float* samples,int dim);
float findMax(float* samples,int dim);
float generalized_hamming_window(float alpha,float beta,int n,int N);
//tuning a0,a1,a2 and a3 you can generate different types of windows
float generalized_cosine_window(float a0,float a1,float a2,float a3,int n,int N);

#endif

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
float computeMean(float* samples,int dim);
float findMax(float* samples,int dim);
float hanning(int n,int N);
float blackman_nuttall(float a0,float a1,float a2,float a3,int n,int N);

#endif

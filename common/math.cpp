#include"math.hpp"

double RationalApproximation(double t)
{
    const double c[] = {2.515517, 0.802853, 0.010328};
    const double d[] = {1.432788, 0.189269, 0.001308};
    return t - ((c[2]*t + c[1])*t + c[0]) /
                (((d[2]*t + d[1])*t + d[0])*t + 1.0);
}

double phi(double z) {
    double expression=0.5*exp(-1.2*pow(z,1.3));
    return 1-expression;
}
double phi_inv(double p) {
    if (p < 0.5) {
        // F^-1(p) = - G^-1(p)
        return -RationalApproximation( sqrt(-2.0*log(p)) );
    }
    else
    {
        // F^-1(p) = G^-1(1-p)
        return RationalApproximation( sqrt(-2.0*log(1-p)) );
    }
}
int next_two_power(int n) {
    int p=1;
    while(p<n)
        p<<=1;
    return p;
}

float computeMean(float* samples,int dim) {
    float mean=0;
    for(int i=0;i<dim;i++)
        mean+=samples[i];
    return mean/dim;
}

float findMax(float* samples,int dim) {
    float max=samples[0];
    for(int i=1;i<dim;i++) {
        if(samples[i]>max)
            max=samples[i];
    }
}

float hanning(int n,int N) {
    return 0.5*(1-cos((2*M_PI*n)/(N-1)));
}

float blackman_nuttall(float a0,float a1,float a2,float a3,int n,int N) {
    return a0
        -a1*cos(2*M_PI*n/(N-1))
        +a2*cos(4*M_PI*n/(N-1))
        -a3*cos(6*M_PI*n/(N-1));
}
#include"math.hpp"

double RationalApproximation(double t) {
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

float generalized_hamming_window(float alpha,float beta,int n,int N) {
    return alpha-beta*cos((2*M_PI*n)/(N-1));
}

float generalized_cosine_window(float a0,float a1,float a2,float a3,int n,int N) {
    return a0
        -a1*cos(2*M_PI*n/(N-1))
        +a2*cos(4*M_PI*n/(N-1))
        -a3*cos(6*M_PI*n/(N-1));
}

/**
 * given a long trace (pointed by data[0]),
 * returns the auto correlation array, where
 * the tau range is from -samplesPerTrace (0 index)
 * to +samplesPerTrace (2*samplesPerTrace index)
 */
float* autoCorrelate(float* data,int samplesPerTrace) {
    float* correlation=new float[2*samplesPerTrace-1];
    float* corr_shifted=new float[2*samplesPerTrace-1];
    fftwf_complex * outa = (fftwf_complex *) fftwf_malloc(sizeof(fftwf_complex) * (2 * samplesPerTrace - 1));
    fftwf_complex * out = (fftwf_complex *) fftwf_malloc(sizeof(fftwf_complex) * (2 * samplesPerTrace - 1));
    fftwf_complex * sig = (fftwf_complex *) fftwf_malloc(sizeof(fftwf_complex) * (2 * samplesPerTrace - 1));
    fftwf_complex * out_shifted = (fftwf_complex *) fftwf_malloc(sizeof(fftwf_complex) * (2 * samplesPerTrace - 1));
    for(int w=0;w<2*samplesPerTrace-1;w++) {
        sig[w][0]=data[w];
        sig[w][1]=0;
    }
    fftwf_plan pa = fftwf_plan_dft_1d(2 * samplesPerTrace - 1,sig, outa,FFTW_FORWARD,FFTW_ESTIMATE);
    fftwf_plan px =  fftwf_plan_dft_1d(2 * samplesPerTrace - 1,out,out_shifted,FFTW_BACKWARD,FFTW_ESTIMATE);
    fftwf_execute(pa);
    for (int i = 0; i< 2 * samplesPerTrace - 1; i++) {
        out[i][0] = outa[i][0] * outa[i][0] - outa[i][1]*(-outa[i][1]);
        out[i][1] = outa[i][0] * (-outa[i][1]) + outa[i][1]*outa[i][0];
    }
    fftwf_execute(px);
    for (int i = 0; i<2 * samplesPerTrace - 1; i++)
        correlation[i] = out_shifted[(i + samplesPerTrace) 
        % (2 * samplesPerTrace - 1)][0] / (2 * samplesPerTrace - 1);
    return correlation;
}
/**
 * cross correlate two signals (floating points arrays)
 * returning the correlation array, in range 
 * [-samplesPerTrace,+samplesPerTrace]
 */
float* crossCorrelate(float* data0,float* data1,int samplesPerTrace) {
    float* correlation=new float[2*samplesPerTrace-1];
    float* corr_shifted=new float[2*samplesPerTrace-1];
    fftwf_complex * outa = (fftwf_complex *) fftwf_malloc(sizeof(fftwf_complex) * (2 * samplesPerTrace - 1));
    fftwf_complex * outb = (fftwf_complex *) fftwf_malloc(sizeof(fftwf_complex) * (2 * samplesPerTrace - 1));
    fftwf_complex * out = (fftwf_complex *) fftwf_malloc(sizeof(fftwf_complex) * (2 * samplesPerTrace - 1));
    fftwf_complex * siga = (fftwf_complex *) fftwf_malloc(sizeof(fftwf_complex) * (2 * samplesPerTrace - 1));
    fftwf_complex * sigb = (fftwf_complex *) fftwf_malloc(sizeof(fftwf_complex) * (2 * samplesPerTrace - 1));
    fftwf_complex * out_shifted = (fftwf_complex *) fftwf_malloc(sizeof(fftwf_complex) * (2 * samplesPerTrace - 1));
    for(int w=0;w<2*samplesPerTrace-1;w++) {
        siga[w][0]=data0[w];
        siga[w][1]=0;
        sigb[w][0]=data1[w];
        sigb[w][1]=0;
    }
    fftwf_plan pa = fftwf_plan_dft_1d(2 * samplesPerTrace - 1,siga, outa,FFTW_FORWARD,FFTW_ESTIMATE);
    fftwf_plan pb = fftwf_plan_dft_1d(2 * samplesPerTrace - 1,sigb, outb,FFTW_FORWARD,FFTW_ESTIMATE);
    fftwf_plan px =  fftwf_plan_dft_1d(2 * samplesPerTrace - 1,out,out_shifted,FFTW_BACKWARD,FFTW_ESTIMATE);
    fftwf_execute(pa);
    fftwf_execute(pb);
    for (int i = 0; i< 2 * samplesPerTrace - 1; i++) {
        out[i][0] = outa[i][0] * outb[i][0] - outa[i][1]*(-outb[i][1]);
        out[i][1] = outa[i][0] * (-outb[i][1]) + outa[i][1]*outb[i][0];
    }
    fftwf_execute(px);
    for (int i = 0; i<2 * samplesPerTrace - 1; i++)
        correlation[i] = out_shifted[(i + samplesPerTrace) 
        % (2 * samplesPerTrace - 1)][0] / (2 * samplesPerTrace - 1);
    return correlation;
}
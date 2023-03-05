#ifndef ENCRYPT_H_INCLUDED
#define ENCRYPT_H_INCLUDED
class image{
public :
    int N,M;
    int**p;
    image(cv::Mat img);
    image(int n,int m);
};
double* get_logistic_sequence(int sz,double k0,double paramater);
int** logistic_quantize(int N,int M,double* p);
int** logistic_quantize2(int N,int M,double* p);
int** msc_encrypt(cv::Mat mat_tmp);
void dnacode(int **Q1,int **Q2,image& a);
void dna_encode(int* arr,int key);
void dna_decode(int* arr,int key);
extern int subblock_width;
#endif // ENCRYPT_H_INCLUDED

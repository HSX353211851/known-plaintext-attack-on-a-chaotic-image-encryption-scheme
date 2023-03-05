#include<iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include<algorithm>
#include<map>
#include"encrypt.h"
int subblock_width=1;

image::image(cv::Mat img){

    this->N = img.rows;
    this->M = img.cols;

    this->p = (int**)malloc(this->N*sizeof(int*));
    for(int i = 0;i < this->N;i++){
        this->p[i] = (int*)malloc(this->M*sizeof(int));
        for(int j = 0;j < this->M;j++){
            this->p[i][j] = (int)img.at<uchar>(i,j);
            //std::cout << p[i][j] << " ";
        }
        //std::cout <<"\n";
    }
}


image::image(int n,int m){

    this->N = n;
    this->M = m;

    this->p = (int**)malloc(this->N*sizeof(int*));
    for(int i = 0;i < this->N;i++){
        this->p[i] = (int*)malloc(this->M*sizeof(int));
        for(int j = 0;j < this->M;j++){
            this->p[i][j] =0;
            //std::cout << p[i][j] << " ";
        }
        //std::cout <<"\n";
    }
}
int** msc_encrypt(cv::Mat mat_tmp){
   /* for(int i = 0;i < a.N;i++){
        for(int j = 0;j < a.M;j++){
            std::cout << a.p[i][j] <<" ";
        }std::cout << "\n";
    }*/
    //imshow("1xx2",mat_tmp);
    cv::waitKey(0);
    image a(mat_tmp);
    double* p = get_logistic_sequence(a.M * a.N,0.5,3.9999);
    int**Q  = logistic_quantize(a.N,a.M,p);
    free(p);



    /*for(int i = 0;i < a.N;i++){
        for(int j = 0;j < a.M;j++){
            std::cout << Q[i][j] << " ";
        }
        std::cout << "\n";
    }*/
    for(int i = 0;i < a.N;i++){
        for(int j = 0;j < a.M;j++){
            a.p[i][j] ^= Q[i][j];       // XOR the plaintext_image with chaotic_sequence
            //std::cout << a.p[i][j] <<" ";
        }//std::cout << "\n";
    }
    free(Q);




    int num_blocks = a.M * a.N/subblock_width/subblock_width;
    p = get_logistic_sequence(num_blocks,0.5,3.8999);
    int**Q1 = logistic_quantize2(a.N/subblock_width,a.M/subblock_width,p);
    free(p);

    p = get_logistic_sequence(num_blocks,0.232,3.8999);
    int**Q2 = logistic_quantize2(a.N/subblock_width,a.M/subblock_width,p);
    free(p);

    dnacode(Q1,Q2,a);
    free(Q1);
    free(Q2);


    int sump = a.N * a.M /subblock_width / subblock_width;

    p = get_logistic_sequence(sump,0.3,3.8999);
    int* X3 = (int*)malloc(sump*sizeof(int));
    for(int i = 0;i < sump;i++){
        X3[i] = p[i]*1e7;
        //std::cout << X3[i] << " ";
    }//std::cout << "\n";
    free(p);

    p = get_logistic_sequence(sump,0.6,3.8999);
    int* X4 = (int*)malloc(sump*sizeof(int));
    for(int i = 0;i < sump;i++){
        X4[i] = p[i]*1e7;
        //std::cout << X4[i] << " ";
    }//std::cout << "\n";
    free(p);
    int* sort_X3 = (int*)malloc(sump*sizeof(int));
    int* sort_X4 = (int*)malloc(sump*sizeof(int));
    for(int i = 0;i <sump;i++)sort_X3[i] = X3[i];
    for(int i = 0;i <sump;i++)sort_X4[i] = X4[i];

    std::sort(sort_X3,sort_X3+sump);
    std::sort(sort_X4,sort_X4+sump);
    std::reverse(sort_X3,sort_X3+sump);
    std::reverse(sort_X4,sort_X4+sump);

    std::map<int,int> mp1;
    std::map<int,int> mp2;
    for(int i = 0;i < sump;i++){
        mp1[sort_X3[i]] = i;
        mp2[sort_X4[i]] = i;
    }

    int *P1 = (int*)malloc(sump*sizeof(int));
    int *P2 = (int*)malloc(sump*sizeof(int));

    for(int i = 0;i <sump;i++){
        P1[i] = mp1[X3[i]] ;
        P2[i] = mp2[X4[i]] ;
    }

    free(sort_X3);
    free(sort_X4);
    free(X3);
    free(X4);



    int** encrypted_image = (int**)malloc(a.N *sizeof(int*));
    for(int i = 0;i < a.N;i++){
        encrypted_image[i] = (int*)malloc(a.M*sizeof(int));
    }

    for(int i = 0;i < a.N;i++){
        for(int j = 0;j < a.M;j++){
            encrypted_image[i][j] = a.p[i][j];
        }
    }
    for(int i = 0;i < sump;i++){
        for(int k1 = 0;k1 < subblock_width;k1++){
            for(int k2 = 0;k2 < subblock_width;k2++){
                int from = P1[i];
                int to = P2[i];

                int fromr = from / (a.M / subblock_width);
                int fromc = from % (a.M / subblock_width);
                int tor = to / (a.M / subblock_width);
                int toc = to % (a.M / subblock_width);

                std::swap(encrypted_image[fromr*subblock_width+k1][fromc*subblock_width+k2],encrypted_image[tor*subblock_width+k1][toc*subblock_width+k2]);
            }
        }
    }
    /*
    for(int i = 0;i < a.N/subblock_width;i++){
        std::cout << P1[i] << " ";
    }std::cout << "\n";
    for(int i = 0;i < a.M/subblock_width;i++){
        std::cout << P2[i] << " ";
    }std::cout << "\n";
*/

    free(P1);
    free(P2);

    return encrypted_image;

}

double*get_logistic_sequence(int sz,double k0,double paramater){
    double* K = (double*)malloc((sz+1)*sizeof(double));
    K[0] = k0;
    for(int i=1;i<=sz;i++){K[i]=paramater*K[i-1]*(1-K[i-1]);}
    return K;
}
int** logistic_quantize(int N,int M,double* p){
    int sz = N*M;
    int* K = (int*)malloc((sz+1)*sizeof(int));
    for(int i = 1;i <= sz;i++ ){
        int now = (p[i]*10000+0.5);
        now  %= 256;
        K[i] = now;
    }
    int** Q = (int**)malloc(sizeof(int*)*N);
    for(int i = 0;i < N;i++){
        Q[i] = (int*)malloc(sizeof(int)*M);
    }
    for(int i = 0;i < N;i++){
        for(int j = 0;j < M;j++){
            Q[i][j] = K[i*M+j+1];
        }
    }
    free(K);

    return Q;
}

int** logistic_quantize2(int N,int M,double* p){
    int sz = N*M;
    int* K = (int*)malloc((sz+1)*sizeof(int));
    for(int i = 1;i <= sz;i++ ){
        int now = (p[i]*10000+0.5);
        now  %= 8;
        K[i] = now+1;
    }
    int** Q = (int**)malloc(sizeof(int*)*N);
    for(int i = 0;i < N;i++){
        Q[i] = (int*)malloc(sizeof(int)*M);
    }
    for(int i = 0;i < N;i++){
        for(int j = 0;j < M;j++){
            Q[i][j] = K[i*M+j+1];
        }
    }
    free(K);

    return Q;
}


void dnacode(int **Q1,int **Q2,image& a){
    for(int i = 0;i < a.N;i++){
        for(int j = 0;j < a.M;j++){
                int newnum = 0;
                for(int k = 0;k < 4;k++){
                    int tmp = 3;
                    int num = (a.p[i][j] & (tmp<<(2*k)))>>(2*k) ;

                    int arr1[4];
                    int arr2[4];
                    dna_encode(arr1,Q1[i/subblock_width][j/subblock_width]);
                    dna_decode(arr2,Q2[i/subblock_width][j/subblock_width]);

                    int now = arr2[arr1[num]];
                    newnum += ((now)<<(2*k));

                }
                a.p[i][j] = newnum;
        }
    }
}


void dna_encode(int* arr,int key){
        if(key == 1){
            arr[0]=0;arr[1]=3;arr[2]=2;arr[3]=1;
        }
        if(key == 2){
            arr[0]=0;arr[1]=2;arr[2]=3;arr[3]=1;
        }
        if(key == 3){
            arr[0]=1;arr[1]=3;arr[2]=2;arr[3]=0;
        }
        if(key == 4){
            arr[0]=1;arr[1]=2;arr[2]=3;arr[3]=0;
        }
        if(key == 5){
            arr[0]=2;arr[1]=0;arr[2]=1;arr[3]=3;
        }
        if(key == 6){
            arr[0]=3;arr[1]=0;arr[2]=1;arr[3]=2;
        }
        if(key == 7){
            arr[0]=2;arr[1]=1;arr[2]=0;arr[3]=3;
        }
        if(key == 8){
            arr[0]=3;arr[1]=1;arr[2]=0;arr[3]=2;
        }

}



void dna_decode(int* arr,int key){
        if(key == 1){
            arr[0]=0;arr[1]=3;arr[2]=2;arr[3]=1;
        }
        if(key == 2){
            arr[0]=0;arr[1]=3;arr[2]=1;arr[3]=2;
        }
        if(key == 3){
            arr[0]=3;arr[1]=0;arr[2]=2;arr[3]=1;
        }
        if(key == 4){
            arr[0]=0;arr[1]=3;arr[2]=1;arr[3]=2;
        }
        if(key == 5){
            arr[0]=1;arr[1]=2;arr[2]=0;arr[3]=3;
        }
        if(key == 6){
            arr[0]=1;arr[1]=2;arr[2]=3;arr[3]=0;
        }
        if(key == 7){
            arr[0]=2;arr[1]=1;arr[2]=0;arr[3]=3;
        }
        if(key == 8){
            arr[0]=2;arr[1]=1;arr[2]=3;arr[3]=0;
        }

}



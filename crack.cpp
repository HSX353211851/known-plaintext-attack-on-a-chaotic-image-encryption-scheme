#include<iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "encrypt.h"
#include "crack.h"

int getnum(int num){
    int sum = 0;
    for(int i = 0;i < 4;i++){
        if((num&3) == 3)  sum += 2;
        else if((num&3) == 0) sum++;

        if(i<3) sum *= 3;
        num >>= 2;
    }
    return sum;
}


bool check_ok(int from,int to,vector<Mat> V,vector<Mat> encrypted_V){
    int n = V[0].rows;
    int m = V[0].cols;

    for(int index1 = 0;index1 < V.size()-1;index1++){
        for(int index2 = index1 + 1;index2 < V.size();index2++){

            int num1 = getnum(V[index1].at<uchar>(from/m,from%m) ^ V[index2].at<uchar>(from/m,from%m)) ;
            int num2 = getnum(encrypted_V[index1].at<uchar>(to/m,to%m) ^ encrypted_V[index2].at<uchar>(to/m,to%m)) ;


            if(num1 != num2)    return false;

        }
    }
    return true;


}



Mat image_crack(vector<Mat> V,Mat Ciperimg){
    printf("Cracking....\n");
    int sum = V[0].rows * V[0].cols;

    vector<Mat> encrypted_V;

    int n = V[0].rows;
    int m = V[0].cols;

    for(int i = 0;i < V.size();i++){
        Mat encrypted_img(n,m,CV_8U);
        int**tmp = msc_encrypt(V[i]);
        for(int i = 0;i < n;i++){
            for(int j = 0;j < m;j++){
                encrypted_img.at<uchar>(i,j) = tmp[i][j];
            }
        }
        free(tmp);
        encrypted_V.push_back(encrypted_img);
    }

    vector<int> vis(sum);
    vector<int> P(sum);
    for(int now = 0;now < sum;now++){

        for(int to = 0;to < sum;to++){
            if(vis[to]) continue;

            if(check_ok(now,to,V,encrypted_V)){
                P[now] = to;
                vis[to] = 1;
                break;
            }
        }

    }


// we now obtained the scrambling permutation
// reverse the scrambling

    vector<int> tmp(sum);
    for(int i = 0;i < sum;i++){
        int to = P[i];
        //printf("%d\n",P[i]);
        tmp[i] = Ciperimg.at<uchar>(to/m,to%m);
    }

    for(int i = 0;i < sum;i++){
        Ciperimg.at<uchar>(i/m,i%m) = tmp[i];
    }

    for(int index = 0;index <encrypted_V.size();index++){
        for(int i = 0;i < sum;i++){
            int to = P[i];
            //printf("%d\n",P[i]);
            tmp[i] = encrypted_V[index].at<uchar>(to/m,to%m);
        }

        for(int i = 0;i < sum;i++){
            encrypted_V[index].at<uchar>(i/m,i%m) = tmp[i];
        }

    }


    // calculate the mapping from Cipher to original

    int*** mp = (int***)malloc(sizeof(int**) * 4);
    for(int i = 0;i < 4;i++){
        mp[i] = (int**)malloc(sizeof(int*) * 4);
        for(int j = 0;j < 4;j++){
            mp[i][j] = (int*)malloc(sizeof(int) * sum);
        }
    }

    for(int k = 0;k < 4;k++){
        for(int j = 0;j < 4;j++){
            for(int i = 0;i < sum;i++){
                mp[j][k][i] = 0;
            }
        }
    }

    for(int index = 0;index < V.size();index++){
        for(int i = 0;i < sum;i++){
            int from = encrypted_V[index].at<uchar>(i/m,i%m);
            int to = V[index].at<uchar>(i/m,i%m);

            for(int j = 0;j < 4;j++){
                mp[(from&3)][j][i] = (to&3);
                mp[ (3&(~(from&3))) ][j][i] = (3&(~(to&3))) ;

                from >>= 2;
                to >>= 2;
            }
        }
    }


    Mat result(n,m,CV_8U);;

    for(int i = 0;i < sum;i++){
        int num = Ciperimg.at<uchar>(i/m,i%m);
        int cnt = 0;
        for(int j = 0;j < 4;j++){
            int to = mp[(num&3)][j][i];
            cnt+= (to*(1<<(2*j))) ;

            num >>= 2;
        }
        result.at<uchar>(i/m,i%m) = cnt;
    }



    for(int i = 0;i < 4;i++){
        for(int j = 0;j < 4;j++){
            free(mp[i][j]);
        }
    }
    for(int i = 0;i < 4;i++){
        free(mp[i]);
    }
    free(mp);

    return result;

}

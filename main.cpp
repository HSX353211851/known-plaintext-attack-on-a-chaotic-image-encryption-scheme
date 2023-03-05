#include<iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "encrypt.h"
#include "crack.h"
using namespace std;
using namespace cv;

int main(){
    /*Mat inputimage = imread("eye.png",0);
    //Mat inputimage = imread("eye.jpg",0);
    //cout << "图像宽为：" << inputimage.cols << "\t高度为：" << inputimage.rows << "\t通道数为：" << inputimage.channels() << endl;
    imshow("original",inputimage);
    //waitKey(0);
    printf("processing.....\n");

    image x = image(inputimage);

    int**encrypted_p = msc_encrypt(x);



    Mat encrypted_image = inputimage;
    for(int i = 0;i < inputimage.rows;i++){
        for(int j = 0;j < inputimage.cols;j++){
            encrypted_image.at<uchar>(i,j) = encrypted_p[i][j];
        }
    }
    imshow("encrypted",encrypted_image);

*/
    //input known_plaintext
    vector<Mat> v;
    Mat known_img = imread("cameraman_128.png" , 0);
    v.push_back(known_img);
    //known_img = imread("firefox.png" , 0);
    //v.push_back(known_img);
    known_img = imread("Baboon128.png" , 0);
    v.push_back(known_img);
    known_img = imread("lake_128.png" , 0);
    v.push_back(known_img);
    known_img = imread("lena_128.png" , 0);
    v.push_back(known_img);

    known_img = imread("house_128.png" , 0);

    int n = known_img.rows;
    int m = known_img.cols;

    Mat C_img(n,m,CV_8U);
    int** tmp = msc_encrypt(known_img);
    for(int i = 0;i < n;i++){
        for(int j = 0;j < m;j++){
            C_img.at<uchar>(i,j) = tmp[i][j];
        }
    }


    Mat result = image_crack(v,C_img);
    imshow("img",known_img);
    imshow("result",result);


    printf("done.\n");

    waitKey(0);


	return 0;
}

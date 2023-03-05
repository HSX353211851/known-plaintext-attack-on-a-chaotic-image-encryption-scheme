#ifndef CRACK_H_INCLUDED
#define CRACK_H_INCLUDED
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
using namespace cv;
using namespace std;

Mat image_crack(vector<Mat> V,Mat Ciperimg);

#endif // CRACK_H_INCLUDED

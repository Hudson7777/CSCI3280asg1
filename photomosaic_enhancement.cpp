#include "stdio.h"
#include <iostream>
#include <vector>
#include <string>
#include "malloc.h"
#include "memory.h"
#include "math.h"
#include "bmp.h"		
#include "list_files.h"
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

#define SAFE_FREE(p)  { if(p){ free(p);  (p)=NULL;} }

//I want to plot the gray value of each pixel in the source image to a histogram.
void plot_histogram(Bitmap* source_image, int output_w, int output_h) {
    int histSize = 256;
    float range[] = { 0, 256 };
    const float* histRange = { range };
    bool uniform = true;
    bool accumulate = false;
    // Do I need to implement some library to plot the histogram?
    
    Mat hist;
    calcHist(&source_image, 1, 0, Mat(), hist, 1, &histSize, &histRange, uniform, accumulate);
    int hist_w = 512;
    int hist_h = 400;
    int bin_w = cvRound((double)hist_w / histSize);
    Mat histImage(hist_h, hist_w, CV_8UC3, Scalar(0, 0, 0));
    normalize(hist, hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
    for (int i = 1; i < histSize; i++)
    {
        line(histImage, Point(bin_w * (i - 1), hist_h - cvRound(hist.at<float>(i - 1))),
            Point(bin_w * (i), hist_h - cvRound(hist.at<float>(i))),
            Scalar(255, 0, 0), 2, 8, 0);
    }
    imshow("Histogram", histImage);
    waitKey(0);
}


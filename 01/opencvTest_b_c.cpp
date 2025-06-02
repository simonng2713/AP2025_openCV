#include <iostream>
#include <opencv2/opencv.hpp>


int main()
{

    cv::Mat src = cv::imread("lena.jpg");
    cv::Mat dst;
  
    cv::namedWindow("Display", cv::WINDOW_AUTOSIZE);
    
    double alpha = 0.1; // contrast 
    double beta = 1.0; // brightness
     
    src.convertTo(dst, -1, alpha, beta);  
    cv::imshow("Display", dst);
    
    cv::waitKey(0);
    return 0;
}



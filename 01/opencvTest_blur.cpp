#include <iostream>
#include <opencv2/opencv.hpp>

int main() {
  cv::Mat src = cv::imread("lena.jpg");
  cv::Mat dst;

  cv::namedWindow("Display", cv::WINDOW_AUTOSIZE);
  
  int blur = 100;  // 変化の度合い 1-10

  cv::medianBlur(src, dst, blur * 2 + 1);
  cv::imshow("Display", dst);

  cv::waitKey(0);

  return 0;
}

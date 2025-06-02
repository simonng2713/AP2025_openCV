// opencvTest_first.cpp
#include <iostream>
#include <opencv2/opencv.hpp>

int main() {
  cv::Mat image = cv::imread("lena.jpg");

  cv::namedWindow("Display", cv::WINDOW_AUTOSIZE);
  cv::imshow("Display", image);

  cv::waitKey(0);

  return 0;
}

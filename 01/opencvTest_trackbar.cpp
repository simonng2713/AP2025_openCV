#include <iostream>
#include <opencv2/opencv.hpp>

int main() {
  cv::Mat src, dst;
  int blur = 5;

  src = cv::imread("lena.jpg");

  cv::namedWindow("Display", cv::WINDOW_AUTOSIZE);
  cv::createTrackbar("blur", "Display", &blur, 10);

  while (cv::waitKey(100) < 0) {
    cv::medianBlur(src, dst, blur * 2 + 1);
    cv::imshow("Display", dst);
  }

  return 0;
}

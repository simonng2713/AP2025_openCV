#include <iostream>
#include <opencv2/opencv.hpp>

cv::Mat src;
cv::Mat dst;

void on_tracker(int blur, void *) {
  cv::medianBlur(src, dst, blur * 2 + 1);
  cv::imshow("Display", dst);
}

int main() {
  src = cv::imread("lena.jpg");

  cv::namedWindow("Display", cv::WINDOW_AUTOSIZE);
  cv::createTrackbar("blur", "Display", nullptr, 10, on_tracker);
  cv::setTrackbarPos("blur", "Display", 5);

  on_tracker(5,0);
  while (cv::waitKey(100) < 0) {
  }

  return 0;
}

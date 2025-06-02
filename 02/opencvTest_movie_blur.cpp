#include <stdio.h>

#include <opencv2/opencv.hpp>

using namespace cv;

int b = 2;

void on_tracker(int b_, void *) { b = b_; }

int main() {
  VideoCapture cap("vtest.avi");

  if (!cap.isOpened()) {
    return -1;
  }

  Mat src, dst;
  namedWindow("movie", WINDOW_AUTOSIZE);
  createTrackbar("blur", "movie", nullptr, 10, on_tracker);
  setTrackbarPos("blur", "movie", b);

  while (cap.read(src)) {
    medianBlur(src, dst, b * 2 + 1);
    imshow("movie", dst);
    waitKey(30);
  }
  return 0;
}

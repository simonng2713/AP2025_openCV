#include <stdio.h>

#include <opencv2/opencv.hpp>

using namespace cv;

int main() {
  VideoCapture cap("vtest.avi");

  if (!cap.isOpened()) {
    return -1;
  }

  Mat src, gray;
  namedWindow("movie", WINDOW_AUTOSIZE);

  while (cap.read(src)) {
    cvtColor(src, gray, COLOR_BGR2GRAY);
    imshow("movie", gray);
    waitKey(30);
  }

  return 0;
}

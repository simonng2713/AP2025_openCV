#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;

int main() {
  VideoCapture cap("vtest.avi");

  if (!cap.isOpened()) {
    return -1;
  }

  Mat src;
  namedWindow("movie", WINDOW_AUTOSIZE);

  bool loopflag = true;

  while (loopflag) {
    if (cap.read(src)) {
      imshow("movie", src);
    } else {
      loopflag = false;
    }

    int c = waitKey(10);
    switch (c) {
      case 'e':
        loopflag = false;
      default:
        break;
    }
  }

  return 0;
}

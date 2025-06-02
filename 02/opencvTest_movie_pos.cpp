#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;

VideoCapture cap;

void on_tracker(int p_, void *) { cap.set(CAP_PROP_POS_FRAMES, p_); }

int main() {
  cap.open("vtest.avi");

  if (!cap.isOpened()) {
    return -1;
  }

  Mat src;
  namedWindow("movie", WINDOW_AUTOSIZE);
  createTrackbar("pos", "movie", nullptr, (int)cap.get(CAP_PROP_FRAME_COUNT),
                 on_tracker);
  setTrackbarPos("pos", "movie", 0);

  while (cap.read(src)) {
    imshow("movie", src);
    waitKey(30);
  }
  return 0;
}

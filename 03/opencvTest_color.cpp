#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;

VideoCapture cap;

void on_tracker(int p, void *) { cap.set(CAP_PROP_POS_FRAMES, p); }

double B_MAX = 50;
double B_MIN = 0;
double G_MAX = 50;
double G_MIN = 0;
double R_MAX = 250;
double R_MIN = 100;

int main() {
  cap.open("vtest.avi");

  if (!cap.isOpened()) {
    return -1;
  }

  Mat src;
  namedWindow("movie", WINDOW_AUTOSIZE);
  setWindowProperty("movie", WND_PROP_TOPMOST, 1);
  createTrackbar("pos", "movie", nullptr, (int)cap.get(CAP_PROP_FRAME_COUNT),
                 on_tracker);
  setTrackbarPos("pos", "movie", 0);

  bool playing = true;
  bool loopflag = true;

  while (loopflag) {
    if (playing && cap.read(src)) {
      Mat dst, mask;

      Scalar s_min = Scalar(B_MIN, G_MIN, R_MIN);
      Scalar s_max = Scalar(B_MAX, G_MAX, R_MAX);
      inRange(src, s_min, s_max, mask);
      src.copyTo(dst, mask);

      imshow("movie", dst);
    }

    char c = waitKey(30);
    switch (c) {
      case ' ':
        playing = !playing;
        break;
      case 'e':
        loopflag = false;
        break;
      case 's':
        cap.set(CAP_PROP_POS_FRAMES, 0);
      default:
        break;
    }
  }

  return 0;
}

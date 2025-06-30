#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;

VideoCapture cap;

void on_tracker(int p, void *) { cap.set(CAP_PROP_POS_FRAMES, p); }

int H_MAX = 255;
int H_MIN = 0;
int S_MAX = 255;
int S_MIN = 0;
int V_MAX = 255;
int V_MIN = 0;

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
    createTrackbar("H_MIN", "movie", &H_MIN, 255);
    createTrackbar("H_MAX", "movie", &H_MAX, 255);
    createTrackbar("S_MIN", "movie", &S_MIN, 255);
    createTrackbar("S_MAX", "movie", &S_MAX, 255);
    createTrackbar("V_MIN", "movie", &V_MIN, 255);
    createTrackbar("V_MAX", "movie", &V_MAX, 255);

  bool playing = true;
  bool loopflag = true;

  while (loopflag) {
    if (playing && cap.read(src)) {
      Mat dst, mask;
      cvtColor(src, dst, COLOR_BGR2HSV);
      Scalar s_min = Scalar(H_MIN, S_MIN, V_MIN);
      Scalar s_max = Scalar(H_MAX, S_MAX, V_MAX);
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

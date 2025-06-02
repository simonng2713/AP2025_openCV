#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;

VideoCapture cap;

void on_tracker(int p, void *) { cap.set(CAP_PROP_POS_FRAMES, p); }

int main() {
  cap.open("vtest.avi");

  if (!cap.isOpened()) {
    return -1;
  }

  Mat src, mask, dst, gray, back;
  namedWindow("movie", WINDOW_AUTOSIZE);
  setWindowProperty("movie", WND_PROP_TOPMOST, 1);
  createTrackbar("pos", "movie", nullptr, (int)cap.get(CAP_PROP_FRAME_COUNT),
                 on_tracker);
  setTrackbarPos("pos", "movie", 0);

  bool playing = true;
  bool loopflag = true;
  double thresh = 50;

  cap.read(src);
  cvtColor(src, back, COLOR_RGB2GRAY);

  while (loopflag) {
    if (playing && cap.read(src)) {
      cvtColor(src, gray, COLOR_RGB2GRAY);
      absdiff(gray, back, mask);
      threshold(mask, dst, thresh, 255, THRESH_BINARY);

      std::vector<std::vector<Point> > contours;
      std::vector<Vec4i> hierarchy;

      findContours(dst, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_NONE);

      for (int i = 0; i < contours.size(); i++) {
        if (contourArea(contours[i]) > 50) {
          drawContours(src, contours, (int)i, Scalar(0, 0, 255), 2);
        }
      }
      imshow("movie", src);
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

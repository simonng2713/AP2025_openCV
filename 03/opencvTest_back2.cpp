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

  Mat src, dst, back, segm;
  namedWindow("movie", WINDOW_AUTOSIZE);
  setWindowProperty("movie", WND_PROP_TOPMOST, 1);
  createTrackbar("pos", "movie", nullptr, (int)cap.get(CAP_PROP_FRAME_COUNT),
                 on_tracker);
  setTrackbarPos("pos", "movie", 0);

  bool playing = true;
  bool loopflag = true;

  int history = 100;
  float varThreshold = 20;
  bool bShadowDetection = false;
  cv::Ptr<cv::BackgroundSubtractor> bgfs =
      createBackgroundSubtractorMOG2(history, varThreshold, bShadowDetection);

  while (loopflag) {
    if (playing && cap.read(src)) {
      bgfs->apply(src, back);
      src.convertTo(segm, CV_8U, 0.5);
      add(src, Scalar(100, 100, 0), segm, back);
      imshow("movie", segm);
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
